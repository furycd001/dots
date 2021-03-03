/* -------------------------------------------------------------

   toplevel.cpp (part of Klipper - Cut & paste history for KDE)

   (C) by Andrew Stanley-Jones
   (C) 2000 by Carsten Pfeiffer <pfeiffer@kde.org>

   Generated with the KDE Application Generator

 ------------------------------------------------------------- */

#include <qclipboard.h>
#include <qcursor.h>
#include <qfile.h>
#include <qintdict.h>
#include <qpainter.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kstringhandler.h>
#include <kwin.h>
#include <kdebug.h>

#include "configdialog.h"
#include "toplevel.h"
#include "urlgrabber.h"


#define QUIT_ITEM    50
#define CONFIG_ITEM  60
#define EMPTY_ITEM   80

#define MENU_ITEMS   7
// the <clipboard empty> item
#define EMPTY (pQPMmenu->count() - MENU_ITEMS)


/* XPM */
static const char*mouse[]={
"20 20 8 1",
"d c #ffa858",
"e c #c05800",
"# c #000000",
"c c #ff8000",
". c None",
"b c #a0a0a4",
"a c #dcdcdc",
"f c #ffffff",
".....###########....",
"..####aaaaaaab####..",
".#cccc#a....b#cccc#.",
".#cd####abbb####de#.",
".#cd#fff####ff.#de#.",
".#cd#fffffffff.#de#.",
".#cd#f.######f.#de#.",
".#cd#fffffffff.#de#.",
".#cd#f.#####ff.#de#.",
".#cd#fffffffff.#de#.",
".#cd#ff#ff#fff.#de#.",
".#cd#ff#f#ffff.#de#.",
".#cd#ff##fffff.#de#.",
".#cd#ff#f#ffff.#de#.",
".#cd#ff#ff#fff.#de#.",
".#cd#fffffffff.#de#.",
".#cd#..........#de#.",
".#cd############de#.",
".#ccccccccccccccde#.",
"..################.."};

TopLevel::TopLevel()
  : KMainWindow(0)
{
    clip = kapp->clipboard();
    pSelectedItem = -1;
    QSempty = i18n("<empty clipboard>");

    toggleURLGrabAction = new KToggleAction( this );
    toggleURLGrabAction->setEnabled( true );

    myURLGrabber = 0L;
    KConfig *kc = kapp->config();
    readConfiguration( kc );
    setURLGrabberEnabled( bURLGrabber );

    QSlast = "";
    pQPMmenu = new KPopupMenu(0x0, "main_menu");
    connect(pQPMmenu, SIGNAL(activated(int)),
            this, SLOT(clickedMenu(int)));

    pQIDclipData = new QIntDict<QString>();
    pQIDclipData->setAutoDelete(TRUE);

    readProperties(kapp->config());
    connect(kapp, SIGNAL(saveYourself()), SLOT(saveProperties()));

    pQTcheck = new QTimer(this, "timer");
    pQTcheck->start(1000, FALSE);
    connect(pQTcheck, SIGNAL(timeout()),
            this, SLOT(newClipData()));
    pQPpic = new QPixmap(mouse);
    resize( pQPpic->size() );

    globalKeys = new KGlobalAccel();
    KGlobalAccel* keys = globalKeys;
#include "klipperbindings.cpp"
    globalKeys->connectItem("show-klipper-popupmenu", this,
                            SLOT(slotPopupMenu()));
    globalKeys->connectItem("repeat-last-klipper-action", this,
                            SLOT(slotRepeatAction()));
    globalKeys->connectItem("toggle-clipboard-actions", this,
                            SLOT( toggleURLGrabber()));
    globalKeys->readSettings();
    toggleURLGrabAction->setAccel( globalKeys->currentKey( "toggle-clipboard-actions" ));

    connect( toggleURLGrabAction, SIGNAL( toggled( bool )), this,
	     SLOT( setURLGrabberEnabled( bool )));
    setBackgroundMode(X11ParentRelative);
}

