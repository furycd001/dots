/****************************************************************************
** $Id: qt/src/widgets/qwidgetstack.h   2.3.2   edited 2001-01-26 $
**
** Definition of QWidgetStack class
**
** Created : 980306
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

#ifndef QWIDGETSTACK_H
#define QWIDGETSTACK_H

#ifndef QT_H
#include "qframe.h"
#include "qintdict.h"
#include "qptrdict.h"
#endif // QT_H

#ifndef QT_NO_WIDGETSTACK


class QWidgetStackPrivate;


class Q_EXPORT QWidgetStack: public QFrame
{
    Q_OBJECT
public:
    QWidgetStack( QWidget * parent = 0, const char *name = 0 );
    ~QWidgetStack();

    void addWidget( QWidget *, int );
    void removeWidget( QWidget * );

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void show();

    QWidget * widget( int ) const;
    int id( QWidget * ) const;

    QWidget * visibleWidget() const;

    void setFrameRect( const QRect & );

signals:
    void aboutToShow( int );
    void aboutToShow( QWidget * );

public slots:
    void raiseWidget( int );
    void raiseWidget( QWidget * );

protected:
    void frameChanged();
    void resizeEvent( QResizeEvent * );

    virtual void setChildGeometries();
    void childEvent( QChildEvent * );

private:
    bool isMyChild( QWidget * );

    QWidgetStackPrivate * d;
    QIntDict<QWidget> * dict;
    QPtrDict<QWidget> * focusWidgets;
    QWidget * topWidget;
    QWidget * invisible;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QWidgetStack( const QWidgetStack & );
    QWidgetStack& operator=( const QWidgetStack & );
#endif
};

#endif // QT_NO_WIDGETSTACK

#endif // QWIDGETSTACK_H
