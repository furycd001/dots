#!/usr/bin/env bash

echo Documents && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Documents/ /home/furycd001/Documents/ && \
echo Emulation && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Emulation/ /home/furycd001/Emulation/ && \
echo Dots && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Dots/ /home/furycd001/Dots/ && \
echo Downloads && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Downloads/ /home/furycd001/Downloads/ && \
echo Gateway && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Gateway/ /home/furycd001/Gateway/ && \
echo Markdown && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Markdown/ /home/furycd001/Markdown/ && \
echo Music && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Music/ /home/furycd001/Music/ && \
echo Pictures && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Pictures/ /home/furycd001/Pictures/ && \
echo Sites && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Sites/ /home/furycd001/Sites/ && \
echo Steam && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Steam/ /home/furycd001/Steam/ && \
echo Videos && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/Videos/ /home/furycd001/Videos/ && \
echo VirtualBox && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/VirtualBox\ VMs/ /home/furycd001/VirtualBox\ VMs/ && \
echo .fonts && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.fonts/ /home/furycd001/.fonts/ && \
echo .mozilla && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.mozilla/ /home/furycd001/.mozilla/ && \
echo .mpd && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.mpd/ /home/furycd001/.mpd/ && \
echo .ncmpcpp && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.ncmpcpp/ /home/furycd001/.ncmpcpp/ && \
echo .mpv && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.config/mpv/ /home/furycd001/.config/mpv/ && \
echo .config/gtk-3.0 && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.config/gtk-3.0/ /home/furycd001/.config/gtk-3.0/ && \
echo .config/htop && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.config/htop/ /home/furycd001/.config/htop/ && \
echo .config/Atom && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.config/Atom/ /home/furycd001/.config/Atom/ && \
echo .atom && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.atom/ /home/furycd001/.atom/ && \
echo /home/furycd001/.newsboat && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.newsboat/ /home/furycd001/.newsboat/ && \
echo /home/furycd001/.purple/ && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/.purple /home/furycd001/.purple/ && \

echo [ PART.2 COPYING BACKUP FILES HAS FINISHED !! ] && \
echo  && \
echo [ PLEASE CONNECT REVIZIIS AND RUN PART.3 (with root) !! ]
