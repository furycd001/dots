/*
    $Id$

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1996 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

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


#include <kapp.h>
#include "kfontdialog.h"
#include <kconfig.h>


 int main( int argc, char **argv )
{
  KApplication app( argc, argv, "KFontDialogTest" );

  KConfig aConfig;
  aConfig.setGroup( "KFontDialog-test" );

  app.setFont(QFont("Helvetica",12));

  //  QFont font = QFont("Times",18,QFont::Bold);

  QFont font = aConfig.readFontEntry( "Chosen" );
  int nRet = KFontDialog::getFont(font);
  aConfig.writeEntry( "Chosen", font, true );

  aConfig.sync();
  return nRet;
}
