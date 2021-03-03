/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>


#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>


#include <kstddirs.h>


#include "usbdb.h"


USBDB::USBDB()
{
  QString db = locate("data", "kcmusb/usb.ids");
  if (db.isEmpty())
    return;

  _classes.setAutoDelete(true);
  _ids.setAutoDelete(true);

  QFile f(db);

  if (f.open(IO_ReadOnly))
    {
      QTextStream ts(&f);

      QString line, name;
      int id=0, subid=0, protid=0;
      int len;
      QRegExp vendor("[0-9a-fA-F]+ ");
      QRegExp product("\\s+[0-9a-fA-F]+ ");
      QRegExp cls("C [0-9a-fA-F][0-9a-fA-F]");
      QRegExp subclass("\\s+[0-9a-fA-F][0-9a-fA-F]  ");
      QRegExp prot("\\s+[0-9a-fA-F][0-9a-fA-F]  ");
      while (!ts.eof())
	{
	  line = ts.readLine();
	  if (line.left(1) == "#" || line.stripWhiteSpace().isEmpty())
	    continue;

	  // skip AT lines
	  if (line.left(2) == "AT")
	    continue;

	  if (cls.match(line, 0, &len) == 0 && len == 4)
	    {
	      id = line.mid(2,2).toInt(0, 16);
	      name = line.mid(4).stripWhiteSpace();
	      _classes.insert(QString("%1").arg(id), new QString(name));
	    }
	  else if (prot.match(line, 0, &len) == 0 && len > 5)
	    {
	      line = line.stripWhiteSpace();
	      protid = line.left(2).toInt(0, 16);
	      name = line.mid(4).stripWhiteSpace();
	      _classes.insert(QString("%1-%2-%3").arg(id).arg(subid).arg(protid), new QString(name));
	    }
	  else if (subclass.match(line, 0, &len) == 0 && len > 4)
	    {
	      line = line.stripWhiteSpace();
	      subid = line.left(2).toInt(0, 16);
	      name = line.mid(4).stripWhiteSpace();
	      _classes.insert(QString("%1-%2").arg(id).arg(subid), new QString(name));
	    }
	  else if (vendor.match(line, 0, &len) == 0 && len == 5)
	    {
	      id = line.left(4).toInt(0,16);
	      name = line.mid(6);
	      _ids.insert(QString("%1").arg(id), new QString(name));
	    }
	  else if (product.match(line, 0, &len) == 0 && len > 5 )
	    {
	      line = line.stripWhiteSpace();
	      subid = line.left(4).toInt(0,16);
	      name = line.mid(6);
	      _ids.insert(QString("%1-%2").arg(id).arg(subid), new QString(name));
	    }

	}

      f.close();
    }
}


QString USBDB::vendor(int id)
{
  QString *s = _ids[QString("%1").arg(id)];
  if ((id!= 0) && s)
    {
      return *s;
    }
  return QString::null;
}


QString USBDB::device(int vendor, int id)
{
  QString *s = _ids[QString("%1-%2").arg(vendor).arg(id)];
  if ((id != 0) && (vendor != 0) && s)
    return *s;
  return QString::null;
}


QString USBDB::cls(int cls)
{
  QString *s = _classes[QString("%1").arg(cls)];
  if (s)
    return *s;
  return QString::null;
}


QString USBDB::subclass(int cls, int sub)
{
  QString *s = _classes[QString("%1-%2").arg(cls).arg(sub)];
  if (s)
    return *s;
  return QString::null;
}


QString USBDB::protocol(int cls, int sub, int prot)
{
  QString *s = _classes[QString("%1-%2-%2").arg(cls).arg(sub).arg(prot)];
  if (s)
    return *s;
  return QString::null;
}

