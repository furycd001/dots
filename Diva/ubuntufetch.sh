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
packages="$(dpkg --list | wc --lines)"
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

## Output

clear && \
echo  && \
cat <<EOF
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} OS: ${reset}${c0}${blue}  ${cr}${white}${distro}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} KR: ${reset}${c0}${blue}  ${cr}${white}${kernel}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} DE: ${reset}${c0}${blue}  ${cr}${white}${de}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} WM: ${reset}${c0}${blue}  ${cr}${white}${wm}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} TM: ${reset}${c0}${blue}  ${cr}${white}${term}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} SH: ${reset}${c0}${blue}  ${cr}${white}${shell}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} PK: ${reset}${c0}${blue}  ${cr}${white}${packages}${reset}
${c0}${cyan}     FℲ FℲ FℲ FℲ FℲ FℲ    ${c0}${blue}${c0}${yellow} UP: ${reset}${c0}${blue}  ${cr}${white}${uptime}${reset}
EOF
echo && \
echo
