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

#ifndef PANEL_CLIENTMENU_H
#define PANEL_CLIENTMENU_H

#include <qstringlist.h>
#include <qpopupmenu.h>

#include <dcopobject.h>


class PanelKMenu;

// Classes to handle client application menus. Used by PanelKButton, which
// also manages the toplevel K Button Menu.

/**
 * Small additions to QPopupMenu to contain data we need for DCop handling
 */
class KickerClientMenu : public QPopupMenu, DCOPObject
{
    Q_OBJECT
public:
    KickerClientMenu( QWidget *parent=0, const char *name=0);
    ~KickerClientMenu();

    // dcop exported
    void clear();
    void insertItem( QPixmap icon, QString text, int id );
    void insertItem( QString text, int id );

    QCString insertMenu( QPixmap icon, QString test, int id );	

    // dcop signals:
    //     void activated(int)

    void connectDCOPSignal( QCString signal, QCString appId, QCString objId );

    // dcop internal
    virtual bool process(const QCString &fun, const QByteArray &data,
			 QCString &replyType, QByteArray &reply);

protected slots:
    void slotActivated(int id);

private:
    QCString app, obj; // for the signal

    // for the panel menu, internal
    friend class PanelKMenu;
    QString text;
    QPixmap icon;

    // for the KickerClientMenu, internal
    friend class KickerMenuManager;
    int idInParentMenu;
    QCString createdBy;
};

#endif
