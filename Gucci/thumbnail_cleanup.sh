#!/bin/bash

# Define the path to the thumbnail cache directory
cache_dir="$HOME/.cache/thumbnails/"

# Define the maximum age of files to keep (in days)
max_age=182

# Use the find command to locate and delete old cache files
find "$cache_dir" -type f -mtime +$max_age -delete

echo "Old thumbnail cache deleted."