TopLevel::~TopLevel()
{
    delete globalKeys;
    delete pQTcheck;
    delete pQPMmenu;
    delete pQIDclipData;
    delete pQPpic;
    delete myURLGrabber;
}

void TopLevel::mousePressEvent(QMouseEvent *e)
{
    if ( e->button() == LeftButton || e->button() == RightButton )
        showPopupMenu( pQPMmenu );
}

void TopLevel::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  int x = (width() - pQPpic->width()) / 2;
  int y = (height() - pQPpic->height()) / 2;
  if ( x < 0 ) x = 0;
  if ( y < 0 ) y = 0;
  p.drawPixmap(x , y, *pQPpic);
  p.end();
}

void TopLevel::newClipData()
{
    QString clipData = clip->text().stripWhiteSpace();
    // If the string is null bug out
    if(clipData.isEmpty()) {
	if (pSelectedItem != -1) {
            pQPMmenu->setItemChecked(pSelectedItem, false);
	    pSelectedItem = -1;
	}
        return;
    }

    if(clipData != QSlast) {
        QSlast = clipData.copy();

        QString *data = new QString(clipData);

        if ( bURLGrabber && myURLGrabber ) {
            if ( myURLGrabber->checkNewData( clipData ))
                return; // don't add into the history
        }

        if (bClipEmpty) { // remove <clipboard empty> from popupmenu and dict
            if (*data != QSempty) {
                bClipEmpty = false;
                pQPMmenu->removeItemAt(EMPTY);
                pQIDclipData->clear();
            }
        }

	trimClipHistory(maxClipItems - 1);

        if(clipData.length() > 50){
            clipData.truncate(47);
            clipData.append("...");
        }
        long int id = pQPMmenu->insertItem(KStringHandler::csqueeze(clipData.simplifyWhiteSpace(), 45), -2, 1); // -2 means unique id, 1 means first location
        pQIDclipData->insert(id, data);

        if (pSelectedItem != -1)
	{
            pQPMmenu->setItemChecked(pSelectedItem, false);
	}
        pSelectedItem = id;

	if ( bClipEmpty )
	{
	    clip->clear();
	    pQPMmenu->setItemEnabled(pSelectedItem, false);
	}
	else
	{
	    pQPMmenu->setItemChecked(pSelectedItem, true);
	}
    }
}

void TopLevel::clickedMenu(int id)
{
    switch ( id ) {
    case -1:
        break;
    case CONFIG_ITEM:
        slotConfigure();
        break;
    case QUIT_ITEM: {
        saveProperties();
        int autoStart = KMessageBox::questionYesNo( 0L, i18n("Should Klipper start automatically\nwhen you login?"), i18n("Automatically start Klipper?") );

        QString file = locateLocal( "data", "../autostart/klipper.desktop" );
        if ( autoStart == KMessageBox::Yes )
            QFile::remove( file );
        else {
            KSimpleConfig config( file );
            config.setDesktopGroup();
            config.writeEntry( "Hidden", true );
            config.sync();
        }
        kapp->quit();
        break;
        }
//    case URLGRAB_ITEM: // handled with an extra slot
//	break;
    case EMPTY_ITEM:
	if ( !bClipEmpty )
	{
	    pQTcheck->stop();

	    trimClipHistory(0);
	    pQIDclipData->clear();
	
	    bClipEmpty = true;
	    clip->setText(QSempty);
	    newClipData();
	    pQTcheck->start(1000);
	}
	break;
    default:
	if ( id == URLGrabItem )
	{
	    break; // handled by its own slot
	}
	else if ( !bClipEmpty )
	{
	    pQTcheck->stop();
	    //CT mark up the currently put into clipboard - so that user can see later
	    pQPMmenu->setItemChecked(pSelectedItem, false);
	    pSelectedItem = id;
	    pQPMmenu->setItemChecked(pSelectedItem, true);
	    QString *data = pQIDclipData->find(id);
	    if(data != 0x0 && *data != QSempty){
		clip->setText(*data);
		if (bURLGrabber && bReplayActionInHistory)
		    myURLGrabber->checkNewData(*data);
		QSlast = data->copy();
	    }

	    pQTcheck->start(1000);
	}
    }
}


