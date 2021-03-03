/***************************************************************************
                          smserverconfigimpl.h  -  description
                             -------------------
    begin                : Thu May 17 2001
    copyright            : (C) 2001 by stulle
    email                : stulle@tux
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SMSERVERCONFIGIMPL_H
#define SMSERVERCONFIGIMPL_H

#include <qwidget.h>
#include <smserverconfigdlg.h>

/**
  *@author stulle
  */

class SMServerConfigImpl : public SMServerConfigDlg  {
   Q_OBJECT
public: 
	SMServerConfigImpl(QWidget *parent=0, const char *name=0);
	~SMServerConfigImpl();
public slots: // Public slots
  /** No descriptions */
  void configChanged();
signals: // Signals
  /** No descriptions */
  void changed();
};

#endif
