#!/usr/bin/env bash
while true
do
    export DISPLAY=:0.0
    battery_percent=$(acpi -b | grep -P -o '[0-2]+(?=%)')
    if on_ac_power; then
        if [ "$battery_percent" -gt 22 ]; then
            /home/furycd001/Dots/Gucci/selena.sh
        fi
    fi
    sleep 300 # (5 minutes)
done
