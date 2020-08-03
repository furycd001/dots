#!/bin/bash

sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y && \
sudo dnf install -y dnf-plugin-system-upgrade akmod-nvidia xorg-x11-drv-nvidia-cuda @base-x xorg-x11-server-Xorg xorg-x11-drv-{evdev,intel,synaptics,libinput} xorg-x11-xinit gvfs-fuse gvfs-backends make automake gcc gcc-c++ kernel-devel libX11-devel libXft-devel libXinerama-devel tlp tlp-rdw thermald cups pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils kernel-modules-extra libXrandr libXrandr-devel htop wget curl thunar pavucontrol xbacklight powertop udiskie ffmpeg nano vim ncmpcpp libmpd volumeicon dunst w3m w3m-img feh lxappearance firefox conky mpv unzip gimp rsync leafpad steam compton transmission darkplaces quake3 xclip evince asunder easytag simple-scan NetworkManager network-manager-applet nm-connection-editor NetworkManager-wifi VirtualBox virtualbox-guest-additions maim file-roller && \
mkdir /home/furycd001/.src/ && cd /home/furycd001/.src/ && \
wget https://dl.suckless.org/dwm/dwm-6.2.tar.gz && \
wget https://dl.suckless.org/st/st-0.8.3.tar.gz && \
wget https://dl.suckless.org/tools/dmenu-4.9.tar.gz && \
wget https://dl.suckless.org/tools/slock-1.4.tar.gz && \
tar xvzf dwm-6.2.tar.gz && \
tar xvzf st-0.8.3.tar.gz && \
tar xvzf dmenu-4.9.tar.gz && \
tar xvzf slock-1.4.tar.gz && \
cd dwm-6.2 && make && sudo make install && cd .. &&
cd st-0.8.3 && make && sudo make install && cd .. && \
cd dmenu-4.9 && make && sudo make install && cd .. && \
cd slock-1.4 && make && sudo make install && cd .. && \
cd /etc/conky/ && \
wget https://raw.githubusercontent.com/furycd001/dots/master/conky/output-bar.conf && \
mv output-bar.conf conky.conf && \
cd /home/furycd001/ && \
mkdir Pictures/ Documents/ Downloads/ Sites/ Music/ Videos/ Terminal/ && \
wget https://i.imgur.com/nIP3YDW.jpg && \
cp nIP3YDW.jpg Coffee.jpg && \
cd /home/furycd001/ && \
feh --bg-fill '/home/furycd001/Pictures/Coffee.jpg' && \
touch ~/.xinitrc && \
echo ~/.fehbg & >> ~/.xinitrc && \
echo (conky | while read LINE; do xsetroot -name "$LINE"; done) & >> ~/.xinitrc && \
echo 'exec dwm' >> ~/.xinitrc && \
systemctl enable cups && systemctl enable tlp && systemctl enable thermald && \
reboot
