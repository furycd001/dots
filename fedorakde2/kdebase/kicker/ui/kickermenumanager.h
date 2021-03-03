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

#ifndef KICKER_MENU_MANAGER_H
#define KICKER_MENU_MANAGER_H

#include <qlist.h>
#include <dcopobject.h>

class PanelKMenu;
class KickerClientMenu;

/**
 * The factory for menus created by other applications. Also the owner of these menus.
 */
class KickerMenuManager : public QObject, DCOPObject
{
    Q_OBJECT
public:
    KickerMenuManager( PanelKMenu* menu, QObject *parent=0, const char *name=0 );
    ~KickerMenuManager();

    // dcop exported

    QCString createMenu( QPixmap icon, QString text );
    void removeMenu( QCString menu );

    // dcop internal
    bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &reply);


protected slots:
    void applicationRemoved( const QCString& );
signals:
    void popupKMenu(int x, int y);
protected:
    PanelKMenu* panelmenu;
    QList<KickerClientMenu> clientmenus;
};

#endif
