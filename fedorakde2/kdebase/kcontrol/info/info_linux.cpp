/*  $Id: info_linux.cpp,v 1.19 2001/06/21 10:20:04 deller Exp $

    Linux-specific Information about the Hardware.

    (C) Copyright 1998-2001 by Helge Deller <deller@gmx.de>

    To do (maybe?):
    - include Information about XFree86 and/or Accelerated X
	(needs to change configure-skript, to see, if Header-files are available !)
    - maybe also include information about the video-framebuffer devices
    - rewrite detection-routines (maybe not to use the /proc-fs)
    - more & better sound-information

    /dev/sndstat support added: 1998-12-08 Duncan Haldane (f.d.m.haldane@cwix.com)
    
    $Log: info_linux.cpp,v $
    Revision 1.19  2001/06/21 10:20:04  deller
    - use lspci (if available) for "PCI Information" instead of directly
      reading from /proc/pci (closes bug #12906)

    Revision 1.18  2001/03/05 15:02:21  faure
    Many fixes like
    -      new QListViewItem(lBox, QString(buf));
    +      new QListViewItem(lBox, QString::fromLocal8Bit(buf));
    from "Sergey A. Sukiyazov" <corwin@micom.don.ru>

    Revision 1.17  2001/01/31 20:26:44  deller
    fixed bug #15517,
    cleaned up comments and indenting.

    Revision 1.16  2000/09/07 19:12:42  faure
    Patch from FX to remove hardcoded sizes for some columns, since
    it breaks with translations. Besides, only the linux version had
    those....

    Revision 1.15  2000/07/16 21:01:58  hoelzer
    Show partition information on linux.

*/

#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <sys/stat.h>
#include <linux/kernel.h>
#include <ctype.h>
#include <config.h>

#ifdef HAVE_FSTAB_H	/* some Linux-versions don't have fstab.h */
#  include <fstab.h>
#  include <sys/statfs.h>
#  define INFO_PARTITIONS_FULL_INFO	/* show complete info */
#elif defined HAVE_MNTENT_H	/* but maybe they have mntent.h ? */
# include <mntent.h>
# include <sys/vfs.h>
#  define INFO_PARTITIONS_FULL_INFO	/* show complete info */
#else
#  undef INFO_PARTITIONS_FULL_INFO	/* no partitions-info */
#endif


#include <kapp.h>

#define INFO_CPU_AVAILABLE
#define INFO_CPU "/proc/cpuinfo"

#define INFO_IRQ_AVAILABLE
#define INFO_IRQ "/proc/interrupts"

#define INFO_DMA_AVAILABLE
#define INFO_DMA "/proc/dma"

#define INFO_PCI_AVAILABLE
#define INFO_PCI "/proc/pci"

#define INFO_IOPORTS_AVAILABLE
#define INFO_IOPORTS "/proc/ioports"

#define INFO_SOUND_AVAILABLE
#define INFO_DEV_SNDSTAT "/dev/sndstat"
#define INFO_SOUND "/proc/sound"
#define INFO_ASOUND "/proc/asound/sndstat"

#define INFO_DEVICES_AVAILABLE
#define INFO_DEVICES "/proc/devices"
#define INFO_MISC "/proc/misc"

#define INFO_SCSI_AVAILABLE
#define INFO_SCSI "/proc/scsi/scsi"

#define INFO_PARTITIONS_AVAILABLE
#define INFO_PARTITIONS "/proc/partitions"
#define INFO_MOUNTED_PARTITIONS "/etc/mtab"	/* on Linux... */

#define INFO_XSERVER_AVAILABLE


#define MAXCOLUMNWIDTH 600

bool GetInfo_ReadfromFile(QListView * lbox, const char *FileName,
			  char splitchar,
			  QListViewItem * lastitem = 0,
			  QListViewItem ** newlastitem = 0)
{
    char buf[512];
    bool added = false;

    QFile *file = new QFile(FileName);

    if (!file->exists()) {
	delete file;
	return false;
    }

    if (!file->open(IO_ReadOnly)) {
	delete file;
	/*   *GetInfo_ErrorString =
	   i18n("You do not have read-access for the file %1!\nPlease ask your system-administrator for advice!")
	   .arg(FileName);
	 */
	return false;
    }

    while (file->readLine(buf, sizeof(buf) - 1) > 0) {
	if (strlen(buf)) {
	    char *p = buf;
	    if (splitchar != 0)	/* remove leading spaces between ':' and the following text */
		while (*p) {
		    if (!isgraph(*p))
			*p = ' ';
		    if (*p == splitchar) {
			*p++ = ' ';
			while (*p == ' ')
			    ++p;
			*(--p) = splitchar;
			++p;
		    } else
			++p;
	    } else {
		while (*p) {
		    if (!isgraph(*p))
			*p = ' ';
		    ++p;
		}
	    }

	    QString s1 = QString::fromLocal8Bit(buf);
	    QString s2 = s1.mid(s1.find(splitchar) + 1);

	    s1.truncate(s1.find(splitchar));
	    if (!(s1.isEmpty() || s2.isEmpty()))
		lastitem = new QListViewItem(lbox, lastitem, s1, s2);
	    added = true;
	}
    }

    file->close();
    delete file;
    if (newlastitem)
	*newlastitem = lastitem;
    return added;
}




