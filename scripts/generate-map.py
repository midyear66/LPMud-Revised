#!/usr/bin/env python3
"""Generate a graphical PNG world map for LPmud 2.4.5.

Reads the known room layout and draws all ~200 rooms as colored boxes
with connection lines, organized by region on a single large PNG.
"""

from PIL import Image, ImageDraw, ImageFont
import math
import os

# ---------------------------------------------------------------------------
# Canvas
# ---------------------------------------------------------------------------
WIDTH = 4600
HEIGHT = 3450
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


FONT_ROOM = load_font(10)
FONT_REGION = load_font(18)
FONT_TITLE = load_font(28)
FONT_LEGEND = load_font(12)
FONT_SMALL = load_font(8)

# ---------------------------------------------------------------------------
# Room definitions: room_id -> (x, y, label, region)
# Coordinates are pixel positions for the center of each room box.
# ---------------------------------------------------------------------------
rooms = {}


def add_rooms(base_x, base_y, defs):
    """Add rooms with positions relative to (base_x, base_y)."""
    for room_id, dx, dy, label, region in defs:
        rooms[room_id] = (base_x + dx, base_y + dy, label, region)


# ===== MOUNTAINS =====
add_rooms(2700, 200, [
    ("mount_top",  -100, -80, "Mountain Top",  "mountains"),
    ("mount_top2",  100, -80, "Plateau",       "mountains"),
    ("ravine",        0,   0, "Ravine",        "mountains"),
    ("mount_pass",    0,  90, "Mountain Pass",  "mountains"),
])

# ===== PLAINS =====
# Plains are north of the forest clearing, south of mount_pass.
# Complex grid — positioned carefully based on connectivity.
add_rooms(2700, 550, [
    ("plane11",    0,   0,  "Plain 11",  "plains"),
    ("plane13",  200,   0,  "Plain 13",  "plains"),
    ("plane12", -200,  80,  "Plain 12",  "plains"),
    ("plane6",     0,  80,  "Plain 6",   "plains"),
    ("plane8",   200,  80,  "Plain 8",   "plains"),
    ("deep_forest1", -400, 80, "Deep Forest", "deep_forest"),
    ("plane10", -200, 170,  "Plain 10",  "plains"),
    ("plane7",  -200, 260,  "Plain 7",   "plains"),
    ("plane3",     0, 170,  "Plain 3",   "plains"),
    ("ruin",     200, 170,  "Ruin",      "plains"),
    ("plane9",   380, 170,  "Plain 9",   "plains"),
    ("plane2",     0, 260,  "Plain 2",   "plains"),
    ("plane4",   200, 260,  "Plain 4",   "plains"),
    ("plane5",  -200, 350,  "Plain 5",   "plains"),
    ("plane1",     0, 350,  "Plain 1",   "plains"),
])

# ===== GIANTS =====
add_rooms(1650, 810, [
    ("big_tree",     0,   0, "Big Tree",       "giants"),
    ("giant_path", -180,   0, "Giant Path",    "giants"),
    ("giant_lair", -360,   0, "Giant Lair",    "giants"),
    ("giant_conf", -540,   0, "Giants' Conf",  "giants"),
])

# ===== ORC VALLEY =====
add_rooms(1500, 1000, [
    ("orc_treasure",  0, -80, "Orc Treasury",  "orcs"),
    ("fortress",      0,   0, "Orc Fortress",  "orcs"),
    ("orc_vall",      0,  80, "Orc Valley",    "orcs"),
])

# ===== FOREST (main corridor) =====
add_rooms(2000, 1100, [
    ("slope",      -500,   0, "Slope",       "forest"),
    ("forest2",    -330,   0, "Forest",      "forest"),
    ("clearing",   -160,   0, "Clearing",    "forest"),
    ("forest1",      10,   0, "Forest",      "forest"),
    ("wild1",       180,   0, "Wilderness",  "forest"),
    ("hump",        370,   0, "Bridge",      "forest"),
])

# ===== DEEP FOREST (south of slope) =====
add_rooms(1500, 1200, [
    ("forest3",     0,    0, "Forest 3",  "deep_forest"),
    ("forest4",     0,   80, "Forest 4",  "deep_forest"),
    ("forest5",  -150,  160, "Forest 5",  "deep_forest"),
    ("forest6",   150,   80, "Forest 6",  "deep_forest"),
    ("forest7",   150,  160, "Forest 7",  "deep_forest"),
    ("forest8",  -150,  250, "Forest 8",  "deep_forest"),
    ("forest9",     0,  250, "Forest 9",  "deep_forest"),
    ("forest10",  150,  250, "Forest 10", "deep_forest"),
    ("forest11",    0,  340, "Forest 11", "deep_forest"),
    ("forest12",  150,  340, "Forest 12", "deep_forest"),
])

