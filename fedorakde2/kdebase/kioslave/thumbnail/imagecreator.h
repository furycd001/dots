/*  This file is part of the KDE libraries
    Copyright (C) 2000 Malte Starostik <malte@kde.org>

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

#ifndef _IMAGECREATOR_H_
#define _IMAGECREATOR_H_ "$Id: imagecreator.h,v 1.4 2001/07/27 22:45:50 malte Exp $"

#include "thumbcreator.h"

class ImageCreator : public ThumbCreator
{
public:
    ImageCreator() {};
    virtual bool create(const QString &path, int, int, QImage &img);
	virtual Flags flags() const;
};

#endif
