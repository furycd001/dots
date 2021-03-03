/***************************************************************************
                          CXdmcp_c.h  -  description
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

#ifndef CXDMCP_C_H
#define CXDMCP_C_H

#include <dm.h>

extern int _XdmcpARRAY8Equal(ARRAY8 *, ARRAY8 *);
extern int _XdmcpAllocARRAY8 (ARRAY8 *, int);
extern void _XdmcpDisposeARRAY8(ARRAY8 *);
extern int _XdmcpFill(int, XdmcpBuffer *, char *, int *);
#ifdef XIMStringConversionRetrival
extern int _XdmcpFlush(int, XdmcpBuffer *, void *, int);
#else
extern int _XdmcpFlush(int, XdmcpBuffer *, char *, int);
#endif
extern int _XdmcpReadARRAY8(XdmcpBuffer *, ARRAY8 *);
extern int _XdmcpReadHeader(XdmcpBuffer *, XdmcpHeader *);
extern int _XdmcpWriteARRAY8(XdmcpBuffer *, ARRAY8 *);
extern int _XdmcpWriteARRAYofARRAY8(XdmcpBuffer *, ARRAYofARRAY8 *);
extern int _XdmcpWriteCARD16(XdmcpBuffer *, CARD16);
extern int _XdmcpWriteHeader(XdmcpBuffer *, XdmcpHeader *);

#endif
