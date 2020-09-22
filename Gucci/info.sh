#!/usr/bin/env bash

clear &&
echo -e "\e[1mTIME ♥\e[0m"
date '+%X'
echo
echo -e "\e[1mDATE ♥\e[0m"
date '+%A %W %B'
echo
echo -e "\e[1mUPTIME ♥\e[0m"
uptime -p
echo
echo -e "\e[1mBATTERY ♥\e[0m"
echo `upower -i $(upower -e | grep '/battery') | grep --color=never -E percentage|xargs|cut -d' ' -f2|sed s/%//`%


echo
echo -e "\e[1mDISK ♥\e[0m"
echo "U: " `df -h / --output=used,pcent | tail -n+2`
echo "F: " `df -h / --output=avail | tail -n+2`
echo
