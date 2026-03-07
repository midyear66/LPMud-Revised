#!/usr/bin/env python3
"""Generate a graphical PNG world map for LPmud 2.4.5.

Auto-discovers rooms by parsing mudlib/room/ .c files, builds a graph
with NetworkX, lays out rooms using a direction-faithful BFS grid layout
(compass exits map to grid offsets), and renders the map as a PNG with Pillow.

Also generates docs/room-connectivity.txt and docs/world-map.txt.

Usage:
    python3 scripts/generate-map.py [--mudlib path] [--output path]
"""

import argparse
import json
import math
import os
import re
import sys
from collections import defaultdict, deque

import networkx as nx
from PIL import Image, ImageDraw, ImageFont

# ---------------------------------------------------------------------------
# Canvas
# ---------------------------------------------------------------------------
WIDTH = 7200
HEIGHT = 5400
BG_COLOR = (25, 25, 35)

# ---------------------------------------------------------------------------
# Region color palette: (fill, border, text)
# ---------------------------------------------------------------------------
PALETTE = {
    "village":      ((222, 200, 160), (170, 140, 80),  (60, 40, 10)),
    "forest":       ((60, 140, 60),   (30, 90, 30),    (220, 255, 220)),
    "deep_forest":  ((30, 100, 30),   (15, 65, 15),    (200, 240, 200)),
    "plains":       ((210, 195, 130), (165, 150, 85),   (50, 40, 10)),
    "mountains":    ((160, 160, 175), (100, 100, 115),  (30, 30, 40)),
    "mines":        ((120, 110, 100), (75, 68, 60),     (230, 220, 210)),
    "shore":        ((100, 175, 215), (55, 120, 160),   (10, 30, 50)),
    "island":       ((175, 135, 195), (125, 85, 150),   (40, 10, 50)),
    "underground":  ((150, 105, 60),  (100, 70, 35),    (240, 220, 190)),
    "sea":          ((60, 105, 200),  (35, 70, 150),    (200, 220, 255)),
    "giants":       ((205, 145, 65),  (155, 105, 35),   (50, 30, 5)),
    "orcs":         ((175, 65, 65),   (130, 35, 35),    (255, 220, 220)),
    "east_road":    ((195, 175, 140), (150, 130, 95),   (50, 40, 20)),
    "south_forest": ((25, 80, 25),    (10, 50, 10),     (190, 230, 190)),
    "special":      ((180, 180, 190), (130, 130, 140),  (30, 30, 40)),
    "elevator":     ((165, 165, 210), (120, 120, 165),  (30, 30, 60)),
}

# Files in mudlib/room/ that are NOT actual rooms (base classes, templates, objects)
SKIP_FILES = {
    "room.c",         # base room class
    "def_castle.c",   # castle facade template
    "port_castle.c",  # portable castle object
    "doorway.c",      # configurable doorway template
    "death.c",        # monster object, not a room
    "death_mark.c",   # portable object, not a room
}

# ---------------------------------------------------------------------------
# Region classification
# ---------------------------------------------------------------------------
REGION_RULES = [
    # Subdirectory-based (checked first)
    (r"^south/sforst",  "south_forest"),
    (r"^south/sshore",  "shore"),
    (r"^south/sislnd",  "island"),
    (r"^south/lair",    "island"),
    (r"^mine/",         "mines"),
    (r"^maze1/",        "underground"),
    (r"^sub/",          "underground"),
    (r"^death/",        "special"),
    # Top-level name patterns
    (r"^mount_",        "mountains"),
    (r"^ravine$",       "mountains"),
    (r"^plane\d",       "plains"),
    (r"^ruin$",         "plains"),
    (r"^deep_forest",   "deep_forest"),
    (r"^big_tree$",     "giants"),
    (r"^giant_",        "giants"),
    (r"^orc_",          "orcs"),
    (r"^fortress$",     "orcs"),
    (r"^forest[0-9]",   "forest"),
    (r"^forest1[0-2]$", "deep_forest"),
    (r"^clearing$",     "forest"),
    (r"^wild",          "forest"),
    (r"^slope$",        "forest"),
    (r"^hump$",         "forest"),
    (r"^church$",       "village"),
    (r"^vill_",         "village"),
    (r"^yard$",         "village"),
    (r"^pub",           "village"),
    (r"^narr_alley$",   "village"),
    (r"^bank",          "village"),
    (r"^post$",         "village"),
    (r"^shop$",         "village"),
    (r"^storage$",      "village"),
    (r"^store$",        "village"),
    (r"^adv_",          "village"),
    (r"^crop$",         "village"),
    (r"^eastroad",      "east_road"),
    (r"^inn$",          "east_road"),
    (r"^sunalley",      "east_road"),
    (r"^jetty",         "sea"),
    (r"^sea",           "sea"),
    (r"^elevator$",     "elevator"),
    (r"^attic$",        "elevator"),
    (r"^wiz_hall$",     "elevator"),
    (r"^quest_room$",   "elevator"),
    (r"^well$",         "underground"),
    (r"^station$",      "underground"),
    (r"^void$",         "special"),
    (r"^prison$",       "special"),
    (r"^test$",         "special"),
    (r"^rum2$",         "special"),
]


def classify_region(room_id):
    """Map a room ID to a region key."""
    for pattern, region in REGION_RULES:
        if re.match(pattern, room_id):
            # Override: forest3-forest12 are deep_forest
            if re.match(r"^forest([3-9]|1[0-2])$", room_id):
                return "deep_forest"
            return region
    return "special"


# ---------------------------------------------------------------------------
# Room file parsing
# ---------------------------------------------------------------------------

def normalize_dest(dest_path):
    """Convert a destination path like 'room/church' to a room_id like 'church'."""
    dest = dest_path.strip().strip('"')
    if dest.startswith("/"):
        dest = dest[1:]
    if dest.startswith("room/"):
        dest = dest[5:]
    return dest


