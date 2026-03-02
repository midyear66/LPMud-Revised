#!/bin/bash
# Fetch the ldmud lp-245 mudlib into the mudlib/ directory
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
DEST="$PROJECT_DIR/mudlib"

if [ "$(ls -A "$DEST" 2>/dev/null)" ]; then
    echo "mudlib/ directory is not empty. Skipping fetch."
    echo "To re-fetch, run: rm -rf $DEST/* && $0"
    exit 0
fi

echo "Fetching ldmud lp-245 mudlib..."
TMPDIR=$(mktemp -d)
git clone --depth 1 --filter=blob:none --sparse \
    https://github.com/ldmud/ldmud.git "$TMPDIR/ldmud" 2>&1
cd "$TMPDIR/ldmud"
git sparse-checkout set mud/lp-245 2>&1
cp -R mud/lp-245/* "$DEST/"
rm -rf "$TMPDIR"
echo "Done. Mudlib installed to $DEST/"
