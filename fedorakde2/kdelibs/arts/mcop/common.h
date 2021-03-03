    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include "buffer.h"
#include "type.h"
#include "dispatcher.h"
#include "object.h"
#include "connection.h"
#include "objectmanager.h"
#include "idlfilereg.h"
#include "asyncstream.h"
#include "mcoputils.h"
#include "core.h"
#include "anyref.h"

namespace Arts {
/* some marshalling helpers */

class Buffer;

template<class T>
void readTypeSeq(Buffer& stream, std::vector<T>& sequence);

template<class T>
void writeTypeSeq(Buffer& stream, const std::vector<T>& sequence);

template<class T>
void writeObject(Buffer& stream, T* object);

template<class T>
void readObject(Buffer& stream, T*& result);

template<class T>
void readObjectSeq(Buffer& stream, std::vector<T>& sequence);

template<class T>
void writeObjectSeq(Buffer& stream, const std::vector<T>& sequence);

#ifndef MCOPBYTE_DEFINED
#define MCOPBYTE_DEFINED
typedef unsigned char mcopbyte;
#endif

};
#endif
