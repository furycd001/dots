/*
 *   Copyright (C) 1997  Michael Roth <mroth@wirlweb.de>
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


#include <kapp.h>
#include <qlayout.h>
#include <qwidget.h>

#include "kseparator.h"


int main(int argc, char **argv)
{
   QApplication app(argc, argv);
   
   QWidget *toplevel = new QWidget();
   
   QBoxLayout *mainbox = new QBoxLayout(toplevel, QBoxLayout::TopToBottom, 10);
   
   KSeparator *sep1 = new KSeparator( KSeparator::VLine, toplevel );
   mainbox->addWidget(sep1);
   
   KSeparator *sep2 = new KSeparator( KSeparator::HLine, toplevel );
   mainbox->addWidget(sep2);
   
   mainbox->activate();
   
   app.setMainWidget(toplevel);
   toplevel->show();
   return app.exec();
}




