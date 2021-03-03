/*
 *  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <dcopclient.h>

#include "kcmlaunch.h"

extern "C"
{
  KCModule * create_launch(QWidget * parent, const char * name)
  {
    KGlobal::locale()->insertCatalogue("kcmlaunch");

    return new LaunchConfig(parent, name);
  }
}

LaunchConfig::LaunchConfig(QWidget * parent, const char * name)
  : KCModule(parent, name)
{
    if ( !name )
	setName( "Form1" );
    resize( 451, 316 );
    setCaption( i18n( "Form1" ) );
    QVBoxLayout* Form1Layout = new QVBoxLayout( this );
    Form1Layout->setSpacing( 6 );
    Form1Layout->setMargin( 11 );

    QGroupBox* GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( i18n( "Busy Cursor" ) );
    QWhatsThis::add(GroupBox1, i18n(
     "<h1>Busy Cursor</h1>\n"
     "KDE offers a busy cursor for application startup notification.\n"
     "To enable the busy cursor, check 'Enable Busy Cursor'.\n"
     "To have the cursor blinking, check 'Enable blinking' below.\n"
     "It may occur, that some applications are not aware of this startup\n"
     "notification. In this case, the cursor stops blinking after the time\n"
     "given in the section 'Startup indication timeout'"));

    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    QGridLayout* GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 11 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox1Layout->addItem( spacer, 2, 4 );

    QLabel* TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setText( i18n( "Startup indication timeout (seconds) :" ) );

    GroupBox1Layout->addMultiCellWidget( TextLabel1, 2, 2, 0, 2 );

    sb_cursorTimeout = new QSpinBox( GroupBox1, "sb_cursorTimeout" );

    GroupBox1Layout->addWidget( sb_cursorTimeout, 2, 3 );

    cb_busyCursor = new QCheckBox( GroupBox1, "cb_busyCursor" );
    cb_busyCursor->setText( i18n( "Enable Busy Cursor " ) );

    GroupBox1Layout->addMultiCellWidget( cb_busyCursor, 0, 0, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    GroupBox1Layout->addItem( spacer_2, 1, 0 );

    cb_busyBlinking = new QCheckBox( GroupBox1, "cb_busyBlinking" );
    cb_busyBlinking->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, cb_busyBlinking->sizePolicy().hasHeightForWidth() ) );
    cb_busyBlinking->setText( i18n( "Enable blinking" ) );

    GroupBox1Layout->addWidget( cb_busyBlinking, 1, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox1Layout->addMultiCell( spacer_3, 1, 1, 2, 4 );
    Form1Layout->addWidget( GroupBox1 );

    QGroupBox* GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setTitle( i18n( "Taskbar Notification" ) );
    QWhatsThis::add(GroupBox2, i18n("<H1>Taskbar Notification</H1>\n"
    "You can enable a second method of startup notification which is\n"
    "used by the taskbar where a button with a rotating disk appears,\n"
    "symbolizing that your started application is loading.\n"
    "It may occur, that some applications are not aware of this startup\n"
     "notification. In this case, the button disappears after the time\n"
     "given in the section 'Startup indication timeout'"));

    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 0 );
    QGridLayout* GroupBox2Layout = new QGridLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );
    GroupBox2Layout->setSpacing( 6 );
    GroupBox2Layout->setMargin( 11 );

    QLabel* TextLabel2 = new QLabel( GroupBox2, "TextLabel2" );
    TextLabel2->setText( i18n( "Startup indication timeout (seconds) :" ) );

    GroupBox2Layout->addWidget( TextLabel2, 1, 0 );

    sb_taskbarTimeout = new QSpinBox( GroupBox2, "sb_taskbarTimeout" );

    GroupBox2Layout->addWidget( sb_taskbarTimeout, 1, 1 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox2Layout->addItem( spacer_4, 1, 2 );

    cb_taskbarButton = new QCheckBox( GroupBox2, "cb_taskbarButton" );
    cb_taskbarButton->setText( i18n( "Enable Taskbar Notification " ) );

    GroupBox2Layout->addWidget( cb_taskbarButton, 0, 0 );
    Form1Layout->addWidget( GroupBox2 );
//    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
//    Form1Layout->addItem( spacer_5 );
    Form1Layout->addStretch();

  load();

  connect
    (
     cb_busyCursor,
     SIGNAL(toggled(bool)),
     SLOT( checkChanged())
    );
  connect
    (
     cb_busyCursor,
     SIGNAL(toggled(bool)),
     cb_busyBlinking,
     SLOT( setEnabled(bool))
    );
  connect
    (
     cb_busyCursor,
     SIGNAL(toggled(bool)),
     sb_cursorTimeout,
     SLOT( setEnabled(bool))
    );
  connect
    (
     cb_busyBlinking,
     SIGNAL(toggled(bool)),
     SLOT( checkChanged())
    );

  connect
    (
     cb_taskbarButton,
     SIGNAL(toggled(bool)),
     SLOT( checkChanged())
    );
  connect
    (
     cb_taskbarButton,
     SIGNAL(toggled(bool)),
     sb_taskbarTimeout,
     SLOT( setEnabled(bool))
    );

  connect
    (
     sb_cursorTimeout,
     SIGNAL(valueChanged(int)),
     SLOT( checkChanged())
    );

  connect
    (
     sb_taskbarTimeout,
     SIGNAL(valueChanged(int)),
     SLOT( checkChanged())
    );

}

LaunchConfig::~LaunchConfig()
{
}

  void
LaunchConfig::load()
{
  KConfig c("klaunchrc", false, false);

  c.setGroup("FeedbackStyle");

  bool busyCursor =
    c.readBoolEntry("BusyCursor", Default & BusyCursor);


  bool taskbarButton =
    c.readBoolEntry("TaskbarButton", Default & TaskbarButton);

  cb_busyCursor->setChecked(busyCursor);
  cb_taskbarButton->setChecked(taskbarButton);
  
  cb_busyBlinking->setEnabled( busyCursor );
  sb_cursorTimeout->setEnabled( busyCursor );

  sb_taskbarTimeout->setEnabled( taskbarButton );

  c.setGroup( "BusyCursorSettings" );
  sb_cursorTimeout->setValue( c.readUnsignedNumEntry( "Timeout", 30 ));
  bool busyBlinking =c.readBoolEntry("Blinking", true);
  cb_busyBlinking->setChecked(busyBlinking);

  c.setGroup( "TaskbarButtonSettings" );
  sb_taskbarTimeout->setValue( c.readUnsignedNumEntry( "Timeout", 30 ));

  emit(changed(false));
}

  void
LaunchConfig::save()
{
  KConfig c("klaunchrc", false, false);

  c.setGroup("FeedbackStyle");
  c.writeEntry("BusyCursor",    cb_busyCursor->isChecked());
  c.writeEntry("TaskbarButton", cb_taskbarButton->isChecked());

  c.setGroup( "BusyCursorSettings" );
  c.writeEntry( "Timeout", sb_cursorTimeout->value());
  c.writeEntry("Blinking", cb_busyBlinking->isChecked());

  c.setGroup( "TaskbarButtonSettings" );
  c.writeEntry( "Timeout", sb_taskbarTimeout->value());

  c.sync();

  emit(changed(false));

  if (!kapp->dcopClient()->isAttached())
     kapp->dcopClient()->attach();
  QByteArray data;
  kapp->dcopClient()->send( "kicker", "Panel", "restart()", data );
  kapp->dcopClient()->send( "kdesktop", "", "configure()", data );
}

  void
LaunchConfig::defaults()
{
  cb_busyCursor->setChecked(Default & BusyCursor);
  cb_busyBlinking->setChecked(Default & BusyCursor);
  cb_taskbarButton->setChecked(Default & TaskbarButton);

  sb_cursorTimeout->setValue( 30 );
  sb_taskbarTimeout->setValue( 30 );
  
  checkChanged();
}

  void
LaunchConfig::checkChanged()
{
  KConfig c("klaunchrc", false, false);

  c.setGroup("FeedbackStyle");

  bool savedBusyCursor =
    c.readBoolEntry("BusyCursor", Default & BusyCursor);

  bool savedTaskbarButton =
    c.readBoolEntry("TaskbarButton", Default & TaskbarButton);

  c.setGroup( "BusyCursorSettings" );
  unsigned int savedCursorTimeout = c.readUnsignedNumEntry( "Timeout", 30 );
  bool savedBusyBlinking =c.readBoolEntry("Blinking", true);

  c.setGroup( "TaskbarButtonSettings" );
  unsigned int savedTaskbarTimeout = c.readUnsignedNumEntry( "Timeout", 30 );

  bool newBusyCursor =cb_busyCursor->isChecked();

  bool newTaskbarButton =cb_taskbarButton->isChecked();

  bool newBusyBlinking= cb_busyBlinking->isChecked();

  unsigned int newCursorTimeout = sb_cursorTimeout->value();
  
  unsigned int newTaskbarTimeout = sb_taskbarTimeout->value();
  
  emit
    (
     changed
     (
      savedBusyCursor     != newBusyCursor
      ||
      savedTaskbarButton  != newTaskbarButton
      ||
      savedCursorTimeout  != newCursorTimeout
      ||
      savedTaskbarTimeout != newTaskbarTimeout
      ||
      savedBusyBlinking != newBusyBlinking
     )
    );
}

  QString
LaunchConfig::quickHelp() const
{
  return i18n
    (
     "<h1>Launch</h1>"
     " You can configure the application-launch feedback here."
    );
}

#include "kcmlaunch.moc"
