/*
    KSysGuard, the KDE System Guard
   
	Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>
    
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

	$Id: Command.h,v 1.12 2001/05/28 16:38:45 cschlaeg Exp $
*/

#ifndef _Command_h_
#define _Command_h_

void print_error(const char *, ...);
void log_error(const char *, ...);

typedef void (*cmdExecutor)(const char*);

extern int ReconfigureFlag;
extern int CheckSetupFlag;

void initCommand(void);

void exitCommand(void);

void registerCommand(const char* command, cmdExecutor ex);

void removeCommand(const char* command);

void registerMonitor(const char* command, const char* type, cmdExecutor ex,
					 cmdExecutor iq);

void removeMonitor(const char* command);

void executeCommand(const char* command);

void printMonitors(const char* cmd);

void printTest(const char* cmd);

void exQuit(const char* cmd);

#endif
