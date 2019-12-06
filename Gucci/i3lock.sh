#!/bin/bash

icon="$HOME/.config/i3/lock.png"
img="$HOME/.cache/i3lock.png"

# Take a screenshot for our background
scrot $img
# Pixelate the background
convert $img -scale 10% -scale 1000% $img
# Add the lock-icon
convert $img $icon -gravity center -composite $img
# Finally run i3lock itself
i3lock -u -i $img
