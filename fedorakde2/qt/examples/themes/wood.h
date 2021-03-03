/****************************************************************************
** $Id: qt/examples/themes/wood.h   2.3.2   edited 2001-01-26 $
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef WOOD_H
#define WOOD_H

#include <qwindowsstyle.h>
#include <qpalette.h>

class NorwegianWoodStyle : public QWindowsStyle
{
public:
    NorwegianWoodStyle( int sbext = -1 );
    void polish( QApplication*);
    void polish( QWidget* );
    void unPolish( QWidget* );
    void unPolish( QApplication*);

    void drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken = FALSE,
			     const QBrush *fill = 0 );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  const QBrush *fill = 0 );
    QRect buttonRect( int x, int y, int w, int h);
    void drawButtonMask( QPainter *p, int x, int y, int w, int h);
    void drawComboButton( QPainter *p, int x, int y, int w, int h,
				    const QColorGroup &g,
				    bool /* sunken */,
				    bool editable,
				    bool /*enabled */,
				    const QBrush *fb );


    void drawPushButton( QPushButton* btn, QPainter *p);
    void drawPushButtonLabel( QPushButton* btn, QPainter *p);
    void drawScrollBarControls( QPainter*,  const QScrollBar*, int sliderStart, uint controls, uint activeControl );

private:
    void drawSemicircleButton(QPainter *p, const QRect &r, int dir, 
			      bool sunken, const QColorGroup &g );
    QPalette oldPalette;
    QPixmap *sunkenDark;
    QPixmap *sunkenLight;

};

#endif
