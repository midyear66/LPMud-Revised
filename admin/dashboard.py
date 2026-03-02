"""Dashboard blueprint: server status overview."""

import os
import glob
import subprocess
from datetime import datetime

from flask import Blueprint, render_template, current_app

from auth import login_required

dashboard_bp = Blueprint("dashboard", __name__)


def _get_backup_info(backup_path: str) -> dict:
    """Get info about the most recent backup."""
    if not os.path.isdir(backup_path):
        return {"count": 0, "latest": None, "total_size": 0}

    files = sorted(glob.glob(os.path.join(backup_path, "*.tar.gz")), reverse=True)
    total_size = sum(os.path.getsize(f) for f in files)
    latest = None
    if files:
        stat = os.stat(files[0])
        latest = {
            "name": os.path.basename(files[0]),
            "time": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M"),
            "size": _format_size(stat.st_size),
        }
    return {"count": len(files), "latest": latest, "total_size": _format_size(total_size)}


def _format_size(size_bytes: int) -> str:
    """Format bytes as human-readable string."""
    for unit in ("B", "KB", "MB", "GB"):
        if size_bytes < 1024:
            return f"{size_bytes:.1f} {unit}"
        size_bytes /= 1024
    return f"{size_bytes:.1f} TB"


def _get_map_info(docs_path: str) -> dict:
    """Get info about the world map file."""
    map_path = os.path.join(docs_path, "world-map.png")
    if not os.path.isfile(map_path):
        return {"exists": False}
    stat = os.stat(map_path)
    return {
        "exists": True,
        "time": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M"),
        "size": _format_size(stat.st_size),
    }


def _get_room_count(mudlib_path: str) -> int:
    """Count .c files in mudlib/room/."""
    room_dir = os.path.join(mudlib_path, "room")
    if not os.path.isdir(room_dir):
        return 0
    count = 0
    for dirpath, _, filenames in os.walk(room_dir):
        count += sum(1 for f in filenames if f.endswith(".c"))
    return count


def _get_scheduler_info():
    """Get next scheduled job info from APScheduler."""
    try:
        from scheduler import get_scheduler
        sched = get_scheduler()
        if sched and sched.running:
            jobs = sched.get_jobs()
            if jobs:
                next_run = jobs[0].next_run_time
                if next_run:
                    return {
                        "active": True,
                        "next_run": next_run.strftime("%Y-%m-%d %H:%M"),
                        "job_count": len(jobs),
                    }
        return {"active": False}
    except Exception:
        return {"active": False}


def _get_server_info() -> dict:
    """Get game server status via Docker."""
    try:
        from server import get_server_status
        return get_server_status()
    except Exception:
        return {"available": False, "status": "unavailable"}


@dashboard_bp.route("/dashboard")
@login_required
def dashboard():
    cfg = current_app.config
    context = {
        "backup_info": _get_backup_info(cfg["BACKUP_PATH"]),
        "map_info": _get_map_info(cfg["DOCS_PATH"]),
        "room_count": _get_room_count(cfg["MUDLIB_PATH"]),
        "scheduler_info": _get_scheduler_info(),
        "server_info": _get_server_info(),
    }
    return render_template("dashboard.html", **context)
