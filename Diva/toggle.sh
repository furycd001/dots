#!/usr/bin/env bash

if pgrep conky && pgrep xfce4-panel; then
    pkill conky
    pkill xfce4-panel
else
    conky -c '/home/furycd001/Dots/conky/mnml.conf' &
    xfce4-panel &
fi