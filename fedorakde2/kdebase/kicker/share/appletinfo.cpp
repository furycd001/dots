/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/              
                         
#include <qfileinfo.h>                                         
#include <kdesktopfile.h>
#include <kapp.h>

#include "appletinfo.h"

AppletInfo::AppletInfo(const QString& deskFile)
  : _name (QString::null)
  , _comment (QString::null)
  , _icon (QString::null)
  , _lib (QString::null)
  , _desktopFile(QString::null)
  , _configFile(QString::null)
  , _unique(true)
{
  QFileInfo fi(deskFile);
  _desktopFile = fi.fileName();

  KDesktopFile df(deskFile);

  // set the appletssimple attributes
  setName(df.readName());
  setComment(df.readComment());
  setIcon(df.readIcon());

  // library
  setLibrary(df.readEntry("X-KDE-Library"));

  // is it a unique applet?
  setIsUnique(df.readBoolEntry("X-KDE-UniqueApplet", false));
 
  // generate a config file base name from the library name
  _configFile = _lib;
  if(_unique)
    _configFile = _configFile.remove(0, 3).lower() + "rc";
  else
    {
      // FIXME: we should check if the file already exists to be 100%
      //        sure it's unique.
      _configFile = _configFile.remove(0, 3).lower();
      _configFile += "_";
      _configFile += kapp->randomString(20).lower();
      _configFile += "_rc";
    }
}
