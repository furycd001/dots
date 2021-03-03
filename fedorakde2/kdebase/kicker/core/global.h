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

#ifndef __pglobal_h__
#define __pglobal_h__

#include <qmap.h>

enum Position { Left = 0, Right, Top, Bottom };
enum Direction { dUp = 0, dDown, dLeft, dRight };
enum Size {Tiny=0, Small, Normal, Large};

class Panel;
class KickerPluginManager;
class KGlobalAccel;
class KWinModule;
class ExtensionManager;

class PGlobal
{
public:
    static int sizeValue(Size s);
    static Panel *panel;
    static KWinModule *kwin_module;
    static KickerPluginManager *pluginmgr;
    static ExtensionManager *extensionManager;
    static KGlobalAccel *globalKeys;
};

#endif // __pglobal_h__
