#!/usr/bin/env bash

# Find folders that have not been touched within  8 months..
# Moves them to a directory to be reviewed..


mkdir -p /home/furycd001/Downloads/deleteme/; 
find /home/furycd001/Reviziis/ -type d -mtime +244 >  /home/furycd001/Downloads/deleteme/deleteme.txt
while read fname; do mv "$fname" /home/furycd001/Downloads/deleteme/; done < /home/furycd001/Downloads/deleteme/deleteme.txt