/*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    $Id: arts.cpp,v 1.45 2001/08/05 21:05:40 wester Exp $

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

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <unistd.h>

#include <qfileinfo.h>
#include <qstring.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qdir.h>
#include <qwhatsthis.h>

#include <kconfig.h>
#include <ksimpleconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kstddirs.h>

#include "arts.h"

/* check if starting realtime would be possible */

bool artswrapper_check()
{
	if (system("artswrapper check") == 0)
		return true;
	return false;
}

extern "C" {
	QString createArgs (bool netTrans, bool duplex, int fragmentCount, int fragmentSize,
			    const QString &deviceName,
			    int rate, int bits, const QString &audioIO,
			    const QString &addOptions, bool autoSuspend,
			    int suspendTime,
			    const QString &messageApplication, int loggingLevel);
	KCModule *create_arts(QWidget *parent, const char *name);
	void init_arts();
}

/*
 * This function uses artsd -A to init audioIOList with the possible audioIO
 * methods. Here is a sample output of artsd -A (note the two spaces before
 * each "interesting" line are used in parsing:
 *
 * # artsd -A
 * possible choices for the audio i/o method:
 * 
 *   null      No audio input/output
 *   alsa      Advanced Linux Sound Architecture
 *   oss       Open Sound System
 * 
 */
void KArtsModule::initAudioIOList()
{
	//audioIOList.setAutoDelete(true);

	FILE *artsd = popen("artsd -A 2>&1","r");
	char line[1024];
	if(artsd)
	{
		while (fgets(line, 1024, artsd))
		{
			// well: don't change the output format ;-)
			if(line[0] == ' ' && line[1] == ' ')
			{
				char *name = strtok(line+2," \n");
				if(name && name[0])
				{
					char *fullName = strtok(0, "\n");
					if(fullName && fullName[0])
					{
						while(fullName[0] == ' ') {
							fullName++;
						}
						audioIOList.append(new AudioIOElement(QString::fromLatin1(name), QString::fromLatin1(fullName)));
					}
		  		}
			}
	  	}
		fclose(artsd);
	}
}

