/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qapplication.h"
#include "qprocess.h"

#include <stdio.h>
#include <stdlib.h>


/*!
  \class QProcess qprocess.h

  \brief The QProcess class provides means to start external programs and
  control their behavior.

  \ingroup ?

  A QProcess allows you to start a external program, control its input and
  output, etc.
*/

/*!
  Constructs a QProcess.
*/
QProcess::QProcess( QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QProcessPrivate( this );
}

/*!
  Constructs a QProcess with the given command (but does not start it).
*/
QProcess::QProcess( const QString& com, QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QProcessPrivate( this );
    setCommand( com );
}

/*!
  Constructs a QProcess with the given command and arguments (but does not
  start it).
*/
QProcess::QProcess( const QString& com, const QStringList& args, QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QProcessPrivate( this );
    setCommand( com );
    setArguments( args );
}

/*!
  Destructor; if the process is running it is NOT terminated! Stdin, stdout and
  stderr of the process are closed.
*/
QProcess::~QProcess()
{
    delete d;
}


/*!
  Set the command that should be executed.
*/
void QProcess::setCommand( const QString& com )
{
    d->command = com;
}

/*!
  Set the arguments for the command. Previous set arguments will be deleted
  first.
*/
void QProcess::setArguments( const QStringList& args )
{
    d->arguments = args;
}

/*!
  Add a argument to the end of the existing list of arguments.
*/
void QProcess::addArgument( const QString& arg )
{
    d->arguments.append( arg );
}

/*!
  Set a working directory in which the command is executed.
*/
void QProcess::setWorkingDirectory( const QDir& dir )
{
    d->workingDir = dir;
}


/*!
  \fn bool QProcess::start()

  Start the program.

  Return TRUE on success, otherwise FALSE.
*/
/*!
  \fn bool QProcess::hangUp()

  Ask the process to terminate. If this does not work you can try \l kill()
  instead.

  Return TRUE on success, otherwise FALSE.
*/
/*!
  \fn bool QProcess::kill()

  Terminate the process. This is not a safe way to end a process; you should
  try \l hangUp() first and use this function only if it failed.

  Return TRUE on success, otherwise FALSE.
*/
/*!
  \fn bool QProcess::isRunning()

  Return TRUE if the process is running, otherwise FALSE.
*/


/*!
  Return TRUE if the process has exited normally, otherwise FALSE.
*/
bool QProcess::normalExit()
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return FALSE;
    else
	return d->exitNormal;
}

/*!
  Return the exit status of the process. This value is only valid if
  \l normalExit() is TRUE.
*/
int QProcess::exitStatus()
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return 0;
    else
	return d->exitStat;
}


/*!
  \fn void QProcess::dataStdout( const QByteArray& buf )

  This signal is emitted if the process wrote data to stdout.
*/
/*!
  \fn void QProcess::dataStdout( const QString& buf )

  This signal is emitted if the process wrote data to stdout.
*/
/*!
  \fn void QProcess::dataStderr( const QByteArray& buf )

  This signal is emitted if the process wrote data to stderr.
*/
/*!
  \fn void QProcess::dataStderr( const QString& buf )

  This signal is emitted if the process wrote data to stderr.
*/
/*!
  \fn void QProcess::processExited()

  This signal is emitted if the process has exited.
*/
/*!
  \fn void QProcess::wroteStdin()

  This signal is emitted if the data send to stdin (via \l dataStdin()) was
  actually read by the process.
*/


/*!
  \fn void QProcess::dataStdin( const QByteArray& buf )
  Write data to the stdin of the process. The process may or may not read this
  data. If the data gets read, the signal \l wroteStdin() is emitted.
*/
/*!
  Write data to the stdin of the process. The string is handled as a text. So
  what is written to the stdin is the \l QString::latin1(). The process may or
  may not read this data. If the data gets read, the signal \l wroteStdin() is
  emitted.
*/
void QProcess::dataStdin( const QString& buf )
{
    QByteArray bbuf;
    bbuf.duplicate( buf.latin1(), buf.length() );
    dataStdin( bbuf );
}

/*!
  \fn void QProcess::closeStdin( )

  Close stdin.
*/


/*!
  \fn void QProcess::socketRead( int fd )

  The process has output data to either stdout or stderr.
*/

/*!
  \fn void QProcess::socketWrite( int fd )

  The process tries to read data from stdin.
*/
