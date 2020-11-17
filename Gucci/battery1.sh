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
        feh --randomize /usr/share/backgrounds/Battery/*.jpg &
        PID=$!
        sleep 0.8
        WINID=$(wmctrl -lp|grep $PID|cut -d' ' -sf1)
        wmctrl -ir $WINID -b toggle,above
      fi
    sleep 2m
done
