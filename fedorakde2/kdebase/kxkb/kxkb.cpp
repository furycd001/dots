#include <unistd.h>
#include <stdlib.h>


#include <qtimer.h>
#include <qpainter.h>
#include <qiconset.h>


#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kpopupmenu.h>
#include <kglobalaccel.h>
#include <kprocess.h>


#include <X11/Xlib.h>
#define explicit int_explicit        // avoid compiler name clash in XKBlib.h
#include <X11/XKBlib.h>


#include "extension.h"
#include "rules.h"
#include "kxkb.moc"
#include "pixmap.h"


TrayWindow::TrayWindow(QWidget *parent, const char *name)
  : KSystemTray(parent, name)
{
  setLayout("C");
}


void TrayWindow::setLayout(QString layout)
{
  setPixmap(findPixmap(layout));
}


QString lookup(const QDict<char> &dict, QString text)
{
  QDictIterator<char> it(dict);
  while (it.current())
    {
      if (it.current() == text)
        return it.currentKey();
      ++it;
    }

  return QString::null;
}

void TrayWindow::setLayouts(QStringList layouts, QString rule)
{
  KeyRules rules(rule);

  int cnt=0;
  QStringList::Iterator it;
  for (it=layouts.begin(); it != layouts.end(); ++it)
    contextMenu()->insertItem(findPixmap(*it), i18n((rules.layouts()[*it])), cnt++);

  contextMenu()->insertItem(i18n("Configure..."), cnt);
}


void TrayWindow::mouseReleaseEvent(QMouseEvent *ev)
{
  if (ev->button() == QMouseEvent::LeftButton)
    emit toggled();

  KSystemTray::mouseReleaseEvent(ev);
}


KXKBApp::KXKBApp(bool allowStyles, bool GUIenabled)
  : KUniqueApplication(allowStyles, GUIenabled)
{
  // verify the Xlib has matching XKB extension
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  if (!XkbLibraryVersion(&major, &minor))
    {
      kdError() << "Xlib XKB extension does not match" << endl;
      exit(-1);
    }
  kdDebug() << "Xlib XKB extension major=" << major << " minor=" << minor << endl;

  // verify the X server has matching XKB extension
  // if yes, the XKB extension is initialized
  int opcode_rtrn;
  int error_rtrn;
  int xkb_opcode;
  if (!XkbQueryExtension(qt_xdisplay(), &opcode_rtrn, &xkb_opcode, &error_rtrn,
                         &major, &minor))
    {
      kdError() << "X server has not matching XKB extension" << endl;
      exit(-1);
    }
  kdDebug() << "X server XKB extension major=" << major << " minor=" << minor << endl;

  extension = new XKBExtension;

  tray = 0;

  // keep in sync with kcmlayout.cpp
  keys = new KGlobalAccel();
#include "kxkbbindings.cpp"
  keys->connectItem( "Next keyboard layout", this, SLOT( toggled() ));

  readSettings();
}


KXKBApp::~KXKBApp()
{
  delete keys;
  delete extension;
}


void KXKBApp::readSettings()
{
  KConfig *config = new KConfig("kxkbrc", true);
  config->setGroup("Layout");

  if (config->readBoolEntry("Use", false) == false)
    exit(-1);

  _rule = config->readEntry("Rule", "xfree86");
  rules = new KeyRules( _rule );
  _model = config->readEntry("Model", "pc104");
  _layout = config->readEntry("Layout", "");
  _encoding = config->readEntry("Encoding", "locale");
  const unsigned int * pGroup = rules->group()[_layout];
  _group = pGroup ? *pGroup : 0;

  extension->setLayout(_rule, _model, _layout, _encoding, _group);

  _list = config->readListEntry("Additional");
  _encList = config->readListEntry("AdditionalEncodings");
  if (!_list.contains(_layout)) {
    _list.append(_layout);
    _encList.append(_encoding);
  }

  if (_list.count() == 1)
    exit(-1);
  else
    {
      delete tray;
      tray = new TrayWindow;

      tray->setLayouts(_list, _rule);
      tray->setLayout(_layout);

      connect(tray->contextMenu(), SIGNAL(activated(int)), this, SLOT(menuActivated(int)));
      connect(tray, SIGNAL(toggled()), this, SLOT(toggled()));

      tray->show();
    }

  delete config;

  KGlobal::config()->reparseConfiguration(); // kcontrol modified kdeglobals
  keys->readSettings();
}


void KXKBApp::toggled()
{
  uint index = _list.findIndex(_layout) + 1;
  if (index >= _list.count())
    index = 0;
  _layout = _list[index];
  _encoding = _encList[index];
  if(_encoding.isEmpty() )
      _encoding = "locale";
  const unsigned int * pGroup = rules->group()[_layout];
  _group = pGroup ? *pGroup : 0;
  extension->setLayout(_rule, _model, _layout, _encoding, _group);
  tray->setLayout(_layout);
}


void KXKBApp::menuActivated(int id)
{
  if (0 <= id && id < (int)_list.count())
    {
      _layout = _list[id];
      _encoding = _encList[id];
      if(_encoding.isEmpty() )
          _encoding = "locale";
      tray->setLayout(_layout);
      const unsigned int * pGroup = rules->group()[_layout];
      _group = pGroup ? *pGroup : 0;
      extension->setLayout(_rule, _model, _layout, _encoding, _group);
    }
  else if (id == (int)_list.count())
    {
      KProcess p;
      p << "kcmshell" << "keyboard";
      p.start(KProcess::DontCare);
    }
  else
    quit();
}


const char * DESCRIPTION =
  I18N_NOOP("A utility to switch keyboard maps.");


int main(int argc, char *argv[])
{
  KAboutData about("kxkb", I18N_NOOP("KDE Keyboard Tool"), "0.1",
                   DESCRIPTION, KAboutData::License_QPL,
                   "(C) 2000, Matthias Hölzer-Klüpfel");
  KCmdLineArgs::init(argc, argv, &about);
  KXKBApp::addCmdLineOptions();

  if (!KXKBApp::start())
    return 0;

  KXKBApp app;
  app.exec();
}



