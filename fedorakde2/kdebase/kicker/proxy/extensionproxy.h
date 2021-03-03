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

#ifndef __extensionproxy_h__
#define __extensionproxy_h__

#include <qcstring.h>
#include <qobject.h>

#include <dcopobject.h>

class AppletInfo;
class KPanelExtension;
class KickerPluginManager;

class ExtensionProxy : public QObject, DCOPObject
{
    Q_OBJECT

public:
    ExtensionProxy(QObject* parent, const char* name = 0);
    ~ExtensionProxy();

    void loadExtension(const QCString& desktopFile, const QCString& configFile);
    void dock(const QCString& callbackID);

    bool process(const QCString &fun, const QByteArray &data,
		 QCString& replyType, QByteArray &replyData);

protected slots:
    void slotUpdateLayout();
    void slotApplicationRemoved(const QCString&);

private:
    AppletInfo          *_info;
    KPanelExtension     *_extension;
    KickerPluginManager *_loader;
    QCString             _callbackID;
};

#endif
