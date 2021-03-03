/****************************************************************************
** $Id: qt/examples/showimg/imagetexteditor.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IMAGETEXTEDITOR_H
#define IMAGETEXTEDITOR_H

#include <qdialog.h>

class QImage;
class QComboBox;
class QListBox;
class QLineEdit;
class QMultiLineEdit;

class ImageTextEditor : public QDialog
{
    Q_OBJECT
public:
    ImageTextEditor( QImage& i, QWidget *parent=0, const char *name=0, WFlags f=0 );
    ~ImageTextEditor();
    void accept();
public slots:
    void imageChanged();
    void updateText();
    void addText();
    void removeText();
private:
    void storeText();
    QImage& image;
    QComboBox* languages;
    QComboBox* keys;
    QMultiLineEdit* text;
    QLineEdit* newlang;
    QLineEdit* newkey;
    QString currKey();
    QString currLang();
    QString currText();
};

#endif // IMAGETEXTEDITOR_H
