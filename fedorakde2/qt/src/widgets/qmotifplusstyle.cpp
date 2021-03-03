/****************************************************************************
** $Id: qt/src/widgets/qmotifplusstyle.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of QMotifPlusStyle class
**
** Created : 2000.07.27
**
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qmotifplusstyle.h"

#ifndef QT_NO_STYLE_MOTIFPLUS

#define INCLUDE_MENUITEM_DEF
#include <qapplication.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qmenubar.h>
#include <qdrawutil.h>
#include <qscrollbar.h>
#include <qtabbar.h>
#include <qguardedptr.h>
#include <qlayout.h>


typedef void (QStyle::*QDrawMenuBarItemImpl) (QPainter *, int, int, int, int, QMenuItem *,
					      QColorGroup &, bool, bool);

QDrawMenuBarItemImpl qt_set_draw_menu_bar_impl(QDrawMenuBarItemImpl impl);


struct QMotifPlusStylePrivate
{
    QMotifPlusStylePrivate()
	: hoverWidget(0), hovering(FALSE), sliderActive(FALSE),
	  scrollbarElement(0), ref(1), hoverPalette(0)
    { ; }

    QGuardedPtr<QWidget> hoverWidget;
    QPalette oldpalette, prelight_palette;
    bool hovering, sliderActive;
    int scrollbarElement, ref;
    QPoint mousePos;
    QPalette *hoverPalette;
};

static QMotifPlusStylePrivate *d = 0;


/*!
  \class QMotifPlusStyle qmotifplusstyle.h
  \brief More sophisticated Motif-ish look and feel
  \ingroup appearance

 This class implements a Motif-ish look and feel with more
 sophisticated bevelling as used by the GIMP Toolkit (GTK+) for
 Unix/X11.
 */

/*!
  Constructs a QMotifPlusStyle

  If \a hoveringHighlight is FALSE (default value), then the style will not
  highlight push buttons, checkboxes, radiobuttons, comboboxes, scrollbars
  and sliders.
 */
QMotifPlusStyle::QMotifPlusStyle(bool hoveringHighlight) : QMotifStyle(TRUE)
{
    if (! d) {
	d = new QMotifPlusStylePrivate;
    } else {
	d->ref++;
    }

    setScrollBarExtent(15, 15);
    setButtonDefaultIndicatorWidth(5);
    setSliderThickness(15);
    setButtonMargin(2);

    useHoveringHighlight = hoveringHighlight;
}

/*!
  Destructs the style.
 */
QMotifPlusStyle::~QMotifPlusStyle()
{
    if (d && d->ref == 0) {
	delete d;
	d = 0;
    }
}


/*!
  \reimp
*/

