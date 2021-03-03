/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mydialog.h"

class MyDialogImpl : public MyDialog
{
public:
    MyDialogImpl();
    
public slots:
    void upClicked();
    
};
