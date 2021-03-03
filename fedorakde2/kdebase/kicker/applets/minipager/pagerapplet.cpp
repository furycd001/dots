/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <stdio.h>

#include <qpainter.h>
#include <qdrawutil.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>

#include <kconfig.h>
#include <kwin.h>
#include <kwinmodule.h>
#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kwin.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <dcopclient.h>
#include <netwm.h>

#include <karrowbutton.h>

#include "pagerapplet.h"
#include "pagerapplet.moc"

#ifdef FocusOut
#undef FocusOut
#endif

const int knDesktopPreviewSize = 12;
const int knBtnSpacing = 1;

extern "C"
{
    KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
      KGlobal::locale()->insertCatalogue("kminipagerapplet");
      return new KMiniPager(configFile, KPanelApplet::Normal,
			    KPanelApplet::Preferences, parent, "kminipagerapplet");
    }
}

KMiniPagerButton::KMiniPagerButton(int desk, KMiniPager *parent, const char *name)
    : QButton(parent, name, WRepaintNoErase )
{
    setToggleButton( TRUE );
    setAcceptDrops( TRUE );
    deskNum = desk;
    lineedit = 0;
    connect(this, SIGNAL(clicked()), SLOT(slotClicked()) );
    connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
    connect(&dragSwitchTimer, SIGNAL( timeout() ), this, SLOT( slotDragSwitch() ) );
}

KMiniPager* KMiniPagerButton::pager()
{
    return (KMiniPager*) parentWidget();
}

void KMiniPagerButton::resizeEvent(QResizeEvent *ev)
{
    if ( lineedit )
	lineedit->setGeometry( rect() );
    QButton::resizeEvent( ev );
}

void KMiniPagerButton::paintEvent(QPaintEvent *)
{
    QPainter p;

    QBrush bg = colorGroup().brush( (!isOn() && !isDown()) ? QColorGroup::Dark : QColorGroup::Base );
    QColor fg = (!isOn() && !isDown()) ? colorGroup().base() : colorGroup().text();

    if ( pager()->mode() == KMiniPager::Preview  ) {
	QPixmap pm( width() - 2, height() - 2 );
	if (pm.isNull()) return;
	QPainter pp( &pm, this  );

	pp.fillRect( pm.rect(), bg );
	pp.setPen(fg);
	pp.drawText(0, 0, width(), height(), AlignCenter, QString::number( deskNum ) );
	int dw = QApplication::desktop()->width();
	int dh = QApplication::desktop()->height();
	QValueList<WId>::ConstIterator it;
	for ( it = pager()->kwin()->stackingOrder().begin();
	      it != pager()->kwin()->stackingOrder().end(); ++it ) {
	    KWin::Info* info = pager()->info( *it );
	    if ( info && info->mappingState == NET::Visible && (info->onAllDesktops || info->desktop == deskNum )
                && !(info->state & NET::SkipPager || info->state & NET::Shaded ) ) {
		QRect r =  info->frameGeometry;
		r = QRect( r.x() * pm.width() / dw, 2 + r.y() * pm.height() / dh,
			   r.width() * pm.width() / dw, r.height() * pm.height() / dh );
		if ( pager()->kwin()->activeWindow() == (*it) ) {
		    qDrawShadeRect( &pp, r, colorGroup(), false, 1, 0, &colorGroup().brush( QColorGroup::Highlight ) );
		} else {
		    pp.fillRect( r, colorGroup().brush(  QColorGroup::Button ) );
		    qDrawShadeRect( &pp, r, colorGroup(), true, 1, 0 );
		}
	    }
	}
	pp.end();
	p.begin( this );
	p.drawPixmap( 1, 1, pm );
    } else {
	p.begin( this );
	p.fillRect( rect(), bg );
    }

    p.setPen( fg );

    if ( pager()->mode() == KMiniPager::Number ) {
	p.drawText(0, 0, width(), height(), AlignCenter, QString::number( deskNum ) );
    } else if ( pager()->mode() == KMiniPager::Name ) {
	p.drawText( 0, 0, width(), height(), AlignVCenter | AlignCenter, pager()->kwin()->desktopName( deskNum ) );
    }

    if(!isOn() && !isDown()){
	p.setPen(colorGroup().light());
	p.drawLine(0, 0, width()-1, 0);
	p.drawLine(0, 0, 0, height()-1);
	p.setPen(Qt::black);
	p.drawLine(0, height()-1, width()-1, height()-1);
	p.drawLine(width()-1, 0, width()-1, height()-1);
    } else {
	p.drawRect(rect());
    }
}

