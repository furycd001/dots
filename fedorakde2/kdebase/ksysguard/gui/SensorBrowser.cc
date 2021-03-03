/*
    KSysGuard, the KDE Task Manager and System Monitor
   
	Copyright (c) 1999, 2000 Chris Schlaeger <cs@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	KSysGuard is currently maintained by Chris Schlaeger <cs@kde.org>. Please do
	not commit any changes without consulting me first. Thanks!

	$Id: SensorBrowser.cc,v 1.40 2001/07/12 20:22:59 tokoe Exp $
*/

#include <assert.h>

#include <qevent.h>
#include <qdragobject.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "SensorBrowser.h"
#include "SensorManager.h"
#include "SensorBrowser.moc"

SensorBrowser::SensorBrowser(QWidget* parent, SensorManager* sm,
							 const char* name) :
	QListView(parent, name), sensorManager(sm)
{
	hostInfos.setAutoDelete(TRUE);

	connect(sm, SIGNAL(update(void)), this, SLOT(update(void)));
	connect(this, SIGNAL(selectionChanged(QListViewItem*)),
			this, SLOT(newItemSelected(QListViewItem*)));

	addColumn(i18n("Sensor Browser"));
	addColumn(i18n("Sensor Type"));
	QToolTip::add(this, i18n("Drag sensors to empty cells of a work sheet "
							 "or the panel applet."));
	setRootIsDecorated(TRUE);

	icons = new KIconLoader();
	CHECK_PTR(icons);

	// The sensor browser can be completely hidden.
	setMinimumWidth(1);

	QWhatsThis::add(this, i18n(
		"The sensor browser lists the connected hosts and the sensors "
		"that they provide. Click and drag sensors into drop zones "
		"of a work sheet or the panel applet. A display will appear "
		"that visualizes the "
		"values provided by the sensor. Some sensor displays can "
		"display values of multiple sensors. Simply drag other "
		"sensors on to the display to add more sensors."));
}

SensorBrowser::~SensorBrowser()
{
	delete icons;
}

void
SensorBrowser::disconnect()
{
	QListIterator<HostInfo> it(hostInfos);

	for (; it.current(); ++it)
		if ((*it)->getLVI()->isSelected())
		{
			kdDebug() << "Disconnecting " <<  (*it)->getHostName().ascii()
					  << endl;
			SensorMgr->disengage((*it)->getSensorAgent());
		}
}

void
SensorBrowser::hostReconfigured(const QString&)
{
	// TODO: not yet implemented.
}

void
SensorBrowser::update()
{
	static int id = 0;

	SensorManagerIterator it(sensorManager);

	hostInfos.clear();
	clear();

	SensorAgent* host;
	for (int i = 0 ; (host = it.current()); ++it, ++i)
	{
		QString hostName = sensorManager->getHostName(host);
		QListViewItem* lvi = new QListViewItem(this, hostName);
		CHECK_PTR(lvi);

		QPixmap pix = icons->loadIcon("computer", KIcon::Desktop,
									  KIcon::SizeSmall);
		lvi->setPixmap(0, pix);

		HostInfo* hostInfo = new HostInfo(id, host, hostName, lvi);
		CHECK_PTR(hostInfo);
		hostInfos.append(hostInfo);

		// request sensor list from host
		host->sendRequest("monitors", this, id);
		++id;
	}
	setMouseTracking(FALSE);
}

void
SensorBrowser::newItemSelected(QListViewItem* item)
{
	if (item->pixmap(0))
		KMessageBox::information(
			this, i18n("Drag sensors to empty fields in a work sheet"),
			QString::null, "ShowSBUseInfo");
}

