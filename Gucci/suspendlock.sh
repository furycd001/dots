#!/usr/bin/env bash
# Custom suspend
#
# (That 'zsh' up there can be switched to 'bash', or
# pretty much any shell - this doesn't do anything too fancy.)
#
# Dependencies are mostly xfce stuff:
#
#   xbacklight
#   xflock4
#   xfce4-session-logout

# Set how dim we want the screen to go (percentage, out of 100)
dim=4

# Pack up your toys
previous_dimness=$(xbacklight -get)

# Turn down the lights
xbacklight -set $dim

# Lock the door (this requires a password to get back in)
xflock4

# And go to sleep
xfce4-session-logout --suspend

# When we wake up, turn the lights back on
xbacklight -set $previous_dimness
