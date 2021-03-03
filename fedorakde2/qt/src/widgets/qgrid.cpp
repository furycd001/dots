/****************************************************************************
** $Id: qt/src/widgets/qgrid.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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


#include "qgrid.h"
#ifndef QT_NO_GRID
#include "qlayout.h"
#include "qapplication.h"

// NOT REVISED
/*!
  \class QGrid qgrid.h
  \brief The QGrid widget performs geometry management on its children

  \ingroup geomanagement

  The number of rows or columns is defined in the constructor. All its
  children will be placed and sized according to their sizeHint() and
  sizePolicy() return values.

  Use setMargin() to add space around the edge, and use setSpacing() to
  add space between the widgets.

  <img src=qgrid-m.png>

  For more general control over grid layout, including multi-column and
  multi-row widgets, use the QGridLayout class directly.

  \sa QVBox and QHBox
 */



/*!
  Constructs a grid widget with parent \a parent and name \a name. If \a dir is
  \c Horizontal, \a n specifies the number of columns. If \a dir is \c Vertical,
  \a n specifies the number of rows.
 */
QGrid::QGrid( int n, Direction dir, QWidget *parent, const char *name, WFlags f )
    :QFrame( parent, name, f )
{
    int nCols, nRows;
    if ( dir == Horizontal ) {
	nCols = n;
	nRows = -1;
    } else {
	nCols = -1;
	nRows = n;
    }
    lay = new QGridLayout( this, nRows, nCols, 0, 0, name );
    lay->setAutoAdd( TRUE );
}



/*!
  Constructs a grid widget with parent \a parent and name \a name.
  \a n specifies the number of columns.
 */
QGrid::QGrid( int n, QWidget *parent, const char *name, WFlags f )
    :QFrame( parent, name, f )
{
    lay = new QGridLayout( this, -1, n, 0, 0, name );
    lay->setAutoAdd( TRUE );
}


/*!
  Sets the spacing between children to \a space.
*/

void QGrid::setSpacing( int space )
{
    if ( layout() )
	layout()->setSpacing( space );
}


/*!\reimp
 */
void QGrid::frameChanged()
{
    if ( !layout() )
	return;
    layout()->setMargin( frameWidth() );
}


/*!
  \reimp
*/

QSize QGrid::sizeHint() const
{
    QWidget *mThis = (QWidget*)this;
    QApplication::sendPostedEvents( mThis, QEvent::ChildInserted );
    return QFrame::sizeHint();
}
#endif