KArtsModule::KArtsModule(QWidget *parent, const char *name)
  : KCModule(parent, name), configChanged(false)
{
	setButtons(Default|Apply);

	initAudioIOList();

	QVBoxLayout *layout = new QVBoxLayout(this, 10);
	artsConfig = new ArtsConfig(this);
	layout->addWidget(artsConfig);
	layout->setMargin(0);

	startServer = artsConfig->startServer;
	networkTransparent = artsConfig->networkTransparent;
	x11Comm = artsConfig->x11Comm;
	startRealtime = artsConfig->startRealtime;
	fullDuplex = artsConfig->fullDuplex;
	customDevice = artsConfig->customDevice;
	deviceName = artsConfig->deviceName;
	customRate = artsConfig->customRate;
	samplingRate = artsConfig->samplingRate;
	autoSuspend = artsConfig->autoSuspend;
	suspendTime = artsConfig->suspendTime;
	displayMessage = artsConfig->displayMessage;
	messageApplication = artsConfig->messageApplication;

   	QString deviceHint = i18n("Normally, the sound server defaults to using the device called <b>/dev/dsp</b> for sound output. That should work in most cases. An exception is for instance if you are using devfs, then you should use <b>/dev/sound/dsp</b> instead. Other alternatives are things like <b>/dev/dsp0</b> or <b>/dev/dsp1</b> if you have a soundcard that supports multiple outputs, or you have multiple soundcards.");

	QString rateHint = i18n("Normally, the sound server defaults to using a sampling rate of 44100 Hz (CD quality), which is supported on almost any hardware. If you are using certain <b>Yamaha soundcards</b>, you might need to configure this to 48000 Hz here, if you are using <b>old SoundBlaster cards</b>, like SoundBlaster Pro, you might need to change this to 22050 Hz. All other values are possible, too, and may make sense in certain contexts (i.e. professional studio equipment).");

	QString optionsHint = i18n("This configuration module is intended to cover almost every aspect of the aRts sound server that you can configure. However, there are some things which may not be available here, so you can add <b>command line options</b> here which will be passed directly to <b>artsd</b>. The command line options will override the choices made in the GUI. To see the possible choices, open a Konsole window, and type <b>artsd -h</b>.");

	QWhatsThis::add(customDevice, deviceHint);
	QWhatsThis::add(deviceName, deviceHint);
	QWhatsThis::add(customRate, rateHint);
	QWhatsThis::add(samplingRate, rateHint);
	QWhatsThis::add(artsConfig->customOptions, optionsHint);
	QWhatsThis::add(artsConfig->addOptions, optionsHint);

	for (AudioIOElement *a = audioIOList.first(); a != 0; a = audioIOList.next())
		artsConfig->audioIO->insertItem(a->fullName);


	config = new KConfig("kcmartsrc");
	GetSettings();

	connect(startServer,SIGNAL(clicked()),this,SLOT(slotChanged()));
	connect(networkTransparent,SIGNAL(clicked()),this,SLOT(slotChanged()));
	connect(x11Comm,SIGNAL(clicked()),this,SLOT(slotChanged()));
	connect(startRealtime,SIGNAL(clicked()),this,SLOT(slotChanged()));
	connect(fullDuplex,SIGNAL(clicked()),this,SLOT(slotChanged()));
	connect(customDevice, SIGNAL(clicked()), SLOT(slotChanged()));
	connect(deviceName, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
	connect(customRate, SIGNAL(clicked()), SLOT(slotChanged()));
	connect(samplingRate, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));

	connect(artsConfig->audioIO,SIGNAL(highlighted(int)),SLOT(slotChanged()));
	connect(artsConfig->customOptions,SIGNAL(clicked()),SLOT(slotChanged()));
	connect(artsConfig->addOptions,SIGNAL(textChanged(const QString&)),SLOT(slotChanged()));
	connect(artsConfig->soundQuality,SIGNAL(highlighted(int)),SLOT(slotChanged()));
	connect(artsConfig->latencySlider,SIGNAL(sliderMoved(int)),SLOT(slotChanged()));
	connect(artsConfig->autoSuspend,SIGNAL(clicked()),SLOT(slotChanged()));
	connect(artsConfig->suspendTime,SIGNAL(valueChanged(const QString &)),SLOT(slotChanged()));
	connect(displayMessage, SIGNAL(clicked()), SLOT(slotChanged()));
	connect(messageApplication, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
	connect(artsConfig->loggingLevel,SIGNAL(highlighted(int)),SLOT(slotChanged()));

	connect(artsConfig->testSound,SIGNAL(clicked()),SLOT(slotTestSound()));
}

void KArtsModule::GetSettings( void )
{
	config->setGroup("Arts");
	startServer->setChecked(config->readBoolEntry("StartServer",true));
	startRealtime->setChecked(config->readBoolEntry("StartRealtime",true));
	networkTransparent->setChecked(config->readBoolEntry("NetworkTransparent",false));
	x11Comm->setChecked(config->readBoolEntry("X11GlobalComm",false));
	fullDuplex->setChecked(config->readBoolEntry("FullDuplex",false));
	autoSuspend->setChecked(config->readBoolEntry("AutoSuspend",true));
	suspendTime->setValue(config->readNumEntry("SuspendTime",60));
	deviceName->setText(config->readEntry("DeviceName",QString::null));
	customDevice->setChecked(!deviceName->text().isEmpty());
	artsConfig->addOptions->setText(config->readEntry("AddOptions",QString::null));
	artsConfig->customOptions->setChecked(!artsConfig->addOptions->text().isEmpty());
	artsConfig->latencySlider->setValue(config->readNumEntry("Latency",250));
	messageApplication->setText(config->readEntry("MessageApplication","artsmessage"));
	displayMessage->setChecked(!messageApplication->text().isEmpty());
	artsConfig->loggingLevel->setCurrentItem(3 - config->readNumEntry("LoggingLevel",3));

	int rate = config->readNumEntry("SamplingRate",0);
	if(rate)
	{
		customRate->setChecked(true);
		samplingRate->setText(QString::number(rate));
	}
	else
	{
		customRate->setChecked(false);
		samplingRate->setText(QString::null);
	}

	switch (config->readNumEntry("Bits", 0)) {
	case 0:
		artsConfig->soundQuality->setCurrentItem(0);
		break;
	case 16:
		artsConfig->soundQuality->setCurrentItem(1);
		break;
	case 8:
		artsConfig->soundQuality->setCurrentItem(2);
		break;
	}

	QString audioIO = config->readEntry("AudioIO", QString::null);
	for(AudioIOElement *a = audioIOList.first(); a != 0; a = audioIOList.next())
	{
		if(a->name == audioIO)		// first item: "autodetect"
			artsConfig->audioIO->setCurrentItem(audioIOList.at() + 1);
	}

	updateWidgets();
}

void KArtsModule::saveParams( void )
{
	QString audioIO;

	int item = artsConfig->audioIO->currentItem() - 1;	// first item: "default"

	if (item >= 0) {
		audioIO = audioIOList.at(item)->name;
	}

	QString dev = customDevice->isChecked() ? deviceName->text() : QString::null;
	QString app = displayMessage->isChecked() ? messageApplication->text() : QString::null;
	int rate = customRate->isChecked()?samplingRate->text().toLong() : 0;
	QString addOptions;
	if(artsConfig->customOptions->isChecked())
		addOptions = artsConfig->addOptions->text();

	int latency = artsConfig->latencySlider->value();
	int bits = 0;

	if (artsConfig->soundQuality->currentItem() == 1)
		bits = 16;
	else if (artsConfig->soundQuality->currentItem() == 2)
		bits = 8;

	config->setGroup("Arts");
	config->writeEntry("StartServer",startServer->isChecked());
	config->writeEntry("StartRealtime",startRealtime->isChecked());
	config->writeEntry("NetworkTransparent",networkTransparent->isChecked());
	config->writeEntry("X11GlobalComm",x11Comm->isChecked());
	config->writeEntry("FullDuplex",fullDuplex->isChecked());
	config->writeEntry("DeviceName",dev);
	config->writeEntry("SamplingRate",rate);
	config->writeEntry("AudioIO",audioIO);
	config->writeEntry("AddOptions",addOptions);
	config->writeEntry("Latency",latency);
	config->writeEntry("Bits",bits);
	config->writeEntry("AutoSuspend", autoSuspend->isChecked());
	config->writeEntry("SuspendTime", suspendTime->value());
	config->writeEntry("MessageApplication",app);
	config->writeEntry("LoggingLevel",3 - artsConfig->loggingLevel->currentItem());

	calculateLatency();
	// Save arguments string in case any other process wants to restart artsd.

	config->writeEntry("Arguments",
		createArgs(networkTransparent->isChecked(), fullDuplex->isChecked(),
					fragmentCount, fragmentSize, dev, rate, bits,
					audioIO, addOptions, autoSuspend->isChecked(),
					suspendTime->value(), app,
				    3 - artsConfig->loggingLevel->currentItem()));

	config->sync();
}

void KArtsModule::load()
{
	GetSettings();
}

void KArtsModule::save()
{
	if (!configChanged)
		return;

	if (startRealtime->isChecked() && !artswrapper_check()) {
		FILE *why = popen("artswrapper check 2>&1","r");

		QString thereason;
		if (why) {
			char reason[1024];

			while (fgets(reason,1024,why)) {
				thereason += reason;
			}
			fclose(why);
		}

		// TODO: check the length, to avoid a possible DoS
		if (thereason.isEmpty()) {
			thereason = i18n("artswrapper couldn't be launched");
		}

		KMessageBox::error( 0,
				    i18n("There is an installation problem which doesn't allow "
					 "starting the aRts server with realtime priority. \n"
					 "The following problem occured:\n")+thereason);
	}

	configChanged = false;
	saveParams();

    QString dialogText;
    if(artsConfig->startServer->isChecked()) {
        dialogText = i18n("Restart sound-server now?\nThis is needed for your changes to take effect\n\nRestarting the sound server might confuse or\neven crash applications using the sound server.");
    } else {
        dialogText = i18n("Shut down sound-server now?\nThis might confuse or even crash applications\nusing the sound server.");
    }
        
	if(KMessageBox::warningYesNo(this,
                     dialogText,
				     i18n("Restart sound server now?")) == KMessageBox::Yes)
	{
		system("artsshell terminate");
		sleep(1);				// leave artsd some time to go away
		init_arts();
	}
}

void KArtsModule::slotTestSound()
{
	if(configChanged) {
		save();
		sleep(1);
	}

	QCString playCmd = "artsplay ";
	playCmd += locate( "sound", "KDE_Startup.wav" ).ascii();
	system(playCmd);
}

void KArtsModule::defaults()
{
	startServer->setChecked(true);
	startRealtime->setChecked(true);
	networkTransparent->setChecked(false);
	x11Comm->setChecked(false);
	fullDuplex->setChecked(false);
	autoSuspend->setChecked(true);
	suspendTime->setValue(60);
	customDevice->setChecked(false);
	deviceName->setText(QString::null);
	customRate->setChecked(false);
	samplingRate->setText(QString::null);
	artsConfig->customOptions->setChecked(false);
	artsConfig->addOptions->setText(QString::null);
	artsConfig->audioIO->setCurrentItem(0);
	artsConfig->soundQuality->setCurrentItem(0);
	artsConfig->latencySlider->setValue(250);
	displayMessage->setChecked(true);
	messageApplication->setText("artsmessage");
	artsConfig->loggingLevel->setCurrentItem(0);
	slotChanged();
}

QString KArtsModule::quickHelp() const
{
	return i18n("<h1>The aRts sound server</h1> Here you can configure aRts, KDE's sound server."
		    " This program not only allows you to hear your system sounds while simultaneously"
		    " listening to some MP3 file or playing a game with a background music. It also allows you"
		    " to apply different effects to your system sounds and provides programmers with"
		    " an easy way to achieve sound support.");
}

void KArtsModule::calculateLatency()
{
	int latencyInBytes, latencyInMs;

	if(artsConfig->latencySlider->value() < 490)
	{
		int rate = customRate->isChecked() ? samplingRate->text().toLong() : 44100;

		if (rate < 4000 || rate > 200000) {
			rate = 44100;
		}

		int sampleSize = (artsConfig->soundQuality->currentItem() == 2) ? 2 : 4;

		latencyInBytes = artsConfig->latencySlider->value()*rate*sampleSize/1000;

		fragmentSize = 2;
		do {
			fragmentSize *= 2;
			fragmentCount = latencyInBytes / fragmentSize;
		} while (fragmentCount > 8 && fragmentSize != 4096);

		latencyInMs = (fragmentSize*fragmentCount*1000) / rate / sampleSize;
		artsConfig->latencyLabel->setText(
						  i18n("%1 milliseconds (%2 fragments with %3 bytes)")
						  .arg(latencyInMs).arg(fragmentCount).arg(fragmentSize));
	}
	else
	{
		fragmentCount = 128;
		fragmentSize = 8192;
		artsConfig->latencyLabel->setText(i18n("as large as possible"));
	}
}

void KArtsModule::updateWidgets()
{
	startRealtime->setEnabled(startServer->isChecked());
	networkTransparent->setEnabled(startServer->isChecked());
	x11Comm->setEnabled(startServer->isChecked());
	fullDuplex->setEnabled(startServer->isChecked());
	customDevice->setEnabled(startServer->isChecked());
	deviceName->setEnabled(startServer->isChecked() && customDevice->isChecked());
	customRate->setEnabled(startServer->isChecked());
	samplingRate->setEnabled(startServer->isChecked() && customRate->isChecked());
	artsConfig->customOptions->setEnabled(startServer->isChecked());
	artsConfig->addOptions->setEnabled(startServer->isChecked() && artsConfig->customOptions->isChecked());
	artsConfig->soundIO->setEnabled(startServer->isChecked());
	artsConfig->testSound->setEnabled(startServer->isChecked());
	autoSuspend->setEnabled(startServer->isChecked());
	suspendTime->setEnabled(startServer->isChecked() && autoSuspend->isChecked());
	displayMessage->setEnabled(startServer->isChecked());
	messageApplication->setEnabled(startServer->isChecked() && displayMessage->isChecked());
	calculateLatency();
}

void KArtsModule::slotChanged()
{
	updateWidgets();
	configChanged = true;
	emit changed(true);
}


KCModule *create_arts(QWidget *parent, const char *name)
{
	KGlobal::locale()->insertCatalogue("kcmarts");
	return new KArtsModule(parent, name);
}

void init_arts()
{
	KConfig *config = new KConfig("kcmartsrc", true, false);

	config->setGroup("Arts");
	bool startServer = config->readBoolEntry("StartServer",true);
	bool startRealtime = config->readBoolEntry("StartRealtime",true);
	bool x11Comm = config->readBoolEntry("X11GlobalComm",false);
	QString args = config->readEntry("Arguments","-F 10 -S 4096 -s 60 -m artsmessage -l 3 -f");

	delete config;

	/* put the value of x11Comm into .mcoprc */
	KConfig *X11CommConfig = new KSimpleConfig(QDir::homeDirPath()+"/.mcoprc");

	if(x11Comm)
		X11CommConfig->writeEntry("GlobalComm","Arts::X11GlobalComm");
	else
		X11CommConfig->writeEntry("GlobalComm","Arts::TmpGlobalComm");

	X11CommConfig->sync();
	delete X11CommConfig;

	if (startServer)
		kapp->kdeinitExec(startRealtime?"artswrapper":"artsd",
				  QStringList::split(" ",args));
}

QString createArgs(bool netTrans,
		   bool duplex, int fragmentCount, int fragmentSize,
		   const QString &deviceName,
		   int rate, int bits, const QString &audioIO,
		   const QString &addOptions, bool autoSuspend,
		   int suspendTime,
		   const QString &messageApplication, int loggingLevel)
{
	QString args;

	if(fragmentCount)
		args += QString::fromLatin1(" -F %1").arg(fragmentCount);

	if(fragmentSize)
		args += QString::fromLatin1(" -S %1").arg(fragmentSize);

	if (audioIO.length() > 0)
		args += QString::fromLatin1(" -a %1").arg(audioIO);

	if (duplex)
		args += QString::fromLatin1(" -d");

	if (netTrans)
		args += QString::fromLatin1(" -n");

	if (deviceName.length() > 0)
		args += QString::fromLatin1(" -D ") + deviceName;

	if (rate)
		args += QString::fromLatin1(" -r %1").arg(rate);

	if (bits)
		args += QString::fromLatin1(" -b %1").arg(bits);

	if (autoSuspend)
		args += QString::fromLatin1(" -s %1").arg(suspendTime);

	if (messageApplication.length() > 0)
		args += QString::fromLatin1(" -m ") + messageApplication;

	if (addOptions.length() > 0)
		args += QChar(' ') + addOptions;

	args += QString::fromLatin1(" -l %1").arg(loggingLevel);
	args += QString::fromLatin1(" -f");

	return args;
}

void dummy() {
	//lukas: these are hacks to allow translation of the following
	(void) i18n("No audio input/output");
	(void) i18n("Advanced Linux Sound Architecture");
	(void) i18n("Open Sound System");
}

#include "arts.moc"
