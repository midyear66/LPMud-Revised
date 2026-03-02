"""Environment-based configuration for the admin dashboard."""

import os
import sys


class Config:
    SECRET_KEY = os.environ.get("SECRET_KEY", "")
    ADMIN_PASSWORD = os.environ.get("ADMIN_PASSWORD", "")

    # Paths inside the Docker container
    MUDLIB_PATH = os.environ.get("MUDLIB_PATH", "/mud/lib")
    SAVE_PATH = os.environ.get("SAVE_PATH", "/mud/save")
    LOG_PATH = os.environ.get("LOG_PATH", "/mud/log")
    DOCS_PATH = os.environ.get("DOCS_PATH", "/docs")
    SCRIPTS_PATH = os.environ.get("SCRIPTS_PATH", "/scripts")
    BACKUP_PATH = os.environ.get("BACKUP_PATH", "/backups")
    ADMIN_DATA_PATH = os.environ.get("ADMIN_DATA_PATH", "/admin-data")

    # Upload limit (500 MB)
    MAX_CONTENT_LENGTH = 500 * 1024 * 1024

    # Docker container name for the game server
    LPMUD_CONTAINER_NAME = os.environ.get("LPMUD_CONTAINER_NAME", "lpmud-server")

    # Session
    SESSION_TIMEOUT_MINUTES = int(os.environ.get("SESSION_TIMEOUT_MINUTES", "30"))
    PERMANENT_SESSION_LIFETIME = SESSION_TIMEOUT_MINUTES * 60

    # Rate limiting
    LOGIN_MAX_ATTEMPTS = 5
    LOGIN_LOCKOUT_SECONDS = 60

    # Session cookie security
    SESSION_COOKIE_HTTPONLY = True
    SESSION_COOKIE_SAMESITE = "Lax"

    @classmethod
    def validate(cls):
        """Refuse to start if required env vars are missing."""
        if not cls.SECRET_KEY:
            print("FATAL: SECRET_KEY env var must be set.", file=sys.stderr)
            sys.exit(1)
        if not cls.ADMIN_PASSWORD:
            print("FATAL: ADMIN_PASSWORD env var must be set.", file=sys.stderr)
            sys.exit(1)
