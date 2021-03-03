/* -------------------------------------------------------------

   urlgrabber.cpp (part of Klipper - Cut & paste history for KDE)

   $Id: urlgrabber.cpp,v 1.23.2.2 2001/10/30 15:28:23 pfeiffer Exp $

   (C) 2000 by Carsten Pfeiffer <pfeiffer@kde.org>

   Licensed under the Artistic License

 ------------------------------------------------------------- */

#include <qtimer.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <keditcl.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kservice.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <netwm.h>
#include <kstringhandler.h>

#include "urlgrabber.h"

// TODO:
// - script-interface?
// - Bug in KPopupMenu::clear() (insertTitle() doesn't go away sometimes)

#define URL_EDIT_ITEM 10
#define DO_NOTHING_ITEM 11

URLGrabber::URLGrabber()
{
    myCurrentAction = 0L;
    myMenu = 0L;
    myPopupKillTimeout = 8;

    myActions = new ActionList();
    myActions->setAutoDelete( true );
    myMatches.setAutoDelete( false );

    readConfiguration( kapp->config() );

    myPopupKillTimer = new QTimer( this );
    connect( myPopupKillTimer, SIGNAL( timeout() ),
             SLOT( slotKillPopupMenu() ));

    // testing
    /*
    ClipAction *action;
    action = new ClipAction( "^http:\\/\\/", "Web-URL" );
    action->addCommand("kfmclient exec %s", "Open with Konqi", true);
    action->addCommand("netscape -no-about-splash -remote \"openURL(%s, new-window)\"", "Open with Netscape", true);
    myActions->append( action );

    action = new ClipAction( "^mailto:", "Mail-URL" );
    action->addCommand("kmail --composer %s", "Launch kmail", true);
    myActions->append( action );

    action = new ClipAction( "^\\/.+\\.jpg$", "Jpeg-Image" );
    action->addCommand("kuickshow %s", "Launch KuickShow", true);
    action->addCommand("kview %s", "Launch KView", true);
    myActions->append( action );
    */
}


URLGrabber::~URLGrabber()
{
    delete myActions;
}

void URLGrabber::invokeAction( const QString& clip )
{
    if ( !clip.isEmpty() )
	myClipData = clip;

    actionMenu( false );
}


void URLGrabber::setActionList( ActionList *list )
{
    delete myActions;
    myActions = list;
}


const ActionList& URLGrabber::matchingActions( const QString& clipData )
{
    myMatches.clear();
    ClipAction *action = 0L;
    ActionListIterator it( *myActions );
    for ( action = it.current(); action; action = ++it ) {
        if ( action->matches( clipData ) )
            myMatches.append( action );
    }

    return myMatches;
}


bool URLGrabber::checkNewData( const QString& clipData )
{
    // kdDebug() << "** checking new data: " << clipData << endl;
    myClipData = clipData;
    if ( myActions->isEmpty() )
        return false;

    actionMenu( true ); // also creates myMatches

    return ( !myMatches.isEmpty() &&
             (!kapp->config()->readBoolEntry("Put Matching URLs in history", true)));
}


void URLGrabber::actionMenu( bool wm_class_check )
{
    if ( myClipData.isEmpty() )
        return;

    ActionListIterator it( matchingActions( myClipData ) );
    ClipAction *action = 0L;
    ClipCommand *command = 0L;

    if ( it.count() > 0 ) {
	// don't react on konqi's/netscape's urls...
        if ( wm_class_check && isAvoidedWindow() )
            return;

        QString item;
        myCommandMapper.clear();

        myPopupKillTimer->stop();
        delete myMenu;
        myMenu = new KPopupMenu;
        connect( myMenu, SIGNAL( activated( int )),
                 SLOT( slotItemSelected( int )));

        for ( action = it.current(); action; action = ++it ) {
            QListIterator<ClipCommand> it2( action->commands() );
            if ( it2.count() > 0 )
                myMenu->insertTitle( kapp->miniIcon(), action->description() +
				     i18n(" -  Actions for: ") +
				     KStringHandler::csqueeze(myClipData, 45));
            for ( command = it2.current(); command; command = ++it2 ) {
                item = command->description;
                if ( item.isEmpty() )
                    item = command->command;

                int id;
                if ( command->pixmap.isEmpty() )
                    id = myMenu->insertItem( item );
                else
                    id = myMenu->insertItem( SmallIcon(command->pixmap), item);
                myCommandMapper.insert( id, command );
            }
        }

        // add an edit-possibility
        myMenu->insertSeparator();
        myMenu->insertSeparator();
        myMenu->insertItem( SmallIcon("edit"), i18n("&Edit contents..."), 
                            URL_EDIT_ITEM );
        myMenu->insertItem( i18n("&Cancel"), DO_NOTHING_ITEM );

        if ( myPopupKillTimer > 0 )
            myPopupKillTimer->start( 1000 * myPopupKillTimeout, true );

        emit sigPopup( myMenu );
    }
}