void QMotifPlusStyle::polish(QPalette &pal)
{
    d->oldpalette = pal;

    QColor bg = pal.color(QPalette::Active, QColorGroup::Background);

    if (bg.red()   == 0xc0 &&
	bg.green() == 0xc0 &&
	bg.blue()  == 0xc0) {
	// assume default palette... no -bg arg or color read from RESOURCE_MANAGER

	QColor gtkdf(0x75, 0x75, 0x75);
	QColor gtksf(0xff, 0xff, 0xff);
	QColor gtkbg(0xd6, 0xd6, 0xd6);
	QColor gtksl(0x00, 0x00, 0x9c);

	pal.setColor(QPalette::Active, QColorGroup::Background, gtkbg);
	pal.setColor(QPalette::Inactive, QColorGroup::Background, gtkbg);
	pal.setColor(QPalette::Disabled, QColorGroup::Background, gtkbg);

	pal.setColor(QPalette::Active, QColorGroup::Button, gtkbg);
	pal.setColor(QPalette::Inactive, QColorGroup::Button, gtkbg);
	pal.setColor(QPalette::Disabled, QColorGroup::Button, gtkbg);

	pal.setColor(QPalette::Active, QColorGroup::Highlight, gtksl);
	pal.setColor(QPalette::Inactive, QColorGroup::Highlight, gtksl);
	pal.setColor(QPalette::Disabled, QColorGroup::Highlight, gtksl);

	pal.setColor(QPalette::Active, QColorGroup::HighlightedText, gtksf);
	pal.setColor(QPalette::Inactive, QColorGroup::HighlightedText, gtksf);
	pal.setColor(QPalette::Disabled, QColorGroup::HighlightedText, gtkdf);
    }

    {
	QColorGroup active(pal.color(QPalette::Active,
				     QColorGroup::Foreground),           // foreground
			   pal.color(QPalette::Active,
				     QColorGroup::Button),               // button
			   pal.color(QPalette::Active,
				     QColorGroup::Background).light(),   // light
			   pal.color(QPalette::Active,
				     QColorGroup::Background).dark(142), // dark
			   pal.color(QPalette::Active,
				     QColorGroup::Background).dark(110), // mid
			   pal.color(QPalette::Active,
				     QColorGroup::Text),                 // text
			   pal.color(QPalette::Active,
				     QColorGroup::BrightText),           // bright text
			   pal.color(QPalette::Active,
				     QColorGroup::Base),                 // base
			   pal.color(QPalette::Active,
				     QColorGroup::Background)),          // background


	    disabled(pal.color(QPalette::Disabled,
			       QColorGroup::Foreground),                 // foreground
		     pal.color(QPalette::Disabled,
			       QColorGroup::Button),                     // button
		     pal.color(QPalette::Disabled,
			       QColorGroup::Background).light(),         // light
		     pal.color(QPalette::Disabled,
			       QColorGroup::Background).dark(156),       // dark
		     pal.color(QPalette::Disabled,
			       QColorGroup::Background).dark(110),       // mid
		     pal.color(QPalette::Disabled,
			       QColorGroup::Text),                       // text
		     pal.color(QPalette::Disabled,
			       QColorGroup::BrightText),                 // bright text
		     pal.color(QPalette::Disabled,
			       QColorGroup::Base),                       // base
		     pal.color(QPalette::Disabled,
			       QColorGroup::Background));                // background

	active.setColor(QColorGroup::Highlight,
			pal.color(QPalette::Active, QColorGroup::Highlight));
	disabled.setColor(QColorGroup::Highlight,
			  pal.color(QPalette::Disabled, QColorGroup::Highlight));

	active.setColor(QColorGroup::HighlightedText,
			pal.color(QPalette::Active, QColorGroup::HighlightedText));
	disabled.setColor(QColorGroup::HighlightedText,
			  pal.color(QPalette::Disabled, QColorGroup::HighlightedText));

	pal.setActive(active);
	pal.setInactive(active);
	pal.setDisabled(disabled);
    }

    {
	QColor prelight;

	if ( (bg.red() + bg.green() + bg.blue()) / 3 > 128)
	    prelight = pal.color(QPalette::Active,
				 QColorGroup::Background).light(110);
	else
	    prelight = pal.color(QPalette::Active,
				 QColorGroup::Background).light(120);

	QColorGroup active2(pal.color(QPalette::Active,
				      QColorGroup::Foreground), // foreground
			    prelight,                           // button
			    prelight.light(),                   // light
			    prelight.dark(156),                 // dark
			    prelight.dark(110),                 // mid
			    pal.color(QPalette::Active,
				      QColorGroup::Text),       // text
			    pal.color(QPalette::Active,
				      QColorGroup::BrightText), // bright text
			    pal.color(QPalette::Active,
				      QColorGroup::Base),       // base
			    prelight);                          // background

	d->prelight_palette = pal;
	d->prelight_palette.setActive(active2);
	d->prelight_palette.setInactive(active2);
    }
}


/*!
  \reimp
*/
void QMotifPlusStyle::polish(QWidget *widget)
{
    if (widget->inherits("QFrame") &&
	((QFrame *) widget)->frameStyle() == QFrame::Panel)
	((QFrame *) widget)->setFrameStyle(QFrame::WinPanel);

#ifndef QT_NO_MENUBAR
    if (widget->inherits("QMenuBar") &&
	((QMenuBar *) widget)->frameStyle() != QFrame::NoFrame)
	((QMenuBar *) widget)->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
#endif

    if (widget->inherits("QToolBar"))
	widget->layout()->setMargin(2);

    if (useHoveringHighlight) {
	if (widget->inherits("QButton") ||
	    widget->inherits("QComboBox"))
	    widget->installEventFilter(this);

	if (widget->inherits("QScrollBar") ||
	    widget->inherits("QSlider")) {
	    widget->setMouseTracking(TRUE);
	    widget->installEventFilter(this);
	}
    }

    QMotifStyle::polish(widget);
}


/*!
  \reimp
*/
void QMotifPlusStyle::unPolish(QWidget *widget)
{
    widget->removeEventFilter(this);
    QMotifStyle::unPolish(widget);
}


/*!
  \reimp
*/
void QMotifPlusStyle::polish(QApplication *)
{
    qt_set_draw_menu_bar_impl((QDrawMenuBarItemImpl) &QMotifPlusStyle::drawMenuBarItem);
}


/*!
  \reimp
*/
void QMotifPlusStyle::unPolish(QApplication *app)
{
    app->setPalette(d->oldpalette);

    qt_set_draw_menu_bar_impl(0);
}


