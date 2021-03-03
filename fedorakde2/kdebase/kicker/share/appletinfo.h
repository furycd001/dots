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

#ifndef __appletinfo_h__
#define __appletinfo_h__

#include <qstring.h>

class AppletInfo
{
 public:

  AppletInfo(const QString& desktopFile = QString::null);

  QString name() const { return _name; }
  QString comment() const { return _comment; }
  QString icon() const { return _icon; }
  QString library() const { return _lib; }
  QString desktopFile() const { return _desktopFile; }

  QString configFile() const { return _configFile; }

  bool isUniqueApplet() const { return _unique; }

  void setConfigFile(QString cf) { _configFile = cf; } // FIXME: check if it exists

 protected:
  void setName(QString name) { _name = name; }
  void setComment(QString comment) { _comment = comment; }
  void setIcon(QString icon) { _icon = icon; }
  void setLibrary(QString lib) { _lib = lib; }
  void setIsUnique(bool u) { _unique = u; }

 private:
  QString _name, _comment, _icon, _lib, _desktopFile, _configFile;
  bool    _unique;
};

#endif
