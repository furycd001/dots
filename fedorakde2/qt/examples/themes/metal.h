/****************************************************************************
** $Id: qt/examples/themes/metal.h   2.3.2   edited 2001-02-15 $
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

#ifndef METAL_H
#define METAL_H

#include <qwindowsstyle.h>
#include <qpalette.h>

class MetalStyle : public QWindowsStyle
{
public:
    MetalStyle();
    void polish( QApplication*);
    void unPolish( QApplication*);
    void polish( QWidget* );
    void unPolish( QWidget* );

    
    void drawMetalButton( QPainter *p, int x, int y, int w, int h,
			  bool sunken = FALSE, bool horz = TRUE );
    
    
    void drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken = FALSE,
			     const QBrush *fill = 0 );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  const QBrush *fill = 0 );

    void drawPushButton( QPushButton* btn, QPainter *p);
    void drawPushButtonLabel( QPushButton* btn, QPainter *p);
    void drawPanel( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &, bool sunken,
		    int lineWidth, const QBrush *fill );

    
    void drawSlider( QPainter *p,
		     int x, int y, int w, int h,
		     const QColorGroup &g,
		     Orientation orient, bool tickAbove, bool tickBelow );
    
    void drawScrollBarControls( QPainter* p, const QScrollBar* sb,
				int sliderStart, uint controls,
				uint activeControl );

    
    void drawComboButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  bool editable = FALSE,
			  bool enabled = TRUE,
			  const QBrush *fill = 0 );

private:
    QPalette oldPalette;
};

#endif
