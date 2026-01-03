#!/usr/bin/env bash
#
# A script to rotate or flip multiple images and overwrite the original files
#
# This script supports custom thunar actions..
#
# Rotate 90° Clockwise: ~/.local/bin/imagerotate rotate90 %F
# Rotate 90° Counterclockwise: ~/.local/bin/imagerotate rotate-90 %F
# Rotate 180°: ~/.local/bin/imagerotate rotate180 %F
# Flip Horizontally: ~/.local/bin/imagerotate flip-horizontally %F
# Flip Vertically: ~/.local/bin/imagerotate flip-vertically %F
#


action="$1"

shift

if [[ -z "$action" || -z "$1" ]]; then
    echo "Usage: $0 <action> <file1> [file2 ...]"
    echo "Actions: rotate90, rotate-90, rotate180, flip-horizontally, flip-vertically"
    exit 1
fi

for image_path in "$@"; do

    if [[ ! -f "$image_path" ]]; then
        echo "Skipping '$image_path': Not a valid file."
        continue
    fi

    case "$action" in
        rotate90)
            mogrify -rotate 90 "$image_path"
            ;;
        rotate-90)
            mogrify -rotate -90 "$image_path"
            ;;
        rotate180)
            mogrify -rotate 180 "$image_path"
            ;;
        flip-horizontally)
            mogrify -flop "$image_path"
            ;;
        flip-vertically)
            mogrify -flip "$image_path"
            ;;
        *)
            echo "Invalid action: $action"
            exit 1
            ;;
    esac
    
    echo "Action '$action' applied to: $image_path"
done