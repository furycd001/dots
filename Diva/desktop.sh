#!/usr/bin/env bash

wmctrl -d | grep '*'  | sed 's/.* //g'