void URLGrabber::slotItemSelected( int id )
{
    myMenu->hide(); // deleted by the timer or the next action

    switch ( id ) {
    case -1:
    case DO_NOTHING_ITEM:
        break;
    case URL_EDIT_ITEM:
        editData();
        break;
    default:
        ClipCommand *command = myCommandMapper.find( id );
        if ( !command )
            qWarning("Klipper: can't find associated action");
        else
            execute( command );
    }
}


void URLGrabber::execute( const struct ClipCommand *command ) const
{
    if ( command->isEnabled ) {
        QString cmdLine = command->command;

        // escape $ to avoid it being expanded by the shell
        QString escClipData = myClipData;
        escClipData.replace( QRegExp( "\\$" ), "\\$" );

        // replace "%s" with the clipboard contents
        // replace \%s to %s
        int pos = 0;
        
        while ( (pos = cmdLine.find("%s", pos)) >= 0 ) {
            if ( pos > 0 && cmdLine.at( pos -1 ) == '\\' ) {
                cmdLine.remove( pos -1, 1 ); // \%s -> %s
                pos++;
            }
            else {
                cmdLine.replace( pos, 2, escClipData );
                pos += escClipData.length();
            }
        }

        startProcess( cmdLine );
    }
}


void URLGrabber::startProcess( const QString& cmdLine ) const
{
    kdDebug() << "now starting " << cmdLine << endl;
    if ( cmdLine.isEmpty() )
        return;

    KShellProcess proc;
    proc << cmdLine.simplifyWhiteSpace().stripWhiteSpace();

    if ( !proc.start(KProcess::DontCare, KProcess::NoCommunication ))
        qWarning("Klipper: Couldn't start process!");
}


void URLGrabber::editData()
{
    myPopupKillTimer->stop();
    KDialogBase *dlg = new KDialogBase( 0, 0, true,
                                        i18n("Edit contents"),
                                        KDialogBase::Ok | KDialogBase::Cancel);
    KEdit *edit = new KEdit( dlg );
    edit->setText( myClipData );
    edit->setMinimumSize( 300, 40 );
    dlg->setMainWidget( edit );
    dlg->adjustSize();

    if ( dlg->exec() == QDialog::Accepted ) {
        myClipData = edit->text();
        delete dlg;
        QTimer::singleShot( 0, this, SLOT( slotActionMenu() ) );
    }
    else
    {
        delete dlg;
        QTimer::singleShot( 0, this, SLOT( slotKillPopupMenu() ) );
    }
}


void URLGrabber::readConfiguration( KConfig *kc )
{
    myActions->clear();
    kc->setGroup( "General" );
    int num = kc->readNumEntry("Number of Actions", 0);
    myAvoidWindows = kc->readListEntry("No Actions for WM_CLASS");
    myPopupKillTimeout = kc->readNumEntry( "Timeout for Action popups (seconds)", 8 );

    QString group;
    for ( int i = 0; i < num; i++ ) {
        group = QString("Action_%1").arg( i );
        kc->setGroup( group );
        myActions->append( new ClipAction( kc ) );
    }
}


void URLGrabber::writeConfiguration( KConfig *kc )
{
    kc->setGroup( "General" );
    kc->writeEntry( "Number of Actions", myActions->count() );
    kc->writeEntry( "Timeout for Action popups (seconds)", myPopupKillTimeout);
    kc->writeEntry( "No Actions for WM_CLASS", myAvoidWindows );

    ActionListIterator it( *myActions );
    ClipAction *action;

    int i = 0;
    QString group;
    while ( (action = it.current()) ) {
        group = QString("Action_%1").arg( i );
        kc->setGroup( group );
        action->save( kc );
        ++i;
        ++it;
    }
}

