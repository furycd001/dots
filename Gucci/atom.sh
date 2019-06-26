#!/usr/bin/env bash
# Run the command and fork it into the background, after grabbing its PID
atom /home/furycd001/Sites/Lacey\ Laney/Markdown.md

# Poll until the command spawned a window, then get its window ID
for ((;;)); {
    wIDs=$(xdotool search --onlyvisible --name Atom 2> /dev/null) && break
}

# Resize known windows
for wID in $wIDs; {
    xdotool windowsize $wID 800 400
}

# Moves known windows
for wID in $wIDs; {
    xdotool windowmove $wID 1090 480
}
