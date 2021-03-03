/***************************************************************************
                          DXdmcp.cpp  -  description
                             -------------------
    begin                : Tue Nov 9 1999
    copyright            : (C) 1999 by Harald Hoyer
    email                : Harald.Hoyer@RedHat.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "DXdmcp.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <klocale.h>
#include <qfont.h>
#include <kmessagebox.h>
#include <stdlib.h>

HostView::HostView(CXdmcp * cxdmcp, QWidget * parent, const char *name, WFlags)
    : QListView(parent, name)
{
    comXdmcp = cxdmcp;

    namecol = addColumn(i18n("Hostname"));
    statcol = addColumn(i18n("Status"));

    connect(comXdmcp, SIGNAL(addHost(CXdmcp::HostName *)), this,
	    SLOT(slotAddHost(CXdmcp::HostName *)));
    connect(comXdmcp,
	    SIGNAL(changeHost(const QString &, CXdmcp::HostName *)), this,
	    SLOT(slotChangeHost(const QString &, CXdmcp::HostName *)));
    connect(comXdmcp, SIGNAL(deleteHost(const QString &)), this,
	    SLOT(slotDeleteHost(const QString &)));
    connect(comXdmcp, SIGNAL(deleteAllHosts()), this,
	    SLOT(slotDeleteAllHosts()));
    connect(this, SIGNAL(doubleClicked(QListViewItem *)), this,
	    SLOT(accept()));

//      setItemMargin(1);
};

/*
  typedef struct _hostName {
    struct _hostName	*next;
    char		*fullname;
    int			willing;
    ARRAY8		hostname, status;
    CARD16		connectionType;
    ARRAY8		hostaddr;
  } HostName;
*/

void HostView::slotAddHost(CXdmcp::HostName * name)
{
    QString nstr =
	QString::fromLatin1((char *) name->hostname.data,
			    name->hostname.length);
    QString sstr =
	QString::fromLatin1((char *) name->status.data,
			    name->status.length);

    QListViewItem *item = new QListViewItem(this, nstr, sstr);
    insertItem(item);
}

void HostView::slotDeleteAllHosts()
{
    clear();
}

void HostView::slotDeleteHost(const QString & name)
{
    QListViewItemIterator it(this);

    for (; it.current(); ++it) {
	if (it.current()->text(namecol) == name) {
	    removeItem(it.current());
	    break;
	}
    }
}

void HostView::slotChangeHost(const QString & oldname,
			      CXdmcp::HostName * name)
{
    if (oldname.isNull())
	return;

    QString nstr =
	QString::fromLatin1((char *) name->hostname.data,
			    name->hostname.length);
    QString sstr =
	QString::fromLatin1((char *) name->status.data,
			    name->status.length);

    QListViewItemIterator it(this);
    for (; it.current(); ++it) {
	if (it.current()->text(namecol) == oldname) {
	    it.current()->setText(namecol, nstr);
	    it.current()->setText(statcol, sstr);
	    break;
	}
    }
}

void HostView::pingHosts()
{
    comXdmcp->emptyHostnames();
    comXdmcp->pingHosts();
}

void HostView::accept()
{
    QListViewItem *item = currentItem();
    if (item != 0) {
	comXdmcp->chooseHost(item->text(namecol).latin1());
	delete comXdmcp;
	exit(EX_NORMAL);
    }
}

void HostView::willing()
{
    // highlight all willing hosts
    // necessary?? needed??
}

void HostView::cancel()
{
    exit(EX_NORMAL);
}

void HostView::slotRegisterHostname(const QString & name)
{
    comXdmcp->registerHostname(name.latin1());
    comXdmcp->pingHosts();
}

static void set_min(QWidget * w)
{
    w->setMinimumSize(w->sizeHint());
}

ChooserDlg::ChooserDlg(CXdmcp * cxdmcp, QWidget * parent, const char *name,
		       bool modal, WFlags f)
    : FDialog(parent, name, modal, f)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, 0);

    QFrame *winFrame = new QFrame(this);
    winFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

    topLayout->addWidget(winFrame);

    QBoxLayout *vbox = new QVBoxLayout(winFrame, 10, 10);

    host_view = new HostView(cxdmcp, winFrame, "hosts");

    // Buttons
    QPushButton *accept = new QPushButton(i18n("&Accept"), winFrame);
    QPushButton *cancel = new QPushButton(i18n("&Cancel"), winFrame);
//     QPushButton *willing = new QPushButton( i18n("&Willing"), winFrame);
    QPushButton *help = new QPushButton(i18n("&Help"), winFrame);
    QPushButton *ping = new QPushButton(i18n("&Ping"), winFrame);

    QLabel *title = new QLabel(i18n("XDMCP Host Menu"), winFrame);
    title->setAlignment(AlignCenter);

    set_min(accept);
    set_min(cancel);
    set_min(ping);
    set_min(help);
    set_min(title);

    vbox->addWidget(title);
    vbox->addWidget(host_view);

    QBoxLayout *hibox = new QHBoxLayout(vbox, 10);
    iline = new QLineEdit(winFrame);
    iline->setEnabled(TRUE);
    QLabel *itxt = new QLabel(iline, i18n("A&dd Host:"), winFrame);
    hibox->addWidget(itxt);
    hibox->addWidget(iline);

    QBoxLayout *hbox = new QHBoxLayout(vbox, 20);
    hbox->addWidget(accept);
    hbox->addWidget(ping);
//     hbox->addWidget( willing);
    hbox->addWidget(help);
    hbox->addWidget(cancel);

    topLayout->activate();

    //     setMinimumSize( winFrame->minimumSize());

    connect(ping, SIGNAL(clicked()), host_view, SLOT(pingHosts()));
    connect(accept, SIGNAL(clicked()), host_view, SLOT(accept()));
//     connect( willing, SIGNAL( clicked()), host_view, SLOT( willing()));
    connect(cancel, SIGNAL(clicked()), host_view, SLOT(cancel()));
    connect(help, SIGNAL(clicked()), this, SLOT(slotHelp()));
    connect(iline, SIGNAL(returnPressed()), this, SLOT(addHostname()));
}

void ChooserDlg::slotHelp()
{
    KMessageBox::information(0,
			     i18n("Choose a host, you want to work on,\n"
				  "in the list or add one.\n\n"
				  "After this box, you must press cancel\n"
				  "in the Host Menu to enter a host. :("));
    iline->setFocus();
}

void ChooserDlg::ping()
{
    host_view->pingHosts();
}

void ChooserDlg::addHostname()
{
    if (iline->text().length()) {
	host_view->slotRegisterHostname(iline->text());
	iline->clear();
    }
}

#include "DXdmcp.moc"
