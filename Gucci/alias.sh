#!/usr/bin/env bash

awk '$1 ~ /^[alias]/ {print $0}' ~/.bashrc
