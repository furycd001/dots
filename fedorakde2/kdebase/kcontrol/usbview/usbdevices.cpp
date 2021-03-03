/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>


#include <qfile.h>
#include <qlist.h>


#include <klocale.h>
#include <kglobal.h>


#include "usbdb.h"
#include "usbdevices.h"


QList<USBDevice> USBDevice::_devices;
USBDB *USBDevice::_db;


USBDevice::USBDevice()
  : _bus(0), _level(0), _parent(0), _port(0), _count(0), _device(0),
    _channels(0), _speed(0.0), _manufacturer(""), _product(""), _serial(""),
    _bwTotal(0), _bwUsed(0), _bwPercent(0), _bwIntr(0), _bwIso(0), _hasBW(false),
    _verMajor(0), _verMinor(0), _class(0), _sub(0), _prot(0), _maxPacketSize(0), _configs(0),
    _className(""), _vendorID(0), _prodID(0), _revMajor(0), _revMinor(0)
{
  _devices.append(this);
  _devices.setAutoDelete(true);

  if (!_db)
    _db = new USBDB;
}


void USBDevice::parseLine(QString line)
{
  if (line.left(2) == "T:")
    sscanf(line.local8Bit().data(),
	   "T:  Bus=%2d Lev=%2d Prnt=%2d Port=%d Cnt=%2d Dev#=%3d Spd=%3f MxCh=%2d",
	   &_bus, &_level, &_parent, &_port, &_count, &_device, &_speed, &_channels);
  else if (line.left(16) == "S:  Manufacturer")
    _manufacturer = line.mid(17);
  else if (line.left(11) == "S:  Product") {
    _product = line.mid(12);
    /* add bus number to root devices */
    if (_device==1)
	_product += QString(" (%1)").arg(_bus);
  }
  else if (line.left(16) == "S:  SerialNumber")
    _serial = line.mid(17);
  else if (line.left(2) == "B:")
    {
      sscanf(line.local8Bit().data(),
	     "B:  Alloc=%3d/%3d us (%2d%%), #Int=%3d, #Iso=%3d",
	     &_bwUsed, &_bwTotal, &_bwPercent, &_bwIntr, &_bwIso);
      _hasBW = true;
    }
  else if (line.left(2) == "D:")
    {
      char buffer[10];
      sscanf(line.local8Bit().data(),
	     "D:  Ver=%x.%x Cls=%x(%s) Sub=%x Prot=%x MxPS=%d #Cfgs=%d",
	     &_verMajor, &_verMinor, &_class, buffer, &_sub, &_prot, &_maxPacketSize, &_configs);
      _className = buffer;
    }
  else if (line.left(2) == "P:")
    sscanf(line.local8Bit().data(),
	   "P:  Vendor=%x ProdID=%x Rev=%x.%x",
	   &_vendorID, &_prodID, &_revMajor, &_revMinor);
}


USBDevice *USBDevice::find(int bus, int device)
{
  QListIterator<USBDevice> it(_devices);
  for ( ; it.current(); ++it)
    if (it.current()->bus() == bus && it.current()->device() == device)
      return it.current();
  return 0;
}

QString USBDevice::product()
{
  if (!_product.isEmpty())
    return _product;
  QString pname = _db->device(_vendorID, _prodID);
  if (!pname.isEmpty())
    return pname;
  return i18n("Unknown");
}


QString USBDevice::dump()
{
  QString r;

  r = "<qml><h2><center>" + product() + "</center></h2><br/><hl/>";

  if (!_manufacturer.isEmpty())
    r += i18n("<b>Manufacturer:</b> ") + _manufacturer + "<br/>";
  if (!_serial.isEmpty())
    r += i18n("<b>Serial #:</b> ") + _serial + "<br/>";

  r += "<br/><table>";

  QString c = QString("<td>%1</td>").arg(_class);
  QString cname = _db->cls(_class);
  if (!cname.isEmpty())
    c += "<td>(" + i18n(cname.latin1()) +")</td>";
  r += i18n("<tr><td><i>Class</i></td>%1</tr>").arg(c);
  QString sc = QString("<td>%1</td>").arg(_sub);
  QString scname = _db->subclass(_class, _sub);
  if (!scname.isEmpty())
    sc += "<td>(" + i18n(scname.latin1()) +")</td>";
  r += i18n("<tr><td><i>Subclass</i></td>%1</tr>").arg(sc);
  QString pr = QString("<td>%1</td>").arg(_prot);
  QString prname = _db->protocol(_class, _sub, _prot);
  if (!prname.isEmpty())
    pr += "<td>(" + prname +")</td>";
  r += i18n("<tr><td><i>Protocol</i></td>%1</tr>").arg(pr);
  r += i18n("<tr><td><i>USB Version</i></td><td>%1.%2</td></tr>").arg(_verMajor).arg(_verMinor);
  r += "<tr><td></td></tr>";

  QString v = QString::number(_vendorID,16);
  QString name = _db->vendor(_vendorID);
  if (!name.isEmpty())
    v += "<td>(" + name +")</td>";
  r += i18n("<tr><td><i>Vendor ID</i></td><td>0x%1</td></tr>").arg(v);
  QString p = QString::number(_prodID,16);
  QString pname = _db->device(_vendorID, _prodID);
  if (!pname.isEmpty())
    p += "<td>(" + pname +")</td>";
  r += i18n("<tr><td><i>Product ID</i></td><td>0x%1</td></tr>").arg(p);
  r += i18n("<tr><td><i>Revision</i></td><td>%1.%2</td></tr>").arg(_revMajor).arg(_revMinor);
  r += "<tr><td></td></tr>";

  r += i18n("<tr><td><i>Speed</i></td><td>%1 Mbit/s</td></tr>").arg(_speed);
  r += i18n("<tr><td><i>Channels</i></td><td>%1</td></tr>").arg(_channels);
  r += i18n("<tr><td><i>Max. Packet Size</i></td><td>%1</td></tr>").arg(_maxPacketSize);
  r += "<tr><td></td></tr>";

  if (_hasBW)
    {
      r += i18n("<tr><td><i>Bandwidth</i></td><td>%1 of %2 (%3%)</td></tr>").arg(_bwUsed).arg(_bwTotal).arg(_bwPercent);
      r += i18n("<tr><td><i>Intr. requests</i></td><td>%1</td></tr>").arg(_bwIntr);
      r += i18n("<tr><td><i>Isochr. requests</i></td><td>%1</td></tr>").arg(_bwIso);
      r += "<tr><td></td></tr>";
    }

  r += "</table>";

  return r;
}


void USBDevice::parse(QString fname)
{
  _devices.clear();

  QString result;

  // read in the complete file
  //
  // Note: we can't use a QTextStream, as the files in /proc
  // are pseudo files with zero length
  char buffer[256];
  int fd = ::open(QFile::encodeName(fname), O_RDONLY);
  if (fd >= 0)
    {
      ssize_t count;
      while ((count = ::read(fd, buffer, 256)) > 0)
	result.append(QString(buffer).left(count));

      ::close(fd);
    }

  // read in the device infos
  USBDevice *device = 0;
  int start=0, end;
  while ((end = result.find('\n', start)) > 0)
    {
      QString line = result.mid(start, end-start);

      if (line.left(2) == "T:")
	device = new USBDevice();

      if (device)
	device->parseLine(line);

      start = end+1;
    }
}

