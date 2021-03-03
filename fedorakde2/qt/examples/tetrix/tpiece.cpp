/****************************************************************************
** $Id: qt/examples/tetrix/tpiece.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tpiece.h"
#include "qstring.h"
#include <stdlib.h>

void TetrixPiece::rotateLeft()
{
    if ( pieceType == 5 )    // don't rotate square piece type
        return;
    int tmp;
    for (int i = 0 ; i < 4 ; i++) {
        tmp = getXCoord(i);
        setXCoord(i,getYCoord(i));
        setYCoord(i,-tmp);
    }
}

void TetrixPiece::rotateRight()
{
    if ( pieceType == 5 )    // don't rotate square piece type
        return;
    int tmp;
    for (int i = 0 ; i < 4 ; i++) {
        tmp = getXCoord(i);
        setXCoord(i,-getYCoord(i));
        setYCoord(i,tmp);
    }
}

int TetrixPiece::getMinX()
{
    int tmp = coordinates[0][0];
    for(int i = 1 ; i < 4 ; i++)
        if (tmp > coordinates[i][0])
            tmp = coordinates[i][0];
    return tmp;
}

int TetrixPiece::getMaxX()
{
    int tmp = coordinates[0][0];
    for(int i = 1 ; i < 4 ; i++)
        if (tmp < coordinates[i][0])
            tmp = coordinates[i][0];
    return tmp;

}

int TetrixPiece::getMinY()
{
    int tmp = coordinates[0][1];
    for(int i = 1 ; i < 4 ; i++)
        if (tmp > coordinates[i][1])
            tmp = coordinates[i][1];
    return tmp;
}

int TetrixPiece::getMaxY()
{
    int tmp = coordinates[0][1];
    for(int i = 1 ; i < 4 ; i++)
        if (tmp < coordinates[i][1])
            tmp = coordinates[i][1];
    return tmp;
}

void TetrixPiece::initialize(int type)
{
    static int pieceTypes[7][4][2] = {{{ 0,-1},
                                       { 0, 0},
                                       {-1, 0},
                                       {-1, 1}},

                                      {{ 0,-1},
                                       { 0, 0},
                                       { 1, 0},
                                       { 1, 1}},

                                      {{ 0,-1},
                                       { 0, 0},
                                       { 0, 1},
                                       { 0, 2}},

                                      {{-1, 0},
                                       { 0, 0},
                                       { 1, 0},
                                       { 0, 1}},

                                      {{ 0, 0},
                                       { 1, 0},
                                       { 0, 1},
                                       { 1, 1}},

                                      {{-1,-1},
                                       { 0,-1},
                                       { 0, 0},
                                       { 0, 1}},

                                      {{ 1,-1},
                                       { 0,-1},
                                       { 0, 0},
                                       { 0, 1}}};
    if (type < 1 || type > 7)
        type = 1;
    pieceType = type;
    for(int i = 0 ; i < 4 ; i++) {
            coordinates[i][0] = pieceTypes[type - 1][i][0];
            coordinates[i][1] = pieceTypes[type - 1][i][1];
    }
}


/*
 *	Sigh, oh beautiful nostalgia! This random algorithm has
 *	been taken from the book "Adventures with your pocket calculator"
 *	and I used it in my first implemented and machine-
 *	run program of any size to speak of. Imagine how hungry I
 *	was after having programmed BASIC on paper for
 *	half a year?!!?!?!?!?!? The first program I typed in was a
 *	slot machine game and was made in BASIC on a SHARP
 *	PC-1211 with 1,47 KB RAM (one point four seven kilobytes) and
 *	a one-line LCD-display (I think it had 32 characters) in the
 *	year of our lord 1981. The man I had bought the machine from worked
 *	as a COBOL programmer and was amazed and impressed
 *	when I demonstrated the program 2 days after I had
 *	bought the machine, quote: "Gees, I have been looking so long
 *	for a "random" command in that BASIC, what is it called?"
 *	Oh, how I still get a thrill out of the thought of the
 *	explanation I then gave him...
 */

/*
 *	Sukk, aa vakre nostalgi! Denne random algoritmen er
 *	tatt fra boka "Adventures with your pocket calculator"
 *	og den brukte jeg i mitt foerste implementerte og maskin-
 *	kjoerte program av nevneverdig stoerrelse. Tror du jeg var
 *	noe sulten etter aa ha programmert BASIC paa papir i et
 *	halvt aar?!!?!?!?!?!? Programmet jeg tasta inn foerst var et
 *	"enarmet banditt" spill og ble laget i BASIC paa en SHARP
 *	PC-1211 med 1,47 KB RAM (en komma foertisju kilobyte) og
 *	et en-linjers LCD-display (tror det hadde 32 karakterer) i det
 *	herrens aar 1981. Mannen jeg kjoepte maskinen av jobbet til
 *	daglig med COBOL programmering og var forbloeffet og imponert
 *	da jeg demonstrerte programmet 2 dager etter at jeg hadde
 *	kjoept maskinen, sitat: "Joess, jeg som har leita saa lenge
 *	etter en random kommando i den BASICen, hva var det den
 *	het?" Aa, jeg frydes ennaa ved tanken paa forklaringen jeg
 *	deretter ga ham...
 */

double TetrixPiece::randomSeed = 0.33333;

void TetrixPiece::setRandomSeed(double seed)
{
    QCString buffer;
    if (seed < 0)
        seed = - seed;
    if (seed >= 1)
        seed = seed - (double) ((int) seed);
    buffer.sprintf("%1.5f",(float) seed);
    for (int i = 0 ; i < 5 ; i++)
        if ((buffer[i + 2] - '0') % 2 == 0)
	    buffer[i + 2]++;
    randomSeed = atof(buffer);
}

int TetrixPiece::randomValue(int maxPlusOne)
{
    randomSeed = randomSeed*147;
    randomSeed = randomSeed - (double) ((int) randomSeed);
    return (int) (randomSeed*maxPlusOne);
}
