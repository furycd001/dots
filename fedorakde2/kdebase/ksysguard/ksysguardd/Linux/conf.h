/*
    KSysGuard, the KDE System Guard
	   
    Copyright (c) 2001 Tobias Koenig <tokoe82@yahoo.de>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "ccont.h"

#ifndef _conf_h_
#define _conf_h_

typedef struct {
	char *name;
	char *path;
} ConfigLogFile;

void parseConfigFile(const char *filename);

int sensorAvailable(const char* sensor);

#endif
