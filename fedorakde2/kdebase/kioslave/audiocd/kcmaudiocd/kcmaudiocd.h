/*

    Copyright (C) 2001 Carsten Duvenhorst <duvenhorst@m2.uni-hannover.de>

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

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

*/


#ifndef KAUDIOCDCONFIG_H
#define KAUDIOCDCONFIG_H

#include <kapp.h>
#include <kcmodule.h>

#include "audiocdconfig.h"
class KAudiocdModule : public KCModule
{
  Q_OBJECT

public:

  KAudiocdModule(QWidget *parent=0, const char *name=0);

   QString quickHelp() const;

public slots:

 void defaults();
 void save();
 void load();
 
private slots:

 void slotSelectMethod(int);
 void slotUpdateVBRWidgets();
 void slotUpdateVBRCombos();
 void slotConfigChanged();
 void slotChangeFilter();
 void slotEcDisable();
 void slotAddCDDBServer();
 void slotDelCDDBServer();
 void slotServerTextChanged(const QString & );
private:

  AudiocdConfig *audiocdConfig;
  KConfig *config;
  bool configChanged;

  QCheckBox *ec_disable_check,*ec_neverskip_check,*cd_autosearch_check;
  QLineEdit *cd_device_string;

  QCheckBox *cddb_enable;
  QLineEdit *cddb_server;
  QListBox *cddb_server_listbox;
  QStringList cddbserverlist;
  QPushButton *cddbserver_add_push,*cddbserver_del_push;

  QCheckBox *vbr_min_br, *vbr_min_hard, *vbr_max_br, *vbr_average_br, *vbr_xing_tag;

  QCheckBox *copyright, *original, *iso, *id3_tag, *crc;

  QCheckBox *set_vorbis_min_br, *set_vorbis_max_br, *set_vorbis_nominal_br, *vorbis_comments;
  QComboBox *vorbis_min_br, *vorbis_max_br, *vorbis_nominal_br;
  QCheckBox *enable_lowpass, *enable_highpass, *set_lpf_width, *set_hpf_width;
  QComboBox *enc_method, *cbr_bitrate, *vbr_min_brate, *vbr_max_brate, *vbr_mean_brate, *stereo;

  QGroupBox *cbr_settings, *vbr_settings;
  
  QSlider *quality;

  QSpinBox *lowfilterfreq, *lowfilterwidth, *highfilterfreq, *highfilterwidth;

  int getBitrateIndex(int value);
  int getVorbisBitrateIndex(int value);
  int getVorbisNominalBitrateIndex(int value);
};

#endif

