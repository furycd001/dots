/*
   Copyright (C) 2001 Carsten Duvenhorst <duvenhorst@m2.uni-hannover.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>

#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include "kcmaudiocd.moc"

#define CDDB_DEFAULT_SERVER "freedb.freedb.org:8880"

// MPEG Layer 3 Bitrates
static int bitrates[] = { 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };

// these are the approx. bitrates for the current 5 Vorbis modes
static int vorbis_nominal_bitrates[] = { 128, 160, 192, 256, 350 };
static int vorbis_bitrates[] = { 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 350 };

KAudiocdModule::KAudiocdModule(QWidget *parent, const char *name)
  : KCModule(parent, name), configChanged(false)
{

    setButtons(Default|Apply);

    QVBoxLayout *layout = new QVBoxLayout(this, 10);

    audiocdConfig = new AudiocdConfig(this);

    layout->addWidget(audiocdConfig);
    layout->setMargin(0);

    enc_method = audiocdConfig->enc_method;
    vbr_settings = audiocdConfig->vbr_settings;
    cbr_settings = audiocdConfig->cbr_settings;
    stereo = audiocdConfig->stereo;
    quality = audiocdConfig->quality;
    copyright = audiocdConfig->copyright;
    original = audiocdConfig->original;
    iso = audiocdConfig->iso;
    crc = audiocdConfig->crc;

    id3_tag = audiocdConfig->id3_tag;

    cbr_bitrate = audiocdConfig->cbr_bitrate;

    vbr_average_br = audiocdConfig->vbr_average_br;
    vbr_mean_brate = audiocdConfig->vbr_mean_brate;

    vbr_min_br = audiocdConfig->vbr_min_br;
    vbr_min_brate = audiocdConfig->vbr_min_brate;
    vbr_min_hard = audiocdConfig->vbr_min_hard;

    vbr_max_br = audiocdConfig->vbr_max_br;
    vbr_max_brate = audiocdConfig->vbr_max_brate;

    vbr_xing_tag = audiocdConfig->vbr_xing_tag;

    enable_lowpass = audiocdConfig->enable_lowpass;
    enable_highpass = audiocdConfig->enable_highpass;
    set_lpf_width = audiocdConfig->set_lpf_width;
    set_hpf_width = audiocdConfig->set_hpf_width;
    lowfilterfreq = audiocdConfig->lowfilterfreq;
    highfilterfreq = audiocdConfig->highfilterfreq;
    lowfilterwidth = audiocdConfig->lowfilterwidth;
    highfilterwidth = audiocdConfig->highfilterwidth;

    set_vorbis_min_br = audiocdConfig->set_vorbis_min_br;
    set_vorbis_max_br = audiocdConfig->set_vorbis_max_br;
    set_vorbis_nominal_br = audiocdConfig->set_vorbis_nominal_br;

    vorbis_min_br = audiocdConfig->vorbis_min_br;
    vorbis_max_br = audiocdConfig->vorbis_max_br;
    vorbis_nominal_br = audiocdConfig->vorbis_nominal_br;

    vorbis_comments = audiocdConfig->vorbis_comments;

    cd_autosearch_check = audiocdConfig->cd_autosearch_check;
    cd_device_string = audiocdConfig->cd_device_string;

    ec_disable_check = audiocdConfig->ec_disable_check;
    ec_neverskip_check = audiocdConfig->ec_neverskip_check;

    cddb_enable = audiocdConfig->cddb_enable;
    cddb_server = audiocdConfig->cddb_server;

    cddb_server_listbox = audiocdConfig->cddb_server_listbox;
    cddbserver_add_push = audiocdConfig->cddbserver_add_push;
    cddbserver_del_push = audiocdConfig->cddbserver_del_push;

    config = new KConfig("kcmaudiocdrc");

    load();

    cddbserver_add_push->setEnabled(!cddb_server->text().isEmpty());

    connect(cddb_server, SIGNAL(textChanged ( const QString & )),this,SLOT(slotServerTextChanged(const QString & )));

    //CDDA Options
    connect(cd_autosearch_check,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(ec_disable_check,SIGNAL(clicked()),this,SLOT(slotEcDisable()));
    connect(ec_neverskip_check,SIGNAL(clicked()),SLOT(slotConfigChanged()));

    //CDDB Options
    connect(cddb_enable, SIGNAL(clicked()), this, SLOT(slotConfigChanged()));
    connect(cddbserver_add_push,SIGNAL(clicked()), this, SLOT(slotAddCDDBServer()));
    connect(cddbserver_del_push,SIGNAL(clicked()), this, SLOT(slotDelCDDBServer()));
    connect(cddb_server,SIGNAL(textChanged(const QString &)),this,SLOT(slotConfigChanged()));

    //MP3 Encoding Method
    connect(enc_method,SIGNAL(activated(int)),SLOT(slotSelectMethod(int)));
    connect(stereo,SIGNAL(activated(int)),SLOT(slotConfigChanged()));
    connect(quality,SIGNAL(valueChanged(int)),SLOT(slotConfigChanged()));

    //MP3 Options
    connect(copyright,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(original,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(iso,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(id3_tag,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));

    //MP3 CBR Settings
    connect(cbr_bitrate,SIGNAL(activated(int)),SLOT(slotConfigChanged()));

    //MP3 VBR Groupbox
    connect(vbr_average_br,SIGNAL(clicked()),this,SLOT(slotUpdateVBRWidgets()));
    connect(vbr_min_hard,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(vbr_min_br,SIGNAL(clicked()),this,SLOT(slotUpdateVBRCombos()));
    connect(vbr_min_brate,SIGNAL(activated(int)),SLOT(slotConfigChanged()));

    connect(vbr_max_br,SIGNAL(clicked()),this,SLOT(slotUpdateVBRCombos()));
    connect(vbr_max_brate,SIGNAL(activated(int)),SLOT(slotConfigChanged()));

    connect(vbr_xing_tag,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));

    //MP3 Filter
    connect(enable_lowpass,SIGNAL(clicked()),this,SLOT(slotChangeFilter()));
    connect(enable_highpass,SIGNAL(clicked()),this,SLOT(slotChangeFilter()));
    connect(set_lpf_width,SIGNAL(clicked()),this,SLOT(slotChangeFilter()));
    connect(set_hpf_width,SIGNAL(clicked()),this,SLOT(slotChangeFilter()));

    connect(lowfilterwidth,SIGNAL(valueChanged(int)),SLOT(slotConfigChanged()));
    connect(highfilterwidth,SIGNAL(valueChanged(int)),SLOT(slotConfigChanged()));
    connect(lowfilterfreq,SIGNAL(valueChanged(int)),SLOT(slotConfigChanged()));
    connect(highfilterfreq,SIGNAL(valueChanged(int)),SLOT(slotConfigChanged()));

    // Vorbis
    connect(set_vorbis_min_br,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(set_vorbis_max_br,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(set_vorbis_nominal_br,SIGNAL(clicked()),this,SLOT(slotConfigChanged()));
    connect(vorbis_min_br,SIGNAL(activated(int)),SLOT(slotConfigChanged()));
    connect(vorbis_max_br,SIGNAL(activated(int)),SLOT(slotConfigChanged()));
    connect(vorbis_nominal_br,SIGNAL(activated(int)),SLOT(slotConfigChanged()));

};

void KAudiocdModule::slotServerTextChanged(const QString &_text )
{
    cddbserver_add_push->setEnabled(!_text.isEmpty());
}

void KAudiocdModule::defaults() {

  cddbserverlist = CDDB_DEFAULT_SERVER;

  cddb_enable->setChecked(true);
  cddb_server->setText(CDDB_DEFAULT_SERVER);
  cddb_server_listbox->clear();
  cddb_server_listbox->insertStringList(cddbserverlist);

  cd_autosearch_check->setChecked(true);
  cd_device_string->setText("/dev/cdrom");

  ec_disable_check->setChecked(false);
  ec_neverskip_check->setChecked(true);

  enc_method->setCurrentItem(0);
  slotSelectMethod(0);

  stereo->setCurrentItem(0);
  quality->setValue(2);

  copyright->setChecked(false);
  original->setChecked(true);
  iso->setChecked(false);
  id3_tag->setChecked(true);
  crc->setChecked(false);

  cbr_bitrate->setCurrentItem(9);

  vbr_min_br->setChecked(false);
  vbr_min_hard->setChecked(false);
  vbr_max_br->setChecked(false);
  vbr_average_br->setChecked(false);

  vbr_min_brate->setCurrentItem(7);
  vbr_max_brate->setCurrentItem(13);
  vbr_mean_brate->setCurrentItem(10);

  vbr_xing_tag->setChecked(true);

  slotUpdateVBRWidgets();

  enable_lowpass->setChecked(false);
  enable_highpass->setChecked(false);
  set_lpf_width->setChecked(false);
  set_hpf_width->setChecked(false);

  lowfilterfreq->setValue(18000);
  lowfilterwidth->setValue(900);
  highfilterfreq->setValue(0);
  highfilterwidth->setValue(0);

  slotChangeFilter();

  set_vorbis_min_br->setChecked(false);
  set_vorbis_max_br->setChecked(false);
  set_vorbis_nominal_br->setChecked(true);

  vorbis_min_br->setCurrentItem(0);
  vorbis_max_br->setCurrentItem(13);
  vorbis_nominal_br->setCurrentItem(1);

  vorbis_comments->setChecked(true);
}

void KAudiocdModule::save() {

  if (!configChanged ) return;

  int encmethod = enc_method->currentItem();
  int mode = stereo->currentItem();
  int encquality = quality->value();

  int cbrbrate = cbr_bitrate->currentItem();
  cbrbrate = bitrates[cbrbrate];

  int vbrminbrate = vbr_min_brate->currentItem();
  vbrminbrate = bitrates[vbrminbrate];

  int vbrmaxbrate = vbr_max_brate->currentItem();
  vbrmaxbrate = bitrates[vbrmaxbrate];

  int vbravrbrate = vbr_mean_brate->currentItem();
  vbravrbrate = bitrates[vbravrbrate];

  int lpf_freq = lowfilterfreq->value();
  int lpf_width = lowfilterwidth->value();

  int hpf_freq = highfilterfreq->value();
  int hpf_width = highfilterwidth->value();

  int vorbis_min_bitrate = vorbis_min_br->currentItem();
  vorbis_min_bitrate = vorbis_bitrates[vorbis_min_bitrate];

  int vorbis_max_bitrate = vorbis_max_br->currentItem();
  vorbis_max_bitrate = vorbis_bitrates[vorbis_max_bitrate];

  int vorbis_nominal_bitrate = vorbis_nominal_br->currentItem();
  vorbis_nominal_bitrate = vorbis_nominal_bitrates[vorbis_nominal_bitrate];

  config->setGroup("CDDA");
  config->writeEntry("autosearch",cd_autosearch_check->isChecked());
  config->writeEntry("device",cd_device_string->text());
  config->writeEntry("disable_paranoia",ec_disable_check->isChecked());
  config->writeEntry("never_skip",ec_neverskip_check->isChecked());

  config->setGroup("CDDB");
  config->writeEntry("enable_cddb",cddb_enable->isChecked());
  config->writeEntry("cddb_server",cddb_server->text());
  config->writeEntry("cddb_server_list",cddbserverlist);

  config->setGroup("MP3");

  config->writeEntry("mode",mode);
  config->writeEntry("quality",encquality);
  config->writeEntry("encmethod",encmethod);

  config->writeEntry("copyright",copyright->isChecked());
  config->writeEntry("original",original->isChecked());
  config->writeEntry("iso",iso->isChecked());
  config->writeEntry("crc",crc->isChecked());
  config->writeEntry("id3",id3_tag->isChecked());

  config->writeEntry("cbrbitrate",cbrbrate);

  config->writeEntry("set_vbr_min",vbr_min_br->isChecked());
  config->writeEntry("set_vbr_max",vbr_max_br->isChecked());
  config->writeEntry("set_vbr_avr",vbr_average_br->isChecked());
  config->writeEntry("vbr_min_hard",vbr_min_hard->isChecked());
  config->writeEntry("vbr_min_bitrate",vbrminbrate);
  config->writeEntry("vbr_max_bitrate",vbrmaxbrate);
  config->writeEntry("vbr_average_bitrate",vbravrbrate);
  config->writeEntry("write_xing_tag",vbr_xing_tag->isChecked());

  config->writeEntry("enable_lowpassfilter",enable_lowpass->isChecked());
  config->writeEntry("enable_highpassfilter",enable_highpass->isChecked());
  config->writeEntry("set_highpassfilter_width",set_hpf_width->isChecked());
  config->writeEntry("set_lowpassfilter_width",set_lpf_width->isChecked());
  config->writeEntry("lowpassfilter_freq",lpf_freq);
  config->writeEntry("lowpassfilter_width",lpf_width);
  config->writeEntry("highpassfilter_freq",hpf_freq);
  config->writeEntry("highpassfilter_width",hpf_width);

  config->setGroup("Vorbis");

  config->writeEntry("set_vorbis_min_bitrate",set_vorbis_min_br->isChecked());
  config->writeEntry("set_vorbis_max_bitrate",set_vorbis_max_br->isChecked());
  config->writeEntry("set_vorbis_nominal_bitrate",set_vorbis_nominal_br->isChecked());
  config->writeEntry("vorbis_comments",vorbis_comments->isChecked());
  config->writeEntry("vorbis_min_bitrate",vorbis_min_bitrate);
  config->writeEntry("vorbis_max_bitrate",vorbis_max_bitrate);
  config->writeEntry("vorbis_nominal_bitrate",vorbis_nominal_bitrate);

  config->sync();

  configChanged = false;

}

void KAudiocdModule::load() {

  config->setGroup("CDDA");

  cd_autosearch_check->setChecked(config->readBoolEntry("autosearch",true));
  cd_device_string->setText(config->readEntry("device","/dev/cdrom"));
  ec_disable_check->setChecked(config->readBoolEntry("disable_paranoia",false));
  ec_neverskip_check->setChecked(config->readBoolEntry("never_skip",true));

  config->setGroup("CDDB");

  cddb_enable->setChecked(config->readBoolEntry("enable_cddb",true));
  cddb_server->setText(config->readEntry("cddb_server",CDDB_DEFAULT_SERVER));
  cddbserverlist = config->readListEntry("cddb_server_list",',');
  cddb_server_listbox->clear();
  cddb_server_listbox->insertStringList(cddbserverlist);

  config->setGroup("MP3");

  int encmethod = config->readNumEntry("encmethod",0);
  enc_method->setCurrentItem(encmethod);
  slotSelectMethod(encmethod);

  stereo->setCurrentItem(config->readNumEntry("mode",0));
  quality->setValue(config->readNumEntry("quality",2));

  copyright->setChecked(config->readBoolEntry("copyright",false));
  original->setChecked(config->readBoolEntry("original",true));
  iso->setChecked(config->readBoolEntry("iso",false));
  crc->setChecked(config->readBoolEntry("crc",false));
  id3_tag->setChecked(config->readBoolEntry("id3",true));


  int brate = config->readNumEntry("cbrbitrate",160);
  cbr_bitrate->setCurrentItem(getBitrateIndex(brate));

  vbr_min_br->setChecked(config->readBoolEntry("set_vbr_min",false));
  vbr_min_hard->setChecked(config->readBoolEntry("vbr_min_hard",false));
  vbr_max_br->setChecked(config->readBoolEntry("set_vbr_max",false));
  vbr_average_br->setChecked(config->readBoolEntry("set_vbr_avr",true));

  brate = config->readNumEntry("vbr_min_bitrate",40);
  vbr_min_brate->setCurrentItem(getBitrateIndex(brate));
  brate = config->readNumEntry("vbr_max_bitrate",320);
  vbr_max_brate->setCurrentItem(getBitrateIndex(brate));
  brate = config->readNumEntry("vbr_average_bitrate",160);
  vbr_mean_brate->setCurrentItem(getBitrateIndex(brate));

  vbr_xing_tag->setChecked(config->readBoolEntry("write_xing_tag",true));

  slotUpdateVBRWidgets();

  enable_lowpass->setChecked(config->readBoolEntry("enable_lowpassfilter",false));
  enable_highpass->setChecked(config->readBoolEntry("enable_highpassfilter",false));
  set_lpf_width->setChecked(config->readBoolEntry("set_lowpassfilter_width",false));
  set_hpf_width->setChecked(config->readBoolEntry("set_highpassfilter_width",false));

  lowfilterfreq->setValue(config->readNumEntry("lowpassfilter_freq",0));
  lowfilterwidth->setValue(config->readNumEntry("lowpassfilter_width",0));
  highfilterfreq->setValue(config->readNumEntry("highpassfilter_freq",0));
  highfilterwidth->setValue(config->readNumEntry("highpassfilter_width",0));

  slotChangeFilter();

  config->setGroup("Vorbis");

  brate = config->readNumEntry("vorbis_min_bitrate",40);
  vorbis_min_br->setCurrentItem(getVorbisBitrateIndex(brate));

  brate = config->readNumEntry("vorbis_max_bitrate",350);
  vorbis_max_br->setCurrentItem(getVorbisBitrateIndex(brate));

  brate = config->readNumEntry("vorbis_nominal_bitrate",160);
  vorbis_nominal_br->setCurrentItem(getVorbisNominalBitrateIndex(brate));

  set_vorbis_min_br->setChecked(config->readBoolEntry("set_vorbis_min_bitrate",false));
  set_vorbis_max_br->setChecked(config->readBoolEntry("set_vorbis_max_bitrate",false));
  set_vorbis_nominal_br->setChecked(config->readBoolEntry("set_vorbis_nominal_bitrate",true));

  vorbis_comments->setChecked(config->readBoolEntry("vorbis_comments",true));


}

int KAudiocdModule::getBitrateIndex(int value) {

  for (uint i=0;i < sizeof(bitrates);i++)
    if (value == bitrates[i])
      return i;
  return -1;
}

int KAudiocdModule::getVorbisBitrateIndex(int value) {

  for (uint i=0;i < sizeof(vorbis_bitrates);i++)
    if (value == vorbis_bitrates[i])
      return i;
  return -1;
}

int KAudiocdModule::getVorbisNominalBitrateIndex(int value) {

  for (uint i=0;i < sizeof(vorbis_nominal_bitrates);i++)
    if (value == vorbis_nominal_bitrates[i])
      return i;
  return -1;
}

void KAudiocdModule::slotConfigChanged() {

  if (!configChanged) configChanged = true;
  emit changed(true);
  return;
}

void KAudiocdModule::slotAddCDDBServer() {

    QString strCddb=cddb_server->text();
    if(strCddb.isEmpty() || (cddbserverlist.find(strCddb) != cddbserverlist.end())) return;

  cddbserverlist.append(cddb_server->text());
  cddbserverlist.sort();

  cddb_server_listbox->clear();
  cddb_server_listbox->insertStringList(cddbserverlist);

  slotConfigChanged();

}

void KAudiocdModule::slotDelCDDBServer() {
    QStringList::Iterator it =  cddbserverlist.find(cddb_server_listbox->currentText ());

    if( it == cddbserverlist.end()) return;

    cddbserverlist.remove(it);
    cddbserverlist.sort();

    cddb_server->clear();
    cddb_server_listbox->clear();
    cddb_server_listbox->insertStringList(cddbserverlist);

    slotConfigChanged();

}


/*
#    slot for the error correction settings
*/
void KAudiocdModule::slotEcDisable() {

  if (ec_neverskip_check->isChecked()) {
    ec_neverskip_check->setChecked(false);
  } else {
    if (ec_neverskip_check->isEnabled()) {
      ec_neverskip_check->setChecked(true);
    }
  }

  slotConfigChanged();

}