def extract_strings(text):
    """Extract all double-quoted string literals from text (ignoring escapes)."""
    return re.findall(r'"([^"]*)"', text)


def parse_macro_exits(content):
    """Parse ONE_EXIT/TWO_EXIT/THREE_EXIT/FOUR_EXIT macro calls."""
    exits = []
    macro_counts = {"ONE_EXIT": 1, "TWO_EXIT": 2, "THREE_EXIT": 3, "FOUR_EXIT": 4}

    for macro, n_exits in macro_counts.items():
        pattern = rf'{macro}\s*\('
        match = re.search(pattern, content)
        if not match:
            continue
        # Extract everything from the macro call opening paren to end of file
        start = match.end()
        # Find balanced closing paren
        depth = 1
        pos = start
        while pos < len(content) and depth > 0:
            if content[pos] == '(':
                depth += 1
            elif content[pos] == ')':
                depth -= 1
            pos += 1
        args_text = content[start:pos - 1]
        strings = extract_strings(args_text)
        # First 2*n_exits strings are (dest, dir) pairs
        for i in range(0, min(n_exits * 2, len(strings)), 2):
            if i + 1 < len(strings):
                dest = normalize_dest(strings[i])
                direction = strings[i + 1]
                exits.append((direction, dest))
        break  # Only one macro per file

    return exits


def parse_dest_dir(content):
    """Parse dest_dir = ({...}) array assignments."""
    exits = []
    pattern = r'dest_dir\s*=\s*\(\{(.*?)\}\)'
    matches = re.findall(pattern, content, re.DOTALL)
    for match_text in matches:
        strings = extract_strings(match_text)
        for i in range(0, len(strings) - 1, 2):
            dest = normalize_dest(strings[i])
            direction = strings[i + 1]
            exits.append((direction, dest))
    return exits


def parse_move_player(content):
    """Parse move_player("direction#destination") calls."""
    exits = []
    pattern = r'move_player\s*\(\s*"([^"]+)"\s*\)'
    for match in re.finditer(pattern, content):
        value = match.group(1)
        if "#" in value:
            direction, dest_path = value.split("#", 1)
            dest = normalize_dest(dest_path)
            exits.append((direction, dest))
    return exits


def parse_short_desc(content):
    """Try to extract the short description from a room file."""
    # Pattern 1: short_desc = "...";
    m = re.search(r'short_desc\s*=\s*"([^"]+)"', content)
    if m:
        return m.group(1)
    # Pattern 2: return "..." inside short() function
    m = re.search(r'string\s+short\s*\([^)]*\)\s*\{[^}]*return\s+"([^"]+)"', content, re.DOTALL)
    if m:
        return m.group(1)
    # Pattern 3: SH parameter in macro (the string after exit pairs)
    macro_counts = {"ONE_EXIT": 1, "TWO_EXIT": 2, "THREE_EXIT": 3, "FOUR_EXIT": 4}
    for macro, n_exits in macro_counts.items():
        match = re.search(rf'{macro}\s*\(', content)
        if match:
            start = match.end()
            depth = 1
            pos = start
            while pos < len(content) and depth > 0:
                if content[pos] == '(':
                    depth += 1
                elif content[pos] == ')':
                    depth -= 1
                pos += 1
            args_text = content[start:pos - 1]
            strings = extract_strings(args_text)
            sh_index = n_exits * 2  # SH comes after the exit pairs
            if sh_index < len(strings):
                return strings[sh_index]
            break
    return None


def truncate_label(label, max_chars=28):
    """Ellipsize labels over max_chars characters."""
    if len(label) <= max_chars:
        return label
    return label[:max_chars - 3] + "..."


def label_from_id(room_id):
    """Generate a human-readable label from a room ID."""
    name = room_id.split("/")[-1]
    # Special short labels for south rooms
    m = re.match(r"sforst(\d+)", name)
    if m:
        return f"SF {m.group(1)}"
    m = re.match(r"sshore(\d+)", name)
    if m:
        return f"Shore {m.group(1)}"
    m = re.match(r"sislnd(\d+)", name)
    if m:
        return f"Isle {m.group(1)}"
    # General: replace underscores, title case
    return name.replace("_", " ").title()


def discover_rooms(mudlib_path):
    """Walk mudlib/room/ and parse all .c files.

    Returns:
        rooms: dict[room_id -> (label, region)]
        edges: list[(room_id, direction, dest_room_id)]
    """
    room_dir = os.path.join(mudlib_path, "room")
    rooms = {}
    edges = []

    for dirpath, _dirnames, filenames in os.walk(room_dir):
        for fname in sorted(filenames):
            if not fname.endswith(".c"):
                continue
            # Compute room_id relative to room/
            rel = os.path.relpath(os.path.join(dirpath, fname), room_dir)
            room_id = rel[:-2]  # strip .c

            # Skip non-room files
            if fname in SKIP_FILES:
                continue

            filepath = os.path.join(dirpath, fname)
            try:
                with open(filepath, "r", encoding="latin-1") as f:
                    content = f.read()
            except OSError:
                continue

            # Parse exits using all three methods
            file_exits = []
            file_exits.extend(parse_macro_exits(content))
            file_exits.extend(parse_dest_dir(content))
            file_exits.extend(parse_move_player(content))

            # Deduplicate exits (same direction+dest)
            seen = set()
            unique_exits = []
            for direction, dest in file_exits:
                key = (direction, dest)
                if key not in seen:
                    seen.add(key)
                    unique_exits.append((direction, dest))

            # Get label
            short = parse_short_desc(content)
            label = truncate_label(short if short else label_from_id(room_id))

            # Classify region
            region = classify_region(room_id)

            rooms[room_id] = (label, region)
            for direction, dest in unique_exits:
                edges.append((room_id, direction, dest))

    return rooms, edges


