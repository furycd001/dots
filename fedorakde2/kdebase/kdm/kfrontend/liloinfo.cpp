    /*

    Front-end to Lilo's boot options
    $Id: liloinfo.cpp,v 1.9 2001/07/14 16:29:53 ossi Exp $

    Copyright (C) 1999 Stefan van den Oord <oord@cs.utwente.nl>
    Copyright (C) 2001 Oswald Buddenhagen <ossi@kde.org>

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

#ifdef __linux__

#include <klocale.h>

#include "liloinfo.moc"
using namespace std;

/*
 * The constructor. Initialize the internal variables and copy the parameter strings.
 */
LiloInfo::LiloInfo ( QString lilolocation,
		     QString bootmaplocation,
		     bool enableHack,
		     bool enableDebug )
{
	debug = enableDebug;
	useHack = enableHack;

	if ( debug ) cerr << "[LiloInfo] Constructor" << endl;

	// Copy the parameter strings
	liloloc = lilolocation.copy();
	bootmaploc = bootmaplocation.copy();

	if ( !QFileInfo ( liloloc ).exists() )
		error = -7;
	else if ( !QFileInfo ( bootmaploc ).exists() )
		error = -8;
	else
		error = 0;
	liloErrorString = QString::null;

	// The options array will make deep copies of its strings
	options = new QStringList();

	// Initialize the remaining internal variables
	optionsAreRead = false;
	nextOptionIsRead = false;
	indexDefault = -1;
	indexNext = -1;
}

/*
 * The destructor.
 */
LiloInfo::~LiloInfo()
{
	// Delete the options array
	delete options;

	if ( debug ) cerr << "[LiloInfo] Destructor" << endl;
}

/*
 * Make a copy of the internal options array in the QStringList parameter.
 * Note that the bootOptions parameter must be allocated before calling this method.
 * I don't know how to handle the situation in which it is not. Exception handling
 * maybe?
 */
int LiloInfo::getBootOptions ( QStringList *bootOptions )
{
	if ( debug ) cerr << "[LiloInfo] Get boot options...";

	// If one of the two locations is invalid, return
	if ( error == -7 || error == -8 ) return error;

	// Reset the error state
	error = 0;
	liloErrorString = QString::null;

	// First, clear the bootOptions array
	bootOptions->clear();

	// If the options are not already read from the Lilo output, do it now
	if ( !optionsAreRead )
		if ( !getOptionsFromLilo() )
	{
		if ( debug ) cerr << "done (error = " << error << ")." << endl;

		return error;
	}

	// Copy options into bootOptions
	for ( unsigned int i = 0; i < options->count(); i++ )
		bootOptions->append ( *options->at(i) );

	if ( debug ) cerr << "done." << endl;

	// Return the error code that was set by getOptionsFromLilo()
	return error;
}

/*
 * Return the index of the default option (or an error code)
 */
int LiloInfo::getDefaultBootOptionIndex()
{
	if ( debug ) cerr << "[LiloInfo] Get default boot option...";

	// If one of the two locations is invalid, return
	if ( error == -7 || error == -8 ) return error;

	// Reset the error state
	error = 0;
	liloErrorString = QString::null;

	// If the options are not already read from the Lilo output, do it now
	if ( !optionsAreRead )
		if ( !getOptionsFromLilo() )
	{
		if ( debug ) cerr << "done (error = " << error << ")." << endl;

		return error;
	}

	// If an error occured, return it. Otherwise, return the index.
	if ( error != 0 )
	{
		if ( debug ) cerr << "done (error = " << error << ")." << endl;

		return error;
	}
	else
	{
		if ( debug ) cerr << "done." << endl;

		return indexDefault;
	}
}

/*
 * The procedure that actually calls Lilo
 */
bool LiloInfo::getOptionsFromLilo()
{
	if ( debug ) cerr << "[LiloInfo] Get options from Lilo..." << endl;

	KProcess liloproc;

	// Reset the optionsAreRead variable
	optionsAreRead = false;

	// Reset the error state
	error = 0;
	liloErrorString = QString::null;

	// Create the process handle. The command line will be something like
	// "/sbin/lilo -q -m /boot/map".
	liloproc << liloloc;
	liloproc << QString::fromLatin1("-q"); 
        liloproc << QString::fromLatin1("-m") << bootmaploc;

	// Connect to the standard output and error signals. Lilo's output will be received
	// by getOptionsFromStdout() and by receivedStderr(). The former receives the boot
	// options, the latter the error messages.
	connect ( &liloproc, SIGNAL(receivedStdout(KProcess *, char *, int)),
	          this, SLOT(getOptionsFromStdout(KProcess *, char *, int)) );
	connect ( &liloproc, SIGNAL(receivedStderr(KProcess *, char *, int)),
	          this, SLOT(processStderr(KProcess *, char *, int)) );

	// Run the command. It is run in blocking mode, which means that the calling process
	// blocks until Lilo is finished. Furthermore, we want to receive both Stdout and Stderr.
	if ( !liloproc.start ( KProcess::Block,
	                       (KProcess::Communication) ( KProcess::Stdout | KProcess::Stderr )
	                     )
	   )
	{
		error = -6;

		if ( debug ) cerr << "[LiloInfo] Done (error = " << error << ")." << endl;

		return false;
	}

	// If the error is zero, reading the options is succeeded. (The error is set by
	// receivedStderr().)
	optionsAreRead = ( error == 0 );

	if ( debug ) cerr << "[LiloInfo] Done." << endl;

	// Return whether it succeeded
	return optionsAreRead;
}

