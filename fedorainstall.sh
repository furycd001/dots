#!/bin/bash

sudo dnf update --refresh -y && sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm && sudo dnf install xorg-x11-drv-nvidia-340xx akmod-nvidia-340xx -y && sudo dnf install xorg-x11-drv-nvidia-340xx-cuda -y && sudo dnf update -y && sudo dnf install pidgin rxvt ncmpcpp libmpd leafpad i3 i3status i3lock feh deja-dup lxappearance pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils mpv ffmpeg pavucontrol ubuntu-title-fonts adobe-source-code-pro-fonts terminus-fonts terminus-fonts-console gvfs -y
