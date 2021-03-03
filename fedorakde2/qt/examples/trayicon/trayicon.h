/****************************************************************************
** $Id: qt/examples/trayicon/trayicon.h   2.3.2   edited 2001-07-12 $
**
** Definition of QTrayIcon class
**
** Created : 120201
**
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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

#ifndef TRAYICON_H
#define TRAYICON_H

#ifndef QT_H
#include <qobject.h>
#include <qpixmap.h>
#endif // QT_H

class QPopupMenu;

class TrayIcon : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString toolTip READ toolTip WRITE setToolTip )
    Q_PROPERTY( QPixmap icon READ icon WRITE setIcon )

public:
    TrayIcon( QObject *parent = 0, const char *name = 0 );
    TrayIcon( const QPixmap &, const QString &, QPopupMenu *popup = 0, QObject *parent = 0, const char *name = 0 );
    ~TrayIcon();

    // Set a popup menu to handle RMB
    void		setPopup( QPopupMenu * );
    QPopupMenu*		popup() const;

    QPixmap		icon() const;
    QString		toolTip() const;

public slots:
    void		setIcon( const QPixmap &icon );
    void		setToolTip( const QString &tip );

    void		show();
    void		hide();

signals:
    void		clicked( const QPoint& );
    void		doubleClicked( const QPoint& );

protected:
    bool		event( QEvent * );
    virtual void	mouseMoveEvent( QMouseEvent *e );
    virtual void	mousePressEvent( QMouseEvent *e );
    virtual void	mouseReleaseEvent( QMouseEvent *e );
    virtual void	mouseDoubleClickEvent( QMouseEvent *e );

private:
    QPopupMenu *pop;
    QPixmap pm;
    QString tip;

    // system-dependant part
    class TrayIconPrivate;
    TrayIconPrivate *d;
    void sysInstall();
    void sysRemove();
    void sysUpdateIcon();
    void sysUpdateToolTip();
};

#endif //TRAYICON_H
