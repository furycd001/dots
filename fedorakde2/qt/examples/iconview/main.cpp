/****************************************************************************
** $Id: qt/examples/iconview/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qiconview.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qpixmap.h>
#include <qiconset.h>

#include <qmime.h>
#include <stdio.h>

class ListenDND : public QObject
{
    Q_OBJECT

public:
    ListenDND( QWidget *w )
        : view( w )
    {}

public slots:
    void dropped( QDropEvent *mime ) {
        printf( "Dropped Mimesource %p into the view %p\n", mime, view );
        printf( "  Formats:\n" );
        int i = 0;
        const char *str = mime->format( i );
        printf( "    %s\n", str );
        while ( str ) {
            printf( "    %s\n", str );
            str = mime->format( ++i );
        }
    };
    void moved() {
        printf( "All selected items were moved to another widget\n" );
    }

protected:
    QWidget *view;

};

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QIconView qiconview;
    qiconview.setSelectionMode( QIconView::Extended );

    for ( unsigned int i = 0; i < 3000; i++ ) {
	QIconViewItem *item = new QIconViewItem( &qiconview, QString( "Item %1" ).arg( i + 1 ) );
	item->setRenameEnabled( TRUE );
    }

    qiconview.setCaption( "Qt Example - Iconview" );

    ListenDND listen_dnd( &qiconview );
    QObject::connect( &qiconview, SIGNAL( dropped( QDropEvent *, const QValueList<QIconDragItem> & ) ),
		      &listen_dnd, SLOT( dropped( QDropEvent * ) ) );
    QObject::connect( &qiconview, SIGNAL( moved() ), &listen_dnd, SLOT( moved() ) );

    a.setMainWidget( &qiconview );
    qiconview.show();
    qiconview.resize( qiconview.sizeHint() );

    return a.exec();
}

#include "main.moc"
