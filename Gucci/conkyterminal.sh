#!/usr/bin/env bash

xdotool getwindowfocus windowmove 2948 22
clear && \
watch -t -n 1 'conky -q -c /home/furycd001/Dots/conky/terminal.conf' && \
echo  && \
exit 0;
