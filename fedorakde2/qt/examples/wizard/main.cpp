/****************************************************************************
** $Id: qt/examples/wizard/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "wizard.h"
#include <qapplication.h>

int main(int argc,char **argv)
{
    QApplication a(argc,argv);

    Wizard wizard;
    wizard.setCaption("Qt Example - Wizard");
    return wizard.exec();
}
