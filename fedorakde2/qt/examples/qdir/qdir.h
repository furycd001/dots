/****************************************************************************
** $Id: qt/examples/qdir/qdir.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef QDIREXAMPLE_H
#define QDIREXAMPLE_H

#include <qscrollview.h>
#include <qfiledialog.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qurl.h>
#include <qpixmap.h>
#include <qstringlist.h>

class QMultiLineEdit;
class QTextView;
class DirectoryView;
class QSpinBox;
class QShowEvent;
class QPopupMenu;

class PixmapView : public QScrollView
{
    Q_OBJECT

public:
    PixmapView( QWidget *parent );
    void setPixmap( const QPixmap &pix );
    void drawContents( QPainter *p, int, int, int, int );

private:
    QPixmap pixmap;

};

class Preview : public QWidgetStack
{
    Q_OBJECT

public:
    Preview( QWidget *parent );
    void showPreview( const QUrl &u, int size );

private:
    QMultiLineEdit *normalText;
    QTextView *html;
    PixmapView *pixmap;

};

class PreviewWidget : public QVBox,
		      public QFilePreview
{
    Q_OBJECT

public:
    PreviewWidget( QWidget *parent );
    void previewUrl( const QUrl &u );

private:
    QSpinBox *sizeSpinBox;
    Preview *preview;

};

class CustomFileDialog : public QFileDialog
{
    Q_OBJECT

public:
    CustomFileDialog();
    ~CustomFileDialog();

protected:
    void showEvent( QShowEvent *e );

public slots:
    void setDir2( const QString & );

private slots:
    void bookmarkChosen( int i );
    void goHome();

private:
    DirectoryView *dirView;
    QPopupMenu *bookmarkMenu;
    QStringList bookmarkList;
    int addId;

};

#endif
