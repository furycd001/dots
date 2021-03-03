/*
    KSysGuard, the KDE Task Manager and System Monitor
   
	Copyright (c) 1999 Chris Schlaeger <cs@kde.org>
    
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

	KSysGuard is currently maintained by Chris Schlaeger <cs@kde.org>. Please do
	not commit any changes without consulting me first. Thanks!

	$Id: ReniceDlg.cc,v 1.3 2001/05/28 23:24:42 mhunter Exp $
*/

#include <kapp.h>
#include <klocale.h>

#include "ReniceDlg.moc"

ReniceDlg::ReniceDlg(QWidget* parent, const char* name, int currentPPrio,
					 int pid)
	: QDialog(parent, name, TRUE)
{
	setCaption(i18n("Renice Process"));

	value = currentPPrio;

	vLay = new QVBoxLayout(this, 20, -1, "ReniceLayout");

	QString msg;
	msg = i18n("You are about to change the scheduling priority of\n"
			   "process %1. Be aware that only the Superuser (root)\n"
			   "can decrease the nice level of a process. The lower\n"
			   "the number is the higher the priority.\n\n"
			   "Please enter the desired nice level:").arg(pid);
	message = new QLabel(msg, this);
	message->setMinimumSize(message->sizeHint());
	vLay->addWidget(message);

	/*
	 * Create a slider with an LCD display to the right using a horizontal
	 * layout. The slider and the LCD are kept in sync through signals
	 */
	sldLay = new QHBoxLayout();
	vLay->addLayout(sldLay);

	slider = new QSlider(-20, 20, 1, 0, QSlider::Horizontal, this, "prio" );
	slider->setMaximumSize(210, 25);
	slider->setMinimumSize(210, 25);
	slider->setTickmarks((QSlider::TickSetting) 2);
	slider->setFocusPolicy(QWidget::TabFocus);
	slider->setFixedHeight(slider->sizeHint().height());
	slider->setValue(value);
	sldLay->addWidget(slider);
	sldLay->addSpacing(10);

	lcd = new QLCDNumber(3, this, "lcd");
	lcd->setMaximumSize(55, 23);
	lcd->setMinimumSize(55, 23);
	lcd->display(value);
	QObject::connect(slider, SIGNAL(valueChanged(int)), lcd,
					 SLOT(display(int)));
	QObject::connect(slider, SIGNAL(valueChanged(int)),
					 SLOT(setPriorityValue(int)));
	sldLay->addWidget(lcd);

	/*
	 * Create an "OK" and a "Cancel" button in a horizontal layout.
	 */
	butLay = new QHBoxLayout();
	vLay->addLayout(butLay);
	butLay->addStretch(1);

	okButton = new QPushButton(i18n("OK"), this);
	okButton->setMaximumSize(100, 30);
	okButton->setMinimumSize(100, 30);
	connect(okButton, SIGNAL(clicked()), SLOT(ok()));
	butLay->addWidget(okButton);
	butLay->addStretch(1);

	cancelButton = new QPushButton(i18n("Cancel"), this);
	cancelButton->setMaximumSize(100, 30);
	cancelButton->setMinimumSize(100, 30);
	connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));
	butLay->addWidget(cancelButton);
	butLay->addStretch(1);

	vLay->activate();
}

