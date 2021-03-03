/*
 * $Id: info_netbsd.cpp,v 1.4.2.2 2001/09/05 18:43:56 deller Exp $
 *
 * info_netbsd.cpp is part of the KDE program kcminfo.  This displays
 * various information about the NetBSD system it's running on.
 *
 * Originally written by Jaromir Dolecek <dolecek@ics.muni.cz>. CPU info
 * code has been imported from implementation of processor.cpp for KDE 1.0
 * by David Brownlee <abs@NetBSD.org> as found in NetBSD packages collection.
 * Hubert Feyer <hubertf@NetBSD.org> enhanced the sound information printing
 * quite a lot, too.
 *
 * The code is placed into public domain. Do whatever you want with it.
 */

#define INFO_CPU_AVAILABLE
#define INFO_IRQ_AVAILABLE
#define INFO_DMA_AVAILABLE
#define INFO_PCI_AVAILABLE
#define INFO_IOPORTS_AVAILABLE
#define INFO_SOUND_AVAILABLE
#define INFO_DEVICES_AVAILABLE
#define INFO_SCSI_AVAILABLE
#define INFO_PARTITIONS_AVAILABLE
#define INFO_XSERVER_AVAILABLE


/*
 * all following functions should return TRUE, when the Information
 * was filled into the lBox-Widget. Returning FALSE indicates that
 * information was not available.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <stdio.h>	/* for NULL */
#include <stdlib.h>	/* for malloc(3) */

#include <qfile.h>
#include <qfontmetrics.h>
#include <qstrlist.h>
#include <qtextstream.h>

#include <kdebug.h>

typedef struct
  {
  int	string;
  int	name;
  const char	*title;
  } hw_info_mib_list_t;

bool GetInfo_CPU(QListView *lBox)
{
  static hw_info_mib_list_t hw_info_mib_list[]= {
	{ 1, HW_MODEL,		"Model" },
	{ 1, HW_MACHINE,	"Machine" },
	{ 1, HW_MACHINE_ARCH,	"Architecture" },
	{ 0, HW_NCPU,		"Number of CPUs" },
	{ 0, HW_PAGESIZE,	"Pagesize" },
	{ 0,0,0 }
	};
  hw_info_mib_list_t *hw_info_mib;

  int mib[2], num;
  char *buf;
  size_t len;
  QString value;

  lBox->addColumn(i18n("Information"));
  lBox->addColumn(i18n("Value"));

  for ( hw_info_mib = hw_info_mib_list ;  hw_info_mib->title ; ++hw_info_mib )
  {
	mib[0] = CTL_HW;
	mib[1] = hw_info_mib->name;
	if ( hw_info_mib->string ) {
		sysctl(mib,2,NULL,&len,NULL,0);
		if ( (buf = (char*)malloc(len)) ) {
			sysctl(mib,2,buf,&len,NULL,0);
			value = QString::fromLocal8Bit(buf);
			free(buf);
		}
		else {
			value = QString("Unknown");
		}
	}
	else {
		len = sizeof(num);
		sysctl(mib,2,&num,&len,NULL,0);
		value.sprintf("%d", num);
	}
	new QListViewItem(lBox, hw_info_mib->title, value);
   }

   return true;
}

// this is used to find out which devices are currently
// on system
static bool GetDmesgInfo(QListView *lBox, const char *filter,
	void func(QListView *, QCString s, void **, bool))
{
        QFile *dmesg = new QFile("/var/run/dmesg.boot");
	bool usepipe = false;
	FILE *pipe = NULL;
	QTextStream *t;
	bool seencpu = false;
	void *opaque = NULL;
	QCString s;
	bool found = false;

	if (dmesg->exists() && dmesg->open(IO_ReadOnly)) {
		t = new QTextStream(dmesg);
	}
	else {
		delete dmesg;
		pipe = popen("/sbin/dmesg", "r");
		if (!pipe) return false;
		usepipe = true;
		t = new QTextStream(pipe, IO_ReadOnly);
	}

	QListViewItem *olditem = NULL;
	while(!(s = t->readLine().local8Bit()).isEmpty()) {
		if (!seencpu) {
			if (s.contains("cpu"))
				seencpu = true;
			else
				continue;
		}
		if (s.contains("boot device") ||
			s.contains("WARNING: old BSD partition ID!"))
			break;

		if (!filter || s.contains(filter)) {
			if (func) {
				func(lBox, s, &opaque, false);
			}
			else {
				olditem = new QListViewItem(lBox, olditem, s);
			}
			found = true;
		}
	}
	if (func) {
		func(lBox, s, &opaque, true);
	}
	//lBox->triggerUpdate();

	delete t;
	if (pipe) {
		pclose(pipe);
	}
	else {
		dmesg->close();
		delete dmesg;
	}

	return found;
}


