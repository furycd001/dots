#!/usr/bin/env bash

# Function to display the notification
show_notification() {
    local workspace_name=$(wmctrl -d | grep '*' | sed 's/.* //g')
    notify-send "$workspace_name"
}

# Monitor for workspace changes and display notification
previous_ws=$(wmctrl -d | grep '*' | sed 's/.* //g')
while true; do
    current_ws=$(wmctrl -d | grep '*' | sed 's/.* //g')
    if [[ "$current_ws" != "$previous_ws" ]]; then
        show_notification
        previous_ws="$current_ws"
    fi
    sleep 1
done