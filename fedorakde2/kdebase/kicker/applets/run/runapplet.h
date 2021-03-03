/*****************************************************************

Copyright (c) 2000 Matthias Elter

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

#ifndef __runapplet_h__
#define __runapplet_h__

#include <qstring.h>
#include <kpanelapplet.h>

class QLabel;
class QHBox;
class QPushButton;
class KHistoryCombo;
class KURIFilterData;

class RunApplet : public KPanelApplet
{
    Q_OBJECT

public:
    RunApplet(const QString& configFile, Type t = Stretch, int actions = 0,
	      QWidget *parent = 0, const char *name = 0);
    virtual ~RunApplet();

    int widthForHeight(int height) const;
    int heightForWidth(int width) const;

protected:
    void resizeEvent(QResizeEvent*);
    void popupDirectionChange(KPanelApplet::Direction);
    bool eventFilter( QObject *, QEvent * );

protected slots:
    void run_command(const QString&);
    void popup_combo();
    void setButtonText();

private:
    KHistoryCombo  *_input;
    KURIFilterData *_filterData;
    QLabel         *_label;
    QPushButton    *_btn;
    QHBox          *_hbox;
};

#endif
