#!/usr/bin/env bash

for f in *.webp; do
  dwebp ./"$f" -o ./"${f%.webp}.png"
done
