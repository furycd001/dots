#!/bin/bash

echo AndroidStudioProjects && \
rsync -aH --delete --info=progress2 /home/furycd001/AndroidStudioProjects/ /media/furycd001/Duck/AndroidStudioProjects/ && \
echo Apps && \
rsync -aH --delete --info=progress2 /home/furycd001/Apps/ /media/furycd001/Duck/Apps/ && \
echo Documents && \
rsync -aH --delete --info=progress2 /home/furycd001/Documents/ /media/furycd001/Duck/Documents/ && \
echo Dos && \
rsync -aH --delete --info=progress2 /home/furycd001/Dos/ /media/furycd001/Duck/Dos/ && \
echo Dots && \
rsync -aH --delete --info=progress2 /home/furycd001/Dots/ /media/furycd001/Duck/Dots/ && \
echo Downloads && \
rsync -aH --delete --info=progress2 /home/furycd001/Downloads/ /media/furycd001/Duck/Downloads/ && \
echo Gateway && \
rsync -aH --delete --info=progress2 /home/furycd001/Gateway/ /media/furycd001/Duck/Gateway/ && \
echo Glitch && \
rsync -aH --delete --info=progress2 /home/furycd001/Glitch/ /media/furycd001/Duck/Glitch/ && \
echo Markdown && \
rsync -aH --delete --info=progress2 /home/furycd001/Markdown/ /media/furycd001/Duck/Markdown/ && \
echo Music && \
rsync -aH --delete --info=progress2 /home/furycd001/Music/ /media/furycd001/Duck/Music/ && \
echo Pictures && \
rsync -aH --delete --info=progress2 /home/furycd001/Pictures/ /media/furycd001/Duck/Pictures/ && \
echo Sites && \
rsync -aH --delete --info=progress2 /home/furycd001/Sites/ /media/furycd001/Duck/Sites/ && \
echo Steam && \
rsync -aH --delete --info=progress2 /home/furycd001/Steam/ /media/furycd001/Duck/Steam/ && \
echo Terminal && \
rsync -aH --delete --info=progress2 /home/furycd001/Terminal/ /media/furycd001/Duck/Terminal/ && \
echo Videos && \
rsync -aH --delete --info=progress2 /home/furycd001/Videos/ /media/furycd001/Duck/Videos/ && \
echo VirtualBox && \
rsync -aH --delete --info=progress2 /home/furycd001/VirtualBox\ VMs/ /media/furycd001/Duck/VirtualBox\ VMs/ && \
echo todo.txt  && \
rsync -aH --delete --info=progress2 /home/furycd001/todo.txt /media/furycd001/Duck/ && \
echo usr/share/backgrounds && \
rsync -aH --delete --info=progress2 /usr/share/backgrounds/ /media/furycd001/Duck/usr-share-backgrounds/ && \
echo .fonts && \
rsync -aH --delete --info=progress2 /home/furycd001/.fonts/ /media/furycd001/Duck/.fonts/ && \
echo .waterfox && \
rsync -aH --delete --info=progress2 /home/furycd001/.waterfox/ /media/furycd001/Duck/.waterfox/ && \
echo .mpd && \
rsync -aH --delete --info=progress2 /home/furycd001/.mpd/ /media/furycd001/Duck/.mpd/ && \
echo .ncmpcpp && \
rsync -aH --delete --info=progress2 /home/furycd001/.ncmpcpp/ /media/furycd001/Duck/.ncmpcpp/ && \
echo .mpv && \
rsync -aH --delete --info=progress2 /home/furycd001/.config/mpv/ /media/furycd001/Duck/.config/mpv/ && \
echo .config/gtk-3.0 && \
rsync -aH --delete --info=progress2 /home/furycd001/.config/gtk-3.0/ /media/furycd001/Duck/.config/gtk-3.0/ && \
echo .config/htop && \
rsync -aH --delete --info=progress2 /home/furycd001/.config/htop/ /media/furycd001/Duck/.config/htop/ && \
echo .config/Atom && \
rsync -aH --delete --info=progress2 /home/furycd001/.config/Atom/ /media/furycd001/Duck/.config/Atom/ && \
echo .atom && \
rsync -aH --delete --info=progress2 /home/furycd001/.atom/ /media/furycd001/Duck/.atom/ && \
echo BACKUP HAS FINISHED !!
