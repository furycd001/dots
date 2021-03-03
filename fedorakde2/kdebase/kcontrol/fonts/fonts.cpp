// KDE Display fonts setup tab
//
// Copyright (c)  Mark Donohoe 1997
//                lars Knoll 1999
//                Rik Hemsley 2000
//
// Ported to kcontrol2 by Geert Jansen.

/* $Id: fonts.cpp,v 1.25 2001/06/27 03:32:26 hasso Exp $ */

#include <qlayout.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qframe.h>
#include <qcheckbox.h>

// X11 headers
#undef Bool
#undef Unsorted

#include <kapp.h>
#include <qbuttongroup.h>
#include <dcopclient.h>
#include <kglobalsettings.h>
#include <kfontdialog.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kipc.h>
#include <kstddirs.h>
#include <kcharsets.h>
#include <kprocess.h>
#include <kmessagebox.h>
#include <stdlib.h>

#include "fonts.h"
#include "fonts.moc"

#include <kdebug.h>

/**** DLL Interface ****/

void applyQtXFT(bool active)
{
   // Pass env. var to kdeinit.
   QCString name = "QT_XFT";
   QCString value = active ? "1" : "0";
   QByteArray params;
   QDataStream stream(params, IO_WriteOnly);
   stream << name << value;
   kapp->dcopClient()->send("klauncher", "klauncher", "setLaunchEnv(QCString,QCString)", params);
}

extern "C" {
  KCModule *create_fonts(QWidget *parent, const char *name) {
    KGlobal::locale()->insertCatalogue("kcmfonts");
    return new KFonts(parent, name);
  }

  void init_fonts() {
    KConfig aacfg("kdeglobals");
    aacfg.setGroup("KDE");
    applyQtXFT(aacfg.readBoolEntry( "AntiAliasing", false ));
  }
}


/**** FontUseItem ****/

FontUseItem::FontUseItem(
  QWidget * parent,
  QLabel * prvw,
  QString n,
  QString grp,
  QString key,
  QString rc,
  QFont default_fnt,
  QString default_charset,
  bool f
)
  : QObject(),
    prnt(parent),
    preview(prvw),
    _text(n),
    _rcfile(rc),
    _rcgroup(grp),
    _rckey(key),
    _font(default_fnt),
    _charset(default_charset),
    _default(default_fnt),
    _defaultCharset(default_charset),
    fixed(f)
{
  readFont();
}

QString FontUseItem::fontString(QFont rFont)
{
  QString aValue;
  aValue = rFont.rawName();
  return aValue;
}

void FontUseItem::setDefault()
{
  _font = _default;
  _charset = _defaultCharset;
  updateLabel();
}

void FontUseItem::readFont()
{
  KConfigBase *config;

  bool deleteme = false;
  if (_rcfile.isEmpty())
    config = KGlobal::config();
  else
  {
    config = new KSimpleConfig(locate("config", _rcfile), true);
    deleteme = true;
  }

  config->setGroup(_rcgroup);
  QFont tmpFnt(_font);
  _font = config->readFontEntry(_rckey, &tmpFnt);
  _charset = config->readEntry( _rckey + "Charset", "default" );
  if ( _charset == "default" )
  {
    _charset = i18n("default");
    KGlobal::charsets()->setQFont(_font, KGlobal::locale()->charset());
  }
  if (deleteme) delete config;
  updateLabel();
}

void FontUseItem::writeFont()
{
  KConfigBase *config;
  QString charset = ( _charset == i18n("default") ) ? QString::fromLatin1("default") : _charset;

  if (_rcfile.isEmpty()) {
    config = KGlobal::config();
    config->setGroup(_rcgroup);
    config->writeEntry(_rckey, _font, true, true);
    config->writeEntry(_rckey+"Charset", charset, true, true);
    config->sync();
  } else {
    config = new KSimpleConfig(locateLocal("config", _rcfile));
    config->setGroup(_rcgroup);
    config->writeEntry(_rckey, _font);
    config->writeEntry(_rckey+"Charset", charset);
    config->sync();
    delete config;
  }
}