# ===== VILLAGE CORE =====
add_rooms(2750, 1200, [
    ("church",       0, -100, "Church",        "village"),
    ("vill_green",   0,    0, "Village Green",  "village"),
    ("vill_track", 150,    0, "Village Track",  "village"),
    ("vill_road1", 300,    0, "Village Road",   "village"),
    ("vill_road2", 470,    0, "Village Road 2", "village"),
    ("vill_shore", 640,    0, "Road",           "village"),
])

# ===== VILLAGE BUILDINGS =====
add_rooms(2750, 1200, [
    ("yard",        300, -80, "Yard",           "village"),
    ("pub2",        430, -80, "Pub",            "village"),
    ("narr_alley",  300,  80, "Narrow Alley",   "village"),
    ("bank",        430,  80, "Bank",           "village"),
    ("bankroom",    560,  80, "Back Room",      "village"),
    ("post",        300, 160, "Post Office",    "village"),
    ("shop",        470, -80, "Shop",           "village"),
    ("storage",     590, -80, "Storage",        "village"),
    ("adv_guild",   470,  80, "Adv Guild",      "village"),
    ("adv_inner",   470, 160, "Adv Inner",      "village"),
    ("adv_inner2",  470, 230, "LPC Board",      "village"),
    ("station",     470, 160, "Station",        "underground"),  # overlap; move
    ("crop",        640,  80, "Fields",         "village"),
])
# Fix station position to avoid overlap with adv_inner
rooms["station"] = (3220, 1440, "Station", "underground")

# ===== JETTY & SEA =====
add_rooms(3600, 1200, [
    ("jetty",         0,   0, "Jetty Road",     "sea"),
    ("vill_shore2", 140,   0, "Shore",          "sea"),
    ("jetty2",      280,   0, "Jetty",          "sea"),
    ("sea",         420,   0, "At Sea",         "sea"),
    ("sea_bottom",  420,  80, "Sea Bottom",     "sea"),
])

# ===== EAST ROAD =====
add_rooms(3550, 900, [
    ("inn",        -140,   0, "Inn",         "east_road"),
    ("eastroad5",     0,   0, "East Rd 5",  "east_road"),
    ("eastroad4",     0,  70, "East Rd 4",  "east_road"),
    ("eastroad3",     0, 140, "East Rd 3",  "east_road"),
    ("eastroad2",     0, 210, "East Rd 2",  "east_road"),
    ("eastroad1",     0, 280, "East Rd 1",  "east_road"),
    ("sunalley1",  -150, 140, "Sun Alley 1", "east_road"),
    ("sunalley2",  -300, 140, "Sun Alley 2", "east_road"),
])

# ===== ELEVATOR SYSTEM =====
add_rooms(2500, 1100, [
    ("elevator",     0,   0, "Elevator",    "elevator"),
    ("attic",        0, -70, "Attic",       "elevator"),
    ("wiz_hall",     0,  70, "Wizards Hall", "elevator"),
    ("quest_room", 150,  70, "Quest Room",  "elevator"),
])

# ===== UNDERGROUND: WELL & MAZE =====
add_rooms(2900, 1550, [
    ("well",          0,   0, "Well",          "underground"),
    ("maze1",       -10,  80, "Maze 1",        "underground"),
    ("maze2",       -10, 150, "Maze 2",        "underground"),
    ("maze3",       -10, 220, "Maze 3",        "underground"),
    ("maze4",       -10, 290, "Maze 4",        "underground"),
    ("maze5",       -10, 360, "Maze 5 (End)",  "underground"),
    ("door_trap",  -180,  80, "Trap Room",     "underground"),
    ("after_trap", -330,  80, "Black Room",    "underground"),
])

# ===== SPECIAL ROOMS =====
add_rooms(400, 200, [
    ("void",     0,   0, "The Void",    "special"),
    ("prison",   0,  60, "Prison",      "special"),
    ("death_room", 0, 120, "Death Room", "special"),
])

