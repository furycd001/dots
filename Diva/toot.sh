#!/usr/bin/env bash

INSTANCE="https://fosstodon.org"  # Replace with your Mastodon instance URL

# Function to fetch and display the timeline
function display_timeline {
    # Fetch the public timeline using curl and the Mastodon API
    toot_timeline=$(curl -s "$INSTANCE/api/v1/timelines/public" | jq -r '.[] | .content? | gsub("<[^>]*>"; "")' | head -n 5)

    # Loop through the toot texts and display them
    while IFS= read -r toot_text; do
        # If the toot text is empty or only contains spaces, continue to the next toot
        if [ -z "${toot_text// }" ]; then
            continue
        fi

        # Print the cleaned text-only toot content with a blank line after each toot
        echo "$toot_text"
        echo
    done <<< "$toot_timeline"
}

# Display the timeline initially
display_timeline

# Loop to check for new toots
while true; do
    sleep 60    # Wait for 60 seconds before fetching the timeline again

    clear       # Clear the terminal screen

    # Display the timeline and check for new toots
    display_timeline
done
