#!/usr/bin/env bash
# this script helps you do hackerish stuff

if [ "$EUID" -ne 0 ]
then
  echo "Please run as root to be hackerish."
  exit
fi

# turn off globbing
set -f
# split on newlines only for for loops
IFS='
'
for log in $(find /var/log -type f); do
  # only use the log if it's a text file; we _will_ encounter some archived logs
  if [ `file $log | grep -e text | wc -l` -ne 0 ]
  then
    echo $log
    for line in $(cat $log); do
      echo $line
      # sleep for a random duration between 0 and 1/4 seconds to indicate hard hackerish work
      bc -l <<< $(bc <<< "$RANDOM % 10")" / 40" | xargs sleep
    done
  fi
done
