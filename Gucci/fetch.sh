#!/bin/bash

echo -ne ${bold}OS:${normal}  && lsb_release -ds
echo -ne ${bold}KR:${normal}  && uname -r
echo -ne ${bold}UP:${normal}  && uptime -p
echo -ne ${bold}SH:${normal}  && echo $SHELL
echo -ne ${bold}RS:${normal}  && xdpyinfo | awk '/dimensions/{print $2}'

f=3 b=4 g=9
for j in f b g; do
  for i in {0..16}; do
    printf -v $j$i %b "\e[${!j}${i}m"
  done
done
d=$'\e[1m'
t=$'\e[0m'
v=$'\e[7m'
 
 
cat << EOF
 
 $f1███$d$g1▄$t  $f2███$d$g2▄$t  $f3███$d$g3▄$t  $f4███$d$g4▄$t  $f5███$d$g5▄$t  $f6███$d$g6▄$t  $f7███$d$g7▄$t  
 $f1███$d$g1█$t  $f2███$d$g2█$t  $f3███$d$g3█$t  $f4███$d$g4█$t  $f5███$d$g5█$t  $f6███$d$g6█$t  $f7███$d$g7█$t  
 $f1███$d$g1█$t  $f2███$d$g2█$t  $f3███$d$g3█$t  $f4███$d$g4█$t  $f5███$d$g5█$t  $f6███$d$g6█$t  $f7███$d$g7█$t  
 $d$g1 ▀▀▀   $g2▀▀▀   $g3▀▀▀   $g4▀▀▀   $g5▀▀▀   $g6▀▀▀   $g7▀▀▀  
EOF