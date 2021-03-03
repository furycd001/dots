/****************************************************************************
** $Id: qt/src/dialogs/qmessagebox.h   2.3.2   edited 2001-01-26 $
**
** Definition of QMessageBox class
**
** Created : 950503
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

#ifndef QMESSAGEBOX_H
#define QMESSAGEBOX_H

#ifndef QT_H
#include "qdialog.h"
#endif // QT_H

#ifndef QT_NO_MESSAGEBOX

class  QLabel;
class  QPushButton;
struct QMBData;

class Q_EXPORT QMessageBox : public QDialog
{
    Q_OBJECT
    Q_ENUMS( Icon )
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( Icon icon READ icon WRITE setIcon )
    Q_PROPERTY( QPixmap iconPixmap READ iconPixmap WRITE setIconPixmap )
    Q_PROPERTY( TextFormat textFormat READ textFormat WRITE setTextFormat )

public:
    enum Icon { NoIcon = 0, Information = 1, Warning = 2, Critical = 3 };

    QMessageBox( QWidget *parent=0, const char *name=0 );
    QMessageBox( const QString& caption, const QString &text, Icon icon,
		 int button0, int button1, int button2,
		 QWidget *parent=0, const char *name=0, bool modal=TRUE,
		 WFlags f=WStyle_DialogBorder  );
   ~QMessageBox();

    enum { NoButton = 0, Ok = 1, Cancel = 2, Yes = 3, No = 4, Abort = 5,
	   Retry = 6, Ignore = 7, ButtonMask = 0x07,
	   Default = 0x100, Escape = 0x200, FlagMask = 0x300 };

    static int information( QWidget *parent, const QString &caption,
			    const QString& text,
			    int button0, int button1=0, int button2=0 );
    static int information( QWidget *parent, const QString &caption,
			    const QString& text,
			    const QString& button0Text = QString::null,
			    const QString& button1Text = QString::null,
			    const QString& button2Text = QString::null,
			    int defaultButtonNumber = 0,
			    int escapeButtonNumber = -1 );

    static int warning( QWidget *parent, const QString &caption,
			const QString& text,
			int button0, int button1, int button2=0 );
    static int warning( QWidget *parent, const QString &caption,
			const QString& text,
			const QString& button0Text = QString::null,
			const QString& button1Text = QString::null,
			const QString& button2Text = QString::null,
			int defaultButtonNumber = 0,
			int escapeButtonNumber = -1 );

    static int critical( QWidget *parent, const QString &caption,
			 const QString& text,
			 int button0, int button1, int button2=0 );
    static int critical( QWidget *parent, const QString &caption,
			 const QString& text,
			 const QString& button0Text = QString::null,
			 const QString& button1Text = QString::null,
			 const QString& button2Text = QString::null,
			 int defaultButtonNumber = 0,
			 int escapeButtonNumber = -1 );

    static void about( QWidget *parent, const QString &caption,
		       const QString& text );

    static void aboutQt( QWidget *parent,
			 const QString& caption=QString::null );

#if 1 /* OBSOLETE */
    static int message( const QString &caption,
			const QString& text, 
			const QString& buttonText=QString::null,
			QWidget *parent=0, const char *name=0 );

    static bool query( const QString &caption,
		       const QString& text, 
		       const QString& yesButtonText=QString::null,
		       const QString& noButtonText=QString::null,
		       QWidget *parent=0, const char *name=0 );
#endif

    QString	text() const;
    void	setText( const QString &);

    Icon	icon() const;

    // ### the next four functions will probably be renamed in 3.0.
    void	setIcon( Icon );
    void	setIcon( const QPixmap & );

    const QPixmap *iconPixmap() const;
    void	setIconPixmap( const QPixmap & );

    QString	buttonText( int button ) const;
    void	setButtonText( int button, const QString &);

    void	adjustSize();

    static QPixmap standardIcon( Icon icon, GUIStyle style );

    TextFormat textFormat() const;
    void 	 setTextFormat( TextFormat );

protected:
    void	resizeEvent( QResizeEvent * );
    void	keyPressEvent( QKeyEvent * );
    void	styleChanged( QStyle& );

private slots:
    void	buttonClicked();

private:
    void	init( int, int, int );
    int		indexOf( int ) const;
    void	resizeButtons();
    QLabel     *label;
    QMBData    *mbd;
    void       *reserved1;
    void       *reserved2;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QMessageBox( const QMessageBox & );
    QMessageBox &operator=( const QMessageBox & );
#endif
};

#endif // QT_NO_MESSAGEBOX

#endif // QMESSAGEBOX_H
