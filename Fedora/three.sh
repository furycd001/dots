#!/usr/bin/env bash

# Check for usage of sudo or root login..
if [ `whoami` != root ]; then
    echo Please use sudo or login as root....
    exit
fi

echo /usr/bin/backup && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/backup /usr/bin/backup && \
echo /usr/bin/fendi && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/fendi /usr/bin/fendi && \
echo usr/share/backgrounds && \
rsync -aH --delete --info=progress2 /media/furycd001/Duck/usr-share-backgrounds/ /usr/share/backgrounds/ && \

echo [ PART.3 COPYING BACKUP SCRIPTS HAS FINISHED !! ] && \
echo  && \
echo [ PLEASE NOW RUN PART.4 (without root) !! ]