void
SensorBrowser::answerReceived(int id, const QString& s)
{
	/* An answer has the following format:

	   cpu/idle	integer
	   cpu/sys 	integer
	   cpu/nice	integer
	   cpu/user	integer
	   ps	table
	*/

	QListIterator<HostInfo> it(hostInfos);

	/* Check if id is still valid. It can get obsolete by rapid calls
	 * of update() or when the sensor died. */
	for (; it.current(); ++it)
		if ((*it)->getId() == id)
			break;
	if (!it.current())
		return;

	SensorTokenizer lines(s, '\n');

	for (unsigned int i = 0; i < lines.numberOfTokens(); ++i)
	{
		if (lines[i].isEmpty())
			break;
		SensorTokenizer words(lines[i], '\t');

		QString sensorName = words[0];
		QString sensorType = words[1];

		/* Calling update() a rapid sequence will create pending
		 * requests that do not get erased by calling
		 * clear(). Subsequent updates will receive the old pending
		 * answers so we need to make sure that we register each
		 * sensor only once. */
		if ((*it)->isRegistered(sensorName))
			return;

		/* The sensor browser can display sensors in a hierachical order.
		 * Sensors can be grouped into nodes by seperating the hierachical
		 * nodes through slashes in the sensor name. E. g. cpu/user is
		 * the sensor user in the cpu node. There is no limit for the
		 * depth of nodes. */
		SensorTokenizer absolutePath(sensorName, '/');
		
		QListViewItem* parent = (*it)->getLVI();
		for (unsigned int j = 0; j < absolutePath.numberOfTokens(); ++j)
		{
			// Localize the sensor name part by part.
			QString name;
			name = SensorMgr->trSensorPath(absolutePath[j]);

			bool found = FALSE;
			QListViewItem* sibling = parent->firstChild();
			while (sibling && !found)
			{
				if (sibling->text(0) == name)
				{
					// The node or sensor is already known.
					found = TRUE;
				}
				else
					sibling = sibling->nextSibling();
			}
			if (!found)
			{
				QListViewItem* lvi = new QListViewItem(parent, name);
				CHECK_PTR(lvi);
				if (j == absolutePath.numberOfTokens() - 1)
				{
					QPixmap pix = icons->loadIcon("ksysguardd",
												  KIcon::Desktop,
												  KIcon::SizeSmall);
					lvi->setPixmap(0, pix);
					lvi->setText(1, SensorMgr->trSensorType(sensorType));
					// add sensor info to internal data structure
					(*it)->addSensor(lvi, sensorName, name, sensorType);
				}
				else
					parent = lvi;

				// The child indicator might need to be updated.
				repaintItem(parent);
			}
			else
				parent = sibling;
		}
	}

	repaintItem((*it)->getLVI());
}

void
SensorBrowser::viewportMouseMoveEvent(QMouseEvent* ev)
{
	/* setMouseTracking(FALSE) seems to be broken. With current Qt
	 * mouse tracking cannot be turned off. So we have to check each event
	 * whether the LMB is really pressed. */

	if (!(ev->state() & LeftButton))
		return;

	QListViewItem* item = itemAt(ev->pos());
	if (!item)
		return;		// no item under cursor

	// Make sure that a sensor and not a node or hostname has been picked.
	QListIterator<HostInfo> it(hostInfos);
	for ( ; it.current() && !(*it)->isRegistered(item); ++it)
		;
	if (!it.current())
		return;

	// Create text drag object as
	// "<hostname> <sensorname> <sensortype> <sensordescription>".
	// Only the description may contain blanks.
	dragText = (*it)->getHostName() + " "
		+ (*it)->getSensorName(item) + " "
		+ (*it)->getSensorType(item) + " "
		+ (*it)->getSensorDescription(item);

	QDragObject* dObj = new QTextDrag(dragText, this);
	CHECK_PTR(dObj);
	dObj->dragCopy();
}

QStringList
SensorBrowser::listHosts()
{
	QStringList hostList;

	QListIterator<HostInfo> it(hostInfos);
	for ( ; it.current(); ++it)
	{
		hostList.append((*it)->getHostName());
	}

	return hostList;
}

QStringList
SensorBrowser::listSensors(const QString& hostName)
{
	QStringList sensorList;

	QListIterator<HostInfo> it(hostInfos);
	for ( ; it.current(); ++it)
	{
		if ((*it)->getHostName() == hostName) {
			(*it)->appendSensors(sensorList);
			return (sensorList);
		}
	}

	return (sensorList);
}
