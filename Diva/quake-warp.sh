#!/usr/bin/env bash

# Set the path to the Warp executable
WARP_EXECUTABLE="warp-terminal"

# Set the desired height of the dropdown terminal
TERMINAL_HEIGHT="44%"  # You can adjust this as needed

# Check if Warp is running
if pgrep -x "$(basename "$WARP_EXECUTABLE")" >/dev/null; then
    # If Warp is running, kill it
    pkill -x "$(basename "$WARP_EXECUTABLE")"
else
    # If Warp is not running, start it as a dropdown terminal
    # Calculate the screen height based on the desired percentage
    SCREEN_HEIGHT=$(xdpyinfo | awk '/dimensions:/ {print $2}' | cut -d 'x' -f 2)
    TERMINAL_Y=$((SCREEN_HEIGHT * (100 - ${TERMINAL_HEIGHT%\%}) / 100))

    # Start Warp with the dropdown effect
    "$WARP_EXECUTABLE" --height "$TERMINAL_HEIGHT" --position "+0+$TERMINAL_Y" &

    # Uncomment the line below if you want to focus the dropdown terminal when toggled
    # wmctrl -r "Warp Terminal" -b add,above
fi