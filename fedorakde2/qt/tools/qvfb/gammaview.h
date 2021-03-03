/****************************************************************************
**
** Qt/Embedded virtual framebuffer
**
** Created : 20000605
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about the Professional Edition licensing.
**
*****************************************************************************/

#ifndef GAMMAVIEW_H
#define GAMMAVIEW_H

#include <qwidget.h>

class GammaView: public QWidget
{
    Q_OBJECT
public:
    GammaView( QWidget *parent = 0,
		const char *name = 0, WFlags f = 0 ) :
	QWidget(parent,name,f)
    { }
};

#endif