/*!
  \reimp
*/
void QMotifPlusStyle::polishPopupMenu(QPopupMenu *menu)
{
    menu->setMouseTracking(TRUE);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawPushButton(QPushButton *button, QPainter *p)
{
    int x1, y1, x2, y2;
    button->rect().coords(&x1, &y1, &x2, &y2);

    if (button->isDefault())
	drawButton(p, x1, y1, x2 - x1 + 1, y2 - y1 + 1,
		   qApp->palette().active(), TRUE);

    if (button->isDefault() || button->autoDefault()) {
	x1 += buttonDefaultIndicatorWidth();
	y1 += buttonDefaultIndicatorWidth();
	x2 -= buttonDefaultIndicatorWidth();
	y2 -= buttonDefaultIndicatorWidth();
    }

    QBrush fill;
    if (button->isDown() || button->isOn())
	fill = button->colorGroup().brush(QColorGroup::Mid);
    else
	fill = button->colorGroup().brush(QColorGroup::Button);

    if ( !button->isFlat() || button->isOn() || button->isDown() )
	drawButton(p, x1, y1, x2 - x1 + 1, y2 - y1 + 1,
		   button->colorGroup(), button->isOn() || button->isDown(), &fill);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawButton(QPainter *p, int x, int y, int w, int h,
				 const QColorGroup &g, bool sunken, const QBrush *fill)
{
    QPen oldpen = p->pen();
    QPointArray a(4);

    if (sunken) p->setPen(g.dark());
    else p->setPen(g.light());

    a.setPoint(0, x, y + h - 1);
    a.setPoint(1, x, y);
    a.setPoint(2, x, y);
    a.setPoint(3, x + w - 1, y);

    p->drawLineSegments(a);

    if (sunken) p->setPen(black);
    else p->setPen(g.button());

    a.setPoint(0, x + 1, y + h - 2);
    a.setPoint(1, x + 1, y + 1);
    a.setPoint(2, x + 1, y + 1);
    a.setPoint(3, x + w - 2, y + 1);

    p->drawLineSegments(a);

    if (sunken) p->setPen(g.button());
    else p->setPen(g.dark());

    a.setPoint(0, x + 2, y + h - 2);
    a.setPoint(1, x + w - 2, y + h - 2);
    a.setPoint(2, x + w - 2, y + h - 2);
    a.setPoint(3, x + w - 2, y + 2);

    p->drawLineSegments(a);

    if (sunken) p->setPen(g.light());
    else p->setPen(black);

    a.setPoint(0, x + 1, y + h - 1);
    a.setPoint(1, x + w - 1, y + h - 1);
    a.setPoint(2, x + w - 1, y + h - 1);
    a.setPoint(3, x + w - 1, y);

    p->drawLineSegments(a);

    if ( fill )
	p->fillRect(x + 2, y + 2, w - 4, h - 4, *fill);
    else
	p->fillRect(x + 2, y + 2, w - 4, h - 4, QBrush(g.button()));

    p->setPen(oldpen);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
				      const QColorGroup &g, bool sunken, const QBrush *fill)
{
    drawButton(p, x, y, w, h, g, sunken, fill);
}


/*!
  \reimp
*/
void QMotifPlusStyle::getButtonShift(int &x, int &y)
{
    x = y = 0;
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawComboButton(QPainter *p, int x, int y, int w, int h,
				 const QColorGroup &g, bool sunken,
				 bool editable, bool,
				 const QBrush *fill)
{
    drawButton(p, x, y, w, h, g, sunken, fill);

    if (editable) {
	QRect r = comboButtonRect(x, y, w, h);
	drawButton(p, r.x() - defaultFrameWidth(),
		   r.y() - defaultFrameWidth(),
		   r.width() + (defaultFrameWidth() * 2),
		   r.height() + (defaultFrameWidth() * 2),
		   g, TRUE);
    }

    int indent = ((y + h) / 2) - 6;
    drawArrow(p, Qt::DownArrow, TRUE, x + w - indent - 13, indent,
	13, 13, g, TRUE, fill);
}


/*!
  \reimp
*/
QRect QMotifPlusStyle::comboButtonRect(int x, int y, int w, int h)
{
    QRect r(x + (defaultFrameWidth() * 2), y + (defaultFrameWidth() * 2),
	    w - (defaultFrameWidth() * 4), h - (defaultFrameWidth() * 4));

    int indent = ((y + h) / 2) - defaultFrameWidth();
    r.setRight(r.right() - indent - 13);

    return r;
}


/*!
  \reimp
*/
QRect QMotifPlusStyle::comboButtonFocusRect(int x, int y, int w, int h)
{
    return comboButtonRect(x, y, w, h);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawPanel(QPainter *p, int x, int y, int w, int h,
			   const QColorGroup &g, bool sunken,
			   int, const QBrush *)
{
    QPen oldpen = p->pen();
    QPointArray a(4);

    if (sunken) p->setPen(g.dark());
    else p->setPen(g.light());

    a.setPoint(0, x, y + h - 1);
    a.setPoint(1, x, y);
    a.setPoint(2, x, y);
    a.setPoint(3, x + w - 1, y);

    p->drawLineSegments(a);

    if (sunken) p->setPen(black);
    else p->setPen(g.button());

    a.setPoint(0, x + 1, y + h - 2);
    a.setPoint(1, x + 1, y + 1);
    a.setPoint(2, x + 1, y + 1);
    a.setPoint(3, x + w - 2, y + 1);

    p->drawLineSegments(a);

    if (sunken) p->setPen(g.button());
    else p->setPen(g.dark());

    a.setPoint(0, x + 2, y + h - 2);
    a.setPoint(1, x + w - 2, y + h - 2);
    a.setPoint(2, x + w - 2, y + h - 2);
    a.setPoint(3, x + w - 2, y + 2);

    p->drawLineSegments(a);

    if (sunken) p->setPen(g.light());
    else p->setPen(black);

    a.setPoint(0, x + 1, y + h - 1);
    a.setPoint(1, x + w - 1, y + h - 1);
    a.setPoint(2, x + w - 1, y + h - 1);
    a.setPoint(3, x + w - 1, y);

    p->drawLineSegments(a);

    p->setPen(oldpen);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawIndicator(QPainter *p, int x, int y ,int w, int h,
			       const QColorGroup &g, int state,
			       bool, bool)
{
    QBrush fill;
    if (state != QButton::Off) fill = g.brush(QColorGroup::Mid);
    else fill = g.brush(QColorGroup::Button);

    if (state == QButton::NoChange) {
        qDrawPlainRect(p, x, y, w, h, g.text(), 1, &fill);
        p->drawLine(x + w - 1, y, x, y + h - 1);
    } else {
        drawButton(p, x, y, w, h, g, (state != QButton::Off), &fill);
    }
}


/*!
  \reimp
*/
QSize QMotifPlusStyle::indicatorSize() const
{
    return QSize(10, 10);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawExclusiveIndicator(QPainter *p, int x, int y, int w, int h,
					const QColorGroup &g, bool on,
					bool, bool)
{
    QPen oldpen =  p->pen();

    p->fillRect(x, y, w, h, g.button());

    QPointArray thick(8);
    QPointArray thin(4);

    if (on) {
        thick.setPoint(0, x, y + (h / 2));
	thick.setPoint(1, x + (w / 2), y);
	thick.setPoint(2, x + 1, y + (h / 2));
	thick.setPoint(3, x + (w / 2), y + 1);
	thick.setPoint(4, x + (w / 2), y);
	thick.setPoint(5, x + w - 1, y + (h / 2));
	thick.setPoint(6, x + (w / 2), y + 1);
	thick.setPoint(7, x + w - 2, y + (h / 2));

	p->setPen(g.dark());
	p->drawLineSegments(thick);

	thick.setPoint(0, x + 1, y + (h / 2) + 1);
	thick.setPoint(1, x + (w / 2), y + h - 1);
	thick.setPoint(2, x + 2, y + (h / 2) + 1);
	thick.setPoint(3, x + (w / 2), y + h - 2);
	thick.setPoint(4, x + (w / 2), y + h - 1);
	thick.setPoint(5, x + w - 2, y + (h / 2) + 1);
	thick.setPoint(6, x + (w / 2), y + h - 2);
	thick.setPoint(7, x + w - 3, y + (h / 2) + 1);

	p->setPen(g.light());
	p->drawLineSegments(thick);

	thin.setPoint(0, x + 2, y + (h / 2));
	thin.setPoint(1, x + (w / 2), y + 2);
	thin.setPoint(2, x + (w / 2), y + 2);
	thin.setPoint(3, x + w - 3, y + (h / 2));

	p->setPen(black);
	p->drawLineSegments(thin);

	thin.setPoint(0, x + 3, y + (h / 2) + 1);
	thin.setPoint(1, x + (w / 2), y + h - 3);
	thin.setPoint(2, x + (w / 2), y + h - 3);
	thin.setPoint(3, x + w - 4, y + (h / 2) + 1);

	p->setPen(g.mid());
	p->drawLineSegments(thin);
    } else {
        thick.setPoint(0, x, y + (h / 2));
	thick.setPoint(1, x + (w / 2), y);
	thick.setPoint(2, x + 1, y + (h / 2));
	thick.setPoint(3, x + (w / 2), y + 1);
	thick.setPoint(4, x + (w / 2), y);
	thick.setPoint(5, x + w - 1, y + (h / 2));
	thick.setPoint(6, x + (w / 2), y + 1);
	thick.setPoint(7, x + w - 2, y + (h / 2));

	p->setPen(g.light());
	p->drawLineSegments(thick);

	thick.setPoint(0, x + 2, y + (h / 2) + 1);
	thick.setPoint(1, x + (w / 2), y + h - 2);
	thick.setPoint(2, x + 3, y + (h / 2) + 1);
	thick.setPoint(3, x + (w / 2), y + h - 3);
	thick.setPoint(4, x + (w / 2), y + h - 2);
	thick.setPoint(5, x + w - 3, y + (h / 2) + 1);
	thick.setPoint(6, x + (w / 2), y + h - 3);
	thick.setPoint(7, x + w - 4, y + (h / 2) + 1);

	p->setPen(g.dark());
	p->drawLineSegments(thick);

	thin.setPoint(0, x + 2, y + (h / 2));
	thin.setPoint(1, x + (w / 2), y + 2);
	thin.setPoint(2, x + (w / 2), y + 2);
	thin.setPoint(3, x + w - 3, y + (h / 2));

	p->setPen(g.button());
	p->drawLineSegments(thin);

	thin.setPoint(0, x + 1, y + (h / 2) + 1);
	thin.setPoint(1, x + (w / 2), y + h - 1);
	thin.setPoint(2, x + (w / 2), y + h - 1);
	thin.setPoint(3, x + w - 2, y + (h / 2) + 1);

	p->setPen(black);
	p->drawLineSegments(thin);

    }

    p->setPen(oldpen);
}


/*!
  \reimp
*/
QSize QMotifPlusStyle::exclusiveIndicatorSize() const
{
    return QSize(11, 11);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawMenuBarItem(QPainter *p, int x, int y, int w, int h, QMenuItem *mi,
			 QColorGroup &g, bool enabled, bool activated)
{
    if (enabled && activated)
	drawButton(p, x, y, w, h, d->prelight_palette.active(), FALSE);

    drawItem(p, x, y, w, h, AlignCenter | ShowPrefix | DontClip | SingleLine,
	     g, enabled, mi->pixmap(), mi->text(), -1, &g.buttonText());
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawPopupMenuItem(QPainter *p, bool checkable, int maxpmw, int tab,
				   QMenuItem *mi, const QPalette &pl, bool act,
				   bool enabled, int x, int y, int w, int h)
{
    QPalette pal = (act && enabled) ? d->prelight_palette : pl;
    const QColorGroup & g = pal.active();
    QColorGroup itemg = (! enabled) ? pal.disabled() : pal.active();

    if (checkable)
	maxpmw = QMAX(maxpmw, 15);

    int checkcol = maxpmw;

    if (mi && mi->isSeparator()) {
	p->setPen( g.dark() );
	p->drawLine( x, y, x+w, y );
	p->setPen( g.light() );
	p->drawLine( x, y+1, x+w, y+1 );
	return;
    }

    if ( act && enabled ) drawButton(p, x, y, w, h, g, FALSE, &g.brush(QColorGroup::Button));
    else p->fillRect(x, y, w, h, g.brush( QColorGroup::Button ));

    if ( !mi )
	return;

    if ( mi->isChecked() ) {
	if ( mi->iconSet() ) {
	    qDrawShadePanel( p, x+2, y+2, checkcol, h-2*2,
			     g, TRUE, 1, &g.brush( QColorGroup::Midlight ) );
	}
    } else if ( !act ) {
	p->fillRect(x+2, y+2, checkcol, h-2*2,
		    g.brush( QColorGroup::Button ));
    }

    if ( mi->iconSet() ) {		// draw iconset
	QIconSet::Mode mode = (enabled) ? QIconSet::Normal : QIconSet::Disabled;

	if (act && enabled)
	    mode = QIconSet::Active;

	QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);

	int pixw = pixmap.width();
	int pixh = pixmap.height();

	QRect cr( x + 2, y+2, checkcol, h-2*2 );
	QRect pmr( 0, 0, pixw, pixh );

	pmr.moveCenter(cr.center());

	p->setPen( itemg.text() );
	p->drawPixmap( pmr.topLeft(), pixmap );

    } else  if (checkable) {
	int mw = checkcol;
	int mh = h - 4;

	if (mi->isChecked())
	    drawCheckMark(p, x+2, y+2, mw, mh, itemg, act, ! enabled);
    }

    p->setPen( g.buttonText() );

    QColor discol;
    if (! enabled) {
	discol = itemg.text();
	p->setPen( discol );
    }

    if (mi->custom()) {
	p->save();
	mi->custom()->paint(p, itemg, act, enabled, x + checkcol + 4, y + 2,
			    w - checkcol - tab - 3, h - 4);
	p->restore();
    }

    QString s = mi->text();
    if ( !s.isNull() ) {			// draw text
	int t = s.find( '\t' );
	int m = 2;
	const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
	if ( t >= 0 ) {				// draw tab text
	    p->drawText( x+w-tab-2-2,
			 y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
	}
	p->drawText(x + checkcol + 4, y + 2, w - checkcol -tab - 3, h - 4,
		    text_flags, s, t);
    } else if (mi->pixmap()) {
	QPixmap *pixmap = mi->pixmap();

	if (pixmap->depth() == 1) p->setBackgroundMode(OpaqueMode);
	p->drawPixmap(x + checkcol + 2, y + 2, *pixmap);
	if (pixmap->depth() == 1) p->setBackgroundMode(TransparentMode);
    }

    if (mi->popup()) {
	int hh = h / 2;

	drawArrow(p, RightArrow, (act) ? mi->isEnabled() : FALSE,
		  x + w - hh - 6, y + (hh / 2), hh, hh, g, mi->isEnabled());
    }
}

/*!
  \reimp
*/
int QMotifPlusStyle::defaultFrameWidth() const
{
    return 2;
}

/*!
  \reimp
*/
void QMotifPlusStyle::drawArrow(QPainter *p, ArrowType type, bool down,
			   int x, int y, int w, int h,
			   const QColorGroup &g, bool, const QBrush *)

{
    QPen oldpen = p->pen();
    QBrush oldbrush = p->brush();
    p->save();

    QPointArray poly(3);

    p->setBrush(g.button());

    switch (type) {
    case UpArrow:
	{
	    poly.setPoint(0, x + (w / 2), y );
	    poly.setPoint(1, x, y + h - 1);
	    poly.setPoint(2, x + w - 1, y + h - 1);

	    p->drawPolygon(poly);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + 1, y + h - 2, x + w - 2, y + h - 2);

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x, y + h - 1, x + w - 1, y + h - 1);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + w - 2, y + h - 1, x + (w / 2), y + 1);

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x + w - 1, y + h - 1, x + (w / 2), y);

	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine(x + (w / 2), y + 1, x + 1, y + h - 1);

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x + (w / 2), y, x, y + h - 1);

	    break;
    	}

    case DownArrow:
	{
	    poly.setPoint(0, x + w - 1, y);
	    poly.setPoint(1, x, y);
	    poly.setPoint(2, x + (w / 2), y + h - 1);

	    p->drawPolygon(poly);

	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine(x + w - 2, y + 1, x + 1, y + 1);

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x + w - 1, y, x, y);

	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine(x + 1, y, x + (w / 2), y + h - 2);

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x, y, x + (w / 2), y + h - 1);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + (w / 2), y + h - 2, x + w - 2, y);

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x + (w / 2), y + h - 1, x + w - 1, y);

	    break;
	}

    case LeftArrow:
	{
	    poly.setPoint(0, x, y + (h / 2));
	    poly.setPoint(1, x + w - 1, y + h - 1);
	    poly.setPoint(2, x + w - 1, y);

	    p->drawPolygon(poly);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + 1, y + (h / 2), x + w - 1, y + h - 1);

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x, y + (h / 2), x + w - 1, y + h - 1);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + w - 2, y + h - 1, x + w - 2, y + 1);

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x + w - 1, y + h - 1, x + w - 1, y);

	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine(x + w - 1, y + 1, x + 1, y + (h / 2));

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x + w - 1, y, x, y + (h / 2));

	    break;
	}

    case RightArrow:
	{
	    poly.setPoint(0, x + w - 1, y + (h / 2));
	    poly.setPoint(1, x, y);
	    poly.setPoint(2, x, y + h - 1);

	    p->drawPolygon(poly);
	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine( x + w - 1, y + (h / 2), x + 1, y + 1);

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x + w - 1, y + (h / 2), x, y);

	    if (down) p->setPen(black);
	    else p->setPen(g.button());
	    p->drawLine(x + 1, y + 1, x + 1, y + h - 2);

	    if (down) p->setPen(g.dark());
	    else p->setPen(g.light());
	    p->drawLine(x, y, x, y + h - 1);

	    if (down) p->setPen(g.button());
	    else p->setPen(g.dark());
	    p->drawLine(x + 1, y + h - 2, x + w - 1, y + (h / 2));

	    if (down) p->setPen(g.light());
	    else p->setPen(black);
	    p->drawLine(x, y + h - 1, x + w - 1, y + (h / 2));

	    break;
	}
    }

    p->restore();
    p->setBrush(oldbrush);
    p->setPen(oldpen);
}


