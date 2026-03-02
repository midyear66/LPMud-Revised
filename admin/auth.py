"""Authentication: login/logout, session management, @login_required."""

import secrets
import time
from functools import wraps

from flask import (
    Blueprint, render_template, request, redirect,
    url_for, session, flash, current_app,
)

auth_bp = Blueprint("auth", __name__)

# In-memory rate limiter: {ip: (attempt_count, window_start)}
_login_attempts: dict[str, tuple[int, float]] = {}


def _is_rate_limited(ip: str) -> bool:
    """Check if an IP is currently locked out."""
    cfg = current_app.config
    max_attempts = cfg.get("LOGIN_MAX_ATTEMPTS", 5)
    lockout = cfg.get("LOGIN_LOCKOUT_SECONDS", 60)

    if ip not in _login_attempts:
        return False
    count, window_start = _login_attempts[ip]
    if time.time() - window_start > lockout:
        del _login_attempts[ip]
        return False
    return count >= max_attempts


def _record_attempt(ip: str):
    """Record a failed login attempt."""
    cfg = current_app.config
    lockout = cfg.get("LOGIN_LOCKOUT_SECONDS", 60)
    now = time.time()

    if ip in _login_attempts:
        count, window_start = _login_attempts[ip]
        if now - window_start > lockout:
            _login_attempts[ip] = (1, now)
        else:
            _login_attempts[ip] = (count + 1, window_start)
    else:
        _login_attempts[ip] = (1, now)


def _clear_attempts(ip: str):
    """Clear rate limit state after successful login."""
    _login_attempts.pop(ip, None)


def login_required(f):
    """Decorator: redirect to login if not authenticated or session expired."""
    @wraps(f)
    def decorated(*args, **kwargs):
        if not session.get("logged_in"):
            return redirect(url_for("auth.login"))
        # Check inactivity timeout
        last_active = session.get("last_active", 0)
        timeout = current_app.config.get("SESSION_TIMEOUT_MINUTES", 30) * 60
        if time.time() - last_active > timeout:
            session.clear()
            flash("Session expired. Please log in again.", "warning")
            return redirect(url_for("auth.login"))
        # Refresh activity timestamp
        session["last_active"] = time.time()
        return f(*args, **kwargs)
    return decorated


@auth_bp.route("/login", methods=["GET", "POST"])
def login():
    if session.get("logged_in"):
        return redirect(url_for("dashboard.dashboard"))

    if request.method == "POST":
        ip = request.remote_addr or "unknown"

        if _is_rate_limited(ip):
            flash("Too many login attempts. Try again in 60 seconds.", "error")
            return render_template("login.html"), 429

        password = request.form.get("password", "")
        expected = current_app.config["ADMIN_PASSWORD"]

        if secrets.compare_digest(password, expected):
            _clear_attempts(ip)
            session.permanent = True
            session["logged_in"] = True
            session["last_active"] = time.time()
            return redirect(url_for("dashboard.dashboard"))

        _record_attempt(ip)
        flash("Invalid password.", "error")

    return render_template("login.html")


@auth_bp.route("/logout")
def logout():
    session.clear()
    flash("Logged out.", "info")
    return redirect(url_for("auth.login"))
