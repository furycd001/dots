/*****************************************************************

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

#ifndef __browser_mnu_h__
#define __browser_mnu_h__

#include <qmap.h>
#include "base_mnu.h"

class PanelBrowserMenu : public PanelMenu
{
    Q_OBJECT

public:
    PanelBrowserMenu(QString path, QWidget *parent = 0, const char *name = 0, int startid = 0);

    void append(const QPixmap &pixmap, const QString &title, const QString &filename, bool mimecheck);
    void append(const QPixmap &pixmap, const QString &title, PanelBrowserMenu *subMenu);

public slots:
    void initialize();

protected slots:
    void slotExec(int id);
    void slotOpenTerminal();
    void slotOpenFileManager();
    void slotMimeCheck();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    void initIconMap();

    QPoint             _lastpress;
    QMap<int, QString> _filemap;
    QMap<int, bool>    _mimemap;
    QTimer            *_mimecheckTimer;

    int                _startid;
    bool               _showhidden;
    int                _maxentries;

    static QMap<QString, QPixmap> *_icons;
};

#endif
