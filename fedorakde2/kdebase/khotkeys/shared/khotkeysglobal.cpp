/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 khotkeysglobal.cpp  - things shared by khotkeys daemon and cfg. module
 
 $Id: khotkeysglobal.cpp,v 1.2 2000/07/25 20:43:57 lunakl Exp $

****************************************************************************/

#define __khotkeysglobal_CPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kglobal.h>
#include <kstddirs.h>

#include "khotkeysglobal.h"

bool KHotData_dict::read_config( KConfigBase& cfg_P )
    {
    cfg_P.setGroup( "Main" ); // main group
    int version = cfg_P.readNumEntry( "Version", 1 );
    if( version != 1 ) // unknown version
        return false;        // CHECKME kdebug ?
    int sections = cfg_P.readNumEntry( "Num_Sections", 0 );
    for( int sect = 1;
         sect <= sections;
         ++sect )
        {
        QString group = QString( "Section%1" ).arg( sect );
        if( !cfg_P.hasGroup( group ))
            continue;
        cfg_P.setGroup( group );
        QString name = cfg_P.readEntry( "Name" );
        if( name == QString::null )
            continue;
        QString shortcut = cfg_P.readEntry( "Shortcut" );
        if( shortcut == QString::null )
            continue;
        QString run = cfg_P.readEntry( "Run" );
        if( run == QString::null )
            continue;
        bool menuentry = cfg_P.readBoolEntry( "MenuEntry", false );
        replace( name, new KHotData( shortcut, run, menuentry ));
        }
    return true;
    }

void KHotData_dict::write_config( KSimpleConfig& cfg_P ) const
    {
    cfg_P.setGroup( "Main" ); // main group
    cfg_P.writeEntry( "Version", 1 );
    cfg_P.writeEntry( "Num_Sections", count());
    int sect = 1;
    for( Iterator it( *this );
         it.current();
         ++it, ++sect )
        {
        cfg_P.setGroup( QString( "Section%1" ).arg( sect ));
        cfg_P.writeEntry( "Name", it.currentKey());
        cfg_P.writeEntry( "Shortcut", it.current()->shortcut );
        cfg_P.writeEntry( "Run", it.current()->run );
        cfg_P.writeEntry( "MenuEntry", it.current()->menuentry );
        }
    while( cfg_P.deleteGroup( QString( "Section%1" ).arg( sect )))
        ++sect;   // delete unneeded sections
    }

// does the opposite of KStandardDirs::findResource() i.e. e.g.
// "/opt/kde2/share/applnk/System/konsole.desktop" -> "System/konsole.desktop"
QString KHotKeys_shared::get_menu_entry_from_path( const QString& path_P )
    {
    QStringList dirs = KGlobal::dirs()->resourceDirs( "apps" );
    for( QStringList::Iterator it = dirs.begin();
         it != dirs.end();
         ++it )
        if( path_P.find( *it ) == 0 )
            {
            QString ret = path_P;
            ret.remove( 0, (*it).length());
            if( ret[ 0 ] == '/' )
                ret.remove( 0, 1 );
            return ret;
            }
    return path_P;
    }
