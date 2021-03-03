/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 khotkeysglobal.h  - things shared by khotkeys daemon and cfg. module

 
 $Id: khotkeysglobal.h,v 1.2 2000/07/25 20:43:58 lunakl Exp $

****************************************************************************/

#ifndef __khotkeysglobal_H
#define __khotkeysglobal_H

#include <qdict.h>
#include <qtimer.h>
#include <kconfigbase.h>
#include <ksimpleconfig.h>


// one khotkeys "action"
// action name is the key in KHotData_dict
struct KHotData
    {
    KHotData( const QString& shortcut_P, const QString& run_P,
        bool menuentry_P = false );
    QString shortcut; // keyboard shortcut of this action
    QString run; // command/menuentry to run
    QTimer timeout; // for run timeout
    bool menuentry; // is this a menuetry
    };

// collection of actions
class KHotData_dict
    : public QDict< KHotData >
    {
    public:
        KHotData_dict( int size=17, bool caseSensitive=TRUE );
        bool read_config( KConfigBase& cfg_P );
        void write_config( KSimpleConfig& cfg_P ) const;
        typedef QDictIterator< KHotData > Iterator;
    };

namespace KHotKeys_shared
    {
    QString get_menu_entry_from_path( const QString& path_P );
    // config file
    const char* const CONFIG_FILE = "khotkeysrc";
    };
    
using namespace KHotKeys_shared;
    
//****************************************************************************
// Inline
//****************************************************************************

inline
KHotData::KHotData( const QString& shortcut_P, const QString& run_P,
    bool menuentry_P )
    : shortcut( shortcut_P ), run( run_P ), menuentry( menuentry_P )
    {
    }
    
inline
KHotData_dict::KHotData_dict( int size, bool caseSensitive )
    : QDict< KHotData >( size, caseSensitive )
    {
    setAutoDelete( true );
    }


#endif
