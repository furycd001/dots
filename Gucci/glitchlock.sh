#!/usr/bin/env bash
# Glitch i3lock....

tmpbg="/tmp/screen.png"
scrot "$tmpbg"; /home/furycd001/Apps/corrupter/corrupter "$tmpbg" "$tmpbg"
i3lock -i "$tmpbg" --insidecolor=272B35 --ringcolor=E0218A --line-uses-inside \
--keyhlcolor=272B35 --bshlcolor=272B35 --separatorcolor=272B35 \
--insidevercolor=272B35 --insidewrongcolor=d23c3dff \
--ringvercolor=272B35 --ringwrongcolor=C13B3A --indpos="x+86:y+1003" \
--radius=15 --veriftext="" --wrongtext="" ; rm "$tmpbg" & \
systemctl suspend & \
