#!/usr/bin/env bash
ffmpeg -f x11grab -framerate 25 \
    $(xwininfo | gawk 'match($0, /-geometry ([0-9]+x[0-9]+).([0-9]+).([0-9]+)/, a)\
      { print "-video_size " a[1] " -i +" a[2] "," a[3] }') \
    $(date +%Y-%m-%d_%H-%M_%S).mp4