// find out whether the active window's WM_CLASS is in our avoid-list
// digged a little bit in netwm.cpp
bool URLGrabber::isAvoidedWindow() const
{
    Display *d = qt_xdisplay();
    static Atom wm_class = XInternAtom( d, "WM_CLASS", true );
    static Atom active_window = XInternAtom( d, "_NET_ACTIVE_WINDOW", true );
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret, unused;
    unsigned char *data_ret;
    long BUFSIZE = 2048;
    bool ret = false;
    Window active = 0L;
    QString wmClass;

    // get the active window
    if (XGetWindowProperty(d, DefaultRootWindow( d ), active_window, 0l, 1l,
                           False, XA_WINDOW, &type_ret, &format_ret,
                           &nitems_ret, &unused, &data_ret)
        == Success) {
        if (type_ret == XA_WINDOW && format_ret == 32 && nitems_ret == 1) {
            active = *((Window *) data_ret);
        }
        XFree(data_ret);
    }
    if ( !active )
        return false;

    // get the class of the active window
    if ( XGetWindowProperty(d, active, wm_class, 0L, BUFSIZE, False, XA_STRING,
                            &type_ret, &format_ret, &nitems_ret,
                            &unused, &data_ret ) == Success) {
        if ( type_ret == XA_STRING && format_ret == 8 && nitems_ret > 0 ) {
            wmClass = QString::fromUtf8( (const char *) data_ret );
            ret = (myAvoidWindows.find( wmClass ) != myAvoidWindows.end());
        }

        XFree( data_ret );
    }

    return ret;
}


void URLGrabber::slotKillPopupMenu()
{
    delete myMenu;
    myMenu = 0L;
}

///////////////////////////////////////////////////////////////////////////
////////

ClipCommand::ClipCommand(const QString &_command, const QString &_description,
                         bool _isEnabled)
    : command(_command),
      description(_description),
      isEnabled(_isEnabled)
{
    int len = command.find(" ");
    if (len == -1)
        len = command.length();
    KService::Ptr service= KService::serviceByDesktopName(command.left(len));
    if (service)
        pixmap = service->icon();
    else
        pixmap = QString::null;
}


ClipAction::ClipAction( const QString& regExp, const QString& description )
{
    myCommands.setAutoDelete( true );
    myRegExp      = regExp;
    myDescription = description;
}


ClipAction::ClipAction( const ClipAction& action )
{
    myCommands.setAutoDelete( true );
    myRegExp      = action.myRegExp;
    myDescription = action.myDescription;

    ClipCommand *command = 0L;
    QListIterator<ClipCommand> it( myCommands );
    for ( ; it.current(); ++it ) {
        command = it.current();
        addCommand(command->command, command->description, command->isEnabled);
    }
}


ClipAction::ClipAction( KConfig *kc )
{
    myCommands.setAutoDelete( true );
    myRegExp      = kc->readEntry( "Regexp" );
    myDescription = kc->readEntry( "Description" ); // i18n'ed
    int num = kc->readNumEntry( "Number of commands" );

    // read the commands
    QString actionGroup = kc->group();
    for ( int i = 0; i < num; i++ ) {
        QString group = actionGroup + "/Command_%1";
        kc->setGroup( group.arg( i ) );

        addCommand( kc->readEntry( "Commandline" ),
                    kc->readEntry( "Description" ), // i18n'ed
                    kc->readBoolEntry( "Enabled" ) );
    }
}


ClipAction::~ClipAction()
{
}


void ClipAction::addCommand( const QString& command,
                             const QString& description, bool enabled )
{
    if ( command.isEmpty() )
        return;

    struct ClipCommand *cmd = new ClipCommand( command, description, enabled );
    //    cmd->id = myCommands.count(); // superfluous, I think...
    myCommands.append( cmd );
}


// precondition: we're in the correct action's group of the KConfig object
void ClipAction::save( KConfig *kc ) const
{
    kc->writeEntry( "Description", description() );
    kc->writeEntry( "Regexp", regExp() );
    kc->writeEntry( "Number of commands", myCommands.count() );

    QString actionGroup = kc->group();
    struct ClipCommand *cmd;
    QListIterator<struct ClipCommand> it( myCommands );

    // now iterate over all commands of this action
    int i = 0;
    while ( (cmd = it.current()) ) {
        QString group = actionGroup + "/Command_%1";
        kc->setGroup( group.arg( i ) );

        kc->writeEntry( "Commandline", cmd->command );
        kc->writeEntry( "Description", cmd->description );
        kc->writeEntry( "Enabled", cmd->isEnabled );

        ++i;
        ++it;
    }
}

#include "urlgrabber.moc"
