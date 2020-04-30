#!/bin/bash

sudo dnf upgrade -y && \
sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y && \
sudo dnf install -y @base-x xorg-x11-server-Xorg xorg-x11-drv-{evdev,intel,synaptics} xorg-x11-xinit xterm terminus-fonts make automake gcc gcc-c++ kernel-devel libX11-devel libXft-devel libXinerama-devel pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils kernel-modules-extra htop wget curl thunar pavucontrol xbacklight powertop gvfs udiskie ffmpeg nano vim ncmpcpp libmpd dunst feh lxappearance firefox conky mpv gimp rsync leafpad steam compton transmission darkplaces quake3 xclip evince asunder easytag simple-scan NetworkManager network-manager-applet nm-connection-editor NetworkManager-wifi VirtualBox virtualbox-guest-additions maim file-roller ubuntu-title-fonts adobe-source-code-pro-fonts terminus-fonts terminus-fonts-console -y && \
mkdir /home/furycd001/.src/ && cd /home/furycd001/.src/ && \
wget https://dl.suckless.org/dwm/dwm-6.2.tar.gz && cd dwm && make && make install && cd .. && \
wget https://dl.suckless.org/st/st-0.8.3.tar.gz && cd st && make && make install && cd .. && \
wget https://dl.suckless.org/tools/dmenu-4.9.tar.gz && cd dmenu && make && make install && cd .. && \
wget https://dl.suckless.org/tools/slock-1.4.tar.gz && cd slock && make && make install && cd .. && \
touch ~/.xinitrc && echo 'exec dwm' >> ~/.xinitrc && \
sudo dnf install lpf-spotify-client -y && \
lpf approve spotify-client -y && \
sudo -u pkg-build lpf build spotify-client && \
sudo dnf install /var/lib/lpf/rpms/spotify-client/spotify-client-*.rpm -y && \
sudo dnf install cups -y && \
systemctl start cups && \
reboot