//
// slot for the filter settings
//
void KAudiocdModule::slotChangeFilter() {

  if (enable_lowpass->isChecked()) {
    lowfilterfreq->setEnabled(true);
    //lowfilterwidth->setEnabled(true);
    set_lpf_width->setEnabled(true);
  } else {
    lowfilterfreq->setDisabled(true);
    lowfilterwidth->setDisabled(true);
    set_lpf_width->setChecked(false);
    set_lpf_width->setDisabled(true);

  }

  if (enable_highpass->isChecked()) {
    highfilterfreq->setEnabled(true);
    //    highfilterwidth->setEnabled(true);
    set_hpf_width->setEnabled(true);
  } else {
    highfilterfreq->setDisabled(true);
    highfilterwidth->setDisabled(true);
    set_hpf_width->setChecked(false);
    set_hpf_width->setDisabled(true);

  }

  if (set_lpf_width->isChecked()) {
    lowfilterwidth->setEnabled(true);
  } else {
     lowfilterwidth->setDisabled(true);
  }

  if (set_hpf_width->isChecked()) {
    highfilterwidth->setEnabled(true);
  } else {
     highfilterwidth->setDisabled(true);
  }

  slotConfigChanged();

}


//
// slot for switching between CBR and VBR
//
void KAudiocdModule::slotSelectMethod(int index) {
  // constant bitrate selected
  if (index == 1 ) {
    vbr_settings->show();
    cbr_settings->hide();
  } else {
    // variable bitrate selected
    vbr_settings->hide();
    cbr_settings->show();
  }
  slotConfigChanged();
  return;
}