# ===== MINES (inset area, top-left) =====
# Main corridor: tunnel through tunnel_room (vertical)
MX, MY = 650, 350
add_rooms(MX, MY, [
    ("mine/tunnel",       0,    0, "Entrance",    "mines"),
    ("mine/tunnel2",      0,   60, "Tunnel 2",    "mines"),
    ("mine/tunnel3",      0,  120, "Tunnel 3",    "mines"),
    ("mine/tunnel4",      0,  180, "Tunnel 4",    "mines"),
    ("mine/tunnel5",      0,  240, "Tunnel 5",    "mines"),
    ("mine/tunnel_room",  0,  300, "Stone Table",  "mines"),
])

# Shaft branch from tunnel3
add_rooms(MX, MY, [
    ("mine/tunnel8",    120, 160, "Shaft",        "mines"),
    ("mine/tunnel9",    120, 230, "Junction",     "mines"),
])

# West branch from tunnel9
add_rooms(MX, MY, [
    ("mine/tunnel10",    30, 280, "Tunnel 10",  "mines"),
    ("mine/tunnel11",    30, 340, "Tunnel 11",  "mines"),
    ("mine/tunnel12",    30, 400, "Tunnel 12",  "mines"),
    ("mine/tunnel13",    30, 460, "Dead End",   "mines"),
])

# East branch from tunnel9
add_rooms(MX, MY, [
    ("mine/tunnel14",   220, 280, "Tunnel 14",  "mines"),
    ("mine/tunnel15",   220, 340, "Tunnel 15",  "mines"),
    ("mine/tunnel16",   220, 400, "Tunnel 16",  "mines"),
    ("mine/tunnel17",   220, 460, "Tunnel 17",  "mines"),
    ("mine/tunnel18",   330, 460, "Dead End",   "mines"),
    ("mine/tunnel19",   220, 520, "Tunnel 19",  "mines"),
])

# Deep mine corridor
add_rooms(MX, MY, [
    ("mine/tunnel20",    30, 580, "Dead End",    "mines"),
    ("mine/tunnel21",   120, 580, "Tunnel 21",   "mines"),
    ("mine/tunnel22",   220, 580, "Tunnel 22",   "mines"),
    ("mine/tunnel23",   330, 580, "Tunnel 23",   "mines"),
    ("mine/tunnel24",   440, 580, "Tunnel 24",   "mines"),
    ("mine/tunnel25",   440, 640, "Tunnel 25",   "mines"),
    ("mine/tunnel26",   440, 700, "Tunnel 26",   "mines"),
    ("mine/tunnel27",   330, 700, "Dead End",    "mines"),
    ("mine/tunnel28",   550, 700, "Tunnel 28",   "mines"),
    ("mine/tunnel29",   660, 700, "DRAGON!",     "mines"),
])

# ===== CRESCENT LAKE REGION =====
# Three concentric rings: forest (outer), shore (middle), island (inner)

LAKE_CX, LAKE_CY = 2300, 2650
LAKE_R = 480       # shore radius
FOREST_R = 640     # outer forest radius
ISLE_R = 200       # island outer radius
ISLE_IR = 100      # island inner radius

# --- South Forest: 49 rooms on outer ring, organized by sector ---
# Sector 1 (NE, E, SE): sforst1-20 — angles 345° to 135° (150° span)
# Sector 2 (S, SW): sforst31-45 — angles 145° to 225° (80° span)
# Sector 3 (W, NW): sforst21-30 — angles 230° to 275° (45° span)
# Sector 4 (N): sforst46-49 — angles 295° to 335° (40° span)

def place_ring(room_nums, start_angle, end_angle, radius, cx, cy, prefix, label_prefix, region):
    """Place rooms evenly on an arc of a circle."""
    n = len(room_nums)
    # Handle wrapping past 360°
    span = end_angle - start_angle
    if span < 0:
        span += 360
    for i, num in enumerate(room_nums):
        if n > 1:
            angle_deg = start_angle + span * i / (n - 1)
        else:
            angle_deg = start_angle
        angle_rad = math.radians(angle_deg - 90)  # -90 so 0° = top
        x = cx + int(radius * math.cos(angle_rad))
        y = cy + int(radius * math.sin(angle_rad))
        rooms[f"{prefix}{num}"] = (x, y, f"{label_prefix}{num}", region)

# South forest sectors
place_ring(list(range(1, 21)), 345, 135, FOREST_R,
           LAKE_CX, LAKE_CY, "south/sforst", "SF ", "south_forest")
