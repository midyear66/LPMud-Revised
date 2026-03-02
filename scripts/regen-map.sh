#!/bin/bash
# Regenerate the world map only if room files have changed since the last run.
# Intended to be called from cron, e.g.:
#   0 2 * * * /path/to/lpmud/scripts/regen-map.sh >> /tmp/map-gen.log 2>&1

set -euo pipefail

REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MAP_FILE="$REPO_DIR/docs/world-map.png"
ROOM_DIR="$REPO_DIR/mudlib/room"

# Skip if no room files are newer than the current map
if [ -f "$MAP_FILE" ] && ! find "$ROOM_DIR" -name "*.c" -newer "$MAP_FILE" | grep -q .; then
    echo "$(date): No room changes detected, skipping."
    exit 0
fi

echo "$(date): Room changes detected, regenerating map..."
docker compose -f "$REPO_DIR/docker/docker-compose.yml" run --rm tools
echo "$(date): Done."