void FontUseItem::choose()
{
  KFontDialog dlg( prnt, "Font Selector", fixed, true, QStringList(), true );
  dlg.setFont( _font, fixed );
  dlg.setCharset( _charset );

  int result = dlg.exec();
  if (KDialog::Accepted == result)
  {
    _font = dlg.font();
    _charset = dlg.charset();
    updateLabel();
    emit changed();
  }
}

void FontUseItem::updateLabel()
{
  QString fontDesc = _font.family() + ' ' + QString::number(_font.pointSize()) + ' ' + _charset;

  preview->setText(fontDesc);
  preview->setFont(_font);
}

/**** KFonts ****/

KFonts::KFonts(QWidget *parent, const char *name)
    :   KCModule(parent, name),
        _changed(false)
{
  QStringList nameGroupKeyRc;

  nameGroupKeyRc
    << i18n("General")        << "General"    << "font"         << ""
    << i18n("Fixed width")    << "General"    << "fixed"        << ""
    << i18n("Toolbar")        << "General"    << "toolBarFont"  << ""
    << i18n("Menu")           << "General"    << "menuFont"     << ""
    << i18n("Window title")   << "WM"         << "activeFont"   << ""
    << i18n("Taskbar")        << "General"    << "taskbarFont"  << "";

  QValueList<QFont> defaultFontList;

  // Keep in sync with kglobalsettings.

  QFont f0("helvetica", 12);
  QFont f1("courier", 12);
  QFont f2("helvetica", 10);
  QFont f3("helvetica", 12, QFont::Bold);
  QFont f4("helvetica", 11);

  f0.setPointSize(12);
  f1.setPointSize(10);
  f2.setPointSize(12);
  f3.setPointSize(12);
  f4.setPointSize(11);

  defaultFontList << f0 << f1 << f2 << f0 << f3 << f4;

  QValueList<bool> fixedList;

  fixedList
    <<  false
    <<  true
    <<  false
    <<  false
    <<  false
    <<  false;

  QStringList quickHelpList;

  quickHelpList
    << i18n("Used for normal text (e.g. button labels, list items).")
    << i18n("A non-proportional font (i.e. typewriter font).")
    << i18n("Used to display text beside toolbar icons.")
    << i18n("Used by menu bars and popup menus.")
    << i18n("Used by the window titlebar.")
    << i18n("Used by the taskbar.");

  QVBoxLayout * layout =
    new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

  QGridLayout * fontUseLayout =
    new QGridLayout(layout, nameGroupKeyRc.count() / 4, 3);

  fontUseLayout->setColStretch(0, 0);
  fontUseLayout->setColStretch(1, 1);
  fontUseLayout->setColStretch(2, 0);

  QValueList<QFont>::ConstIterator defaultFontIt(defaultFontList.begin());
  QValueList<bool>::ConstIterator fixedListIt(fixedList.begin());
  QStringList::ConstIterator quickHelpIt(quickHelpList.begin());
  QStringList::ConstIterator it(nameGroupKeyRc.begin());

  unsigned int count = 0;

  while (it != nameGroupKeyRc.end()) {

    QLabel * preview = new QLabel(this);
    preview->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    // preview->setMaximumWidth(200);

    FontUseItem * i =
      new FontUseItem(
        this,
        preview,
        *it++,
        *it++,
        *it++,
        *it++,
        *defaultFontIt++,
        i18n("default"),
        *fixedListIt++
      );

    fontUseList.append(i);
    connect(i, SIGNAL(changed()), this, SLOT(fontChanged()));

    QWhatsThis::add(preview, i18n("This is a preview of the \"%1\" font. You can change this font by clicking the \"Choose...\" button to the right.").arg(i->text()));
    QToolTip::add(preview, i18n("Preview of the \"%1\" font").arg(i->text()));

    QLabel * fontUse = new QLabel(i->text(), this);

    QWhatsThis::add(fontUse, *quickHelpIt++);

    QPushButton * chooseButton = new QPushButton(i18n("Choose..."), this);

    QWhatsThis::add(chooseButton, i18n("Click to select a font"));

    connect(chooseButton, SIGNAL(clicked()), i, SLOT(choose()));

    fontUseLayout->addWidget(fontUse, count, 0);
    fontUseLayout->addWidget(preview, count, 1);
    fontUseLayout->addWidget(chooseButton, count, 2);

    ++count;
  }

   cbAA = new QCheckBox( i18n( "Use A&nti-Aliasing for fonts and icons" ),this);

   QWhatsThis::add(cbAA,
     i18n(
       "If this option is selected, KDE will use anti-aliased fonts and "
       "pixmaps, meaning fonts can use more than"
       " just one color to simulate curves."));

   layout->addWidget(cbAA);
   layout->addStretch(1);

   connect(cbAA, SIGNAL(clicked()), SLOT(slotUseAntiAliasing()));

   KConfig aacfg("kdeglobals", true, true);
   aacfg.setGroup("KDE");
   useAA = aacfg.readBoolEntry( "AntiAliasing", true);
   useAA_original = useAA;

   cbAA->setChecked(useAA);
}

