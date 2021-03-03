/*****************************************************************

Copyright (c) 2000-2001 Matthias Ettrich <ettrich@kde.org>
              2000-2001 Matthias Elter   <elter@kde.org>
       	      2001      Carsten Pfeiffer <pfeiffer@kde.org>
              2001      Martijn Klingens <mklingens@yahoo.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qtooltip.h>

#include <qpopupmenu.h>
#include <krun.h>

#include <dcopclient.h>

#include <kglobal.h>
#include <kapp.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kwinmodule.h>

#include "systemtrayapplet.h"
#include "systemtrayapplet.moc"

#include <X11/Xlib.h>

template class QList<KXEmbed>;

extern "C"
{
    KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
	KGlobal::locale()->insertCatalogue("ksystemtrayapplet");
        return new SystemTrayApplet(configFile, KPanelApplet::Normal,
                                    0, parent, "ksystemtrayapplet");
    }
}

SystemTrayApplet::SystemTrayApplet(const QString& configFile, Type type, int actions,
				   QWidget *parent, const char *name)
  : KPanelApplet(configFile, type, actions, parent, name)
{
    m_Wins.setAutoDelete(true);
    setFrameStyle(Panel | Sunken);
    setBackgroundMode(X11ParentRelative);

    lockButton = new TrayButton( this, "lock");
    logoutButton = new TrayButton( this, "logout");

    lockButton->setBackgroundMode(X11ParentRelative);
    logoutButton->setBackgroundMode(X11ParentRelative);

    QToolTip::add( lockButton, i18n("Lock the desktop") );
    QToolTip::add( logoutButton, i18n("Logout") );

    lockButton->setPixmap( SmallIcon( "lock" ));
    logoutButton->setPixmap( SmallIcon( "exit" ));
    lockButton->adjustSize();
    logoutButton->adjustSize();

    lockButton->move(0, 0);
    lockButton->installEventFilter( this );
    logoutButton->installEventFilter( this );
    connect( lockButton, SIGNAL( clicked() ), SLOT( lock() ));
    connect( logoutButton, SIGNAL( clicked() ), SLOT( logout() ));

    if ( !kapp->dcopClient()->isAttached() )
        kapp->dcopClient()->attach();

    kwin_module = new KWinModule(this);

    // register existing tray windows
    const QValueList<WId> systemTrayWindows = kwin_module->systemTrayWindows();
    bool existing = false;
    for (QValueList<WId>::ConstIterator it = systemTrayWindows.begin(); it!=systemTrayWindows.end(); ++it ) {
        KXEmbed *emb = new KXEmbed(this);
        emb->setBackgroundMode(X11ParentRelative);
        connect(emb, SIGNAL(embeddedWindowDestroyed()), SLOT(updateTrayWindows()));
        m_Wins.append(emb);

        emb->embed(*it);
        emb->resize(24, 24);
        emb->show();
        existing = true;
    }
    if (existing)
        layoutTray();

    // the KWinModule notifies us when tray windows are added or removed
    connect( kwin_module, SIGNAL( systemTrayWindowAdded(WId) ),
             this, SLOT( systemTrayWindowAdded(WId) ) );
    connect( kwin_module, SIGNAL( systemTrayWindowRemoved(WId) ),
             this, SLOT( updateTrayWindows() ) );
}

SystemTrayApplet::~SystemTrayApplet()
{
    kdDebug() << "SystemTrayApplet::~SystemTrayApplet" << endl;
    m_Wins.clear();
}

bool SystemTrayApplet::eventFilter( QObject *o, QEvent *e )
{
	if( e->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent *me = dynamic_cast<QMouseEvent *>( e );
		if( me->button() == QMouseEvent::RightButton )
		{
			if( o == lockButton )
			{
				QPopupMenu *_popup = new QPopupMenu();

				/*
					TODO - Martijn Klingens:
					_popup->insertItem(SmallIcon("panel"), i18n("Panel Menu"), new PanelOpMenu(false, this));
					_popup->insertSeparator();
				*/
				_popup->insertItem( SmallIconSet( "lock" ), i18n("Lock Screen"), this, SLOT( lock() ) );
				_popup->insertSeparator();
				_popup->insertItem( SmallIconSet( "configure" ), i18n( "&Preferences..." ), this, SLOT( slotLockPrefs() ) );
				_popup->exec( me->globalPos() );
				delete _popup;

				return true;
			}
			else if ( o == logoutButton )
			{
				QPopupMenu *_popup = new QPopupMenu();

				/*
					TODO - Martijn Klingens:
					_popup->insertItem(SmallIcon("panel"), i18n("Panel Menu"), new PanelOpMenu(false, this));
					_popup->insertSeparator();
				*/
				_popup->insertItem( SmallIconSet( "exit" ), i18n("&Logout..."), this, SLOT( logout() ) );
				_popup->insertSeparator();
				_popup->insertItem( SmallIconSet( "configure" ), i18n( "&Preferences..." ), this, SLOT( slotLogoutPrefs() ) );
				_popup->exec( me->globalPos() );
				delete _popup;

				return true;
			}	// if o
		}	// if me->button
	}	// if e->type

	// Process event normally:
	return false;
}

void SystemTrayApplet::slotLockPrefs()
{
    // Run the screensaver settings
    KRun::run( "kcmshell screensaver", KURL::List() );
}

void SystemTrayApplet::slotLogoutPrefs()
{
	// Run the logout settings.
    KRun::run( "kcmshell kcmsmserver", KURL::List() );
}

void SystemTrayApplet::mousePressEvent(QMouseEvent* e)
{
    propagateMouseEvent(e);
}

void SystemTrayApplet::mouseReleaseEvent(QMouseEvent* e)
{
    propagateMouseEvent(e);
}

