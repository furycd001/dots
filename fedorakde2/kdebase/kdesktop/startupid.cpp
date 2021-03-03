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

#include "startupid.h"

#include <qwidget.h>
#include <kiconloader.h>
#include <qcursor.h>
#include <qapplication.h>
#include <kconfig.h>
#include <qwindowsstyle.h>
#include <X11/Xlib.h>

StartupId::StartupId( QObject* parent, const char* name )
    : QObject( parent, name ), startup_info( true ), startup_widget( NULL ), blinking( true )
    {
    connect( &update_timer, SIGNAL( timeout()), SLOT( update_startupid()));
    connect( &startup_info,
        SIGNAL( gotNewStartup( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotNewStartup( const KStartupInfoId&, const KStartupInfoData& )));
    connect( &startup_info,
        SIGNAL( gotStartupChange( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotStartupChange( const KStartupInfoId&, const KStartupInfoData& )));
    connect( &startup_info,
        SIGNAL( gotRemoveStartup( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotRemoveStartup( const KStartupInfoId& )));
    qstyle = new QWindowsStyle;
    }

StartupId::~StartupId()
    {
    stop_startupid();
    delete qstyle;
    }
    
void StartupId::configure()
    {
    KConfig c( "klaunchrc", true );
    c.setGroup( "BusyCursorSettings" );
    startup_info.setTimeout( c.readUnsignedNumEntry( "Timeout", 30 ));
    blinking = c.readBoolEntry( "Blinking", true );
    }
    
void StartupId::gotNewStartup( const KStartupInfoId& id_P, const KStartupInfoData& data_P )
    {
    QString icon = data_P.findIcon();
    current_startup = id_P;
    startups[ id_P ] = icon;
    start_startupid( icon );
    }

void StartupId::gotStartupChange( const KStartupInfoId& id_P, const KStartupInfoData& data_P )
    {
    if( current_startup == id_P )
        {
        QString icon = data_P.findIcon();
        if( !icon.isEmpty() && icon != startups[ current_startup ] )
            {
            startups[ id_P ] = icon;
            start_startupid( icon );
            }
        }
    }

void StartupId::gotRemoveStartup( const KStartupInfoId& id_P )
    {
    startups.remove( id_P );
    if( startups.count() == 0 )
        {
        stop_startupid();
        current_startup = KStartupInfoId(); // null
        return;
        }
    current_startup = startups.begin().key();
    start_startupid( startups[ current_startup ] );
    }

void StartupId::stop_startupid()
    {
    delete startup_widget;
    startup_widget = NULL;
    if( blinking )
        for( int i = 0;
             i < NUM_BLINKING_PIXMAPS;
             ++i )
            pixmaps[ i ] = QPixmap(); // null
    update_timer.stop();
    }

const QColor StartupId::startup_colors[ StartupId::NUM_BLINKING_PIXMAPS ]
    = { Qt::black, Qt::darkGray, Qt::lightGray, Qt::white };

void StartupId::start_startupid( const QString& icon_P )
    {
    QPixmap icon_pixmap = KGlobal::iconLoader()->loadIcon( icon_P, KIcon::Small, 0,
        KIcon::DefaultState, 0, true ); // return null pixmap if not found
    if( icon_pixmap.isNull())
        icon_pixmap = SmallIcon( "exec" );
    if( startup_widget == NULL )
        {
        startup_widget = new QWidget( NULL, NULL, WX11BypassWM );
        XSetWindowAttributes attr;
        attr.save_under = True; // useful saveunder if possible to avoid redrawing
        XChangeWindowAttributes( qt_xdisplay(), startup_widget->winId(), CWSaveUnder, &attr );
        // set a simple style which doesn't paint anything on the widget
        startup_widget->setStyle( qstyle );
        }
    startup_widget->resize( icon_pixmap.width(), icon_pixmap.height());
    if( blinking )
        {
        startup_widget->clearMask();
        int window_w = icon_pixmap.width();
        int window_h = icon_pixmap.height();
        for( int i = 0;
             i < NUM_BLINKING_PIXMAPS;
             ++i )
            {
            pixmaps[ i ] = QPixmap( window_w, window_h );
            pixmaps[ i ].fill( startup_colors[ i ] );
            bitBlt( &pixmaps[ i ], 0, 0, &icon_pixmap );
            }
        color_index = 0;
        }
    else
        {
        if( icon_pixmap.mask() != NULL )
            startup_widget->setMask( *icon_pixmap.mask());
        else
            startup_widget->clearMask();
        startup_widget->setBackgroundPixmap( icon_pixmap );
        startup_widget->erase();
        }
    update_startupid();
    startup_widget->show();
    }

namespace
{
const int X_DIFF = 15;
const int Y_DIFF = 15;
const int color_to_pixmap[] = { 0, 1, 2, 3, 2, 1 };
}

void StartupId::update_startupid()
    {
    if( blinking )
        {
        startup_widget->setBackgroundPixmap( pixmaps[ color_to_pixmap[ color_index ]] );
        if( ++color_index >= ( sizeof( color_to_pixmap ) / sizeof( color_to_pixmap[ 0 ] )))
            color_index = 0;
        }
    QPoint c_pos = QCursor::pos();
    if( startup_widget->x() != c_pos.x() + X_DIFF
        || startup_widget->y() != c_pos.y() + Y_DIFF )
        startup_widget->move( c_pos.x() + X_DIFF, c_pos.y() + Y_DIFF );
    XRaiseWindow( qt_xdisplay(), startup_widget->winId());
    update_timer.start( 100, true ); // every 0.1sec
    QApplication::flushX();
    }

#include "startupid.moc"
