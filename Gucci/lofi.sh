#!/usr/bin/env bash

# Lo-fi video wallpaper

sleep 4s && \
xwinwrap -ni -fs -s -st -sp -b -nf -- mpv --profile=wallpaper -wid WID https://www.youtube.com/watch?v=5qap5aO4i9A & \
mpv --no-video https://www.youtube.com/watch?v=5qap5aO4i9A