place_ring(list(range(31, 46)), 145, 225, FOREST_R,
           LAKE_CX, LAKE_CY, "south/sforst", "SF ", "south_forest")
place_ring(list(range(21, 31)), 230, 275, FOREST_R,
           LAKE_CX, LAKE_CY, "south/sforst", "SF ", "south_forest")
place_ring(list(range(46, 50)), 295, 335, FOREST_R,
           LAKE_CX, LAKE_CY, "south/sforst", "SF ", "south_forest")

# --- Shore: 30 rooms on middle ring, evenly spaced ---
place_ring(list(range(1, 31)), 0, 348, LAKE_R,
           LAKE_CX, LAKE_CY, "south/sshore", "Shore ", "shore")

# --- Isle of the Magi: outer ring (1-12) and inner ring (13-18) ---
place_ring(list(range(1, 13)), 0, 330, ISLE_R,
           LAKE_CX, LAKE_CY, "south/sislnd", "Isle ", "island")

isle_inner_labels = {
    13: "Isle 13", 14: "Isle 14", 15: "Isle 15",
    16: "Isle 16", 17: "Old Well", 18: "Tower Ruins",
}
for i, num in enumerate([13, 14, 15, 16, 17, 18]):
    angle_deg = i * 60  # evenly spaced
    angle_rad = math.radians(angle_deg - 90)
    x = LAKE_CX + int(ISLE_IR * math.cos(angle_rad))
    y = LAKE_CY + int(ISLE_IR * math.sin(angle_rad))
    rooms[f"south/sislnd{num}"] = (x, y, isle_inner_labels[num], "island")

# Fix special island labels
rooms["south/sislnd10"] = (rooms["south/sislnd10"][0], rooms["south/sislnd10"][1],
                           "Focus Point", "island")

# Lair (below the island well)
rooms["south/lair"] = (LAKE_CX, LAKE_CY + 40, "Lair", "island")


