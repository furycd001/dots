#!/usr/bin/env bash
while true
do
    export DISPLAY=:0.0
    battery_percent=$(acpi -b | grep -P -o '[0-9]+(?=%)')
    if on_ac_power; then
        if [ "$battery_percent" -gt 98 ]; then
            notify-send -i "/usr/share/icons/Arc/panel/22/battery-full.svg" "Battery Charged" "Level: ${battery_percent}% "
        fi
    fi
    sleep 300 # (5 minutes)
done
