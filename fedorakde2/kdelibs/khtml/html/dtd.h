/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#ifndef _DOM_dtd_h_
#define _DOM_dtd_h_

#include "dom_string.h"

namespace DOM
{

void addForbidden(int tagId, ushort *forbiddenTags);
void removeForbidden(int tagId, ushort *forbiddenTags);

enum tagStatus { OPTIONAL, REQUIRED, FORBIDDEN };

bool checkChild(ushort tagID, ushort childID);

extern const unsigned short tagPriority[];
extern const tagStatus endTag[];

} //namespace DOM
#endif
