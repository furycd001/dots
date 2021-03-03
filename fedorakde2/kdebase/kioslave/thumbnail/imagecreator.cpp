/*  This file is part of the KDE libraries
    Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
                  2000 Malte Starostik <malte@kde.org>

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

// $Id: imagecreator.cpp,v 1.6 2001/07/27 22:45:50 malte Exp $

#include <assert.h>

#include <qimage.h>

#include <kimageio.h>

#include "imagecreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        KImageIO::registerFormats();
        return new ImageCreator;
    }
};

bool ImageCreator::create(const QString &path, int, int, QImage &img)
{
    // create image preview
    return img.load( path );
}

ThumbCreator::Flags ImageCreator::flags() const
{
    return static_cast<Flags>(DrawFrame);
}