void KMiniPagerButton::mousePressEvent( QMouseEvent * e)
{
    if ( e->button() != RightButton )
	QButton::mousePressEvent( e );
    else
	emit showMenu( e->globalPos(), deskNum );
}

void KMiniPagerButton::dragEnterEvent( QDragEnterEvent* e )
{
    // if a dragitem is held for over a pager button for two seconds,
    // activate corresponding desktop
    dragSwitchTimer.start( 1000, TRUE );

    QButton::dragEnterEvent( e );
}

void KMiniPagerButton::dragLeaveEvent( QDragLeaveEvent* e )
{
    dragSwitchTimer.stop();

    QButton::dragLeaveEvent( e );
}

void KMiniPagerButton::slotDragSwitch()
{
    emit buttonSelected( deskNum );
}

void KMiniPagerButton::slotClicked()
{
    if ( isOn() )
	emit buttonSelected( deskNum );
    else {
	setOn(true );
	if ( pager()->mode() == KMiniPager::Name ) {
	    if ( !lineedit ) {
		lineedit = new QLineEdit( this );
		connect( lineedit, SIGNAL( returnPressed() ), lineedit, SLOT( hide() ) );
		lineedit->installEventFilter( this );
	    }
	    lineedit->setGeometry( rect() );
	    lineedit->setText( pager()->kwin()->desktopName( deskNum ) );
	    lineedit->show();
	    lineedit->setFocus();
	    lineedit->selectAll();
	    pager()->emitRequestFocus();
	}
    }
}

void KMiniPagerButton::slotToggled( bool b )
{
    if ( !b && lineedit )
	lineedit->hide();
}


bool KMiniPagerButton::eventFilter( QObject *o, QEvent * e)
{
    if ( o && o == lineedit && ( e->type() == QEvent::FocusOut || e->type() == QEvent::Hide ) ) {
 	pager()->kwin()->setDesktopName( deskNum, lineedit->text() );
	delete lineedit;
	lineedit = 0;
	return TRUE;
    }
    return QButton::eventFilter( o, e );
}

KMiniPager::KMiniPager(const QString& configFile, Type type, int actions,
		       QWidget *parent, const char *name)
    : KPanelApplet(configFile, type, actions, parent, name),
      layout(0), m(Preview),
      m_pBtnDesktopPreview(0), m_pDesktopPreviewFrame(0),
      m_pBoxLayout(0), m_pPagerProcess(0)
{
    windows.setAutoDelete( TRUE );

    KConfig *conf = config();
    conf->setGroup("minipager");
    QFont defFont("Helvetica", 10, QFont::Bold);
    defFont = conf->readFontEntry("Font", &defFont);
    setFontPropagation(AllChildren);
    setFont(defFont);

    QString ms = conf->readEntry("Mode", "Preview" );
    if ( ms == "Number" )
	m = Number;
    else if ( ms == "Name" )
	m = Name;
    else
	m = Preview;

    bShowDesktopPreviewButton = conf->readBoolEntry("ShowPreviewBtn", false);

    kwin_module = new KWinModule(this);
    active = kwin_module->activeWindow();
    curDesk = kwin_module->currentDesktop();
    if (curDesk == 0) // kwin not yet launched
        curDesk = 1;

    allocateButtons();
    m_pBtnDesktopPreview = new KArrowButton(this);
    QToolTip::add(m_pBtnDesktopPreview, i18n("Desktop Preview"));

    connect(m_pBtnDesktopPreview, SIGNAL(clicked()), SLOT(desktopPreview()) );
//    connect(m_pBtnDesktopPreview, SIGNAL(showMenu(const QPoint&, int )), SLOT(slotShowMenu(const QPoint&, int )));
    if (!bShowDesktopPreviewButton)
        m_pBtnDesktopPreview->hide();

    connect(kwin_module, SIGNAL(currentDesktopChanged(int)), SLOT(slotSetDesktop(int)));
    connect(kwin_module, SIGNAL(numberOfDesktopsChanged(int)),
            SLOT(slotSetDesktopCount(int)));
    connect(kwin_module, SIGNAL(activeWindowChanged(WId)), SLOT(slotActiveWindowChanged(WId)));
    connect( kwin_module, SIGNAL( windowAdded(WId) ), this, SLOT( slotWindowAdded(WId) ) );
    connect( kwin_module, SIGNAL( windowRemoved(WId) ), this, SLOT( slotWindowRemoved(WId) ) );
    connect( kwin_module, SIGNAL( windowChanged(WId,unsigned int) ), this, SLOT( slotWindowChanged(WId,unsigned int) ) );
    connect( kwin_module, SIGNAL( stackingOrderChanged() ), this, SLOT( slotStackingOrderChanged() ) );
    connect( kwin_module, SIGNAL( desktopNamesChanged() ), this, SLOT( slotDesktopNamesChanged() ) );
}

