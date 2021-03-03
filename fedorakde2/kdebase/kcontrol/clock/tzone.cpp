/*
 *  tzone.cpp
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <config.h>

#include <qlabel.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>
#include <kmessagebox.h>

#include "xpm/world.xpm"
#include "tzone.h"
#include "tzone.moc"

Tzone::Tzone(QWidget * parent, const char *name)
  : QWidget (parent, name)
{
    QGroupBox* gBox = new QGroupBox ( this );

    QBoxLayout *top_lay = new QVBoxLayout( gBox, 10 );
    QBoxLayout *lay = new QHBoxLayout(top_lay);

    currentzonetitle = new QLabel(i18n("Current time zone: "), gBox);
    currentzonetitle->setAutoResize(true);
    lay->addWidget(currentzonetitle);

    currentzone = new QLabel(gBox);
    lay->addWidget(currentzone);
    currentzone->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

    QLabel* instructions = new QLabel(i18n("To change the time zone, select your area from the list below:"), gBox);
    top_lay->addWidget(instructions);

    tzonelist = new QComboBox( FALSE, gBox, "ComboBox_1" );
    connect( tzonelist, SIGNAL(activated(int)), SLOT(handleZoneChange()) );
    top_lay->addWidget( tzonelist );

    QBoxLayout *top = new QVBoxLayout( this, 5 );
    top->addWidget(gBox);

    fillTimeZones();

    load();

    if (getuid() != 0)
        tzonelist->setEnabled(false);

}

void Tzone::fillTimeZones()
{
    FILE	*f;
    char	tempstring[101] = "Unknown";
    QStrList	list;

    getCurrentZone(tempstring);
    currentzone->setText(tempstring);

    tzonelist->insertItem(i18n("[No selection]"));

    // Read all system time zones
    f = popen("grep -e  ^[^#] /usr/share/zoneinfo/zone.tab | cut -f 3 ","r");
    if (!f)
	    return;
    while(fgets(tempstring, 100, f) != NULL)
        list.inSort(tempstring);
    pclose(f);

    tzonelist->insertStrList(&list);
}

void Tzone::getCurrentZone(char* szZone)
{
    FILE	*f;

    // Read the current time zone
    f = popen("date +%Z","r");
    if (!f) return;
    fscanf(f, "%s", szZone);
    pclose(f);
}

void Tzone::load()
{
    KConfig *config = KGlobal::config();
    config->setGroup("tzone");
    FILE *f;
    char tempstring[101] = "Unknown";
    char szCurrentlySet[101] = "Unknown";
    QStrList list;
    int nCurrentlySet = 0;

    getCurrentZone(tempstring);
    currentzone->setText(tempstring);

    // read the currently set time zone
    if((f = fopen("/etc/timezone", "r")) != NULL)
    {
        // get the currently set timezone
        fgets(szCurrentlySet, 100, f);
        fclose(f);
    }

    tzonelist->insertItem(i18n("[No selection]"));

    // Read all system time zones
    f = popen("grep -e  ^[^#] /usr/share/zoneinfo/zone.tab | cut -f 3","r");
    if (!f) return;
    while(fgets(tempstring, 100, f) != NULL)
	list.inSort(tempstring);
    pclose(f);

    tzonelist->insertStrList(&list);

    // find the currently set time zone and select it
    for (int i = 0; i < tzonelist->count(); i++)
    {
        if (tzonelist->text(i) == QString::fromLatin1(szCurrentlySet))
        {
            nCurrentlySet = i;
            break;
        }
    }

    tzonelist->setCurrentItem(nCurrentlySet);
}


void Tzone::save()
{
    QString tz;
    QString selectedzone(tzonelist->currentText());
    QString currentlySetZone;

    if( selectedzone != i18n("[No selection]"))
    {
        QFile fTimezoneFile("/etc/timezone");

        if (fTimezoneFile.open(IO_WriteOnly | IO_Truncate) )
        {
            QTextStream t(&fTimezoneFile);
            t << selectedzone;
            fTimezoneFile.close();
        }

        tz = "/usr/share/zoneinfo/" + tzonelist->currentText();
        tz.truncate(tz.length()-1);

        kdDebug() << "Set time zone " << tz << endl;

        // This is extremely ugly. Who knows the better way?
        unlink( "/etc/localtime" );
        if (symlink( QFile::encodeName(tz), "/etc/localtime" ) != 0)
            KMessageBox::error( 0,  i18n("Error setting new Time Zone!"),
                                i18n("Timezone Error"));

        QString val = ":" + tz;
        setenv("TZ", val.ascii(), 1);
        tzset();

        // write some stuff
        KConfig *config = KGlobal::config();
        config->setGroup("tzone");
        config->writeEntry("TZ", tzonelist->currentItem() );
        config->sync();
    }
}
