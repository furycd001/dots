#!/usr/bin/env bash
set -x
while true
do
    export DISPLAY=:0.0
    battery=$(acpi -b | grep -P -o '[0-9]+(?=%)')
    echo $battery
  if [[ $battery -eq 100 ]]; then
        notify-send -i "/usr/share/icons/Arc-X-P/panel/22/battery-full-charged.svg" "Battery Charged" "Level: ${battery}%"
      elif [[ $battery -le 23 ]]; then
        yad --no-buttons --center --borders=22 --timeout=22 --title="BATTERY LOW.." --image /usr/share/backgrounds/Battery/batterylow7.jpg
      fi
    sleep 2m
done
