#!/usr/bin/env bash

while [ ! "$term" ]; do
	while IFS=':	' read -r key val; do
		case $key in
			PPid) ppid=$val; break;;
		esac
	done < "/proc/${ppid:-$PPID}/status"

	read -r name < "/proc/$ppid/comm"
	case $name in
		*sh) ;;
		"${0##*/}") ;;
		*[Ll]ogin*|*init*) term=linux;;
		*) term="$name";;
	esac
done

# DE Name

## WM Name
id_bloat=$(xprop -root _NET_SUPPORTING_WM_CHECK)
id=${id_bloat##* }
wm_bloat=$(xprop -id "$id" _NET_WM_NAME)

## Get system info
#user="${USER}@$(hostname)"
distro=$(. /etc/lsb-release ; echo $DISTRIB_DESCRIPTION)
kernel="$(uname -sr | sed 's/-.*//')"
uptime="$(uptime -p | sed 's/up //')"
shell="$(basename ${SHELL})"
packages="$(pacman -Q | wc -l)"
wm="$(echo $wm_bloat | cut -d'"' -f 2)"
de="$(echo $XDG_CURRENT_DESKTOP)"


bold="$(tput bold)"
white="$(tput setaf 7)"
yellow="$(tput setaf 12)"
red="$(tput setaf 1)"
blue="$(tput setaf 4)"
green="$(tput setaf 2)"
orange="$(tput setaf 8)"
violet="$(tput setaf 5)"
cyan="$(tput setaf 6)"
reset="$(tput sgr0)"
cbg="${reset}${bold}${bgaccent}${white}"
cr="${reset}"
c0="${reset}${bold}"
c1="${reset}${accent}"

colors=($(xrdb -query | sed -n 's/.*color\([0-9]\)/\1/p' | sort -nu | cut -f2))

f=3 b=4
for j in f b; do
    for i in {0..7}; do
        printf -v $j$i %b "\e[${!j}${i}m"
    done
done
t=$'\e[0m'

## Output

clear && \
echo  && \
cat <<EOF
  OS:   ${cr}${white}${distro}${reset}
  KR:   ${cr}${white}${kernel}${reset}
  DE:   ${cr}${white}${de}${reset}
  WM:   ${cr}${white}${wm}${reset}
  TM:   ${cr}${white}${term}${reset}
  SH:   ${cr}${white}${shell}${reset}
  PK:   ${cr}${white}${packages}${reset}
  UP:   ${cr}${white}${uptime}${reset}

  $f0■■■■$t $f1■■■■$t $f2■■■■$t $f3■■■■$t
  $f4■■■■$t $f5■■■■$t $f6■■■■$t $f7■■■■$t
EOF
echo  &&\
echo
