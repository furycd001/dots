#!/usr/bin/env bash

# Set the output filename
output_filename="combined.jpg"

# Initialize the output image with dimensions of the first image
first_img=$(find . -maxdepth 1 -type f -name '*.jpg' -print -quit)
if [ -z "$first_img" ]; then
    echo "No JPG images found in the current directory."
    exit 1
fi

img_dimensions=$(identify -format "%wx%h" "$first_img")

# Create a white canvas of the same dimensions as the first image
convert -size "$img_dimensions" xc:white "$output_filename"

# Loop through all images in the current directory
for img in *.jpg; do
    if [ "$img" != "$output_filename" ]; then
        composite -blend 50% "$img" "$output_filename" "$output_filename"
    fi
done