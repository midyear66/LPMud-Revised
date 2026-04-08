# LPmud Server and Admin Panel (Dockerized)

Recreating a classic LPmud server using [ldmud](https://github.com/ldmud/ldmud) as the game driver, running the LPmud 2.4.5 mudlib, with a Flask-based admin panel for server management, player editing, backups, and an interactive world map — all packaged in Docker for easy deployment.

## Overview

**LPmud** (Lars Pensjö MUD) is one of the original MUD server architectures from the early 1990s. Lars Pensjö created the first LPmud driver in 1989, introducing the revolutionary concept of an in-game programming language (LPC) that allowed builders to extend the game world without modifying the driver itself.

This project runs the classic **2.4.5 mudlib** — the most widely used LPmud mudlib of the era — on top of **ldmud**, the modern direct descendant of the original LPmud driver.

## Credits & Acknowledgments

This project builds on the work of many contributors to the LPmud ecosystem:

- **Lars Pensjö** — Created the original LPmud driver and the 2.4.5 mudlib at Chalmers University, Gothenburg. His work introduced in-game LPC programming and spawned an entire family of MUD servers. All core mudlib code descends from his original codebase.
- **[ldmud project](https://github.com/ldmud/ldmud)** — The modern, actively maintained LPmud driver used in this project, and the source of the pre-ported [lp-245](https://github.com/ldmud/ldmud/tree/master/mud/lp-245) mudlib fetched by `scripts/extract-mudlib.sh`.
- **Anders Ripa ("Tech")** — Author of the roommaker (`mudlib/obj/roommaker.c`), quicktyper (`mudlib/obj/quicktyper.c`), and debug header (`mudlib/sys/debug.h`).
- **Padrone** — Gender system modifications to `mudlib/obj/soul.c`.
- **UC Berkeley** — BSD-licensed telnet protocol definitions in `mudlib/sys/telnet.h` (1983).
- **[Genesis MUD](https://www.genesismud.org/)** — The first LPmud, founded at Chalmers University, Gothenburg, where this mudlib originated.

## Architecture

- **Driver**: [ldmud](https://github.com/ldmud/ldmud) 3.6.x, compiled from source in a multi-stage Docker build
- **Base image**: Debian bookworm-slim
- **Mudlib**: LPmud 2.4.5 (ldmud's pre-ported [lp-245](https://github.com/ldmud/ldmud/tree/master/mud/lp-245))
- **Compat mode**: Enabled (`--compat`) for 2.4.5 mudlib compatibility
- **Port**: 4000 (telnet), 8080 (admin web interface)
- **Admin dashboard**: Flask + Gunicorn, runs as a separate Docker service
- **Mudlib editing**: Mounted as a Docker volume for live editing during development
- **Configuration**: `config/ldmud.conf` in ldmud `--args` format (one flag per line)
- **Security**: Both containers run as non-root users

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/) and Docker Compose

## Quick Start

```bash
# Clone the repository
git clone git@github.com:midyear66/LPMud-Revised.git
cd LPMud-Revised

# Set up environment variables (required for admin interface)
cp docker/.env.example docker/.env
# Edit docker/.env with your own ADMIN_PASSWORD and SECRET_KEY

# Build the containers (compiles ldmud from source, bundles the mudlib)
docker compose -f docker/docker-compose.yml build

# Start all services (MUD server + admin dashboard)
docker compose -f docker/docker-compose.yml up

# Connect to the MUD via telnet (port 4000)
telnet localhost 4000

# Access the admin dashboard at http://localhost:8080
```

Create a character at the login prompt. To explore the world, try `look`, `north`, `south`, `east`, `west`, `inventory`, and `say <message>`.

## Project Structure

```
lpmud/
├── admin/                  # Flask admin web interface
│   ├── Dockerfile          #   Python 3.12-slim, Gunicorn
│   ├── app.py              #   App factory, blueprints, security headers
│   ├── auth.py             #   Login, session mgmt, rate limiting
│   ├── dashboard.py        #   Status overview page
│   ├── backups.py          #   Backup create/list/download/restore/delete
│   ├── scheduler.py        #   APScheduler cron UI for map regen & backups
│   ├── retention.py        #   Backup retention settings & pruning logic
│   ├── mapviewer.py        #   Interactive Leaflet map viewer
│   ├── players.py          #   Player save file viewer/editor
│   ├── templates/          #   Jinja2 templates (base, login, dashboard, etc.)
│   └── static/             #   CSS (light/dark theme), JS (theme toggle, map)
├── config/                 # ldmud runtime configuration
│   └── ldmud.conf          #   --args format config file
├── docker/                 # Docker build files
│   ├── Dockerfile          #   Multi-stage build (compile ldmud + runtime)
│   ├── docker-compose.yml  #   Service definition, ports, volumes
│   └── .env.example        #   Template for required environment variables
├── docs/                   # Project documentation and maps
│   ├── world-map.png       #   Visual world map
│   ├── world-map.json      #   Room/edge data for interactive viewer
│   ├── world-map.txt       #   ASCII world map
│   └── room-connectivity.txt  # Room connection graph
├── mudlib-orig/             # Unmodified original 2.4.5 mudlib (for reference/diffing)
├── mudlib/                 # LPmud 2.4.5 mudlib (live-editable via volume)
│   ├── obj/                #   Core objects (master, player, etc.)
│   ├── room/               #   World rooms
│   ├── monsters/           #   NPC definitions
│   ├── players/            #   Player save data (gitignored)
│   └── ...
├── scripts/                # Helper scripts
│   ├── build.sh            #   Build helper
│   ├── extract-mudlib.sh   #   Re-fetch mudlib from ldmud repo
│   └── generate-map.py     #   Generate world map from room files
├── backups/                # Backup archives (bind-mounted, gitignored)
├── admin-data/             # Admin persistent data (bind-mounted, gitignored)
├── .gitignore
├── CLAUDE.md               # AI assistant project context
└── README.md               # This file
```

## Configuration

The ldmud driver is configured via `config/ldmud.conf` using the `--args` format (one flag per line):

| Setting | Value | Description |
|---------|-------|-------------|
| `--compat` | | Enable 2.4.5 compatibility mode |
| `--mudlib` | `/mud/lib` | Path to mudlib inside container |
| `--master` | `obj/master` | Master object (relative to mudlib) |
| `--debug-file` | `/mud/lib/log/debug.log` | Driver debug log location |
| `--max-array` | `5000` | Maximum array size |
| `--max-mapping` | `5000` | Maximum mapping size |
| `--max-bytes` | `100000` | Maximum byte transfer |
| `--no-erq` | | Disable external request daemon |
| Port | `4000` | Telnet listen port |

## Mudlib

The mudlib comes from ldmud's maintained [lp-245](https://github.com/ldmud/ldmud/tree/master/mud/lp-245) directory — the original 2.4.5 mudlib, pre-ported to work with ldmud 3.x in compat mode.

To re-fetch a clean copy of the mudlib:

```bash
rm -rf mudlib/*
./scripts/extract-mudlib.sh
```

## Custom Additions

<details>
<summary>Changes beyond the original 2.4.5 mudlib (click to expand)</summary>

Use `mudlib-orig/` to diff against the unmodified source.

- **Orc Fortress Quest** — 12-room dungeon connected from the existing fortress room (`room/fortress.c`). Includes gate, mess hall, armory, barracks, great hall, pit, towers, walls, cells, stairs, and throne room (boss). Located in `room/orc_fortress/`.
- **Forest Remedy Quest** — Multi-step fetch quest starting with a wounded traveler in `room/deep_forest1`. Player finds Doctor Willem (`room/doctor_hut` off `narr_alley`), who needs a silverleaf herb to make a remedy. The old herbalist (`room/herbalist_hut` off `forest6`) can hint at where the herb grows. Player collects the herb from a random forest room, brings it back to the doctor who creates a healing medicine, then delivers it to the traveler. Exclusive (one player at a time), awards 500 XP and the `forest_remedy` quest flag for wizard promotion. Controller: `obj/forest_quest.c`.
- **The Restless Dead Quest** — Combat + puzzle quest for mid-level players (4-8), starting in the cemetery north of the church (`room/cemetery`). Brother Cedric asks the player to enter an ancient crypt and lay the spirit of the sorcerer Valdris to rest. Player receives holy water, fights through skeleton guardians in a dark 3-room crypt (`room/crypt/`), uses the holy water to break a ward on the tomb, then defeats the Wraith of Valdris (level 7 boss). Exclusive (one player at a time), awards 750 XP, wearable enchanted silver ring (AC 1), and the `restless_dead` quest flag for wizard promotion. Controller: `obj/crypt_quest.c`.
- **Underground Rail** — Fast-travel mine cart system connecting four stations: Village, Eastroad Inn, Plains Crossing, and Orc Valley. The cart runs a continuous loop on a ~60-second circuit, pausing 10 seconds at each stop. Players `board` the cart and `exit` at their destination. Stations are accessed via `down` from surface rooms. Train object: `obj/train.c`, station rooms: `room/station.c`, `room/station_east.c`, `room/station_plains.c`, `room/station_orc.c`, sign: `obj/sign.c`.
- **Harry NPC overhaul** — Harry (`room/vill_road2.c`) has expanded chat lines (16 idle, 10 combat), varied reactions to arrivals/attacks/speech, and responds to emotes (hug, poke, kick, slap, cuddle, growl), equipment actions (wield, wear), and social commands (laugh, yawn). He also follows players between rooms and nags idle visitors.
- **The Fairy Plague Quest** — Multi-stage quest for mid-level players (7-10), set in the southern forest and Crescent Lake shore. A sick fairy at the lakeshore (`room/south/sshore5`) reveals a plague killing the fairy colony. The player finds the fairy elder in a hidden glade (`room/south/fairy_glade` off `sforst35`), gathers a rare moonpetal flower from the forest, receives a purifying draught, then enters a corrupted grotto (`room/south/grotto` off `sshore10`) to destroy a cursed runestone and defeat the Corrupted Sprite (level 8 boss with poison spells). Exclusive (one player at a time), awards 1000 XP, wearable fairy amulet (AC 1), and the `fairy_plague` quest flag for wizard promotion. Controller: `obj/fairy_quest.c`.
- **Wizard path shortcuts** — The `clone` and `goto` commands now search common directories (`room/`, `room/south/`, `obj/`, etc.) when a direct path isn't found. Example: `goto fairy_glade` instead of `goto room/south/fairy_glade`, `clone moonpetal` instead of `clone obj/moonpetal`.
- **Bank vault** — Persistent bank vault in `room/bank.c` where players can deposit/withdraw gold and store/retrieve items. Data survives logoff and server restarts via per-player vault files (`players/<name>/bank.vault`). A bank clerk NPC narrates transactions with flavor text. Commands: `deposit`, `withdraw`, `balance`, `store`, `retrieve`, `vault`. Supports weapons, armour, and treasure items with full property serialization. Max 20 items per vault. Controller: `obj/bank_vault.c`.
- **Inventory gold display** — The `i` (inventory) command now shows carried gold coins alongside items.
- **NPC prompt fix** — NPC ambient chat and `set_match` responses from heartbeats now re-display the `> ` command prompt afterward, so players no longer need to press return to get their prompt back.
- **The Wyrm's Spellbook Quest** — Retrieval quest for high-level players (16+), set on the Isle of the Magi. The wizard Marvus at the crumbling monument (`room/south/sislnd15`) asks the player to descend the old well and slay the Wyrm of Arcanarton (level 17 boss) to recover his dead brother Aldric's spellbook. The spellbook spawns on the lair floor when the wyrm dies (`room/south/lair.c`). Exclusive (one player at a time), awards 1500 XP, wearable arcane bracer (AC 1), and the `wyrm_spellbook` quest flag for wizard promotion. Controller: `obj/wyrm_quest.c`.
- **The Lost Miners Quest** — Rescue quest for mid-level players (10-14), set in the mine tunnels. Old miner Grimjaw in `room/mine/tunnel2` asks the player to find his crew trapped by a cave-in deep below. Player receives a rope, navigates through the mine (encountering new monsters along the way), finds the trapped miner in `room/mine/tunnel27`, gives the rope to free him, and returns a mining token to Grimjaw. Exclusive (one player at a time), awards 2000 XP, miner's lamp, and the `lost_miners` quest flag for wizard promotion. Controller: `obj/mine_quest.c`.
- **World monster population** — 17 new monsters across 5 regions to fill the level 7-16 gap: mines (giant rat, mine beetle, tunnel spider, rock worm, cave troll, mine foreman), south forest (wild boar, dark wolf, forest ogre, shadow stalker), Crescent Lake shore (marsh ghoul, shore troll, lake serpent), mountains (mountain lion, storm eagle), plains (plains bandit, ruin golem). Several drop unique treasure items.
- **Seamless wizard castles** — Wizard castles now integrate as directional exits in the room where they are dropped. The castle template (`room/def_castle.c`) supports configurable direction, custom description text (e.g. "To the south is a long bridge into a marsh"), and open/closed gating via `DIRECTION`, `CASTLE_SHORT`, and `OPEN` defines. Wizards edit their `players/<name>/castle.c` to customize. Direction conflict detection prevents duplicating existing room exits. A default workroom (`room/def_workroom.c`) is auto-created so the `home` command works immediately. Player deletion via the admin panel now cleans up wizard directories and init_file entries. The map generator auto-discovers open wizard castles and all rooms under `players/<name>/rooms/`, rendering each wizard's area as its own named region (e.g. "Peace's Castle") with a unique color tile on the world map.
- **Familiar orb** — Cloneable wizard tool (`obj/familiar_orb.c`) that lets any wizard conjure and control a remote familiar NPC. Commands: `familiar create <desc>`, `look`, `move`, `say`, `emote`, `send`, `recall`, `where`, `mute`, `destroy` (also via `fm` shorthand). The familiar relays room messages back to the wizard with a `[Familiar]` prefix, flees when attacked, and can be used to scout rooms remotely. Level-gated to wizards (level 20+); non-wizards cannot pick it up. Dropping the orb destroys it and any active familiar. Familiar NPC: `obj/familiar.c`.
- **Bug fixes** — Runtime error logging in `obj/master.c`, relaxed wizard level check in `obj/leo.c`, `query_ip_name`/`query_ip_number` crash guard for non-interactive objects in `obj/simul_efun.c`, wyrm gem drop fix in `room/south/lair.c` (sapphire could never drop due to off-by-one). Roommaker fixes: array overflow in max exits check, broken `long()` display string, generated code type mismatch in `query_room_maker()`, off-by-one in direction editor bounds, overly broad filename validation. Player teleport fix: error message now shows original filename instead of "0". Elevator typo fix.

> **Convention:** When adding new public areas to the mudlib, document them in this section.

</details>

## Becoming a Wizard

To gain wizard privileges, log out and edit your player save file:

1. Find your save file at `mudlib/players/<yourname>.o` (or use the admin Player Editor at `:8080/players`)
2. Set the `level` field to 25 or higher
3. Log back in — you now have wizard commands

Wizard mode grants access to in-game building commands, object manipulation, and LPC evaluation.

## World Map

The `docs/` directory contains world map documentation:

- **`world-map.png`** — Visual map of the game world (~200 rooms across 15+ regions)
- **`world-map.json`** — Machine-readable room/edge data for the interactive map viewer
- **`world-map.txt`** — ASCII representation of the map
- **`room-connectivity.txt`** — Complete room connection graph

## Development

### Live Editing

The mudlib is mounted as a Docker volume (`docker-compose.yml`), so changes to files in `mudlib/` are reflected immediately in the running server. Most LPC objects can be reloaded in-game without a restart using wizard commands.

### Map Generation

The world map is auto-generated by `scripts/generate-map.py`, which parses all room `.c` files in the mudlib to discover rooms and connections, then renders a PNG using [Pillow](https://pillow.readthedocs.io/) and [NetworkX](https://networkx.org/). Rooms are laid out on per-region grids with compass-faithful BFS placement and orthogonal connection routing. The script also exports `world-map.json` with room coordinates, exits, and edge waypoints for the interactive map viewer.

**Using Docker (recommended):**

```bash
docker compose -f docker/docker-compose.yml run --rm tools
```

**Running locally:**

```bash
pip3 install -r scripts/requirements.txt
python3 scripts/generate-map.py
```

Output is saved to `docs/world-map.png`, `docs/world-map.json`, `docs/room-connectivity.txt`, and `docs/world-map.txt`. When rooms are added or modified in the mudlib, re-run the script to update the map automatically.

**Automated regeneration:**

A wrapper script `scripts/regen-map.sh` only regenerates the map when room files have changed. Add it to cron for nightly updates:

```bash
crontab -e
# Add this line:
0 2 * * * /path/to/lpmud/scripts/regen-map.sh >> /tmp/map-gen.log 2>&1
```

## Admin Web Interface

A Flask-based admin dashboard runs as a separate Docker service on **port 8080**, providing server management without shell access.

### Setup

```bash
# Create .env from the template
cp docker/.env.example docker/.env

# Edit docker/.env — set a real password and secret key:
#   ADMIN_PASSWORD=your-password-here
#   SECRET_KEY=$(python3 -c "import secrets; print(secrets.token_hex(32))")

# Build and start
docker compose -f docker/docker-compose.yml build admin
docker compose -f docker/docker-compose.yml up -d admin

# Visit http://localhost:8080
```

### Features

| Page | Description |
|------|-------------|
| **Dashboard** | Server overview — room count, player count, backup status, scheduler status. Graceful shutdown with configurable countdown (1-15 min) warns players in-game before stopping |
| **Map** | Interactive world map viewer (Leaflet.js) — clickable rooms with popups showing exits (with destination name and region), region info, and file paths; room search; click-to-navigate exit links. Falls back to static PNG if JSON data is unavailable |
| **Players** | Browse, create, edit, and delete player save files — stats, levels, inventory, flags. Live online/offline status badges poll every 10 seconds; editing is disabled while a player is online or stale to prevent save conflicts |
| **Backups** | Create, download, restore, and delete tar.gz backups of mudlib, saves, or logs |
| **Scheduler** | Daily schedules for automatic map regeneration and backups, backup retention policy (configurable daily/monthly/yearly limits with automatic pruning), and on-demand actions |

### Security

- Single admin password via `ADMIN_PASSWORD` env var (timing-safe comparison)
- Session-based auth with 8-hour inactivity timeout (configurable via `SESSION_TIMEOUT_MINUTES`)
- Login rate limiting (5 attempts/minute, then 60-second lockout)
- CSRF protection on all forms (Flask-WTF)
- Security headers: CSP, X-Frame-Options, X-Content-Type-Options
- Mudlib mounted read-write — admin can edit player saves directly
- Container runs as non-root with `no-new-privileges`

> **Production**: Bind to localhost only (`127.0.0.1:8080:8080` in docker-compose.yml) and put a reverse proxy (nginx/Caddy) with TLS in front.

## Design Decisions

- **ldmud over FluffOS**: FluffOS (MudOS lineage) was considered but rejected — it has no compat mode and would require a full mudlib rewrite to work with 2.4.5. ldmud is the direct descendant of the original LPmud driver and supports compat mode natively.
- **Compat mode**: Required for the 2.4.5 mudlib. Modern ldmud defaults to "native" mode with different semantics.
- **`transfer()` warnings**: The 2.4.5 mudlib uses the deprecated `transfer()` efun extensively. These warnings are non-fatal and can be addressed later by creating a `simul_efun` wrapper.
- **Multi-stage Docker build**: Keeps the runtime image small (~50MB) by separating build dependencies from the final image.
- **Bind-mounted mudlib**: Host directories are bind-mounted into both containers. Both run as uid 1000 (matching the host user), so file permissions just work — no named volumes or group hacks needed.

## References

- [ldmud](https://github.com/ldmud/ldmud) — The game driver (direct descendant of original LPmud)
- [ldmud Documentation](https://www.ldmud.eu/) — Official ldmud docs
- [LPMud on Wikipedia](https://en.wikipedia.org/wiki/LPMud) — History and background
- [MUD on Encyclopedia MDPI](https://encyclopedia.pub/entry/31155) — Academic overview
- [FluffOS](https://www.fluffos.info/) — Alternative MUD driver (MudOS lineage)
- [LPmuds.net Downloads](http://lpmuds.net/downloads.html) — MUD resources and archives
- [Intermud Protocol](http://lpmuds.net/intermud.html) — Inter-MUD communication
- [CD Gamedriver](https://github.com/cotillion/cd-gamedriver) — Another LPmud driver fork
- [Awesome MUDs](https://github.com/maldorne/awesome-muds) — Curated list of MUD codebases, drivers, clients, protocols, tools, and community resources

## Status

Server builds and runs successfully. The 2.4.5 mudlib loads with some `transfer()` deprecation warnings (non-fatal). The game world is explorable and functional.

## License

This project is licensed under the [MIT License](LICENSE). You are free to copy, modify, and distribute this code as long as the original copyright notice and permission notice are included.

Note: Bundled third-party code retains its original licensing terms:

- **ldmud driver & LPmud 2.4.5 mudlib** — BSD-like license with a **non-commercial restriction**: "may not be used in any way whatsoever for monetary gain." See the ldmud [COPYRIGHT](https://github.com/ldmud/ldmud/blob/master/COPYRIGHT) file for full terms.
- **`mudlib/sys/telnet.h`** — Original BSD license (UC Berkeley, 1983).
- **Anders Ripa's tools** (`roommaker.c`, `quicktyper.c`, `debug.h`) — Author-attributed, no formal license specified.