KMiniPager::~KMiniPager()
{
    destroyDesktopPreview();
}

void KMiniPager::slotSetDesktop(int desktop)
{
    hideDesktopPreview();

    if ( curDesk == desktop )
	return;

    if ( kwin_module->numberOfDesktops() > static_cast<int>(btnList.count()))
	slotSetDesktopCount( kwin_module->numberOfDesktops() );
    btnList[curDesk-1]->setOn(false);
    curDesk = desktop;
    btnList[curDesk-1]->setOn(true);
}

void KMiniPager::slotButtonSelected(int desk )
{
    KWin::setCurrentDesktop( desk );
    slotSetDesktop( desk );
}

int KMiniPager::widthForHeight(int h) const
{
    int deskNum = kwin_module->numberOfDesktops();
    int deskHalf = (deskNum+1)/2;

    int bw = h < 32 ? h : (h/2);
    if ( mode() == Preview )
	bw = (int) ( bw * (double) QApplication::desktop()->width() / QApplication::desktop()->height() );
    else if ( mode() == Name ) {
	for (int i = 1; i <= deskNum; i++ ) {
	    int sw = fontMetrics().width( kwin_module->desktopName( i ) ) + 6;
	    if ( sw > bw )
		bw = sw;
	}
    }

    int nWd = ( h <= 32 ? deskNum * bw : deskHalf * bw);
    if (bShowDesktopPreviewButton && orientation() == Horizontal)
        nWd += knDesktopPreviewSize + knBtnSpacing;

    return nWd;
}

int KMiniPager::heightForWidth(int w) const
{
    int deskNum = kwin_module->numberOfDesktops();
    int deskHalf = (deskNum+1)/2;

    bool small = w <= 32;

    int bh = small ? w : (w/2);
    if ( mode() == Preview )
	bh = (int) ( bh *  (double) QApplication::desktop()->height() / QApplication::desktop()->width() );
    else if ( mode() == Name ) {
	bh = fontMetrics().lineSpacing() + 8;
	small = true;
    }

    int nHg = ( small ? deskNum * bh : deskHalf * bh);
    if (bShowDesktopPreviewButton && orientation() != Horizontal)
        nHg += knDesktopPreviewSize + knBtnSpacing;

    return nHg;
}

void KMiniPager::popupDirectionChange( Direction d )
{
    ArrowType at = UpArrow;

    switch(d) {
        case Up:
            at = UpArrow;
            break;
        case Down:
            at = DownArrow;
            break;
        case Left:
            at = LeftArrow;
            break;
        case Right:
            at = RightArrow;
            break;
    }
    m_pBtnDesktopPreview->setArrowType(at);
}

