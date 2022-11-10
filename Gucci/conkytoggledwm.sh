#!/usr/bin/env bash

  if pgrep conky; then
    pkill conky; else
    wmctrl -r :ACTIVE: -b toggle,above && conky -c '/home/furycd001/Dots/conky/external-dwm.conf';
  fi
