#!/usr/bin/env bash
sleep 4s && \
notify-send -u critical -t 4000 "$(fold -s -w 50 /home/furycd001/Documents/todo.txt)"