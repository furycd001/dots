/****************************************************************************
** $Id: qt/examples/tetrix/gtetrix.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef GTETRIX_H
#define GTETRIX_H

#include "tpiece.h"


class GenericTetrix
{
public:
    GenericTetrix(int boardWidth = 10,int boardHeight = 22);
    virtual ~GenericTetrix();

    void clearBoard(int fillRandomLines = 0);
    void revealNextPiece(int revealIt);
    void updateBoard(int x1,int y1,int x2,int y2,int dontUpdateBlanks = 0);
    void updateNext(){if (showNext) showNextPiece();}
    void hideBoard();
    void showBoard();
    void fillRandom(int line);

    void moveLeft(int steps = 1);
    void moveRight(int steps = 1);
    void rotateLeft();
    void rotateRight();
    void dropDown();
    void oneLineDown();
    void newPiece();
    void removePiece();

    int  noOfClearLines()                     {return nClearLines;}
    int  getLinesRemoved()                    {return nLinesRemoved;}
    int  getPiecesDropped()                   {return nPiecesDropped;}
    int  getScore()                           {return score;}
    int  getLevel()                           {return level;}
    int  boardHeight()                        {return height;}
    int  boardWidth()                         {return width;}

    virtual void drawSquare(int x,int y,int value) = 0;
    virtual void gameOver() = 0;

    virtual void startGame(int gameType = 0,int fillRandomLines = 0);
    virtual void drawNextSquare(int x,int y,int value);
    virtual void pieceDropped(int dropHeight);
    virtual void updateRemoved(int noOfLines);
    virtual void updateScore(int newScore);
    virtual void updateLevel(int newLevel);

private:
    void  draw(int x, int y, int value){drawSquare(x,height - y,value);}
    void  removeFullLines();
    void  removeLine(int line);
    void  showPiece();
    void  erasePiece();
    void  internalPieceDropped(int dropHeight);
    void  gluePiece();
    void  showNextPiece(int erase = 0);
    void  eraseNextPiece(){showNextPiece(1);};
    int   canPosition(TetrixPiece &piece);    // Returns a boolean value.
    int   canMoveTo(int xPosition, int line); // Returns a boolean value.
    void  moveTo(int xPosition,int line);
    void  position(TetrixPiece &piece);
    void  optimizedMove(int newPos, int newLine,TetrixPiece &newPiece);
    
    int  &board(int x,int y){return boardPtr[width*y + x];}

    TetrixPiece currentPiece;
    TetrixPiece nextPiece;
    int         currentLine;
    int         currentPos;
    int         showNext;                    // Boolean variable.
    int         nLinesRemoved;
    int         nPiecesDropped;
    int         score;
    int         level;
    int         gameID;
    int         nClearLines;
    int         width;
    int         height;
    int         *boardPtr;
};


#endif