/*!
  \reimp
*/
void QMotifPlusStyle::scrollBarMetrics(const QScrollBar *scrollbar, int &sliderMin,
				  int &sliderMax, int &sliderLength, int &buttonDim)
{
    QMotifStyle::scrollBarMetrics(scrollbar, sliderMin, sliderMax,
				  sliderLength, buttonDim);

    sliderMin += 1;
    sliderMax -= 1;

    return;
}


#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	defaultFrameWidth()
#define SLIDER_MIN	buttonDim

/*!
  \reimp
*/
void QMotifPlusStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
					 int sliderStart, uint controls,
					 uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();
    QColorGroup pg = d->prelight_palette.active();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }

    int b = MOTIF_BORDER;
    int dimB = buttonDim;
    QRect addB;
    QRect subB;
    QRect addPageR;
    QRect subPageR;
    QRect sliderR;
    int addX, addY, subX, subY;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( HORIZONTAL ) {
	subY = addY = ( extent - dimB ) / 2;
	subX = b;
	addX = length - dimB - b;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = b;
	addY = length - dimB - b;
    }

    subB.setRect( subX,subY,dimB,dimB );
    addB.setRect( addX,addY,dimB,dimB );

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent - b*2;
    if ( HORIZONTAL ) {
	subPageR.setRect( subB.right() + 1, b,
			  sliderStart - subB.right() - 1 , sliderW );
	addPageR.setRect( sliderEnd, b, addX - sliderEnd, sliderW );
	sliderR .setRect( sliderStart, b, sliderLength, sliderW );
    } else {
	subPageR.setRect( b, subB.bottom() + 1, sliderW,
			  sliderStart - subB.bottom() - 1 );
	addPageR.setRect( b, sliderEnd, sliderW, addY - sliderEnd );
	sliderR .setRect( b, sliderStart, sliderW, sliderLength );
    }

    bool scrollbarUpdate = FALSE;
    if (d->hovering) {
	if (addB.contains(d->mousePos)) {
	    scrollbarUpdate = (d->scrollbarElement == AddLine);
	    d->scrollbarElement = AddLine;
	} else if (subB.contains(d->mousePos)) {
	    scrollbarUpdate = (d->scrollbarElement == SubLine);
	    d->scrollbarElement = SubLine;
	} else if (sliderR.contains(d->mousePos)) {
	    scrollbarUpdate = (d->scrollbarElement == Slider);
	    d->scrollbarElement = Slider;
	} else
	    d->scrollbarElement = 0;
    } else
	d->scrollbarElement = 0;

    if (scrollbarUpdate) return;

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) )
	drawButton(p, sb->rect().x(), sb->rect().y(),
		   sb->rect().width(), sb->rect().height(), g, TRUE,
		   &g.brush(QColorGroup::Mid));

    if ( controls & AddLine )
	drawArrow( p, VERTICAL ? DownArrow : RightArrow,
		   ADD_LINE_ACTIVE, addB.x(), addB.y(),
		   addB.width(), addB.height(),
		   (ADD_LINE_ACTIVE ||
		    d->scrollbarElement == AddLine) ? pg : g, TRUE );
    if ( controls & SubLine )
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow,
		   SUB_LINE_ACTIVE, subB.x(), subB.y(),
		   subB.width(), subB.height(),
		   (SUB_LINE_ACTIVE ||
		    d->scrollbarElement == SubLine) ? pg : g, TRUE );

    QBrush fill = g.brush( QColorGroup::Mid );
    if (sb->backgroundPixmap() ){
	fill = QBrush( g.mid(), *sb->backgroundPixmap() );
    }

    if ( controls & SubPage )
	p->fillRect( subPageR, fill );

    if ( controls & AddPage )
	p->fillRect( addPageR, fill );

    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	if ( sliderR.isValid() ) {
	    drawBevelButton( p, sliderR.x(), sliderR.y(),
			     sliderR.width(), sliderR.height(),
			     (activeControl & Slider ||
			      d->scrollbarElement == Slider) ? pg : g,
			     FALSE,
			     (activeControl & Slider ||
			      d->scrollbarElement == Slider) ?
			     &pg.brush( QColorGroup::Button ) :
			     &g.brush( QColorGroup::Button ) );
	}

	p->setBrushOrigin(bo);
    }

}


