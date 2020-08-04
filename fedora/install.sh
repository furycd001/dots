#!/usr/bin/env bash

sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpmu openbox obconf xorg-x11-drv-nvidia-cuda @base-x xorg-x11-server-Xorg
xorg-x11-drv-{evdev,intel,synaptics,libinput} xorg-x11-xinit gvffs gvfs-fuse make automake gcc gcc-c++ kernel-devel libX11-devel libXft-devel libXinerama-devel tlp
tlp-rdw thermald cups pulseaudio alsa-plugins-pulseaudio
pulseaudio-module-x11 pulseaudio-utils kernel-modules-extra libXrandr htop wget pcmanfm pavucontrol xbacklight
powertop udiskie ffmpeg nano ncmpcpp libmpd dunst w3m
w3m-img feh lxappearance firefox conky mpv unzip gimp
rsync leafpad steam compton transmission darkplaces
quake3 xclip evince asunder easytag simple-scan NetworkManager network-manager-applet nm-connection-editor NetworkManager-wifi file-roller && mkdir Documents/ Dots/ Downloads/ Music/ Pictures/ Public/ Sites/ Terminal/ Videos/ && echo INSTALL FINISHED !! HAVE A NICE DAY....
