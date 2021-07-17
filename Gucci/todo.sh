#!/usr/bin/env bash
sleep 4s && \
notify-send -u critical -t 4000 "$(cat /home/furycd001/Documents/todo.txt)"