void SystemTrayApplet::mouseDoubleClickEvent(QMouseEvent* e)
{
    propagateMouseEvent(e);
}

void SystemTrayApplet::mouseMoveEvent(QMouseEvent* e)
{
    propagateMouseEvent(e);
}

void SystemTrayApplet::propagateMouseEvent(QMouseEvent* e)
{
    if ( !isTopLevel()  ) {
	QMouseEvent me(e->type(), mapTo( topLevelWidget(), e->pos() ),
		       e->globalPos(), e->button(), e->state() );
	QApplication::sendEvent( topLevelWidget(), &me );
    }
}

void SystemTrayApplet::systemTrayWindowAdded( WId w )
{
    KXEmbed *emb = new KXEmbed(this);
    emb->setBackgroundMode(X11ParentRelative);
    connect(emb, SIGNAL(embeddedWindowDestroyed()), SLOT(updateTrayWindows()));
    m_Wins.append(emb);

    emb->embed(w);
    emb->resize(24, 24);
    emb->show();

    layoutTray();
    emit updateLayout();
}

void SystemTrayApplet::updateTrayWindows()
{
    KXEmbed *emb = m_Wins.first();
    while ((emb = m_Wins.current()) != 0L) {
        WId wid = emb->embeddedWinId();
        if ((wid == 0) || !kwin_module->systemTrayWindows().contains(wid))
            m_Wins.remove(emb);
        else
            m_Wins.next();
    }
    layoutTray();
    emit updateLayout();
}

int SystemTrayApplet::widthForHeight(int h) const
{
    int ret;

    if (h < 48)
        ret = m_Wins.count() * 24 + 4;
    else
        ret = ( (m_Wins.count()+1) / 2 ) * 24 + 4;

    ret = (ret >= 28) ? ret : 0;
    return ret + logoutButton->x() + logoutButton->width();
}

int SystemTrayApplet::heightForWidth(int w) const
{
    int ret;

    if (w < 48)
        ret =  m_Wins.count() * 24 + 4;
    else
        ret = ( (m_Wins.count()+1) / 2 ) * 24 + 4;

    ret = (ret >= 28) ? ret : 0;
    return ret + logoutButton->y() + logoutButton->height();
}

void SystemTrayApplet::resizeEvent( QResizeEvent* )
{
    // crappy layout management for two single buttons. Arranges them perfectly
    // in all sizes and both orientations. The -2 is due to kicker giving us a
    // width/height of 42, while our two buttons would be 44 large. So we
    // cheat a bit to make it not suck.
    if ( orientation() == Vertical ) {
        if ( width() < lockButton->sizeHint().width() +
             logoutButton->sizeHint().width() -2 ) {
            lockButton->resize( width(), lockButton->sizeHint().height() );
            logoutButton->resize( width(), logoutButton->sizeHint().height() );
            logoutButton->move(0, lockButton->height());
        }
        else {
            int w = width()/2;
            lockButton->resize( w, lockButton->sizeHint().height() );
            logoutButton->resize( w, logoutButton->sizeHint().height() );
            logoutButton->move(lockButton->width(), 0);
        }
    }
    else {
        if ( height() < lockButton->sizeHint().height() +
             logoutButton->sizeHint().height() -2 ) {
            lockButton->resize( lockButton->sizeHint().width(), height() );
            logoutButton->resize( logoutButton->sizeHint().width(), height() );
            logoutButton->move(lockButton->width(), 0);
        }
        else {
            int h = height()/2;
            lockButton->resize( lockButton->sizeHint().width(), h );
            logoutButton->resize( logoutButton->sizeHint().width(), h );
            logoutButton->move(0, lockButton->height());
        }
    }

    layoutTray();
}

void SystemTrayApplet::layoutTray()
{
    if (m_Wins.count() == 0)
        return;

    int i;

    KXEmbed *emb;
    int col = 0;

    if (orientation() == Vertical) {
        int yoffset = logoutButton->y() + logoutButton->height();

        i = 0;
        for (emb = m_Wins.first(); emb != 0L; emb = m_Wins.next()) {
            if ( (m_Wins.count() == 1) || width() < 48 )
                emb->move(width() / 2 - 12, yoffset + 2 + i*24);
            else {
                emb->move(((i%2) == 0) ? 2 : width() - 26, yoffset + 2 + col*24);
                if ( (i%2) != 0 )
                    ++col;
            }
            i++;
        }
    }
    else {
        int xoffset = logoutButton->x() + logoutButton->width();

        i = 0;
        for (emb = m_Wins.first(); emb != 0L; emb = m_Wins.next()) {
            if ( (m_Wins.count() == 1) || height() < 48 )
                emb->move(xoffset + 2 + i*24, height() / 2 - 12);
            else {
                emb->move(xoffset + 2 + col*24, ((i%2) == 0) ? 2 : height() - 26);
                if ( (i%2) != 0 )
                    ++col;
            }
            i++;
        }
    }
    updateGeometry();
}

void SystemTrayApplet::lock()
{
    kapp->dcopClient()->send( "kdesktop", "KScreensaverIface", "lock()", "" );
}

void SystemTrayApplet::logout()
{
    kapp->dcopClient()->send( "kdesktop", "", "logout()", "" );
}

void TrayButton::drawButton(QPainter *p)
{
    bool sunken = isDown();

    style().drawPanel (p, 0, 0, width(), height(), colorGroup(), sunken, 1);

    if (sunken)
        p->translate(1,1);

    QRect br(1, 1, width()-2, height()-2);

    // draw icon
    if (!pixmap()->isNull()) {

        int dx = (br.width()   - pixmap()->width())   / 2;
        int dy = (br.height()  - pixmap()->height())  / 2;

        p->drawPixmap(br.x() + dx, br.y() + dy, *pixmap());
    }
}
