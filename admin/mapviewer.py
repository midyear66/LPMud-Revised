"""Map viewer blueprint: Leaflet-based interactive world map."""

import os

from flask import Blueprint, render_template, send_file, current_app, abort

from auth import login_required

map_bp = Blueprint("map", __name__, url_prefix="/map")


@map_bp.route("/")
@login_required
def map_view():
    docs_path = current_app.config["DOCS_PATH"]
    map_path = os.path.join(docs_path, "world-map.png")
    map_exists = os.path.isfile(map_path)
    return render_template("map.html", map_exists=map_exists)


@map_bp.route("/image")
@login_required
def map_image():
    docs_path = current_app.config["DOCS_PATH"]
    map_path = os.path.join(docs_path, "world-map.png")
    if not os.path.isfile(map_path):
        abort(404)
    return send_file(map_path, mimetype="image/png")
