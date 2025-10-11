#!/usr/bin/env bash

# Launch Sublime Text in the background
subl &

# Wait until the Sublime Text window appears
while true; do
    wIDs=$(xdotool search --onlyvisible --name "Sublime Text" 2>/dev/null)
    if [[ -n "$wIDs" ]]; then
        break
    fi
    sleep 0.2
done

# Resize and move each found window
for wID in $wIDs; do
    # Resize window
    xdotool windowsize "$wID" 1007 1140

    # Moves known window
    xdotool windowmove "$wID" 32 32
done