KFonts::~KFonts()
{
}

void KFonts::fontChanged()
{
  _changed = true;
  emit changed(true);
}

void KFonts::defaults()
{
  for ( int i = 0; i < (int) fontUseList.count(); i++ )
    fontUseList.at( i )->setDefault();

  _changed = true;
  emit changed(true);
}

QString KFonts::quickHelp() const
{
    return i18n( "<h1>Fonts</h1> This module allows you to choose which"
      " fonts will be used to display text in KDE. You can select not only"
      " the font family (for example, <em>helvetica</em> or <em>times</em>),"
      " but also the attributes that make up a specific font (for example,"
      " <em>bold</em> style and <em>12 points</em> in height.)<p>"
      " Just click the \"Choose\" button that is next to the font you want to"
      " change."
      " You can ask KDE to try and apply font and color settings to non-KDE"
      " applications as well. See the \"Style\" control module for more"
			" information.");
}

void KFonts::load()
{
  for ( int i = 0; i < (int) fontUseList.count(); i++ )
    fontUseList.at( i )->readFont();

  KConfig aacfg("kdeglobals", true, true);
  aacfg.setGroup("KDE");
  useAA = aacfg.readBoolEntry( "AntiAliasing", false);
  useAA_original = useAA;
  kdDebug() << "AA:" << useAA << endl;
  cbAA->setChecked(useAA);

  _changed = true;
  emit changed(false);

}

void KFonts::save()
{
  if ( !_changed )
    return;

  for ( FontUseItem* i = fontUseList.first(); i; i = fontUseList.next() )
      i->writeFont();

  // KDE-1.x support
  KSimpleConfig* config = new KSimpleConfig( QCString(::getenv("HOME")) + "/.kderc" );
  config->setGroup( "General" );
  for ( FontUseItem* i = fontUseList.first(); i; i = fontUseList.next() ) {
      kdDebug () << "write entry " <<  i->rcKey() << endl;
      config->writeEntry( i->rcKey(), i->font() );
  }
  config->sync();
  delete config;

  KIPC::sendMessageAll(KIPC::FontChanged);

  KConfig * cfg = KGlobal::config();
  cfg->setGroup("X11");

  if (cfg->readBoolEntry("useResourceManager", true)) {
    QApplication::setOverrideCursor( waitCursor );
    KProcess proc;
    proc.setExecutable("krdb2");
    proc.start( KProcess::Block );
    QApplication::restoreOverrideCursor();
  }

  applyQtXFT(useAA); // Apply config now

  KConfig aacfg("kdeglobals");
  aacfg.setGroup("KDE");
  aacfg.writeEntry( "AntiAliasing", useAA);
  aacfg.sync();

  if(useAA != useAA_original) {
    KMessageBox::information(this, i18n("You have changed anti-aliasing related settings.\nThis change won't take effect until you restart KDE."), i18n("Anti-aliasing settings changed"), "AAsettingsChanged", false);
    useAA_original = useAA;
  }
  _changed = false;
  emit changed(false);
}

int KFonts::buttons()
{
  return KCModule::Help | KCModule::Default | KCModule::Apply;
}

void KFonts::slotUseAntiAliasing()
{
    useAA = cbAA->isChecked();
    _changed = true;
    emit changed(true);
}

// vim:ts=2:sw=2:tw=78
