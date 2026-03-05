"""Player editor blueprint: list, view, and edit player save files."""

import os
import re

from flask import (
    Blueprint, render_template, request, redirect,
    url_for, flash, current_app,
)

from auth import login_required

players_bp = Blueprint("players", __name__)

# Fields editable in the player editor, grouped by category.
# Each entry: (variable_name, label, input_type, description)
EDITABLE_FIELDS = {
    "Identity": [
        ("name", "Name", "text", "Player name (lowercase)"),
        ("title", "Title", "text", "Displayed title"),
        ("al_title", "Alignment Title", "text", "e.g. neutral, good, evil"),
        ("gender", "Gender", "select:0=Neuter,1=Male,2=Female", ""),
        ("level", "Level", "number", "Player level (-1 = new)"),
    ],
    "Stats": [
        ("Str", "Strength", "number", ""),
        ("Int", "Intelligence", "number", ""),
        ("Con", "Constitution", "number", ""),
        ("Dex", "Dexterity", "number", ""),
    ],
    "Vitals": [
        ("hit_point", "Hit Points", "number", "Current HP"),
        ("max_hp", "Max HP", "number", ""),
        ("spell_points", "Spell Points", "number", "Current SP"),
        ("max_sp", "Max SP", "number", ""),
        ("experience", "Experience", "number", "XP total"),
        ("alignment", "Alignment", "number", "Positive=good, negative=evil"),
        ("money", "Gold", "number", "Coins carried"),
    ],
    "Status": [
        ("ghost", "Ghost", "number", "0=alive, 1=ghost"),
        ("frog", "Frog", "number", "0=normal, 1=frog"),
        ("dead", "Dead", "number", "0=alive, 1=dead"),
        ("is_invis", "Invisible", "number", "0=visible, 1=invisible"),
        ("whimpy", "Whimpy", "number", "Auto-flee when low HP"),
        ("intoxicated", "Intoxicated", "number", "Ticks remaining drunk"),
        ("stuffed", "Stuffed", "number", "Ticks remaining full"),
        ("soaked", "Soaked", "number", "Ticks remaining soaked"),
        ("headache", "Headache", "number", "Current headache"),
        ("max_headache", "Max Headache", "number", ""),
        ("scar", "Scars", "number", "Bitmask of scars"),
        ("age", "Age", "number", "Heart beat count"),
    ],
    "Messages": [
        ("msgin", "Arrive Message", "text", "e.g. arrives"),
        ("msgout", "Leave Message", "text", "e.g. leaves"),
        ("mmsgin", "Magic Arrive", "text", "Magical arrival message"),
        ("mmsgout", "Magic Leave", "text", "Magical departure message"),
    ],
    "Other": [
        ("mailaddr", "Email", "text", "Player email address"),
        ("quests", "Quests", "text", "Completed quest list"),
        ("auto_load", "Auto Load", "text", "Auto-loaded objects"),
        ("flags", "Flags", "text", "Bit field of flags"),
        ("called_from_ip", "Last IP", "text", "Last login IP"),
        ("tot_value", "Total Value", "number", "Saved item values"),
        ("access_list", "Access List", "text", "Wizard file access"),
    ],
}

# All known editable variable names
_ALL_FIELD_NAMES = {
    f[0] for fields in EDITABLE_FIELDS.values() for f in fields
}


def _save_dir() -> str:
    """Return the path to the player save directory."""
    return current_app.config["SAVE_PATH"]


def _parse_save_file(path: str) -> dict[str, str]:
    """Parse an ldmud save_object file into a dict of variable_name -> raw_value."""
    data = {}
    try:
        with open(path, "r", errors="replace") as f:
            for line in f:
                line = line.rstrip("\n")
                if not line:
                    continue
                # Format: "variable_name value"
                # The variable name is the first whitespace-delimited token
                parts = line.split(" ", 1)
                if len(parts) == 2:
                    data[parts[0]] = parts[1]
                elif len(parts) == 1:
                    data[parts[0]] = ""
    except (OSError, IOError):
        pass
    return data


