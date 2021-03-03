/****************************************************************************
** $Id: qt/src/dialogs/qprogressdialog.h   2.3.2   edited 2001-01-26 $
**
** Definition of QProgressDialog class
**
** Created : 970520
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

#ifndef QPROGRESSDIALOG_H
#define QPROGRESSDIALOG_H

#ifndef QT_H
#include "qsemimodal.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qprogressbar.h"
#endif // QT_H

#ifndef QT_NO_PROGRESSDIALOG

struct QProgressData;
class QTimer;

class Q_EXPORT QProgressDialog : public QSemiModal
{
    Q_OBJECT
    Q_PROPERTY( bool wasCancelled READ wasCancelled )
    Q_PROPERTY( int totalSteps READ totalSteps WRITE setTotalSteps )
    Q_PROPERTY( int progress READ progress WRITE setProgress )
    Q_PROPERTY( bool autoReset READ autoReset WRITE setAutoReset )
    Q_PROPERTY( bool autoClose READ autoClose WRITE setAutoClose )
    Q_PROPERTY( QString labelText READ labelText WRITE setLabelText )
	
public:
    QProgressDialog( QWidget *parent=0, const char *name=0, bool modal=FALSE,
		     WFlags f=0 );
    QProgressDialog( const QString& labelText, const QString &cancelButtonText,
		     int totalSteps, QWidget *parent=0, const char *name=0,
		     bool modal=FALSE, WFlags f=0 );
   ~QProgressDialog();

    void	setLabel( QLabel * );
    void	setCancelButton( QPushButton * );
    void	setBar( QProgressBar * );

    bool	wasCancelled() const;

    int		totalSteps() const;
    int		progress()   const;

    QSize	sizeHint() const;

    QString     labelText() const;

    void setAutoReset( bool b );
    bool autoReset() const;
    void setAutoClose( bool b );
    bool autoClose() const;

public slots:
    void	cancel();
    void	reset();
    void	setTotalSteps( int totalSteps );
    void	setProgress( int progress );
    void	setLabelText( const QString &);
    void	setCancelButtonText( const QString &);

    void	setMinimumDuration( int ms );
public:
    int		minimumDuration() const;

signals:
    void	cancelled();

protected:
    void	resizeEvent( QResizeEvent * );
    void	closeEvent( QCloseEvent * );
    void	styleChange( QStyle& );
    void	showEvent( QShowEvent *e );

protected slots:
    void	forceShow();
    
private:
    void	   init( QWidget *creator, const QString& lbl, const QString &canc,
		         int totstps);
    void	   center();
    void	   layout();
    QLabel	  *label()  const;
    QProgressBar  *bar()    const;
    QProgressData *d;
    QTimer	  *forceTimer;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QProgressDialog( const QProgressDialog & );
    QProgressDialog &operator=( const QProgressDialog & );
#endif
};

#endif // QT_NO_PROGRESSDIALOG

#endif // QPROGRESSDIALOG_H
