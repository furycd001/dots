#!/usr/bin/env bash

# A script for copying files listed in a playlist to a folder in Downloads....

playlistfile=$1
copylocation=$2


if [ $# -eq 0 ]; then
    >&2 echo "filename.m3u & /home/furycd001/Downloads/foldername"
    exit 1
fi


mkdir -p $copylocation
while read fname; do cp "$fname" $copylocation; done < $playlistfile &&
echo FINISHED !!