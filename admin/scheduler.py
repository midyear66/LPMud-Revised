"""Scheduler blueprint: APScheduler cron UI for map regeneration."""

import os
import subprocess
import sys
from datetime import datetime

from flask import (
    Blueprint, render_template, request, redirect,
    url_for, flash, current_app,
)
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.jobstores.sqlalchemy import SQLAlchemyJobStore
from apscheduler.triggers.cron import CronTrigger

from auth import login_required

scheduler_bp = Blueprint("scheduler", __name__, url_prefix="/scheduler")

_scheduler: BackgroundScheduler | None = None
MAP_JOB_ID = "map_regeneration"


def get_scheduler() -> BackgroundScheduler | None:
    return _scheduler


def init_scheduler(app):
    """Initialize and start APScheduler with SQLite persistence."""
    global _scheduler

    data_dir = app.config.get("ADMIN_DATA_PATH", "/admin-data")
    os.makedirs(data_dir, exist_ok=True)
    db_path = os.path.join(data_dir, "scheduler.db")

    jobstores = {"default": SQLAlchemyJobStore(url=f"sqlite:///{db_path}")}
    _scheduler = BackgroundScheduler(jobstores=jobstores)
    _scheduler.start()


def _needs_regeneration(app_config) -> bool:
    """Check if any room/*.c files are newer than world-map.png."""
    docs_path = app_config["DOCS_PATH"]
    mudlib_path = app_config["MUDLIB_PATH"]
    map_path = os.path.join(docs_path, "world-map.png")

    if not os.path.isfile(map_path):
        return True

    map_mtime = os.path.getmtime(map_path)
    room_dir = os.path.join(mudlib_path, "room")
    if not os.path.isdir(room_dir):
        return False

    for dirpath, _, filenames in os.walk(room_dir):
        for fname in filenames:
            if fname.endswith(".c"):
                if os.path.getmtime(os.path.join(dirpath, fname)) > map_mtime:
                    return True
    return False


def _run_map_generation(app_config=None):
    """Execute generate-map.py directly (no Docker-in-Docker)."""
    # When called from scheduler, we need the app config
    if app_config is None:
        from flask import current_app
        app_config = current_app.config

    scripts_path = app_config["SCRIPTS_PATH"]
    mudlib_path = app_config["MUDLIB_PATH"]
    docs_path = app_config["DOCS_PATH"]

    script = os.path.join(scripts_path, "generate-map.py")
    if not os.path.isfile(script):
        raise FileNotFoundError(f"generate-map.py not found at {script}")

    result = subprocess.run(
        [sys.executable, script, "--mudlib", mudlib_path, "--output", docs_path],
        capture_output=True,
        text=True,
        timeout=300,
    )

    if result.returncode != 0:
        raise RuntimeError(f"Map generation failed: {result.stderr}")

    return result.stdout


def _scheduled_job():
    """Job function called by APScheduler — checks freshness then regenerates."""
    from app import create_app
    app = create_app.__wrapped__() if hasattr(create_app, "__wrapped__") else None

    # Read config from environment directly since we're outside request context
    from config import Config
    app_config = {
        "MUDLIB_PATH": Config.MUDLIB_PATH,
        "DOCS_PATH": Config.DOCS_PATH,
        "SCRIPTS_PATH": Config.SCRIPTS_PATH,
    }

    if _needs_regeneration(app_config):
        _run_map_generation(app_config)


@scheduler_bp.route("/")
@login_required
def scheduler_view():
    sched = get_scheduler()
    job = sched.get_job(MAP_JOB_ID) if sched else None

    schedule_info = None
    if job and job.next_run_time:
        trigger = job.trigger
        # Extract hour/minute from CronTrigger
        hour = minute = "?"
        if isinstance(trigger, CronTrigger):
            for field in trigger.fields:
                if field.name == "hour":
                    hour = str(field)
                elif field.name == "minute":
                    minute = str(field)
        schedule_info = {
            "hour": hour,
            "minute": minute,
            "next_run": job.next_run_time.strftime("%Y-%m-%d %H:%M"),
        }

    # Check if map needs regeneration
    needs_regen = _needs_regeneration(current_app.config)

    return render_template(
        "scheduler.html",
        schedule_info=schedule_info,
        needs_regen=needs_regen,
    )


@scheduler_bp.route("/set", methods=["POST"])
@login_required
def set_schedule():
    try:
        hour = int(request.form.get("hour", "2"))
        minute = int(request.form.get("minute", "0"))
    except ValueError:
        flash("Hour and minute must be integers.", "error")
        return redirect(url_for("scheduler.scheduler_view"))

    if not (0 <= hour <= 23):
        flash("Hour must be between 0 and 23.", "error")
        return redirect(url_for("scheduler.scheduler_view"))
    if not (0 <= minute <= 59):
        flash("Minute must be between 0 and 59.", "error")
        return redirect(url_for("scheduler.scheduler_view"))

    sched = get_scheduler()
    if not sched:
        flash("Scheduler not initialized.", "error")
        return redirect(url_for("scheduler.scheduler_view"))

    # Remove existing job if any
    if sched.get_job(MAP_JOB_ID):
        sched.remove_job(MAP_JOB_ID)

    sched.add_job(
        _scheduled_job,
        CronTrigger(hour=hour, minute=minute),
        id=MAP_JOB_ID,
        name="Map Regeneration",
        replace_existing=True,
    )

    flash(f"Schedule set: daily at {hour:02d}:{minute:02d}", "success")
    return redirect(url_for("scheduler.scheduler_view"))


@scheduler_bp.route("/remove", methods=["POST"])
@login_required
def remove_schedule():
    sched = get_scheduler()
    if sched and sched.get_job(MAP_JOB_ID):
        sched.remove_job(MAP_JOB_ID)
        flash("Schedule removed.", "success")
    else:
        flash("No schedule to remove.", "warning")
    return redirect(url_for("scheduler.scheduler_view"))


@scheduler_bp.route("/run-now", methods=["POST"])
@login_required
def run_now():
    try:
        output = _run_map_generation(current_app.config)
        flash(f"Map regenerated successfully.", "success")
    except FileNotFoundError as e:
        flash(str(e), "error")
    except subprocess.TimeoutExpired:
        flash("Map generation timed out (5 min limit).", "error")
    except RuntimeError as e:
        flash(str(e), "error")
    except Exception as e:
        flash(f"Unexpected error: {e}", "error")

    return redirect(url_for("scheduler.scheduler_view"))