void TopLevel::showPopupMenu( QPopupMenu *menu )
{
    ASSERT( menu != 0L );

    menu->move(-1000,-1000);
    menu->show();
    menu->hide();

    if (bPopupAtMouse) {
        QPoint g = QCursor::pos();
        if ( menu->height() < g.y() )
            menu->popup(QPoint( g.x(), g.y() - menu->height()));
        else
            menu->popup(QPoint(g.x(), g.y()));
    }

    else {
        KWin::Info i = KWin::info( winId() );
        QRect g = i.geometry;

        if ( g.x() > QApplication::desktop()->width()/2 &&
             g.y() + menu->height() > QApplication::desktop()->height() )
            menu->popup(QPoint( g.x(), g.y() - menu->height()));
        else
            menu->popup(QPoint( g.x() + width(), g.y() + height()));

        //      menu->exec(mapToGlobal(QPoint( width()/2, height()/2 )));
    }
}


void TopLevel::readProperties(KConfig *kc)
{
  QStringList dataList;
  long int id;

  pQPMmenu->clear();
  pQPMmenu->insertTitle(kapp->miniIcon(), i18n("Klipper - Clipboard Tool"));

  if (bKeepContents) { // load old clipboard if configured
      KConfigGroupSaver groupSaver(kc, "General");
      dataList = kc->readListEntry("ClipboardData");

      for (QStringList::ConstIterator it = dataList.begin();
           it != dataList.end(); ++it)
      {
          id = pQPMmenu->insertItem( KStringHandler::csqueeze(*it, 45), -2, -1 );
          pQIDclipData->insert( id, new QString( *it ));
      }
  }

  bClipEmpty = ((QString)clip->text()).simplifyWhiteSpace().isEmpty() && dataList.isEmpty();

  pQPMmenu->insertSeparator();
  toggleURLGrabAction->plug( pQPMmenu, -1 );
  URLGrabItem = pQPMmenu->idAt( pQPMmenu->count() - 1 );

  pQPMmenu->insertItem( SmallIcon("fileclose"),
			i18n("&Clear Clipboard History"), EMPTY_ITEM );
  pQPMmenu->insertItem(SmallIcon("configure"), i18n("&Preferences..."),
		       CONFIG_ITEM);
  pQPMmenu->insertSeparator();
  pQPMmenu->insertItem(SmallIcon("exit"), i18n("&Quit"), QUIT_ITEM );
  pQPMmenu->insertTearOffHandle();

  if(bClipEmpty)
    clip->setText(QSempty);

  newClipData();
}


void TopLevel::readConfiguration( KConfig *kc )
{
    kc->setGroup("General");
    bPopupAtMouse = kc->readBoolEntry("PopupAtMousePosition", false);
    bKeepContents = kc->readBoolEntry("KeepClipboardContents", true);
    bURLGrabber = kc->readBoolEntry("URLGrabberEnabled", true);
    bReplayActionInHistory = kc->readBoolEntry("ReplayActionInHistory", false);
    maxClipItems = kc->readNumEntry("MaxClipItems", 7);
}

void TopLevel::writeConfiguration( KConfig *kc )
{
    kc->setGroup("General");
    kc->writeEntry("PopupAtMousePosition", bPopupAtMouse);
    kc->writeEntry("KeepClipboardContents", bKeepContents);
    kc->writeEntry("ReplayActionInHistory", bReplayActionInHistory);
    kc->writeEntry("MaxClipItems", maxClipItems);
    kc->writeEntry("Version", kapp->aboutData()->version());

    if ( myURLGrabber )
        myURLGrabber->writeConfiguration( kc );

    kc->sync();
}


