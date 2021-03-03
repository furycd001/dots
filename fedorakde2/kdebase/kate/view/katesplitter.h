/***************************************************************************
                          katesplitter.h  -  description
                             -------------------
    begin                : Fri Mar 02 2001
    copyright            : (C) 2001 by Anders Lund, anders@alweb.dk
    email                : anders@alweb.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _KANTSPLITTER_H_
#define _KANTSPLITTER_H_

#include <qsplitter.h>


/** This class is needed because QSplitter cant return an index for a widget. */
class KateSplitter : public QSplitter
{
  Q_OBJECT

  public:
    KateSplitter(QWidget* parent=0, const char* name=0);
    KateSplitter(Orientation o, QWidget* parent=0, const char* name=0);
    ~KateSplitter();

    /** Since there is supposed to be only 2 childs of a katesplitter,
     * any child other than the last is the first.
     * This method uses QSplitter::idAfter(widget) which
     * returns 0 if there is no widget after this one.
     * This results in an error if widget is not a child
     * in this splitter */
    bool isLastChild(QWidget* w);
};

#endif