void KMiniPager::resizeEvent(QResizeEvent*)
{
    int deskNum = btnList.count();
    int deskHalf = (deskNum+1)/2;
    bool horiz = orientation() == Horizontal;
    bool small = (horiz && height() <=32) || (!horiz && width() <=32);

    if ( !horiz && mode() == KMiniPager::Name )
	small = TRUE;

    if (m_pBoxLayout)
    {
        delete m_pBoxLayout;
        m_pBoxLayout = 0;
    }
    else if (layout)
    {
        delete layout;
        layout = 0;
    }

    int nDX, nDY;
    if(horiz)
    {
        if (small)
	    nDX = 1, nDY = deskNum;
        else
	    nDX = 2, nDY = deskHalf;
        if (bShowDesktopPreviewButton)
            m_pBoxLayout = new QHBoxLayout(this);
    }
    else
    {
        if (small)
	    nDX = deskNum, nDY = 1;
        else
	    nDX = deskHalf, nDY = 2;
        if (bShowDesktopPreviewButton)
            m_pBoxLayout = new QVBoxLayout(this);
    }

    if (m_pBoxLayout && m_pBtnDesktopPreview)
    {
        ArrowType at;

        if (horiz)
        {
            at = (popupDirection() == Down ? DownArrow : UpArrow);
            m_pBtnDesktopPreview->setMinimumSize(knDesktopPreviewSize, 1);
            m_pBtnDesktopPreview->setMaximumSize(knDesktopPreviewSize, 1024);
        }
        else // o == Vertical
        {
            at = (popupDirection() == Right ? RightArrow : LeftArrow);
            m_pBtnDesktopPreview->setMinimumSize(1, knDesktopPreviewSize);
            m_pBtnDesktopPreview->setMaximumSize(1024, knDesktopPreviewSize);
        }
        m_pBtnDesktopPreview->setArrowType(at);

        m_pBoxLayout->addWidget(m_pBtnDesktopPreview);
        m_pBoxLayout->addSpacing(knBtnSpacing);

        layout = new QGridLayout(m_pBoxLayout, nDX, nDY);
    }
    else
    {
        layout = new QGridLayout(this, nDX, nDY);
    }

    int c = 0;
    QValueList<KMiniPagerButton*>::Iterator it = btnList.begin();

    if (small) {
	while( it != btnList.end() ) {
	    if(horiz)
		layout->addWidget( *it, 0, c);
	    else
		layout->addWidget( *it, c, 0);
	    ++it;
	    ++c;
	}
    } else {
	while( it != btnList.end() ) {
	    if(horiz)
		layout->addWidget( *it, 0, c);
	    else
		layout->addWidget( *it, c, 0);
	    if ( ++it != btnList.end() ) {
		if(horiz)
		    layout->addWidget( *it, 1, c);
		else
		    layout->addWidget( *it, c, 1);
		++it;
	    }
	    ++c;
	}
    }

    layout->activate();
    if (m_pBoxLayout)
        m_pBoxLayout->activate();
    updateGeometry();
}

void KMiniPager::allocateButtons()
{
    int i;
    int deskNum = kwin_module->numberOfDesktops();
    int act = kwin_module->currentDesktop();
		KMiniPagerButton *btn;
    for (i=1; i <= deskNum; ++i) {
	btn = new KMiniPagerButton(i, this);
	btn->setOn(i == act);
	btn->show();
	QToolTip::add(btn, kwin()->desktopName(i));

	btnList.append(btn);
	connect(btn, SIGNAL(buttonSelected(int)), SLOT(slotButtonSelected(int)));
	connect(btn, SIGNAL(showMenu(const QPoint&, int )), SLOT(slotShowMenu(const QPoint&, int )));
    }
}

void KMiniPager::slotSetDesktopCount(int )
{
    QValueList<KMiniPagerButton*>::Iterator it;
    for(it=btnList.begin(); it != btnList.end(); ++it)
	delete (*it);
    btnList.clear();
    allocateButtons();
    curDesk = kwin_module->currentDesktop();
    if (curDesk == 0) // kwin not yet launched
	curDesk = 1;
    resizeEvent(0);
    updateLayout();

    // create desktop preview here
    static bool bFirst = true;
    if (bFirst && bShowDesktopPreviewButton)
    {
        bFirst = false;
        createDesktopPreview();
    }
}

void KMiniPager::slotActiveWindowChanged( WId win )
{
    hideDesktopPreview();

    if ( m != Preview )
	return;
    KWin::Info* inf1 = active ? info( active ) : NULL;
    KWin::Info* inf2 = info( win );
    active = win;
    for ( int i=1; i <= (int) btnList.count(); ++i) {
	if ( (inf1 && (inf1->onAllDesktops || inf1->desktop == i ) )
	     || (inf2 && (inf2->onAllDesktops || inf2->desktop == i ) ) )
	    btnList[i-1]->update();
    }
}

void KMiniPager::slotWindowAdded( WId win)
{
    hideDesktopPreview();

    if ( m != Preview )
	return;
    KWin::Info* inf = info( win );
    for ( int i=1; i <= (int) btnList.count(); ++i) {
	if ( inf->onAllDesktops || inf->desktop == i )
	    btnList[i-1]->update();
    }
}

