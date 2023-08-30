#!/usr/bin/env bash

  if pgrep conky; then
    pkill conky; else
    conky -c '/home/furycd001/Dots/conky/mnml.conf';
  fi