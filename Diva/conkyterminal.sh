#!/usr/bin/env bash

clear && \
tput civis && \
watch -t -n 1 'conky -q -c /home/furycd001/Dots/conky/terminal.conf' && \
exit 0;
