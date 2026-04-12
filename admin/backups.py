"""Backups blueprint: create, list, download, restore, delete, and import tar.gz archives."""

import os
import re
import tarfile
import glob
import shutil
import tempfile
from datetime import datetime

from flask import (
    Blueprint, render_template, request, redirect,
    url_for, flash, current_app, send_file, abort,
)
from werkzeug.utils import secure_filename

from auth import login_required
from retention import classify_backups

backups_bp = Blueprint("backups", __name__, url_prefix="/backups")

# Filename safety: only alphanumeric, hyphens, underscores
_SAFE_LABEL_RE = re.compile(r"^[a-zA-Z0-9_-]{0,50}$")
_SAFE_FILENAME_RE = re.compile(r"^[a-zA-Z0-9_.-]+$")

# Components included in every full backup
_BACKUP_COMPONENTS = {
    "mudlib": "MUDLIB_PATH",
    "saves": "SAVE_PATH",
    "logs": "LOG_PATH",
}


def _backup_dir() -> str:
    path = current_app.config["BACKUP_PATH"]
    os.makedirs(path, exist_ok=True)
    return path


def _validate_filename(filename: str) -> bool:
    """Validate that a filename is safe (no path traversal)."""
    base = os.path.basename(filename)
    if base != filename:
        return False
    if not _SAFE_FILENAME_RE.match(base):
        return False
    if not base.endswith(".tar.gz"):
        return False
    return True


def _format_size(size_bytes: int) -> str:
    for unit in ("B", "KB", "MB", "GB"):
        if size_bytes < 1024:
            return f"{size_bytes:.1f} {unit}"
        size_bytes /= 1024
    return f"{size_bytes:.1f} TB"


def _list_backups() -> list[dict]:
    """List all backups sorted by date (newest first)."""
    backup_dir = _backup_dir()
    files = sorted(glob.glob(os.path.join(backup_dir, "*.tar.gz")), reverse=True)
    backups = []
    for f in files:
        name = os.path.basename(f)
        stat = os.stat(f)
        backups.append({
            "name": name,
            "time": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M:%S"),
            "size": _format_size(stat.st_size),
            "size_bytes": stat.st_size,
        })
    return backups


@backups_bp.route("/")
@login_required
def backups_list():
    backups = _list_backups()
    tiers = classify_backups(current_app.config)
    for b in backups:
        b["retention"] = tiers.get(b["name"], "")
    return render_template("backups.html", backups=backups)


@backups_bp.route("/create", methods=["POST"])
@login_required
def create_backup():
    label = request.form.get("label", "").strip()
    label = re.sub(r"[^a-zA-Z0-9_-]", "", label)[:50]

    cfg = current_app.config
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    parts = ["full", timestamp]
    if label:
        parts.append(label)
    filename = "_".join(parts) + ".tar.gz"
    dest = os.path.join(_backup_dir(), filename)

    try:
        with tarfile.open(dest, "w:gz") as tar:
            for component, config_key in _BACKUP_COMPONENTS.items():
                source_path = cfg[config_key]
                if os.path.isdir(source_path):
                    tar.add(source_path, arcname=component)
        flash(f"Backup created: {filename}", "success")
    except Exception as e:
        flash(f"Backup failed: {e}", "error")
        if os.path.exists(dest):
            os.remove(dest)

    return redirect(url_for("backups.backups_list"))


@backups_bp.route("/download/<filename>")
@login_required
def download_backup(filename):
    if not _validate_filename(filename):
        abort(400)
    filepath = os.path.join(_backup_dir(), filename)
    real_backup_dir = os.path.realpath(_backup_dir())
    real_filepath = os.path.realpath(filepath)
    if not real_filepath.startswith(real_backup_dir + os.sep):
        abort(403)
    if not os.path.isfile(real_filepath):
        abort(404)
    return send_file(real_filepath, as_attachment=True)


