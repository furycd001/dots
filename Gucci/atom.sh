#!/usr/bin/env bash
# Run the command and fork it into the background, after grabbing its PID
atom

# Poll until the command spawned a window, then get its window ID
for ((;;)); {
    wIDs=$(xdotool search --onlyvisible --name Atom 2> /dev/null) && break
}

# Resize known windows
for wID in $wIDs; {
    xdotool windowsize $wID 880 600
}

# Moves known windows
for wID in $wIDs; {
    xdotool windowmove $wID 980 80
}
