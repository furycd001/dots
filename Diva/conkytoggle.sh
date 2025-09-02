#!/usr/bin/env bash

  if pgrep conky; then
    pkill conky; else
    conky -c '/home/furycd001/Dots/conky/made.conf';
  fi


# conky -c '/home/furycd001/Dots/conky/mnml.conf';