/*
 * Also calls Lilo, but this time to find out the next boot option. This is done by
 * running Lilo in verbose mode. The actual next boot option string is parsed by a
 * sed script.
 */
bool LiloInfo::getNextOptionFromLilo()
{
	if ( debug ) cerr << "[LiloInfo] Get next option from Lilo..." << endl;

	KShellProcess liloproc;

	// Reset the optionsAreRead variable
	nextOptionIsRead = false;

	// Reset the error state
	error = -2;
	liloErrorString = QString::null;

	// Create the process handle
	liloproc << liloloc;
	liloproc << QString::fromLatin1("-q") << QString::fromLatin1("-v") << QString::fromLatin1("-m") << bootmaploc << QString::fromLatin1("|") << QString::fromLatin1("sed") << QString::fromLatin1("-n") << QString::fromLatin1("'s/\"[^\"]*$//;/Default boot command/s/.*\"//p'") << QString::fromLatin1("|") << QString::fromLatin1("sed 's/ .*//'");

	// Connect to the standard output and error signals. This time standard output is
	// received by getNextOptionFromStdout().
	connect ( &liloproc, SIGNAL(receivedStdout(KProcess *, char *, int)),
	          this, SLOT(getNextOptionFromStdout(KProcess *, char *, int)) );
	connect ( &liloproc, SIGNAL(receivedStderr(KProcess *, char *, int)),
	          this, SLOT(processStderr(KProcess *, char *, int)) );

	// Run the command, again in blocking mode and with Stdout and Stderr.
	if ( !liloproc.start ( KProcess::Block,
	                       (KProcess::Communication) ( KProcess::Stdout | KProcess::Stderr )
	                     )
	   )
	{
		error = -6;

		if ( debug ) cerr << "[LiloInfo] Done (error = " << error << ")." << endl;

		return false;
	}

	// If the error is zero, we succeeded.
	nextOptionIsRead = ( error == 0 );

	if ( debug ) cerr << "[LiloInfo] Done." << endl;

	// Return wether we succeeded.
	return nextOptionIsRead;
}

/*
 * Parse the options from the standard output. For each option the standard output contains
 * a line. The option is at the beginning of the line, possibly followed by whitespace. If
 * the end of a line is an asterix, the option on that line is the default option. Example:
 *
 * Line 1: "Linux           *" <-- default option
 * Line 2: "Win98            "
 * Line 3: "DOS              "
 *
 * Note that probably the default option will always be on the first line, but I don't use
 * that, so it that changes, this will keep working.
 */
void LiloInfo::getOptionsFromStdout ( KProcess *, char *buffer, int len)
{
	bool ready = false, // indicates whether the entire input has been parsed
		curIsDefault = false; // true if the current line contains the default option
	int curPos, // the index in the input string, indicating where we are reading
		nextPos; // the position of the next line in the input string
	QString line, // the current line of the input string
		defaultOption; // contains the default option

	// Copy the received string into a QString, and add a '\n'.
	buffer[len ? len-1 : 0] = '\0';
	QString outString = QString::fromLatin1(buffer) + '\n';

	if ( debug ) cerr << "[LiloInfo]     Received on standard output: \"" << outString.latin1() << "\"" << endl;

	// Parse the output string. The boolean 'ready' indicates whether the entire input
	// has been parsed. 'curPos' is the index in the input string, the position at which
	// we are reading. 'nextPos' is the start of the next line.
	curPos = 0;
	while ( !ready )
	{
		// Compute the index of the first char of the next line
		nextPos = outString.find ( '\n', curPos ) + 1;

		// The current line (without '\n'):
		line = outString.mid ( curPos, nextPos - 1 - curPos );

		// If the current line ends with '*', it is the default option
		if ( line.at ( line.length() - 1 ) == '*' )
		{
			// We found the default option
			curIsDefault = true;

			// Remove the '*' from the input line
			line.truncate ( line.length() - 1 );
		}

		// Strip the trailing spaces from the line
		line = line.stripWhiteSpace();

		// If the current line contains the default option, set the defaultOption string
		// This is done because the options array will be sorted, which changes the index
		// of the default option.
		if ( curIsDefault )
		{
			// Copy the line into defaultOption and reset curIsDefault for the next iteration
			defaultOption = line.copy();
			curIsDefault = false;

			if ( debug ) cerr << "[LiloInfo]     Default option found: '" << defaultOption.latin1() << "'" << endl;
		}

		// Add the string to the list of options (keeping it sorted)
		options->append( line );

		if ( debug ) cerr << "[LiloInfo]     Option added: '" << line.latin1() << "'" << endl;

		// Proceed to the next line
		curPos = nextPos;

		// If we are at the end of the string, the job is done
		if ( curPos == (int)outString.length() ) ready = true;
	}
	options->sort();

	// The options array is filled now. We have to find the defaultOption string
	// in the (sorted) array in order to set the indexDefault to the right value.
	indexDefault = options->findIndex ( defaultOption );
}

