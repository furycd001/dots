/***************************************************************************
                          CXdmcp_c.cpp  -  description
                             -------------------
    begin                : Fri Dec 8 2000
    copyright            : (C) 2000 by Oswald Buddenhagen
    email                : ossi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "CXdmcp_c.h"

int _XdmcpARRAY8Equal(ARRAY8 *a, ARRAY8 *b)
{ return XdmcpARRAY8Equal (a, b); }

int _XdmcpAllocARRAY8 (ARRAY8 *a, int b)
{ return XdmcpAllocARRAY8 (a, b); }

void _XdmcpDisposeARRAY8(ARRAY8 *a)
{ XdmcpDisposeARRAY8 (a); }

int _XdmcpFill(int a, XdmcpBuffer *b, char *c, int *d)
{ return XdmcpFill (a, b, c, d); }

#ifdef XIMStringConversionRetrival
int _XdmcpFlush(int a, XdmcpBuffer *b, void *c, int d)
#else
int _XdmcpFlush(int a, XdmcpBuffer *b, char *c, int d)
#endif
{ return XdmcpFlush (a, b, c, d); }

int _XdmcpReadARRAY8(XdmcpBuffer *a, ARRAY8 *b)
{ return XdmcpReadARRAY8 (a, b); }

int _XdmcpReadHeader(XdmcpBuffer *a, XdmcpHeader *b)
{ return XdmcpReadHeader (a, b); }

int _XdmcpWriteARRAY8(XdmcpBuffer *a, ARRAY8 *b)
{ return XdmcpWriteARRAY8 (a, b); }

int _XdmcpWriteARRAYofARRAY8(XdmcpBuffer *a, ARRAYofARRAY8 *b)
{ return XdmcpWriteARRAYofARRAY8 (a, b); }

int _XdmcpWriteCARD16(XdmcpBuffer *a, CARD16 b)
{ return XdmcpWriteCARD16 (a, b); }

int _XdmcpWriteHeader(XdmcpBuffer *a, XdmcpHeader *b)
{ return XdmcpWriteHeader (a, b); }

