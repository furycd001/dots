#!/bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

colors=($(xrdb -query | sed -n 's/.*color\([0-9]\)/\1/p' | sort -nu | cut -f2))

f=3 b=4
for j in f b; do
    for i in {0..7}; do
        printf -v $j$i %b "\e[${!j}${i}m"
    done
done
t=$'\e[0m'


# output

cat << EOF

$f0▀▀▀▀▀▀▀$t   $f1▀▀▀▀▀▀▀$t   $f2▀▀▀▀▀▀▀$t   $f3▀▀▀▀▀▀▀$t   $f4▀▀▀▀▀▀▀$t   $f5▀▀▀▀▀▀▀$t   $f6▀▀▀▀▀▀▀$t   $f7▀▀▀▀▀▀▀$t
EOF
for i in {0..7}; do echo -en "\e[$((30+$i))m${colors[i]} \e[0m  "; done
echo
echo

echo -ne ${bold}OS:${normal}  && lsb_release -ds
echo -ne ${bold}KR:${normal}  && uname -r
echo -ne ${bold}UP:${normal}  && uptime -p
echo -ne ${bold}SH:${normal}  && echo $SHELL
echo -ne ${bold}RS:${normal}  && xdpyinfo | awk '/dimensions/{print $2}'
