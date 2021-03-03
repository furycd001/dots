/****************************************************************************
** $Id: qt/examples/tetrix/tpiece.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TPIECE_H
#define TPIECE_H

class TetrixPiece
{
public:
    TetrixPiece()                        {setRandomType();}
    TetrixPiece(int type)                {initialize(type % 7 + 1);}

    void setRandomType()                 {initialize(randomValue(7) + 1);}

    void rotateLeft();
    void rotateRight();

    int  getType()                       {return pieceType;}
    int  getXCoord(int index)            {return coordinates[index][0];}
    int  getYCoord(int index)            {return coordinates[index][1];}
    void getCoord(int index,int &x,int&y){x = coordinates[index][0];
                                          y = coordinates[index][1];}
    int  getMinX();
    int  getMaxX();
    int  getMinY();
    int  getMaxY();

    static void   setRandomSeed(double seed);
    static int    randomValue(int maxPlusOne);

private:
    void setXCoord(int index,int value)  {coordinates[index][0] = value;}
    void setYCoord(int index,int value)  {coordinates[index][1] = value;}
    void setCoords(int index,int x,int y){coordinates[index][0] = x;
                                          coordinates[index][1] = y;}
    void initialize(int type);

    int  pieceType;
    int  coordinates[4][2];

    static double randomSeed;
};

#endif
