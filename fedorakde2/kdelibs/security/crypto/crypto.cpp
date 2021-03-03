/**
 * crypto.cpp
 *
 * Copyright (c) 2000-2001 George Staikos <staikos@kde.org>
 *               2000 Carsten Pfeiffer <pfeiffer@kde.org>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//
//   Attention.  The suck factor on this code is increasing.  It's a bit of a
//   hack. </understatement>  It might be time to rewrite it soon.
//

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

#include <qfile.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#include <kfiledialog.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kpassdlg.h>
#include <kseparator.h>
#include <kdatepik.h>
#include <kurllabel.h>

#include <qframe.h>


#include <kdebug.h>

#include <config.h>
#ifdef HAVE_SSL
#define crypt _openssl_crypt
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#undef crypt
#endif

#include <ksslall.h>
#include <kopenssl.h>

#include "crypto.h"
#include "certexport.h"
#include "kdatetimedlg.h"


CipherItem::CipherItem( QListView *view, const QString& cipher, int bits,
			int maxBits, KCryptoConfig *module )
    : QCheckListItem( view, QString::null, CheckBox )
{
    m_cipher = cipher;
    m_bits = bits;
    m_module = module;

    QString tmp( i18n("%1 (%2 of %3 bits)") );
    setText( 0, tmp.arg( cipher ).arg( bits ).arg( maxBits ));
}

void CipherItem::stateChange( bool )
{
    m_module->configChanged();
}

QString CipherItem::configName() const
{
    QString cipherName("cipher_%1");
    return cipherName.arg( m_cipher );
}



OtherCertItem::OtherCertItem( QListView *view, QString& sub, bool perm, int policy, QDateTime exp, KCryptoConfig *module )
    : QListViewItem( view, QString::null ), _sub(sub), _exp(exp), _perm(perm), _policy(policy)

{
    m_module = module;
KSSLX509Map cert(sub);
    setText(0, cert.getValue("O"));
    setText(1, cert.getValue("CN"));

    if (_exp.date().year() > 3000 || _exp.date().year() < 1900)
       _exp.setDate(QDate(3000,1,1));
}

void OtherCertItem::stateChange( bool )
{
    m_module->configChanged();
}

QString OtherCertItem::configName() const
{
    return _sub;
}


YourCertItem::YourCertItem( QListView *view, QString pkcs, QString pass, QString name, KCryptoConfig *module )
    : QListViewItem( view, QString::null )

{
    m_module = module;
KSSLX509Map cert(name);
    setText(0, cert.getValue("CN"));
    setText(1, cert.getValue("Email"));
    _pkcs = pkcs;
    _name = name;
    _pass = pass;
}

void YourCertItem::stateChange( bool )
{
    m_module->configChanged();
}

QString YourCertItem::configName() const
{
    return _name;
}




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////




KCryptoConfig::KCryptoConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
QGridLayout *grid;
QBoxLayout *top = new QVBoxLayout(this);
QString whatstr;

  ___lehack = false;
  otherCertDelList.setAutoDelete(true);
  yourCertDelList.setAutoDelete(true);
  authDelList.setAutoDelete(true);

  ///////////////////////////////////////////////////////////////////////////
  // Create the GUI here - there are currently a total of 4 tabs.
  // The first is SSL and cipher related
  // The second is OpenSSL settings
  // The third is user's SSL certificate related
  // The fourth is certificate authentication related
  // The fifth is other SSL certificate related
  // The sixth is CA related
  // The seventh is misc. settings related
  ///////////////////////////////////////////////////////////////////////////

  tabs = new QTabWidget(this);
  top->addWidget(tabs);

  ///////////////////////////////////////////////////////////////////////////
  // FIRST TAB
  ///////////////////////////////////////////////////////////////////////////
  tabSSL = new QFrame(this);
  grid = new QGridLayout(tabSSL, 7, 2, KDialog::marginHint(),
                                       KDialog::spacingHint() );
  mUseTLS = new QCheckBox(i18n("Enable &TLS support if supported by the server."), tabSSL);
  connect(mUseTLS, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mUseTLS, 0, 0);
  whatstr = i18n("TLS is the newest revision of the SSL protocol."
                 "  It integrates better with other protocols and has"
                 " replaced SSL in protocols such as POP3 and SMTP.");
  QWhatsThis::add(mUseTLS, whatstr);

  mUseSSLv2 = new QCheckBox(i18n("Enable SSLv&2"), tabSSL);
  connect(mUseSSLv2, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mUseSSLv2, 1, 0);
  whatstr = i18n("SSL v2 is the second revision of the SSL protocol."
                " It is most common to enable v2 and v3.");
  QWhatsThis::add(mUseSSLv2, whatstr);

  mUseSSLv3 = new QCheckBox(i18n("Enable SSLv&3"), tabSSL);
  connect(mUseSSLv3, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mUseSSLv3, 1, 1);
  whatstr = i18n("SSL v3 is the third revision of the SSL protocol."
                " It is most common to enable v2 and v3.");
  QWhatsThis::add(mUseSSLv3, whatstr);

#ifdef HAVE_SSL
  SSLv2Box = new QListView(tabSSL, "v2ciphers");
  (void) SSLv2Box->addColumn(i18n("SSLv2 Ciphers To Use:"));
  whatstr = i18n("Select the ciphers you wish to enable when using the"
                " SSL v2 protocol.  The actual protocol used will be"
                " negotiated with the server at connection time.");
  QWhatsThis::add(SSLv2Box, whatstr);
  SSLv2Box->setSelectionMode(QListView::NoSelection);

  grid->addWidget( SSLv2Box, 2, 0 );
  connect( mUseSSLv2, SIGNAL( toggled( bool ) ),
	   SSLv2Box, SLOT( setEnabled( bool )));
#else
  QLabel *nossllabel = new QLabel(i18n("SSL ciphers cannot be configured"
                               " because this module was not linked"
                               " with OpenSSL."), tabSSL);
  grid->addMultiCellWidget(nossllabel, 2, 2, 0, 1);
  grid->addRowSpacing( 3, 100 ); // give minimum height to look better
#endif

  // no need to parse kdeglobals.
  config = new KConfig("cryptodefaults", false, false);
  policies = new KSimpleConfig("ksslpolicies", false);
  pcerts = new KSimpleConfig("ksslcertificates", false);
  authcfg = new KSimpleConfig("ksslauthmap", false);

#ifdef HAVE_SSL
  SSLv3Box = new QListView(tabSSL, "v3ciphers");
  (void) SSLv3Box->addColumn(i18n("SSLv3 Ciphers To Use:"));
  whatstr = i18n("Select the ciphers you wish to enable when using the"
                " SSL v3 protocol.  The actual protocol used will be"
                " negotiated with the server at connection time.");
  QWhatsThis::add(SSLv3Box, whatstr);
  SSLv3Box->setSelectionMode(QListView::NoSelection);
  grid->addWidget(SSLv3Box, 2, 1);
  connect( mUseSSLv3, SIGNAL( toggled( bool ) ),
	   SSLv3Box, SLOT( setEnabled( bool )));

  loadCiphers();

  //
  //  CipherWizards
  //
  QHButtonGroup *cwbg = new QHButtonGroup(i18n("Cipher Wizards..."), tabSSL);
  mCWcompatible = new QPushButton(i18n("Most &Compatible"), cwbg);
  mCWus = new QPushButton(i18n("&US Ciphers Only"), cwbg);
  mCWexp = new QPushButton(i18n("E&xport Ciphers Only"), cwbg);
  mCWall = new QPushButton(i18n("Enable &All"), cwbg);
  connect(mCWcompatible, SIGNAL(clicked()), SLOT(slotCWcompatible()));
  connect(mCWus, SIGNAL(clicked()), SLOT(slotCWus()));
  connect(mCWexp, SIGNAL(clicked()), SLOT(slotCWexp()));
  connect(mCWall, SIGNAL(clicked()), SLOT(slotCWall()));
  grid->addMultiCellWidget(cwbg, 3, 3, 0, 1);
  whatstr = i18n("Use these buttons to more easily configure the SSL encryption settings.");
  QWhatsThis::add(cwbg, whatstr);
  whatstr = i18n("Select the settings found to be most compatible.");
  QWhatsThis::add(mCWcompatible, whatstr);
  whatstr = i18n("Select only the US strong (>= 128 bit) encryption ciphers.");
  QWhatsThis::add(mCWus, whatstr);
  whatstr = i18n("Select only the weak ciphers (<= 56 bit).");
  QWhatsThis::add(mCWexp, whatstr);
  whatstr = i18n("Select all SSL ciphers and methods.");
  QWhatsThis::add(mCWall, whatstr);


  //
  //  Settings for the EGD
  //
  mUseEGD = new QCheckBox(i18n("Use EGD"), tabSSL);
  connect(mUseEGD, SIGNAL(clicked()), SLOT(slotUseEGD()));
  grid->addWidget(mUseEGD, 4, 0);
  QFrame *egdframe = new QFrame(tabSSL);
  QGridLayout *grid2 = new QGridLayout(egdframe, 2, 2, KDialog::marginHint(),
                                                      KDialog::spacingHint() );
  mEGDLabel = new QLabel(i18n("Path to EGD:"), egdframe);
  grid2->addWidget(mEGDLabel, 0, 0);
  mEGDPath = new QLineEdit(egdframe);
  connect(mEGDPath, SIGNAL(textChanged(const QString&)), SLOT(configChanged()));
  grid2->addWidget(mEGDPath, 1, 0);
  mChooseEGD = new QPushButton("...", egdframe);
  connect(mChooseEGD, SIGNAL(clicked()), SLOT(slotChooseEGD()));
  grid2->addWidget(mChooseEGD, 1, 1);
  grid->addWidget(egdframe, 4, 1);
  whatstr = i18n("If selected, OpenSSL will be asked to use the entropy gathering"
          " daemon (EGD) for initializing the pseudo-random number generator.");
  QWhatsThis::add(mUseEGD, whatstr);
  whatstr = i18n("Enter the path to the socket created by the entropy gathering"
                " daemon here.");
  QWhatsThis::add(mChooseEGD, whatstr);
  QWhatsThis::add(mEGDPath, whatstr);
  whatstr = i18n("Click here to browse for the EGD socket file.");
  QWhatsThis::add(mEGDPath, whatstr);



  
#endif

  mWarnOnEnter = new QCheckBox(i18n("Warn on &entering SSL mode"), tabSSL);
  connect(mWarnOnEnter, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mWarnOnEnter, 5, 0);
  whatstr = i18n("If selected, you will be notified when entering an SSL"
                " enabled site");
  QWhatsThis::add(mWarnOnEnter, whatstr);

  mWarnOnLeave = new QCheckBox(i18n("Warn on &leaving SSL mode"), tabSSL);
  connect(mWarnOnLeave, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mWarnOnLeave, 5, 1);
  whatstr = i18n("If selected, you will be notified when leaving an SSL"
                " based site.");
  QWhatsThis::add(mWarnOnLeave, whatstr);

#if 0  // NOT IMPLEMENTED IN KDE 2.0
  mWarnOnUnencrypted = new QCheckBox(i18n("Warn on sending &unencrypted data"), tabSSL);
  connect(mWarnOnUnencrypted, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mWarnOnUnencrypted, 6, 0);
  whatstr = i18n("If selected, you will be notified before sending"
                " unencrypted data via a web browser.");
  QWhatsThis::add(mWarnOnUnencrypted, whatstr);

  mWarnOnMixed = new QCheckBox(i18n("Warn on &mixed SSL/non-SSL pages"), tabSSL);
  connect(mWarnOnMixed, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addWidget(mWarnOnMixed, 6, 1);
  whatstr = i18n("If selected, you will be notified if you view a page"
                " that has both encrypted and non-encrypted parts.");
  QWhatsThis::add(mWarnOnMixed, whatstr);
#endif

  ///////////////////////////////////////////////////////////////////////////
  // SECOND TAB
  ///////////////////////////////////////////////////////////////////////////

#ifdef HAVE_SSL
  tabOSSL = new QFrame(this);
  grid = new QGridLayout(tabOSSL, 6, 6, KDialog::marginHint(), KDialog::spacingHint());

  oInfo = new QLabel(i18n("Enter the path to your OpenSSL shared libraries:"), tabOSSL);
  grid->addMultiCellWidget(oInfo, 0, 0, 0, 5);
  oPath = new QLineEdit(tabOSSL);
  grid->addMultiCellWidget(oPath, 1, 1, 0, 4);
  oFind = new QPushButton(i18n("..."), tabOSSL);
  grid->addWidget(oFind, 1, 5);
  oTest = new QPushButton(i18n("&Test..."), tabOSSL);
  grid->addWidget(oTest, 2, 5);
  connect(oTest, SIGNAL(clicked()), SLOT(slotTestOSSL()));

  connect(oPath, SIGNAL(textChanged(const QString&)), SLOT(configChanged()));
  connect(oFind, SIGNAL(clicked()), SLOT(slotChooseOSSL()));

#endif



  ///////////////////////////////////////////////////////////////////////////
  // THIRD TAB
  ///////////////////////////////////////////////////////////////////////////
  tabYourSSLCert = new QFrame(this);

#ifdef HAVE_SSL
  grid = new QGridLayout(tabYourSSLCert, 15, 6, KDialog::marginHint(), KDialog::spacingHint() );

  yourSSLBox = new QListView(tabYourSSLCert);
  whatstr = i18n("This list box shows which certificates of yours KDE"
                " knows about.  You can easily manage them from here.");
  QWhatsThis::add(yourSSLBox, whatstr);
  grid->addMultiCellWidget(yourSSLBox, 0, 5, 0, 4);
  yourSSLBox->addColumn(i18n("Common Name"));
  yourSSLBox->addColumn(i18n("Email Address"));
  connect(yourSSLBox, SIGNAL(selectionChanged()), SLOT(slotYourCertSelect()));

  yourSSLImport = new QPushButton(i18n("&Import..."), tabYourSSLCert);
  connect(yourSSLImport, SIGNAL(clicked()), SLOT(slotYourImport()));
  grid->addWidget(yourSSLImport, 0, 5);

  yourSSLExport = new QPushButton(i18n("&Export..."), tabYourSSLCert);
  yourSSLExport->setEnabled(false);
  connect(yourSSLExport, SIGNAL(clicked()), SLOT(slotYourExport()));
  grid->addWidget(yourSSLExport, 1, 5);

  yourSSLRemove = new QPushButton(i18n("Remo&ve"), tabYourSSLCert);
  yourSSLRemove->setEnabled(false);
  connect(yourSSLRemove, SIGNAL(clicked()), SLOT(slotYourRemove()));
  grid->addWidget(yourSSLRemove, 2, 5);

  yourSSLUnlock = new QPushButton(i18n("&Unlock"), tabYourSSLCert);
  yourSSLUnlock->setEnabled(false);
  connect(yourSSLUnlock, SIGNAL(clicked()), SLOT(slotYourUnlock()));
  grid->addWidget(yourSSLUnlock, 3, 5);

  yourSSLVerify = new QPushButton(i18n("Verif&y..."), tabYourSSLCert);
  yourSSLVerify->setEnabled(false);
  connect(yourSSLVerify, SIGNAL(clicked()), SLOT(slotYourVerify()));
  grid->addWidget(yourSSLVerify, 4, 5);

  yourSSLPass = new QPushButton(i18n("&Change Password..."), tabYourSSLCert);
  yourSSLPass->setEnabled(false);
  connect(yourSSLPass, SIGNAL(clicked()), SLOT(slotYourPass()));
  grid->addWidget(yourSSLPass, 5, 5);

  grid->addMultiCellWidget(new KSeparator(KSeparator::HLine, tabYourSSLCert), 6, 6, 0, 5);
  ySubject = KSSLInfoDlg::certInfoWidget(tabYourSSLCert, QString(""));
  yIssuer = KSSLInfoDlg::certInfoWidget(tabYourSSLCert, QString(""));
  grid->addMultiCellWidget(ySubject, 7, 11, 0, 2);
  grid->addMultiCellWidget(yIssuer, 7, 11, 3, 5);
  whatstr = i18n("This is the information known about the owner of the certificate.");
  QWhatsThis::add(ySubject, whatstr);
  whatstr = i18n("This is the information known about the issuer of the certificate.");
  QWhatsThis::add(yIssuer, whatstr);

  grid->addWidget(new QLabel(i18n("Valid From:"), tabYourSSLCert), 12, 0);
  grid->addWidget(new QLabel(i18n("Valid Until:"), tabYourSSLCert), 13, 0);
  yValidFrom = new QLabel(tabYourSSLCert);
  grid->addWidget(yValidFrom, 12, 1);
  yValidUntil = new QLabel(tabYourSSLCert);
  grid->addWidget(yValidUntil, 13, 1);
  whatstr = i18n("The certificate is valid starting at this date.");
  QWhatsThis::add(yValidFrom, whatstr);
  whatstr = i18n("The certificate is valid until this date.");
  QWhatsThis::add(yValidUntil, whatstr);

#if 0
  QHButtonGroup *ocbg = new QHButtonGroup(i18n("On SSL Connection..."), tabYourSSLCert);
  yourSSLUseDefault = new QRadioButton(i18n("&Use default certificate"), ocbg);
  yourSSLList = new QRadioButton(i18n("&List upon connection"), ocbg);
  yourSSLDont = new QRadioButton(i18n("&Do not use certificates"), ocbg);
  grid->addMultiCellWidget(ocbg, 14, 14, 0, 5);
#endif
#else
  nossllabel = new QLabel(i18n("SSL certificates cannot be managed"
                               " because this module was not linked"
                               " with OpenSSL."), tabYourSSLCert);
  grid->addMultiCellWidget(nossllabel, 3, 3, 0, 5);
#endif


  ///////////////////////////////////////////////////////////////////////////
  // FOURTH TAB
  ///////////////////////////////////////////////////////////////////////////
  tabAuth = new QFrame(this);

#ifdef HAVE_SSL
  grid = new QGridLayout(tabAuth, 20, 6, KDialog::marginHint(), KDialog::spacingHint());

  grid->addMultiCellWidget(new QLabel(i18n("Default Authentication Certificate"), tabAuth), 0, 0, 0, 2);
  defCertBG = new QVButtonGroup(i18n("Default Action..."), tabAuth);
  defSend = new QRadioButton(i18n("&Send"), defCertBG);
  defPrompt = new QRadioButton(i18n("&Prompt"), defCertBG);
  defDont = new QRadioButton(i18n("D&on't Send"), defCertBG);
  grid->addMultiCellWidget(defCertBG, 1, 3, 0, 2);
  grid->addMultiCellWidget(new QLabel(i18n("Default Certificate:"), tabAuth), 1, 1, 3, 5);
  defCertBox = new QComboBox(false, tabAuth);
  grid->addMultiCellWidget(defCertBox, 2, 2, 3, 5);
  
  grid->addMultiCellWidget(new KSeparator(KSeparator::HLine, tabAuth), 4, 4, 0, 5);


  grid->addMultiCellWidget(new QLabel(i18n("Host Authentication"), tabAuth), 5, 5, 0, 1);
  hostAuthList = new QListView(tabAuth);
  grid->addMultiCellWidget(hostAuthList, 6, 13, 0, 5);
  hostAuthList->addColumn(i18n("Host"));
  hostAuthList->addColumn(i18n("Certificate"));
  hostAuthList->addColumn(i18n("Policy"));

  grid->addWidget(new QLabel(i18n("Host:"), tabAuth), 14, 0);
  grid->addWidget(new QLabel(i18n("Certificate:"), tabAuth), 15, 0);

  authHost = new QLineEdit(tabAuth);
  grid->addMultiCellWidget(authHost, 14, 14, 1, 4);
  hostCertBox = new QComboBox(false, tabAuth);
  grid->addMultiCellWidget(hostCertBox, 15, 15, 1, 4);

  hostCertBG = new QHButtonGroup(i18n("Action..."), tabAuth);
  hostSend = new QRadioButton(i18n("Send"), hostCertBG);
  hostPrompt = new QRadioButton(i18n("Prompt"), hostCertBG);
  hostDont = new QRadioButton(i18n("Don't Send"), hostCertBG);
  grid->addMultiCellWidget(hostCertBG, 16, 16, 0, 5);

  authAdd = new QPushButton(i18n("Ne&w"), tabAuth);
  authRemove = new QPushButton(i18n("Remo&ve"), tabAuth);
  grid->addWidget(authAdd, 17, 4);
  grid->addWidget(authRemove, 17, 5);

  authHost->setEnabled(false);
  hostCertBox->setEnabled(false);
  hostCertBG->setEnabled(false);
  authRemove->setEnabled(false);

  connect(defCertBox, SIGNAL(activated(int)), this, SLOT(configChanged()));
  connect(defCertBG, SIGNAL(clicked(int)), this, SLOT(configChanged()));
  connect(hostAuthList, SIGNAL(selectionChanged()), this, SLOT(slotAuthItemChanged()));
  connect(authAdd, SIGNAL(clicked()), this, SLOT(slotNewHostAuth()));
  connect(authRemove, SIGNAL(clicked()), this, SLOT(slotRemoveHostAuth()));
  connect(authHost, SIGNAL(textChanged(const QString &)), this, SLOT(slotAuthText(const QString &)));
  connect(hostCertBG, SIGNAL(clicked(int)), this, SLOT(slotAuthButtons()));
  connect(hostCertBox, SIGNAL(activated(int)), this, SLOT(slotAuthCombo()));

#else
  nossllabel = new QLabel(i18n("SSL certificates cannot be managed"
                               " because this module was not linked"
                               " with OpenSSL."), tabAuth);
  grid->addMultiCellWidget(nossllabel, 3, 3, 0, 5);
#endif



  ///////////////////////////////////////////////////////////////////////////
  // FIFTH TAB
  ///////////////////////////////////////////////////////////////////////////
  tabOtherSSLCert = new QFrame(this);

#ifdef HAVE_SSL
  oGrid = grid = new QGridLayout(tabOtherSSLCert, 20, 6, KDialog::marginHint(), KDialog::spacingHint());

  otherSSLBox = new QListView(tabOtherSSLCert);
  connect(otherSSLBox, SIGNAL(selectionChanged()), SLOT(slotOtherCertSelect()));
  whatstr = i18n("This list box shows which site and person certificates KDE"
                " knows about.  You can easily manage them from here.");
  QWhatsThis::add(otherSSLBox, whatstr);
  otherSSLBox->addColumn(i18n("Organization"));
  otherSSLBox->addColumn(i18n("Common Name"));
  grid->addMultiCellWidget(otherSSLBox, 0, 7, 0, 4);

  otherSSLExport = new QPushButton(i18n("&Export..."), tabOtherSSLCert);
  connect(otherSSLExport, SIGNAL(clicked()), SLOT(slotExportCert()));
  grid->addWidget(otherSSLExport, 0, 5);
  whatstr = i18n("This button allows you to export the selected certificate"
                " to a file of various formats.");
  QWhatsThis::add(otherSSLExport, whatstr);

  otherSSLRemove = new QPushButton(i18n("&Remove..."), tabOtherSSLCert);
  connect(otherSSLRemove, SIGNAL(clicked()), SLOT(slotRemoveCert()));
  grid->addWidget(otherSSLRemove, 1, 5);
  whatstr = i18n("This button removes the selected certificate"
                " from the certificate cache.");
  QWhatsThis::add(otherSSLRemove, whatstr);

  otherSSLVerify = new QPushButton(i18n("&Verify..."), tabOtherSSLCert);
  connect(otherSSLVerify, SIGNAL(clicked()), SLOT(slotVerifyCert()));
  grid->addWidget(otherSSLVerify, 2, 5);
  whatstr = i18n("This button tests the selected certificate"
                " for validity.");
  QWhatsThis::add(otherSSLVerify, whatstr);

      otherSSLExport->setEnabled(false);
      otherSSLVerify->setEnabled(false);
      otherSSLRemove->setEnabled(false);

  grid->addMultiCellWidget(new KSeparator(KSeparator::HLine, tabOtherSSLCert), 8, 8, 0, 5);
  oSubject = KSSLInfoDlg::certInfoWidget(tabOtherSSLCert, QString(""));
  oIssuer = KSSLInfoDlg::certInfoWidget(tabOtherSSLCert, QString(""));
  grid->addMultiCellWidget(oSubject, 9, 13, 0, 2);
  grid->addMultiCellWidget(oIssuer, 9, 13, 3, 5);
  whatstr = i18n("This is the information known about the owner of the certificate.");
  QWhatsThis::add(oSubject, whatstr);
  whatstr = i18n("This is the information known about the issuer of the certificate.");
  QWhatsThis::add(oIssuer, whatstr);

  grid->addWidget(new QLabel(i18n("Valid From:"), tabOtherSSLCert), 14, 0);
  grid->addWidget(new QLabel(i18n("Valid Until:"), tabOtherSSLCert), 15, 0);
  validFrom = new QLabel(tabOtherSSLCert);
  grid->addWidget(validFrom, 14, 1);
  validUntil = new QLabel(tabOtherSSLCert);
  grid->addWidget(validUntil, 15, 1);
  whatstr = i18n("The certificate is valid starting at this date.");
  QWhatsThis::add(validFrom, whatstr);
  whatstr = i18n("The certificate is valid until this date.");
  QWhatsThis::add(validUntil, whatstr);

  grid->addWidget(new QLabel(i18n("Cache..."), tabOtherSSLCert), 16, 0);
  cachePerm = new QRadioButton(i18n("Permanentl&y"), tabOtherSSLCert);
  cacheUntil = new QRadioButton(i18n("&Until..."), tabOtherSSLCert);
  cachePerm->setEnabled(false);
  cacheUntil->setEnabled(false);
  grid->addWidget(cachePerm, 17, 0);
  grid->addWidget(cacheUntil, 18, 0);
  connect(cachePerm, SIGNAL(clicked()), SLOT(slotPermanent()));
  connect(cacheUntil, SIGNAL(clicked()), SLOT(slotUntil()));
  untilDate = new KURLLabel("", "", tabOtherSSLCert);
  grid->addWidget(untilDate, 18, 1);
  connect(untilDate, SIGNAL(leftClickedURL()), SLOT(slotDatePick()));
  untilDate->setEnabled(false);
  whatstr = i18n("Select here to make the cache entry permanent.");
  QWhatsThis::add(cachePerm, whatstr);
  whatstr = i18n("Select here to make the cache entry temporary.");
  QWhatsThis::add(cacheUntil, whatstr);
  whatstr = i18n("The date and time until the certificate cache entry should expire.");
  QWhatsThis::add(untilDate, whatstr);

  policyGroup = new QVButtonGroup(i18n("Policy"), tabOtherSSLCert);
  policyAccept = new QRadioButton(i18n("A&ccept"), policyGroup);
  policyReject = new QRadioButton(i18n("Re&ject"), policyGroup);
  policyPrompt = new QRadioButton(i18n("&Prompt"), policyGroup);
  policyGroup->setEnabled(false);
  grid->addMultiCellWidget(policyGroup, 14, 17, 3, 5);
  connect(policyGroup, SIGNAL(clicked(int)), SLOT(slotPolicyChanged(int)));
  whatstr = i18n("Select this to always accept this certificate.");
  QWhatsThis::add(policyAccept, whatstr);
  whatstr = i18n("Select this to always reject this certificate.");
  QWhatsThis::add(policyReject, whatstr);
  whatstr = i18n("Select this if you wish to be prompted for action when receiving this certificate.");
  QWhatsThis::add(policyPrompt, whatstr);

#else
  nossllabel = new QLabel(i18n("SSL certificates cannot be managed"
                               " because this module was not linked"
                               " with OpenSSL."), tabOtherSSLCert);
  grid->addMultiCellWidget(nossllabel, 1, 1, 0, 1);
#endif


#if 0
  ///////////////////////////////////////////////////////////////////////////
  // SIXTH TAB
  ///////////////////////////////////////////////////////////////////////////
  tabSSLCA = new QFrame(this);

#ifdef HAVE_SSL
  grid = new QGridLayout(tabSSLCA, 8, 2, KDialog::marginHint(), KDialog::spacingHint());

  caSSLBox = new QListBox(tabSSLCA);
  whatstr = i18n("This list box shows which certificate authorities KDE"
                " knows about.  You can easily manage them from here.");
  QWhatsThis::add(caSSLBox, whatstr);
  caSSLBox->setSelectionMode(QListBox::Single);
  caSSLBox->setColumnMode(QListBox::FixedNumber);
  grid->addMultiCellWidget(caSSLBox, 0, 7, 0, 0);

  caSSLImport = new QPushButton(i18n("&Import..."), tabSSLCA);
  //connect(caSSLImport, SIGNAL(), SLOT());
  grid->addWidget(caSSLImport, 0, 1);

  caSSLView = new QPushButton(i18n("&View/Edit..."), tabSSLCA);
  //connect(caSSLView, SIGNAL(), SLOT());
  grid->addWidget(caSSLView, 1, 1);

  caSSLRemove = new QPushButton(i18n("&Remove..."), tabSSLCA);
  //connect(caSSLRemove, SIGNAL(), SLOT());
  grid->addWidget(caSSLRemove, 2, 1);

  caSSLVerify = new QPushButton(i18n("Verif&y..."), tabSSLCA);
  //connect(caSSLVerify, SIGNAL(), SLOT());
  grid->addWidget(caSSLVerify, 3, 1);

#else
  nossllabel = new QLabel(i18n("SSL certificates cannot be managed"
                               " because this module was not linked"
                               " with OpenSSL."), tabSSLCA);
  grid->addMultiCellWidget(nossllabel, 1, 1, 0, 1);
#endif

  ///////////////////////////////////////////////////////////////////////////
  // SEVENTH TAB
  ///////////////////////////////////////////////////////////////////////////
  tabSSLCOpts = new QFrame(this);

#ifdef HAVE_SSL
  grid = new QGridLayout(tabSSLCOpts, 9, 4, KDialog::marginHint(), KDialog::spacingHint());
  mWarnSelfSigned = new QCheckBox(i18n("Warn on &self-signed certificates or unknown CA's"), tabSSLCOpts);
  connect(mWarnSelfSigned, SIGNAL(clicked()), SLOT(configChanged()));
  mWarnExpired = new QCheckBox(i18n("Warn on &expired certificates"), tabSSLCOpts);
  connect(mWarnExpired, SIGNAL(clicked()), SLOT(configChanged()));
  mWarnRevoked = new QCheckBox(i18n("Warn on re&voked certificates"), tabSSLCOpts);
  connect(mWarnRevoked, SIGNAL(clicked()), SLOT(configChanged()));
  grid->addMultiCellWidget(mWarnSelfSigned, 0, 0, 0, 3);
  grid->addMultiCellWidget(mWarnExpired, 1, 1, 0, 3);
  grid->addMultiCellWidget(mWarnRevoked, 2, 2, 0, 3);

  macCert = new QLineEdit(tabSSLCOpts);
  grid->addMultiCellWidget(macCert, 4, 4, 0, 2);

  macBox = new QListBox(tabSSLCOpts);
  whatstr = i18n("This list box shows which sites you have decided to accept"
                " a certificate from even though the certificate might fail"
                " the validation procedure.");
  QWhatsThis::add(macBox, whatstr);
  caSSLBox->setSelectionMode(QListBox::Single);
  caSSLBox->setColumnMode(QListBox::FixedNumber);
  grid->addMultiCellWidget(macBox, 5, 8, 0, 2);

  macAdd = new QPushButton(i18n("&Add"), tabSSLCOpts);
  //connect(macAdd, SIGNAL(), SLOT());
  grid->addWidget(macAdd, 4, 3);

  macRemove = new QPushButton(i18n("&Remove"), tabSSLCOpts);
  //connect(macRemove, SIGNAL(), SLOT());
  grid->addWidget(macRemove, 5, 3);

  macClear = new QPushButton(i18n("&Clear"), tabSSLCOpts);
  //connect(macAdd, SIGNAL(), SLOT());
  grid->addWidget(macClear, 6, 3);

#else
  nossllabel = new QLabel(i18n("These options are not configurable"
                               " because this module was not linked"
                               " with OpenSSL."), tabSSLCOpts);
  grid->addMultiCellWidget(nossllabel, 1, 1, 0, 1);
#endif
#endif

  ///////////////////////////////////////////////////////////////////////////
  // Add the tabs and startup
  ///////////////////////////////////////////////////////////////////////////
  tabs->addTab(tabSSL, i18n("SSL"));
#ifdef HAVE_SSL
  tabs->addTab(tabOSSL, i18n("OpenSSL"));
#endif
  tabs->addTab(tabYourSSLCert, i18n("Your Certificates"));
  tabs->addTab(tabAuth, i18n("Authentication"));
  tabs->addTab(tabOtherSSLCert, i18n("Peer SSL Certificates"));

#if 0
  tabs->addTab(tabSSLCA, i18n("SSL C.A.s"));
  tabs->addTab(tabSSLCOpts, i18n("Validation Options"));
#endif

  tabs->resize(tabs->sizeHint());
  load();
}

KCryptoConfig::~KCryptoConfig()
{
    delete config;
    delete policies;
    delete pcerts;
    delete authcfg;
}

void KCryptoConfig::configChanged()
{
    emit changed(true);
}


void KCryptoConfig::load()
{
#ifdef HAVE_SSL
  otherCertDelList.clear();
  yourCertDelList.clear();
  authDelList.clear();
  config->setGroup("TLS");
  mUseTLS->setChecked(config->readBoolEntry("Enabled", true));

  config->setGroup("SSLv2");
  mUseSSLv2->setChecked(config->readBoolEntry("Enabled", true));

  config->setGroup("SSLv3");
  mUseSSLv3->setChecked(config->readBoolEntry("Enabled", true));

  config->setGroup("Warnings");
  mWarnOnEnter->setChecked(config->readBoolEntry("OnEnter", false));
  mWarnOnLeave->setChecked(config->readBoolEntry("OnLeave", true));

  config->setGroup("EGD");
  mUseEGD->setChecked(config->readBoolEntry("UseEGD", false));
  mEGDLabel->setEnabled(mUseEGD->isChecked());
  mChooseEGD->setEnabled(mUseEGD->isChecked());
  mEGDPath->setEnabled(mUseEGD->isChecked());
  mEGDPath->setText(config->readEntry("EGDPath", ""));

#ifdef HAVE_SSL
  config->setGroup("OpenSSL");
  oPath->setText(config->readEntry("Path", ""));
#endif

#if 0 // NOT IMPLEMENTED IN KDE 2.0
  mWarnOnUnencrypted->setChecked(config->readBoolEntry("OnUnencrypted", false));
  mWarnOnMixed->setChecked(config->readBoolEntry("OnMixed", true));

  config->setGroup("Validation");
  mWarnSelfSigned->setChecked(config->readBoolEntry("WarnSelfSigned", true));
  mWarnExpired->setChecked(config->readBoolEntry("WarnExpired", true));
  mWarnRevoked->setChecked(config->readBoolEntry("WarnRevoked", true));
#endif

  config->setGroup("SSLv2");
  CipherItem *item = static_cast<CipherItem *>(SSLv2Box->firstChild());
  while ( item ) {
      item->setOn(config->readBoolEntry(item->configName(),
					item->bits() >= 40));
      item = static_cast<CipherItem *>(item->nextSibling());
  }

  config->setGroup("SSLv3");
  item = static_cast<CipherItem *>(SSLv3Box->firstChild());
  while ( item ) {
      item->setOn(config->readBoolEntry(item->configName(),
					item->bits() >= 40));
      item = static_cast<CipherItem *>(item->nextSibling());
  }

  SSLv2Box->setEnabled( mUseSSLv2->isChecked() );
  SSLv3Box->setEnabled( mUseSSLv3->isChecked() );

  QStringList groups = policies->groupList();
 
  otherSSLBox->clear();
  for (QStringList::Iterator i = groups.begin();
                             i != groups.end();
                             ++i) {
    if ((*i).isEmpty() || *i == "<default>") continue;
    policies->setGroup(*i);
    new OtherCertItem(otherSSLBox, *i, 
                      policies->readBoolEntry("Permanent", true), 
                      policies->readNumEntry("Policy", 3),
                      policies->readDateTimeEntry("Expires"), this );
  }

  groups = pcerts->groupList();
 
  yourSSLBox->clear();
  for (QStringList::Iterator i = groups.begin();
                             i != groups.end();
                             ++i) {
    if ((*i).isEmpty() || *i == "<default>") continue;
    pcerts->setGroup(*i);
    YourCertItem *j = new YourCertItem(yourSSLBox,
                     pcerts->readEntry("PKCS12Base64", ""), 
                     pcerts->readEntry("Password", ""),
                     *i, this );
    j->setPassCache("");
  }

  setAuthCertLists();

  config->setGroup("Auth");
  QString whichAuth = config->readEntry("AuthMethod", "none");
  if (whichAuth == "send")
    defCertBG->setButton(defCertBG->id(defSend));
  else if (whichAuth == "prompt")
    defCertBG->setButton(defCertBG->id(defPrompt));
  else
    defCertBG->setButton(defCertBG->id(defDont));
  
  QString whichCert = config->readEntry("DefaultCert", "");
  defCertBox->setCurrentItem(0);
  for (int i = 0; i < defCertBox->count(); i++) {
     if (defCertBox->text(i) == whichCert) {
       defCertBox->setCurrentItem(i);
       break;
     }
  }
  
  groups = authcfg->groupList();
  for (QStringList::Iterator i = groups.begin();
                             i != groups.end();
                             ++i) {
    if ((*i).isEmpty() || *i == "<default>") continue;
    authcfg->setGroup(*i);
    KSSLCertificateHome::KSSLAuthAction aa = KSSLCertificateHome::AuthDont;
    if (authcfg->readBoolEntry("send", false) == true) 
       aa = KSSLCertificateHome::AuthSend;
    else if (authcfg->readBoolEntry("prompt", false) == true) 
       aa = KSSLCertificateHome::AuthPrompt;
    HostAuthItem *j = new HostAuthItem(hostAuthList,
                                       *i,
                                       authcfg->readEntry("certificate", ""), 
                                       this );
    j->setAction(aa);
    j->setOriginalName(*i);
  }

  
  slotOtherCertSelect();
  slotYourCertSelect();
#endif

  emit changed(false);
}

void KCryptoConfig::save()
{
#ifdef HAVE_SSL
  if (!mUseSSLv2->isChecked() &&
      !mUseSSLv3->isChecked())
    KMessageBox::information(this, i18n("If you don't select at least one"
                                       " SSL algorithm, either SSL will not"
                                       " work or the application may be"
                                       " forced to choose a suitable default."),
                                   i18n("SSL"));

  config->setGroup("TLS");
  config->writeEntry("Enabled", mUseTLS->isChecked());

  config->setGroup("SSLv2");
  config->writeEntry("Enabled", mUseSSLv2->isChecked());

  config->setGroup("SSLv3");
  config->writeEntry("Enabled", mUseSSLv3->isChecked());

  config->setGroup("Warnings");
  config->writeEntry("OnEnter", mWarnOnEnter->isChecked());
  config->writeEntry("OnLeave", mWarnOnLeave->isChecked());

  config->setGroup("EGD");
  config->writeEntry("UseEGD", mUseEGD->isChecked());
  config->writeEntry("EGDPath", mEGDPath->text());

#ifdef HAVE_SSL
  config->setGroup("OpenSSL");
  config->writeEntry("Path", oPath->text());
#endif

#if 0  // NOT IMPLEMENTED IN KDE 2.0
  config->writeEntry("OnUnencrypted", mWarnOnUnencrypted->isChecked());
  config->writeEntry("OnMixed", mWarnOnMixed->isChecked());

  config->setGroup("Validation");
  config->writeEntry("WarnSelfSigned", mWarnSelfSigned->isChecked());
  config->writeEntry("WarnExpired", mWarnExpired->isChecked());
  config->writeEntry("WarnRevoked", mWarnRevoked->isChecked());
#endif

  int ciphercount = 0;
  config->setGroup("SSLv2");
  CipherItem *item = static_cast<CipherItem *>(SSLv2Box->firstChild());
  while ( item ) {
    if (item->isOn()) {
      config->writeEntry(item->configName(), true);
      ciphercount++;
    } else config->writeEntry(item->configName(), false);

    item = static_cast<CipherItem *>(item->nextSibling());
  }

  if (mUseSSLv2->isChecked() && ciphercount == 0)
    KMessageBox::information(this, i18n("If you don't select at least one"
                                       " cipher, SSLv2 will not work."),
                                   i18n("SSLv2 Ciphers"));

  ciphercount = 0;
  config->setGroup("SSLv3");
  item = static_cast<CipherItem *>(SSLv3Box->firstChild());
  while ( item ) {
    if (item->isOn()) {
      config->writeEntry(item->configName(), true);
      ciphercount++;
    } else config->writeEntry(item->configName(), false);

    item = static_cast<CipherItem *>(item->nextSibling());
  }

  if (mUseSSLv3->isChecked() && ciphercount == 0)
    KMessageBox::information(this, i18n("If you don't select at least one"
                                       " cipher, SSLv3 will not work."),
                                   i18n("SSLv3 Ciphers"));
  // SSL Policies code
  for (OtherCertItem *x = otherCertDelList.first(); x != 0; x = otherCertDelList.next()) {
     policies->deleteGroup(x->configName());
     otherCertDelList.remove(x);
  }
  // Go through the non-deleted ones and save them
  for (OtherCertItem *x = 
        static_cast<OtherCertItem *>(otherSSLBox->firstChild()); 
                                                              x;
             x = static_cast<OtherCertItem *>(x->nextSibling())) {
     policies->setGroup(x->configName());
     policies->writeEntry("Policy", x->getPolicy());
     policies->writeEntry("Expires", x->getExpires());
     policies->writeEntry("Permanent", x->isPermanent());
  }

  // SSL Personal certificates code
  for (YourCertItem *x = yourCertDelList.first(); x != 0; x = yourCertDelList.next()) {
     pcerts->deleteGroup(x->configName());
     yourCertDelList.remove(x);
  }
  // Go through the non-deleted ones and save them
  for (YourCertItem *x = 
        static_cast<YourCertItem *>(yourSSLBox->firstChild()); 
                                                            x;
             x = static_cast<YourCertItem *>(x->nextSibling())) {
     pcerts->setGroup(x->configName());
     pcerts->writeEntry("PKCS12Base64", x->getPKCS());
     pcerts->writeEntry("Password", x->getPass());
  }

  config->setGroup("Auth");
  QString whichAuth = config->readEntry("AuthMethod", "none");
  if (defCertBG->selected() == defSend)
    config->writeEntry("AuthMethod", "send");
  else if (defCertBG->selected() == defPrompt)
    config->writeEntry("AuthMethod", "prompt");
  else
    config->writeEntry("AuthMethod", "none");

  if (defCertBox->currentItem() == 0)
     config->writeEntry("DefaultCert", "");
  else config->writeEntry("DefaultCert", defCertBox->currentText());

  for (HostAuthItem *x = authDelList.first(); x != 0; x = authDelList.next()) {
     authcfg->deleteGroup(x->configName());
     authDelList.remove(x);
  }

  for (HostAuthItem *x = 
        static_cast<HostAuthItem *>(hostAuthList->firstChild()); 
                                                              x;
             x = static_cast<HostAuthItem *>(x->nextSibling())) {
     if (x->originalName() != QString::null)
        authcfg->deleteGroup(x->originalName());
  }

  for (HostAuthItem *x = 
        static_cast<HostAuthItem *>(hostAuthList->firstChild()); 
                                                              x;
             x = static_cast<HostAuthItem *>(x->nextSibling())) {
     authcfg->setGroup(x->configName());
     authcfg->writeEntry("certificate", x->getCertName());
     authcfg->writeEntry("prompt", (x->getAction() == KSSLCertificateHome::AuthPrompt));
     authcfg->writeEntry("send", (x->getAction() == KSSLCertificateHome::AuthSend));
  }

#endif

  config->sync();
  policies->sync();
  pcerts->sync();

  // insure proper permissions -- contains sensitive data
  QString cfgName(KGlobal::dirs()->findResource("config", "cryptodefaults"));
  if (!cfgName.isEmpty())
    ::chmod(QFile::encodeName(cfgName), 0600);

  emit changed(false);
}

void KCryptoConfig::defaults()
{
  mUseTLS->setChecked(true);
  mUseSSLv2->setChecked(true);
  mUseSSLv3->setChecked(false);  // GS 3/2001 - this seems to be more compatible
  mWarnOnEnter->setChecked(false);
  mWarnOnLeave->setChecked(true);
#if 0  // NOT IMPLEMENTED IN KDE 2.0
  mWarnOnUnencrypted->setChecked(false);
  mWarnOnMixed->setChecked(true);
  mWarnSelfSigned->setChecked(true);
  mWarnExpired->setChecked(true);
  mWarnRevoked->setChecked(true);
#endif

#ifdef HAVE_SSL
    // We don't want to make
    // ciphers < 40 bit a default selection.  This is very unsafe and
    // I have already witnessed OpenSSL negotiate a 0 bit connection
    // on me after tracing the https ioslave on a suspicion.

  CipherItem *item;
  for ( item = static_cast<CipherItem *>(SSLv2Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 56 );
  }

  for ( item = static_cast<CipherItem *>(SSLv3Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 56 );
  }
  mUseEGD->setChecked(false);
  mEGDLabel->setEnabled(false);
  mChooseEGD->setEnabled(false);
  mEGDPath->setEnabled(false);
  mEGDPath->setText("");
  oPath->setText("");

  defCertBG->setButton(defCertBG->id(defDont));
#endif

  emit changed(true);
}

QString KCryptoConfig::quickHelp() const
{
  return i18n("<h1>crypto</h1> This module allows you to configure SSL for"
     " use with most KDE applications, as well as manage your personal"
     " certificates and the known certificate authorities.");
}


void KCryptoConfig::slotCWcompatible() {
  #ifdef HAVE_SSL
  CipherItem *item;
  for ( item = static_cast<CipherItem *>(SSLv2Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 56 && item->bits() <= 128 );
  }

  for ( item = static_cast<CipherItem *>(SSLv3Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 56 && item->bits() <= 128 );
  }

  mUseTLS->setChecked(true);
  mUseSSLv2->setChecked(true);
  mUseSSLv3->setChecked(true);
  configChanged();
  #endif
}


void KCryptoConfig::slotCWus() {
  #ifdef HAVE_SSL
  CipherItem *item;
  for ( item = static_cast<CipherItem *>(SSLv2Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 128 );
  }

  for ( item = static_cast<CipherItem *>(SSLv3Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() >= 128 );
  }

  configChanged();
  #endif
}


void KCryptoConfig::slotCWexp() {
  #ifdef HAVE_SSL
  CipherItem *item;
  for ( item = static_cast<CipherItem *>(SSLv2Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() <= 56 && item->bits() > 0);
  }

  for ( item = static_cast<CipherItem *>(SSLv3Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( item->bits() <= 56 && item->bits() > 0);
  }

  configChanged();
  #endif
}


void KCryptoConfig::slotCWall() {
  #ifdef HAVE_SSL
  CipherItem *item;
  for ( item = static_cast<CipherItem *>(SSLv2Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( true );
  }

  for ( item = static_cast<CipherItem *>(SSLv3Box->firstChild()); item;
	item = static_cast<CipherItem *>(item->nextSibling()) ) {
    item->setOn( true );
  }

  mUseTLS->setChecked(true);
  mUseSSLv2->setChecked(true);
  mUseSSLv3->setChecked(true);
  configChanged();
  #endif
}



void KCryptoConfig::slotExportCert() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());
   if (x) {
     policies->setGroup(x->getSub());
     KSSLCertificate *cert = KSSLCertificate::fromString(policies->readEntry("Certificate", "").local8Bit());
     if (cert) {
        KCertExport kce;
        kce.setCertificate(cert);
        kce.exec();
        delete cert;
     } else KMessageBox::sorry(this, i18n("Couldn't open the certificate."), 
                                     i18n("SSL"));
   }
}



void KCryptoConfig::slotRemoveCert() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());
   if (x) {
      otherSSLBox->takeItem(x);
      otherCertDelList.append(x);
      configChanged();
   }
}



void KCryptoConfig::slotVerifyCert() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());
  if (!x) return;

  policies->setGroup(x->getSub());
  KSSLCertificate *cert = KSSLCertificate::fromString(policies->readEntry("Certificate", "").local8Bit());

  if (!cert) {
    KMessageBox::error(this, i18n("Error obtaining the certificate."), i18n("SSL"));
    return;
  }
 
  cert->chain().setChain(policies->readListEntry("Chain"));

  if (cert->isValid()) {
     KMessageBox::information(this, i18n("This certificate passed the verification tests successfully."), i18n("SSL"));
  } else {
     KMessageBox::detailedError(this, i18n("This certificate has failed the tests and should be considered invalid."), KSSLCertificate::verifyText(cert->validate()), i18n("SSL"));
  }

  delete cert;
}



void KCryptoConfig::slotUntil() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());

   cachePerm->setChecked(false);
   untilDate->setEnabled(true);

   if (!x) return;
   x->setPermanent(false);

   configChanged();
}



void KCryptoConfig::slotPermanent() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());

   cacheUntil->setChecked(false);
   untilDate->setEnabled(false);

   if (!x) return;
   x->setPermanent(true);

   configChanged();
}



void KCryptoConfig::slotPolicyChanged(int id) {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());
if (!x) return;

   if (id == policyGroup->id(policyAccept)) {
     x->setPolicy(KSSLCertificateCache::Accept);
   } else if (id == policyGroup->id(policyReject)) {
     x->setPolicy(KSSLCertificateCache::Reject);
   } else if (id == policyGroup->id(policyPrompt)) {
     x->setPolicy(KSSLCertificateCache::Prompt);
   }

   configChanged();
}



void KCryptoConfig::slotDatePick() {
KDateTimeDlg kdtd;
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());

   if (!x || !untilDate->isEnabled()) return;

QDateTime qdt = x->getExpires();

   kdtd.setDateTime(qdt);
   int rc = kdtd.exec();
   if (rc == KDialog::Accepted) {
      x->setExpires(kdtd.getDateTime());
      untilDate->setText(x->getExpires().toString());
      configChanged();
   }
}



void KCryptoConfig::slotOtherCertSelect() {
OtherCertItem *x = static_cast<OtherCertItem *>(otherSSLBox->selectedItem());
QString iss = "";
   if (x) {
      otherSSLExport->setEnabled(true);
      otherSSLVerify->setEnabled(true);
      otherSSLRemove->setEnabled(true);
      policyGroup->setEnabled(true);
      cachePerm->setEnabled(true);
      cacheUntil->setEnabled(true);
      policies->setGroup(x->getSub());

      KSSLCertificate *cert = KSSLCertificate::fromString(policies->readEntry("Certificate", "").local8Bit());

      if (cert) {
         QPalette cspl;
         iss = cert->getIssuer();
         cspl = validFrom->palette();
         if (QDateTime::currentDateTime() < cert->getQDTNotBefore()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         validFrom->setPalette(cspl);

         cspl = validUntil->palette();
         if (QDateTime::currentDateTime() > cert->getQDTNotAfter()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         validUntil->setPalette(cspl);

         validFrom->setText(cert->getNotBefore());
         validUntil->setText(cert->getNotAfter());
         untilDate->setText(x ? x->getExpires().toString()
                              : QDateTime::currentDateTime().toString());
         untilDate->setEnabled(x && !x->isPermanent());
         delete cert;
      } else {
         validFrom->setText("");
         validUntil->setText("");
      }

      switch(x->getPolicy()) {
      case KSSLCertificateCache::Accept:
        policyGroup->setButton(policyGroup->id(policyAccept)); 
      break;
      case KSSLCertificateCache::Reject:
        policyGroup->setButton(policyGroup->id(policyReject)); 
      break;
      case KSSLCertificateCache::Prompt:
        policyGroup->setButton(policyGroup->id(policyPrompt)); 
      break;
      }

      cachePerm->setChecked(x->isPermanent());
      cacheUntil->setChecked(!x->isPermanent());

   } else {
      otherSSLExport->setEnabled(false);
      otherSSLVerify->setEnabled(false);
      otherSSLRemove->setEnabled(false);
      policyGroup->setEnabled(false);
      cachePerm->setChecked(false);
      cacheUntil->setChecked(false);
      policyAccept->setChecked(false);
      policyReject->setChecked(false);
      policyPrompt->setChecked(false);
      cachePerm->setEnabled(false);
      cacheUntil->setEnabled(false);
      validFrom->setText("");
      validUntil->setText("");
      untilDate->setText("");
      untilDate->setEnabled(false);
   }


   oSubject->setValues(x ? x->getSub() : QString(""));
   oIssuer->setValues(iss);

}


void KCryptoConfig::slotYourImport() {
#ifdef HAVE_SSL
KSSLPKCS12 *cert = NULL;
QCString pass;

   QString certFile = KFileDialog::getOpenFileName();
   if (certFile.isEmpty())
      return;

TryImportPassAgain:
   int rc = KPasswordDialog::getPassword(pass, i18n("Certificate password")); 
   if (rc != KPasswordDialog::Accepted) return;

   cert = KSSLPKCS12::loadCertFile(certFile, QString(pass));

   if (!cert) {
      rc = KMessageBox::warningYesNo(this, i18n("The certificate file could not be loaded.  Try a different password?"), i18n("SSL"));
      if (rc == KMessageBox::Yes) goto TryImportPassAgain;
      return;
   }

   // At this point, we know that we can read the certificate in.
   // The procedure will be to convert it to Base64 in it's raw form
   // and add it to the ListView - eventually going into the SimpleConfig.
   
   // FIXME: prompt if the user wants the password stored along with the
   //        certificate

   QString name = cert->getCertificate()->getSubject();
   for (YourCertItem *i =
        static_cast<YourCertItem *>(yourSSLBox->firstChild());
                                                            i;
             i = static_cast<YourCertItem *>(i->nextSibling())) {
      if (i->configName() == name) {
         rc = KMessageBox::warningYesNo(this, i18n("A certificate with that name already exists.  Are you sure that you wish to replace it?"), i18n("SSL"));
         if (rc == KMessageBox::No) {
            delete cert;
            return;
         }
         yourSSLBox->takeItem(i);
         yourCertDelList.append(i);
      }
   }

   new YourCertItem(yourSSLBox,
                    cert->toString(), 
                    "",  // the password - don't store it yet!
                    name, 
                    this );

   setAuthCertLists();
   configChanged();
   delete cert;
#endif
}


void KCryptoConfig::slotYourExport() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
   if (!x) return;

   KSSLPKCS12 *pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPass());
   if (!pkcs)
      pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPassCache());
   if (!pkcs) {
      QString pprompt = i18n("Enter the certificate password:");
      QCString oldpass;
      do {
         int i = KPasswordDialog::getPassword(oldpass, pprompt);
         if (i != KPasswordDialog::Accepted) return; 
         pkcs = KSSLPKCS12::fromString(x->getPKCS(), oldpass);
         pprompt = i18n("Decoding failed.  Please try again:");
      } while (!pkcs);
      x->setPassCache(oldpass);
      slotYourUnlock();
   }

  // For now, we will only export to PKCS#12
   QString certFile = KFileDialog::getSaveFileName();
   if (certFile.isEmpty())
      return;

   if (!pkcs->toFile(certFile))
      KMessageBox::sorry(this, i18n("Export failed."), i18n("SSL"));
}


void KCryptoConfig::slotYourVerify() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
QString iss;
   if (!x) return;

   KSSLPKCS12 *pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPass());
   if (!pkcs)
      pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPassCache());
   if (!pkcs) {
      QString pprompt = i18n("Enter the certificate password:");
      QCString oldpass;
      do {
         int i = KPasswordDialog::getPassword(oldpass, pprompt);
         if (i != KPasswordDialog::Accepted) return; 
         pkcs = KSSLPKCS12::fromString(x->getPKCS(), oldpass);
         pprompt = i18n("Decoding failed.  Please try again:");
      } while (!pkcs);
      x->setPassCache(oldpass);
      slotYourUnlock();
   }

  if (pkcs->isValid()) {
     KMessageBox::information(this, i18n("This certificate passed the verification tests successfully."), i18n("SSL"));
  } else {
     KMessageBox::detailedError(this, i18n("This certificate has failed the tests and should be considered invalid."), KSSLCertificate::verifyText(pkcs->validate()), i18n("SSL"));
  }

  delete pkcs;
}


void KCryptoConfig::slotYourRemove() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
   if (x) {
      yourSSLBox->takeItem(x);
      yourCertDelList.append(x);
      setAuthCertLists();
      configChanged();
   }
}


void KCryptoConfig::slotYourUnlock() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
QString iss;
   if (!x || !yourSSLUnlock->isEnabled()) return;

   KSSLPKCS12 *pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPass());
   if (!pkcs)
      pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPassCache());
   if (!pkcs) {
      QString pprompt = i18n("Enter the certificate password:");
      QCString oldpass;
      do {
         int i = KPasswordDialog::getPassword(oldpass, pprompt);
         if (i != KPasswordDialog::Accepted) return; 
         pkcs = KSSLPKCS12::fromString(x->getPKCS(), oldpass);
         pprompt = i18n("Decoding failed.  Please try again:");
      } while (!pkcs);
      x->setPassCache(oldpass);
   }

   // update the info
   iss = pkcs->getCertificate()->getIssuer();
   ySubject->setValues(x ? x->getName() : QString(""));
   yIssuer->setValues(iss);
         QPalette cspl;
         KSSLCertificate *cert = pkcs->getCertificate();
         cspl = yValidFrom->palette();
         if (QDateTime::currentDateTime() < cert->getQDTNotBefore()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         yValidFrom->setPalette(cspl);

         cspl = yValidUntil->palette();
         if (QDateTime::currentDateTime() > cert->getQDTNotAfter()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         yValidUntil->setPalette(cspl);

         yValidFrom->setText(cert->getNotBefore());
         yValidUntil->setText(cert->getNotAfter());
   yourSSLUnlock->setEnabled(false);
   delete pkcs;
}


void KCryptoConfig::slotYourCertSelect() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
QString iss;

   yourSSLExport->setEnabled(x != NULL);
   yourSSLPass->setEnabled(x != NULL);
   yourSSLUnlock->setEnabled(false);
   yourSSLVerify->setEnabled(x != NULL);
   yourSSLRemove->setEnabled(x != NULL);

   if (x) {
      KSSLPKCS12 *pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPass());
      if (pkcs) {
         QPalette cspl;
         KSSLCertificate *cert = pkcs->getCertificate();
         iss = cert->getIssuer();
         cspl = yValidFrom->palette();
         if (QDateTime::currentDateTime() < cert->getQDTNotBefore()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         yValidFrom->setPalette(cspl);

         cspl = yValidUntil->palette();
         if (QDateTime::currentDateTime() > cert->getQDTNotAfter()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         yValidUntil->setPalette(cspl);

         yValidFrom->setText(cert->getNotBefore());
         yValidUntil->setText(cert->getNotAfter());
         delete pkcs;
      } else {
         yourSSLUnlock->setEnabled(x != NULL);
      }
   } else {
   }

   ySubject->setValues(x ? x->getName() : QString(""));
   yIssuer->setValues(iss);
}


void KCryptoConfig::slotYourPass() {
YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->selectedItem());
QCString oldpass = "";
   if (!x) return;

   KSSLPKCS12 *pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPass());
   if (!pkcs)
      pkcs = KSSLPKCS12::fromString(x->getPKCS(), x->getPassCache());
   if (!pkcs) {
      QString pprompt = i18n("Enter the OLD password for the certificate:");
      do {
         int i = KPasswordDialog::getPassword(oldpass, pprompt);
         if (i != KPasswordDialog::Accepted) break; 
         pkcs = KSSLPKCS12::fromString(x->getPKCS(), oldpass);
         pprompt = i18n("Decoding failed.  Please try again:");
      } while (!pkcs);
   }

   if (pkcs) {
      QCString pass;
      x->setPassCache(oldpass);
      slotYourUnlock();
      int i = KPasswordDialog::getNewPassword(pass, 
                                   i18n("Enter the new certificate password"));
      if (i == KPasswordDialog::Accepted) {
         pkcs->changePassword(QString(oldpass), QString(pass));
         x->setPKCS(pkcs->toString());
         x->setPassCache(pass);
         configChanged();
      }
      delete pkcs;
   }
}



void KCryptoConfig::slotNewHostAuth() {
    HostAuthItem *j = new HostAuthItem(hostAuthList,
                                       "",
                                       "",
                                       this );
    j->setAction(KSSLCertificateHome::AuthSend);
    hostAuthList->setSelected(j, true);
  authHost->setEnabled(true);
  hostCertBox->setEnabled(true);
  hostCertBG->setEnabled(true);
  authRemove->setEnabled(true);

  hostCertBG->setButton(0);

  authHost->setFocus();
}



void KCryptoConfig::slotRemoveHostAuth() {
HostAuthItem *x = static_cast<HostAuthItem *>(hostAuthList->selectedItem());

  if (x) {
      hostAuthList->takeItem(x);
      authDelList.append(x);
      configChanged();
  }
}


void KCryptoConfig::slotAuthItemChanged() {
HostAuthItem *x = static_cast<HostAuthItem *>(hostAuthList->selectedItem());

if (x) {
  // Make sure the fields are enabled
  authHost->setEnabled(true);
  hostCertBox->setEnabled(true);
  hostCertBG->setEnabled(true);
  authRemove->setEnabled(true);

  // Populate the fields
  switch(x->getAction()) {
  case KSSLCertificateHome::AuthSend:
    hostCertBG->setButton(hostCertBG->id(hostSend));
   break;
  case KSSLCertificateHome::AuthPrompt:
    hostCertBG->setButton(hostCertBG->id(hostPrompt));
   break;
  case KSSLCertificateHome::AuthDont:
    hostCertBG->setButton(hostCertBG->id(hostDont));
   break;
  default:
    hostSend->setChecked(false);
    hostPrompt->setChecked(false);
    hostDont->setChecked(false);
   break;
  }

  ___lehack = true;
  authHost->setText(x->configName());
  ___lehack = false;
  hostCertBox->setCurrentItem(0);

  QString theCert = x->getCertName();
  for (int i = 0; i < hostCertBox->count(); i++) {
    if (hostCertBox->text(i) == theCert) {
       hostCertBox->setCurrentItem(i);
       break;
    }
  }
} else {
  authHost->clear();
  authHost->setEnabled(false);
  hostCertBox->setEnabled(false);
  hostCertBG->setEnabled(false);
  authRemove->setEnabled(false);
}
}


void KCryptoConfig::slotAuthText(const QString &t) {
if (___lehack) return;
HostAuthItem *x = static_cast<HostAuthItem *>(hostAuthList->selectedItem());

if (x) {
   x->setHost(t);
configChanged();
}   
}



void KCryptoConfig::slotAuthButtons() {
HostAuthItem *x = static_cast<HostAuthItem *>(hostAuthList->selectedItem());

if (x) {
   KSSLCertificateHome::KSSLAuthAction aa = KSSLCertificateHome::AuthDont;
   int sel = hostCertBG->id(hostCertBG->selected());

   if (sel ==  hostCertBG->id(hostSend))
    aa = KSSLCertificateHome::AuthSend;
   else if (sel == hostCertBG->id(hostPrompt))
    aa = KSSLCertificateHome::AuthPrompt;
   else
    aa =  KSSLCertificateHome::AuthDont;

   x->setAction(aa);
configChanged();
}   
}


void KCryptoConfig::slotAuthCombo() {
HostAuthItem *x = static_cast<HostAuthItem *>(hostAuthList->selectedItem());

if (x) {
  if (hostCertBox->currentItem() == 0)
    x->setCertName("");
  else x->setCertName(hostCertBox->currentText());
  configChanged();
}
}



void KCryptoConfig::slotChooseEGD() {
  QString newFile = KFileDialog::getOpenFileName();
  if (newFile.length() > 0)
    mEGDPath->setText(newFile);
}


void KCryptoConfig::slotChooseOSSL() {
  QString newFile = KFileDialog::getExistingDirectory();
  if (newFile.length() > 0)
    oPath->setText(newFile);
}


void KCryptoConfig::slotTestOSSL() {
KOSSL::self()->destroy();

if (!KOSSL::self()->hasLibSSL()) {
   KMessageBox::detailedSorry(this, 
                              i18n("Failed to load OpenSSL."), 
                              i18n("libssl was not found or successfully loaded."), 
                              i18n("OpenSSL"));
   return;
}

if (!KOSSL::self()->hasLibCrypto()) {
   KMessageBox::detailedSorry(this, 
                              i18n("Failed to load OpenSSL."), 
                              i18n("libcrypto was not found or successfully loaded."), 
                              i18n("OpenSSL"));
   return;
}

   KMessageBox::information(this, i18n("OpenSSL was successfully loaded."), i18n("OpenSSL"));
}


void KCryptoConfig::slotUseEGD() {
  mEGDPath->setEnabled(mUseEGD->isChecked());
  mChooseEGD->setEnabled(mUseEGD->isChecked());
  mEGDLabel->setEnabled(mUseEGD->isChecked());
  configChanged();
}


void KCryptoConfig::slotGeneratePersonal() {
#if 0
  QStringList qslCertTypes;

  qslCertTypes << i18n("Personal SSL")
               << i18n("Server SSL")
               << i18n("S/MIME")
               << i18n("PGP")
               << i18n("GPG")
               << i18n("SSL Personal Request")
               << i18n("SSL Server Request")
               << i18n("Netscape SSL")
               << i18n("Server certificate authority", "Server CA")
               << i18n("Personal certificate authority", "Personal CA")
               << i18n("Secure MIME certificate authority", "S/MIME CA");
#endif
   
}


#ifdef HAVE_SSL
// This gets all the available ciphers from OpenSSL
bool KCryptoConfig::loadCiphers() {
unsigned int i;
SSL_CTX *ctx;
SSL *ssl;
SSL_METHOD *meth;

  SSLv2Box->clear();
  SSLv3Box->clear();

  meth = SSLv2_client_method();
  SSLeay_add_ssl_algorithms();
  ctx = SSL_CTX_new(meth);
  if (ctx == NULL) return false;

  ssl = SSL_new(ctx);
  if (!ssl) return false;

  CipherItem *item;
  for (i=0; ; i++) {
    int j, k;
    SSL_CIPHER *sc;
    QString cname;
    sc = (meth->get_cipher)(i);
    if (!sc) break;
    k = SSL_CIPHER_get_bits(sc, &j);

    item = new CipherItem( SSLv2Box, sc->name, k, j, this );
  }

  if (ctx) SSL_CTX_free(ctx);
  if (ssl) SSL_free(ssl);

  // We repeat for SSLv3
  meth = SSLv3_client_method();
  SSLeay_add_ssl_algorithms();
  ctx = SSL_CTX_new(meth);
  if (ctx == NULL) return false;

  ssl = SSL_new(ctx);
  if (!ssl) return false;

  for (i=0; ; i++) {
    int j, k;
    SSL_CIPHER *sc;
    QString cname;
    sc = (meth->get_cipher)(i);
    if (!sc) break;
    k = SSL_CIPHER_get_bits(sc, &j);

    item = new CipherItem( SSLv3Box, sc->name, k, j, this );
  }

  if (ctx) SSL_CTX_free(ctx);
  if (ssl) SSL_free(ssl);

return true;
}
#endif


void KCryptoConfig::setAuthCertLists() {
QString oldDef, oldHost;
bool noneDef, noneHost;

// get the old setting
  oldDef = defCertBox->currentText();
  oldHost = hostCertBox->currentText();
  noneDef = (0 == defCertBox->currentItem());
  noneHost = (0 == hostCertBox->currentItem());

// repopulate
  defCertBox->clear();
  hostCertBox->clear();
  QStringList defCertStrList; // = KSSLCertificateHome::getCertificateList();
  defCertStrList.append(i18n("None"));
  for (YourCertItem *x = static_cast<YourCertItem *>(yourSSLBox->firstChild()); 
                                                                             x;
                           x = static_cast<YourCertItem *>(x->nextSibling())) {
     defCertStrList.append(x->configName());
  }
  defCertBox->insertStringList(defCertStrList);
  hostCertBox->insertStringList(defCertStrList);

// try to restore it

  defCertBox->setCurrentItem(0);
  if (!noneDef) {
    for (int i = 0; i < defCertBox->count(); i++) {
      if (defCertBox->text(i) == oldDef) {
         defCertBox->setCurrentItem(i);
         break;
      }
    }
  }

  if (!noneDef && defCertBox->currentItem() == 0) configChanged();

  hostCertBox->setCurrentItem(0);
  if (!noneHost) {
    for (int i = 0; i < hostCertBox->count(); i++) {
      if (hostCertBox->text(i) == oldHost) {
         hostCertBox->setCurrentItem(i);
         break;
      }
    }
  }

  if (!noneHost && hostCertBox->currentItem() == 0) configChanged();

  // Update the host entries too
  for (HostAuthItem *x = 
        static_cast<HostAuthItem *>(hostAuthList->firstChild()); 
                                                              x;
             x = static_cast<HostAuthItem *>(x->nextSibling())) {
     QString newValue = "";
     for (int i = 1; i < hostCertBox->count(); i++) {
        if (hostCertBox->text(i) == x->getCertName()) {
           newValue = x->getCertName();
           break;
        }
     }
     if (newValue != x->getCertName()) configChanged();
     x->setCertName(newValue);
  }

  
}


extern "C"
{
  KCModule *create_crypto(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmcrypto");
    return new KCryptoConfig(parent, name);
  };
}

#include "crypto.moc"

