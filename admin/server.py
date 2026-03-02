"""Server blueprint: start, stop, restart the LPmud game server container."""

from flask import Blueprint, redirect, url_for, flash, current_app

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