bool GetInfo_CPU(QListView * lBox)
{
    lBox->addColumn(i18n("Information"));
    lBox->addColumn(i18n("Value"));
    return GetInfo_ReadfromFile(lBox, INFO_CPU, ':');
}


bool GetInfo_IRQ(QListView * lBox)
{
    lBox->setFont(KGlobalSettings::fixedFont());
    return GetInfo_ReadfromFile(lBox, INFO_IRQ, 0);
}

bool GetInfo_DMA(QListView * lBox)
{
    lBox->addColumn(i18n("DMA-Channel"));
    lBox->addColumn(i18n("used by"));
    return GetInfo_ReadfromFile(lBox, INFO_DMA, ':');
}

bool GetInfo_PCI(QListView * lBox)
{
    int num;
    sorting_allowed = false;	/* no sorting by user */

    /* ry to get the output of the lspci package first */
    if ((num = GetInfo_ReadfromPipe(lBox, "lspci -v", true)) ||
        (num = GetInfo_ReadfromPipe(lBox, "/sbin/lspci -v", true)) ||
        (num = GetInfo_ReadfromPipe(lBox, "/usr/sbin/lspci -v", true)) ||
        (num = GetInfo_ReadfromPipe(lBox, "/usr/local/sbin/lspci -v", true)))
	    return num;

    /* if lspci failed, read the contents of /proc/pci */
    return GetInfo_ReadfromFile(lBox, INFO_PCI, 0);
}

bool GetInfo_IO_Ports(QListView * lBox)
{
    lBox->addColumn(i18n("I/O-Range"));
    lBox->addColumn(i18n("used by"));
    return GetInfo_ReadfromFile(lBox, INFO_IOPORTS, ':');
}

bool GetInfo_Sound(QListView * lBox)
{
    sorting_allowed = false;	/* no sorting by user */
    if (GetInfo_ReadfromFile(lBox, INFO_DEV_SNDSTAT, 0))
	return true;
    else if (GetInfo_ReadfromFile(lBox, INFO_SOUND, 0))
	return true;
    else
	return GetInfo_ReadfromFile(lBox, INFO_ASOUND, 0);
}

bool GetInfo_Devices(QListView * lBox)
{
    QListViewItem *lastitem = 0;
    sorting_allowed = false;	/* no sorting by user */
    GetInfo_ReadfromFile(lBox, INFO_DEVICES, 0, lastitem, &lastitem);
    lastitem = new QListViewItem(lBox, lastitem, "");	/* add empty line */
    /* don't use i18n() for "Misc devices", because all other info is english too! */
    lastitem = new QListViewItem(lBox, lastitem, QString("Misc devices:"));
    GetInfo_ReadfromFile(lBox, INFO_MISC, 0, lastitem, &lastitem);
    return true;
}

bool GetInfo_SCSI(QListView * lBox)
{
    return GetInfo_ReadfromFile(lBox, INFO_SCSI, 0);
}

static void cleanPassword(QString & str)
{
    int index = 0;
    QString passwd("password=");

    while (index >= 0) 
    {
	index = str.find(passwd, index, FALSE);
	if (index >= 0) {
	    index += passwd.length();
	    while (index < (int) str.length() && 
		   str[index] != ' ' && str[index] != ',')
		str[index++] = '*';
	}
    }
}

#ifndef INFO_PARTITIONS_FULL_INFO

bool GetInfo_Partitions(QListView * lBox)
{
    return GetInfo_ReadfromFile(lBox, INFO_PARTITIONS, 0);
}

#else	/* INFO_PARTITIONS_FULL_INFO */

// Some Ideas taken from garbazo from his source in info_fbsd.cpp

