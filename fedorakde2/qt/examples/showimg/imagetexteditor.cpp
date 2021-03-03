/****************************************************************************
** $Id: qt/examples/showimg/imagetexteditor.cpp   2.3.2   edited 2001-02-16 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "imagetexteditor.h"
#include <qimage.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcombobox.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>


ImageTextEditor::ImageTextEditor( QImage& i, QWidget *parent, const char *name, WFlags f ) :
    QDialog(parent,name,TRUE,f),
    image(i)
{
    QVBoxLayout* vbox = new QVBoxLayout(this,8);
    vbox->setAutoAdd(TRUE);

    QGrid* controls = new QGrid(3,QGrid::Horizontal,this);
    controls->setSpacing(8);
    QLabel* l;
    l=new QLabel("Language",controls); l->setAlignment(AlignCenter);
    l=new QLabel("Key",controls); l->setAlignment(AlignCenter);
    (void)new QLabel("",controls); // dummy
    languages = new QComboBox(controls);
    keys = new QComboBox(controls);
    QPushButton* remove = new QPushButton("Remove",controls);

    newlang = new QLineEdit(controls);
    newkey = new QLineEdit(controls);
    QPushButton* add = new QPushButton("Add",controls);

    text = new QMultiLineEdit(this);

    QHBox* hbox = new QHBox(this);
    QPushButton* cancel = new QPushButton("Cancel",hbox);
    QPushButton* ok = new QPushButton("OK",hbox);

    connect(add,SIGNAL(clicked()),
	this,SLOT(addText()));

    connect(remove,SIGNAL(clicked()),
	this,SLOT(removeText()));

    connect(ok,SIGNAL(clicked()),
	this,SLOT(accept()));

    connect(cancel,SIGNAL(clicked()),
	this,SLOT(reject()));

    connect(languages,SIGNAL(activated(int)),
	this,SLOT(updateText()));

    connect(keys,SIGNAL(activated(int)),
	this,SLOT(updateText()));

    imageChanged();
}

ImageTextEditor::~ImageTextEditor()
{
}

void ImageTextEditor::imageChanged()
{
    languages->clear();
    keys->clear();
    text->clear();
    languages->insertItem("<any>");

    languages->insertStringList(image.textLanguages());
    keys->insertStringList(image.textKeys());

    updateText();
}

void ImageTextEditor::accept()
{
    storeText();
    QDialog::accept();
}

void ImageTextEditor::updateText()
{
    storeText();
    newlang->setText(languages->currentText());
    newkey->setText(keys->currentText());
    QString t = image.text(currKey(),currLang());

    text->setText(t);
}

QString ImageTextEditor::currKey()
{
    return newkey->text();
}

QString ImageTextEditor::currLang()
{
    QString l = newlang->text();
    if ( l=="<any>" )
	l = QString::null;
    return l;
}

QString ImageTextEditor::currText()
{
    QString t = text->text();
    if ( t.isNull() ) t = "";
    return t;
}


void ImageTextEditor::removeText()
{
    image.setText(currKey(),currLang(),QString::null);
}

void ImageTextEditor::addText()
{
    storeText();
}

void ImageTextEditor::storeText()
{
    if ( currKey().length() > 0 ) {
	image.setText(currKey(),currLang(),currText());
    }
}
