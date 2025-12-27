#!/usr/bin/env bash

OUTPUT_DIR="compressed_images"
QUALITY=90
THREADS=$(nproc)
DONE_LOG="$OUTPUT_DIR/.compressed_list.txt"

mkdir -p "$OUTPUT_DIR"
touch "$DONE_LOG" # Ensure the log exists

# 1. Gather all potential images
ALL_IMAGES=$(find . -maxdepth 1 -type f -regextype posix-extended -iregex '.*\.(jpg|jpeg|png|webp)')
TOTAL_FILES=$(echo "$ALL_IMAGES" | wc -l)

# 2. Filter images using the log (Extremely Fast)
# We use 'grep' to compare the list of files against our log
TO_PROCESS_LIST=$(mktemp)
echo "$ALL_IMAGES" | while read -r img; do
    filename=$(basename "$img")
    if ! grep -qxF "$filename" "$DONE_LOG"; then
        echo "$img" >> "$TO_PROCESS_LIST"
    fi
done

REMAINING_FILES=$(wc -l < "$TO_PROCESS_LIST")

if [ "$REMAINING_FILES" -eq 0 ]; then
    dialog --title "Task Complete" --msgbox "All $TOTAL_FILES images are already in the log." 6 50
    rm "$TO_PROCESS_LIST"
    clear; exit 0
fi

# 3. Define the worker function to update the log
do_convert() {
    img="$1"
    out_dir="$2"
    qual="$3"
    log="$4"
    filename=$(basename "$img")
    
    if magick "$img" -quality "$qual" "$out_dir/$filename"; then
        echo "$filename" >> "$log"
    fi
}
export -f do_convert

# 4. Start Compression in background
cat "$TO_PROCESS_LIST" | xargs -I {} -P "$THREADS" \
    bash -c 'do_convert "$1" "$2" "$3" "$4"' -- {} "$OUTPUT_DIR" "$QUALITY" "$DONE_LOG" &

# 5. The Dialog Loop
(
    while true; do
        # Count progress based on the log file size
        CURRENT_COUNT=$(wc -l < "$DONE_LOG")
        PERCENT=$(( CURRENT_COUNT * 100 / TOTAL_FILES ))
        
        # Get the latest line added to the log
        LATEST_FILE=$(tail -n 1 "$DONE_LOG")
        
        echo "XXX"
        echo "$PERCENT"
        echo "Progress: $CURRENT_COUNT / $TOTAL_FILES"
        echo "Latest: ${LATEST_FILE:-...}"
        echo "XXX"
        
        if [ "$CURRENT_COUNT" -ge "$TOTAL_FILES" ]; then
            sleep 1; break
        fi
        if ! pgrep -f "magick" > /dev/null && [ "$CURRENT_COUNT" -lt "$TOTAL_FILES" ]; then
            # If no magick processes are running, assume it finished or stopped
            break
        fi
        sleep 0.3
    done
) | dialog --title "compressimg.." --gauge "Resuming..." 10 70 0

# 6. Final Cleanup
rm "$TO_PROCESS_LIST"
clear