#if SIZEOF_LONG > 4
#define LONG_TYPE	unsigned long
#else
#ifdef HAVE_LONG_LONG
#define LONG_TYPE	unsigned long long
#else
/* On 32-bit systems we would get an overflow in unsigned int for
   drives bigger than 4GB. Let's use the ugly type double ! */
#define LONG_TYPE	double
#endif
#endif

bool GetInfo_Partitions(QListView * lbox)
{
#define NUMCOLS 6
    QString Title[NUMCOLS];
    QStringList Mounted_Partitions;
    bool found_in_List;
    int n;

#ifdef HAVE_FSTAB_H
    struct fstab *fstab_ent;
# define FS_NAME	fstab_ent->fs_spec	// device-name
# define FS_FILE	fstab_ent->fs_file	// mount-point
# define FS_TYPE	fstab_ent->fs_vfstype	// fs-type
# define FS_MNTOPS 	fstab_ent->fs_mntops	// mount-options
#else
    struct mntent *mnt_ent;
    FILE *fp;
# define FS_NAME	mnt_ent->mnt_fsname	// device-name
# define FS_FILE	mnt_ent->mnt_dir	// mount-point
# define FS_TYPE	mnt_ent->mnt_type	// fs-type
# define FS_MNTOPS 	mnt_ent->mnt_opts	// mount-options
#endif

    struct statfs sfs;
    LONG_TYPE total, avail;
    QString str, mountopts;
    QString MB(i18n("MB"));	/* "MB" = "Mega-Byte" */


#ifdef HAVE_FSTAB_H
    if (setfsent() == 0)	/* Try to open fstab */
	return false;
#else
    if (!(fp = setmntent("/etc/fstab", "r")))
	return false;
#endif

    /* read the list of already mounted file-systems.. */
    QFile *file = new QFile(INFO_MOUNTED_PARTITIONS);
    if (file->open(IO_ReadOnly)) {
	char buf[1024];
	while (file->readLine(buf, 1024) > 0) {
	    str = QString::fromLocal8Bit(buf);
	    if (str.length()) {
		int p = str.find(' ');	/* find first space. */
		if (p)
		    str.remove(p, 1024); /* erase all chars including space. */
		Mounted_Partitions.append(str);
	    }
	}
	file->close();
    }
    delete file;

    /* create the header-tables */
    MB = QString(" ") + MB;
    Title[0] = i18n("Device");
    Title[1] = i18n("Mount Point");
    Title[2] = i18n("FS Type");
    Title[3] = i18n("Total Size");
    Title[4] = i18n("Free Size");
    Title[5] = i18n("Mount Options");

    for (n = 0; n < NUMCOLS; ++n)
	lbox->addColumn(Title[n]);

    /* loop through all partitions... */
#ifdef HAVE_FSTAB_H
    while ((fstab_ent = getfsent()) != NULL)
#else
    while ((mnt_ent = getmntent(fp)) != NULL)
#endif
    {
	total = avail = 0;	/* initialize size.. */
	found_in_List = (Mounted_Partitions.contains(FS_NAME) > 0);
	if (found_in_List && statfs(FS_FILE, &sfs) == 0) {
	    total = ((LONG_TYPE) sfs.f_blocks) * sfs.f_bsize;
	    avail = (getuid()? sfs.f_bavail : sfs.f_bfree)
		* ((LONG_TYPE) sfs.f_bsize);
	};
	/*
	   if (stat(fstab_ent->fs_file,&st)!=0)
	   total = 0;
	   if (!S_ISDIR(st.st_mode))
	   total = 0;
	 */
	mountopts = FS_MNTOPS;
	cleanPassword(mountopts);
	if (total)
	    new QListViewItem(lbox, QString(FS_NAME) + "  ",
			      QString(FS_FILE) + "  ",
			      QString(FS_TYPE) + "  ",
			      Value((int) (((total / 1024) + 512) / 1024),
				    6) + MB,
			      Value((int) (((avail / 1024) + 512) / 1024),
				    6) + MB, mountopts);
	else
	    new QListViewItem(lbox, QString(FS_NAME), QString(FS_FILE),
			      QString(FS_TYPE), " ", " ", mountopts);
    }

#ifdef HAVE_FSTAB_H
    endfsent();			/* close fstab.. */
#else
    endmntent(fp);		/* close fstab.. */
#endif

    sorting_allowed = true;	/* sorting by user allowed ! */
    lbox->setSorting(1);

    return true;
}
#endif				/* INFO_PARTITIONS_FULL_INFO */




bool GetInfo_XServer_and_Video(QListView * lBox)
{
    return GetInfo_XServer_Generic(lBox);
}
