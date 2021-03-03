#include <stdlib.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qheader.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

#include <kkeydialog.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kapp.h>

#include "rules.h"
#include "kcmlayout.moc"
#include "pixmap.h"
#include "kcmmisc.h"

#include <X11/Xlib.h>

LayoutConfig::LayoutConfig(QWidget *parent, const char *name)
  : KCModule(parent, name), rules(0)
{
  QVBoxLayout *main = new QVBoxLayout(this, 0,0);
  QTabWidget *tab = new QTabWidget(this);
  main->addWidget(tab);

  QWidget *layout = new QWidget(this);
  QVBoxLayout *vvbox = new QVBoxLayout(layout, 6,6);

  disableCheckbox = new QCheckBox( i18n( "Disable keyboard layouts" ), layout );
  QString wtstr = i18n("Here you can completely disable this module, for example if you use"
    " other tools for switching keyboard layouts." );
  connect( disableCheckbox, SIGNAL( toggled( bool )), this, SLOT(configChanged()));
  vvbox->addWidget( disableCheckbox );
  QWhatsThis::add( disableCheckbox, wtstr );

  QGroupBox *grp = new QGroupBox(i18n("Configuration"), layout);
  connect( disableCheckbox, SIGNAL( toggled( bool )), grp, SLOT( setDisabled( bool )));
  vvbox->addWidget(grp);

  QGridLayout *grid = new QGridLayout(grp, 4,2, 6,6);
  grid->addRowSpacing(0, grp->fontMetrics().height());

  QLabel *l;
  /*  ruleCombo = new QComboBox(grp);
  l = new QLabel(ruleCombo, i18n("Keyboard &Rule Set"), grp);
  grid->addWidget(l, 1,0);
  grid->addWidget(ruleCombo, 1,1);

  connect(ruleCombo, SIGNAL(activated(const QString&)), this, SLOT(ruleChanged(const QString&)));
  connect(ruleCombo, SIGNAL(activated(const QString&)), this, SLOT(configChanged()));
  */

  modelCombo = new QComboBox(grp);
  l = new QLabel(modelCombo, i18n("Keyboard &Model"), grp);
  grid->addWidget(l, 2,0);
  grid->addWidget(modelCombo, 2,1);
  wtstr = i18n("Here you can choose a keyboard model. This setting is independent of your"
    " keyboard layout and refers to the \"hardware\" model, i.e. the way your keyboard is manufactured."
    " Modern keyboards that come with your computer usually have two extra keys and are referred to as"
    " \"104-key\" models, which is probably what you want if you don't know what kind of keyboard you have.");
  QWhatsThis::add( modelCombo, wtstr );
  QWhatsThis::add( l, wtstr );

  connect(modelCombo, SIGNAL(activated(int)), this, SLOT(configChanged()));

  layoutCombo = new QComboBox(grp);
  l = new QLabel(layoutCombo, i18n("Primary &Layout"), grp);
  grid->addWidget(l, 3,0);
  grid->addWidget(layoutCombo, 3,1);
  wtstr = i18n("Here you can choose your primary keyboard layout, i.e. the keyboard layout used as default."
    " The keyboard layout defines \"which keys do what\". For example German keyboards have"
    " the letter 'Y' where US keyboards have the letter 'Z'.  ");
  QWhatsThis::add( layoutCombo, wtstr );
  QWhatsThis::add( l, wtstr );

  connect(layoutCombo, SIGNAL(activated(int)), this, SLOT(configChanged()));

  grp = new QGroupBox(i18n("Additional layouts"), layout);
  connect( disableCheckbox, SIGNAL( toggled( bool )), grp, SLOT( setDisabled( bool )));
  vvbox->addWidget(grp,1);
  QWhatsThis::add( grp, i18n("You can select an arbitrary number of additional keyboard layouts."
    " If one or more additional layouts have been selected, the KDE panel will offer a docked flag."
    " By clicking on this flag you can easily switch between layouts.") );

  QVBoxLayout *vbox = new QVBoxLayout(grp, 6,6);
  vbox->addSpacing(grp->fontMetrics().height());

  additional = new QListView(grp);
  vbox->addWidget(additional);
  additional->header()->hide();
  additional->addColumn(i18n("Check"), 22);
  additional->addColumn(i18n("Map"), 28);
  additional->addColumn(i18n("Layout"));

  connect(additional, SIGNAL(clicked(QListViewItem*)), this, SLOT(configChanged()));

  misc = new KeyboardConfig(tab);
  connect( misc, SIGNAL( changed(bool) ), SIGNAL( changed(bool) ));


  tab->addTab(layout, i18n("Layout"));
  tab->addTab(misc, i18n("Advanced"));

  load();
}


