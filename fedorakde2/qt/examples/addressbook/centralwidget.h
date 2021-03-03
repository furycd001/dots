/****************************************************************************
** $Id: qt/examples/addressbook/centralwidget.h   2.3.2   edited 2001-01-26 $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef AB_CENTRALWIDGET_H
#define AB_CENTRALWIDGET_H

#include <qwidget.h>
#include <qstring.h>

class QTabWidget;
class QListView;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QListViewItem;
class QCheckBox;

class ABCentralWidget : public QWidget
{
    Q_OBJECT

public:
    ABCentralWidget( QWidget *parent, const char *name = 0 );

    void save( const QString &filename );
    void load( const QString &filename );

protected slots:
    void addEntry();
    void changeEntry();
    void itemSelected( QListViewItem* );
    void selectionChanged();
    void toggleFirstName();
    void toggleLastName();
    void toggleAddress();
    void toggleEMail();
    void findEntries();

protected:
    void setupTabWidget();
    void setupListView();

    QGridLayout *mainGrid;
    QTabWidget *tabWidget;
    QListView *listView;
    QPushButton *add, *change, *find;
    QLineEdit *iFirstName, *iLastName, *iAddress, *iEMail,
        *sFirstName, *sLastName, *sAddress, *sEMail;
    QCheckBox *cFirstName, *cLastName, *cAddress, *cEMail;

};

#endif
