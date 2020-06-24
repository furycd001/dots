#!/usr/bin/env bash

xfce4-terminal --hide-menubar --geometry=62x46 --execute newsboat &

# Process ID of the process we just launched
PID=$!

# Window ID of the process...pray that there's
# only one window! Otherwise this might break.
# We also need to wait for the process to spawn
# a window.
while [ "$WID" == "" ]; do
        WID=$(wmctrl -lp | grep $PID | cut "-d " -f1)
done
# Set the size and location of the window
# See man wmctrl for more info
wmctrl -i -r $WID -e 0,2790,18,478,734

exit 0
