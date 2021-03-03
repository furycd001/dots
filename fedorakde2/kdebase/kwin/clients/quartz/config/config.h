/* 	
 * $Id: config.h,v 1.2 2001/06/04 09:51:23 gallium Exp $
 *
 *	This file contains the quartz configuration widget
 *
 *	Copyright (c) 2001
 *		Karol Szwed <gallium@kde.org>
 *		http://gallium.n3.net/
 */

#ifndef __KDE_QUARTZCONFIG_H
#define __KDE_QUARTZCONFIG_H

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <kconfig.h>

class QuartzConfig: public QObject
{
	Q_OBJECT

	public:
		QuartzConfig( KConfig* conf, QWidget* parent );
		~QuartzConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );	
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use

	private:
		KConfig*   quartzConfig;
		QCheckBox* cbColorBorder;
		QGroupBox* gb;
};


#endif

// vim: ts=4