# ---------------------------------------------------------------------------
# Connections: (room_id_1, room_id_2)
# ---------------------------------------------------------------------------
connections = [
    # Mountains
    ("mount_top", "mount_top2"),
    ("mount_top", "ravine"),
    ("ravine", "mount_pass"),
    ("mount_pass", "plane11"),
    ("mount_pass", "mine/tunnel"),

    # Plains grid
    ("plane11", "plane13"),
    ("plane11", "plane12"),
    ("plane11", "plane6"),
    ("plane6", "plane13"),
    ("plane6", "plane12"),
    ("plane6", "plane3"),
    ("plane6", "plane10"),
    ("plane8", "plane13"),
    ("plane8", "plane6"),
    ("plane8", "ruin"),
    ("plane3", "ruin"),
    ("plane3", "plane7"),
    ("plane3", "plane2"),
    ("ruin", "plane4"),
    ("ruin", "plane9"),
    ("plane2", "plane4"),
    ("plane2", "plane5"),
    ("plane2", "plane1"),
    ("plane7", "plane5"),
    ("plane7", "plane10"),
    ("plane7", "big_tree"),
    ("plane10", "plane12"),
    ("plane12", "deep_forest1"),
    ("plane1", "clearing"),

    # Giants
    ("big_tree", "giant_path"),
    ("giant_path", "giant_lair"),
    ("giant_lair", "giant_conf"),

    # Orc Valley
    ("orc_vall", "slope"),
    ("orc_vall", "fortress"),
    ("fortress", "orc_treasure"),

    # Forest corridor
    ("slope", "forest2"),
    ("forest2", "clearing"),
    ("clearing", "forest1"),
    ("forest1", "wild1"),
    ("wild1", "hump"),
    ("hump", "vill_green"),
    ("slope", "forest3"),

    # Deep forest
    ("forest3", "forest4"),
    ("forest4", "forest5"),
    ("forest4", "forest6"),
    ("forest4", "forest7"),
    ("forest5", "forest8"),
    ("forest7", "forest10"),
    ("forest8", "forest9"),
    ("forest9", "forest10"),
    ("forest9", "forest11"),
    ("forest11", "forest12"),

    # Village core
    ("church", "vill_green"),
    ("vill_green", "vill_track"),
    ("vill_track", "vill_road1"),
    ("vill_road1", "vill_road2"),
    ("vill_road2", "vill_shore"),

    # Village buildings
    ("vill_road1", "yard"),
    ("yard", "pub2"),
    ("vill_road1", "narr_alley"),
    ("narr_alley", "bank"),
    ("bank", "bankroom"),
    ("narr_alley", "post"),
    ("vill_road2", "shop"),
    ("shop", "storage"),
    ("vill_road2", "adv_guild"),
    ("adv_guild", "adv_inner"),
    ("adv_inner", "adv_inner2"),
    ("vill_road2", "station"),
    ("vill_shore", "crop"),

    # East Road
    ("vill_shore", "eastroad1"),
    ("eastroad1", "eastroad2"),
    ("eastroad2", "eastroad3"),
    ("eastroad3", "eastroad4"),
    ("eastroad4", "eastroad5"),
    ("eastroad5", "inn"),
    ("eastroad3", "sunalley1"),
    ("sunalley1", "sunalley2"),

    # Jetty & Sea
    ("vill_shore", "jetty"),
    ("jetty", "vill_shore2"),
    ("vill_shore2", "jetty2"),
    ("jetty2", "sea"),
    ("sea", "sea_bottom"),

    # Elevator
    ("church", "elevator"),
    ("elevator", "attic"),
    ("elevator", "wiz_hall"),
    ("wiz_hall", "quest_room"),

    # Underground
    ("narr_alley", "well"),
    ("well", "maze1"),
    ("maze1", "maze2"),
    ("maze2", "maze3"),
    ("maze3", "maze4"),
    ("maze4", "maze5"),
    ("well", "door_trap"),
    ("door_trap", "after_trap"),

    # Mine main corridor
    ("mine/tunnel", "mine/tunnel2"),
    ("mine/tunnel2", "mine/tunnel3"),
    ("mine/tunnel3", "mine/tunnel4"),
    ("mine/tunnel4", "mine/tunnel5"),
    ("mine/tunnel5", "mine/tunnel_room"),

    # Mine shaft branch
    ("mine/tunnel3", "mine/tunnel8"),
    ("mine/tunnel8", "mine/tunnel9"),

    # Mine west branch
    ("mine/tunnel9", "mine/tunnel10"),
    ("mine/tunnel10", "mine/tunnel11"),
    ("mine/tunnel11", "mine/tunnel12"),
    ("mine/tunnel12", "mine/tunnel13"),

    # Mine east branch
    ("mine/tunnel9", "mine/tunnel14"),
    ("mine/tunnel14", "mine/tunnel15"),
    ("mine/tunnel15", "mine/tunnel16"),
    ("mine/tunnel16", "mine/tunnel17"),
    ("mine/tunnel17", "mine/tunnel18"),
    ("mine/tunnel17", "mine/tunnel19"),

    # Mine deep corridor
    ("mine/tunnel19", "mine/tunnel22"),
    ("mine/tunnel20", "mine/tunnel21"),
    ("mine/tunnel21", "mine/tunnel22"),
    ("mine/tunnel22", "mine/tunnel23"),
    ("mine/tunnel23", "mine/tunnel24"),
    ("mine/tunnel24", "mine/tunnel25"),
    ("mine/tunnel25", "mine/tunnel26"),
    ("mine/tunnel26", "mine/tunnel27"),
    ("mine/tunnel26", "mine/tunnel28"),
    ("mine/tunnel28", "mine/tunnel29"),

    # South forest connections (main grid)
    ("forest12", "south/sforst1"),
    ("south/sforst1", "south/sforst2"),
    ("south/sforst2", "south/sforst3"),
    ("south/sforst3", "south/sforst4"),
    ("south/sforst1", "south/sforst5"),
    ("south/sforst5", "south/sforst6"),
    ("south/sforst6", "south/sforst7"),
    ("south/sforst7", "south/sforst8"),
    ("south/sforst2", "south/sforst6"),
    ("south/sforst3", "south/sforst7"),
    ("south/sforst4", "south/sforst8"),
    ("south/sforst4", "south/sforst9"),
    ("south/sforst9", "south/sforst10"),
    ("south/sforst10", "south/sforst11"),
    ("south/sforst11", "south/sforst12"),
    ("south/sforst12", "south/sforst13"),
    ("south/sforst13", "south/sforst14"),
    ("south/sforst14", "south/sforst15"),
    ("south/sforst15", "south/sforst16"),
    ("south/sforst16", "south/sforst17"),
    ("south/sforst16", "south/sforst19"),
    ("south/sforst17", "south/sforst18"),
    ("south/sforst18", "south/sforst19"),
    ("south/sforst18", "south/sforst20"),
    ("south/sforst19", "south/sforst20"),

    # South forest - southwest cluster
    ("south/sforst21", "south/sforst22"),
    ("south/sforst22", "south/sforst23"),
    ("south/sforst23", "south/sforst24"),
    ("south/sforst24", "south/sforst25"),
    ("south/sforst25", "south/sforst26"),
    ("south/sforst26", "south/sforst27"),
    ("south/sforst22", "south/sforst27"),
    ("south/sforst23", "south/sforst26"),
    ("south/sforst25", "south/sforst29"),
    ("south/sforst26", "south/sforst28"),
    ("south/sforst27", "south/sforst28"),  # via shore
    ("south/sforst28", "south/sforst29"),
    ("south/sforst29", "south/sforst30"),

    # South forest - southeast cluster
    ("south/sforst31", "south/sforst32"),
    ("south/sforst32", "south/sforst33"),
    ("south/sforst33", "south/sforst34"),
    ("south/sforst34", "south/sforst35"),
    ("south/sforst35", "south/sforst36"),
    ("south/sforst32", "south/sforst36"),
    ("south/sforst36", "south/sforst37"),
    ("south/sforst37", "south/sforst38"),
    ("south/sforst38", "south/sforst39"),
    ("south/sforst39", "south/sforst40"),
    ("south/sforst40", "south/sforst41"),
    ("south/sforst40", "south/sforst42"),
    ("south/sforst41", "south/sforst42"),
    ("south/sforst42", "south/sforst43"),  # was sforst44
    ("south/sforst43", "south/sforst44"),
    ("south/sforst44", "south/sforst45"),
    ("south/sforst37", "south/sforst45"),
    ("south/sforst35", "south/sforst38"),
    ("south/sforst33", "south/sforst35"),

    # South forest - far south cluster
    ("south/sforst46", "south/sforst47"),
    ("south/sforst46", "south/sforst48"),
    ("south/sforst47", "south/sforst48"),
    ("south/sforst48", "south/sforst49"),
    ("south/sforst49", "south/sforst46"),

    # Key forest-to-shore crossing points (sparse, to reduce visual noise)
    ("south/sforst8", "south/sshore1"),
    ("south/sforst20", "south/sshore9"),
    ("south/sforst41", "south/sshore14"),
    ("south/sforst31", "south/sshore19"),
    ("south/sforst21", "south/sshore23"),
    ("south/sforst46", "south/sshore26"),

    # Shore ring (adjacent shore connections)
    ("south/sshore1", "south/sshore2"),
    ("south/sshore2", "south/sshore3"),
    ("south/sshore3", "south/sshore4"),
    ("south/sshore4", "south/sshore5"),
    ("south/sshore5", "south/sshore6"),
    ("south/sshore6", "south/sshore7"),
    ("south/sshore7", "south/sshore8"),
    ("south/sshore8", "south/sshore9"),
    ("south/sshore9", "south/sshore10"),
    ("south/sshore10", "south/sshore11"),
    ("south/sshore11", "south/sshore12"),
    ("south/sshore12", "south/sshore13"),
    ("south/sshore13", "south/sshore14"),
    ("south/sshore14", "south/sshore15"),
    ("south/sshore15", "south/sshore16"),
    ("south/sshore16", "south/sshore17"),
    ("south/sshore17", "south/sshore18"),
    ("south/sshore18", "south/sshore19"),
    ("south/sshore19", "south/sshore20"),
    ("south/sshore20", "south/sshore21"),
    ("south/sshore21", "south/sshore22"),
    ("south/sshore22", "south/sshore23"),
    ("south/sshore23", "south/sshore24"),
    ("south/sshore24", "south/sshore25"),
    ("south/sshore25", "south/sshore26"),
    ("south/sshore26", "south/sshore27"),
    ("south/sshore27", "south/sshore28"),
    ("south/sshore28", "south/sshore29"),
    ("south/sshore29", "south/sshore1"),
    ("south/sshore30", "south/sshore1"),

    # Island crossing
    ("south/sshore26", "south/sislnd1"),

    # Island connections (outer ring)
    ("south/sislnd1", "south/sislnd2"),
    ("south/sislnd2", "south/sislnd3"),
    ("south/sislnd3", "south/sislnd4"),
    ("south/sislnd4", "south/sislnd5"),
    ("south/sislnd5", "south/sislnd6"),
    ("south/sislnd6", "south/sislnd7"),
    ("south/sislnd7", "south/sislnd8"),
    ("south/sislnd8", "south/sislnd9"),
    ("south/sislnd9", "south/sislnd10"),
    ("south/sislnd10", "south/sislnd11"),
    ("south/sislnd11", "south/sislnd12"),
    ("south/sislnd12", "south/sislnd1"),

    # Island inner connections
    ("south/sislnd1", "south/sislnd13"),
    ("south/sislnd2", "south/sislnd14"),
    ("south/sislnd4", "south/sislnd14"),
    ("south/sislnd5", "south/sislnd15"),
    ("south/sislnd6", "south/sislnd16"),
    ("south/sislnd7", "south/sislnd16"),
    ("south/sislnd8", "south/sislnd16"),
    ("south/sislnd8", "south/sislnd17"),
    ("south/sislnd9", "south/sislnd17"),
    ("south/sislnd12", "south/sislnd17"),
    ("south/sislnd12", "south/sislnd13"),
    ("south/sislnd13", "south/sislnd14"),
    ("south/sislnd13", "south/sislnd18"),
    ("south/sislnd14", "south/sislnd15"),
    ("south/sislnd15", "south/sislnd18"),
    ("south/sislnd16", "south/sislnd18"),
    ("south/sislnd17", "south/sislnd18"),
    ("south/sislnd11", "south/sislnd12"),

    # Lair
    ("south/sislnd17", "south/lair"),
]


