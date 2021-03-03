/****************************************************************************
** $Id: qt/src/dialogs/qfontdialog.h   2.3.2   edited 2001-01-26 $
**
** Definition of QFontDialog
**
** Created : 970605
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
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

#ifndef QFONTDIALOG_H
#define QFONTDIALOG_H

#include "qwindowdefs.h"

#ifndef QT_NO_FONTDIALOG

//
//  W A R N I N G
//  -------------
//
//  This class is under development and has private constructors.
//
//  You may use the public static getFont() functions which are guaranteed
//  to be available in the future.
//

#ifndef QT_H
#include "qdialog.h"
#include "qfont.h"
#endif // QT_H

class  QListBox;
class  QComboBox;
class QFontDialogPrivate;


class Q_EXPORT QFontDialog: public QDialog
{
    Q_OBJECT

public:
    static QFont getFont( bool *ok, const QFont &def,
    			  QWidget *parent = 0, const char* name = 0);

    static QFont getFont( bool *ok, QWidget *parent = 0, const char* name = 0);

private:
    QFontDialog( QWidget *parent=0, const char *name=0, bool modal=FALSE,
		 WFlags f=0 );
   ~QFontDialog();

    QFont font() const;
    void setFont( const QFont &font );

signals:
    void fontSelected( const QFont &font );
    void fontHighlighted( const QFont &font );

protected:
    bool eventFilter( QObject *, QEvent * );

    QListBox * familyListBox() const;
    virtual void updateFamilies();

    QListBox * styleListBox() const;
    virtual void updateStyles();

    QListBox * sizeListBox() const;
    virtual void updateSizes();

    QComboBox * scriptCombo() const;
    virtual void updateScripts();

#if 0
    QString family() const;
    QString script() const;
    QString style() const;
    QString size() const;
#endif

protected slots:
    void sizeChanged( const QString &);

private slots:
    void familyHighlighted( const QString &);
    void familyHighlighted( int );
    void scriptHighlighted( const QString &);
    void scriptHighlighted( int );
    void styleHighlighted( const QString &);
    void sizeHighlighted( const QString &);
    void updateSample();
    void emitSelectedFont();

private:
    static QFont getFont( bool *ok, const QFont *def,
			  QWidget *parent = 0, const char* name = 0);

    QFontDialogPrivate * d;
    friend class QFontDialogPrivate;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QFontDialog( const QFontDialog & );
    QFontDialog& operator=( const QFontDialog & );
#endif
};

#endif

#endif // QFONTDIALOG_H
