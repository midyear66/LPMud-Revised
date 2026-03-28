"""Server blueprint: start, stop, restart the LPmud game server container."""

import os
import threading

from flask import Blueprint, redirect, url_for, flash, current_app, request

from auth import login_required

server_bp = Blueprint("server", __name__, url_prefix="/server")

_docker_client = None


def _get_docker():
    """Lazily connect to Docker. Returns client or None."""
    global _docker_client
    if _docker_client is not None:
        return _docker_client
    try:
        import docker
        _docker_client = docker.from_env()
        _docker_client.ping()
        return _docker_client
    except Exception:
        _docker_client = None
        return None


def get_server_status() -> dict:
    """Return server status dict for the dashboard."""
    client = _get_docker()
    if client is None:
        return {"available": False, "status": "unavailable"}
    try:
        name = current_app.config.get("LPMUD_CONTAINER_NAME", "lpmud-server")
        container = client.containers.get(name)
        return {
            "available": True,
            "status": container.status,
            "running": container.status == "running",
        }
    except Exception:
        return {"available": True, "status": "not found", "running": False}


@server_bp.route("/start", methods=["POST"])
@login_required
def start_server():
    client = _get_docker()
    if client is None:
        flash("Docker is not available.", "error")
        return redirect(url_for("dashboard.dashboard"))
    try:
        name = current_app.config.get("LPMUD_CONTAINER_NAME", "lpmud-server")
        container = client.containers.get(name)
        container.start()
        flash("Game server started.", "success")
    except Exception as e:
        flash(f"Failed to start server: {e}", "error")
    return redirect(url_for("dashboard.dashboard"))


@server_bp.route("/stop", methods=["POST"])
@login_required
def stop_server():
    client = _get_docker()
    if client is None:
        flash("Docker is not available.", "error")
        return redirect(url_for("dashboard.dashboard"))
    try:
        name = current_app.config.get("LPMUD_CONTAINER_NAME", "lpmud-server")
        container = client.containers.get(name)
        container.stop(timeout=30)
        flash("Game server stopped.", "success")
    except Exception as e:
        flash(f"Failed to stop server: {e}", "error")
    return redirect(url_for("dashboard.dashboard"))


def _delayed_container_stop(container_name, delay_seconds):
    """Background thread: wait for the countdown, then stop the container."""
    import time
    import docker

    time.sleep(delay_seconds + 15)  # buffer after Armageddon finishes
    try:
        client = docker.from_env()
        container = client.containers.get(container_name)
        if container.status == "running":
            container.stop(timeout=30)
    except Exception:
        pass


@server_bp.route("/graceful-stop", methods=["POST"])
@login_required
def graceful_stop_server():
    client = _get_docker()
    if client is None:
        flash("Docker is not available.", "error")
        return redirect(url_for("dashboard.dashboard"))
    try:
        delay = int(request.form.get("delay", 5))
    except (TypeError, ValueError):
        delay = 5
    delay = max(1, min(delay, 30))
    mudlib = current_app.config.get("MUDLIB_PATH", "/mud/lib")
    trigger = os.path.join(mudlib, "shutdown_request")
    try:
        with open(trigger, "w") as f:
            f.write(str(delay) + "\n")
        name = current_app.config.get("LPMUD_CONTAINER_NAME", "lpmud-server")
        # Ensure the container actually stops after the countdown
        t = threading.Thread(
            target=_delayed_container_stop,
            args=(name, delay * 60),
            daemon=True,
        )
        t.start()
        flash(
            f"Graceful shutdown started. Players will be warned for "
            f"{delay} minute{'s' if delay != 1 else ''} before the server stops.",
            "success",
        )
    except Exception as e:
        flash(f"Failed to initiate graceful shutdown: {e}", "error")
    return redirect(url_for("dashboard.dashboard"))


@server_bp.route("/restart", methods=["POST"])
@login_required
def restart_server():
    client = _get_docker()
    if client is None:
        flash("Docker is not available.", "error")
        return redirect(url_for("dashboard.dashboard"))
    try:
        name = current_app.config.get("LPMUD_CONTAINER_NAME", "lpmud-server")
        container = client.containers.get(name)
        container.restart(timeout=30)
        flash("Game server restarted.", "success")
    except Exception as e:
        flash(f"Failed to restart server: {e}", "error")
    return redirect(url_for("dashboard.dashboard"))
