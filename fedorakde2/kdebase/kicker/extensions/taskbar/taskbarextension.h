#/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

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

#ifndef __taskbarextension_h__
#define __taskbarextension_h__

#include <kpanelextension.h>

class TaskBar;
class WindowListButton;
class ScrollButton;

class TaskBarExtension : public KPanelExtension
{
    Q_OBJECT

public:
    TaskBarExtension( const QString& configFile, Type t = Normal,
		      int actions = 0, QWidget *parent = 0, const char *name = 0 );
    ~TaskBarExtension();

    QSize sizeHint( Position, QSize maxSize ) const;
    Position preferedPosition() const { return Bottom; }

protected slots:
    void containerCountChanged();
    void enableScrollButtons( bool );

protected:
    void positionChange( Position );
    void resizeEvent( QResizeEvent* );
    void preferences();
    void configure();
    void reLayout();

private:
    bool		  showWindowListButton;
    bool		  showScrollButtons;
    TaskBar 		* taskBar;
    WindowListButton 	* windowListButton;
    ScrollButton        * leftScrollButton;
    ScrollButton        * rightScrollButton;
};

#endif
