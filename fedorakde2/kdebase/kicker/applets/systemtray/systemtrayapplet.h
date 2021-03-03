/*****************************************************************

Copyright (c) 1996-2001 the kicker authors. See file AUTHORS.

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

#ifndef __systemtrayapplet_h__
#define __systemtrayapplet_h__

#include <kpanelapplet.h>

#include <qlist.h>
#include <qxembed.h>
#include <qpushbutton.h>

class KWinModule;

// Hacky class that is the same as QXEmbed, but does not destroy its
// window in the destructor. It would be nice if Qt would have support
// for this.
class KXEmbed: public QXEmbed
{
public:
    KXEmbed(QWidget *parent=0L, const char *name=0L, WFlags f=0)
        : QXEmbed(parent, name, f)
    { setAutoDelete(false); }
    virtual ~KXEmbed() {}
};

class TrayButton : public QPushButton
{
    Q_OBJECT

public:
    TrayButton(QWidget* parent, const char* name)
        : QPushButton(parent, name) { }
    virtual ~TrayButton() {}

protected:
    void drawButton(QPainter *p);
};

class SystemTrayApplet : public KPanelApplet
{
    Q_OBJECT

public:

    SystemTrayApplet(const QString& configFile, Type t = Normal, int actions = 0,
                     QWidget *parent = 0, const char *name = 0);
    ~SystemTrayApplet();

    int widthForHeight(int h) const;
    int heightForWidth(int w) const;
		virtual bool eventFilter( QObject *o, QEvent *e );

protected:
    void resizeEvent( QResizeEvent* );

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void propagateMouseEvent(QMouseEvent*);

protected slots:
    void systemTrayWindowAdded( WId );
    void updateTrayWindows();
    void layoutTray();

    void lock();
    void logout();


private:
    QList<KXEmbed> m_Wins;
    KWinModule *kwin_module;
    TrayButton *lockButton, *logoutButton;

private slots:
		void slotLockPrefs();
		void slotLogoutPrefs();
};

#endif
