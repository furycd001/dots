//-----------------------------------------------------------------------------
//
// Screen savers for KDE
//
// Copyright (c)  Martin R. Jones 1999
//

#ifndef __DEMOWIN_H__
#define __DEMOWIN_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kapp.h>

//----------------------------------------------------------------------------

class DemoWindow : public QWidget
{
    Q_OBJECT
public:
    DemoWindow() : QWidget()
    {
        setFixedSize(600, 420);
    }

protected:
    virtual void keyPressEvent(QKeyEvent *e)
    {
        if (e->ascii() == 'q')
        {
            kapp->quit();
        }
    }
};

#endif

