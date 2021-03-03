/*
 * kcmioslaveinfo.cpp
 *
 * Copyright 2001 Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
 * Copyright 2001 George Staikos  <staikos@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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

#include "kcmioslaveinfo.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qwhatsthis.h>

#include <kprotocolinfo.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kdialog.h>
#include <kio/job.h>
#include <kdebug.h>
#include <qtextcodec.h>

KCMIOSlaveInfo::KCMIOSlaveInfo(QWidget *parent, const char *name)
               :KCModule(parent,name),m_ioslavesLb(0)
{
   QVBoxLayout *layout=new QVBoxLayout(this,KDialog::marginHint(),KDialog::spacingHint());

   QLabel* label=new QLabel(i18n("Available IOSlaves"),this);
   QHBox *hbox=new QHBox(this);
   m_ioslavesLb=new KListBox(hbox);
   m_ioslavesLb->setMinimumSize(fontMetrics().width("blahfaselwhatever----"),10);
   connect( m_ioslavesLb, SIGNAL( executed( QListBoxItem * ) ), SLOT( showInfo( QListBoxItem * ) ) );
   //TODO make something useful after 2.1 is released
   m_info=new KTextBrowser(hbox);
   hbox->setSpacing(KDialog::spacingHint());

   layout->addWidget(label);
   layout->addWidget(hbox);
   hbox->setStretchFactor(m_ioslavesLb,1);
   hbox->setStretchFactor(m_info,5);

   QStringList protocols=KProtocolInfo::protocols();
   for (QStringList::Iterator it=protocols.begin(); it!=protocols.end(); it++)
   {
      m_ioslavesLb->insertItem(*it);
   };
   m_ioslavesLb->sort();

   setButtons(buttons());
   load();
}


KCMIOSlaveInfo::~KCMIOSlaveInfo() {

}


void KCMIOSlaveInfo::load() {
   emit changed(false);
}

void KCMIOSlaveInfo::defaults() {
   emit changed(true);
}

void KCMIOSlaveInfo::save() {
   emit changed(true);
}

int KCMIOSlaveInfo::buttons () {
return KCModule::Default|KCModule::Apply|KCModule::Help;
}

void KCMIOSlaveInfo::configChanged() {
  emit changed(true);
}

void KCMIOSlaveInfo::childChanged(bool really) {
  emit changed(really);
}

QString KCMIOSlaveInfo::quickHelp() const
{
   return i18n("<qt>Gives you an overview of the installed ioslaves and allows"
               " you to configure the network timeout values for those slaves.</qt>");
}

void KCMIOSlaveInfo::slaveHelp( KIO::Job *, const QByteArray &data)
{
    if ( data.size() == 0 ) { // EOF
        int index = helpData.find( "<meta http-equiv=\"Content-Type\"" );
        index = helpData.find( "charset=", index ) + 8;
        QString charset = helpData.mid( index, helpData.find( '\"', index ) - index );
        QString text = QTextCodec::codecForName(charset.latin1())->toUnicode( helpData );
        index = text.find( "<div class=\"article\">" );
        text = text.mid( index );
        index = text.find( "<div id=\"bottom-nav\"" );
        text = text.left( index );
        m_info->setText(text);
        return;
    }
    helpData += data;
}

void KCMIOSlaveInfo::showInfo(const QString& protocol)
{
   QString file = QString("kioslave/%1.docbook").arg( protocol );
   file = KGlobal::locale()->langLookup( file );

   if (!file.isEmpty())
   {
       helpData.truncate( 0 );
       KIO::Job *tfj = KIO::get( QString("help:/kioslave/%1.html").arg( protocol ), true, false );
       connect( tfj, SIGNAL( data( KIO::Job *, const QByteArray &) ), SLOT( slaveHelp( KIO::Job *, const QByteArray &) ) );
       return;
   }
   m_info->setText(QString("Some info about protocol %1:/ ...").arg(protocol));
}

void KCMIOSlaveInfo::showInfo(QListBoxItem *item)
{
   if (item==0)
      return;
   showInfo( item->text() );
}


extern "C"
{

  KCModule *create_ioslaveinfo(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmioslaveinfo");
    return new KCMIOSlaveInfo(parent, name);
  }
}

#include "kcmioslaveinfo.moc"
