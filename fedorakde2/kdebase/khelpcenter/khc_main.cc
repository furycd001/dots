 /*
 *  khc_main.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *                2001 Stephan Kulow (coolo@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>

#include <kapp.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <kstringhandler.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <assert.h>

#include "khc_main.h"
#include "khc_view.h"

#include <kaboutdata.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <qtimer.h>

#include "version.h"
#include <khtml_part.h>
#include <qlayout.h>
#include <kaction.h>
#include <qsplitter.h>
#include "khc_navigator.h"
#include <qhbox.h>

KHMainWindow::KHMainWindow(const KURL &url)
    : KMainWindow(0, "khmainwindow"), m_goMenuIndex( -1 ), m_goMenuHistoryStartPos( -1 ),
      m_goMenuHistoryCurrentPos( -1 )
{
    splitter = new QSplitter(this);
    m_goBuffer=0;

    doc = new KHCView( splitter, 0,
                       this, 0, KHTMLPart::BrowserViewGUI );
    connect(doc, SIGNAL(setWindowCaption(const QString &)),
            SLOT(setCaption(const QString &)));
    connect(doc, SIGNAL(setStatusBarText(const QString &)),
            statusBar(), SLOT(message(const QString &)));
    connect(doc, SIGNAL(onURL(const QString &)),
            statusBar(), SLOT(message(const QString &)));
    connect(doc, SIGNAL(started(KIO::Job *)),
            SLOT(slotStarted(KIO::Job *)));
    connect(doc, SIGNAL(completed()),
            SLOT(documentCompleted()));

    statusBar()->message(i18n("Preparing Index"));

    connect(doc->browserExtension(),
            SIGNAL(openURLRequest( const KURL &,
                                   const KParts::URLArgs &)),
            SLOT(slotOpenURLRequest( const KURL &,
                                     const KParts::URLArgs &)));

    nav = new khcNavigator(splitter, this, "nav");
    connect(nav->widget(), SIGNAL(itemSelected(const QString &)),
            SLOT(openURL(const QString &)));
    connect(nav->widget(), SIGNAL(glossSelected(const khcNavigatorWidget::GlossaryEntry &)),
            SLOT(slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &)));

    splitter->moveToFirst(nav->widget());
    splitter->setResizeMode(nav->widget(), QSplitter::KeepSize);
    setCentralWidget( splitter );
    QValueList<int> sizes;
    sizes << 220 << 580;
    splitter->setSizes(sizes);
    setGeometry(366, 0, 800, 600);

    (*actionCollection()) += *doc->actionCollection();
    (void)KStdAction::quit(this, SLOT(close()), actionCollection());

    back = new KToolBarPopupAction( i18n( "&Back" ), "back", ALT+Key_Left,
                                    this, SLOT( slotBack() ),
                                    actionCollection(), "back" );
    connect( back->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( slotBackActivated( int ) ) );
    connect( back->popupMenu(), SIGNAL( aboutToShow() ), this, SLOT( fillBackMenu() ) );
    back->setEnabled( false );

    forward = new KToolBarPopupAction( i18n( "&Forward" ), "forward",
                                       ALT+Key_Right, this,
                                       SLOT( slotForward() ),
                                       actionCollection(), "forward" );
    connect( forward->popupMenu(), SIGNAL( activated( int ) ), this, SLOT( slotForwardActivated( int ) ) );
    connect( forward->popupMenu(), SIGNAL( aboutToShow() ), this, SLOT( fillForwardMenu() ) );
    forward->setEnabled( false );

    createGUI( "khelpcenterui.rc" );

    QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>( guiFactory()->container( "go", this ) );
    if ( goMenu )
    {
        connect( goMenu, SIGNAL( aboutToShow() ), this, SLOT( fillGoMenu() ) );
        connect( goMenu, SIGNAL( activated( int ) ), this, SLOT( goMenuActivated( int ) ) );
        m_goMenuIndex = goMenu->count();
    }
    

    m_lstHistory.setAutoDelete( true );

    if (url.isEmpty())
        openURL( QString::fromLatin1( "about:khelpcenter" ) );
    else
        openURL( url );

    statusBar()->message(i18n("Ready"));
}

void KHMainWindow::slotStarted(KIO::Job *job)
{
    kdDebug() << "slotStarted\n";
    if (job)
       connect(job, SIGNAL(infoMessage( KIO::Job *, const QString &)),
            this, SLOT(slotInfoMessage(KIO::Job *, const QString &)));

    updateHistoryActions();
}

void KHMainWindow::createHistoryEntry()
{
    // First, remove any forward history
    HistoryEntry * current = m_lstHistory.current();
    if (current)
    {
        //kdDebug(1202) << "Truncating history" << endl;
        m_lstHistory.at( m_lstHistory.count() - 1 ); // go to last one
        for ( ; m_lstHistory.current() != current ; )
        {
            if ( !m_lstHistory.removeLast() ) // and remove from the end (faster and easier)
                assert(0);
            else
                m_lstHistory.at( m_lstHistory.count() - 1 );
        }
        // Now current is the current again.
    }
    // Append a new entry
    //kdDebug(1202) << "Append a new entry" << endl;
    m_lstHistory.append( new HistoryEntry ); // made current
    //kdDebug(1202) << "at=" << m_lstHistory.at() << " count=" << m_lstHistory.count() << endl;
    assert( m_lstHistory.at() == (int) m_lstHistory.count() - 1 );
}

void KHMainWindow::updateHistoryEntry()
{
    HistoryEntry *current = m_lstHistory.current();

    QDataStream stream( current->buffer, IO_WriteOnly );

    doc->browserExtension()->saveState( stream );

    current->url = doc->url();
    current->title = doc->title();
}

void KHMainWindow::slotOpenURLRequest( const KURL &url,
                                       const KParts::URLArgs &args)
{
    QString proto = url.protocol().lower();
    if ( proto != "help" && proto != "glossentry" && proto != "about" &&
         proto != "man" && proto != "info" && proto != "file")
    {
        kapp->invokeBrowser( url.url() );
        return;
    }

    stop();

    doc->browserExtension()->setURLArgs( args );

    if (proto == QString::fromLatin1("glossentry"))
        slotGlossSelected(static_cast<khcNavigatorWidget *>(nav->widget())->glossEntry(KURL::decode_string(url.encodedPathAndQuery())));
    else
    {
        createHistoryEntry();
        doc->openURL(url);
    }  
}

void KHMainWindow::slotBack()
{
    slotGoHistoryActivated( -1 );
}

void KHMainWindow::slotBackActivated( int id )
{
    slotGoHistoryActivated( -(back->popupMenu()->indexOf( id ) + 1) );
}

void KHMainWindow::slotForward()
{
    slotGoHistoryActivated( 1 );
}

void KHMainWindow::slotForwardActivated( int id )
{
    slotGoHistoryActivated( forward->popupMenu()->indexOf( id ) + 1 );
}

void KHMainWindow::slotGoHistoryActivated( int steps )
{
    if (!m_goBuffer)
    {
        // Only start 1 timer.
        m_goBuffer = steps;
        QTimer::singleShot( 0, this, SLOT(slotGoHistoryDelayed()));
    }
}

void KHMainWindow::slotGoHistoryDelayed()
{
  if (!m_goBuffer) return;
  int steps = m_goBuffer;
  m_goBuffer = 0;
  goHistory( steps );
}

void KHMainWindow::goHistory( int steps )
{
    stop();
    int newPos = m_lstHistory.at() + steps;
 
    HistoryEntry *current = m_lstHistory.at( newPos );
 
    assert( current );
 
    HistoryEntry h( *current );
    h.buffer.detach();
 
    QDataStream stream( h.buffer, IO_ReadOnly );
 
    doc->browserExtension()->restoreState( stream );
 
    updateHistoryActions();
}

void KHMainWindow::documentCompleted()
{
    updateHistoryEntry();

    updateHistoryActions();
}

void KHMainWindow::fillBackMenu()
{
    QPopupMenu *menu = back->popupMenu();
    menu->clear();
    fillHistoryPopup( menu, true, false, false );
}

void KHMainWindow::fillForwardMenu()
{
    QPopupMenu *menu = forward->popupMenu();
    menu->clear();
    fillHistoryPopup( menu, false, true, false );
}

void KHMainWindow::fillGoMenu()
{
    QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>( guiFactory()->container( "go", this ) );
    if ( !goMenu || m_goMenuIndex == -1 )
        return;

    for ( int i = goMenu->count() - 1 ; i >= m_goMenuIndex; i-- )
        goMenu->removeItemAt( i );

    // TODO perhaps smarter algorithm (rename existing items, create new ones only if not enough) ?
 
    // Ok, we want to show 10 items in all, among which the current url...
 
    if ( m_lstHistory.count() <= 9 )
    {
        // First case: limited history in both directions -> show it all
        m_goMenuHistoryStartPos = m_lstHistory.count() - 1; // Start right from the end
    } else
    // Second case: big history, in one or both directions
    {
        // Assume both directions first (in this case we place the current URL in the middle)
        m_goMenuHistoryStartPos = m_lstHistory.at() + 4;
 
        // Forward not big enough ?
        if ( m_lstHistory.at() > (int)m_lstHistory.count() - 4 )
          m_goMenuHistoryStartPos = m_lstHistory.count() - 1;
    }
    assert( m_goMenuHistoryStartPos >= 0 && (uint)m_goMenuHistoryStartPos < m_lstHistory.count() );
    m_goMenuHistoryCurrentPos = m_lstHistory.at(); // for slotActivated
    fillHistoryPopup( goMenu, false, false, true, m_goMenuHistoryStartPos );
}

void KHMainWindow::goMenuActivated( int id )
{
  QPopupMenu *goMenu = dynamic_cast<QPopupMenu *>( guiFactory()->container( "go", this ) );
  if ( !goMenu )
    return;

  // 1 for first item in the list, etc.
  int index = goMenu->indexOf(id) - m_goMenuIndex + 1;
  if ( index > 0 )
  {
      kdDebug(1202) << "Item clicked has index " << index << endl;
      // -1 for one step back, 0 for don't move, +1 for one step forward, etc.
      int steps = ( m_goMenuHistoryStartPos+1 ) - index - m_goMenuHistoryCurrentPos; // make a drawing to understand this :-)
      kdDebug(1202) << "Emit activated with steps = " << steps << endl;
      goHistory( steps );
  }
}

void KHMainWindow::slotInfoMessage(KIO::Job *, const QString &m)
{
    statusBar()->message(m);
}

void KHMainWindow::openURL(const QString &url)
{
    openURL( KURL( url ) );
}

void KHMainWindow::openURL(const KURL &url)
{
    stop();
    createHistoryEntry();
    doc->openURL(url);
}

void KHMainWindow::slotGlossSelected(const khcNavigatorWidget::GlossaryEntry &entry)
{
    stop();
    createHistoryEntry();
    doc->showGlossaryEntry( entry );
}

void KHMainWindow::updateHistoryActions()
{
    back->setEnabled( canGoBack() );
    forward->setEnabled( canGoForward() );
}

void KHMainWindow::stop()
{
    doc->closeURL();
    if ( m_lstHistory.count() > 0 )
        updateHistoryEntry();
}

// ripped from konq_actions :) TODO after 2.2: centralize ;-)
void KHMainWindow::fillHistoryPopup( QPopupMenu *popup, bool onlyBack, bool onlyForward,
                                     bool checkCurrentItem, uint startPos )
{
  assert ( popup ); // kill me if this 0... :/
 
  //kdDebug(1202) << "fillHistoryPopup position: " << history.at() << endl;
  HistoryEntry * current = m_lstHistory.current();
  QListIterator<HistoryEntry> it( m_lstHistory );
  if (onlyBack || onlyForward)
  {
      it += m_lstHistory.at(); // Jump to current item
      if ( !onlyForward ) --it; else ++it; // And move off it
  } else if ( startPos )
      it += startPos; // Jump to specified start pos
 
  uint i = 0;
  while ( it.current() )
  {
      QString text = it.current()->title;
      text = KStringHandler::csqueeze(text, 50); //CT: squeeze
      text.replace( QRegExp( "&" ), "&&" );
      if ( checkCurrentItem && it.current() == current )
      {
          int id = popup->insertItem( text ); // no pixmap if checked
          popup->setItemChecked( id, true );
      } else
          popup->insertItem( text );
      if ( ++i > 10 )
          break;
      if ( !onlyForward ) --it; else ++it;
  }
  //kdDebug(1202) << "After fillHistoryPopup position: " << history.at() << endl;
}

KHMainWindow::~KHMainWindow()
{
    delete doc;
}

static KCmdLineOptions options[] =
{
   { "+[url]", I18N_NOOP("A URL to display"), "" },
   { 0,0,0 }
};


extern "C" int kdemain(int argc, char *argv[])
{
    KAboutData aboutData( "khelpcenter", I18N_NOOP("KDE HelpCenter"),
                          HELPCENTER_VERSION,
                          I18N_NOOP("The KDE Help Center"),
                          KAboutData::License_GPL,
                          "(c) 1999-2000, Matthias Elter");
    aboutData.addAuthor("Matthias Elter",0, "me@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication::addCmdLineOptions();

    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KURL url;

    if (args->count())
        url = args->url(0);
    KHMainWindow *mw = new KHMainWindow(url);
    mw->show();

    return app.exec();
}


#include "khc_main.moc"
