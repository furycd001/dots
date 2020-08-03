cd /home/furycd001/ && mkdir /Documents /Dots /Downloads /Music /Pictures /Public /Sites /Videos /Terminal /.src && \
mkdir /etc/conky/ && \
sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm && \
sudo dnf install akmod-nvidia xorg-x11-drv-nvidia-cuda @base-x xorg-x11-server-Xorg xorg-x11-drv-{evdev,intel,synaptics,libinput} xorg-x11-xinit gvfs-fuse gvfs-backends make automake gcc gcc-c++ kernel-devel libX11-devel libXft-devel libXinerama-devel tlp tlp-rdw thermald git cups pulseaudio alsa-plugins-pulseaudio pulseaudio-module-x11 pulseaudio-utils kernel-modules-extra libXrandr libXrandr-deve htop thunar pavucontrol xbacklight powertop udiskie ffmpeg nano ncmpcpp libmpd volumeicon dunst w3m w3m-img feh lxappearance firefox conky mpv unzip gimp rsync leafpad steam compton transmission darkplaces quake3 xclip evince asunder easytag simple-scan NetworkManager network-manager-applet nm-connection-editor NetworkManager-wifi VirtualBox virtualbox-guest-additions file-roller && \
cd /etc/conky/ && sudo wget https://raw.githubusercontent.com/furycd001/dots/master/conky/output-bar.conf && \
cd &&  cd /.src && \
git clone https://github.com/JLErvin/berry && cd /berry && make && sudo make install && \
cd /usr/share/xsessions && sudo wget https://raw.githubusercontent.com/furycd001/dots/master/fedora/berry.desktop && \
cd /home/furycd001/Pictures && \
wget https://i.imgur.com/MXo2UpZ.jpg && mv MXo2UpZ.jpg tsq.jpg && rm MXo2UpZ.jpg && \
feh --bg-fill /home/furycd001/Pictures/tsq.jpg && \
echo ~/.fehbg & >> .xinitrc && \
echo conky & >> .xinitrc && \
echo exec berry >> .xinitrc && \
echo INSTALL HAS FINISHED. PLEASE REBOOT NOW !!!!
