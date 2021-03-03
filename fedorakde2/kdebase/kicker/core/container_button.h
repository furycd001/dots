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

#ifndef __container_button_h__
#define __container_button_h__

#include "container_base.h"

class QLayout;
class PanelButton;
class KConfig;

class ButtonContainer : public BaseContainer
{
    Q_OBJECT

public:
    ButtonContainer(QWidget* parent = 0);

    // buttons have a quadratic shape
    virtual int widthForHeight(int height) { return height; }
    virtual int heightForWidth(int width) { return width; }

    virtual void saveConfiguration(KConfig* config, const QString& group);
    virtual void configure();


    bool eventFilter (QObject *, QEvent *);
    void completeMoveOperation();

public slots:
    void slotSetPopupDirection(Direction d);
    void slotSetOrientation(Orientation o);

protected:
    void embedButton(PanelButton* p);

protected:
    PanelButton  *_button;
    QLayout      *_layout;
    QPoint        _oldpos;
};

class KMenuButtonContainer : public ButtonContainer
{
public:
    KMenuButtonContainer(QWidget* parent);
    QString appletType() { return "KMenuButton"; }

    virtual int widthForHeight( int height );
    virtual int heightForWidth( int width );
};

class DesktopButtonContainer : public ButtonContainer
{
public:
    DesktopButtonContainer(QWidget* parent);
    QString appletType() { return "DesktopButton"; }
};

class URLButtonContainer : public ButtonContainer
{
public:
    URLButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent = 0);
    URLButtonContainer(QWidget* parent, const QString& url);
    QString appletType() { return "URLButton"; }
};

class BrowserButtonContainer : public ButtonContainer
{
public:
    BrowserButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent = 0);
    BrowserButtonContainer(QWidget* parent, const QString& startDir, const QString& icon = "kdisknav");
    QString appletType() { return "BrowserButton"; }
};

class ServiceMenuButtonContainer : public ButtonContainer
{
public:
    ServiceMenuButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent = 0);
    ServiceMenuButtonContainer(QWidget* parent, const QString& label, const QString& relPath);
    QString appletType() { return "ServiceMenuButton"; }
};

class WindowListButtonContainer : public ButtonContainer
{
public:
    WindowListButtonContainer(QWidget* parent = 0);
    QString appletType() { return "WindowListButton"; }
};

class BookmarksButtonContainer : public ButtonContainer
{
public:
    BookmarksButtonContainer(QWidget* parent = 0);
    QString appletType() { return "BookmarksButton"; }
};

class RecentDocumentsButtonContainer : public ButtonContainer
{
public:
    RecentDocumentsButtonContainer(QWidget* parent = 0);
    QString appletType() { return "RecentDocumentsButton"; }
};

class ExeButtonContainer : public ButtonContainer
{
public:
    ExeButtonContainer(KConfig* config, const QString& configGroup, QWidget *parent=0);
    ExeButtonContainer(QWidget *parent, const QString &filePath, const QString &icon,
                       const QString &cmdLine, bool inTerm);
    QString appletType() { return "ExeButton"; }
};

class KonsoleButtonContainer : public ButtonContainer
{
public:
    KonsoleButtonContainer(QWidget *parent = 0);
    QString appletType() { return "KonsoleButton"; }
};

#endif

