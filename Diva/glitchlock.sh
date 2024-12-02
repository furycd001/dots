#!/usr/bin/env bash
# Glitch i3lock....

scrot "/tmp/bg.png"; corrupter "/tmp/bg.png" "/tmp/bg1.png"
i3lock -i "/tmp/bg1.png" ; rm "/tmp/bg.png" "/tmp/bg1.png" & \
systemctl suspend & \