void KMiniPager::slotWindowRemoved( WId win )
{
    if ( m != Preview ) {
	windows.remove( win );
	return;
    }
    KWin::Info* inf = info( win );
    bool onAllDesktops = inf->onAllDesktops;
    int desktop = inf->desktop;

    if (win == active)
        active = 0;

    windows.remove( (long) win );

    for ( int i=1; i <= (int) btnList.count(); ++i) {
	if ( onAllDesktops || desktop == i )
	    btnList[i-1]->update();
    }
}

void KMiniPager::slotWindowChanged( WId win , unsigned int )
{
    if ( m != Preview ) {
	windows.remove( win );
	return;
    }
    KWin::Info* inf = windows[win];
    bool onAllDesktops = inf ? inf->onAllDesktops : false;
    int desktop = inf ? inf->desktop : 0;
    windows.remove( (long) win );
    inf = info( win );
    for ( int i=1; i <= (int) btnList.count(); ++i) {
	if ( inf->onAllDesktops || inf->desktop == i  || onAllDesktops || desktop == i )
	    btnList[i-1]->update();
    }
}

KWin::Info* KMiniPager::info( WId win )
{
    KWin::Info* info = windows[win];
    if (!info ) {
	info = new KWin::Info( KWin::info( win ) );
	windows.insert( (long) win, info );
    }
    return info;
}

void KMiniPager::slotStackingOrderChanged()
{
	hideDesktopPreview();

    if ( m != Preview )
	return;
    slotRefresh();

}
void KMiniPager::slotRefresh()
{
    for ( int i=1; i <= (int) btnList.count(); ++i)
	btnList[i-1]->update();
}

void KMiniPager::slotShowMenu( const QPoint& pos, int desktop )
{
    QPopupMenu* p = new QPopupMenu;

		if( desktop >= 0)
		{
//			p->insertTitle( kwin()->desktopName( desktop ), 1 );
	    p->insertItem( i18n("Activate"), 97);
	    p->insertSeparator();
		}
//		else
//			p->insertTitle( i18n( "Virtual Desktops" ), 1 );

    p->setCheckable( TRUE );
    p->insertItem( i18n("Preview"), KMiniPager::Preview );
    p->insertItem( i18n("Number"), KMiniPager::Number );
    p->insertItem( i18n("Name"), KMiniPager::Name );
    p->insertSeparator();
    p->insertItem( i18n("Enable Desktop Preview"), 98);
    p->insertSeparator();
    p->insertItem( SmallIcon("configure"), i18n("&Preferences..."), 99);
    p->setItemChecked( m, TRUE  );
    p->setItemChecked(98, bShowDesktopPreviewButton);

    int result = p->exec( pos );
    delete p;
    if ( result <= 0 || result == m )
        return;

    if (result == 99)
    {
        preferences();
        return;
    }

    if (result == 97)
    {
	    KWin::setCurrentDesktop( desktop );
			slotSetDesktop( desktop );
      return;
    }

    KConfig* conf = config();
    conf->setGroup("minipager");

    if(result == 98)
    {
        bShowDesktopPreviewButton = !bShowDesktopPreviewButton;
        conf->writeEntry( "ShowPreviewBtn", bShowDesktopPreviewButton);
        resizeEvent(0);
        if (bShowDesktopPreviewButton)
        {
            m_pBtnDesktopPreview->show();
            createDesktopPreview();
        }
        else
        {
            destroyDesktopPreview();
            delete m_pDesktopPreviewFrame;
            m_pDesktopPreviewFrame = 0L;
            m_pBtnDesktopPreview->hide();
        }
    }
    else
    {
        m = (KMiniPager::Mode)result;
        if ( m == Number )
            conf->writeEntry( "Mode", "Number" );
        else if ( m == Name )
            conf->writeEntry( "Mode", "Name" );
        else
            conf->writeEntry( "Mode", "Preview" );
    }

    conf->sync();

    slotRefresh();
    emit updateLayout();
}

void KMiniPager::slotDesktopNamesChanged()
{
    for ( int i=1; i <= (int) btnList.count(); ++i)
      {
	QToolTip::remove(btnList[i-1]);
	QToolTip::add(btnList[i-1], kwin()->desktopName(i));
      }

    if ( m != Name )
      return;

    slotRefresh();
    emit updateLayout();
}

void KMiniPager::preferences()
{
  kapp->startServiceByDesktopName("virtualdesktops");
}