@backups_bp.route("/restore", methods=["POST"])
@login_required
def restore_backup():
    filename = request.form.get("filename", "")
    if not _validate_filename(filename):
        flash("Invalid filename.", "error")
        return redirect(url_for("backups.backups_list"))

    filepath = os.path.join(_backup_dir(), filename)
    real_backup_dir = os.path.realpath(_backup_dir())
    real_filepath = os.path.realpath(filepath)
    if not real_filepath.startswith(real_backup_dir + os.sep):
        flash("Invalid file path.", "error")
        return redirect(url_for("backups.backups_list"))

    if not os.path.isfile(real_filepath):
        flash("Backup file not found.", "error")
        return redirect(url_for("backups.backups_list"))

    cfg = current_app.config
    tmp_dir = tempfile.mkdtemp(prefix="lpmud_restore_")

    try:
        # Extract archive to temp directory
        with tarfile.open(real_filepath, "r:gz") as tar:
            if hasattr(tarfile, "data_filter"):
                tar.extractall(path=tmp_dir, filter="data")
            else:
                for member in tar.getmembers():
                    member_path = os.path.join(tmp_dir, member.name)
                    real_member = os.path.realpath(member_path)
                    real_tmp = os.path.realpath(tmp_dir)
                    if not real_member.startswith(real_tmp + os.sep):
                        raise ValueError(
                            f"Path traversal detected in archive: {member.name}"
                        )
                tar.extractall(path=tmp_dir)

        # Copy each component to its destination if present in archive
        restored = []
        for component, config_key in _BACKUP_COMPONENTS.items():
            extracted = os.path.join(tmp_dir, component)
            if os.path.isdir(extracted):
                dest_path = cfg[config_key]
                if os.path.isdir(dest_path):
                    shutil.rmtree(dest_path)
                shutil.copytree(extracted, dest_path)
                restored.append(component)

        if restored:
            flash(f"Restored {', '.join(restored)} from {filename}", "success")
        else:
            flash(f"No recognized components found in {filename}.", "warning")
    except Exception as e:
        flash(f"Restore failed: {e}", "error")
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)

    return redirect(url_for("backups.backups_list"))


@backups_bp.route("/import", methods=["POST"])
@login_required
def import_backup():
    if "file" not in request.files:
        flash("No file selected.", "error")
        return redirect(url_for("backups.backups_list"))

    uploaded = request.files["file"]
    if not uploaded.filename:
        flash("No file selected.", "error")
        return redirect(url_for("backups.backups_list"))

    original = secure_filename(uploaded.filename)
    if not original.endswith(".tar.gz"):
        flash("Only .tar.gz files are accepted.", "error")
        return redirect(url_for("backups.backups_list"))

    # Validate archive integrity by saving to a temp file first
    tmp_fd, tmp_path = tempfile.mkstemp(suffix=".tar.gz")
    try:
        os.close(tmp_fd)
        uploaded.save(tmp_path)

        # Verify it's a valid tar.gz
        with tarfile.open(tmp_path, "r:gz") as tar:
            tar.getnames()

        # Move to backup dir with import prefix
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        base_name = original.replace(".tar.gz", "")
        safe_name = re.sub(r"[^a-zA-Z0-9_-]", "", base_name)[:50]
        dest_name = f"imported_{timestamp}_{safe_name}.tar.gz"
        dest_path = os.path.join(_backup_dir(), dest_name)

        shutil.move(tmp_path, dest_path)
        flash(f"Imported: {dest_name} — use Restore to apply it.", "success")
    except tarfile.TarError:
        flash("Invalid archive: file is not a valid .tar.gz.", "error")
    except Exception as e:
        flash(f"Import failed: {e}", "error")
    finally:
        if os.path.exists(tmp_path):
            os.remove(tmp_path)

    return redirect(url_for("backups.backups_list"))


@backups_bp.route("/delete", methods=["POST"])
@login_required
def delete_backup():
    filename = request.form.get("filename", "")
    if not _validate_filename(filename):
        flash("Invalid filename.", "error")
        return redirect(url_for("backups.backups_list"))

    filepath = os.path.join(_backup_dir(), filename)
    real_backup_dir = os.path.realpath(_backup_dir())
    real_filepath = os.path.realpath(filepath)
    if not real_filepath.startswith(real_backup_dir + os.sep):
        flash("Invalid file path.", "error")
        return redirect(url_for("backups.backups_list"))

    if not os.path.isfile(real_filepath):
        flash("File not found.", "error")
        return redirect(url_for("backups.backups_list"))

    try:
        os.remove(real_filepath)
        flash(f"Deleted {filename}", "success")
    except Exception as e:
        flash(f"Delete failed: {e}", "error")

    return redirect(url_for("backups.backups_list"))
