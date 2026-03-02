"""Flask app factory for the LPmud admin dashboard."""

from datetime import timedelta

from flask import Flask, session, redirect, url_for
from flask_wtf.csrf import CSRFProtect

from config import Config

csrf = CSRFProtect()


def create_app():
    Config.validate()

    app = Flask(__name__)
    app.config.from_object(Config)
    app.permanent_session_lifetime = timedelta(
        minutes=Config.SESSION_TIMEOUT_MINUTES
    )

    csrf.init_app(app)

    # Register blueprints
    from auth import auth_bp
    from dashboard import dashboard_bp
    from backups import backups_bp
    from scheduler import scheduler_bp, init_scheduler
    from mapviewer import map_bp

    app.register_blueprint(auth_bp)
    app.register_blueprint(dashboard_bp)
    app.register_blueprint(backups_bp)
    app.register_blueprint(scheduler_bp)
    app.register_blueprint(map_bp)

    # Initialize APScheduler
    init_scheduler(app)

    # Root redirect
    @app.route("/")
    def index():
        return redirect(url_for("dashboard.dashboard"))

    # Security headers on every response
    @app.after_request
    def set_security_headers(response):
        response.headers["X-Content-Type-Options"] = "nosniff"
        response.headers["X-Frame-Options"] = "DENY"
        response.headers["X-XSS-Protection"] = "1; mode=block"
        response.headers["Referrer-Policy"] = "strict-origin-when-cross-origin"
        response.headers["Content-Security-Policy"] = (
            "default-src 'self'; "
            "script-src 'self' https://unpkg.com; "
            "style-src 'self' 'unsafe-inline' https://unpkg.com; "
            "img-src 'self' data:"
        )
        # Prevent caching of authenticated pages
        if session.get("logged_in"):
            response.headers["Cache-Control"] = "no-store"
        return response

    return app


if __name__ == "__main__":
    application = create_app()
    application.run(host="0.0.0.0", port=8080, debug=True)
