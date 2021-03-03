/* This file is part of the KDE project
   Copyright (C) 2001 Lubos Lunak <l.lunak@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __startup_h__
#define __startup_h__

#include <sys/types.h>

#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>
#include <qmap.h>
#include <kstartupinfo.h>

class QStyle;

class StartupId
    : public QObject
    {
    Q_OBJECT
    public:
        StartupId( QObject* parent = 0, const char* name = 0 );
        virtual ~StartupId();
        void configure();
    protected:
        void start_startupid( const QString& icon );
        void stop_startupid();
    protected slots:
        void update_startupid();
        void gotNewStartup( const KStartupInfoId& id, const KStartupInfoData& data );
        void gotStartupChange( const KStartupInfoId& id, const KStartupInfoData& data );
        void gotRemoveStartup( const KStartupInfoId& id );
    protected:
        KStartupInfo startup_info;
        QWidget* startup_widget;
        QTimer update_timer;
        QMap< KStartupInfoId, QString > startups; // QString == pixmap
        KStartupInfoId current_startup;
        QStyle* qstyle;
        bool blinking;
        unsigned int color_index;
        enum { NUM_BLINKING_PIXMAPS = 4 };
        QPixmap pixmaps[ NUM_BLINKING_PIXMAPS ];
        static const QColor startup_colors[ NUM_BLINKING_PIXMAPS ];
    };        

#endif
