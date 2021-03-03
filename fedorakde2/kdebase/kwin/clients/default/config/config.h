/*
 *	$Id: config.h,v 1.1 2001/06/07 11:35:06 gallium Exp $
 *
 *	KDE2 Default configuration widget
 *
 *	Copyright (c) 2001
 *		Karol Szwed <gallium@kde.org>
 *		http://gallium.n3.net/
 */

#ifndef _KDE_DEFAULT_CONFIG_H
#define _KDE_DEFAULT_CONFIG_H

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <kconfig.h>
#include <qslider.h>
#include <qhbox.h>
#include <qlabel.h>

class KDEDefaultConfig: public QObject
{
	Q_OBJECT

	public:
		KDEDefaultConfig( KConfig* conf, QWidget* parent );
		~KDEDefaultConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use
		void slotSelectionChanged(int);	// Internal use

	private:
		QCheckBox* cbShowStipple;
		QCheckBox* cbShowGrabBar;
		QCheckBox* cbUseGradients;
		QSlider*   titleBarSizeSlider;
		QGroupBox* gb;
		bool 	   highcolor;
		QLabel*	   label1;
		QLabel*	   label2;
		QLabel*	   label3;
		QGroupBox* gbSlider;
		QHBox* 	   hbox;
};

#endif
// vim: ts=4
