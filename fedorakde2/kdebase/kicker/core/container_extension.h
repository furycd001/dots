/*****************************************************************

Copyright (c) 2000 Matthias Elter <elter@kde.org>

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

#ifndef __container_extension_h__
#define __container_extension_h__

#include <kpanelextension.h>
#include <dcopobject.h>

#include "global.h"
#include "appletinfo.h"
#include "container_panel.h"

class QXEmbed;
class PanelExtensionOpMenu;

class ExtensionContainer : public PanelContainer
{
    Q_OBJECT

public:
    ExtensionContainer(const AppletInfo& info, QWidget *parent=0);
    virtual ~ExtensionContainer();

    virtual QSize sizeHint(Position, QSize maxSize) { return maxSize; }

    KPanelExtension::Type type() const { return _type; }
    const AppletInfo& info() const { return _info; }

    QString extensionId() const { return _id; }
    void setExtensionId(const QString& s) { _id = s; }

    bool eventFilter (QObject *, QEvent *);

    void readConfig();
    void writeConfig();

    virtual void about() {}
    virtual void help() {}
    virtual void preferences() {}
    virtual void reportBug() {}

    void removeSessionConfigFile();

signals:
    void removeme(ExtensionContainer*);

protected:
    virtual PanelSettings defaultSettings();
    virtual QRect workArea();

    QString               _id;
    PanelExtensionOpMenu *_opMnu;
    AppletInfo            _info;
    KPanelExtension::Type _type;
    int                   _actions;
};

class InternalExtensionContainer : public ExtensionContainer
{
    Q_OBJECT

public:
    InternalExtensionContainer(const AppletInfo& info, QWidget *parent = 0);
    ~InternalExtensionContainer();

    QSize sizeHint(Position, QSize maxSize);

    void about();
    void help();
    void preferences();
    void reportBug();

protected:
    PanelSettings defaultSettings();

protected slots:
    void slotSetPosition(Position p);

private:
    KPanelExtension *_extension;
};

class ExternalExtensionContainer : public ExtensionContainer, DCOPObject
{
    Q_OBJECT

public:
    ExternalExtensionContainer(const AppletInfo& info, QWidget *parent = 0);
    ~ExternalExtensionContainer();

    QSize sizeHint(Position, QSize maxSize);

    void about();
    void help();
    void preferences();
    void reportBug();

    bool process(const QCString &fun, const QByteArray &data,
                 QCString& replyType, QByteArray &replyData);

signals:
    void embeddedWindowDestroyed();
    void docked(ExternalExtensionContainer*);

protected:
    void dockRequest(QCString app, int actions, int type);

protected slots:
    void slotSetPosition(Position p);

private:
    QXEmbed  *_embed;
    QCString  _app;
    bool      _isdocked;
};

#endif

