/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 */

#include <qwidget.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>
#include <kpassdlg.h>

#include <kdesu/defaults.h>
#include <kdesu/client.h>

#include "passwords.h"

/**
 * DLL interface.
 */
extern "C" {
    KCModule *create_passwords(QWidget *parent, const char *name) {
    KGlobal::locale()->insertCatalogue("passwords");
    return new KPasswordConfig(parent, name);
    }
}


/**** KPasswordConfig ****/

KPasswordConfig::KPasswordConfig(QWidget *parent, const char *name)
    : KCModule(parent, name)
{
    QVBoxLayout *top = new QVBoxLayout(this, 10, 10);

    // Echo mode
    m_EMGroup = new QButtonGroup(i18n("Echo characters as"), this);
    QWhatsThis::add( m_EMGroup,  i18n("You can select the type of visual feedback given"
      " when you enter a password in kdesu. Choose one of the following options:<p>"
      " <ul><li><em>1 star:</em> each character you type is shown as an asterisk (*) symbol.</li>"
      " <li><em>3 stars:</em> three asterisks are shown for each character you type.</li>"
      " <li><em>no echo:</em> there is no visual feedback at all, so nothing on your screen"
      " shows how many characters are in your password.</li></ul>"));
    top->addWidget(m_EMGroup);
    QVBoxLayout *vbox = new QVBoxLayout(m_EMGroup, 10, 10);
    vbox->addSpacing(10);
    QRadioButton *rb = new QRadioButton(i18n("1 star"), m_EMGroup);
    vbox->addWidget(rb, 0, AlignLeft);
    rb = new QRadioButton(i18n("3 stars"), m_EMGroup);
    vbox->addWidget(rb, 0, AlignLeft);
    rb = new QRadioButton(i18n("no echo"), m_EMGroup);
    vbox->addWidget(rb, 0, AlignLeft);
    connect(m_EMGroup, SIGNAL(clicked(int)), SLOT(slotEchoMode(int)));

    // Keep password

    m_KeepBut = new QCheckBox(i18n("&Remember passwords"), this);
    QWhatsThis::add( m_KeepBut, i18n("If this option is checked, kdesu will remember your passwords"
       " for the specified amount of time. Until then, you won't have to enter your password again."
       " Keep in mind that this option is insecure and may enable others to damage your system.<p>"
       " Please <em>do not</em> use this option if you are working in an insecure environment,"
       " for example, on a workstation that is located in a publicly accessible area.<p>"
       " This option does not affect passwords explicitly set in other applications, for example,"
       " your email account password in KMail.") );
    connect(m_KeepBut, SIGNAL(toggled(bool)), SLOT(slotKeep(bool)));
    top->addWidget(m_KeepBut);
    QHBoxLayout *hbox = new QHBoxLayout();
    top->addLayout(hbox);
    QLabel *lbl = new QLabel(i18n("&Timeout"), this);
    lbl->setFixedSize(lbl->sizeHint());
    hbox->addSpacing(20);
    hbox->addWidget(lbl);
    m_TimeoutEdit = new QSpinBox(this);
    QString wtstr = i18n("You can specify how long kdesu will remember your"
       " passwords. A short timeout is more secure than a long timeout.");
    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_TimeoutEdit, wtstr );
    lbl->setBuddy(m_TimeoutEdit);
    m_TimeoutEdit->setRange(5, 1200);
    m_TimeoutEdit->setSteps(5, 10);
    m_TimeoutEdit->setSuffix(i18n(" minutes"));
    m_TimeoutEdit->setFixedSize(m_TimeoutEdit->sizeHint());
    hbox->addWidget(m_TimeoutEdit);
    hbox->addStretch();

    top->addStretch();

    m_pConfig = KGlobal::config();
    load();
}


KPasswordConfig::~KPasswordConfig()
{
}


void KPasswordConfig::load()
{
    KConfigGroupSaver saver(m_pConfig, "Passwords");

    QString val = m_pConfig->readEntry("EchoMode", "x");
    if (val == "OneStar")
    m_Echo = KPasswordEdit::OneStar;
    else if (val == "ThreeStars")
    m_Echo = KPasswordEdit::ThreeStars;
    else if (val == "NoEcho")
    m_Echo = KPasswordEdit::NoEcho;
    else
    m_Echo = defEchoMode;

    m_bKeep = m_pConfig->readBoolEntry("Keep", defKeep);
    m_Timeout = m_pConfig->readNumEntry("Timeout", defTimeout);

    apply();
    emit changed(false);
}


void KPasswordConfig::save()
{
    KConfigGroupSaver saver(m_pConfig, "Passwords");

    QString val;
    if (m_Echo == KPasswordEdit::OneStar)
    val = "OneStar";
    else if (m_Echo == KPasswordEdit::ThreeStars)
    val = "ThreeStars";
    else
    val = "NoEcho";
    m_pConfig->writeEntry("EchoMode", val, true, true);

    m_pConfig->writeEntry("Keep", m_bKeep, true, true);
    m_Timeout = m_TimeoutEdit->value()*60;
    m_pConfig->writeEntry("Timeout", m_Timeout, true, true);

    m_pConfig->sync();

    if (!m_bKeep) {
    // Try to stop daemon
    KDEsuClient client;
    if (client.ping() != -1)
        client.stopServer();
    }
    emit changed(false);
}


void KPasswordConfig::defaults()
{
    m_Echo = defEchoMode;
    m_bKeep = defKeep;
    m_Timeout = defTimeout;

    apply();
    emit changed(true);
}


void KPasswordConfig::apply()
{
    m_EMGroup->setButton(m_Echo);
    m_KeepBut->setChecked(m_bKeep);

    m_TimeoutEdit->setValue(m_Timeout/60);
    m_TimeoutEdit->setEnabled(m_bKeep);
}


void KPasswordConfig::slotEchoMode(int i)
{
    m_Echo = i;
    emit changed(true);
}


void KPasswordConfig::slotKeep(bool keep)
{
    m_bKeep = keep;
    m_TimeoutEdit->setEnabled(m_bKeep);
    emit changed(true);
}


int KPasswordConfig::buttons()
{
    return KCModule::Help | KCModule::Default | KCModule::Apply ;
}

QString KPasswordConfig::quickHelp() const
{
    return i18n("<h1>Passwords</h1> This module gives you options for"
       " configuring the way in which the \"kdesu\" program treats passwords."
       " Kdesu will ask you for a password when you try to carry out some"
       " privileged actions, such as changing the date/time stored in your"
       " system clock, or adding new users on your computer.<p>"
       " You can configure the type of visual feedback given when you type"
       " a password, and whether kdesu should remember passwords for some"
       " time after you have given them.<p>"
       " Note that these settings affect <em>only</em> kdesu. This means that"
       " the behavior of passwords in KMail and other programs cannot be"
       " configured here.");
}

#include "passwords.moc"
