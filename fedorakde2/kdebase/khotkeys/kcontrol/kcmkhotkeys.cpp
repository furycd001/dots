/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 kcmkhotkeys.cpp  -
 
 $Id: kcmkhotkeys.cpp,v 1.5.2.1 2001/09/07 18:41:17 lunakl Exp $

****************************************************************************/

#define __kcmkhotkeys_CPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ksimpleconfig.h>
#include <kaccel.h>
#include <kdialogbase.h>
#include <dcopclient.h>
#include <kkeydialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include "khotkeysglobal.h"

#include "kcmkhotkeys.h"

static void write_conf( KHotData_dict& data_P );
static bool edit_shortcut( const QString& action_name_P, KHotData* data_P,
    KHotData_dict& data_P1, const QString& shortcut_P );
static QString change_shortcut_internal( const QString& file_P,
    const QString& shortcut_P, bool save_P, bool edit_P );

void khotkeys_init()
    {
    // I hope this works
    KGlobal::locale()->insertCatalogue("khotkeys");
    }

QString khotkeys_get_menu_entry_shortcut( const QString& file_P )
    {
    KHotData_dict data;
    KSimpleConfig cfg( CONFIG_FILE, true );
    data.read_config( cfg );
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        if( it.current()->menuentry && it.current()->run == file_P )
            return it.current()->shortcut;
    return "";
    }

bool khotkeys_menu_entry_moved( const QString& new_P, const QString& old_P )
    {
    KHotData_dict data;
        {
        KSimpleConfig cfg( CONFIG_FILE, true );
        data.read_config( cfg );
        }
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        if( it.current()->menuentry && it.current()->run == new_P )
            return false;
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        if( it.current()->menuentry && it.current()->run == old_P )
            {
            it.current()->run = new_P;
            write_conf( data );
            return true;
            }
    return false;
    }

void khotkeys_menu_entry_deleted( const QString& entry_P )
    {
    KHotData_dict data;
        {
        KSimpleConfig cfg( CONFIG_FILE, true );
        data.read_config( cfg );
        }
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        if( it.current()->menuentry && it.current()->run == entry_P )
            {
            data.remove( it.currentKey());
            write_conf( data );
            return;
            }
    }

QString khotkeys_change_menu_entry_shortcut( const QString& entry_P,
    const QString& shortcut_P )
    {
    return change_shortcut_internal( entry_P, shortcut_P, true, false );
    }
    
QString khotkeys_edit_menu_entry_shortcut( const QString& entry_P,
    const QString& shortcut_P, bool save_if_edited_P )
    {
    return change_shortcut_internal( entry_P, shortcut_P, save_if_edited_P,
        true );
    }
    
QString change_shortcut_internal( const QString& entry_P,
    const QString& shortcut_P, bool save_if_edited_P, bool edit_P )
    {
    KHotData_dict data;
        {
        KSimpleConfig cfg( CONFIG_FILE, true );
        data.read_config( cfg );
        }
    KHotData* pos = NULL;
    QString name;
    bool new_data = false;
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        if( it.current()->menuentry && it.current()->run == entry_P )
            {
            name = it.currentKey();
            pos = data.take( name );
            break;
            }
    if( pos == NULL ) // new action
        {
        name = "K Menu"; // CHECKME i18n
        name += " - ";
        name += entry_P;
        pos = new KHotData( "", entry_P, true );
        new_data = true;
        }
    if( edit_P )
        {
        if( !edit_shortcut( name, pos, data, shortcut_P ))
            return shortcut_P;
        }
    else    
        pos->shortcut = KAccel::keyToString(    // make sure the shortcut
            KAccel::stringToKey( shortcut_P ), false); // is valid
    if( !save_if_edited_P )
        return pos->shortcut;
    if( pos->shortcut.isEmpty())
        {            
        delete pos;
        if( !new_data ) // remove from config file
            write_conf( data );
        return "";
        }
    data.insert( name, pos );
    write_conf( data );
    return pos->shortcut;
    }

void write_conf( KHotData_dict& data_P )
    {
        {
        KSimpleConfig cfg( CONFIG_FILE, false );
        data_P.write_config( cfg );
        }
    if( !kapp->dcopClient()->isApplicationRegistered( "khotkeys" ))
        KApplication::kdeinitExec( "khotkeys" );
    else
        {
        QByteArray data;
        kapp->dcopClient()->send( "khotkeys*", "khotkeys", "reread_configuration()", data );
        }
    }
    
bool edit_shortcut( const QString& action_name_P, KHotData* item_P,
    KHotData_dict& data_P, const QString& shortcut_P )
    {
    desktop_shortcut_dialog* dlg =
        new desktop_shortcut_dialog( action_name_P, item_P, data_P,
        shortcut_P );
    bool ret = dlg->dlg_exec();
    delete dlg;
    return ret;
    }
    
desktop_shortcut_dialog::desktop_shortcut_dialog(
    const QString& action_name_P, KHotData* item_P, KHotData_dict& data_P,
    QString shortcut_P )
    : KDialogBase( NULL, NULL, true, i18n( "Select shortcut" ), Ok | Cancel ),
        data( data_P ), item( item_P ), action_name( action_name_P )
    {
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        {
        if( it.current() == item )
            continue;
        if( it.current()->shortcut == shortcut_P )
            shortcut_P = ""; // this shortcut is taken up by some other action
        }
    KKeyEntry entry;
    entry.aCurrentKeyCode = entry.aConfigKeyCode
        = KAccel::stringToKey( shortcut_P );
    entry.bConfigurable = true;
    entry.descr = action_name_P;
    entry.bEnabled = true;
    map.insert( action_name_P, entry );
    QWidget* page = new QWidget( this );
    setMainWidget( page );         // CHECKME i18n
    QLabel* label = new QLabel( i18n( "Desktop file to run" ), page );
//    QLabel* label = new QLabel( "K Menu entry to run", page ); 
    QLineEdit* line = new QLineEdit( page );
    line->setText( item_P->run );
//    line->setMinimumWidth( 500 );
    line->setReadOnly( true );
    keychooser = new KKeyChooser( &map, page, true, false, true );
    connect( keychooser, SIGNAL( keyChange()), this, SLOT( key_changed()));
    QBoxLayout* main_layout = new QVBoxLayout( page, KDialog::marginHint(),
        KDialog::spacingHint());
    main_layout->addWidget( label, 1 );
    main_layout->addWidget( line, 1 );
    main_layout->addWidget( keychooser, 10 );
    }
    
bool desktop_shortcut_dialog::dlg_exec()
    {
    if( exec() == Accepted )
        {
        item->shortcut
            = KAccel::keyToString( map[ action_name ].aConfigKeyCode, false );
        return true;
        }
    return false;    
    }

void desktop_shortcut_dialog::key_changed()
    {
    int shortcut = map[ action_name ].aConfigKeyCode;
    for( KHotData_dict::Iterator it( data );
         it.current();
         ++it )
        {
        if( it.current() == item )
            continue;
        if( KAccel::stringToKey( it.current()->shortcut ) == shortcut )
            {
            QString str =
                i18n( "The %1 key combination has already "
                     "been allocated\n"
                     "to the %2 action.\n"
                     "\n"
                     "Please choose a unique key combination."
                    ).arg( it.current()->shortcut ).arg( it.currentKey());
            KMessageBox::sorry( this, str, i18n( "Key conflict" ));
            map[ action_name ].aConfigKeyCode = 0;
            keychooser->listSync(); // cancel the selected shortcut
            keychooser->update();   // in KKeyChooser
            return;
            }
        }
    }

#include "kcmkhotkeys.moc"