//
// slot for changing the VBR Widgets logically
//
void KAudiocdModule::slotUpdateVBRWidgets() {

  if (vbr_average_br->isEnabled()) {

    if(vbr_average_br->isChecked()) {

      vbr_min_br->setChecked(false);
      vbr_min_br->setDisabled(true);
      vbr_min_hard->setChecked(false);
      vbr_max_br->setChecked(false);
      vbr_max_br->setDisabled(true);
      vbr_mean_brate->setEnabled(true);

    } else {

       vbr_min_br->setEnabled(true);
       vbr_max_br->setEnabled(true);
       vbr_mean_brate->setDisabled(true);
    }
  }

  slotUpdateVBRCombos();

  return;
}

//
// slot for enabling and disabling VBR bitrate combo's
//

void KAudiocdModule::slotUpdateVBRCombos() {

  vbr_min_brate->setEnabled((vbr_min_br->isEnabled())&&(vbr_min_br->isChecked())?true:false);
  vbr_max_brate->setEnabled((vbr_max_br->isEnabled())&&(vbr_max_br->isChecked())?true:false);

  slotConfigChanged();

}

QString KAudiocdModule::quickHelp() const
{
    return i18n("<h1>Audio-CD Slave</h1> The Audio-CD Slave enables you to easily"
                        " rip wav, MP3 or ogg vorbis files from your CD-ROM or DVD drive."
                        " The slave is invoked by typing <i>\"audiocd:/\"</i> in Konqueror's location"
                        " bar. In this module, you can configure all aspects of the slave like"
                        " encoding, CDDB lookup and device settings. Note that MP3 and ogg"
                        " vorbis encoding are only available if the KDE was built with a recent"
                        " version of the lame or ogg libraries.");
}

extern "C"
{
    KCModule *create_audiocd(QWidget *parent, const char *name)
    {
        KGlobal::locale()->insertCatalogue("kcmaudiocd");
        return new KAudiocdModule(parent, name);
    }

}
