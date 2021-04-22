#!/usr/bin/env bash

  if xrandr | grep -q 'HDMI-1-1 connected' ; then
      conky -c '/home/furycd001/Dots/conky/external.conf'
    else
      pkill conky;
  fi
