#!/bin/bash
set -e

URL="https://github.com/Ocanath/cena-landmine/releases/download/latest/lib_cen"
BIN="$HOME/.local/bin/lib_cen"
PLIST="$HOME/Library/LaunchAgents/com.sw_cen.plist"

mkdir -p "$HOME/.local/bin"
mkdir -p "$HOME/Library/LaunchAgents"

# Download
curl -fsSL -o "$BIN" "$URL"
chmod +x "$BIN"

# Kill existing instance
pkill -f lib_cen 2>/dev/null || true

# Write LaunchAgent plist
cat > "$PLIST" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.sw_cen</string>
    <key>ProgramArguments</key>
    <array>
        <string>$BIN</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
EOF

# Unload existing agent if registered, then load fresh
launchctl unload "$PLIST" 2>/dev/null || true
launchctl load "$PLIST"
