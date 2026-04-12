"""Backup retention settings and pruning logic."""

import glob
import json
import os
import re
from datetime import datetime

_DEFAULTS = {
    "enabled": True,
    "keep_daily": 7,
    "keep_weekly": 4,
    "keep_monthly": 6,
    "keep_yearly": 3,
    "prune_manual": False,
    "prune_imported": False,
}

# Matches: prefix_YYYYMMDD_HHMMSS[_optional].tar.gz
_BACKUP_RE = re.compile(
    r"^(full|scheduled|imported)_(\d{8})_(\d{6})(?:_.*)?\.tar\.gz$"
)


def _settings_path(data_path):
    return os.path.join(data_path, "retention.json")


def load_retention_settings(data_path):
    """Load retention settings from JSON, returning defaults for missing keys."""
    path = _settings_path(data_path)
    settings = dict(_DEFAULTS)
    if os.path.isfile(path):
        try:
            with open(path) as f:
                stored = json.load(f)
            settings.update(stored)
        except (json.JSONDecodeError, OSError):
            pass
    return settings


def save_retention_settings(settings, data_path):
    """Atomically write retention settings to JSON."""
    os.makedirs(data_path, exist_ok=True)
    path = _settings_path(data_path)
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(settings, f, indent=2)
    os.replace(tmp, path)


def classify_backups(app_config):
    """Classify backups by retention tier. Returns dict of filename -> tier label."""
    data_path = app_config.get("ADMIN_DATA_PATH", "/admin-data")
    settings = load_retention_settings(data_path)

    if not settings.get("enabled", True):
        return {}

    backup_dir = app_config.get("BACKUP_PATH", "/backups")
    if not os.path.isdir(backup_dir):
        return {}

    keep_daily = max(0, int(settings.get("keep_daily", 7)))
    keep_weekly = max(0, int(settings.get("keep_weekly", 4)))
    keep_monthly = max(0, int(settings.get("keep_monthly", 6)))
    keep_yearly = max(0, int(settings.get("keep_yearly", 3)))
    prune_manual = settings.get("prune_manual", False)
    prune_imported = settings.get("prune_imported", False)

    # Parse and filter backup files
    candidates = []
    for filepath in glob.glob(os.path.join(backup_dir, "*.tar.gz")):
        name = os.path.basename(filepath)
        m = _BACKUP_RE.match(name)
        if not m:
            continue
        prefix, date_str, time_str = m.group(1), m.group(2), m.group(3)

        if prefix == "full" and not prune_manual:
            continue
        if prefix == "imported" and not prune_imported:
            continue

        try:
            ts = datetime.strptime(date_str + time_str, "%Y%m%d%H%M%S")
        except ValueError:
            continue

        candidates.append((ts, name))

    candidates.sort(key=lambda x: x[0], reverse=True)

    # Greedy tier assignment (same logic as prune_backups)
    tiers = {}
    daily_count = 0
    weekly_seen = set()
    weekly_count = 0
    monthly_seen = set()
    monthly_count = 0
    yearly_seen = set()
    yearly_count = 0

    for ts, name in candidates:
        if daily_count < keep_daily:
            tiers[name] = "daily"
            daily_count += 1
            continue

        yw = ts.isocalendar()[:2]
        if yw not in weekly_seen and weekly_count < keep_weekly:
            tiers[name] = "weekly"
            weekly_seen.add(yw)
            weekly_count += 1
            continue

        ym = (ts.year, ts.month)
        if ym not in monthly_seen and monthly_count < keep_monthly:
            tiers[name] = "monthly"
            monthly_seen.add(ym)
            monthly_count += 1
            continue

        if ts.year not in yearly_seen and yearly_count < keep_yearly:
            tiers[name] = "yearly"
            yearly_seen.add(ts.year)
            yearly_count += 1
            continue

        tiers[name] = "expires"

    return tiers


def prune_backups(app_config):
    """Delete backups that exceed the retention policy. Returns list of deleted filenames."""
    data_path = app_config.get("ADMIN_DATA_PATH", "/admin-data")
    settings = load_retention_settings(data_path)

    if not settings.get("enabled", True):
        return []

    backup_dir = app_config.get("BACKUP_PATH", "/backups")
    if not os.path.isdir(backup_dir):
        return []

    keep_daily = max(0, int(settings.get("keep_daily", 7)))
    keep_weekly = max(0, int(settings.get("keep_weekly", 4)))
    keep_monthly = max(0, int(settings.get("keep_monthly", 6)))
    keep_yearly = max(0, int(settings.get("keep_yearly", 3)))
    prune_manual = settings.get("prune_manual", False)
    prune_imported = settings.get("prune_imported", False)

    # Parse and filter backup files
    candidates = []
    for filepath in glob.glob(os.path.join(backup_dir, "*.tar.gz")):
        name = os.path.basename(filepath)
        m = _BACKUP_RE.match(name)
        if not m:
            continue
        prefix, date_str, time_str = m.group(1), m.group(2), m.group(3)

        # Filter by type
        if prefix == "full" and not prune_manual:
            continue
        if prefix == "imported" and not prune_imported:
            continue

        try:
            ts = datetime.strptime(date_str + time_str, "%Y%m%d%H%M%S")
        except ValueError:
            continue

        candidates.append((ts, name, filepath))

    # Sort newest first
    candidates.sort(key=lambda x: x[0], reverse=True)

    # Greedy tier assignment
    kept = set()
    daily_count = 0
    weekly_seen = set()
    weekly_count = 0
    monthly_seen = set()
    monthly_count = 0
    yearly_seen = set()
    yearly_count = 0

    for ts, name, filepath in candidates:
        # Daily tier: keep the N most recent
        if daily_count < keep_daily:
            kept.add(name)
            daily_count += 1
            continue

        # Weekly tier: keep 1 per unique ISO week
        yw = ts.isocalendar()[:2]  # (year, week)
        if yw not in weekly_seen and weekly_count < keep_weekly:
            kept.add(name)
            weekly_seen.add(yw)
            weekly_count += 1
            continue

        # Monthly tier: keep 1 per unique (year, month)
        ym = (ts.year, ts.month)
        if ym not in monthly_seen and monthly_count < keep_monthly:
            kept.add(name)
            monthly_seen.add(ym)
            monthly_count += 1
            continue

        # Yearly tier: keep 1 per unique year
        if ts.year not in yearly_seen and yearly_count < keep_yearly:
            kept.add(name)
            yearly_seen.add(ts.year)
            yearly_count += 1
            continue

    # Delete anything not kept
    deleted = []
    for ts, name, filepath in candidates:
        if name not in kept:
            try:
                os.remove(filepath)
                deleted.append(name)
            except OSError:
                pass

    return deleted
