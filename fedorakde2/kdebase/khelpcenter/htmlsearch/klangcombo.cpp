/*
 * klangcombo.cpp - Adds some methods for inserting languages.
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qiconset.h>

#include <ksimpleconfig.h>
#include <kstddirs.h>

#include "klangcombo.h"
#include "klangcombo.moc"

KLanguageCombo::~KLanguageCombo ()
{
}

KLanguageCombo::KLanguageCombo (QWidget * parent, const char *name)
  : KTagComboBox(parent, name)
{
}

void KLanguageCombo::insertLanguage(const QString& path, const QString& name, const QString& sub, const QString &submenu, int index)
{
  QString output = name + QString::fromLatin1(" (") + path + QString::fromLatin1(")");
  QPixmap flag(locate("locale", sub + path + QString::fromLatin1("/flag.png")));
  insertItem(QIconSet(flag), output, path, submenu, index);
}

void KLanguageCombo::changeLanguage(const QString& name, int i)
{
  if (i < 0 || i >= count()) return;
  QString output = name + QString::fromLatin1(" (") + tag(i) + QString::fromLatin1(")");
  changeItem(output, i);
}
