/*
 * Copyright (C) 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef KPARTSAVER_H_INCLUDED
#define KPARTSAVER_H_INCLUDED


#include <qwidget.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kparts/part.h>
#include <ktrader.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kmimetype.h>

#include <kscreensaver.h>

#include "configwidget.h"


class SaverConfig : public ConfigWidget {
Q_OBJECT

 public:
    SaverConfig( QWidget* parent = 0, const char* name = 0 );
    ~SaverConfig(); 

 protected slots:
    void apply();    
    void add();
    void remove();
    void select();
    void up();
    void down();
};


class KPartSaver : public KScreenSaver {
Q_OBJECT

 public:
    KPartSaver( WId id=0 );
    virtual ~KPartSaver();

 public slots:
    void next( bool random );
    void queue( KURL url );
    void timeout();
    void closeURL();

 protected:    
    struct Medium {
	KURL url;
	bool failed;
    };   

    bool openURL(  KURL url );    

    QValueList<Medium> m_media;
    QTimer *m_timer;
    KParts::ReadOnlyPart *m_part;
    int m_current; 
    
    bool m_single;
    bool m_random;
    int m_delay;
    QStringList m_files;
    KLibFactory *m_factory;
    QLabel *m_back;
};

#endif
