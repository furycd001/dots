/*************************************************************************

    windowdrag.cpp  - The windowDrag object, used to drag windows across
       	                desktops
    Copyright (C) 1998,99,2000  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to larrosa@kde.org

*************************************************************************/
#include "windowdrag.h"
#include <stdio.h>

PagerWindowDrag::PagerWindowDrag(WId w,int deltax,int deltay, int origdesk,QWidget *parent)
    : QStoredDrag("application/x-kpager",parent,"windowdrag")
{
    char *tmp=new char[200];
    sprintf(tmp,"%d %d %d %d",w,deltax,deltay,origdesk);
    QByteArray data(strlen(tmp)+1);
    data.assign(tmp,strlen(tmp)+1);

    setEncodedData(data);
}

PagerWindowDrag::~PagerWindowDrag()
{
}

bool PagerWindowDrag::canDecode (QDragMoveEvent *e)
{
    return e->provides("application/x-kpager");
}

bool PagerWindowDrag::decode( QDropEvent *e, WId &w,int &deltax,int &deltay,int &origdesk)
{
    QByteArray data=e->data("application/x-kpager");
    if (data.size())
	{
	    char *tmp=data.data();
	    sscanf(tmp,"%d %d %d %d",&w,&deltax,&deltay,&origdesk);
	    e->accept();
	    return TRUE;
	}
    return FALSE;
}
