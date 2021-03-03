/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef __USB_DEVICES_H__
#define __USB_DEVICES_H__


#include <qstring.h>
#include <qlist.h>


class USBDB;


class USBDevice
{
public:
  
  USBDevice();

  void parseLine(QString line);

  int level() { return _level; };
  int device() { return _device; };
  int parent() { return _parent; };
  int bus() { return _bus; };
  QString product();

  QString dump();

  static QList<USBDevice> &devices() { return _devices; };
  static USBDevice *find(int bus, int device);
  static void parse(QString fname);


private:

  static QList<USBDevice> _devices;

  static USBDB *_db;

  int _bus, _level, _parent, _port, _count, _device, _channels;
  float _speed;

  QString _manufacturer, _product, _serial;

  int _bwTotal, _bwUsed, _bwPercent, _bwIntr, _bwIso;
  bool _hasBW;

  int _verMajor, _verMinor, _class, _sub, _prot, _maxPacketSize, _configs;
  QString _className;

  int _vendorID, _prodID, _revMajor, _revMinor;

};


#endif