QDesktopPreviewFrame::QDesktopPreviewFrame(KMiniPager *pPager)
    : QFrame(0, 0, WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Dialog)
{
    m_pPager = pPager;

    // Set the wm flags to this window
    KWin::setState( winId(), NET::StaysOnTop | NET::SkipTaskbar | NET::SkipPager );
}

void QDesktopPreviewFrame::keyPressEvent(QKeyEvent *evt)
{
    if (isVisible())
    {
        m_pPager->hideDesktopPreview();
    }
    QFrame::keyPressEvent(evt);
}

void KMiniPager::hideDesktopPreview()
{
    if (m_pDesktopPreviewFrame && m_pDesktopPreviewFrame->isVisible())
    {
        m_pDesktopPreviewFrame->hide();
        m_pBtnDesktopPreview->setOn(false);
    }
}

void KMiniPager::desktopPreviewProcessExited(KProcess *p)
{
    kdDebug() << "KMiniPager::desktopPreviewProcessExited" << endl;
    if (m_pPagerProcess == p)
    {
        delete m_pPagerProcess;
        m_pPagerProcess = 0;

        hideDesktopPreview();
        delete m_pDesktopPreviewFrame;
        m_pDesktopPreviewFrame = 0;
    }
}

void KMiniPager::destroyDesktopPreview()
{
    if (m_pPagerProcess)
        m_pPagerProcess->kill();
}

QSize KMiniPager::calculateDesktopPreviewFrameSize() const
{
    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    int nHg = 120; // two rows always
    int nNumColumns = (kwin_module->numberOfDesktops() + 1) / 2;

    // take into account an internal borders
    int nWdCorr = 6 + (nNumColumns) * 2;
    if (nNumColumns <= 3)
        nWdCorr--;

    int nWd = (60 * nNumColumns * w) / h + nWdCorr;
    return QSize(nWd, nHg);
}

void KMiniPager::createDesktopPreview()
{
    // create desktop preview frame
    if (!m_pDesktopPreviewFrame)
    {
        m_pDesktopPreviewFrame = new QDesktopPreviewFrame(this);
        m_pDesktopPreviewFrame->setFrameStyle(QFrame::Panel| QFrame::Raised);
        m_pDesktopPreviewFrame->hide();

        m_pDesktopPreviewFrame->resize(calculateDesktopPreviewFrameSize());

        QString strAppPath(locate("exe", "kpager2"));
        if (!strAppPath.isEmpty())
        {
            KProcess * process = new KProcess();
            if (!process)
                return;

            // set executable
            *process << strAppPath;

            // set parameters
            *process << "-parent";
            *process << QString().setNum(m_pDesktopPreviewFrame->winId());

            QObject::connect(process, SIGNAL(processExited(KProcess *)),
                             this, SLOT(desktopPreviewProcessExited(KProcess *)));

            // start process with a link to StdIn for oneway comm.
            process->start(KProcess::NotifyOnExit);
            m_pPagerProcess = process;
        }
    }
}

void KMiniPager::desktopPreview()
{
    createDesktopPreview();

    if (m_pDesktopPreviewFrame)
    {
        if (!m_pDesktopPreviewFrame->isVisible())
        {
            // calculate correct frame position
            int w = QApplication::desktop()->width();
            int h = QApplication::desktop()->height();
            QPoint btnPos(m_pBtnDesktopPreview->mapToGlobal(m_pBtnDesktopPreview->pos()));

            QSize frmSz(calculateDesktopPreviewFrameSize());

            int nX, nY;
            if (orientation() == Horizontal)
            {
                nX = w - frmSz.width();
                if (nX > btnPos.x())
                    nX = btnPos.x();

                if (popupDirection() == Down)
                {
                    nY = height() + 1;
                }
                else
                {
                    nY = h - height() - frmSz.height() - 2;
                }
            }
            else // orientation == vertical
            {
                nY = h - frmSz.height();
                if (nY > btnPos.y())
                    nY = btnPos.y();

                if (popupDirection() == Right)
                {
                    nX = width() + 1;
                }
                else
                {
                    nX = w - width() - frmSz.width() -1;
                }
            }

            m_pDesktopPreviewFrame->setGeometry(nX, nY, frmSz.width(), frmSz.height());

            m_pDesktopPreviewFrame->show();
            m_pDesktopPreviewFrame->setFocus();
            KWin::setActiveWindow(m_pDesktopPreviewFrame->winId());
        }
        else
        {
            m_pDesktopPreviewFrame->hide();
        }
    }
}
