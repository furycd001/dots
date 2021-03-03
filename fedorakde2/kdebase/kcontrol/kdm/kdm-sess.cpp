/* This file is part of the KDE Display Manager Configuration package
    Copyright (C) 1997-1998 Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <unistd.h>
#include <sys/types.h>


#include <qdir.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <ksimpleconfig.h>
#include <kdbtn.h>
#include <klineedit.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kdialog.h>
#include <kurlrequester.h>

#include "kdm-sess.moc"


extern KSimpleConfig *c;

KDMSessionsWidget::KDMSessionsWidget(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
      QString wtstr;


      QGroupBox *group0 = new QGroupBox( i18n("Allow shutdown"), this );

      sdlcombo = new QComboBox( FALSE, group0 );
      sdllabel = new QLabel (sdlcombo, i18n ("Co&nsole:"), group0);
      sdlcombo->insertItem(i18n("Everybody"), SdAll);
      sdlcombo->insertItem(i18n("Only root"), SdRoot);
      sdlcombo->insertItem(i18n("Nobody"), SdNone);
      connect(sdlcombo, SIGNAL(activated(int)), this, SLOT(changed()));
      sdrcombo = new QComboBox( FALSE, group0 );
      sdrlabel = new QLabel (sdrcombo, i18n ("Re&mote:"), group0);
      sdrcombo->insertItem(i18n("Everybody"), SdAll);
      sdrcombo->insertItem(i18n("Only root"), SdRoot);
      sdrcombo->insertItem(i18n("Nobody"), SdNone);
      connect(sdrcombo, SIGNAL(activated(int)), this, SLOT(changed()));
      QWhatsThis::add( group0, i18n("Here you can select who is allowed to shutdown"
        " the computer using KDM. You can specify different values for local (console) and remote displays. "
	"Possible values are:<ul>"
        " <li><em>Everybody:</em> everybody can shutdown the computer using KDM</li>"
        " <li><em>Only root:</em> KDM will only allow shutdown after the user has entered the root password</li>"
        " <li><em>Nobody:</em> nobody can shutdown the computer using KDM</li></ul>") );


      QGroupBox *group1 = new QGroupBox( i18n("Commands"), this );

      shutdown_lined = new KURLRequester(group1);
      QLabel *shutdown_label = new QLabel(shutdown_lined, i18n("Ha&lt"), group1);
      connect(shutdown_lined->lineEdit(), SIGNAL(textChanged(const QString&)),
	      this, SLOT(changed()));
      wtstr = i18n("Command to initiate the system halt. Typical value: /sbin/halt");
      QWhatsThis::add( shutdown_label, wtstr );
      QWhatsThis::add( shutdown_lined, wtstr );

      restart_lined = new KURLRequester(group1);
      QLabel *restart_label = new QLabel(restart_lined, i18n("&Reboot"), group1);
      connect(restart_lined->lineEdit(), SIGNAL(textChanged(const QString&)),
	      this, SLOT(changed()));
      wtstr = i18n("Command to initiate the system reboot. Typical value: /sbin/reboot");
      QWhatsThis::add( restart_label, wtstr );
      QWhatsThis::add( restart_lined, wtstr );

#ifdef __linux__
      QGroupBox *group4 = new QGroupBox( i18n("Lilo"), this );

      lilo_check = new QCheckBox(i18n("Show boot opt&ions"), group4);
      connect(lilo_check, SIGNAL(toggled(bool)),
	      this, SLOT(slotLiloCheckToggled(bool)));
      connect(lilo_check, SIGNAL(toggled(bool)),
	      this, SLOT(changed()));
      wtstr = i18n("Enable Lilo boot options in the \"Shutdown ...\" dialog.");
      QWhatsThis::add( lilo_check, wtstr );

      lilocmd_lined = new KURLRequester(group4);
      lilocmd_label = new QLabel(lilocmd_lined , i18n("Lilo command"), group4);
      connect(lilocmd_lined, SIGNAL(textChanged(const QString&)),
          this, SLOT(changed()));
      wtstr = i18n("Command to run Lilo. Typical value: /sbin/lilo");
      QWhatsThis::add( lilocmd_label, wtstr );
      QWhatsThis::add( lilocmd_lined, wtstr );

      lilomap_lined = new KURLRequester(group4);
      lilomap_label = new QLabel(lilomap_lined, i18n("Lilo map file"), group4);
      connect(lilomap_lined, SIGNAL(textChanged(const QString&)),
	      this, SLOT(changed()));
      wtstr = i18n("Position of Lilo's map file. Typical value: /boot/map");
      QWhatsThis::add( lilomap_label, wtstr );
      QWhatsThis::add( lilomap_lined, wtstr );
#endif

      QGroupBox *group2 = new QGroupBox( i18n("Session types"), this );

      session_lined = new QLineEdit(group2);
      QLabel *type_label = new QLabel(session_lined, i18n("New t&ype"), group2);
      connect(session_lined, SIGNAL(textChanged(const QString&)),
	      SLOT(slotCheckNewSession(const QString&)));
      connect(session_lined, SIGNAL(returnPressed()),
	      SLOT(slotAddSessionType()));
      connect(session_lined, SIGNAL(returnPressed()),
	      this, SLOT(changed()));
      wtstr = i18n( "To create a new session type, enter its name here and click on <em>Add new</em>" );
      QWhatsThis::add( type_label, wtstr );
      QWhatsThis::add( session_lined, wtstr );

      btnadd = new QPushButton( i18n("Add ne&w"), group2 );
      btnadd->setEnabled(false);
      connect( btnadd, SIGNAL( clicked() ), SLOT( changed() ) );
      connect( btnadd, SIGNAL( clicked() ), SLOT( slotAddSessionType() ) );
      QWhatsThis::add( btnadd, i18n( "Click here to add the new session type entered in the <em>New type</em> field to the list of available sessions." ) );

      sessionslb = new MyListBox(group2);
      QLabel *types_label = new QLabel(sessionslb, i18n("Available &types"), group2);
      connect(sessionslb, SIGNAL(highlighted(int)),
	      SLOT(slotSessionHighlighted(int)));
      wtstr = i18n( "This box lists the available session types that will be presented to the user."
		    " Names other than \"default\" and \"failsafe\" are usually treated as program names,"
		    " but it depends on your Xsession script what the session type means." );
      QWhatsThis::add( types_label, wtstr );
      QWhatsThis::add( sessionslb, wtstr );

      btnrm = new QPushButton( i18n("R&emove"), group2 );
      btnrm->setEnabled(false);
      connect( btnrm, SIGNAL( clicked() ), SLOT( slotRemoveSessionType() ) );
      connect( btnrm, SIGNAL( clicked() ), SLOT( changed() ) );
      QWhatsThis::add( btnrm, i18n( "Click here to remove the currently selected session type" ) );

      btnup = new KDirectionButton(UpArrow, group2);
      btnup->setEnabled(false);
      connect(btnup, SIGNAL( clicked() ), SLOT( slotSessionUp() ));
      connect(btnup, SIGNAL( clicked() ), SLOT( changed() ));
      btndown = new KDirectionButton(DownArrow, group2);
      btndown->setEnabled(false);
      btndown->setFixedSize(20, 20);
      connect(btndown,SIGNAL( clicked() ), SLOT( slotSessionDown() ));
      connect(btndown,SIGNAL( clicked() ), SLOT( changed() ));
      wtstr = i18n( "With these two arrow buttons, you can change the order in which the available session types are presented to the user" );
      QWhatsThis::add( btnup, wtstr );
      QWhatsThis::add( btndown, wtstr );

      QBoxLayout *main = new QVBoxLayout( this, 10 );
      QGridLayout *lgroup0 = new QGridLayout( group0, 3, 5, 10);
      QGridLayout *lgroup1 = new QGridLayout( group1, 3, 5, 10);
#ifdef __linux__
      QGridLayout *lgroup4 = new QGridLayout( group4, 3, 4, 10);
#endif
      QGridLayout *lgroup2 = new QGridLayout( group2, 5, 5, 10);

      main->addWidget(group0);
      main->addWidget(group1);
#ifdef __linux__
      main->addWidget(group4);
#endif
      main->addWidget(group2);

      lgroup0->addRowSpacing(0, group0->fontMetrics().height()/2);
      lgroup0->addColSpacing(2, KDialog::spacingHint() * 2);
      lgroup0->setColStretch(1, 1);
      lgroup0->setColStretch(4, 1);
      lgroup0->addWidget(sdllabel, 1, 0);
      lgroup0->addWidget(sdlcombo, 1, 1);
      lgroup0->addWidget(sdrlabel, 1, 3);
      lgroup0->addWidget(sdrcombo, 1, 4);

      lgroup1->addRowSpacing(0, group1->fontMetrics().height()/2);
      lgroup1->addColSpacing(2, KDialog::spacingHint() * 2);
      lgroup1->setColStretch(1, 1);
      lgroup1->setColStretch(4, 1);
      lgroup1->addWidget(shutdown_label, 1, 0);
      lgroup1->addWidget(shutdown_lined, 1, 1);
      lgroup1->addWidget(restart_label, 1, 3);
      lgroup1->addWidget(restart_lined, 1, 4);

#ifdef __linux__
      lgroup4->addRowSpacing(0, group4->fontMetrics().height()/2);
      lgroup4->addColSpacing(1, KDialog::spacingHint() * 2);
      lgroup4->setColStretch(3, 1);
      lgroup4->addWidget(lilo_check, 1, 0);
      lgroup4->addWidget(lilocmd_label, 1, 2);
      lgroup4->addWidget(lilocmd_lined, 1, 3);
      lgroup4->addWidget(lilomap_label, 2, 2);
      lgroup4->addWidget(lilomap_lined, 2, 3);
#endif

      lgroup2->addRowSpacing(0, group2->fontMetrics().height()/2);
      lgroup2->addWidget(type_label, 1, 0);
      lgroup2->addMultiCellWidget(session_lined, 2, 2, 0, 2);
      lgroup2->addWidget(types_label, 1, 3);
      lgroup2->addMultiCellWidget(sessionslb, 2, 5, 3, 3);
      lgroup2->addWidget(btnadd, 3, 0);
      lgroup2->addWidget(btnrm, 3, 2);
      lgroup2->addWidget(btnup, 2, 4);
      lgroup2->addWidget(btndown, 3, 4);
      lgroup2->setColStretch(1, 1);
      lgroup2->setColStretch(3, 2);
      lgroup2->setRowStretch(4, 1);

      main->activate();

      load();

    // read only mode
    if (getuid() != 0)
    {
      sdlcombo->setEnabled(false);
      sdrcombo->setEnabled(false);

      restart_lined->lineEdit()->setReadOnly(true);
      restart_lined->button()->setEnabled(false);
      shutdown_lined->lineEdit()->setReadOnly(true);
      shutdown_lined->button()->setEnabled(false);
#ifdef __linux__
      lilo_check->setEnabled(false);
      lilocmd_lined->lineEdit()->setReadOnly(true);
      lilocmd_lined->button()->setEnabled(false);
      lilomap_lined->lineEdit()->setReadOnly(true);
      lilomap_lined->button()->setEnabled(false);
#endif
      session_lined->setReadOnly(true);
      sessionslb->setEnabled(false);
      btnup->setEnabled(false);
      btndown->setEnabled(false);
      btnrm->setEnabled(false);
      btnadd->setEnabled(false);
    }
}

void KDMSessionsWidget::slotLiloCheckToggled(bool on)
{
#ifdef __linux__
    lilocmd_label->setEnabled(on);
    lilocmd_lined->setEnabled(on);
    lilomap_label->setEnabled(on);
    lilomap_lined->setEnabled(on);
#endif
}

void KDMSessionsWidget::slotSessionHighlighted(int s)
{
  session_lined->setText(sessionslb->text(s));
  btnup->setEnabled(s > 0);
  btndown->setEnabled(s < (int)sessionslb->count()-1);
  btnrm->setEnabled(sessionslb->currentItem() > -1);
  if(!sessionslb->isItemVisible(s))
    sessionslb->centerCurrentItem();
}

void KDMSessionsWidget::slotCheckNewSession(const QString& str)
{
  btnadd->setEnabled(!str.isEmpty());
}

void KDMSessionsWidget::slotSessionUp()
{
  moveSession(-1);
}

void KDMSessionsWidget::slotSessionDown()
{
  moveSession(1);
}

void KDMSessionsWidget::moveSession(int d)
{
  int id = sessionslb->currentItem();
  QString str = sessionslb->text(id);
  sessionslb->removeItem(id);
  sessionslb->insertItem(str, id+d);
  sessionslb->setCurrentItem(id+d);
}

void KDMSessionsWidget::slotAddSessionType()
{
  if(!session_lined->text().isEmpty())
  {
    sessionslb->insertItem(session_lined->text());
    session_lined->setText("");
  }
}

void KDMSessionsWidget::slotRemoveSessionType()
{
  int i = sessionslb->currentItem();
  if(i > -1)
    sessionslb->removeItem(i);
}

void KDMSessionsWidget::writeSD(QComboBox *combo)
{
    QString what;
    switch (combo->currentItem()) {
    case SdAll: what = "All"; break;
    case SdRoot: what = "Root"; break;
    default: what = "None"; break;
    }
    c->writeEntry( "AllowShutdown", what);
}

void KDMSessionsWidget::save()
{
    c->setGroup("X-:*-Greeter");
    writeSD(sdlcombo);

    c->setGroup("X-*-Greeter");
    writeSD(sdrcombo);
    QString sesstr;
    for(uint i = 0; i < sessionslb->count(); i++)
    {
      sesstr.append(sessionslb->text(i));
      sesstr.append(",");
    }
    c->writeEntry( "SessionTypes", sesstr );

    c->setGroup("Shutdown");
    c->writeEntry("HaltCmd", shutdown_lined->lineEdit()->text(), true);
    c->writeEntry("RebootCmd", restart_lined->lineEdit()->text(), true);
#ifdef __linux__
    c->writeEntry("UseLilo", lilo_check->isChecked());
    c->writeEntry("LiloCmd", lilocmd_lined->lineEdit()->text());
    c->writeEntry("LiloMap", lilomap_lined->lineEdit()->text());
#endif
}

void KDMSessionsWidget::readSD(QComboBox *combo, QString def)
{
  QString str = c->readEntry("AllowShutdown", def);
  SdModes sdMode;
  if(str == "All")
    sdMode = SdAll;
  else if(str == "Root")
    sdMode = SdRoot;
  else
    sdMode = SdNone;
  combo->setCurrentItem(sdMode);
}

void KDMSessionsWidget::load()
{
  QString str;

  c->setGroup("X-:*-Greeter");
  readSD(sdlcombo, "All");

  c->setGroup("X-*-Greeter");
  readSD(sdrcombo, "Root");
  QStringList sessions = c->readListEntry( "SessionTypes");
  if (sessions.isEmpty())
    sessions << "default" << "kde" << "failsafe";
  sessionslb->clear();
  sessionslb->insertStringList(sessions);

  c->setGroup("Shutdown");
  restart_lined->lineEdit()->setText(c->readEntry("RebootCmd", "/sbin/reboot"));
  shutdown_lined->lineEdit()->setText(c->readEntry("HaltCmd", "/sbin/halt"));
#ifdef __linux__
  bool lien = c->readBoolEntry("UseLilo", false);
  lilo_check->setChecked(lien);
  slotLiloCheckToggled(lien);
  lilocmd_lined->lineEdit()->setText(c->readEntry("LiloCmd", "/sbin/lilo"));
  lilomap_lined->lineEdit()->setText(c->readEntry("LiloMap", "/boot/map"));
#endif
}



void KDMSessionsWidget::defaults()
{
  restart_lined->lineEdit()->setText("/sbin/reboot");
  shutdown_lined->lineEdit()->setText("/sbin/halt");

  sdlcombo->setCurrentItem(SdAll);
  sdrcombo->setCurrentItem(SdRoot);

  sessionslb->clear();
  sessionslb->insertItem("default");
  sessionslb->insertItem("kde");
  sessionslb->insertItem("failsafe");

#ifdef __linux__
    lilo_check->setChecked(false);
    slotLiloCheckToggled(false);

    lilocmd_lined->lineEdit()->setText("/sbin/lilo");
    lilomap_lined->lineEdit()->setText("/boot/map");
#endif
}


void KDMSessionsWidget::changed()
{
  emit KCModule::changed(true);
}