void AddIRQLine(QListView *lBox, QCString s, void **opaque, bool final)
{
	if (!final) {
		char str[3];
		const char *p = s.data();
		int pos = s.find(" irq ");
		int irq = (pos<0) ? 0 : atoi(p+pos+5);

		if (irq) {
			sprintf(str, "%2d", irq);
		}
		else {
			str[0] = str[1] = '?';
			str[2] = 0;
		}
		new QListViewItem(lBox, str, p);
	}
}

bool GetInfo_IRQ (QListView *lBox)
{
	lBox->addColumn(i18n("IRQ"));
	lBox->addColumn(i18n("Device"));
	(void) GetDmesgInfo(lBox, " irq ", AddIRQLine);
	sorting_allowed = true;
	lBox->setSorting(1);
	return true;
}

bool GetInfo_DMA (QListView *)
{
	return false;
}

bool GetInfo_PCI (QListView *lbox)
{
	if (!GetDmesgInfo(lbox, "at pci", NULL))
		new QListViewItem(lbox, i18n("No PCI devices found."));
	return true;
}

bool GetInfo_IO_Ports (QListView *lbox)
{
	if (!GetDmesgInfo(lbox, "port 0x", NULL))
		new QListViewItem(lbox, i18n("No I/O port devices found."));
	return true;
}

bool GetInfo_Sound (QListView *lbox)
{
	if (!GetDmesgInfo(lbox, "audio", NULL))
		new QListViewItem(lbox, i18n("No audio devices found."));

	// append information on any audio devices found
	QListViewItem *lvitem = lbox->firstChild();
	for(; lvitem; lvitem = lvitem->nextSibling()) {
		QString s;
		int pos, len;
		const char *start, *end;
		char *dev;

		s = lvitem->text(0);
		if ((pos = s.find("at ")) >= 0) {
			pos += 3;	// skip "at "
			start = end = s.ascii();
			for(; (*end!=':') && (*end!='\n'); end++);
			len = end - start;
			dev = (char *) malloc(len + 1);
			strncpy(dev, start, len);
			dev[len] = '\0';

			GetDmesgInfo(lbox, dev, NULL);

			free(dev);
		}
	}

	return true;
}

bool GetInfo_Devices (QListView *lBox)
{
	(void) GetDmesgInfo(lBox, NULL, NULL);
	return true;
}

bool GetInfo_SCSI (QListView *lbox)
{
	if (!GetDmesgInfo(lbox, "scsibus", NULL))
		new QListViewItem(lbox, i18n("No SCSI devices found."));
	return true;
}

bool GetInfo_Partitions (QListView *lbox)
{
	QCString s;
	char *line, *orig_line;
	const char *device, *mountpoint, *type, *flags;
	FILE *pipe = popen("/sbin/mount", "r");
	QTextStream *t;

	if (!pipe) {
		kdError(0) << i18n("Ahh couldn't run /sbin/mount!") << endl;
		return false;
	}
	t = new QTextStream(pipe, IO_ReadOnly);

	lbox->addColumn(i18n("Device"));
	lbox->addColumn(i18n("Mount Point"));
	lbox->addColumn(i18n("FS Type"));
	lbox->addColumn(i18n("Mount Options"));

	QListViewItem *olditem = 0;
	while (!(s = t->readLine().latin1()).isEmpty()) {
		orig_line = line = strdup(s);

		device = strsep(&line, " ");

		(void) strsep(&line, " "); // consume word "on"
		mountpoint = strsep(&line, " ");

		(void) strsep(&line, " "); // consume word "type"
		type = strsep(&line, " ");

		flags = line;

		olditem = new QListViewItem(lbox, olditem, device, mountpoint,
					type, flags);

		free(orig_line);
	}

	delete t;
	pclose(pipe);
	return true;
}

bool GetInfo_XServer_and_Video (QListView *lBox)
{
	return GetInfo_XServer_Generic( lBox );
}
