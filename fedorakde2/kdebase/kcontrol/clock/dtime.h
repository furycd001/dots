/*
 *  dtime.h
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef dtime_included
#define dtime_included

#include <qtableview.h>
#include <qdatetime.h> 
#include <qlineedit.h> 
#include <qspinbox.h> 
#include <qtimer.h>
#include <qvalidator.h>

#include <kdatetbl.h>

class Kclock;

class Dtime : public QWidget
{
  Q_OBJECT
 public:
  Dtime( QWidget *parent=0, const char* name=0 );

  void	save();
  void	load();

  QString quickHelp() const;

signals:
	void timeChanged(bool);

 private slots:
  void	timeout();
  void	inc_time();
  void	dec_time();
  void	joke(QLineEdit* edit,int incr,int Max,int Min,bool refr);
  void	set_month(int);
  void	set_year(int);
  void	set_time();
  void	changeDate(QDate);

private:
  KDateTable	*cal;
  QComboBox	*month;
  QSpinBox	*year;

  QLineEdit	*hour;
  QLineEdit	*minute;
  QLineEdit	*second;

  Kclock        *kclock;
  
  QTime		time;
  QDate		date;
  QTimer        internalTimer;
  
  QString	BufS;
  int		BufI;
  bool		refresh;
  bool		ontimeout;
};

class Kclock : public QWidget
{
  Q_OBJECT

public:
  Kclock( QWidget *parent=0, const char *name=0 ) 
    : QWidget(parent, name) {};
  
  void setTime(const QTime&);
  
protected:
  virtual void	paintEvent( QPaintEvent *event );
  
  
private:
  QTime		time;
};

class KStrictIntValidator : public QIntValidator 
{
public:
  KStrictIntValidator(int bottom, int top, QWidget * parent,
		      const char * name = 0 )
    : QIntValidator(bottom, top, parent, name) {};
  
  QValidator::State validate( QString & input, int & d ) const; 
};

#endif // dtime_included
