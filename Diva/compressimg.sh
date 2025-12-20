#!/usr/bin/env bash

OUTPUT_DIR="compressed_images"
QUALITY=90
THREADS=$(nproc)

echo "Starting batch compression using $THREADS cores..."
mkdir -p "$OUTPUT_DIR"

find . -maxdepth 1 -type f -regextype posix-extended \
    -iregex '.*\.(jpg|jpeg|png|webp)' -print0 | \
    xargs -0 -P "$THREADS" -I {} \
    magick "{}" -quality "$QUALITY" "$OUTPUT_DIR/{}"

COUNT=$(find "$OUTPUT_DIR" -type f | wc -l)
echo "-------------------------------------------"
echo "Done! $COUNT images saved to: $OUTPUT_DIR"