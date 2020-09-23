#!/usr/bin/env bash

timeout 0.1s  conky -q -c /home/furycd001/Dots/conky/notify.conf > /tmp/notify.txt && \
notify-send -t 4000 "$(cat /tmp/notify.txt)" && \
sleep 4s && \
rm /tmp/notify.txt