# ---------------------------------------------------------------------------
# Graph construction
# ---------------------------------------------------------------------------

def build_graph(rooms, edges):
    """Build a NetworkX graph from rooms and edges.

    Each edge stores a ``directions`` list — every directed exit label
    (src→dst and dst→src) so the layout can pick the best compass bearing.
    """
    G = nx.Graph()

    for room_id, (label, region) in rooms.items():
        G.add_node(room_id, label=label, region=region)

    # Collect all directed exit labels per undirected pair
    pair_dirs = defaultdict(list)  # (src, dst) -> [(direction, src)]
    for src, direction, dst in edges:
        if src in rooms and dst in rooms:
            pair_dirs[(src, dst)].append((direction, src))
            # Also store under the canonical undirected key
            if src != dst:
                pair_dirs[(dst, src)].append((direction, src))

    added = set()
    for src, direction, dst in edges:
        if src in rooms and dst in rooms:
            key = tuple(sorted((src, dst)))
            if key not in added:
                added.add(key)
                # Merge all directed exits for this pair
                all_dirs = {}
                for d, origin in pair_dirs.get((src, dst), []):
                    all_dirs[(d, origin)] = True
                for d, origin in pair_dirs.get((dst, src), []):
                    all_dirs[(d, origin)] = True
                G.add_edge(src, dst,
                           direction=direction,
                           directions=list(all_dirs.keys()))

    return G


# ---------------------------------------------------------------------------
# Layout
# ---------------------------------------------------------------------------

# Region bounding boxes: (x, y, w, h) in pixels — layout target areas
# Sized proportionally to room count; arranged to match MUD geography
REGION_BOXES = {
    "special":      (120,   120,  900,  450),
    "mines":        (120,   700,  1200, 1300),
    "mountains":    (2800,  120,  2400, 800),
    "plains":       (5400,  120,  1650, 800),
    "giants":       (1500,  700,  1100, 700),
    "orcs":         (1500,  1500, 1100, 700),
    "forest":       (2800,  1050, 1600, 900),
    "deep_forest":  (1500,  2300, 1600, 900),
    "village":      (4550,  1050, 1800, 1000),
    "east_road":    (5600,  2150, 1450, 800),
    "sea":          (5600,  3050, 1450, 800),
    "elevator":     (3300,  2050, 1000, 500),
    "underground":  (4550,  2700, 2500, 900),
    "south_forest": (1500,  3350, 1700, 650),
    "shore":        (1500,  4100, 1700, 600),
    "island":       (1500,  4800, 1700, 450),
}

MARGIN = 120

# Direction-to-grid offsets (x, y) — y grows downward so north is -y
DIRECTION_OFFSETS = {
    "north":     ( 0, -3),
    "south":     ( 0,  3),
    "east":      ( 3,  0),
    "west":      (-3,  0),
    "northeast": ( 3, -3),
    "southeast": ( 3,  3),
    "southwest": (-3,  3),
    "northwest": (-3, -3),
}

VERTICAL_DIRECTIONS = {"up", "down"}

OPPOSITES = {
    "north": "south", "south": "north",
    "east": "west", "west": "east",
    "northeast": "southwest", "southwest": "northeast",
    "northwest": "southeast", "southeast": "northwest",
    "up": "down", "down": "up",
}


def _best_direction(src, dst, G):
    """Return the compass direction from *src* toward *dst*.

    Looks at the ``directions`` list stored on the edge and picks the first
    spatial (non-vertical) direction originating from *src*.  Falls back to the
    opposite of a direction originating from *dst*, then to ``None``.
    """
    if not G.has_edge(src, dst):
        return None
    edge_data = G.edges[src, dst]
    dirs = edge_data.get("directions", [])

    # First pass: direction where the origin is src
    for d, origin in dirs:
        if origin == src and d in DIRECTION_OFFSETS:
            return d

    # Second pass: direction where origin is dst → use opposite
    for d, origin in dirs:
        if origin == dst and d in DIRECTION_OFFSETS:
            opp = OPPOSITES.get(d)
            if opp and opp in DIRECTION_OFFSETS:
                return opp

    # Third pass: any spatial direction at all
    for d, _origin in dirs:
        if d in DIRECTION_OFFSETS:
            return d

    return None


def _label_pixel_width(label, font):
    """Estimate the pixel width of a label rendered with the given font."""
    try:
        bbox = font.getbbox(label)
        return bbox[2] - bbox[0]
    except Exception:
        return len(label) * 7


