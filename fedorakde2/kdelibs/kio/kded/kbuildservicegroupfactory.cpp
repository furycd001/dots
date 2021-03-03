/*  This file is part of the KDE libraries
 *  Copyright (C) 2000 Waldo Bastian <bastian@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "kbuildservicegroupfactory.h"
#include "ksycoca.h"
#include "ksycocadict.h"
#include "kresourcelist.h"

#include <kglobal.h>
#include <kstddirs.h>
#include <kmessageboxwrapper.h>
#include <kdebug.h>
#include <klocale.h>
#include <assert.h>
#include <stdio.h>

KBuildServiceGroupFactory::KBuildServiceGroupFactory() :
  KServiceGroupFactory()
{
   m_resourceList = new KSycocaResourceList();
   m_resourceList->add( "apps", "*.directory" );
}

KBuildServiceGroupFactory::~KBuildServiceGroupFactory()
{
   delete m_resourceList;
}

KServiceGroup *
KBuildServiceGroupFactory::createEntry( const QString& file, const char *resource )
{
  return addNewEntry(file, resource, 0);
}

KServiceGroup *
KBuildServiceGroupFactory::addNewEntry( const QString& file, const char *resource, KSycocaEntry *newEntry)
{
  if (strcmp(resource, "apps") != 0) return 0;

  QString name = file;
  int pos = name.findRev('/');
  if (pos != -1) {
     name = name.left(pos+1);

#ifdef XDG_COMPAT
  } else if (newEntry && newEntry->sycocaType() == KST_KService) {
     KDesktopFile desktopFile(locate(resource, file), true);
     desktopFile.setDesktopGroup();
     const QStringList categories = desktopFile.readListEntry("Categories", ';');
     if (!categories.isEmpty()) {
         // Has category, but maybe not a known one, so to avoid filling the
         // entire popup menu with random apps just stuff it in the generic
         // subfolder
         name = "Applications/";
     } else {
         name = "/";
     }
     // Reverse iteration, the most specific ones are at the end
     for (int i=categories.count() - 1; i >= 0; i--) {
         QString category = categories[i];
         if (category == "Translation") {
             name = "Development/";
             break;
         } else if (category == "WebDevelopment") {
             name = "Development/";
             break;
         } else if (category == "AudioVideo") {
             name = "Multimedia/";
             break;
         } else if (category == "TextEditor") {
             name = "Editors/";
             break;
         } else if (category == "Game") {
             name = "Games/";
             break;
         } else if (category == "StrategyGame") {
             name = "Games/TacticStrategy/";
             break;
         } else if (category == "ArcadeGame") {
             name = "Games/Arcade/";
             break;
         } else if (category == "ActionGame") {
             name = "Games/Arcade/";
             break;
         } else if (category == "Network") {
             name = "Internet/";
             break;
         } else if (category == "Utility") {
             name = "Utilities/";
             break;
         } else if (category == "WordProcessor") {
             name = "WordProcessing/";
             break;
         } else if (category == "Application") {
             continue;
         }
         QString fullPath = locate( resource, category + "/.directory");
         if (fullPath.isEmpty() || fullPath == "/") {
             continue;
         }
         name = category + "/";
         break;
     }

     if (name == "Applications/") {
         printf("Missing category for %s\n", file.ascii());
     }
#endif
  } else {
     name = "/";
  }

  KServiceGroup *entry = 0;
  KSycocaEntry::Ptr *ptr = m_entryDict->find(name);
  if (ptr)
     entry = dynamic_cast<KServiceGroup *>(ptr->data());

  if (!entry)
  {
     // Create new group entry
     QString fullPath = locate( resource, name + ".directory");

     entry = new KServiceGroup(fullPath, name);
     addEntry( entry, resource );

     if (name != "/")
     {
        // Make sure parent dir exists.
        KServiceGroup *parentEntry = 0;
        QString parent = name.left(name.length()-1);
        int i = parent.findRev('/');
        if (i > 0) {
           parent = parent.left(i+1);
        } else {
           parent = "/";
        }
        parentEntry = 0;
        ptr = m_entryDict->find(parent);
        if (ptr)
           parentEntry = dynamic_cast<KServiceGroup *>(ptr->data());
        if (!parentEntry)
        {
           parentEntry = addNewEntry( parent, resource, 0 );
        }
        if (parentEntry && !entry->isDeleted())
           parentEntry->addEntry( entry );
     }
  }
  if (newEntry)
     entry->addEntry( newEntry );

  return entry;
}

void
KBuildServiceGroupFactory::addEntry( KSycocaEntry *newEntry, const char *resource)
{
   KSycocaFactory::addEntry(newEntry, resource);
   KServiceGroup * serviceGroup = (KServiceGroup *) newEntry;

   if ( !serviceGroup->baseGroupName().isEmpty() )
   {
       m_baseGroupDict->add( serviceGroup->baseGroupName(), newEntry );
   }
}

void
KBuildServiceGroupFactory::saveHeader(QDataStream &str)
{
   KSycocaFactory::saveHeader(str);

   str << (Q_INT32) m_baseGroupDictOffset;
}

void
KBuildServiceGroupFactory::save(QDataStream &str)
{
   KSycocaFactory::save(str);

   m_baseGroupDictOffset = str.device()->at();
   m_baseGroupDict->save(str);

   int endOfFactoryData = str.device()->at();

   // Update header (pass #3)
   saveHeader(str);

   // Seek to end.
   str.device()->at(endOfFactoryData);
}
