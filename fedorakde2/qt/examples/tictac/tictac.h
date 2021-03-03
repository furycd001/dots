/****************************************************************************
** $Id: qt/examples/tictac/tictac.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TICTAC_H
#define TICTAC_H


#include <qpushbutton.h>
#include <qvector.h>

class QComboBox;
class QLabel;


// --------------------------------------------------------------------------
// TicTacButton implements a single tic-tac-toe button
//

class TicTacButton : public QPushButton
{
    Q_OBJECT
public:
    TicTacButton( QWidget *parent );
    enum Type { Blank, Circle, Cross };
    Type	type() const		{ return t; }
    void	setType( Type type )	{ t = type; repaint(); }
    QSizePolicy sizePolicy() const
    { return QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ); }
    QSize sizeHint() const { return QSize( 32, 32 ); }
    QSize minimumSizeHint() const { return QSize( 10, 10 ); }
protected:
    void	drawButtonLabel( QPainter * );
private:
    Type t;
};

// Using template vector to make vector-class of TicTacButton.
// This vector is used by the TicTacGameBoard class defined below.

typedef QVector<TicTacButton>	TicTacButtons;
typedef QArray<int>		TicTacArray;


// --------------------------------------------------------------------------
// TicTacGameBoard implements the tic-tac-toe game board.
// TicTacGameBoard is a composite widget that contains N x N TicTacButtons.
// N is specified in the constructor.
//

class TicTacGameBoard : public QWidget
{
    Q_OBJECT
public:
    TicTacGameBoard( int n, QWidget *parent=0, const char *name=0 );
   ~TicTacGameBoard();
    enum	State { Init, HumansTurn, HumanWon, ComputerWon, NobodyWon };
    State	state() const		{ return st; }
    void	computerStarts( bool v );
    void        newGame();
signals:
    void	finished();			// game finished
private slots:
    void	buttonClicked();
private:
    void        setState( State state ) { st = state; }
    void	updateButtons();
    int  	checkBoard( TicTacArray * );
    void 	computerMove();
    State	st;
    int		nBoard;
    bool	comp_starts;
    TicTacArray *btArray;
    TicTacButtons *buttons;
};


// --------------------------------------------------------------------------
// TicTacToe implements the complete game.
// TicTacToe is a composite widget that contains a TicTacGameBoard and
// two push buttons for starting the game and quitting.
//

class TicTacToe : public QWidget
{
    Q_OBJECT
public:
    TicTacToe( int boardSize=3, QWidget *parent=0, const char *name=0 );
private slots:
    void	newGameClicked();
    void	gameOver();
private:
    void	newState();
    QComboBox	*whoStarts;
    QPushButton *newGame;
    QPushButton *quit;
    QLabel	*message;
    TicTacGameBoard *board;
};


#endif // TICTAC_H
