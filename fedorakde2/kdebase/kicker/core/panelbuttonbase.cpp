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

#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qapplication.h>
#include <qstyle.h>

#include <kstddirs.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kimageeffect.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <kglobalsettings.h>
#include <kcursor.h>
#include <kapp.h>
#include <kipc.h>
#include <kalphapainter.h>
#include <qwidgetlist.h>
#include "global.h"
#include "panel.h"
#include "panelbuttonbase.h"
#include "panelbuttonbase.moc"


ZoomButton::ZoomButton()
	: PanelButtonBase( 0, 0, WStyle_Customize | WX11BypassWM | WMouseNoMask)
{
    zooming = FALSE;
    locked = 0;
    hideTimer = 0;
    raiseTimer = 0;
    qApp->installEventFilter( this );
    setMouseTracking( TRUE );
    connect( PGlobal::panel, SIGNAL( configurationChanged() ), this, SLOT(reconfigure() ) );
    reconfigure();
}
ZoomButton::~ZoomButton()
{
}

void ZoomButton::reconfigure()
{
    KConfig* config = KGlobal::config();
    config->setGroup("buttons");
    zooming = config->readBoolEntry("EnableIconZoom", true);
}


bool ZoomButton::isWatching( PanelButtonBase* btn )
{
    return !isHidden() && btn == watch;
}

void ZoomButton::watchMe( PanelButtonBase* btn, const QPoint& center, const QPixmap& pm)
{
    hide();
    if ( watch )
	watch->update();
    watch = btn;
    _icon = pm;
    resize( pm.size() );
    QPoint p = center - rect().center();
    if ( p.x() < 0 )
	p.rx() = 0;
    if ( p.y() < 0 )
	p.ry() = 0;
    if ( p.x() + width() > qApp->desktop()->width() )
	p.rx() = qApp->desktop()->width() - width();
    if ( p.y() + height() > qApp->desktop()->height() )
	p.ry() = qApp->desktop()->height() - height();
    move( p );
    mypos = p;
    setDown( false );
    clearMask();
    if ( pm.mask() )
         setMask( *pm.mask() );
    raise();
    show();
    watch->update();
    if ( !raiseTimer )
	raiseTimer = startTimer( 100 );
}

void ZoomButton::drawButtonLabel(QPainter *p, const QPixmap &)
{
    if ( isDown() || isOn() )
	move ( mypos + QPoint(2,2) );
    else
	move ( mypos );
    if ( !_icon.isNull()) {
	int x = (width() - _icon.width())/2;
	int y = (height() - _icon.height())/2;
	p->drawPixmap(x, y, _icon);
    }
}

/* This event filter is very tricky and relies on Qt
   internals. It's written this way to make all panel buttons work
   without modification and to keep advanced functionality like tool
   tips for the buttons alive.

   Don't hack around in this filter unless you REALLY know what you
   are doing. In case of doubt, ask ettrich@kde.org.
*/
bool ZoomButton::eventFilter( QObject *o, QEvent * e)
{
    if ( !watch )
	return FALSE;
    if ( e == locked )
	return FALSE;
    if ( !isVisible() )
	return FALSE;
    if ( e->type() == QEvent::Enter ) {
	QPoint globalPos = QCursor::pos();
	if ( geometry().contains( globalPos, true ) ) {
	    if (_changeCursorOverItem)
		setCursor(KCursor().handCursor());
	}
    }
    if ( e->type() == QEvent::Leave ) {
	if ( !hideTimer )
	    hideTimer = startTimer( 200 );
    }


    if ( o == this ) {
	if ( e->type() == QEvent::Timer && ((QTimerEvent*)e)->timerId() == raiseTimer ) {
	    bool tooltip_visible = FALSE;
	    QWidgetList  *list = QApplication::topLevelWidgets();
	    for ( QWidget * w = list->first(); w; w = list->next() )
		if ( w->isVisible() && w->inherits( "QTipLabel" ) ) {
		    tooltip_visible = TRUE;
		    break;
		}
	    delete list;
	    if ( !tooltip_visible && !qApp->activePopupWidget() )
		raise();
	}

	if ( e->type() == QEvent::Timer && ((QTimerEvent*)e)->timerId() == hideTimer ) {
	    if ( watch && watch->isDown() )
		return TRUE;
	    QPoint globalPos = QCursor::pos();
	    if ( !geometry().contains( globalPos ) ) {
		killTimer( hideTimer );
		hideTimer = 0;
		killTimer( raiseTimer );
		raiseTimer = 0;
		hide();
		if ( watch )
		    watch->update();
		if(!_changeCursorOverItem)
		    setCursor(oldCursor);
	    }
	    return TRUE;
	}

	if ( e->type() == QEvent::MouseButtonPress )
	    raise();

	(void) event( e ); // send to us directly , below we'll  hide the event from the tooltip filters


	if ( e->type() == QEvent::MouseButtonPress ||
	     e->type() == QEvent::MouseButtonRelease ||
	     e->type() == QEvent::MouseMove ) {
	    QMouseEvent* ev = (QMouseEvent*) e;
	    if ( rect().contains( ev->pos() ) && watch && !watch->rect().contains( ev->pos() ) )
		ev = new QMouseEvent( ev->type(),
				      watch->rect().center(),
				      ev->globalPos(),
				      ev->button(),
				      ev->state() );
	    locked = e;
	    QApplication::sendEvent( watch, ev );
	    if ( ev != e )
		delete ev;
	    locked = 0;
	}

	if ( e->type() == QEvent::Enter || e->type() == QEvent::Leave ) {
	    locked = e;
	    QApplication::sendEvent( watch, e );
	    locked = 0;
	}



	return TRUE;
    }
    if ( watch == o && e != locked ) {
	if ( e->type() == QEvent::MouseButtonPress ||
	     e->type() == QEvent::MouseButtonRelease ||
	     e->type() == QEvent::MouseMove ||
	     e->type() == QEvent::Enter ||
	     e->type() == QEvent::Leave ) {
	    QApplication::sendEvent( this, e );
	    return TRUE;
	}
    }
    return FALSE;
}
static ZoomButton* zoomButton = 0;

