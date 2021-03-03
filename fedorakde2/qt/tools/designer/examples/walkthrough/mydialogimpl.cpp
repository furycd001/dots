/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mydialogimpl.h"
#include <qmessagebox.h>

MyDialogImpl::MyDialogImpl()
    : MyDialog()
{
}

void MyDialogImpl::upClicked()
{
    QMessageBox::information( this, tr( "Message" ), tr( "Item-Up clicked!" ) );
}
