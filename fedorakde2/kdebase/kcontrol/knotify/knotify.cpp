/*
    $Id: knotify.cpp,v 1.45 2001/05/23 01:06:09 mhunter Exp $

    Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include <qdir.h>
#include <qglobal.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qsplitter.h>
#include <qtimer.h>
#include <qvgroupbox.h>

#include <kaboutdata.h>
#include <kapp.h>
#include <kaudioplayer.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kstddirs.h>
#include <kurlcompletion.h>
#include <kurlrequester.h>

#include "knotify.h"
#include "knotify.moc"

static const int COL_FILENAME = 1;

KNotifyWidget::KNotifyWidget(QWidget *parent, const char *name):
    KCModule(parent, name)
{
    updating = true;
    currentItem = 0L;
    setButtons( Help | Apply );

    QVBoxLayout *lay = new QVBoxLayout( this, KDialog::marginHint(),
					KDialog::spacingHint() );
    QVGroupBox *box = new QVGroupBox( i18n("System notification settings"),
				      this );
    lay->addWidget( box );
    view =  new QListView( box );
    view->addColumn(i18n("Application/Events"));
    view->addColumn(i18n("Filename"));
    view->setSelectionMode( QListView::Single );
    view->setRootIsDecorated( true );
    view->setSorting( -1 );

    QHBox *hbox = new QHBox( box );
    hbox->setSpacing( KDialog::spacingHint() );
    QLabel *l = new QLabel( i18n("&Filename: "), hbox );
    requester = new KURLRequester( hbox );
    l->setBuddy( requester );
    connect( requester, SIGNAL( openFileDialog( KURLRequester * )),
	     SLOT( slotRequesterClicked( KURLRequester * )));

    playButton = new QPushButton(  hbox );
    playButton->setFixedSize( requester->button()->size() );
    playButton->setPixmap( UserIcon("play") );
    QToolTip::add( playButton, i18n("Play the given sound") );
    playButton->hide();

    connect( playButton, SIGNAL( clicked() ), SLOT( playSound() ));
    connect(requester, SIGNAL( textChanged( const QString& )),
	    SLOT( slotFileChanged( const QString& )) );
    connect( view, SIGNAL( currentChanged( QListViewItem * )),
	     SLOT( slotItemActivated( QListViewItem * )));


    hbox = new QHBox( box );
    hbox->setSpacing( KDialog::spacingHint() );
    cbExternal = new QCheckBox( i18n("Use e&xternal player: "), hbox );
    reqExternal = new KURLRequester( hbox );
    reqExternal->completionObject()->setMode( KURLCompletion::ExeCompletion );
    connect( cbExternal, SIGNAL( toggled( bool )),
	     SLOT( externalClicked( bool )));
    connect( reqExternal, SIGNAL( textChanged( const QString& )),
	     SLOT( changed() ));

    hbox = new QHBox( box );
    hbox->setSpacing( KDialog::spacingHint() );
    l = new QLabel( i18n( "&Volume: " ), hbox );
    volumeSlider = new QSlider( hbox );
    volumeSlider->setOrientation( Horizontal );
    volumeSlider->setRange( 0, 100 );
    connect( volumeSlider, SIGNAL( valueChanged( int ) ), SLOT( changed() ) );
    l->setBuddy( volumeSlider );

    m_events = new Events();
    qApp->processEvents(); // let's show up

    // reading can take some time
    QTimer::singleShot( 0, this, SLOT( load() ));
    updating = false;
};

KNotifyWidget::~KNotifyWidget()
{
    delete m_events;
}


/**
 * Clears the view and iterates over all apps, creating listview-items
 */
void KNotifyWidget::updateView()
{
    bool save_updating = updating;
    updating = true;
    view->clear();
    QListViewItem *appItem = 0L;
    KNListViewItem *eItem  = 0L;
    KNEvent *e;

    QPixmap icon = SmallIcon("idea");

    // using the last appItem and eItem as "after-item" to get proper sorting
    KNApplicationListIterator it( m_events->apps() );
    while ( it.current() ) {
	appItem = new QListViewItem( view, appItem, (*it)->text() );
	appItem->setPixmap( 0, SmallIcon( (*it)->icon() ));

	KNEventListIterator it2( *(*it)->eventList() );
	while( (e = it2.current()) ) {
	    eItem = new KNListViewItem( appItem, eItem, e );
	    eItem->setPixmap( 0, icon );

	    connect( eItem, SIGNAL( changed() ), SLOT( changed() ));
	    ++it2;
	}

	++it;
    }
    updating = save_updating;
}


