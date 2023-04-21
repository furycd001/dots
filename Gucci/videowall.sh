#!/usr/bin/env bash
#set video as wallpaper using xwinwrap and mpv - change path to your video!! Do not use my default settings
# Using pgrep and pkill, we can check to see if MPV is running and, if so, kill it or start it.
# There's probably a better way of doing this because you could maybe have more than one instance of MPV open and this way would kill them all.
# It works though....

 if pgrep mpv; then
    pkill mpv; else
    xwinwrap -ni -fs -s -st -sp -b -nf -- mpv --profile=wallpaper -wid %WID /home/furycd001/Terminal/
  fi