/*
   This file is part of the KDE libraries
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (C) 1997 Matthias Kalle Dalheimer <kalle@kde.org>
 
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

#ifndef KMIMESOURCEFACTORY_H
#define KMIMESOURCEFACTORY_H

#include <qmime.h>
#include <kglobal.h>

/**
 * An extension to @ref QMimeSourceFactory that uses @ref KIconLoader to
 * find images.
 *
 * Normally you don't have to instantiate this class at all, @ref KApplication does that for
 * you automagically and sets @ref QMimeSourceFactory::setDefaultFactory.
 *
 * @version $Id$
 * @author Peter Putzer <putzer@kde.org>
 */
class KMimeSourceFactory : public QMimeSourceFactory
{
public:

  /**
   * Constructor.
   *
   * @param loader is the iconloader used to find images.
   */
  KMimeSourceFactory (KIconLoader* loader = KGlobal::iconLoader());

  /**
   * Destructor.
   */
  virtual ~KMimeSourceFactory();

  /**
   * This function is maps an absolute or relative name for a resource to 
   * the absolute one.
   *
   * To load an icon, prepend the @p category name before the @p icon name, in the style
   * of <category>|<icon>.
   *
   * Example:
   * <pre> "<img src=\"user|ksysv_start\"/>", "<img src="\desktop|trash\">", ...
   * </pre>
   *
   * @param abs_or_rel_name is the absolute or relative pathname.
   * @param context is the path of the context object for the queried resource. Almost always empty.
   */
  virtual QString makeAbsolute (const QString& abs_or_rel_name, const QString& context) const;

private:
  class KMimeSourceFactoryPrivate;
  KMimeSourceFactoryPrivate* d;
};

#endif // KMIMESOURCEFACTORY_H
