#!/usr/bin/env bash

xdotool getwindowfocus windowmove 1588 22
clear && \
tput civis && \
watch -t -n 1 'conky -q -c /home/furycd001/Dots/conky/terminal.conf' && \
exit 0;