# ---------------------------------------------------------------------------
# Drawing
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


def draw_connection(draw, id1, id2):
    """Draw a line between two rooms."""
    if id1 not in rooms or id2 not in rooms:
        return
    x1, y1 = rooms[id1][0], rooms[id1][1]
    x2, y2 = rooms[id2][0], rooms[id2][1]
    draw.line([(x1, y1), (x2, y2)], fill=(140, 140, 160, 200), width=2)


def draw_region_bg(draw, region_name, region_key, bbox, alpha=30):
    """Draw a semi-transparent region background with label."""
    x0, y0, x1, y1 = bbox
    fill = PALETTE[region_key][0]
    # Darken the fill and apply low alpha
    bg = (fill[0] // 3, fill[1] // 3, fill[2] // 3, alpha)
    draw.rounded_rectangle([x0, y0, x1, y1], radius=10, fill=bg,
                           outline=(fill[0] // 2, fill[1] // 2, fill[2] // 2, 60),
                           width=1)
    # Region label
    tw, th = text_size(draw, region_name, FONT_REGION)
    draw.text((x0 + 8, y0 + 4), region_name,
              fill=(fill[0], fill[1], fill[2], 180), font=FONT_REGION)


def get_region_bbox(region_key, padding=25):
    """Compute bounding box for all rooms in a region."""
    xs = []
    ys = []
    for rid, (x, y, lbl, reg) in rooms.items():
        if reg == region_key:
            tw, _ = text_size(ImageDraw.Draw(Image.new("RGBA", (1, 1))), lbl, FONT_ROOM)
            xs.extend([x - tw // 2 - 8, x + tw // 2 + 8])
            ys.extend([y - 14, y + 14])
    if not xs:
        return None
    return (min(xs) - padding, min(ys) - padding,
            max(xs) + padding, max(ys) + padding)


# ---------------------------------------------------------------------------
# Main rendering
# ---------------------------------------------------------------------------
def render():
    img = Image.new("RGBA", (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(img, "RGBA")

    # --- Region labels (floating text near each region, no background boxes) ---
    region_annotations = [
        ("MOUNTAINS",          2700, 85,   "mountains"),
        ("MINES",              650, 310,   "mines"),
        ("THE GREAT PLAINS",   2550, 510,  "plains"),
        ("GIANT TERRITORY",    1050, 775,  "giants"),
        ("ORC VALLEY",         1440, 885,  "orcs"),
        ("FOREST",             1850, 1065, "forest"),
        ("DEEP FOREST",        1350, 1165, "deep_forest"),
        ("VILLAGE",            2900, 1065, "village"),
        ("EAST ROAD",          3580, 855,  "east_road"),
        ("THE SEA",            3800, 1165, "sea"),
        ("UNDERGROUND",        2750, 1510, "underground"),
        ("DIMLY LIT FOREST",   2100, 1920, "south_forest"),
        ("CRESCENT LAKE",      2150, 2080, "shore"),
        ("ISLE OF THE MAGI",   2200, 2620, "island"),
        ("SPECIAL ROOMS",      350,  165,  "special"),
    ]
    for label, lx, ly, rkey in region_annotations:
        fill = PALETTE[rkey][0]
        color = (fill[0], fill[1], fill[2], 120)
        draw.text((lx, ly), label, fill=color, font=FONT_REGION)

    # --- Draw lake water (separate layer with proper alpha compositing) ---
    lake_layer = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    lake_draw = ImageDraw.Draw(lake_layer, "RGBA")
    lake_inner = LAKE_R - 30
    lake_draw.ellipse([LAKE_CX - lake_inner, LAKE_CY - lake_inner,
                       LAKE_CX + lake_inner, LAKE_CY + lake_inner],
                      fill=(20, 45, 100, 25),
                      outline=(40, 70, 130, 50), width=1)
    img = Image.alpha_composite(img, lake_layer)
    draw = ImageDraw.Draw(img, "RGBA")

    # --- Connection lines (drawn before rooms so they go behind) ---
    for id1, id2 in connections:
        draw_connection(draw, id1, id2)

    # --- Draw special long-distance connection indicators ---
    # mount_pass to mine/tunnel (dashed-style)
    if "mount_pass" in rooms and "mine/tunnel" in rooms:
        x1, y1 = rooms["mount_pass"][0], rooms["mount_pass"][1]
        x2, y2 = rooms["mine/tunnel"][0], rooms["mine/tunnel"][1]
        # Draw dashed line
        steps = 30
        for i in range(0, steps, 2):
            t1 = i / steps
            t2 = min((i + 1) / steps, 1.0)
            sx = int(x1 + (x2 - x1) * t1)
            sy = int(y1 + (y2 - y1) * t1)
            ex = int(x1 + (x2 - x1) * t2)
            ey = int(y1 + (y2 - y1) * t2)
            draw.line([(sx, sy), (ex, ey)], fill=(150, 150, 170, 120), width=2)

    # --- Room boxes ---
    for rid, (x, y, label, region) in rooms.items():
        draw_room_box(draw, x, y, label, region)

    # --- Title ---
    title = "LPmud 2.4.5 World Map"
    tw, th = text_size(draw, title, FONT_TITLE)
    draw.text((WIDTH // 2 - tw // 2, 15), title,
              fill=(220, 210, 180), font=FONT_TITLE)
    subtitle = "~200 rooms across 15+ regions"
    sw, sh = text_size(draw, subtitle, FONT_LEGEND)
    draw.text((WIDTH // 2 - sw // 2, 48), subtitle,
              fill=(160, 155, 140), font=FONT_LEGEND)

    # --- Compass Rose ---
    cx, cy = WIDTH - 80, 80
    r = 30
    draw.line([(cx, cy - r), (cx, cy + r)], fill=(180, 180, 190), width=2)
    draw.line([(cx - r, cy), (cx + r, cy)], fill=(180, 180, 190), width=2)
    for label, dx, dy in [("N", 0, -r - 12), ("S", 0, r + 4),
                          ("W", -r - 14, -4), ("E", r + 4, -4)]:
        draw.text((cx + dx, cy + dy), label,
                  fill=(200, 200, 210), font=FONT_LEGEND)

    # --- Legend ---
    legend_x, legend_y = WIDTH - 200, 160
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
        ("Special",        "special"),
    ]
    for i, (name, key) in enumerate(items):
        y = legend_y + i * 22
        fill, border, _ = PALETTE[key]
        draw.rounded_rectangle([legend_x, y, legend_x + 16, y + 14],
                               radius=2, fill=fill, outline=border)
        draw.text((legend_x + 22, y + 1), name,
                  fill=(190, 185, 170), font=FONT_ROOM)

    # --- Notes ---
    notes = [
        "Dashed line = distant connection",
        "DRAGON! = Boss encounter",
        "Maze has randomized exits",
    ]
    ny = legend_y + len(items) * 22 + 15
    for note in notes:
        draw.text((legend_x, ny), note,
                  fill=(140, 135, 120), font=FONT_SMALL)
        ny += 13

    # --- Convert to RGB and save ---
    output = img.convert("RGB")
    out_dir = os.path.join(os.path.dirname(__file__), "..", "docs")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "world-map.png")
    output.save(out_path, "PNG")
    print(f"Map saved to {os.path.abspath(out_path)}")
    print(f"  Canvas: {WIDTH}x{HEIGHT}")
    print(f"  Rooms: {len(rooms)}")
    print(f"  Connections: {len(connections)}")


if __name__ == "__main__":
    render()