LayoutConfig::~LayoutConfig()
{
}


void setCurrent(QComboBox *box, QString text)
{
  for (int i=0; i<box->count(); ++i)
    if (box->text(i) == text)
      {
	box->setCurrentItem(i);
	return;
      }
}


QString lookup(const QDict<char> &dict, QString text)
{
  QDictIterator<char> it(dict);
  while (it.current())
    {
      if ( it.current() == text )
	return it.currentKey();
      ++it;
    }

  return QString::null;
}

QString lookupLocalized(const QDict<char> &dict, QString text)
{

  QDictIterator<char> it(dict);
  while (it.current())
    {
      if ( i18n(it.current()) == text )
        return it.currentKey();
      ++it;
    }

  return QString::null;
}

void LayoutConfig::load()
{
  // fill the rules combo
  /*
  ruleCombo->clear();
  ruleCombo->insertStringList(KeyRules::rules());
  */

  misc->load();

  // open the config file
  KConfig *config = new KConfig("kxkbrc", true);
  config->setGroup("Layout");

  bool use = config->readBoolEntry( "Use", false );

  // find out which rule applies
  QString rule = config->readEntry("Rule", "xfree86");
  //  setCurrent(ruleCombo, rule);

  // update the other files
  ruleChanged(rule);

  // find out about the layout and the model
  QString model = config->readEntry("Model", "pc104");
  QString layout = config->readEntry("Layout", "us");

  // TODO: When no model and no layout are known (1st start), we really
  // should ask the X-server about it's settings!

  QString m_name = rules->models()[model];
  setCurrent(modelCombo, i18n(m_name.local8Bit()));

  QString l_name = rules->layouts()[layout];
  setCurrent(layoutCombo, i18n(l_name.local8Bit()));

  // fill in the additional layouts
  additional->clear();
  QStringList tmp;
  QDictIterator<char> it2(rules->layouts());
  while (it2.current())
    {
      tmp.append(it2.current());
      ++it2;
    }
  tmp.sort();

  QStringList adds = config->readListEntry("Additional");

  QStringList::Iterator l;
  for (l = tmp.begin(); l != tmp.end(); ++l)
    {
      QCheckListItem *item = new QCheckListItem(additional, "", QCheckListItem::CheckBox);
      if (adds.contains(lookup(rules->layouts(), *l)))
	item->setOn(true);
      item->setPixmap(1, findPixmap(lookup(rules->layouts(), *l)));
      item->setText(2, i18n((*l).local8Bit())  );
    }
  additional->setSorting(2);

  delete config;

  disableCheckbox->setChecked( !use );
}


void LayoutConfig::ruleChanged(const QString &rule)
{
  QString model, layout;
  if (rules)
    {
      model = lookupLocalized(rules->models(), modelCombo->currentText());
      layout = lookupLocalized(rules->layouts(), layoutCombo->currentText());
    }

  delete rules;
  rules = new KeyRules(rule);

  QStringList tmp;
  modelCombo->clear();
  QDictIterator<char> it(rules->models());
  while (it.current())
    {
      tmp.append(i18n(it.current()));
      ++it;
    }
  tmp.sort();
  modelCombo->insertStringList(tmp);

  tmp.clear();
  layoutCombo->clear();
  QDictIterator<char> it2(rules->layouts());
  while (it2.current())
    {
      tmp.append(it2.current());
      ++it2;
    }
  tmp.sort();

  QStringList::Iterator l;
  for (l = tmp.begin(); l != tmp.end(); ++l)
    {
      layoutCombo->insertItem(findPixmap(lookup(rules->layouts(), *l)), i18n((*l).local8Bit()));
    }

  if (!model.isEmpty())
  {
    QString m_name = rules->models()[model];
    setCurrent(modelCombo, m_name);
  }
  if (!layout.isEmpty())
  {
    QString l_name = rules->layouts()[layout];
    setCurrent(layoutCombo, l_name);
  }
}


