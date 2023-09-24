#!/usr/bin/env bash

# Specify the directory containing your MP3 files
mp3_directory="/path/to/your/mp3/files"

# Loop through all MP3 files in the directory
for mp3_file in "$mp3_directory"/*.mp3; do
    # Extract filename without extension (to use as title, for example)
    filename_without_extension=$(basename -- "$mp3_file" .mp3)

    # Set the metadata tags using id3v2 (modify as needed)
    id3v2 --song "$filename_without_extension" --artist "Artist Name" --album "Album Name" "$mp3_file"

    # Check if album.jpg exists in the same directory
    album_art_file="$mp3_directory/album.jpg"
    if [ -f "$album_art_file" ]; then
        # Embed album art into the MP3 file
        eyeD3 --add-image="$album_art_file":FRONT_COVER "$mp3_file"
    fi
done