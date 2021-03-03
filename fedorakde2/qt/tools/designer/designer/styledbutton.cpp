/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qvariant.h>  // HP-UX compiler needs this here
#include "styledbutton.h"
#include "pixmapchooser.h"
#include "formwindow.h"
#include <qcolordialog.h>
#include <qpalette.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qimage.h>

StyledButton::StyledButton(QWidget* parent, const char* name)
    : QButton( parent, name ), pix( 0 ), spix( 0 ), s( 0 ), formWindow( 0 )
{
    setMinimumSize( minimumSizeHint() );

    connect( this, SIGNAL(clicked()), SLOT(onEditor()));

    setEditor( ColorEditor );
}

StyledButton::StyledButton( const QBrush& b, QWidget* parent, const char* name, WFlags f )
    : QButton( parent, name, f ), spix( 0 ), s( 0 ), formWindow( 0 )
{
    col = b.color();
    pix = b.pixmap();
    setMinimumSize( minimumSizeHint() );
}

StyledButton::~StyledButton()
{
}

void StyledButton::setEditor( EditorType e )
{
    if ( edit == e )
	return;

    edit = e;
    update();
}

StyledButton::EditorType StyledButton::editor() const
{
    return edit;
}

void StyledButton::setColor( const QColor& c )
{
    col = c;
    update();
}

void StyledButton::setPixmap( const QPixmap & pm )
{
    if ( !pm.isNull() ) {
	delete pix;
	pix = new QPixmap( pm );
    } else {
	delete pix;
	pix = 0;
    }
    scalePixmap();
}

QColor StyledButton::color() const
{
    return col;
}

QPixmap* StyledButton::pixmap() const
{
    return pix;
}

bool StyledButton::scale() const
{
    return s;
}

void StyledButton::setScale( bool on )
{
    if ( s == on )
	return;

    s = on;
    scalePixmap();
}

QSize StyledButton::sizeHint() const
{
    return QSize( 50, 25 );
}

QSize StyledButton::minimumSizeHint() const
{
    return QSize( 50, 25 );
}

void StyledButton::scalePixmap()
{
    delete spix;

    if ( pix ) {
	spix = new QPixmap( width()/2, height()/2 );
	QImage img = pix->convertToImage();

	spix->convertFromImage( s? img.smoothScale( width()/2, height()/2 ) : img );
    } else {
	spix = 0;
    }

    update();
}

void StyledButton::resizeEvent( QResizeEvent* e )
{
    scalePixmap();
    QButton::resizeEvent( e );
}

void StyledButton::drawButton( QPainter *paint )
{
    style().drawBevelButton( paint, 0, 0, width(), height(), colorGroup(), isDown() );
    drawButtonLabel( paint );
    if ( hasFocus() ) {
 	style().drawFocusRect( paint, style().bevelButtonRect( 0, 0, width(), height()),
			       colorGroup(), &colorGroup().button() );
    }
}

void StyledButton::drawButtonLabel( QPainter *paint )
{
    QColor pen = isEnabled() ?
		 hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
		 : palette().disabled().buttonText();
    paint->setPen( pen );
    if ( edit == PixmapEditor && spix ) {
	paint->setBrush( QBrush( col, *spix ) );
	paint->setBrushOrigin( width()/4, height()/4 );
    } else
	paint->setBrush( QBrush( col ) );

    paint->drawRect( width()/4, height()/4, width()/2, height()/2 );
}

void StyledButton::onEditor()
{
    switch (edit) {
    case ColorEditor: {
	QColor c = QColorDialog::getColor( palette().active().background(), this );
	if ( c.isValid() ) {
	    setColor( c );
	    emit changed();
	}
    } break;
    case PixmapEditor: {
	QPixmap p;
        if ( pixmap() )
		p = qChoosePixmap( this, formWindow, *pixmap() );
        else
		p = qChoosePixmap( this, formWindow, QPixmap() );
	if ( !p.isNull() ) {
	    setPixmap( p );
	    emit changed();
	}
    } break;
    default:
	break;
    }
}