def _region_grid_layout(G, edges_raw):
    """Place rooms on per-region grids using BFS with compass offsets.

    Each region gets its own local grid where cell size is based on the
    widest label in that region.  Rooms are placed via BFS from the
    highest-degree node using compass direction offsets of +/-1.

    Returns:
        pixel_pos: dict[node -> (px, py)]
        vertical_edges: set of (u, v) tuples for purely vertical connections
    """
    # Group nodes by region
    region_nodes = defaultdict(list)
    for node in G.nodes:
        region = G.nodes[node].get("region", "special")
        region_nodes[region].append(node)

    # Local direction offsets (unit grid steps)
    local_offsets = {
        "north":     ( 0, -1),
        "south":     ( 0,  1),
        "east":      ( 1,  0),
        "west":      (-1,  0),
        "northeast": ( 1, -1),
        "southeast": ( 1,  1),
        "southwest": (-1,  1),
        "northwest": (-1, -1),
    }

    pixel_pos = {}

    for region, nodes in region_nodes.items():
        box = REGION_BOXES.get(region, (3600, 2700, 1000, 500))
        bx, by, bw, bh = box

        # Compute cell size based on widest label + padding
        max_lw = 0
        for n in nodes:
            lbl = G.nodes[n].get("label", n)
            lw = _label_pixel_width(lbl, FONT_ROOM)
            if lw > max_lw:
                max_lw = lw
        cell_w = max_lw + 24
        cell_h = 28

        # Build region subgraph
        sub = G.subgraph(nodes)

        # BFS seed: highest-degree node in this region
        seed = max(nodes, key=lambda n: sub.degree(n))

        # BFS placement on local grid
        local_grid = {}  # node -> (col, row)
        occupied = set()  # occupied (col, row) cells

        local_grid[seed] = (0, 0)
        occupied.add((0, 0))
        queue = deque([seed])

        while queue:
            current = queue.popleft()
            cc, cr = local_grid[current]

            for neighbor in sub.neighbors(current):
                if neighbor in local_grid:
                    continue
                direction = _best_direction(current, neighbor, G)
                if direction and direction in local_offsets:
                    dc, dr = local_offsets[direction]
                    target = (cc + dc, cr + dr)
                else:
                    # No spatial direction — try placing nearby
                    target = (cc + 1, cr)

                # Spiral search if occupied
                if target in occupied:
                    target = _local_spiral(target, occupied,
                                           prefer_dx=target[0] - cc,
                                           prefer_dy=target[1] - cr)
                local_grid[neighbor] = target
                occupied.add(target)
                queue.append(neighbor)

        # Place any disconnected nodes in this region
        for n in nodes:
            if n not in local_grid:
                target = _local_spiral((0, 0), occupied)
                local_grid[n] = target
                occupied.add(target)

        if not local_grid:
            continue

        # Convert local grid to pixel coords centered in region box
        cols = [c for c, r in local_grid.values()]
        rows = [r for c, r in local_grid.values()]
        min_c, max_c = min(cols), max(cols)
        min_r, max_r = min(rows), max(rows)
        grid_w = (max_c - min_c) * cell_w
        grid_h = (max_r - min_r) * cell_h

        # Scale down if grid exceeds box
        scale = 1.0
        if grid_w > 0 and grid_w > bw - 40:
            scale = min(scale, (bw - 40) / grid_w)
        if grid_h > 0 and grid_h > bh - 40:
            scale = min(scale, (bh - 40) / grid_h)

        eff_cell_w = cell_w * scale
        eff_cell_h = cell_h * scale

        # Center grid in box
        used_w = (max_c - min_c) * eff_cell_w
        used_h = (max_r - min_r) * eff_cell_h
        origin_x = bx + (bw - used_w) / 2
        origin_y = by + (bh - used_h) / 2

        for node, (c, r) in local_grid.items():
            px = int(origin_x + (c - min_c) * eff_cell_w)
            py = int(origin_y + (r - min_r) * eff_cell_h)
            pixel_pos[node] = (px, py)

    # Track vertical-only edges
    vertical_edges = set()
    for u, v in G.edges:
        all_dirs = []
        for d, _origin in G.edges[u, v].get("directions", []):
            all_dirs.append(d)
        spatial = [d for d in all_dirs if d in DIRECTION_OFFSETS]
        vertical = [d for d in all_dirs if d in VERTICAL_DIRECTIONS]
        if vertical and not spatial:
            vertical_edges.add((u, v))

    return pixel_pos, vertical_edges


def _local_spiral(target, occupied, prefer_dx=0, prefer_dy=0, max_radius=60):
    """Find nearest empty cell on a local grid with clearance=1."""
    if target not in occupied:
        return target
    tx, ty = target
    for radius in range(1, max_radius):
        best = None
        best_score = float("inf")
        for dx in range(-radius, radius + 1):
            for dy in range(-radius, radius + 1):
                if abs(dx) != radius and abs(dy) != radius:
                    continue
                cell = (tx + dx, ty + dy)
                if cell in occupied:
                    continue
                score = abs(dx) + abs(dy)
                if prefer_dx and dx * prefer_dx > 0:
                    score -= 0.5
                if prefer_dy and dy * prefer_dy > 0:
                    score -= 0.5
                if score < best_score:
                    best_score = score
                    best = cell
        if best is not None:
            return best
    return (tx + max_radius, ty)


def _resolve_overlaps(pixel_pos, G):
    """Nudge overlapping rooms apart based on actual label pixel widths."""
    nodes = list(pixel_pos.keys())
    if len(nodes) <= 1:
        return pixel_pos

    # Precompute half-widths and half-heights for each node
    pad_x, pad_y = 6, 3
    half_sizes = {}
    for n in nodes:
        lbl = G.nodes[n].get("label", n)
        lw = _label_pixel_width(lbl, FONT_ROOM)
        hw = lw / 2 + pad_x + 4  # 4px extra margin
        hh = 10 + pad_y + 2
        half_sizes[n] = (hw, hh)

    pos = dict(pixel_pos)
    for iteration in range(50):
        moved = False
        for i in range(len(nodes)):
            for j in range(i + 1, len(nodes)):
                a, b = nodes[i], nodes[j]
                ax, ay = pos[a]
                bx, by = pos[b]
                hw_a, hh_a = half_sizes[a]
                hw_b, hh_b = half_sizes[b]

                # Check overlap
                overlap_x = (hw_a + hw_b) - abs(ax - bx)
                overlap_y = (hh_a + hh_b) - abs(ay - by)
                if overlap_x > 0 and overlap_y > 0:
                    # Nudge along axis of least overlap
                    if overlap_x < overlap_y:
                        nudge = (overlap_x / 2) + 1
                        if ax <= bx:
                            pos[a] = (ax - nudge, ay)
                            pos[b] = (bx + nudge, by)
                        else:
                            pos[a] = (ax + nudge, ay)
                            pos[b] = (bx - nudge, by)
                    else:
                        nudge = (overlap_y / 2) + 1
                        if ay <= by:
                            pos[a] = (ax, ay - nudge)
                            pos[b] = (bx, by + nudge)
                        else:
                            pos[a] = (ax, ay + nudge)
                            pos[b] = (bx, by - nudge)
                    moved = True
        if not moved:
            break

    # Convert back to int
    return {n: (int(x), int(y)) for n, (x, y) in pos.items()}


