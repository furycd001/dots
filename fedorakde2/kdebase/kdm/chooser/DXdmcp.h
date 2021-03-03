/***************************************************************************
                          DXdmcp.h  -  description
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
#ifndef DXDCMP_H
#define DXDCMP_H

#ifndef QT_CLEAN_NAMESPACE
#define QT_CLEAN_NAMESPACE
#endif
#include <qlistview.h>
#include <qlineedit.h>

#include "kfdialog.h"
#include "CXdmcp.h"

class HostView:public QListView {
    Q_OBJECT

  public:
    HostView(CXdmcp * cxdmcp, QWidget * parent = 0, const char *name =
	     0, WFlags f = 0);

  public slots:
    void pingHosts();
    void accept();
    void willing();
    void cancel();

    /* Add host to list.
     */
    void slotAddHost(CXdmcp::HostName * name);

    /* No more hosts to display.
     */
    void slotDeleteAllHosts();

    /* Remove host from list.
     */
    void slotDeleteHost(const QString & name);

    /* Change hosts name in list.
     */
    void slotChangeHost(const QString & oldname,
			CXdmcp::HostName * newname);

    /* Add hostname to ping.
     * "BROADCAST" is special.
     */
    void slotRegisterHostname(const QString & name);

  private:
    CXdmcp *comXdmcp;
    int namecol, statcol;
};


class ChooserDlg:public FDialog {
    Q_OBJECT

  public:
    ChooserDlg(CXdmcp * cxdmcp, QWidget * parent = 0, const char *name =
	       0, bool modal = false, WFlags f = 0);

    void ping();

  public slots:
    void addHostname();
    void slotHelp();

  private:
    HostView *host_view;
    QLineEdit *iline;
};

#endif
