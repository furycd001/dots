#!/usr/bin/env bash

# Set the URL of the webpage you want to monitor
URL="yoururlgoeshere"

# Set the path for the snapshots
SNAPSHOT_DIR="$HOME/webpage_monitor"
INITIAL_SNAPSHOT="$SNAPSHOT_DIR/initial_snapshot.html"
NEW_SNAPSHOT="$SNAPSHOT_DIR/new_snapshot.html"

# Create the snapshot directory if it doesn't exist
mkdir -p "$SNAPSHOT_DIR"

# Check if the initial snapshot exists; if not, create one
if [ ! -e "$INITIAL_SNAPSHOT" ]; then
    curl -s "$URL" > "$INITIAL_SNAPSHOT"
fi

# Create a new snapshot
curl -s "$URL" > "$NEW_SNAPSHOT"

# Compare the snapshots
if diff "$INITIAL_SNAPSHOT" "$NEW_SNAPSHOT" > /dev/null; then
    echo "No changes detected."
else
    echo "Changes detected on the webpage!"
    
    # You can add your notification method here, e.g., sending an email, using notify-send, etc.
    # Example: notify-send "Webpage update" "Changes detected on $URL"
    
    # Update the initial snapshot
    mv "$NEW_SNAPSHOT" "$INITIAL_SNAPSHOT"
fi
