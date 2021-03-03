/****************************************************************************
** $Id: qt/examples/showimg/imagefip.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "imagefip.h"
#include <qimage.h>

/* XPM */
static const char *image_xpm[] = {
"17 15 9 1",
" 	c #7F7F7F",
".	c #FFFFFF",
"X	c #00B6FF",
"o	c #BFBFBF",
"O	c #FF6C00",
"+	c #000000",
"@	c #0000FF",
"#	c #6CFF00",
"$	c #FFB691",
"             ..XX",
" ........o   .XXX",
" .OOOOOOOo.  XXX+",
" .O@@@@@@+++XXX++",
" .O@@@@@@O.XXX+++",
" .O@@@@@@OXXX+++.",
" .O######XXX++...",
" .O#####XXX++....",
" .O##$#$XX+o+....",
" .O#$$$$$+.o+....",
" .O##$$##O.o+....",
" .OOOOOOOO.o+....",
" ..........o+....",
" ooooooooooo+....",
"+++++++++++++...."
};

ImageIconProvider::ImageIconProvider( QWidget *parent, const char *name ) :
    QFileIconProvider( parent, name ),
    imagepm(image_xpm)
{
    fmts = QImage::inputFormats();
}

ImageIconProvider::~ImageIconProvider()
{
}

const QPixmap * ImageIconProvider::pixmap( const QFileInfo &fi )
{
    QString ext = fi.extension().upper();
    if ( fmts.contains(ext) ) {
	return &imagepm;
    } else {
	return QFileIconProvider::pixmap(fi);
    }
}
