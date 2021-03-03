/***************************************************************************
                          katefactory.cpp  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by Christoph Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "katefactory.h"
#include "katefactory.moc"

#include "../document/katedocument.h"

#include <klocale.h>
#include <kinstance.h>
#include <kaboutdata.h>

extern "C"
{
  void *init_libkatecore()
  {
    return new KateFactory();
  }
}

KInstance *KateFactory::s_instance = 0L;

KateFactory::KateFactory()
{
  s_instance = 0L;
}

KateFactory::~KateFactory()
{
  if ( s_instance )
  {
    delete s_instance->aboutData();
    delete s_instance;
  }
  s_instance = 0L;
}

KParts::Part *KateFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const char *classname, const QStringList & )
{
  bool bWantSingleView = !( (  classname == QString("KTextEditor::Document") ) || ( classname == QString("Kate::Document") ) );
  bool bWantBrowserView = ( classname == QString("Browser/View") );

  KParts::ReadWritePart *part = new KateDocument (bWantSingleView, bWantBrowserView, parentWidget, widgetName, parent, name);

  if ( bWantBrowserView || ( classname == QString("KParts::ReadOnlyPart") ) )
    part->setReadWrite( false );

  emit objectCreated( part );
  return part;
}

KInstance *KateFactory::instance()
{
  if ( !s_instance )
    s_instance = new KInstance( aboutData() );
  return s_instance;
}

const KAboutData *KateFactory::aboutData()
{
  KAboutData *data = new KAboutData  ("kate", I18N_NOOP("Kate"), "1.0",
                                                           I18N_NOOP( "Kate - KDE Advanced Text Editor" ), KAboutData::License_GPL,
                                                           "(c) 2000-2001 The Kate Authors", 0, "http://kate.sourceforge.net");

  data->addAuthor ("Christoph Cullmann", I18N_NOOP("Project Manager and Core Developer"), "cullmann@kde.org", "http://www.babylon2k.de");
  data->addAuthor ("Anders Lund", I18N_NOOP("Core Developer"), "anders@alweb.dk", "http://www.alweb.dk");
  data->addAuthor ("Joseph Wenninger", I18N_NOOP("Core Developer"), "jowenn@bigfoot.com","http://stud3.tuwien.ac.at/~e9925371");
  data->addAuthor ("Michael Bartl", I18N_NOOP("Core Developer"), "michael.bartl1@chello.at");
  data->addAuthor ("Phlip", I18N_NOOP("The Project Compiler"), "phlip_cpp@my-deja.com");
  data->addAuthor ("Waldo Bastian", I18N_NOOP( "The cool buffersystem" ), "bastian@kde.org" );
  data->addAuthor ("Matt Newell", I18N_NOOP("Testing, ..."), "newellm@proaxis.com");
  data->addAuthor ("Michael McCallum", I18N_NOOP("Core Developer"), "gholam@xtra.co.nz");
  data->addAuthor ("Jochen Wilhemly", I18N_NOOP( "KWrite Author" ), "digisnap@cs.tu-berlin.de" );
  data->addAuthor ("Michael Koch",I18N_NOOP("KWrite port to KParts"), "koch@kde.org");
  data->addAuthor ("Christian Gebauer", 0, "gebauer@kde.org" );
  data->addAuthor ("Simon Hausmann", 0, "hausmann@kde.org" );
  data->addAuthor ("Glen Parker",I18N_NOOP("KWrite Undo History, Kspell integration"), "glenebob@nwlink.com");
  data->addAuthor ("Scott Manson",I18N_NOOP("KWrite XML Syntax highlighting support"), "sdmanson@alltel.net");
  data->addAuthor ("John Firebaugh",I18N_NOOP("Patches and more"), "jfirebaugh@kde.org");

  data->addCredit ("Matteo Merli",I18N_NOOP("Highlighting for RPM Spec-Files, Perl, Diff and more"), "merlim@libero.it");
  data->addCredit ("Rocky Scaletta",I18N_NOOP("Highlighting for VHDL"), "rocky@purdue.edu");
  data->addCredit ("Yury Lebedev",I18N_NOOP("Highlighting for SQL"),"");
  data->addCredit ("Cristi Dumitrescu",I18N_NOOP("PHP Keyword/Datatype list"),"");
  data->addCredit ("Carsten Presser", I18N_NOOP("Betatest"), "mord-slime@gmx.de");
  data->addCredit ("Jens Haupert", I18N_NOOP("Betatest"), "al_all@gmx.de");
  data->addCredit ("Carsten Pfeiffer", I18N_NOOP("Very nice help"), "");
  data->addCredit ("Mr. Doerr", I18N_NOOP("For absolutely nothing"), "");

  return data;
}
