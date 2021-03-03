/****************************************************************************
** $Id: qt/examples/ftpclient/ftpview.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FTPVIEW_H
#define FTPVIEW_H

#include <qlistview.h>
#include <qvaluelist.h>
#include <qurlinfo.h>

class FtpViewItem : public QListViewItem
{
public:
    FtpViewItem( QListView *parent, const QUrlInfo &i );
    
    QString key( int c, bool ) const;
    QString text( int c ) const;
    const QPixmap* pixmap( int c ) const;
    
    QUrlInfo entryInfo() {
	return info;
    }
    
private:
    QUrlInfo info;
    
};

class FtpView : public QListView
{
    Q_OBJECT
    
public:
    FtpView( QWidget *parent );
    QValueList<QUrlInfo> selectedItems() const;
    
public slots:
    void slotInsertEntries( const QValueList<QUrlInfo> &info );
    
signals:
    void itemSelected( const QUrlInfo &info );
    
private slots:
    void slotSelected( QListViewItem *item );
    
};

#endif