def _write_save_file(path: str, data: dict[str, str]):
    """Write a dict back to ldmud save_object format."""
    with open(path, "w") as f:
        for key, value in data.items():
            if value:
                f.write(f"{key} {value}\n")
            else:
                f.write(f"{key}\n")


def _display_value(raw: str) -> str:
    """Convert raw save value to display string. Strips quotes from strings."""
    if raw.startswith('"') and raw.endswith('"'):
        return raw[1:-1]
    return raw


def _to_save_value(field_name: str, input_type: str, value: str) -> str:
    """Convert form input back to save file format."""
    if input_type == "number" or input_type.startswith("select:"):
        # Ensure it's a valid integer
        try:
            return str(int(value))
        except ValueError:
            return "0"
    else:
        # String field — quote it
        # Escape backslashes and quotes within
        escaped = value.replace("\\", "\\\\").replace('"', '\\"')
        return f'"{escaped}"'


def _get_field_type(field_name: str) -> str:
    """Look up the input type for a field name."""
    for fields in EDITABLE_FIELDS.values():
        for fname, _, ftype, _ in fields:
            if fname == field_name:
                return ftype
    return "text"


def _safe_filename(name: str) -> bool:
    """Validate that a player filename is safe."""
    return bool(re.match(r'^[a-z][a-z0-9]{0,10}$', name))


@players_bp.route("/players")
@login_required
def players_list():
    save_dir = _save_dir()
    players = []
    if os.path.isdir(save_dir):
        for f in sorted(os.listdir(save_dir)):
            if f.endswith(".o"):
                name = f[:-2]
                path = os.path.join(save_dir, f)
                data = _parse_save_file(path)
                stat = os.stat(path)
                players.append({
                    "name": name,
                    "level": data.get("level", "?"),
                    "title": _display_value(data.get("title", "")),
                    "experience": data.get("experience", "0"),
                    "size": stat.st_size,
                    "gender": {"0": "Neuter", "1": "Male", "2": "Female"}.get(
                        data.get("gender", "0"), "?"
                    ),
                })
    return render_template("players.html", players=players, editing=None)


@players_bp.route("/players/<name>")
@login_required
def player_edit(name):
    if not _safe_filename(name):
        flash("Invalid player name.", "error")
        return redirect(url_for("players.players_list"))

    path = os.path.join(_save_dir(), f"{name}.o")
    if not os.path.isfile(path):
        flash(f"Player '{name}' not found.", "error")
        return redirect(url_for("players.players_list"))

    data = _parse_save_file(path)

    # Build field values for the template
    field_groups = {}
    for group_name, fields in EDITABLE_FIELDS.items():
        group_fields = []
        for fname, label, ftype, desc in fields:
            raw = data.get(fname, "")
            display = _display_value(raw) if raw else ""
            group_fields.append({
                "name": fname,
                "label": label,
                "type": ftype,
                "description": desc,
                "value": display,
                "raw": raw,
            })
        field_groups[group_name] = group_fields

    return render_template(
        "players.html",
        players=None,
        editing=name,
        field_groups=field_groups,
    )


@players_bp.route("/players/<name>/save", methods=["POST"])
@login_required
def player_save(name):
    if not _safe_filename(name):
        flash("Invalid player name.", "error")
        return redirect(url_for("players.players_list"))

    path = os.path.join(_save_dir(), f"{name}.o")
    if not os.path.isfile(path):
        flash(f"Player '{name}' not found.", "error")
        return redirect(url_for("players.players_list"))

    # Read existing data to preserve non-editable fields
    data = _parse_save_file(path)

    # Update only the fields we know about
    changes = []
    for fname in _ALL_FIELD_NAMES:
        if fname in request.form:
            new_val = request.form[fname]
            ftype = _get_field_type(fname)
            save_val = _to_save_value(fname, ftype, new_val)
            old_val = data.get(fname, "")
            if save_val != old_val:
                changes.append(fname)
            data[fname] = save_val

    _write_save_file(path, data)

    if changes:
        flash(f"Saved {name}: updated {', '.join(changes)}.", "success")
    else:
        flash(f"Saved {name}: no changes.", "info")

    return redirect(url_for("players.player_edit", name=name))
