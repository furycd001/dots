/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "printpart.h"
#include "printpartfactory.h"

#include <kmmainview.h>
#include <kaction.h>
#include <klocale.h>
#include <qwidget.h>

PrintPart::PrintPart(QWidget *parent, const char *name)
: KParts::ReadOnlyPart(parent, name)
{
	setInstance(PrintPartFactory::instance());
	m_extension = new PrintPartExtension(this);

	m_view = new KMMainView(parent, "MainView");
	m_view->setFocusPolicy(QWidget::ClickFocus);
	m_view->enableToolbar(false);
	setWidget(m_view);

	initActions();
}

PrintPart::~PrintPart()
{
}

bool PrintPart::openFile()
{
	return true;
}

void PrintPart::initActions()
{
	KAction	*act(0);

	act = new KAction(i18n("Add printer/class..."),"wizard",0,m_view,SLOT(slotAdd()),actionCollection(),"printer_add");
	connectAction(act);
	act = new KAction(i18n("Add special (pseudo) printer..."),"filequickprint",0,m_view,SLOT(slotAddSpecial()),actionCollection(),"printer_add_special");
	connectAction(act);
	act = new KAction(i18n("Enable"),"run",0,m_view,SLOT(slotEnable()),actionCollection(),"printer_enable");
	connectAction(act);
	act = new KAction(i18n("Disable"),"stop",0,m_view,SLOT(slotDisable()),actionCollection(),"printer_disable");
	connectAction(act);
	act = new KAction(i18n("Remove"),"edittrash",0,m_view,SLOT(slotRemove()),actionCollection(),"printer_remove");
	connectAction(act);
	act = new KAction(i18n("Configure"),"configure",0,m_view,SLOT(slotConfigure()),actionCollection(),"printer_configure");
	connectAction(act);
	act = new KAction(i18n("Set as local default"),"kdeprint_printer",0,m_view,SLOT(slotHardDefault()),actionCollection(),"printer_hard_default");
	connectAction(act);
	act = new KAction(i18n("Set as user default"),"exec",0,m_view,SLOT(slotSoftDefault()),actionCollection(),"printer_soft_default");
	connectAction(act);
	act = new KAction(i18n("Test printer"),"fileprint",0,m_view,SLOT(slotTest()),actionCollection(),"printer_test");
	connectAction(act);
	act = new KAction(i18n("Configure manager"),"configure",0,m_view,SLOT(slotManagerConfigure()),actionCollection(),"manager_configure");
	connectAction(act);
	act = new KAction(i18n("Refresh view"),"reload",0,m_view,SLOT(slotTimer()),actionCollection(),"view_refresh");
	connectAction(act);
	act = new KAction(i18n("Restart server"),"gear",0,m_view,SLOT(slotServerRestart()),actionCollection(),"server_restart");
	connectAction(act);
	act = new KAction(i18n("Configure server"),"configure",0,m_view,SLOT(slotServerConfigure()),actionCollection(),"server_configure");
	connectAction(act);

	setXMLFile("kdeprint_part.rc");
}

void PrintPart::connectAction(KAction *act)
{
	KAction	*mact = m_view->action(act->name());
	if (mact)
	{
		connect(mact,SIGNAL(enabled(bool)),act,SLOT(setEnabled(bool)));
		act->setEnabled(mact->isEnabled());
	}
}

PrintPartExtension::PrintPartExtension(PrintPart *parent)
: KParts::BrowserExtension(parent, "PrintPartExtension")
{
}

PrintPartExtension::~PrintPartExtension()
{
}

#include "printpart.moc"