// session management
void TopLevel::saveProperties()
{
  if (bKeepContents) { // save the clipboard eventually
      QString  *data;
      QStringList dataList;
      KConfig  *kc = kapp->config();
      KConfigGroupSaver groupSaver(kc, "General");
      QIntDictIterator<QString> it( *pQIDclipData );
      if ( !bClipEmpty )
      {
	  while ( (data = it.current()) ) {
	      dataList.prepend( *it );
	      ++it;
	  }
      }
      kc->writeEntry("ClipboardData", dataList);
      kc->sync();
  }
}


void TopLevel::slotConfigure()
{
    bool haveURLGrabber = bURLGrabber;
    if ( !myURLGrabber ) { // temporary, for the config-dialog
        setURLGrabberEnabled( true );
        readConfiguration( kapp->config() );
    }

    KKeyEntryMap map = globalKeys->keyDict();
    ConfigDialog *dlg = new ConfigDialog( myURLGrabber->actionList(),
                                          &map );
    dlg->setKeepContents( bKeepContents );
    dlg->setPopupAtMousePos( bPopupAtMouse );
    dlg->setReplayActionInHistory( bReplayActionInHistory);
    dlg->setPopupTimeout( myURLGrabber->popupTimeout() );
    dlg->setMaxItems( maxClipItems );
    dlg->setNoActionsFor( myURLGrabber->avoidWindows() );
//    dlg->setEnableActions( haveURLGrabber );

    if ( dlg->exec() == QDialog::Accepted ) {
        bKeepContents = dlg->keepContents();
        bPopupAtMouse = dlg->popupAtMousePos();
        bReplayActionInHistory = dlg->replayActionInHistory();
        globalKeys->setKeyDict( map );
        globalKeys->writeSettings();
	toggleURLGrabAction->setAccel( globalKeys->currentKey( "toggle-clipboard-actions" ));

//	haveURLGrabber = dlg->enableActions();

        myURLGrabber->setActionList( dlg->actionList() );
        myURLGrabber->setPopupTimeout( dlg->popupTimeout() );
	myURLGrabber->setAvoidWindows( dlg->noActionsFor() );

	maxClipItems = dlg->maxItems();
	trimClipHistory( maxClipItems );
	
        writeConfiguration( kapp->config() );
    }
    setURLGrabberEnabled( haveURLGrabber );

    delete dlg;
}


void TopLevel::slotRepeatAction()
{
    if ( !myURLGrabber ) {
	myURLGrabber = new URLGrabber();
	connect( myURLGrabber, SIGNAL( sigPopup( QPopupMenu * )),
		 SLOT( showPopupMenu( QPopupMenu * )) );
    }

    myURLGrabber->invokeAction( QSlast );
}

void TopLevel::setURLGrabberEnabled( bool enable )
{
    bURLGrabber = enable;
    toggleURLGrabAction->setChecked( enable );
    KConfig *kc = kapp->config();
    kc->setGroup("General");
    kc->writeEntry("URLGrabberEnabled", bURLGrabber);
    kc->sync();

    if ( !bURLGrabber ) {
        delete myURLGrabber;
        myURLGrabber = 0L;
	toggleURLGrabAction->setText(i18n("Enable &actions"));
    }

    else {
	toggleURLGrabAction->setText(i18n("&Actions enabled"));
        if ( !myURLGrabber ) {
            myURLGrabber = new URLGrabber();
            connect( myURLGrabber, SIGNAL( sigPopup( QPopupMenu * )),
                     SLOT( showPopupMenu( QPopupMenu * )) );
        }
    }
}

void TopLevel::toggleURLGrabber()
{
    setURLGrabberEnabled( !bURLGrabber );
}

void TopLevel::trimClipHistory( int new_size )
{
    while(pQPMmenu->count() - MENU_ITEMS > (unsigned)new_size){
	int id = pQPMmenu->idAt(EMPTY);

	pQIDclipData->remove(id);
	pQPMmenu->removeItemAt(EMPTY);
    }
}
#include "toplevel.moc"
