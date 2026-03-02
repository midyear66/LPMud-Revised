# LPmud Server (Dockerized)

Recreating a classic LPmud server using [ldmud](https://github.com/ldmud/ldmud) as the game driver, running the LPmud 2.4.5 mudlib — all packaged in a Docker container for easy deployment.

## Overview

**LPmud** (Lars Pensjö MUD) is one of the original MUD server architectures from the early 1990s. Lars Pensjö created the first LPmud driver in 1989, introducing the revolutionary concept of an in-game programming language (LPC) that allowed builders to extend the game world without modifying the driver itself.

This project runs the classic **2.4.5 mudlib** — the most widely used LPmud mudlib of the era — on top of **ldmud**, the modern direct descendant of the original LPmud driver.

## Architecture

- **Driver**: [ldmud](https://github.com/ldmud/ldmud) 3.6.x, compiled from source in a multi-stage Docker build
- **Base image**: Debian bookworm-slim
- **Mudlib**: LPmud 2.4.5 (ldmud's pre-ported [lp-245](https://github.com/ldmud/ldmud/tree/master/mud/lp-245))
- **Compat mode**: Enabled (`--compat`) for 2.4.5 mudlib compatibility
- **Port**: 4000 (telnet)
- **Mudlib editing**: Mounted as a Docker volume for live editing during development
- **Configuration**: `config/ldmud.conf` in ldmud `--args` format (one flag per line)
- **Security**: Runs as non-root `mud` user inside the container

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/) and Docker Compose

## Quick Start

```bash
# Clone the repository
git clone git@github.com:midyear66/LPMud-Revised.git
cd LPMud-Revised

# Build the container (compiles ldmud from source, bundles the mudlib)
docker compose -f docker/docker-compose.yml build

# Start the MUD server
docker compose -f docker/docker-compose.yml up

# Connect via telnet (default port 4000)
telnet localhost 4000
```

Create a character at the login prompt. To explore the world, try `look`, `north`, `south`, `east`, `west`, `inventory`, and `say <message>`.

## Project Structure

```
lpmud/
├── config/                 # ldmud runtime configuration
│   └── ldmud.conf          #   --args format config file
├── docker/                 # Docker build files
│   ├── Dockerfile          #   Multi-stage build (compile ldmud + runtime)
│   └── docker-compose.yml  #   Service definition, ports, volumes
├── docs/                   # Project documentation and maps
│   ├── world-map.png       #   Visual world map
│   ├── world-map.txt       #   ASCII world map
│   └── room-connectivity.txt  # Room connection graph
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
| `--debug-file` | `/mud/log/debug.log` | Driver debug log location |
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

## Becoming a Wizard

To gain wizard privileges, log out and edit your player save file:

1. Find your save file at `mudlib/players/<yourname>.o`
2. Set the `level` field to 25 or higher
3. Log back in — you now have wizard commands

Wizard mode grants access to in-game building commands, object manipulation, and LPC evaluation.

## World Map

The `docs/` directory contains world map documentation:

- **`world-map.png`** — Visual map of the game world (~200 rooms across 15+ regions)
- **`world-map.txt`** — ASCII representation of the map
- **`room-connectivity.txt`** — Complete room connection graph

## Development

### Live Editing

The mudlib is mounted as a Docker volume (`docker-compose.yml`), so changes to files in `mudlib/` are reflected immediately in the running server. Most LPC objects can be reloaded in-game without a restart using wizard commands.

### Map Generation

The world map is auto-generated by `scripts/generate-map.py`, which parses all room `.c` files in the mudlib to discover rooms and connections, then renders a PNG using [Pillow](https://pillow.readthedocs.io/) and [NetworkX](https://networkx.org/) for layout.

**Using Docker (recommended):**

```bash
docker compose -f docker/docker-compose.yml run --rm tools
```

**Running locally:**

```bash
pip3 install -r scripts/requirements.txt
python3 scripts/generate-map.py
```

Output is saved to `docs/world-map.png`, `docs/room-connectivity.txt`, and `docs/world-map.txt`. When rooms are added or modified in the mudlib, re-run the script to update the map automatically.

**Automated regeneration:**

A wrapper script `scripts/regen-map.sh` only regenerates the map when room files have changed. Add it to cron for nightly updates:

```bash
crontab -e
# Add this line:
0 2 * * * /path/to/lpmud/scripts/regen-map.sh >> /tmp/map-gen.log 2>&1
```

## Design Decisions

- **ldmud over FluffOS**: FluffOS (MudOS lineage) was considered but rejected — it has no compat mode and would require a full mudlib rewrite to work with 2.4.5. ldmud is the direct descendant of the original LPmud driver and supports compat mode natively.
- **Compat mode**: Required for the 2.4.5 mudlib. Modern ldmud defaults to "native" mode with different semantics.
- **`transfer()` warnings**: The 2.4.5 mudlib uses the deprecated `transfer()` efun extensively. These warnings are non-fatal and can be addressed later by creating a `simul_efun` wrapper.
- **Multi-stage Docker build**: Keeps the runtime image small (~50MB) by separating build dependencies from the final image.
- **Volume-mounted mudlib**: Allows live editing during development without rebuilding the container.

## References

- [ldmud](https://github.com/ldmud/ldmud) — The game driver (direct descendant of original LPmud)
- [ldmud Documentation](https://www.ldmud.eu/) — Official ldmud docs
- [LPMud on Wikipedia](https://en.wikipedia.org/wiki/LPMud) — History and background
- [MUD on Encyclopedia MDPI](https://encyclopedia.pub/entry/31155) — Academic overview
- [FluffOS](https://www.fluffos.info/) — Alternative MUD driver (MudOS lineage)
- [LPmuds.net Downloads](http://lpmuds.net/downloads.html) — MUD resources and archives
- [Intermud Protocol](http://lpmuds.net/intermud.html) — Inter-MUD communication
- [CD Gamedriver](https://github.com/cotillion/cd-gamedriver) — Another LPmud driver fork

## Status

Server builds and runs successfully. The 2.4.5 mudlib loads with some `transfer()` deprecation warnings (non-fatal). The game world is explorable and functional.
