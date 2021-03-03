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

#include <qlayout.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kpanelapplet.h>

#include "global.h"
#include "panel.h"
// TODO: Martijn Klingens
// #include "buttonop_mnu.h"
#include "appletop_mnu.h"
#include "panelbutton.h"

#include "container_button.h"
#include "container_button.moc"

ButtonContainer::ButtonContainer(QWidget* parent)
  : BaseContainer(parent)
  , _button(0)
  , _layout(0)
  , _oldpos(0,0){}

void ButtonContainer::configure()
{
    if(_button)
        _button->configure();
}

void  ButtonContainer::saveConfiguration(KConfig* config, const QString& group)
{
    if(_button) _button->saveConfiguration(config, group);
}

void ButtonContainer::slotSetPopupDirection(Direction d)
{
    BaseContainer::slotSetPopupDirection(d);

    if(_button)
        _button->slotSetPopupDirection(d);
}

void ButtonContainer::slotSetOrientation(Orientation o)
{
    BaseContainer::slotSetOrientation(o);

    if(_button)
        _button->slotSetOrientation(o);
}

void ButtonContainer::embedButton(PanelButton* b)
{
    if (!b) return;

    delete _layout;
    _layout = new QVBoxLayout(this);
    _button = b;

    _button->installEventFilter(this);
    _layout->add(_button);
    connect(_button, SIGNAL(requestSave()), SIGNAL(requestSave()));
}

bool ButtonContainer::eventFilter (QObject *, QEvent *e)
{
  switch (e->type())
  {
    case QEvent::MouseButtonPress:
    {
	    QMouseEvent* ev = (QMouseEvent*) e;
	    if ( ev->button() == RightButton )
      {
        if (!_opMnu)
// TODO: Martijn Klingens
//          _opMnu = new PanelButtonOpMenu(_actions, _button->title(), _button->icon());
        _opMnu = new PanelAppletOpMenu(_actions, _button->title(), _button->icon());

        switch(_opMnu->exec(getPopupPosition(_opMnu, ev->pos())))
        {
          case PanelAppletOpMenu::Move:
            _moveOffset = QPoint(width()/2, height()/2);
            emit moveme(this);
            break;
          case PanelAppletOpMenu::Remove:
            emit removeme(this);
            return true;
          case PanelAppletOpMenu::Help:
            help();
            break;
          case PanelAppletOpMenu::About:
            about();
            break;
          case PanelAppletOpMenu::Preferences:
            if (_button)
            _button->properties();
            break;
          default:
            break;
        }
        return false;
      }
      else if ( ev->button() == MidButton )
      {
        if (_button)
          _button->setDown(true);
        _moveOffset = ev->pos();
        emit moveme(this);
        return true;
      }
      return false;
    }
    default:
      return false;
  }
}

void ButtonContainer::completeMoveOperation()
{
    if(_button)
        _button->setDown(false);
}

// KMenuButton containerpan
KMenuButtonContainer::KMenuButtonContainer(QWidget* parent)
  : ButtonContainer(parent)
{
    PanelKButton *b = new PanelKButton(this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

int KMenuButtonContainer::widthForHeight( int height )
{
    if ( height < 32 )
	return height + 10;
    return height;
}

int KMenuButtonContainer::heightForWidth( int width )
{
    if ( width < 32 )
	return width + 10;
    return width;
}

// DesktopButton container
DesktopButtonContainer::DesktopButtonContainer(QWidget* parent)
  : ButtonContainer(parent)
{
    PanelDesktopButton *b = new PanelDesktopButton(this);
    embedButton(b);
}

// URLButton container
URLButtonContainer::URLButtonContainer(QWidget* parent, const QString &url )
  : ButtonContainer(parent)
{
    PanelURLButton *b = new PanelURLButton(url, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

URLButtonContainer::URLButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent)
  : ButtonContainer(parent)
{
    config->setGroup(configGroup);
    QString url = config->readEntry("URL");

    PanelURLButton *b = new PanelURLButton(url, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

// BrowserButton container
BrowserButtonContainer::BrowserButtonContainer(QWidget* parent, const QString &startDir, const QString& icon)
  : ButtonContainer(parent)
{
    PanelBrowserButton *b = new PanelBrowserButton(icon, startDir, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

BrowserButtonContainer::BrowserButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent)
  : ButtonContainer(parent)
{
    config->setGroup(configGroup);
    QString startDir = config->readEntry("Path");
    QString icon = config->readEntry("Icon", "kdisknav");

    PanelBrowserButton *b = new PanelBrowserButton(icon, startDir, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

// ServiceMenuButton container
ServiceMenuButtonContainer::ServiceMenuButtonContainer(QWidget* parent, const QString &label, const QString& relPath)
  : ButtonContainer(parent)
{
    PanelServiceMenuButton *b = new PanelServiceMenuButton(label, relPath, this);
    embedButton(b);
}

ServiceMenuButtonContainer::ServiceMenuButtonContainer(KConfig* config, const QString& configGroup, QWidget* parent)
  : ButtonContainer(parent)
{
    config->setGroup(configGroup);
    QString relPath = config->readEntry("RelPath");
    QString label = config->readEntry("Label");

    PanelServiceMenuButton *b = new PanelServiceMenuButton(label, relPath, this);
    embedButton(b);
}

// WindowListButton container
WindowListButtonContainer::WindowListButtonContainer(QWidget* parent)
  : ButtonContainer(parent)
{
    PanelWindowListButton *b = new PanelWindowListButton(this);
    embedButton(b);
}

// BookmarkButton container
BookmarksButtonContainer::BookmarksButtonContainer(QWidget* parent)
  : ButtonContainer(parent)
{
    PanelBookmarksButton *b = new PanelBookmarksButton(this);
    embedButton(b);
}

// RecentDocumentsButton container
RecentDocumentsButtonContainer::RecentDocumentsButtonContainer(QWidget* parent)
  : ButtonContainer(parent)
{
    PanelRecentDocumentsButton *b = new PanelRecentDocumentsButton(this);
    embedButton(b);
}


// ExeButton container
ExeButtonContainer::ExeButtonContainer(QWidget *parent, const QString &filePath, const QString &icon,
                                       const QString &cmdLine, bool inTerm)
  : ButtonContainer(parent)
{
    PanelExeButton *b = new PanelExeButton(filePath, icon, cmdLine, inTerm, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

ExeButtonContainer::ExeButtonContainer(KConfig* config, const QString& configGroup, QWidget *parent)
  : ButtonContainer(parent)
{
    config->setGroup(configGroup);
    QString filePath = config->readEntry("Path");
    QString icon = config->readEntry("Icon");
    QString cmdLine = config->readEntry("CommandLine");
    bool inTerm = config->readBoolEntry("RunInTerminal");

    PanelExeButton *b = new PanelExeButton(filePath, icon, cmdLine, inTerm, this);
    _actions = KPanelApplet::Preferences;
    embedButton(b);
}

KonsoleButtonContainer::KonsoleButtonContainer(QWidget *parent)
  : ButtonContainer(parent)
{
    PanelKonsoleButton *b = new PanelKonsoleButton(this);
    embedButton(b);
}

