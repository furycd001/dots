/*
  Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/                                                                            

#include <qlayout.h>
#include <qlabel.h>

#include <kglobal.h>
#include <klocale.h>

#include "kcrootonly.h"

KCRootOnly::KCRootOnly(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
   QVBoxLayout *layout=new QVBoxLayout(this);
   QLabel *label = new QLabel(i18n("<big>You need super user privileges to run this control module.</big><br>"
                                    "Click on the \"Modify\" button below."), this);
   layout->addWidget(label);
   label->setAlignment(AlignCenter);
   label->setTextFormat(RichText);
   label->setMinimumSize(label->sizeHint());
}


