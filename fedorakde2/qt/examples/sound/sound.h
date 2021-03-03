/****************************************************************************
** $Id: qt/examples/sound/sound.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#ifndef PLAY_H
#define PLAY_H

#include "qsound.h"
#include <qapp.h>
#include <qmainwindow.h>

class SoundPlayer : public QMainWindow {
    Q_OBJECT
public:
    SoundPlayer();

public slots:
    void doPlay1();
    void doPlay2();
    void doPlay3();
    void doPlay4();
    void doPlay34();
    void doPlay1234();

private:
    QSound bucket3;
    QSound bucket4;
};

#endif
