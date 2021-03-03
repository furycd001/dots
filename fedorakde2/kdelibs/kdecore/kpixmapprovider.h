/* This file is part of the KDE libraries

   Copyright (c) 2000 Carsten Pfeiffer <pfeiffer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
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

#ifndef KPIXMAPPROVIDER_H
#define KPIXMAPPROVIDER_H

#include <qpixmap.h>

/**
 * A tiny abstract class with just one method:
 * @ref pixmapFor()
 *
 * It will be called whenever an icon is searched for @p text.
 *
 * Used e.g. by @ref KHistoryCombo
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @short an abstract interface for looking up icons
 */
class KPixmapProvider
{
public:
    virtual ~KPixmapProvider();
    /**
     * You may subclass this and return a pixmap of size @p size for @p text.
     */
    virtual QPixmap pixmapFor( const QString& text, int size = 0 ) = 0;
};


#endif // KPIXMAPPROVIDER_H
