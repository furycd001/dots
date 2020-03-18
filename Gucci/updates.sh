#!/bin/bash

sudo apt-get -qy update > /dev/null
NUMOFUPDATES=$(sudo aptitude search "~U" | wc -l)
echo Updates: $NUMOFUPDATES
