/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _KCMUSB_H
#define _KCMUSB_H


#include <qintdict.h>


#include <kcmodule.h>


class QListView;
class QListViewItem;
class QTextView;


class USBViewer : public KCModule
{
  Q_OBJECT

public:

  USBViewer(QWidget *parent = 0L, const char *name = 0L);
  virtual ~USBViewer();

  void load();
  void save();
  void defaults();
  
  int buttons();
  QString quickHelp() const;


protected slots:

  void selectionChanged(QListViewItem *item);


private:

  QListView *_devices;
  QTextView *_details;
};


#endif