PanelButtonBase::PanelButtonBase(QWidget *parent, const char *name, WFlags f)
  : QButton(parent, name, f)
  , _dir(Bottom)
  , _drawArrow(false)
  , _highlight(false)
  , _changeCursorOverItem(true)
  , _tile(QString::null)
{
    setBackgroundMode( PaletteBackground );
    oldCursor = cursor();

    slotSettingsChanged(KApplication::SETTINGS_MOUSE);
    connect(kapp, SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
    kapp->addKipcEventMask(KIPC::SettingsChanged);
}

void PanelButtonBase::slotSettingsChanged(int category)
{
    if (category != KApplication::SETTINGS_MOUSE) return;

    _changeCursorOverItem = KGlobalSettings::changeCursorOverIcon();

    if(!_changeCursorOverItem)
	setCursor(oldCursor);
}

void PanelButtonBase::setTile(const QString& tile)
{
    _tile = tile;
    loadTiles();
    update();
}

void PanelButtonBase::setIcon(const QString & nm, const QString & fallback)
{
    KIcon::StdSizes sz;

    if ( orientation() == Horizontal ) {
	if ( height() < 32 ) sz = KIcon::SizeSmall;
	else if ( height() < 48 ) sz = KIcon::SizeMedium;
	else sz = KIcon::SizeLarge;
    }
    else {
	if ( width() < 32 ) sz = KIcon::SizeSmall;
	else if ( width() < 48 ) sz = KIcon::SizeMedium;
	else sz = KIcon::SizeLarge;
    }

    KIconLoader * ldr = KGlobal::iconLoader();
    _iconName = nm;

    _icon = ldr->loadIcon(nm, KIcon::Panel, sz, KIcon::DefaultState, 0L, true);

    if (_icon.isNull())
	_icon = ldr->loadIcon(fallback, KIcon::Panel, sz, KIcon::DefaultState);

    _iconh = ldr->loadIcon(nm, KIcon::Panel, sz, KIcon::ActiveState, 0L, true);

    if (_iconh.isNull())
	_iconh = ldr->loadIcon(fallback, KIcon::Panel, sz, KIcon::ActiveState);

    QPixmap nullpm;
    _iconz = nullpm;
    _iconz = ldr->loadIcon( nm, KIcon::Panel, KIcon::SizeLarge, KIcon::ActiveState, 0L, true );
    if ( _iconz.isNull() )
	_iconz = ldr->loadIcon( fallback, KIcon::Panel, KIcon::SizeLarge, KIcon::ActiveState, 0L, true );

    update();
}

void PanelButtonBase::setIcon(const KURL & u)
{
    QString name = KMimeType::iconForURL(u, 0);
    setIcon( name , "unknown");
}

void PanelButtonBase::setTitle(const QString & t)
{
    _title = t;
}

void PanelButtonBase::setDrawArrow(bool v)
{
    if (_drawArrow == v) return;
    _drawArrow = v;
    update();
}

void PanelButtonBase::slotSetPopupDirection(Direction d)
{
    dir = d;

    if (dir == dUp)
        setArrowDirection(Top);
    else if (dir == dDown)
        setArrowDirection(Bottom);
    else if (dir == dLeft)
        setArrowDirection(Left);
    else
        setArrowDirection(Right);
}

void PanelButtonBase::setArrowDirection(Position dir)
{
    if (_dir == dir) return;
    _dir = dir;
    update();
}

void PanelButtonBase::setBackground()
{
    KConfig* config = KGlobal::config();
    config->setGroup("General");
    if (config->readBoolEntry("UseBackgroundTheme", false))
    {
	// Get the pixmap from the container area object
        QWidget* pContainer = (QWidget*) PGlobal::panel;
	QPalette pal = pContainer->palette();
	QBrush bgBrush = pal.brush( QPalette::Active, QColorGroup::Background );
	QPixmap* containerBG = bgBrush.pixmap();

	// Make sure the background pixmap exists
	if ( containerBG )
	{
	    // Create a pixmap the same size as the button to use as the bg
	    QPixmap bgPix( width(), height() );

	    // Calculate which part of the container area background should be copied
	    QWidget* p = (QWidget*) parent();

	    int srcx, srcy;
	    if ( _dir == Bottom || _dir == Top )
	    {
		srcx = p->x() % containerBG->width();
		srcy = 0;
	    }
	    else
	    {
		srcx = 0;
		srcy = p->y() % containerBG->height();
	    }

	    // Check if we need to take care of a wrap-around
	    if ( srcx + p->width() <= containerBG->width() &&
		    srcy + p->height() <= containerBG->height() )
	    {
		// Nothing funny going on with overlaps - just copy

		QPoint dp( 0, 0 ); // destination point
		QRect sr( srcx, srcy, width(), height() );
		bitBlt( &bgPix, dp, containerBG, sr, CopyROP );
	    }
	    else
	    {
		// Need to do 2 seperate bitBlts to take care of the overlap

		// Copy the left/top of the image first up to the wrap-
		// around point
		int x = 0;
		int y = 0;
		int w = containerBG->width() - srcx;
		int h = containerBG->height() - srcy;
		QPoint dp( x, y ); // destination point
		QRect sr( srcx, srcy, w, h );
		bitBlt( &bgPix, dp, containerBG, sr, CopyROP );

		// Now copy the wrap-around bit
		if ( _dir == Bottom || _dir == Top )
		{
		    x = containerBG->width() - srcx;
		    y = 0;
		    w = srcx + p->width() - containerBG->width();
		    h = p->height();
		}
		else
		{
		    x = 0;
		    y = containerBG->height() - srcy;
		    w = p->width();
		    h = srcy + p->height() - containerBG->height();
		}
		dp = QPoint( x, y );
		sr = QRect( 0, 0, w, h );
		bitBlt( &bgPix, dp, containerBG, sr, CopyROP );
	    }

	    _bg = bgPix;
	}
	else
	{
	    // Conatainer palette pixmap not set yet
	    _bg = QPixmap();
	}
    }
    else
    {
	// No background pixmap to use
	_bg = QPixmap();
    }
}

void PanelButtonBase::resizeEvent(QResizeEvent*)
{
    // optimize: reload only when size really changes
    loadTiles();
}

void PanelButtonBase::enterEvent(QEvent* e)
{
  if (_changeCursorOverItem)
    setCursor(KCursor().handCursor());

  if ( !zoomButton )
    zoomButton = new ZoomButton;

  if ( zoomButton->isZoomingEnabled() && /* _icon.width() < 32 && */
      !_iconz.isNull() && _iconz.width() > _icon.width() &&
      !mouseGrabber() && !qApp->activePopupWidget() ) { // we can and should zoom
    if ( !zoomButton->isWatching( this ) ) {
      zoomButton->watchMe( this, mapToGlobal( rect().center() ), _iconz );
      update();
    }
    return;
  }

  _highlight = true;
  repaint(false);
  QButton::enterEvent(e);
}

void PanelButtonBase::leaveEvent(QEvent* e)
{
  if (_changeCursorOverItem)
    setCursor(oldCursor);

  if ( !_highlight )
    return;
  _highlight = false;
  repaint(false);
  QButton::leaveEvent(e);
}

void PanelButtonBase::loadTiles()
{
    if( _tile == QString::null ) {
	_up = _down = QPixmap();
	return;
    }

    QString uptile, downtile;

    if ( height() < 42 ) {
	uptile = _tile + "_tiny_up.png";
	downtile = _tile + "_tiny_down.png";
    }
    else if ( height() < 54 ) {
	uptile = _tile + "_normal_up.png";
	downtile = _tile + "_normal_down.png";
    }
    else {
	uptile = _tile + "_large_up.png";
	downtile = _tile + "_large_down.png";
    }

    _up = QPixmap( KGlobal::dirs()->findResource("tiles", uptile) );
    _down = QPixmap ( KGlobal::dirs()->findResource("tiles", downtile) );

    // scale if size does not match exactly
    if ( !_up.isNull() ) {
	if ( _up.width() != width() || _up.height() != height() ) {
	    QImage upI = _up.convertToImage();
	    _up.convertFromImage( upI.smoothScale( width(), height() ) );
	}
    }

    // scale if size does not match exactly
    if ( !_down.isNull() ) {
	if ( _down.width() != width() || _down.height() != height() ) {
	    QImage downI = _down.convertToImage();
	    _down.convertFromImage( downI.smoothScale( width(), height() ) );
	}
    }
}

void PanelButtonBase::drawButton(QPainter *p)
{
    setBackground();

    QPixmap bg(width(),height());
    QPainter pbg;
    pbg.begin(&bg);
    if(isDown() || isOn() ){

	if (!_down.isNull())     // draw down tile
	{
	    int x = (width() - _down.width())/2;
	    int y = (height() - _down.height())/2;
	    pbg.drawPixmap(x, y, _down);
	}
	else // no tile loaded
	{
	    if ( !_bg.isNull() )
		pbg.drawPixmap( 0, 0, _bg );
 	    else pbg.fillRect(rect(), colorGroup().brush(QColorGroup::Background));

	    pbg.setPen(Qt::black);
	    pbg.drawLine(0, 0, width()-1, 0);
	    pbg.drawLine(0, 0, 0, height()-1);
	    pbg.setPen(colorGroup().light());
	    pbg.drawLine(0, height()-1, width()-1, height()-1);
	    pbg.drawLine(width()-1, 0, width()-1, height()-1);
	}
    }
    else {

	if (!_up.isNull())  // draw up tile
	{
	    int x = (width() - _up.width())/2;
	    int y = (height() - _up.height())/2;
	    pbg.drawPixmap(x, y, _up);
	}
	else // no tile loaded
	{
	    if ( !_bg.isNull() )
		pbg.drawPixmap( 0, 0, _bg );
	    else
	        pbg.fillRect(rect(), colorGroup().brush(QColorGroup::Background));
	}
    }
    pbg.end();
    p->drawPixmap(0,0,bg);
    drawButtonLabel(p, bg);

    if ( hasFocus() ) {
	int x1, y1, x2, y2;
	rect().coords( &x1, &y1, &x2, &y2 );// get coordinates
	QRect r(x1+2, y1+2, x2-x1-3, y2-y1-3);
	style().drawFocusRect( p, r , colorGroup(), &colorGroup().button() );
    }
}

void PanelButtonBase::drawButtonLabel(QPainter *p, const QPixmap &bg)
{
    bool hl = _highlight;

    if ( !zoomButton || !zoomButton->isWatching( this ) ) {
	// draw icon
	if (!hl && !_icon.isNull())
	{
	    int x = (width() - _icon.width())/2;
	    int y = (height() - _icon.height())/2;
	    if (isDown()||isOn()) {
		x+=2;
		y+=2;
	    }
	    KAlphaPainter::draw(p, _icon, bg, x, y);
	}
	else if (hl && !_iconh.isNull())
	{
	    int x = (width() - _iconh.width())/2;
	    int y = (height() - _iconh.height())/2;
	    if (isDown()||isOn()) {
		x+=2;
		y+=2;
	    }
	    KAlphaPainter::draw(p, _iconh, bg, x, y);
	}
    }

    int d = 0;
    int offset = 0;

    if(width() > 32 && height() > 32) offset = 0;
    if(isDown()||isOn()) d = 2;

    // draw arrow
    if(_drawArrow)
    {
	if(_dir == Top)
	    QApplication::style().drawArrow(p, Qt::UpArrow, 
		    isDown()||isOn(), offset + d, offset + d,
		    8, 8, colorGroup(), true);
	else if (_dir == Bottom)
	    QApplication::style().drawArrow(p, Qt::DownArrow, 
		    isDown()||isOn(), offset + d, height() - 8 - offset + d,
		    8, 8, colorGroup(), true);
	else if (_dir == Right)
	    QApplication::style().drawArrow(p, Qt::RightArrow, 
		    isDown()||isOn(), width() - 8 - offset + d, offset + d,
		    8, 8, colorGroup(), true);
	else
	    QApplication::style().drawArrow(p, Qt::LeftArrow, 
		    isDown()||isOn(), offset + d, offset + d,
		    8, 8, colorGroup(), true);
    }
}
