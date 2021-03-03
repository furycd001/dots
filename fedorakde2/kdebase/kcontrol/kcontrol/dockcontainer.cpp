/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <qapplication.h>
#include <qlabel.h>

#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>
#include <kdebug.h>

#include "dockcontainer.h"
#include "dockcontainer.moc"

#include "global.h"
#include "modules.h"
#include "proxywidget.h"
#include <qobjectlist.h>
#include <qaccel.h>

DockContainer::DockContainer(QWidget *parent)
  : QWidget(parent, "DockContainer")
  , _basew(0L)
  , _module(0L)
{
  _busy = new QLabel(i18n("<big>Loading ...</big>"), this);
  _busy->setAlignment(AlignCenter);
  _busy->setTextFormat(RichText);
  _busy->setGeometry(0,0, width(), height());
  _busy->hide();
}

DockContainer::~DockContainer()
{
  deleteModule();
}

void DockContainer::setBaseWidget(QWidget *widget)
{
  delete _basew;
  _basew = 0;
  if (!widget) return;

  _basew = widget;
  _basew->reparent(this, 0, QPoint(0,0), true);

  // "inherit" the minimum size
  setMinimumSize( _basew->minimumSize() );
  _basew->resize( size() );
  emit newModule(widget->caption(), "", "");
}

void DockContainer::dockModule(ConfigModule *module)
{
  if (module == _module) return;

  if (_module && _module->isChanged())
    {

      int res = KMessageBox::warningYesNo(this,
module ?
i18n("There are unsaved changes in the "
     "active module.\n"
     "Do you want to apply the changes "
     "before running\n"
     "the new module or forget the changes?") :
i18n("There are unsaved changes in the "
     "active module.\n"
     "Do you want to apply the changes "
     "before exiting\n"
     "the Control Center or forget the changes?"),
                                          i18n("Unsaved changes"),
                                          i18n("&Apply"),
                                          i18n("&Forget"));
      if (res == KMessageBox::Yes)
        _module->module()->applyClicked();
    }

  deleteModule();
  if (!module) return;

  _busy->raise();
  _busy->show();
  _busy->repaint();
  QApplication::setOverrideCursor( waitCursor );

  ProxyWidget *widget = module->module();

  if (widget)
    {
      _module = module;
      connect(_module, SIGNAL(childClosed()),
              this, SLOT(removeModule()));
      connect(_module, SIGNAL(changed(ConfigModule *)),
              SIGNAL(changedModule(ConfigModule *)));
      //####### this doesn't work anymore, what was it supposed to do? The signal is gone.
//       connect(_module, SIGNAL(quickHelpChanged()),
//               this, SLOT(quickHelpChanged()));

      widget->reparent(this, 0 , QPoint(0,0), false);
      widget->resize(size());
      // "inherit" the minimum size
      setMinimumSize( widget->minimumSize() );

      emit newModule(widget->caption(), module->docPath(), widget->quickHelp());
      QApplication::restoreOverrideCursor();
    }
  else
    QApplication::restoreOverrideCursor();

  if (widget)  {
      widget->show();
      QApplication::sendPostedEvents( widget, QEvent::ShowWindowRequest ); // show NOW
  }
  _busy->hide();

  KCGlobal::repairAccels( topLevelWidget() );
}

void DockContainer::removeModule()
{
  deleteModule();

  resizeEvent(0L);

  if (_basew)
  {
    setMinimumSize( _basew->minimumSize() );
	emit newModule(_basew->caption(), "", "");
  }
  else
	emit newModule("", "", "");
}

void DockContainer::deleteModule()
{
  if(_module) {
	_module->deleteClient();
	_module = 0;
  }
}

void DockContainer::resizeEvent(QResizeEvent *)
{
  _busy->resize(width(), height());
  if (_module)
	{
	  _module->module()->resize(size());
	  _basew->hide();
	}
  else if (_basew)
	{
	  _basew->resize(size());
	  _basew->show();
	}
}

void DockContainer::quickHelpChanged()
{
  if (_module && _module->module())
	emit newModule(_module->module()->caption(),  _module->docPath(), _module->module()->quickHelp());
}
