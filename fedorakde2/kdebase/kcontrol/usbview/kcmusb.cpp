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


#include <qlayout.h>
#include <qgroupbox.h>
#include <qsplitter.h>
#include <qlistview.h>
#include <qtextview.h>
#include <qheader.h>
#include <qvaluelist.h>

#include <klocale.h>
#include <kglobal.h>


#include "usbdevices.h"
#include "kcmusb.moc"


USBViewer::USBViewer(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  setButtons(Help);

  QVBoxLayout *vbox = new QVBoxLayout(this);
  QGroupBox *gbox = new QGroupBox(i18n("USB Devices"), this);
  vbox->addWidget(gbox);

  QVBoxLayout *vvbox = new QVBoxLayout(gbox, 6);
  vvbox->addSpacing(gbox->fontMetrics().height());

  QSplitter *splitter = new QSplitter(gbox);
  vvbox->addWidget(splitter);

  _devices = new QListView(splitter);
  _devices->addColumn(i18n("Device"));
  _devices->setRootIsDecorated(true);
  _devices->header()->hide();
  _devices->setMinimumWidth(200);
  _devices->setColumnWidthMode(0, QListView::Maximum);

  QValueList<int> sizes;
  sizes.append(200);
  splitter->setSizes(sizes);

  _details = new QTextView(splitter);

  splitter->setResizeMode(_devices, QSplitter::KeepSize);

  connect(_devices, SIGNAL(selectionChanged(QListViewItem*)),
	  this, SLOT(selectionChanged(QListViewItem*)));

  load();
}


USBViewer::~USBViewer()
{
}


void USBViewer::load()
{ 
  QIntDict<QListViewItem> _items;

  _devices->clear();

  USBDevice::parse("/proc/bus/usb/devices");

  int level = 0;
  bool found = true;

  while (found)
    {
      found = false;

      QListIterator<USBDevice> it(USBDevice::devices());
      for ( ; it.current(); ++it)
	if (it.current()->level() == level)
	  {
	    if (level == 0)
	      {
		QListViewItem *item = new QListViewItem(_devices, 
				it.current()->product(), 
				QString("%1").arg(it.current()->bus()),
				QString("%1").arg(it.current()->device()) );
		_items.insert(it.current()->bus()*256+it.current()->device(),
				item);
		found = true;
	      }
	    else
	      {
		QListViewItem *parent = _items.find(it.current()->bus()*256+1);
		if (parent)
		  {
		    QListViewItem *item = new QListViewItem(parent, 
				    it.current()->product(),
				    QString("%1").arg(it.current()->bus()),
				    QString("%1").arg(it.current()->device()) );
		    _items.insert(it.current()->bus()*256+it.current()->device(),
				item);
		    parent->setOpen(true);
		    found = true;
		  }
	      }
	  }

      ++level;
    }

  selectionChanged(_devices->firstChild());
}


void USBViewer::selectionChanged(QListViewItem *item)
{
  if (item)
    {
      USBDevice *dev = USBDevice::find(item->text(1).toInt(),
		      item->text(2).toInt());
      if (dev)
	{
	  _details->setText(dev->dump());
	  return;
	}
    }
  _details->setText("");
}


void USBViewer::save()
{
}


void USBViewer::defaults()
{
}


QString USBViewer::quickHelp() const
{
  return i18n("<h1>USB Devices</h1> This module allows you to see"
     " the devices attached to your USB bus(es).");
}


extern "C"
{
  KCModule *create_usb(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmusb");
    return new USBViewer(parent, name);
  };
}
