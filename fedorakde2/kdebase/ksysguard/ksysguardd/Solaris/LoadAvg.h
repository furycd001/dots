/*
    KTop, the KDE Task Manager
   
	Copyright (c) 1999 Chris Schlaeger <cs@kde.org>

	Solaris support by Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
    
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

	$Id: LoadAvg.h,v 1.1 2001/03/06 11:09:47 cschlaeg Exp $
*/

#ifndef _LoadAvg_h_
#define _LoadAvg_h_

#define LOAD(a) ((double)(a) / (1 << 8 ))

void initLoadAvg(void);
void exitLoadAvg(void);

int updateLoadAvg(void);

void printLoadAvg1( const char *cmd );
void printLoadAvg1Info( const char *cmd );
void printLoadAvg5( const char *cmd );
void printLoadAvg5Info( const char *cmd );
void printLoadAvg15( const char *cmd );
void printLoadAvg15Info( const char *cmd );

#endif /* _LoadAvg_h_ */
