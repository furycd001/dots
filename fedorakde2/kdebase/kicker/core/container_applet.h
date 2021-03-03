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

#ifndef __container_applet_h__
#define __container_applet_h__

#include <kpanelapplet.h>
#include <dcopobject.h>

#include "appletinfo.h"
#include "container_base.h"

class QHBox;
class QXEmbed;
class QBoxLayout;
class KConfig;

class AppletHandle;

class AppletContainer : public BaseContainer
{
    Q_OBJECT

public:
    AppletContainer(const AppletInfo& info, QWidget* parent = 0);

    bool eventFilter (QObject *, QEvent *);

    KPanelApplet::Type type() const { return _type; }
    const AppletInfo& info() const { return _info; }

    void resetLayout();

    virtual void configure();

    void setWidthForHeightHint(int w) { _widthForHeightHint = w; }
    void setHeightForWidthHint(int h) { _heightForWidthHint = h; }

    void removeSessionConfigFile();
signals:
    void updateLayout();

public slots:
    virtual void slotSetPopupDirection(Direction d);
    virtual void slotSetOrientation(Orientation o);
    void activateWindow() { setActiveWindow(); }

protected:
    AppletHandle      *_handle;
    AppletInfo         _info;
    QHBox             *_appletframe;
    QBoxLayout        *_layout;
    KPanelApplet::Type _type;
    int                _widthForHeightHint, _heightForWidthHint;
    QString            _deskFile, _configFile;
};

class InternalAppletContainer : public AppletContainer
{
    Q_OBJECT

public:
    InternalAppletContainer(const AppletInfo& info, QWidget *parent);
    ~InternalAppletContainer();

    int widthForHeight(int height);
    int heightForWidth(int width);

    void about();
    void help();
    void preferences();
    void reportBug();

    void saveConfiguration(KConfig* config, const QString& group);

    QString appletType() { return "Applet"; }

public slots:
    void slotSetPopupDirection(Direction d);
    void slotSetOrientation(Orientation o);

private:
    QCString      _id;
    KPanelApplet *_applet;
};


class ExternalAppletContainer : public AppletContainer, DCOPObject
{
    Q_OBJECT

public:
    ExternalAppletContainer(const AppletInfo& info, QWidget *parent);
    ~ExternalAppletContainer();

    int widthForHeight(int height);
    int heightForWidth(int width);

    void about();
    void help();
    void preferences();
    void reportBug();

    void saveConfiguration(KConfig* config, const QString& group);

    QString appletType() { return "Applet"; }

    bool process(const QCString &fun, const QByteArray &data,
		 QCString& replyType, QByteArray &replyData);
public slots:
    void slotSetPopupDirection(Direction d);
    void slotSetOrientation(Orientation o);

signals:
    void embeddedWindowDestroyed();
    void docked(ExternalAppletContainer*);

protected:
    void dockRequest(QCString app, int actions, int type);

private:
    QXEmbed  *_embed;
    QCString  _app;
    bool      _isdocked;
};


#endif

