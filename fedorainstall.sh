#!/bin/bash

sudo dnf update --refresh -y && sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y && sudo dnf install xorg-x11-drivers xorg-x11-xinit xorg-x11-drv-nvidia-340xx akmod-nvidia-340xx xorg-x11-drv-nvidia-340xx-cuda xorg-x11-drv-fbdev xorg-x11-drv-synaptics pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils -y && sudo dnf update -y && sudo dnf install gstreamer pavucontrol xbacklight gvfs udiskie ffmpeg nano rxvt ncmpcpp libmpd dunst i3 i3status i3lock dmenu feh lxappearance mpv deja-dup gimp pidgin leafpad steam compton scrot transmission darkplaces quake3 xclip evince asunder easytag simple-scan network-manager nm-connection-editor VirtualBox virtualbox-guest-additions file-roller ubuntu-title-fonts adobe-source-code-pro-fonts terminus-fonts terminus-fonts-console -y && sudo dnf install lpf-spotify-client && lpf approve spotify-client && sudo -u pkg-build lpf build spotify-client && sudo dnf install /var/lib/lpf/rpms/spotify-client/spotify-client-*.rpm && sudo dnf install cups && systemctl start cups && echo 'exec i3' >> ~/.xinitrc && reboot