void LayoutConfig::save()
{
  misc->save();
  KConfig *config = new KConfig("kxkbrc", false);
  config->setGroup("Layout");
  //  config->writeEntry("Rule", ruleCombo->currentText());

  QString layout = lookupLocalized(rules->layouts(), layoutCombo->currentText());
  config->writeEntry("Model", lookupLocalized(rules->models(), modelCombo->currentText()));
  config->writeEntry("Layout", layout );
  config->writeEntry("Encoding", rules->encodings().find( layout ) );

  config->writeEntry("Test", "Test");

  QStringList tmp;
  QStringList encodings;
  QListViewItem *item = additional->firstChild();
  while (item)
    {
      QCheckListItem *cli = (QCheckListItem*) item;
      if (cli->isOn()) {
	  QString layout = lookupLocalized(rules->layouts(), cli->text(2));
	tmp.append(layout);
	encodings.append(rules->encodings().find( layout ) );
      }
      item = item->nextSibling();
    }
  config->writeEntry("Additional", tmp);
  config->writeEntry("AdditionalEncodings", encodings);

  config->writeEntry("Use", !disableCheckbox->isChecked());

  config->sync();

  delete config;

  kapp->kdeinitExec("kxkb");
}


void LayoutConfig::defaults()
{
  misc->defaults();
  disableCheckbox->setChecked(true);
  ruleChanged("xfree86");

  setCurrent(modelCombo, "pc104");
  setCurrent(layoutCombo, "us");

  QListViewItem *item = additional->firstChild();
  while (item)
    {
      QCheckListItem *cli = (QCheckListItem*) item;
      cli->setOn(false);
      item = item->nextSibling();
    }
}

QString LayoutConfig::quickHelp() const
{
  return i18n("<h1>Keyboard Layout & Model</h1> Here you can choose your keyboard layout and model."
    " The 'model' refers to the type of keyboard that is connected to your computer, while the keyboard"
    " layout defines \"which key does what\" and may be different for different countries.<p>"
    " In addition to the 'Primary Layout', which will be used as the default, you can specify additional"
    " layouts, which you can easily switch between using the KDE panel.");
}

void LayoutConfig::configChanged()
{
  emit changed(true);
}


extern "C"
{
  KCModule *create_keyboard(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmlayout");
    return new LayoutConfig(parent, name);
  };

  void init_keyboard()
  {
    KConfig *config = new KConfig("kcminputrc", true); // Read-only, no globals
    config->setGroup("Keyboard");

    XKeyboardState   kbd;
    XKeyboardControl kbdc;

    XGetKeyboardControl(kapp->getDisplay(), &kbd);
    bool key = config->readBoolEntry("KeyboardRepeating", true);
    kbdc.key_click_percent = config->readNumEntry("ClickVolume", kbd.key_click_percent);
    kbdc.auto_repeat_mode = (key ? AutoRepeatModeOn : AutoRepeatModeOff);
    XChangeKeyboardControl(kapp->getDisplay(),
                           KBKeyClickPercent | KBAutoRepeatMode,
                           &kbdc);
    int numlockState = config->readNumEntry( "NumLock", 2 );
    if( numlockState != 2 )
        numlockx_change_numlock_state( numlockState == 0 );

    delete config;

    config = new KConfig("kxkbrc", true, false);
    config->setGroup("Layout");
    if (config->readBoolEntry("Use", false) == true)
        kapp->startServiceByDesktopName("kxkb");
    delete config;
  }
}
