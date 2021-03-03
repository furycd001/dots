/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kdebugdialog.h"
#include "klistdebugdialog.h"
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kstddirs.h>
#include <qtextstream.h>
#include <klocale.h>
#include <kdebug.h>
#include <kuniqueapp.h>
#include <kconfig.h>

#include <qfile.h>

QStringList readAreaList()
{
  QStringList lst;
  lst.append( "0 (generic)" );

  QString confAreasFile = locate( "config", "kdebug.areas" );
  QFile file( confAreasFile );
  if (!file.open(IO_ReadOnly)) {
    kdWarning() << "Couldn't open debug areas file " << confAreasFile << endl;
    file.close();
  }
  else
  {
    QString data;

    QTextStream *ts = new QTextStream(&file);
    ts->setEncoding( QTextStream::Latin1 );
    while (!ts->eof()) {
      data = ts->readLine().simplifyWhiteSpace();

      int pos = data.find("#");
      if ( pos != -1 )
        data.truncate( pos );

      if (data.isEmpty())
        continue;

      lst.append( data );
    }

    delete ts;
    file.close();
  }

  return lst;
}

static KCmdLineOptions options[] =
{
  { "fullmode", I18N_NOOP("Show the fully-fledged dialog instead of the default list dialog."), 0 },
  { "on <area>", /*I18N_NOOP TODO*/ "Turn area on", 0 },
  { "off <area>", /*I18N_NOOP TODO*/ "Turn area off", 0 },
  { 0, 0, 0 }
};

int main(int argc, char ** argv)
{
  KAboutData data( "kdebugdialog", I18N_NOOP( "KDebugDialog"),
    "1.0", I18N_NOOP("A dialog box for setting preferences for debug output"),
    KAboutData::License_GPL, "(c) 1999-2000, David Faure <faure@kde.org>");
  data.addAuthor("David Faure", I18N_NOOP("Maintainer"), "faure@kde.org");
  KCmdLineArgs::init( argc, argv, &data );
  KCmdLineArgs::addCmdLineOptions( options );
  KUniqueApplication::addCmdLineOptions();
  KUniqueApplication app;
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  QStringList areaList ( readAreaList() );
  KAbstractDebugDialog * dialog;
  if (args->isSet("fullmode"))
      dialog = new KDebugDialog(areaList, 0L);
  else
  {
      KListDebugDialog * listdialog = new KListDebugDialog(areaList, 0L);
      if (args->isSet("on"))
      {
          listdialog->activateArea( args->getOption("on"), true );
          /*listdialog->save();
          listdialog->config()->sync();
          return 0;*/
      } else if ( args->isSet("off") )
      {
          listdialog->activateArea( args->getOption("off"), false );
          /*listdialog->save();
          listdialog->config()->sync();
          return 0;*/
      }
      dialog = listdialog;
  }

  /* Show dialog */
  int nRet = dialog->exec();
  if( nRet == QDialog::Accepted )
  {
      dialog->save();
      dialog->config()->sync();
  }
  else
    dialog->config()->rollback( true );

  return 0;
}
