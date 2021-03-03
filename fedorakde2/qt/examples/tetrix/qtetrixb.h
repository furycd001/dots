/****************************************************************************
** $Id: qt/examples/tetrix/qtetrixb.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef QTETRIXB_H
#define QTETRIXB_H

#include "gtetrix.h"
#include <qframe.h>

class QTimer;

class QTetrixBoard : public QFrame, public GenericTetrix
{
    Q_OBJECT
public:
    QTetrixBoard( QWidget *parent=0, const char *name=0 );

    void      gameOver();
    void      startGame(int gameType = 0,int fillRandomLines = 0);

public slots:
    void      timeout();
    void      updateNext()	{ GenericTetrix::updateNext(); }
    void      key(QKeyEvent *e) { keyPressEvent(e); }
    void      start()		{ startGame(); }
    void      pause();

signals:
    void      gameOverSignal();
    void      drawNextSquareSignal(int x,int y,QColor *color1);
    void      updateRemovedSignal(int noOfLines);
    void      updateScoreSignal(int score);
    void      updateLevelSignal(int level);

public:       // until we have keyboard focus, should be protected
    void      keyPressEvent( QKeyEvent * );

private:
    void      drawContents( QPainter * );
    void      resizeEvent( QResizeEvent * );
    void      drawSquare(int x,int y,int value);
    void      drawNextSquare(int x,int y,int value);
    void      updateRemoved(int noOfLines);
    void      updateScore(int newScore);
    void      updateLevel(int newLlevel);
    void      pieceDropped(int dropHeight);
    void      updateTimeoutTime();

    QTimer   *timer;

    int       xOffset,yOffset;
    int       blockWidth,blockHeight;
    int       timeoutTime;
    bool      noGame;
    bool      isPaused;
    bool      waitingAfterLine;

    QColor    colors[7];
    QPainter *paint;
};

#endif
