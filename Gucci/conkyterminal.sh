#!/usr/bin/env bash

#xdotool getwindowfocus windowmove 1588 22
xdotool getwindowfocus windowmove 2948 22
clear && \
watch -t -n 1 'conky -q -c /home/furycd001/Dots/conky/terminal.conf' && \
exit 0;