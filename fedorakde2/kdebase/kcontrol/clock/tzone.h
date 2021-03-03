/*
 *  tzone.h
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

#ifndef tzone_included
#define tzone_included

class QComboBox;

class Tzone : public QWidget
{
  Q_OBJECT

public:
  Tzone( QWidget *parent=0, const char* name=0 );
  
  void	save();
  void  load();
  
signals:
	void zoneChanged(bool);

protected slots:
  void handleZoneChange() {emit zoneChanged( TRUE );}

private:
  void  fillTimeZones();
	void	getCurrentZone(char* szString);

  QComboBox       *tzonelist;
	QLabel					*currentzonetitle;
	QLabel          *currentzone;
  QString         BufS;
  int             pos;
};

#endif // tzone_included
