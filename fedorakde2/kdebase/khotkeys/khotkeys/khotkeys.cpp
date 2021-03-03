/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 khotkeys.cpp  -
 
 $Id: khotkeys.cpp,v 1.2 2000/07/25 20:43:57 lunakl Exp $

****************************************************************************/

#define __khotkeys_CPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <krun.h>
#include <kdesktopfile.h>
#include <ksimpleconfig.h>
#include <kurifilter.h>
#include <kstddirs.h>
#include <kdebug.h>

#include "khotkeys.h"

KHotKeysApp::KHotKeysApp()
    : KUniqueApplication( false, true ) // no styles
    {
    accel = new KHKGlobalAccel();
    reread_configuration();
    }

KHotKeysApp::~KHotKeysApp()
    {
    delete accel;
    }

// called when any action shortcut is pressed
void KHotKeysApp::accel_activated( const QString& action_P, const QString&,
    int )
    {
    KHotData* current = data[ action_P ];
    if( current->timeout.isActive()) // a little timeout after running
        return;
    if( current->menuentry )
        start_menuentry( action_P );
    else
        start_general( action_P );
    }

// run executables, URLs, whatever
void KHotKeysApp::start_general( const QString& action_P )
    {
    KHotData* current = data[ action_P ];
    // this code used to start commands is based on kdesktop's minicli.cpp
    QString run = current->run.stripWhiteSpace();
    if( run == "" )
        return;
    int space_pos = run.find( ' ' );
    QString cmd = run;
    KURIFilterData uri;
    if( run[ 0 ] != '\'' && run[ 0 ] != '"' && space_pos > -1
        && run[ space_pos - 1 ] != '\\' )
        cmd = run.left( space_pos ); // get first 'word'
    uri.setData( cmd );
    KURIFilter::self()->filterURI( uri );
#if 0
     printf( "URI : %s\nFiltered URI : %s\n URI Type : %i\n"
         "Was Filtered :%i\n", cmd.latin1(), uri.uri().url().latin1(),
         uri.uriType(), uri.hasBeenFiltered() );
#endif
    switch( uri.uriType())
        {
        case KURIFilterData::LOCAL_FILE:
        case KURIFilterData::LOCAL_DIR:
        case KURIFilterData::NET_PROTOCOL:
        case KURIFilterData::HELP:
            {
            ( void ) new KRun( uri.uri());
          break;
            }
        case KURIFilterData::EXECUTABLE:
        case KURIFilterData::SHELL:
            {
            QString icon_name = uri.iconName();
            if( icon_name.isNull())
                icon_name = QString::fromLatin1( "go" );
            if( !KRun::runCommand( run, cmd, icon_name ))
                ;
          break;
            }
        default: // error
          return; // CHECKME remove this action ??
        }
    current->timeout.start( 1000, true ); // 1sec timeout
    }

// start menuentry configured by kmenuedit
void KHotKeysApp::start_menuentry( const QString& action_P )
    {
    KHotData* current = data[ action_P ];
    if( current->run.isEmpty())
        return;
    if( current->run.right( 8 ) != ".desktop" )
        return;
    bool needs_search = false;
    if( KGlobal::dirs()->findResource( "apps", current->run )
        == QString::null )
        needs_search = true; // menu entry no loger exists
    else
        {
        KDesktopFile cfg( current->run, true );
        if( cfg.readBoolEntry( "Hidden" )) // menu entry is hidden
            needs_search = true; 
        }
    if( needs_search ) // try to find the menu entry ( if it has been moved )
        {
        int slash = current->run.findRev( '/' );
        QString desktop_file;
        if( slash >= 0 )
            desktop_file = current->run.mid( slash + 1 );
        else
            desktop_file = current->run;
        QStringList possibilities = KGlobal::dirs()->findAllResources( "apps",
            desktop_file, true ); // search whole K-menu
        if( possibilities.count() <= 0 )
            { // remove this action as its menu entry can't be found
            data.remove( action_P );
            KSimpleConfig cfg( CONFIG_FILE, false );
            data.write_config( cfg );
            return;
            }
        desktop_file = "";
        for( QStringList::Iterator it = possibilities.begin();
             it != possibilities.end();
             ++it )
            {
            KDesktopFile cfg( *it, true );
            if( !cfg.readBoolEntry( "Hidden" ))
                {
                desktop_file = *it;
                break;
                }
            }
        if( desktop_file.isEmpty())
            return;
        desktop_file = get_menu_entry_from_path( desktop_file );
        current->run = desktop_file;
        KSimpleConfig cfg( CONFIG_FILE, false );
        data.write_config( cfg );
        }
    // run the menu entry's .desktop file
    ( void ) new KRun( KGlobal::dirs()->findResource( "apps", current->run ));
    current->timeout.start( 1000, true ); // 1sec timeout
    }

void KHotKeysApp::reread_configuration()
    {
    accel->clear();
    data.clear();    
    KSimpleConfig cfg( CONFIG_FILE, true );
    data.read_config( cfg );
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        {
        if( !accel->insertItem( it.currentKey(), it.currentKey(),
            it.current()->shortcut ))
            continue; // invalid shortcut
        accel->connectItem( it.currentKey(), this,
            SLOT( accel_activated( const QString&, const QString&, int )));
        }
    }
    
// needed for KHKGlobalAccel
bool KHotKeysApp::x11EventFilter(XEvent * ev) 
    {
    if( accel->x11EventFilter( ev ))
        return true;
    return KUniqueApplication::x11EventFilter( ev );
    }

#include "khotkeys.moc"
