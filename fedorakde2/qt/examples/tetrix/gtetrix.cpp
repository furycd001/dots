/****************************************************************************
** $Id: qt/examples/tetrix/gtetrix.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "gtetrix.h"

#include <string.h>

GenericTetrix::GenericTetrix(int boardWidth,int boardHeight)
{
    int i,j;

    width    = boardWidth;
    height   = boardHeight;
    boardPtr = new int[height*width]; // Note the order, this makes it easier
                                      // to remove full lines.
    for(i = 0 ; i < height ; i++)
        for(j = 0 ; j < width ; j++)
            board(j,i) = 0;
    currentLine       = -1;           // -1 if no falling piece.
    currentPos        = 0;
    showNext          = 0;            // FALSE
    nLinesRemoved     = 0;
    nPiecesDropped    = 0;
    score             = 0;
    level	      = 1;
    gameID            = 0;
    nClearLines       = height;
}

GenericTetrix::~GenericTetrix()
{
    delete[] boardPtr;
}


void GenericTetrix::clearBoard(int fillRandomLines)
{
    int i,j;

    if (fillRandomLines >= height)
        fillRandomLines = height - 1;

    erasePiece();
    for(i = height - nClearLines - 1 ; i >= fillRandomLines ; i--)
        for(j = 0 ; j < width ; j++)
            if (board(j,i) != 0) {
                draw(j,i,0);
                board(j,i) = 0;
            }
    if (fillRandomLines != 0)
        for (i = 0 ; i < fillRandomLines ; i++) {
            fillRandom(i);
    }
    nClearLines = height - fillRandomLines;
}

void GenericTetrix::showBoard()
{
    int i,j;

    showPiece();
    for(i = height - nClearLines - 1 ; i >= 0 ; i--)
        for(j = 0 ; j < width ; j++)
            if (board(j,i) != 0)
                draw(j,i,board(j,i));
}

void GenericTetrix::hideBoard()
{
    int i,j;

    erasePiece();
    for(i = height - nClearLines - 1 ; i >= 0 ; i--)
        for(j = 0 ; j < width ; j++)
            if (board(j,i) != 0)
                draw(j,i,0);
}

void GenericTetrix::startGame(int gameType,int fillRandomLines)
{
    gameID             = gameType;
    clearBoard(fillRandomLines);
    nLinesRemoved      = 0;
    updateRemoved(nLinesRemoved);
    nClearLines        = height;
    nPiecesDropped     = 0;
    score              = 0;
    updateScore(score);
    level              = 1;
    updateLevel(level);
    newPiece();
}

void GenericTetrix::revealNextPiece(int revealIt)
{
    if (showNext == revealIt)
        return;
    showNext = revealIt;
    if (!showNext)
        eraseNextPiece();
    else
        showNextPiece();
}

void GenericTetrix::updateBoard(int x1,int y1,int x2, int y2,
                                int dontUpdateBlanks)
{
    int i,j;
    int tmp;

    if (x1 > x2) {
        tmp = x2;
        x2  = x1;
        x1  = tmp;
    }
    if (y1 > y2) {
        tmp = y2;
        y2  = y1;
        y1  = tmp;
    }
    if (x1 < 0)
        x1 = 0;
    if (x2 >= width)
        x2 = width - 1;
    if (y1 < 0)
        y1 = 0;
    if (y2 >= height)
        y2 = height - 1;
    for(i = y1 ; i <= y2 ; i++)
        for(j = x1 ; j <=  x2 ; j++)
	    if (!dontUpdateBlanks || board(j,height - i - 1) != 0)
                draw(j,height - i - 1,board(j,height - i - 1));
    showPiece();        // Remember to update piece correctly!!!!
}


void GenericTetrix::fillRandom(int line)
{
    int i,j;
    int holes;

    for(i = 0 ; i < width ; i++)
        board(i,line) = TetrixPiece::randomValue(7);
    holes = 0;
    for(i = 0 ; i < width ; i++)
        if (board(i,line) == 0)   // Count holes in the line.
            holes++;
    if (holes == 0)                // Full line, make a random hole:
        board(TetrixPiece::randomValue(width),line) = 0;
    if (holes == width)            // Empty line, make a random square:
        board(TetrixPiece::randomValue(width),line) = 
                                    TetrixPiece::randomValue(6) + 1;
    for(j = 0 ; j < width ; j++)
        draw(j,i,board(j,i));
}

void GenericTetrix::moveLeft(int steps)
{
    while(steps) {
        if (!canMoveTo(currentPos - 1,currentLine))
            return;
        moveTo(currentPos - 1,currentLine);
        steps--;
    }
}

void GenericTetrix::moveRight(int steps)
{
    while(steps) {
        if (!canMoveTo(currentPos + 1,currentLine))
            return;
        moveTo(currentPos + 1,currentLine);
        steps--;
    }
}

void GenericTetrix::rotateLeft()
{
    TetrixPiece tmp(currentPiece);

    tmp.rotateLeft();
    if (!canPosition(tmp))
        return;
    position(tmp);
    currentPiece = tmp;
}

void GenericTetrix::rotateRight()
{
    TetrixPiece tmp(currentPiece);

    tmp.rotateRight();
    if (!canPosition(tmp))
        return;
    position(tmp);
    currentPiece = tmp;
}

void GenericTetrix::dropDown()
{
    if (currentLine == -1)
        return;

    int dropHeight = 0;
    int newLine    = currentLine;
    while(newLine) {
        if (!canMoveTo(currentPos,newLine - 1))
            break;
        newLine--;
        dropHeight++;
    }
    if (dropHeight != 0)
        moveTo(currentPos,newLine);
    internalPieceDropped(dropHeight);
}

void GenericTetrix::oneLineDown()
{
    if (currentLine == -1)
        return;
    if (canMoveTo(currentPos,currentLine - 1)) {
        moveTo(currentPos,currentLine - 1);
    } else {
	internalPieceDropped(0);
    }
}

void GenericTetrix::newPiece()
{
    currentPiece = nextPiece;
    if (showNext)
        eraseNextPiece();
    nextPiece.setRandomType();
    if (showNext)
        showNextPiece();
    currentLine = height - 1 + currentPiece.getMinY();
    currentPos  = width/2 + 1;
    if (!canMoveTo(currentPos,currentLine)) {
	currentLine = -1;
        gameOver();
    } else {
        showPiece();
    }
}

void GenericTetrix::removePiece()
{
    erasePiece();
    currentLine = -1;
}

void GenericTetrix::drawNextSquare(int,int,int)
{

}

void GenericTetrix::pieceDropped(int)
{
    newPiece();
}

void GenericTetrix::updateRemoved(int) 
{
}

void GenericTetrix::updateScore(int)
{
}

void GenericTetrix::updateLevel(int)
{
}

void GenericTetrix::removeFullLines()
{
    int i,j,k;
    int nFullLines;
    
    for(i = 0 ; i < height - nClearLines ; i++) {
        for(j = 0 ; j < width ; j++)
            if (board(j,i) == 0)
                break;
        if (j == width) {
	    nFullLines = 1;
	    for(k = i + 1 ; k < height - nClearLines ; k++) {
                for(j = 0 ; j < width ; j++)
                    if (board(j,k) == 0)
		        break;
		if (j == width) {
		    nFullLines++;
		} else {
                    for(j = 0 ; j < width ; j++) {			
		        if (board(j,k - nFullLines) != board(j,k)) {
			    board(j,k - nFullLines) = board(j,k);
			    draw(      j,k - nFullLines,
			               board(j,k - nFullLines));
		        }
		    }
		}
	    }
	    nClearLines   = nClearLines + nFullLines;
	    nLinesRemoved = nLinesRemoved + nFullLines;
	    updateRemoved(nLinesRemoved);
	    score = score + 10*nFullLines; // updateScore must be
	                                   // called by caller!
	    for (i = height - nClearLines              ;
	         i < height - nClearLines + nFullLines ;
		 i++)
	        for(j = 0 ; j < width ; j++)
		    if (board(j,i) != 0) {
			draw(j,i,0);
			board(j,i) = 0;
		    }
	}
    }
}

void GenericTetrix::showPiece()
{
    int x,y;

    if (currentLine == -1)
        return;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoord(i,x,y);
        draw(currentPos + x,currentLine - y,currentPiece.getType());
    }
}

void GenericTetrix::erasePiece()
{
    int x,y;

    if (currentLine == -1)
        return;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoord(i,x,y);
        draw(currentPos + x,currentLine - y,0);
    }
}

void GenericTetrix::internalPieceDropped(int dropHeight)
{
    gluePiece();
    nPiecesDropped++;
    if (nPiecesDropped % 25 == 0) {
        level++;
	updateLevel(level);
    }
    score = score + 7 + dropHeight;
    removeFullLines();
    updateScore(score);
    pieceDropped(dropHeight);
}

void GenericTetrix::gluePiece()
{
    int x,y;
    int min;

    if (currentLine == -1)
        return;
    
    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoord(i,x,y);
        board(currentPos + x,currentLine - y) = currentPiece.getType();
    }
    min = currentPiece.getMinY();
    if (currentLine - min >= height - nClearLines)
        nClearLines = height - currentLine + min - 1;
}

void GenericTetrix::showNextPiece(int erase)
{
    int x,y;
    int minX = nextPiece.getMinX();
    int minY = nextPiece.getMinY();
    int maxX = nextPiece.getMaxX();
    int maxY = nextPiece.getMaxY();

    int xOffset = (3 - (maxX - minX))/2;
    int yOffset = (3 - (maxY - minY))/2;

    for(int i = 0 ; i < 4 ; i++) {
        nextPiece.getCoord(i,x,y);
	if (erase)
            drawNextSquare(x + xOffset - minX,
	                   y + yOffset - minY,0);
	else
            drawNextSquare(x + xOffset - minX,
	                   y + yOffset - minY,nextPiece.getType());
    }
}

int GenericTetrix::canPosition(TetrixPiece &piece)
{
    if (currentLine == -1)
        return 0;

    int x,y;

    for(int i = 0 ; i < 4 ; i++) {
        piece.getCoord(i,x,y);
        x = currentPos + x;
        y = currentLine - y; // Board and pieces have inverted y-coord. systems.
        if (x < 0 || x >= width || y < 0 || y >= height)
            return 0;     // Outside board, cannot put piece here.
        if (board(x,y) != 0)
            return 0;     // Over a non-zero square, cannot put piece here.
    }
    return 1;             // Inside board and no non-zero squares underneath.

}

int GenericTetrix::canMoveTo(int xPosition,int line)
{
    if (currentLine == -1)
        return 0;

    int x,y;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoord(i,x,y);
        x = xPosition + x;
        y = line - y;     // Board and pieces have inverted y-coord. systems.
        if (x < 0 || x >= width || y < 0 || y >= height)
            return 0;     // Outside board, cannot put piece here.
        if (board(x,y) != 0)
            return 0;     // Over a non-zero square, cannot put piece here.
    }
    return 1;             // Inside board and no non-zero squares underneath.
}

void GenericTetrix::moveTo(int xPosition,int line)
{
    if (currentLine == -1)
        return;
    optimizedMove(xPosition,line,currentPiece);
    currentPos  = xPosition;
    currentLine = line;
}

void GenericTetrix::position(TetrixPiece &piece)
{
    if (currentLine == -1)
        return;

    optimizedMove(currentPos,currentLine,piece);
}

void GenericTetrix::optimizedMove(int newPos, int newLine,
                                  TetrixPiece &newPiece)
{
    int updates [8][3];
    int nUpdates;
    int value;
    int x,y;
    int i,j;

    for(i = 0 ; i < 4 ; i++) { // Put the erasing coords into updates
        currentPiece.getCoord(i,x,y);
	updates[i][0] = currentPos  + x;
	updates[i][1] = currentLine - y;
	updates[i][2] = 0;
    }
    nUpdates = 4;
    for(i = 0 ; i < 4 ; i++) { // Any drawing coord same as an erasing one?
        newPiece.getCoord(i,x,y);
	x = newPos  + x;
	y = newLine - y;
	for (j = 0 ; j < 4 ; j++)
	    if (updates[j][0] == x && updates[j][1] == y) { // Same coord,
		                                        // don't have to erase
	        if (currentPiece.getType() == newPiece.getType())
	            updates[j][2] = -1; // Correct on screen, no update!
	        else
	            updates[j][2] = newPiece.getType();
		break;
	    }
	if (j == 4) {         // This coord does not overlap an erasing one
	    updates[nUpdates][0] = x;
	    updates[nUpdates][1] = y;
	    updates[nUpdates][2] = newPiece.getType();
	    nUpdates++;
	}
    }
    for (i = 0 ; i < nUpdates ; i++) {  // Do the updating
	x     = updates[i][0];
	y     = updates[i][1];
	value = updates[i][2];
	if (value != -1)                // Only update if new value != current
	    draw(x,y,value);
    }
}
