#!/usr/bin/env bash

mkdir -p output_frames

for file in *.mp4; do
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    mkdir -p "output_frames/$filename"
    ffmpeg -i "$file" "output_frames/$filename/image_%04d.png"
done