// FIXME: this doesn't work, it's a Reset, not a Defaults.
void KNotifyWidget::defaults()
{
    if (KMessageBox::warningContinueCancel(this,
        i18n("This will cause the notifications for *All Applications* "
             "to be reset to their defaults!"), i18n("Are you sure?!"), i18n("Continue"))
        != KMessageBox::Continue)
        return;

    load();
}

void KNotifyWidget::changed()
{
    if (!updating)
       emit KCModule::changed(true);
}

/**
 * Someone typing in the url-requester -> update the listview item and its
 * event.
 */
void KNotifyWidget::slotFileChanged( const QString& text )
{
    playButton->setEnabled( !text.isEmpty() );

    if ( !currentItem )
	return;

    KNEvent *event = currentItem->event;
    QString *itemText = 0L;

    if ( currentItem->eventType() == KNotifyClient::Sound )
	itemText = &(event->soundfile);
    else if ( currentItem->eventType() == KNotifyClient::Logfile )
	itemText = &(event->logfile);

    if ( itemText && *itemText != text ) {
	*itemText = text;
	changed();
    }

    currentItem->setText( COL_FILENAME, text );
}

void KNotifyWidget::playSound()
{
    KAudioPlayer::play( requester->url() );
}

void KNotifyWidget::load()
{
    bool save_updating = updating;
    updating = true;

    setEnabled( false );
    setCursor( KCursor::waitCursor() );
    currentItem = 0L;

    KConfig *kc = new KConfig( "knotifyrc", true, false );
    kc->setGroup( "Misc" );
    cbExternal->setChecked( kc->readBoolEntry( "Use external player", false ));
    reqExternal->setURL( kc->readEntry( "External player" ));
    reqExternal->setEnabled( cbExternal->isChecked() );
    volumeSlider->setValue( kc->readNumEntry( "Volume", 100 ) );
    static_cast<QHBox *>( volumeSlider->parent() )->setEnabled( !cbExternal->isChecked() );
    delete kc;

    requester->clear();
    requester->setEnabled( false );
    playButton->hide();

    view->clear();
    m_events->load();
    updateView();
    setEnabled( true );
    unsetCursor();

    updating = save_updating;
}

void KNotifyWidget::save()
{
    // see kdelibs/arts/knotify/knotify.cpp
    KConfig *kc = new KConfig( "knotifyrc", false, false );
    kc->setGroup( "Misc" );
    kc->writeEntry( "External player", reqExternal->url() );
    kc->writeEntry( "Use external player", cbExternal->isChecked() );
    kc->writeEntry( "Volume", volumeSlider->value() );
    kc->sync();
    delete kc;
	
    m_events->save();
    if ( !kapp->dcopClient()->isAttached() )
	kapp->dcopClient()->attach();
    kapp->dcopClient()->send("knotify", "", "reconfigure()", "");

    emit KCModule::changed( false );
}

void KNotifyWidget::slotItemActivated( QListViewItem *i )
{
    bool enableButton = false;
    currentItem = dynamic_cast<KNCheckListItem *>( i );
    if ( currentItem ) {
	const KNEvent *event = currentItem->event;

	if ( currentItem->eventType() == KNotifyClient::Sound ) {
	    requester->setURL( event->soundfile );
	    enableButton = true;
	    playButton->show();
	    playButton->setEnabled( !event->soundfile.isEmpty() );
	}
	else if ( currentItem->eventType() == KNotifyClient::Logfile ) {
	    requester->setURL( event->logfile );
	    enableButton = true;
	    playButton->hide();
	}
	else {
	    requester->lineEdit()->clear();
	    playButton->hide();
	}
    }
    else {
	requester->lineEdit()->clear();
	playButton->hide();
    }

    requester->setEnabled( enableButton );
}

void KNotifyWidget::externalClicked( bool on )
{
    if ( on )
	reqExternal->setFocus();
    reqExternal->setEnabled( on );
    static_cast<QHBox *>( volumeSlider->parent() )->setEnabled( !on );
    changed();
}

