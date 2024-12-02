#!/usr/bin/env bash
# Display number of days since Arch was last updated....

day=$((($(date +%s) - $(date -d $(sed -n '/upgrade$/x;${x;s/.\([0-9-]*\).*/\1/p}' /var/log/pacman.log) +%s)) / 86400)) 
printf '%d days\n' $day