/*!
  \reimp
*/
void QMotifPlusStyle::drawTab(QPainter *p, const QTabBar *tabbar, QTab *tab,
			      bool selected)
{
    QColorGroup g = tabbar->colorGroup();
    QPen oldpen = p->pen();
    QRect fr(tab->r);

    if (! selected) {
	if (tabbar->shape() == QTabBar::RoundedAbove ||
	    tabbar->shape() == QTabBar::TriangularAbove) {
	    
    	    fr.setTop(fr.top() + 2);
	} else {
	    fr.setBottom(fr.bottom() - 2);
	}
    }
    
    fr.setWidth(fr.width() - 3);
    
    p->fillRect(fr.left() + 1, fr.top() + 1, fr.width() - 2, fr.height() - 2,
		(selected) ? g.brush(QColorGroup::Button) : g.brush(QColorGroup::Mid));

    if (tabbar->shape() == QTabBar::RoundedAbove) {
	// "rounded" tabs on top
	fr.setBottom(fr.bottom() - 1);

	p->setPen(g.light());
	p->drawLine(fr.left(), fr.top() + 1, fr.left(), fr.bottom() - 1);
	p->drawLine(fr.left() + 1, fr.top(), fr.right() - 1, fr.top());
	if (! selected) p->drawLine(fr.left(), fr.bottom(),
				    fr.right() + 3, fr.bottom());

	if (fr.left() == 0)
	    p->drawLine(fr.left(), fr.bottom(), fr.left(), fr.bottom() + 1);

	p->setPen(g.dark());
	p->drawLine(fr.right() - 1, fr.top() + 2, fr.right() - 1, fr.bottom() - 1);

	p->setPen(black);
	p->drawLine(fr.right(), fr.top() + 1, fr.right(), fr.bottom() - 1);
    } else if (tabbar->shape() == QTabBar::RoundedBelow) {
	// "rounded" tabs on bottom
	fr.setTop(fr.top() + 1);

	p->setPen(g.dark());
	p->drawLine(fr.right() + 3, fr.top() - 1,
		    fr.right() - 1, fr.top() - 1);
	p->drawLine(fr.right() - 1, fr.top(),
		    fr.right() - 1, fr.bottom() - 2);
	p->drawLine(fr.right() - 1, fr.bottom() - 2,
		    fr.left() + 2,  fr.bottom() - 2);
	if (! selected) {
	    p->drawLine(fr.right(), fr.top() - 1,
			fr.left() + 1,  fr.top() - 1);
	    
	    if (fr.left() != 0)
		p->drawPoint(fr.left(), fr.top() - 1);
	}

	p->setPen(black);
	p->drawLine(fr.right(), fr.top(),
		    fr.right(), fr.bottom() - 2);
	p->drawLine(fr.right() - 1, fr.bottom() - 1,
		    fr.left(), fr.bottom() - 1);
	if (! selected)
	    p->drawLine(fr.right() + 3, fr.top(), fr.left(), fr.top());
	else
	    p->drawLine(fr.right() + 3, fr.top(), fr.right(), fr.top());
	
	p->setPen(g.light());
	p->drawLine(fr.left(), fr.top() + 1,
		    fr.left(), fr.bottom() - 2);
	
	if (selected) {
	    p->drawPoint(fr.left(), fr.top());
	    if (fr.left() == 0)
		p->drawPoint(fr.left(), fr.top() - 1);
	
	    p->setPen(g.button());
	    p->drawLine(fr.left() + 2, fr.top() - 1, fr.left() + 1, fr.top() - 1);
	}
    } else {
	// triangular drawing code
	QCommonStyle::drawTab(p, tabbar, tab, selected);
    }

    p->setPen(oldpen);
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, Orientation orientation,
		    bool, bool)
{
    QRect sliderR(x, y, w, h);
    QColorGroup cg;

    cg = ((d->hovering && sliderR.contains(d->mousePos)) || d->sliderActive) ?
	 d->prelight_palette.active() : g;

    if (orientation == Horizontal) {
	drawButton(p, x, y, w / 2, h, cg, FALSE, &cg.brush(QColorGroup::Button));
	drawButton(p, x + (w / 2), y, w / 2, h, cg, FALSE, &cg.brush(QColorGroup::Button));
    } else {
	drawButton(p, x, y, w, h / 2, cg, FALSE, &cg.brush(QColorGroup::Button));
	drawButton(p, x, y + (h / 2), w, h / 2, cg, FALSE, &cg.brush(QColorGroup::Button));
    }
}


