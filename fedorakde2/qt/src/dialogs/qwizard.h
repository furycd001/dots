/****************************************************************************
** $Id: qt/src/dialogs/qwizard.h   2.3.2   edited 2001-01-26 $
**
** Definition of the QWizard class.
**
** Created : 990101
**
** Copyright (C) 1999 by Trolltech AS.  All rights reserved.
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

#ifndef QWIZARDDIALOG_H
#define QWIZARDDIALOG_H


#ifndef QT_H
#include "qdialog.h"
#endif // QT_H

#ifndef QT_NO_WIZARD

class QWizardPrivate;
class QWizardPagePrivate;
class QWizard;
class QHBoxLayout;

class Q_EXPORT QWizard : public QDialog
{
    Q_OBJECT
public:
    QWizard( QWidget *parent=0, const char *name=0, bool modal=FALSE,
	     WFlags f=0 );
    ~QWizard();

    void show();

    void setFont( const QFont & font );

    virtual void addPage( QWidget *, const QString & );
    virtual void removePage( QWidget * );

    QString title( QWidget * ) const;
    void setTitle( QWidget *, const QString & );
    
    virtual void showPage( QWidget * );

    QWidget * currentPage() const;

    QWidget* page( int pos ) const;
    int pageCount() const;

    virtual bool appropriate( QWidget * ) const;
    virtual void setAppropriate( QWidget *, bool );

    QPushButton * backButton() const;
    QPushButton * nextButton() const;
    QPushButton * finishButton() const;
    QPushButton * cancelButton() const;
    QPushButton * helpButton() const;

    bool eventFilter( QObject *, QEvent * );

public slots:
    virtual void setBackEnabled( QWidget *, bool );
    virtual void setNextEnabled( QWidget *, bool );
    virtual void setFinishEnabled( QWidget *, bool );

    virtual void setHelpEnabled( QWidget *, bool );

    virtual void setFinish(  QWidget *, bool );

protected slots:
    virtual void back();
    virtual void next();
    virtual void help();

signals:
    void helpClicked();
    void selected( const QString& );

protected:
    virtual void layOutButtonRow( QHBoxLayout * );
    virtual void layOutTitleRow( QHBoxLayout *, const QString & );

private:
    void setBackEnabled( bool );
    void setNextEnabled( bool );

    void setHelpEnabled( bool );

    void setNextPage( QWidget * );

    void updateButtons();

    void layOut();
    QWizardPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QWizard( const QWizard & );
    QWizard& operator=( const QWizard & );
#endif
};

#endif // QT_NO_WIZARD

#endif // QWIZARD_H
