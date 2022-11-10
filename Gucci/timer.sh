#!/usr/bin/env bash

# This is a simple timer written in bash....

for i in $(seq $1 -1 1); do echo -ne "\r$i"; sleep 1; done