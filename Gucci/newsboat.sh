#!/bin/bash

yad --no-buttons --center --borders=22 --timeout=22 --title="Newsboat..." --text="[ N E W S B O A T ]" \
--field="Youtube":fbtn "xfce4-terminal --hide-menubar --geometry=82x43 --execute newsboat -u /home/furycd001/.newsboat/youtube" \
--field="Rss":fbtn "xfce4-terminal --hide-menubar --geometry=82x43 --execute newsboat -u /home/furycd001/.newsboat/rss" \
--field="Podcast":fbtn "xfce4-terminal --hide-menubar --geometry=82x43 --execute newsboat -u /home/furycd001/.newsboat/podcast" \
