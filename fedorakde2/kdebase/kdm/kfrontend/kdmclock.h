/*
 *   clock module for kdm
 *   Copyright (C) 2000 Espen Sand, espen@kde.org Based on work by NN
 *   (yet to be determined)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _KDM_CLOCK_H_
#define _KDM_CLOCK_H_

#include <qframe.h>
#include <qdatetime.h>

class KdmClock : public QFrame
{
  Q_OBJECT

  public:
    KdmClock( QWidget *parent=0, const char *name=0 );

  protected:
    virtual void showEvent( QShowEvent * );
    virtual void paintEvent( QPaintEvent * );
     
  private slots:
    void timeout();
    void initialize();

  private:
     QTime  mTime;
     QFont  mFont;
     QColor mBackground;
     QColor mForeground;     
     bool   mSecond;
     bool   mDigital;
     bool   mDate;
     bool   mBorder;
};


#endif