/*!
  \reimp
*/
void QMotifPlusStyle::drawSliderGroove(QPainter *p, int x, int y, int w, int h,
				       const QColorGroup& g, QCOORD,
				       Orientation )
{
    drawButton(p, x, y, w, h, g, TRUE, &g.brush(QColorGroup::Mid));
}


/*!
  \reimp
*/
bool QMotifPlusStyle::eventFilter(QObject *object, QEvent *event)
{
    switch(event->type()) {
    case QEvent::MouseButtonPress:
	{
	    if (object->inherits("QSlider"))
		d->sliderActive = TRUE;

	    break;
	}

    case QEvent::MouseButtonRelease:
	{
	    if (object->inherits("QSlider")) {
		d->sliderActive = FALSE;
		((QWidget *) object)->repaint(FALSE);
	    }

	    break;
	}

    case QEvent::Enter:
	{
	    if (object->isWidgetType()) {
		d->hoverWidget = (QWidget *) object;
		if (d->hoverWidget->isEnabled()) {
		    if (object->inherits("QScrollBar") ||
			object->inherits("QSlider")) {
			d->hoverWidget->repaint(FALSE);
		    } else if (object->inherits("QPushButton")) {
			QPalette pal = d->hoverWidget->palette();

			if (d->hoverWidget->ownPalette())
			    d->hoverPalette = new QPalette(pal);

			pal.setColor(QPalette::Active, QColorGroup::Button,
				     d->prelight_palette.color(QPalette::Active,
							       QColorGroup::Button));

			d->hoverWidget->setPalette(pal);
		    } else
			d->hoverWidget->setPalette(d->prelight_palette);
		} else
		    d->hoverWidget = 0;
	    }

	    break;
	}

    case QEvent::Leave:
	{
	    if (object == d->hoverWidget) {
		if (d->hoverPalette) {
		    d->hoverWidget->setPalette(*(d->hoverPalette));
		    delete d->hoverPalette;
		    d->hoverPalette = 0;
		} else {
		    d->hoverWidget->unsetPalette();
		}

		d->hoverWidget = 0;
	    }

	    break;
	}

    case QEvent::MouseMove:
	{
	    if (object->isWidgetType() &&
		object == d->hoverWidget) {
		if (object->inherits("QScrollBar") ||
		    object->inherits("QSlider")) {
		    d->mousePos = ((QMouseEvent *) event)->pos();
		    d->hovering = (((QMouseEvent *) event)->button() == NoButton);
		    d->hoverWidget->repaint(FALSE);
		    d->hovering = FALSE;
		}
	    }

	    break;
	}

    default:
	{
	    ;
	}
    }

    return QMotifStyle::eventFilter(object, event);
}


#endif // QT_NO_STYLE_MOTIFPLUS
