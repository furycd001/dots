    /*

    Front-end to Lilo's boot options
    $Id: liloinfo.h,v 1.3 2001/06/10 20:33:48 ossi Exp $

    Copyright (C) 1999 Stefan van den Oord <oord@cs.utwente.nl>
    Copyright (C) 2001 Oswald Buddenhagen <ossi@kde.org>

    NOTE: this is a heavily cut-down version of the liloinfo class.
    Check out kde 2.1 for the full version.

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

    */

#ifndef LILOINFO_H
#define LILOINFO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qfileinfo.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kapp.h>
#include <kprocess.h>

#include <iostream>

/*
 * Errors:
 *      0 = No error
 *     -1 = Lilo Error (description in getErrorDescription)
 *     -2 = No next option was defined
 *     -3 = Next option does not occur in option list
 *     -4 = Hack is not enabled
 *     -5 = Attempt to write invalid next boot option
 *     -6 = Failed to run Lilo
 *     -7 = Lilo location is a non-existing file
 *     -8 = Bootmap location is a non-existing file
 */

class LiloInfo : QObject
{
	Q_OBJECT

	public:
		LiloInfo ( QString lilolocation,
		           QString bootmaplocation,
		           bool enableHack = true,
		           bool enableDebug = true );
		~LiloInfo();

		int getBootOptions ( QStringList *bootOptions );
		int getDefaultBootOptionIndex();
		int setNextBootOption ( int nextBootOptionIndex );

	private:
		QString liloloc, bootmaploc;
		bool optionsAreRead;               // true as soon as lilo -q is called
		bool nextOptionIsRead;          // true as soon as lilo -q -v is called
		QStringList *options;
		int indexDefault;             // index in options of the default option
		int indexNext;                   // index in options of the next option
		bool debug, useHack;
		int error;
		QString liloErrorString;      // is set when lilo resulted in an error;
		                             // getErrorDescription() then returns this
		                                               // string and clears it.

		bool getOptionsFromLilo();
		bool getNextOptionFromLilo();

	private slots:
		void getOptionsFromStdout ( KProcess *, char *buffer, int len );
		void getNextOptionFromStdout ( KProcess *, char *buffer, int len );
		void processStderr ( KProcess *, char *buffer, int len );
};

#endif // LILOINFO_H
