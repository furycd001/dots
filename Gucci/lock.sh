#!/usr/bin/env bash

systemctl suspend & \

sleep 4s && \

i3lock -c E5B680 \
    --insidecolor=E5B680 --ringcolor=ffeced --line-uses-inside \
    --keyhlcolor=E5B680 --bshlcolor=E5B680 --separatorcolor=E5B680 \
    --insidevercolor=E5B680 --insidewrongcolor=d23c3dff \
    --ringvercolor=E5B680 --ringwrongcolor=C13B3A --indpos="x+86:y+1003" \
    --radius=15 --veriftext="" --wrongtext="" \
