#!/usr/bin/env bash
# This script simulates doing hackerish stuff

# Set IFS to handle filenames with spaces correctly
IFS=$'\n'

# Loop through text files in /var/log directory
for log in /var/log/*.log; do
    # Check if the file is a regular file
    if [ -f "$log" ]; then
        echo "$log"
        # Read each line from the log file
        while IFS= read -r line; do
            echo "$line"
            # Sleep for a random duration between 0 and 0.25 seconds
            sleep "$(bc -l <<< "scale=2; $RANDOM / 100")"
        done < "$log"
    fi
done
