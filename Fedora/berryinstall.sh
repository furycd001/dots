#!/bin/bash

sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y && \
sudo dnf install -y dnf-plugin-system-upgrade akmod-nvidia xorg-x11-drv-nvidia-cuda @base-x xorg-x11-server-Xorg xorg-x11-drv-{evdev,intel,synaptics,libinput} xorg-x11-xinit gvfs-fuse gvfs-backends make automake gcc gcc-c++ kernel-devel libX11-devel libXft-devel libXinerama-devel tlp tlp-rdw thermald cups pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils kernel-modules-extra libXrandr libXrandr-devel htop wget curl thunar pavucontrol xbacklight powertop udiskie ffmpeg nano vim ncmpcpp libmpd volumeicon dunst w3m w3m-img feh lxappearance firefox conky mpv unzip gimp rsync leafpad steam compton transmission darkplaces quake3 xclip evince asunder easytag simple-scan NetworkManager network-manager-applet nm-connection-editor NetworkManager-wifi VirtualBox virtualbox-guest-additions maim file-roller && \
cd /etc/conky/ && \
wget https://raw.githubusercontent.com/furycd001/dots/master/conky/output-bar.conf && \
mv output-bar.conf conky.conf && \
cd /home/furycd001/ && \
mkdir -p Pictures/ Documents/ Downloads/ Sites/ Music/ Videos/ Terminal/ .font/ && \
cd Pictures/
wget https://i.imgur.com/nIP3YDW.jpg && \
cp nIP3YDW.jpg Coffee.jpg && rm nIP3YDW.jpg && \
cd /home/furycd001/ && \
feh --bg-fill '/home/Pictures/Coffee.jpg' && \
touch ~/.xinitrc && \
echo ~/.fehbg & >> ~/.xinitrc && \
echo conky & ~/.xinitrc && \
echo 'exec berry' >> ~/.xinitrc && \
cd .fonts && \
wget https://assets.ubuntu.com/v1/fad7939b-ubuntu-font-family-0.83.zip && \
unzip fad7939b-ubuntu-font-family-0.83.zip && \
cp -rvf ubuntu-font-family-0.83 .fonts/ && \
cd .. && rm *.zip && cd && \
systemctl enable cups && systemctl enable tlp && systemctl enable thermald && \
mkdir /home/furycd001/.src/ && cd /home/furycd001/.src/ && \
git clone https://github.com/JLErvin/berry && cd berry && make && sudo make install && \
mkdir /home/furycd001/.config/berry && \
cp examples/sxhkdrc /home/furycd001/.config/berry/sxhkdrc && \
cp examples/autostart /home/furycd001/.config/berry/autostart && /
cd /usr/share/xsessions && sudo wget https://raw.githubusercontent.com/furycd001/dots/master/Fedora/berry.desktop && cd && \
reboot
