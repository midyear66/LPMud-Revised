#!/bin/bash
# Build the LPmud Docker container
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# Extract mudlib if not already done
"$SCRIPT_DIR/extract-mudlib.sh"

echo "Building Docker image..."
docker compose -f "$PROJECT_DIR/docker/docker-compose.yml" build

echo ""
echo "Build complete. Start the server with:"
echo "  docker compose -f docker/docker-compose.yml up"
echo ""
echo "Connect with:"
echo "  telnet localhost 4000"
