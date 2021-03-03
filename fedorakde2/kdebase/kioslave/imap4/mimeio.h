/***************************************************************************
                          mimeio.h  -  description
                             -------------------
    begin                : Wed Oct 25 2000
    copyright            : (C) 2000 by Sven Carstens
    email                : s.carstens@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MIMEIO_H
#define MIMEIO_H

#include <qcstring.h>
#include <qfile.h>

/**
  *@author Sven Carstens
  */

class mimeIO
{
public:
  mimeIO ();
  virtual ~ mimeIO ();

  virtual int outputLine (const QCString &);
  virtual int outputMimeLine (const QCString &);
  virtual int inputLine (QCString &);
  virtual int outputChar (char);
  virtual int inputChar (char &);

  void setCRLF (const char *);

protected:
    QCString theCRLF;
};

class mimeIOQFile:public mimeIO
{
public:
  mimeIOQFile (const QString &);
    virtual ~ mimeIOQFile ();
  virtual int outputLine (const QCString &);
  virtual int inputLine (QCString &);

protected:
    QFile myFile;
};

class mimeIOQString:public mimeIO
{
public:
  mimeIOQString ();
  virtual ~ mimeIOQString ();
  virtual int outputLine (const QCString &);
  virtual int inputLine (QCString &);
  QString getString ()
  {
    return theString;
  };
  void setString (const QString & _str)
  {
    theString = _str;
  };

protected:
  QString theString;
};

#endif