/*
 * Parse the output of Lilo to find the next option. The shell command, i.e. the output
 * of Lilo piped through sed scripts, is either empty or contains the next boot option.
 */
void LiloInfo::getNextOptionFromStdout ( KProcess *, char *buffer, int len )
{
	buffer[len ? len-1 : 0] = '\0';
	QString nextOption = QString::fromLatin1(buffer);

	if ( debug ) cerr << "[LiloInfo]     Received on standard output: \"" << nextOption.latin1() << "\"" << endl;

	if ( !nextOption.isEmpty() )
	{
		// If the input is not empty, try to find the string in the options array.
		// Because Lilo works case insensitive, we compare the strings after making
		// them upper case.
		error = -3;
		QString upperNextOption = nextOption.upper();
		for ( unsigned int i = 0; i < options->count(); i++ )
		{
			if ( (*options->at(i)).upper() == upperNextOption )
			{
				// Found the option in the options array. Set the index and reset the
				// error code.
				indexNext = i;
				error = 0;
				break;
			}
		}
	}
	else
	{
		// The input was empty, so there is no next boot option
		error = -2;
	}
}

/*
 * Process the strings Lilo writes to standard error. Because we manually provide the boot
 * map file, Lilo says "Ignoring entry 'map'". This is not an error, so we skip that.
 * BTW: we assume that this ignore message is always the first line on stderr.
 */
void LiloInfo::processStderr ( KProcess *, char *buffer, int len )
{
	// Copy the received string into a QString
	buffer[len ? len-1 : 0] = '\0';
	QString errString = QString::fromLatin1(buffer) + '\n';

	if ( debug ) cerr << "[LiloInfo]     Received on standard error: \"" << errString.latin1() << "\"" << endl;

	// If the string starts with "Ignoring entry ...", remove the first line
	if ( errString.left ( 8 ) == QString::fromLatin1("Ignoring") )
		errString.remove(0, errString.find ( '\n' ) + 1 );

	if ( !errString.isEmpty() )
	{
		// If there is something left of the received string, set the global error string
		liloErrorString = errString;
		error = -1;

		if ( debug ) cerr << "[LiloInfo]     Lilo Error: " << liloErrorString.latin1() << endl;
	}
}

/*
 * Set the boot option using an index.
 */
int LiloInfo::setNextBootOption ( int nextBootOptionIndex)
{
	// If one of the two locations is invalid, return
	if ( error == -7 || error == -8 ) return error;

	if ( debug ) cerr << "[LiloInfo] Set next boot option..." << endl;

	// If the options are not already read from the Lilo output, do it now
	if ( !optionsAreRead )
		if ( !getOptionsFromLilo() )
	{
		if ( debug ) cerr << "[LiloInfo] Done (error = " << error << ")." << endl;

		return error;
	}

	// Check that the index is within the right range
	if ( nextBootOptionIndex < 0 || nextBootOptionIndex >= (int)options->count() )
	{
		error = -5;
		if ( debug ) cerr << "[LiloInfo] Done (error = " << error << ")." << endl;

		return error;
	}

	KProcess liloproc;

	// Reset the error state
	error = 0;
	liloErrorString = QString::null;

	// The command line will look like "/sbin/lilo -m /boot/map -R Linux"
	liloproc << liloloc;
	liloproc << QString::fromLatin1("-m") << bootmaploc 
	   << QString::fromLatin1("-R") << *options->at ( nextBootOptionIndex );

	// Connect to standard error. We don't expect standard output, so we don't connect to
	// that.
	connect ( &liloproc, SIGNAL(receivedStderr(KProcess *, char *, int)), this, SLOT(processStderr(KProcess *, char *, int)) );

	// Run Lilo, again in blocking mode, but this time only receiving standard output.
	// Which we should not get, because we checked that the string was correct already.
	liloproc.start ( KProcess::Block,
	                 (KProcess::Communication) ( KProcess::Stderr ) );

	if ( debug ) cerr << "[LiloInfo] Done." << endl;

	// Return the error code
	return error;
}

#endif
