"""Backups blueprint: create, list, download, restore, delete tar.gz archives."""

import os
import re
import tarfile
import glob
from datetime import datetime

from flask import (
    Blueprint, render_template, request, redirect,
    url_for, flash, current_app, send_file, abort,
)

from auth import login_required

backups_bp = Blueprint("backups", __name__, url_prefix="/backups")

# Allowed backup targets mapped to container paths
TARGETS = {
    "mudlib": {"config_key": "MUDLIB_PATH", "label": "Mudlib (room files, objects)"},
    "saves": {"config_key": "SAVE_PATH", "label": "Player saves"},
    "logs": {"config_key": "LOG_PATH", "label": "Server logs"},
}

# Filename safety: only alphanumeric, hyphens, underscores
_SAFE_LABEL_RE = re.compile(r"^[a-zA-Z0-9_-]{0,50}$")
_SAFE_FILENAME_RE = re.compile(r"^[a-zA-Z0-9_.-]+$")


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
        # Parse target from filename: {target}_{timestamp}_{label}.tar.gz
        parts = name.replace(".tar.gz", "").split("_", 2)
        target = parts[0] if parts else "unknown"
        backups.append({
            "name": name,
            "target": target,
            "time": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M:%S"),
            "size": _format_size(stat.st_size),
            "size_bytes": stat.st_size,
        })
    return backups


@backups_bp.route("/")
@login_required
def backups_list():
    backups = _list_backups()
    return render_template("backups.html", backups=backups, targets=TARGETS)


@backups_bp.route("/create", methods=["POST"])
@login_required
def create_backup():
    target = request.form.get("target", "")
    label = request.form.get("label", "").strip()

    # Validate target
    if target not in TARGETS:
        flash("Invalid backup target.", "error")
        return redirect(url_for("backups.backups_list"))

    # Validate and sanitize label
    # Strip to alphanumeric + hyphens
    label = re.sub(r"[^a-zA-Z0-9_-]", "", label)[:50]

    source_path = current_app.config[TARGETS[target]["config_key"]]
    if not os.path.isdir(source_path):
        flash(f"Source directory not found: {source_path}", "error")
        return redirect(url_for("backups.backups_list"))

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    parts = [target, timestamp]
    if label:
        parts.append(label)
    filename = "_".join(parts) + ".tar.gz"
    dest = os.path.join(_backup_dir(), filename)

    try:
        with tarfile.open(dest, "w:gz") as tar:
            tar.add(source_path, arcname=os.path.basename(source_path))
        flash(f"Backup created: {filename}", "success")
    except Exception as e:
        flash(f"Backup failed: {e}", "error")

    return redirect(url_for("backups.backups_list"))


@backups_bp.route("/download/<filename>")
@login_required
def download_backup(filename):
    if not _validate_filename(filename):
        abort(400)
    filepath = os.path.join(_backup_dir(), filename)
    # Ensure the resolved path is within the backup directory
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

    # Determine target from filename
    parts = os.path.basename(filename).replace(".tar.gz", "").split("_", 2)
    target = parts[0] if parts else ""
    if target not in TARGETS:
        flash("Cannot determine restore target from filename.", "error")
        return redirect(url_for("backups.backups_list"))

    dest_path = current_app.config[TARGETS[target]["config_key"]]
    dest_parent = os.path.dirname(dest_path)

    try:
        with tarfile.open(real_filepath, "r:gz") as tar:
            # CVE-2007-4559 mitigation: use data_filter (Python 3.12+)
            if hasattr(tarfile, "data_filter"):
                tar.extractall(path=dest_parent, filter="data")
            else:
                # Fallback: manually validate each member
                for member in tar.getmembers():
                    member_path = os.path.join(dest_parent, member.name)
                    real_member = os.path.realpath(member_path)
                    real_dest = os.path.realpath(dest_parent)
                    if not real_member.startswith(real_dest + os.sep):
                        raise ValueError(
                            f"Path traversal detected in archive: {member.name}"
                        )
                tar.extractall(path=dest_parent)
        flash(f"Restored {filename} to {dest_path}", "success")
    except Exception as e:
        flash(f"Restore failed: {e}", "error")

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
