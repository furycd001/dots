#!/usr/bin/env bash

# Check for usage of sudo or root login..
if [ `whoami` != root ]; then
    echo Please use sudo or login as root....
    exit
fi

# Enable rpmfusion..
dnf install -y https://mirrors.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://mirrors.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm && \

# Enable RPM Sphere..
wget https://github.com/rpmsphere/noarch/raw/master/r/rpmsphere-release-34-1.noarch.rpm && \
rpm -U r_rpmsphere-release-34-1.noarch.rpm && \

# Update system..
dnf upgrade -y && \

# Install apps..
dnf install -y \
feh pidgin gimp ffmpeg mpv newsboat tmux git file-roller xfce4-screenshooter \
libmpd libmpdclient ncmpcpp xwinwrap steam clipman mousepad evince simple-scan \
VirtualBox virtualbox-guest-additions transmission filezilla asunder audacity \
easytag xfce4-genmon-plugin xfce4-clipman-plugin htop retroarch retroarch-assets && \

# Install packages needed by sound and video packages..
dnf groupupdate -y sound-and-video && \

# Install Nvidia stuff..
dnf install -y gcc kernel-headers kernel-devel akmod-nvidia xorg-x11-drv-nvidia xorg-x11-drv-nvidia-libs xorg-x11-drv-nvidia-cuda

# Manually download and install applications..
# Maybe "dnf localinstall"
wget https://atom.io/download/rpm && \
rpm -U atom.x86_64.rpm && \
wget https://www.gitkraken.com/download/linux-rpm && \
rpm -U gitkraken-amd64.tar.gz && \
wget https://cdn.getpublii.com/Publii-0.38.1.rpm && \
rpm -U Publii-0.38.1.rpm && \
wget https://zoom.us/client/latest/zoom_x86_64.rpm && \
rpm -U zoom_x86_64.rpm && \

# Install etcher..
sudo sudo dnf config-manager --add-repo https://balena.io/etcher/static/etcher-rpm.repo && \
sudo dnf install -y balena-etcher-electron && \

# Remove packages..
dnf remove -y \
galculator orage xfburn Geany ristretto parole pragha atril gnumeric xfdashboard && \
dnf autoremove -y && \
dnf clean packages && \

# Create directories in home..
cd /home/furycd001/ && \
mkdir Emulation/ Dots/ Gateway/ Markdown/ Sites/ Steam/ VirtualBox/ Terminal/ .fonts/ Apps/ && \

# Install discord..
cd Apps/ && mkdir RPM/ && cd RPM/ && \
cp /home/furycd001/Dots/Fedora/discord-0.0.14-2.x86_64.rpm /home/furycd001/Apps/RPM/ && \
rpm -U discord-0.0.14.rpm && \
cd .. && \

echo [ PART.1 PACKAGE INSTALLATION HAS FINISHED !! ] && \
echo  && \
echo [ PLEASE CONNECT DUCK AND RUN PART.2 WITHOUT ROOT !! ]
