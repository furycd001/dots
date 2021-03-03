/**
 *  kcmaccess.cpp
 *
 *  Copyright (c) 2000 Matthias Hölzer-Klüpfel
 *
 */


#include <stdlib.h>


#include <qtabwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qwhatsthis.h>


#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <krun.h>
#include <kurl.h>
#include <kinstance.h>
#include <kcolorbtn.h>
#include <kfiledialog.h>
#include <knuminput.h>
#include <kapp.h>


#include "kcmaccess.moc"


KAccessConfig::KAccessConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QVBoxLayout *main = new QVBoxLayout(this, 0,0);
  QTabWidget *tab = new QTabWidget(this);
  main->addWidget(tab);

  // bell settings ---------------------------------------
  QWidget *bell = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(bell, 6,6);

  QGroupBox *grp = new QGroupBox(i18n("Audible bell"), bell);
  vbox->addWidget(grp);

  QVBoxLayout *vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  systemBell = new QCheckBox(i18n("Use &System bell"), grp);
  vvbox->addWidget(systemBell);
  customBell = new QCheckBox(i18n("Use &customized bell"), grp);
  vvbox->addWidget(customBell);
  QWhatsThis::add( systemBell, i18n("If this option is checked, the default system bell will be used. See the"
    " \"System Bell\" control module for how to customize the system bell."
    " Normally, this is just a \"beeep\".") );
  QWhatsThis::add( customBell, i18n("Check this option if you want to use a customized bell, playing"
    " a sound file. If you do this, you will probably want to turn off the system bell.<p> Please note"
    " that on slow machines this may cause a \"lag\" between the event causing the bell and the sound being played.") );

  QHBoxLayout *hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  soundEdit = new QLineEdit(grp);
  soundLabel = new QLabel(soundEdit, i18n("Sound to play:"), grp);
  hbox->addWidget(soundLabel);
  hbox->addWidget(soundEdit);
  soundButton = new QPushButton(i18n("&Browse..."), grp);
  hbox->addWidget(soundButton);
  QString wtstr = i18n("If the option \"Use customized bell\" is enabled, you can choose a sound file here."
    " Click \"Browse ...\" to choose a sound file using the file dialog.");
  QWhatsThis::add( soundEdit, wtstr );
  QWhatsThis::add( soundLabel, wtstr );
  QWhatsThis::add( soundButton, wtstr );

  connect(soundButton, SIGNAL(clicked()), this, SLOT(selectSound()));

  connect(customBell, SIGNAL(clicked()), this, SLOT(checkAccess()));

  connect(systemBell, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(customBell, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(soundEdit, SIGNAL(textChanged(const QString&)), this, SLOT(configChanged()));

  // -----------------------------------------------------

  // visible bell ----------------------------------------
  grp = new QGroupBox(i18n("Visible bell"), bell);
  vbox->addWidget(grp);

  vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  visibleBell = new QCheckBox(i18n("&Use visible bell"), grp);
  vvbox->addWidget(visibleBell);
  QWhatsThis::add( visibleBell, i18n("This option will turn on the \"visible bell\", i.e. a visible"
    " notification shown every time that normally just a bell would occur. This is especially useful"
    " for deaf people.") );

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  invertScreen = new QRadioButton(i18n("&Invert screen"), grp);
  hbox->addWidget(invertScreen);
  hbox = new QHBoxLayout(vvbox, 6);
  QWhatsThis::add( invertScreen, i18n("All screen colors will be inverted for the amount of time specified below.") );
  hbox->addSpacing(24);
  flashScreen = new QRadioButton(i18n("&Flash screen"), grp);
  hbox->addWidget(flashScreen);
  QWhatsThis::add( flashScreen, i18n("The screen will turn to a custom color for the amount of time specified below.") );
  hbox->addSpacing(12);
  colorButton = new KColorButton(grp);
  colorButton->setFixedWidth(colorButton->sizeHint().height()*2);
  hbox->addWidget(colorButton);
  hbox->addStretch();
  QWhatsThis::add( colorButton, i18n("Click here to choose the color used for the \"flash screen\" visible bell.") );

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);

  durationSlider = new KIntNumInput(grp);
  durationSlider->setRange(100, 2000, 100);
  durationSlider->setLabel(i18n("&Duration:"));
  durationSlider->setSuffix(i18n("ms"));
  hbox->addWidget(durationSlider);
  QWhatsThis::add( durationSlider, i18n("Here you can customize the duration of the \"visible bell\" effect being shown.") );

  connect(invertScreen, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(flashScreen, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(visibleBell, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(visibleBell, SIGNAL(clicked()), this, SLOT(checkAccess()));
  connect(colorButton, SIGNAL(clicked()), this, SLOT(changeFlashScreenColor()));

  connect(invertScreen, SIGNAL(clicked()), this, SLOT(invertClicked()));
  connect(flashScreen, SIGNAL(clicked()), this, SLOT(flashClicked()));

  connect(durationSlider, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));

  vbox->addStretch();

  // -----------------------------------------------------

  tab->addTab(bell, i18n("&Bell"));


  // keys settings ---------------------------------------
  QWidget *keys = new QWidget(this);

  vbox = new QVBoxLayout(keys, 6,6);

  grp = new QGroupBox(i18n("Sticky Keys"), keys);
  vbox->addWidget(grp);

  vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  stickyKeys = new QCheckBox(i18n("Use &sticky keys"), grp);
  vvbox->addWidget(stickyKeys);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  stickyKeysLock = new QCheckBox(i18n("&Lock sticky keys"), grp);
  hbox->addWidget(stickyKeysLock);

  grp = new QGroupBox(i18n("Slow Keys"), keys);
  vbox->addWidget(grp);

  vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  slowKeys = new QCheckBox(i18n("Use s&low keys"), grp);
  vvbox->addWidget(slowKeys);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  slowKeysDelay = new KIntNumInput(grp);
  slowKeysDelay->setSuffix(i18n("ms"));
  slowKeysDelay->setRange(100, 2000, 100);
  slowKeysDelay->setLabel(i18n("Delay:"));
  hbox->addWidget(slowKeysDelay);


  grp = new QGroupBox(i18n("Bounce Keys"), keys);
  vbox->addWidget(grp);

  vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  bounceKeys = new QCheckBox(i18n("Use &bounce keys"), grp);
  vvbox->addWidget(bounceKeys);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  bounceKeysDelay = new KIntNumInput(grp);
  bounceKeysDelay->setSuffix(i18n("ms"));
  bounceKeysDelay->setRange(100, 2000, 100);
  bounceKeysDelay->setLabel(i18n("Delay:"));
  hbox->addWidget(bounceKeysDelay);


  connect(stickyKeys, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(stickyKeysLock, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(slowKeysDelay, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(slowKeys, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(slowKeys, SIGNAL(clicked()), this, SLOT(checkAccess()));
  connect(stickyKeys, SIGNAL(clicked()), this, SLOT(checkAccess()));

  connect(bounceKeysDelay, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(bounceKeys, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(bounceKeys, SIGNAL(clicked()), this, SLOT(checkAccess()));

  vbox->addStretch();

  tab->addTab(keys, i18n("&Keyboard"));

  // -----------------------------------------------------



  // mouse settings ---------------------------------------
  QWidget *mouse = new QWidget(this);

  vbox = new QVBoxLayout(mouse, 6,6);

  grp = new QGroupBox(i18n("Mouse navigation"), mouse);
  vbox->addWidget(grp);

  vvbox = new QVBoxLayout(grp, 6,6);
  vvbox->addSpacing(grp->fontMetrics().height());

  mouseKeys = new QCheckBox(i18n("&Move mouse with keyboard (using the Num pad)"), grp);
  vvbox->addWidget(mouseKeys);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  mk_delay = new KIntNumInput(grp);
  mk_delay->setLabel(i18n("&Acceleration delay"), AlignVCenter);
  mk_delay->setSuffix(i18n("ms"));
  mk_delay->setRange(1, 1000, 50);
  hbox->addWidget(mk_delay);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  mk_interval = new KIntNumInput(mk_delay, 0, grp);
  mk_interval->setLabel(i18n("&Repeat interval"), AlignVCenter);
  mk_interval->setSuffix(i18n("ms"));
  mk_interval->setRange(1, 1000, 10);
  hbox->addWidget(mk_interval);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  mk_time_to_max = new KIntNumInput(mk_interval, 0, grp);
  mk_time_to_max->setLabel(i18n("Acceleration &time"), AlignVCenter);
  mk_time_to_max->setRange(1, 5000, 250);
  hbox->addWidget(mk_time_to_max);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  mk_max_speed = new KIntNumInput(mk_time_to_max, 0, grp);
  mk_max_speed->setLabel(i18n("&Maximum speed"), AlignVCenter);
  mk_max_speed->setRange(1, 1000, 10);
  hbox->addWidget(mk_max_speed);

  hbox = new QHBoxLayout(vvbox, 6);
  hbox->addSpacing(24);
  mk_curve = new KIntNumInput(mk_max_speed, 0, grp);
  mk_curve->setLabel(i18n("Acceleration &profile"), AlignVCenter);
  mk_curve->setRange(-1000, 1000, 100);
  hbox->addWidget(mk_curve);

  connect(mouseKeys, SIGNAL(clicked()), this, SLOT(checkAccess()));
  connect(mouseKeys, SIGNAL(clicked()), this, SLOT(configChanged()));
  connect(mk_delay, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(mk_interval, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(mk_time_to_max, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(mk_max_speed, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));
  connect(mk_curve, SIGNAL(valueChanged(int)), this, SLOT(configChanged()));

  vbox->addStretch();

  tab->addTab(mouse, i18n("&Mouse"));

  // -----------------------------------------------------


  vbox->addStretch();

  load();
}


KAccessConfig::~KAccessConfig()
{
}

void KAccessConfig::changeFlashScreenColor()
{
    invertScreen->setChecked(false);
    flashScreen->setChecked(true);
    configChanged();
}

void KAccessConfig::selectSound()
{
  QStringList list = KGlobal::dirs()->findDirs("sound", "");
  QString start;
  if (list.count()>0)
    start = list[0];
  // TODO: Why only wav's? How can I find out what artsd supports?
  QString fname = KFileDialog::getOpenFileName(start, i18n("*.wav|WAV files"));
  if (!fname.isEmpty())
    soundEdit->setText(fname);
}


void KAccessConfig::configChanged()
{
  emit changed(true);
}


void KAccessConfig::load()
{
  KConfig *config = new KConfig("kaccessrc", true);

  config->setGroup("Bell");

  systemBell->setChecked(config->readBoolEntry("SystemBell", true));
  customBell->setChecked(config->readBoolEntry("ArtsBell", false));
  soundEdit->setText(config->readEntry("ArtsBellFile"));

  visibleBell->setChecked(config->readBoolEntry("VisibleBell", false));
  invertScreen->setChecked(config->readBoolEntry("VisibleBellInvert", true));
  flashScreen->setChecked(!invertScreen->isChecked());
  QColor def(Qt::red);
  colorButton->setColor(config->readColorEntry("VisibleBellColor", &def));

  durationSlider->setValue(config->readNumEntry("VisibleBellPause", 500));


  config->setGroup("Keyboard");

  stickyKeys->setChecked(config->readBoolEntry("StickyKeys", false));
  stickyKeysLock->setChecked(config->readBoolEntry("StickyKeysLatch", true));
  slowKeys->setChecked(config->readBoolEntry("SlowKeys", false));
  slowKeysDelay->setValue(config->readNumEntry("SlowKeysDelay", 500));
  bounceKeys->setChecked(config->readBoolEntry("BounceKeys", false));
  bounceKeysDelay->setValue(config->readNumEntry("BounceKeysDelay", 500));


  config->setGroup("Mouse");
  mouseKeys->setChecked(config->readBoolEntry("MouseKeys", false));
  mk_delay->setValue(config->readNumEntry("MKDelay", 160));
  mk_interval->setValue(config->readNumEntry("MKInterval", 5));
  mk_time_to_max->setValue(config->readNumEntry("MKTimeToMax", 1000));
  mk_max_speed->setValue(config->readNumEntry("MKMaxSpeed", 500));
  mk_curve->setValue(config->readNumEntry("MKCurve", 0));

  delete config;

  checkAccess();

  emit changed(false);
}


void KAccessConfig::save()
{
  KConfig *config= new KConfig("kaccessrc", false);

  config->setGroup("Bell");

  config->writeEntry("SystemBell", systemBell->isChecked());
  config->writeEntry("ArtsBell", customBell->isChecked());
  config->writeEntry("ArtsBellFile", soundEdit->text());

  config->writeEntry("VisibleBell", visibleBell->isChecked());
  config->writeEntry("VisibleBellInvert", invertScreen->isChecked());
  config->writeEntry("VisibleBellColor", colorButton->color());

  config->writeEntry("VisibleBellPause", durationSlider->value());


  config->setGroup("Keyboard");

  config->writeEntry("StickyKeys", stickyKeys->isChecked());
  config->writeEntry("StickyKeysLatch", stickyKeysLock->isChecked());

  config->writeEntry("SlowKeys", slowKeys->isChecked());
  config->writeEntry("SlowKeysDelay", slowKeysDelay->value());

  config->writeEntry("BounceKeys", bounceKeys->isChecked());
  config->writeEntry("BounceKeysDelay", bounceKeysDelay->value());


  config->setGroup("Mouse");

  config->writeEntry("MouseKeys", mouseKeys->isChecked());
  config->writeEntry("MKDelay", mk_delay->value());
  config->writeEntry("MKInterval", mk_interval->value());
  config->writeEntry("MKTimeToMax", mk_time_to_max->value());
  config->writeEntry("MKMaxSpeed", mk_max_speed->value());
  config->writeEntry("MKCurve", mk_curve->value());

  config->sync();
  delete config;

  // restart kaccess
  kapp->startServiceByDesktopName("kaccess");

  emit changed(false);
}


void KAccessConfig::defaults()
{
  systemBell->setChecked(true);
  customBell->setChecked(false);
  soundEdit->setText("");

  visibleBell->setChecked(false);
  invertScreen->setChecked(true);
  flashScreen->setChecked(false);
  colorButton->setColor(QColor(Qt::red));

  durationSlider->setValue(500);

  slowKeys->setChecked(false);
  slowKeysDelay->setValue(500);

  bounceKeys->setChecked(false);
  bounceKeysDelay->setValue(500);

  stickyKeys->setChecked(true);
  stickyKeysLock->setChecked(true);

  mouseKeys->setChecked(false);
  mk_delay->setValue(160);
  mk_interval->setValue(5);
  mk_time_to_max->setValue(1000);
  mk_max_speed->setValue(500);
  mk_curve->setValue(0);

  checkAccess();

  emit changed(true);
}


void KAccessConfig::invertClicked()
{
  flashScreen->setChecked(false);
}


void KAccessConfig::flashClicked()
{
  invertScreen->setChecked(false);
}


void KAccessConfig::checkAccess()
{
  bool custom = customBell->isChecked();
  soundEdit->setEnabled(custom);
  soundButton->setEnabled(custom);
  soundLabel->setEnabled(custom);

  bool visible = visibleBell->isChecked();
  invertScreen->setEnabled(visible);
  flashScreen->setEnabled(visible);
  colorButton->setEnabled(visible);

  durationSlider->setEnabled(visible);

  stickyKeysLock->setEnabled(stickyKeys->isChecked());

  slowKeysDelay->setEnabled(slowKeys->isChecked());

  bounceKeysDelay->setEnabled(bounceKeys->isChecked());

  mk_delay->setEnabled(mouseKeys->isChecked());
  mk_interval->setEnabled(mouseKeys->isChecked());
  mk_time_to_max->setEnabled(mouseKeys->isChecked());
  mk_max_speed->setEnabled(mouseKeys->isChecked());
  mk_curve->setEnabled(mouseKeys->isChecked());
}


extern "C"
{
  KCModule *create_access(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmaccess");
    return new KAccessConfig(parent, name);
  };

  void init_access()
  {
    bool run=false;

    KConfig *config = new KConfig("kaccessrc", true, false);

    config->setGroup("Bell");

    if (!config->readBoolEntry("SystemBell", true))
      run = true;
    if (config->readBoolEntry("ArtsBell", false))
      run = true;
    if (config->readBoolEntry("VisibleBell", false))
      run = true;

    delete config;
    if (run)
      kapp->startServiceByDesktopName("kaccess");
  }
}