def compute_layout(G, edges_raw):
    """Compute room positions using per-region grid layout with overlap resolution."""
    if len(G.nodes) == 0:
        return {}, set()

    pixel_pos, vertical_edges = _region_grid_layout(G, edges_raw)
    pixel_pos = _resolve_overlaps(pixel_pos, G)

    return pixel_pos, vertical_edges


# ---------------------------------------------------------------------------
# Font helpers
# ---------------------------------------------------------------------------

def load_font(size):
    """Try to load a good monospace/sans font, fall back to default."""
    candidates = [
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/SFNSMono.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    ]
    for path in candidates:
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size)
            except Exception:
                continue
    return ImageFont.load_default()


FONT_ROOM = load_font(11)
FONT_REGION = load_font(18)
FONT_TITLE = load_font(28)
FONT_LEGEND = load_font(12)
FONT_SMALL = load_font(8)


# ---------------------------------------------------------------------------
# Drawing helpers
# ---------------------------------------------------------------------------

def text_size(draw, text, font):
    """Get text width and height."""
    bbox = draw.textbbox((0, 0), text, font=font)
    return bbox[2] - bbox[0], bbox[3] - bbox[1]


def draw_room_box(draw, x, y, label, region):
    """Draw a single room as a colored rounded rectangle with label."""
    fill, border, text_color = PALETTE.get(region, PALETTE["special"])
    tw, th = text_size(draw, label, FONT_ROOM)
    pad_x, pad_y = 6, 3
    w = tw + pad_x * 2
    h = th + pad_y * 2
    x0 = x - w // 2
    y0 = y - h // 2
    x1 = x + w // 2
    y1 = y + h // 2

    # Shadow
    draw.rounded_rectangle([x0 + 2, y0 + 2, x1 + 2, y1 + 2], radius=4,
                           fill=(0, 0, 0, 80))
    # Box
    draw.rounded_rectangle([x0, y0, x1, y1], radius=4,
                           fill=fill, outline=border, width=1)
    # Label
    draw.text((x - tw // 2, y - th // 2), label, fill=text_color, font=FONT_ROOM)


def _route_orthogonal(x1, y1, x2, y2, direction):
    """Return a list of (x, y) waypoints that route a connection orthogonally.

    For cardinal directions (N/S/E/W), the line goes along the primary axis
    first, then turns 90 degrees to reach the destination.
    For diagonal directions (NE/NW/SE/SW), a 45-degree line is appropriate
    so we return a direct path.
    For unknown directions, route horizontally then vertically.
    """
    if direction in ("north", "south"):
        # Go vertical first, then horizontal
        return [(x1, y1), (x1, y2), (x2, y2)]
    elif direction in ("east", "west"):
        # Go horizontal first, then vertical
        return [(x1, y1), (x2, y1), (x2, y2)]
    elif direction in ("northeast", "northwest", "southeast", "southwest"):
        # Diagonal is appropriate — straight line
        return [(x1, y1), (x2, y2)]
    else:
        # Unknown — route horizontal then vertical
        return [(x1, y1), (x2, y1), (x2, y2)]


def draw_connection(draw, pos, id1, id2, G, color=(140, 140, 160, 200), width=2):
    """Draw an orthogonally-routed line between two rooms."""
    if id1 not in pos or id2 not in pos:
        return
    x1, y1 = pos[id1]
    x2, y2 = pos[id2]
    direction = _best_direction(id1, id2, G)
    waypoints = _route_orthogonal(x1, y1, x2, y2, direction)
    for i in range(len(waypoints) - 1):
        draw.line([waypoints[i], waypoints[i + 1]], fill=color, width=width)


def draw_dashed_line(draw, x1, y1, x2, y2, direction=None,
                     color=(150, 150, 170, 120), width=2, steps=30):
    """Draw a dashed line between two points, orthogonally routed."""
    waypoints = _route_orthogonal(x1, y1, x2, y2, direction)
    # Dash along the full path
    for seg in range(len(waypoints) - 1):
        sx, sy = waypoints[seg]
        ex, ey = waypoints[seg + 1]
        for i in range(0, steps, 2):
            t1 = i / steps
            t2 = min((i + 1) / steps, 1.0)
            px1 = int(sx + (ex - sx) * t1)
            py1 = int(sy + (ey - sy) * t1)
            px2 = int(sx + (ex - sx) * t2)
            py2 = int(sy + (ey - sy) * t2)
            draw.line([(px1, py1), (px2, py2)], fill=color, width=width)


def draw_vertical_connection(draw, pos, id1, id2,
                             color=(160, 100, 200, 200), width=2):
    """Draw a dotted purple line for up/down connections with a UD indicator."""
    if id1 not in pos or id2 not in pos:
        return
    x1, y1 = pos[id1]
    x2, y2 = pos[id2]
    draw_dashed_line(draw, x1, y1, x2, y2, direction=None,
                     color=color, width=width, steps=20)
    # Draw UD indicator at midpoint
    mx = (x1 + x2) // 2
    my = (y1 + y2) // 2
    draw.text((mx - 3, my - 6), "UD", fill=color, font=FONT_SMALL)


# ---------------------------------------------------------------------------
# Region label positions (computed from room positions)
# ---------------------------------------------------------------------------

REGION_DISPLAY_NAMES = {
    "mountains":    "MOUNTAINS",
    "mines":        "MINES",
    "plains":       "THE GREAT PLAINS",
    "giants":       "GIANT TERRITORY",
    "orcs":         "ORC VALLEY",
    "forest":       "FOREST",
    "deep_forest":  "DEEP FOREST",
    "village":      "VILLAGE",
    "east_road":    "EAST ROAD",
    "sea":          "THE SEA",
    "underground":  "UNDERGROUND",
    "south_forest": "DIMLY LIT FOREST",
    "shore":        "CRESCENT LAKE",
    "island":       "ISLE OF THE MAGI",
    "special":      "SPECIAL ROOMS",
    "elevator":     "ELEVATOR",
}


def compute_region_label_positions(G, pos):
    """Compute a label position for each region (above its bounding box)."""
    region_positions = defaultdict(list)
    for node in G.nodes:
        if node in pos:
            region = G.nodes[node].get("region", "special")
            region_positions[region].append(pos[node])

    labels = []
    for region, positions in region_positions.items():
        if not positions:
            continue
        xs = [p[0] for p in positions]
        ys = [p[1] for p in positions]
        # Place label above the top-left of the region bbox
        lx = min(xs) - 10
        ly = min(ys) - 30
        name = REGION_DISPLAY_NAMES.get(region, region.upper())
        labels.append((name, lx, ly, region))

    return labels


# ---------------------------------------------------------------------------
# Identify long-distance connections for dashed lines
# ---------------------------------------------------------------------------

def find_long_connections(G, pos, threshold=500):
    """Find edges that span a long pixel distance (cross-region links)."""
    long_edges = set()
    for u, v in G.edges:
        if u in pos and v in pos:
            x1, y1 = pos[u]
            x2, y2 = pos[v]
            dist = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
            if dist > threshold:
                long_edges.add((u, v))
    return long_edges


# ---------------------------------------------------------------------------
# Main rendering
# ---------------------------------------------------------------------------

def render(G, pos, out_dir, vertical_edges=None):
    """Render the world map PNG."""
    if vertical_edges is None:
        vertical_edges = set()
    img = Image.new("RGBA", (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(img, "RGBA")

    # Region boundary boxes (semi-transparent rounded rectangles)
    for region, (bx, by, bw, bh) in REGION_BOXES.items():
        fill = PALETTE.get(region, PALETTE["special"])[0]
        bg_color = (fill[0], fill[1], fill[2], 18)
        border_color = (fill[0], fill[1], fill[2], 50)
        draw.rounded_rectangle([bx, by, bx + bw, by + bh], radius=12,
                               fill=bg_color, outline=border_color, width=1)

    # Region labels
    region_labels = compute_region_label_positions(G, pos)
    for label, lx, ly, rkey in region_labels:
        fill = PALETTE.get(rkey, PALETTE["special"])[0]
        color = (fill[0], fill[1], fill[2], 140)
        draw.text((max(5, lx), max(5, ly)), label, fill=color, font=FONT_REGION)

    # Draw lake water if shore rooms exist
    shore_positions = [pos[n] for n in G.nodes
                       if G.nodes[n].get("region") == "shore" and n in pos]
    if shore_positions:
        lake_cx = sum(p[0] for p in shore_positions) // len(shore_positions)
        lake_cy = sum(p[1] for p in shore_positions) // len(shore_positions)
        max_r = max(
            math.sqrt((p[0] - lake_cx) ** 2 + (p[1] - lake_cy) ** 2)
            for p in shore_positions
        )
        lake_r = int(max_r * 0.85)
        if lake_r > 20:
            lake_layer = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
            lake_draw = ImageDraw.Draw(lake_layer, "RGBA")
            lake_draw.ellipse(
                [lake_cx - lake_r, lake_cy - lake_r,
                 lake_cx + lake_r, lake_cy + lake_r],
                fill=(20, 45, 100, 25),
                outline=(40, 70, 130, 50), width=1,
            )
            img = Image.alpha_composite(img, lake_layer)
            draw = ImageDraw.Draw(img, "RGBA")

    # Connection lines (behind rooms)
    long_edges = find_long_connections(G, pos)
    for u, v in G.edges:
        if u in pos and v in pos:
            if (u, v) in vertical_edges or (v, u) in vertical_edges:
                draw_vertical_connection(draw, pos, u, v)
            elif (u, v) in long_edges or (v, u) in long_edges:
                direction = _best_direction(u, v, G)
                draw_dashed_line(draw, pos[u][0], pos[u][1],
                                 pos[v][0], pos[v][1],
                                 direction=direction)
            else:
                draw_connection(draw, pos, u, v, G)

    # Room boxes
    for node in G.nodes:
        if node in pos:
            x, y = pos[node]
            label = G.nodes[node].get("label", node)
            region = G.nodes[node].get("region", "special")
            draw_room_box(draw, x, y, label, region)

    # Title
    title = "LPmud 2.4.5 World Map"
    tw, th = text_size(draw, title, FONT_TITLE)
    draw.text((WIDTH // 2 - tw // 2, 15), title,
              fill=(220, 210, 180), font=FONT_TITLE)
    subtitle = f"~{len(G.nodes)} rooms across {len(set(nx.get_node_attributes(G, 'region').values()))} regions (auto-discovered)"
    sw, sh = text_size(draw, subtitle, FONT_LEGEND)
    draw.text((WIDTH // 2 - sw // 2, 48), subtitle,
              fill=(160, 155, 140), font=FONT_LEGEND)

    # Compass Rose
    cx, cy = WIDTH - 100, 100
    r = 30
    draw.line([(cx, cy - r), (cx, cy + r)], fill=(180, 180, 190), width=2)
    draw.line([(cx - r, cy), (cx + r, cy)], fill=(180, 180, 190), width=2)
    for lbl, dx, dy in [("N", 0, -r - 12), ("S", 0, r + 4),
                         ("W", -r - 14, -4), ("E", r + 4, -4)]:
        draw.text((cx + dx, cy + dy), lbl,
                  fill=(200, 200, 210), font=FONT_LEGEND)

    # Legend
    legend_x, legend_y = WIDTH - 220, 200
    draw.text((legend_x, legend_y - 25), "REGIONS",
              fill=(200, 195, 180), font=FONT_LEGEND)
    items = [
        ("Village",        "village"),
        ("Forest",         "forest"),
        ("Deep Forest",    "deep_forest"),
        ("Plains",         "plains"),
        ("Mountains",      "mountains"),
        ("Mines",          "mines"),
        ("Giants",         "giants"),
        ("Orcs",           "orcs"),
        ("East Road",      "east_road"),
        ("Sea",            "sea"),
        ("Underground",    "underground"),
        ("South Forest",   "south_forest"),
        ("Lake Shore",     "shore"),
        ("Isle of Magi",   "island"),
        ("Elevator",       "elevator"),
        ("Special",        "special"),
    ]
    for i, (name, key) in enumerate(items):
        y = legend_y + i * 22
        fill_c, border_c, _ = PALETTE[key]
        draw.rounded_rectangle([legend_x, y, legend_x + 16, y + 14],
                               radius=2, fill=fill_c, outline=border_c)
        draw.text((legend_x + 22, y + 1), name,
                  fill=(190, 185, 170), font=FONT_ROOM)

    # Notes
    notes = [
        "Solid line = direct connection",
        "Dashed line = distant connection",
        "Dotted purple = up/down connection",
        "Rooms and connections auto-discovered from mudlib",
    ]
    ny = legend_y + len(items) * 22 + 15
    for note in notes:
        draw.text((legend_x, ny), note,
                  fill=(140, 135, 120), font=FONT_SMALL)
        ny += 13

    # Save
    output = img.convert("RGB")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world-map.png")
    output.save(out_path, "PNG")
    print(f"Map saved to {os.path.abspath(out_path)}")
    print(f"  Canvas: {WIDTH}x{HEIGHT}")
    print(f"  Rooms: {len(G.nodes)}")
    print(f"  Connections: {len(G.edges)}")
    return out_path


# ---------------------------------------------------------------------------
# Text file generation
# ---------------------------------------------------------------------------

def generate_connectivity_txt(G, edges_raw, out_dir):
    """Generate room-connectivity.txt from the graph."""
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "room-connectivity.txt")

    # Group edges by source room
    exits_by_room = defaultdict(list)
    for src, direction, dst in edges_raw:
        if src in G.nodes and dst in G.nodes:
            exits_by_room[src].append((direction, dst))

    # Group rooms by region
    rooms_by_region = defaultdict(list)
    for node in sorted(G.nodes):
        region = G.nodes[node].get("region", "special")
        rooms_by_region[region].append(node)

    region_order = [
        ("special",      "SPECIAL ROOMS"),
        ("village",      "VILLAGE"),
        ("elevator",     "ELEVATOR SYSTEM"),
        ("east_road",    "EAST ROAD"),
        ("sea",          "JETTY & SEA"),
        ("forest",       "FOREST"),
        ("deep_forest",  "DEEP FOREST"),
        ("plains",       "PLAINS"),
        ("mountains",    "MOUNTAINS"),
        ("giants",       "GIANTS"),
        ("orcs",         "ORC VALLEY"),
        ("underground",  "UNDERGROUND"),
        ("mines",        "MINES"),
        ("south_forest", "SOUTH FOREST"),
        ("shore",        "CRESCENT LAKE SHORE"),
        ("island",       "ISLE OF THE MAGI"),
    ]

    lines = []
    sep = "=" * 78
    lines.append(sep)
    lines.append("LPMud 2.4.5 COMPLETE ROOM CONNECTIVITY MAP")
    lines.append(sep)
    lines.append("")
    lines.append("Auto-generated by scripts/generate-map.py from mudlib room files.")
    lines.append("Format: room_id: \"Short Description\" -> direction:destination, ...")
    lines.append("")

    for region_key, region_title in region_order:
        if region_key not in rooms_by_region:
            continue
        nodes = rooms_by_region[region_key]
        lines.append(sep)
        lines.append(region_title)
        lines.append(sep)
        lines.append("")
        for node in sorted(nodes):
            label = G.nodes[node].get("label", node)
            room_exits = exits_by_room.get(node, [])
            if room_exits:
                exit_str = ", ".join(f"{d}:room/{dst}" for d, dst in room_exits)
            else:
                exit_str = "(no exits)"
            lines.append(f'room/{node}.c: "{label}" -> {exit_str}')
        lines.append("")

    # Summary
    lines.append(sep)
    lines.append("ROOM COUNT SUMMARY")
    lines.append(sep)
    lines.append("")
    for region_key, region_title in region_order:
        if region_key in rooms_by_region:
            count = len(rooms_by_region[region_key])
            lines.append(f"  {region_title}: {count} rooms")
    lines.append(f"\n  Total: {len(G.nodes)} rooms")
    lines.append("")

    with open(out_path, "w") as f:
        f.write("\n".join(lines))
    print(f"Connectivity saved to {os.path.abspath(out_path)}")


def generate_world_map_txt(G, pos, out_dir):
    """Generate a simplified world-map.txt with region layout info."""
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world-map.txt")

    sep = "=" * 79

    lines = []
    lines.append(sep)
    lines.append(f"{'LPmud 2.4.5 World Map':^79}")
    lines.append(f"{'~' + str(len(G.nodes)) + ' rooms (auto-discovered)':^79}")
    lines.append(sep)
    lines.append("")
    lines.append("This file is auto-generated by scripts/generate-map.py.")
    lines.append("See world-map.png for the graphical version.")
    lines.append("")

    # List regions with their rooms
    rooms_by_region = defaultdict(list)
    for node in sorted(G.nodes):
        region = G.nodes[node].get("region", "special")
        rooms_by_region[region].append(node)

    region_order = [
        ("special",      "SPECIAL ROOMS"),
        ("mountains",    "MOUNTAINS"),
        ("mines",        "MINES"),
        ("plains",       "THE GREAT PLAINS"),
        ("giants",       "GIANT TERRITORY"),
        ("orcs",         "ORC VALLEY"),
        ("forest",       "FOREST"),
        ("deep_forest",  "DEEP FOREST"),
        ("village",      "VILLAGE"),
        ("elevator",     "ELEVATOR SYSTEM"),
        ("east_road",    "EAST ROAD"),
        ("sea",          "JETTY & SEA"),
        ("underground",  "UNDERGROUND"),
        ("south_forest", "DIMLY LIT FOREST"),
        ("shore",        "CRESCENT LAKE SHORE"),
        ("island",       "ISLE OF THE MAGI"),
    ]

    for region_key, region_title in region_order:
        if region_key not in rooms_by_region:
            continue
        nodes = rooms_by_region[region_key]
        lines.append(f"=== {region_title} ({len(nodes)} rooms) ===")
        lines.append("")
        for node in sorted(nodes):
            label = G.nodes[node].get("label", node)
            neighbors = sorted(G.neighbors(node))
            neighbor_str = ", ".join(neighbors) if neighbors else "(no exits)"
            lines.append(f"  {node}: \"{label}\" -- {neighbor_str}")
        lines.append("")

    lines.append(sep)
    lines.append(f"  Total rooms: {len(G.nodes)}")
    lines.append(f"  Total connections: {len(G.edges)}")
    lines.append(sep)

    with open(out_path, "w") as f:
        f.write("\n".join(lines))
    print(f"Text map saved to {os.path.abspath(out_path)}")


# ---------------------------------------------------------------------------
# JSON export for interactive map viewer
# ---------------------------------------------------------------------------

def generate_json(G, pos, edges_raw, vertical_edges, out_dir):
    """Generate world-map.json for the interactive Leaflet map viewer."""
    os.makedirs(out_dir, exist_ok=True)

    # Build exits dict per room from directed edges
    exits_by_room = defaultdict(dict)
    for src, direction, dst in edges_raw:
        if src in G.nodes and dst in G.nodes:
            exits_by_room[src][direction] = dst

    # Regions
    regions = {}
    for region_key, (bx, by, bw, bh) in REGION_BOXES.items():
        fill, border, _ = PALETTE.get(region_key, PALETTE["special"])
        _, _, text = PALETTE.get(region_key, PALETTE["special"])
        regions[region_key] = {
            "displayName": REGION_DISPLAY_NAMES.get(region_key, region_key.upper()),
            "box": [bx, by, bw, bh],
            "fill": list(fill),
            "border": list(border),
            "text": list(text),
        }

    # Rooms
    rooms = {}
    for node in G.nodes:
        if node not in pos:
            continue
        x, y = pos[node]
        label = G.nodes[node].get("label", node)
        region = G.nodes[node].get("region", "special")
        lw = _label_pixel_width(label, FONT_ROOM)
        rooms[node] = {
            "label": label,
            "region": region,
            "x": x,
            "y": y,
            "labelWidth": lw,
            "file": "room/" + node + ".c",
            "exits": exits_by_room.get(node, {}),
        }

    # Edges with waypoints
    long_edges = find_long_connections(G, pos)
    edges_out = []
    for u, v in G.edges:
        if u not in pos or v not in pos:
            continue
        x1, y1 = pos[u]
        x2, y2 = pos[v]
        if (u, v) in vertical_edges or (v, u) in vertical_edges:
            edge_type = "vertical"
            waypoints = _route_orthogonal(x1, y1, x2, y2, None)
        elif (u, v) in long_edges or (v, u) in long_edges:
            edge_type = "long"
            direction = _best_direction(u, v, G)
            waypoints = _route_orthogonal(x1, y1, x2, y2, direction)
        else:
            edge_type = "normal"
            direction = _best_direction(u, v, G)
            waypoints = _route_orthogonal(x1, y1, x2, y2, direction)
        edges_out.append({
            "from": u,
            "to": v,
            "type": edge_type,
            "waypoints": waypoints,
        })

    data = {
        "canvas": {"width": WIDTH, "height": HEIGHT},
        "regions": regions,
        "rooms": rooms,
        "edges": edges_out,
    }

    out_path = os.path.join(out_dir, "world-map.json")
    with open(out_path, "w") as f:
        json.dump(data, f)
    print(f"JSON data saved to {os.path.abspath(out_path)}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="Generate LPmud world map")
    default_mudlib = os.path.join(os.path.dirname(__file__), "..", "mudlib")
    default_output = os.path.join(os.path.dirname(__file__), "..", "docs")
    parser.add_argument("--mudlib", default=default_mudlib,
                        help="Path to mudlib directory")
    parser.add_argument("--output", default=default_output,
                        help="Output directory for generated files")
    args = parser.parse_args()

    mudlib_path = os.path.abspath(args.mudlib)
    out_dir = os.path.abspath(args.output)

    print(f"Scanning rooms in {mudlib_path}/room/ ...")
    rooms, edges = discover_rooms(mudlib_path)
    print(f"  Found {len(rooms)} rooms, {len(edges)} directed exits")

    print("Building graph ...")
    G = build_graph(rooms, edges)
    print(f"  Graph: {len(G.nodes)} nodes, {len(G.edges)} edges")

    print("Computing layout ...")
    pos, vertical_edges = compute_layout(G, edges)

    print("Rendering map ...")
    render(G, pos, out_dir, vertical_edges=vertical_edges)

    print("Generating text files ...")
    generate_connectivity_txt(G, edges, out_dir)
    generate_world_map_txt(G, pos, out_dir)

    print("Generating JSON data ...")
    generate_json(G, pos, edges, vertical_edges, out_dir)

    print("Done!")


if __name__ == "__main__":
    main()