QString KNotifyWidget::quickHelp() const
{
    return i18n("<h1>System Notifications</h1>"
		"KDE allows for a great deal of control over how you "
		"will be notified when certain events occur.  There are "
		"several choices as to how you are notified:"
		"<ul><li>As the application was originally designed."
		"<li>With a beep or other noise."
		"<li>Via a popup dialog box with additional information."
		"<li>By recording the event in a logfile without "
		"any additional visual or audible alert."
		"</ul>");
}

const KAboutData *KNotifyWidget::aboutData() const
{
    static KAboutData* ab = 0;

    if(!ab)
    {
        ab = new KAboutData(
            "kcmnotify", I18N_NOOP("KNotify"), "2.0",
            I18N_NOOP("System Notification Control Panel Module"),
            KAboutData::License_GPL, 0, 0, 0 );
        ab->addAuthor( "Carsten Pfeiffer", 0, "pfeiffer@kde.org" );
        ab->addCredit( "Charles Samuels", I18N_NOOP("Original implementation"),
                       "charles@altair.dhs.org" );
    }

    return ab;
}

void KNotifyWidget::slotRequesterClicked( KURLRequester *requester )
{
    static bool init = true;
    if ( !init )
	return;
    
    // find the first "sound"-resource that contains files
    QStringList soundDirs = KGlobal::dirs()->resourceDirs( "sound" );
    if ( !soundDirs.isEmpty() ) {
	KURL soundURL;
	QDir dir;
	dir.setFilter( QDir::Files | QDir::Readable );
	QStringList::Iterator it = soundDirs.begin();
	while ( it != soundDirs.end() ) {
	    dir = *it;
	    if ( dir.isReadable() && dir.count() > 2 ) {
		soundURL.setPath( *it );
		requester->fileDialog()->setURL( soundURL );
		break;
	    }
	    ++it;
	}
    }
}


///////////////////////////////////////////////////////////////////

/**
 * Custom item that represents a KNotify-event
 * creates and handles checkable child-items
 */
KNListViewItem::KNListViewItem( QListViewItem *parent,
				QListViewItem *afterItem, KNEvent *e )
    : QListViewItem( parent, afterItem, e->text() )
{
    event = e;

    if ( (e->dontShow & KNotifyClient::Stderr) == 0 ) {
	stderrItem = new KNCheckListItem( this, event, KNotifyClient::Stderr,
					  i18n("Standard error output"));
	stderrItem->setOn( e->presentation & KNotifyClient::Stderr );
    }

    if ( (e->dontShow & KNotifyClient::Messagebox) == 0 ) {
	msgboxItem = new KNCheckListItem(this, event,KNotifyClient::Messagebox,
					  i18n("Show messagebox"));
	msgboxItem->setOn( e->presentation & KNotifyClient::Messagebox );
    }

    if ( (e->dontShow & KNotifyClient::Sound) == 0 ) {
	soundItem = new KNCheckListItem( this, event, KNotifyClient::Sound,
					 i18n("Play sound"));

	soundItem->setOn( e->presentation & KNotifyClient::Sound );
//        kdDebug() << "******* soundfile: " << e->soundfile << " " << bool(e->presentation & KNotifyClient::Sound) << " " << soundItem->isOn() << endl;
        soundItem->setText( COL_FILENAME, e->soundfile );
    }

    if ( (e->dontShow & KNotifyClient::Logfile) == 0 ) {
	logItem = new KNCheckListItem( this, event, KNotifyClient::Logfile,
				       i18n("Log to file"));
	logItem->setOn( e->presentation & KNotifyClient::Logfile  );
	//	kdDebug() << "******** logfile: " << e->logfile << endl;
	logItem->setText( COL_FILENAME, e->logfile );
    }
}

/**
 * a child has changed -> update the KNEvent
 * not implemented as signal/slot to avoid lots of QObjects and connects
 */
void KNListViewItem::itemChanged( KNCheckListItem *item )
{
    if ( item->isOn() )
	event->presentation |= item->eventType();
    else
	event->presentation &= ~item->eventType();

    changed();
}



//////////////////////////////////////////////////////////////////////

/**
 * custom checkable item telling its parent when it was clicked
 */
KNCheckListItem::KNCheckListItem( QListViewItem *parent, KNEvent *e, int t,
				  const QString& text )
    : QCheckListItem( parent, text, QCheckListItem::CheckBox ),
      event( e ),
      _eventType( t )

{
}

void KNCheckListItem::stateChange( bool b )
{
    ((KNListViewItem *) parent())->itemChanged( this );
    QCheckListItem::stateChange(b);
}
