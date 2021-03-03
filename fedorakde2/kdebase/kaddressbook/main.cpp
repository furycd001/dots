#include <stdlib.h>

#include <qstring.h>

#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kuniqueapp.h>
#include <kwin.h>

#include "kaddressbook.h"

class KAddressBookApp : public KUniqueApplication {
  public:
    KAddressBookApp() : mMainWin(0) {}

    int newInstance();

  private:
    KAddressBook *mMainWin;
};

int KAddressBookApp::newInstance()
{
  if (isRestored()) {
    // There can only be one main window
    if (KMainWindow::canBeRestored(1)) {
      mMainWin = new KAddressBook;
      mMainWin->restore(1);
    }
  } else {
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QCString addrStr = args->getOption("addr");

    QString addr;
    if (!addrStr.isEmpty()) addr = QString::fromLocal8Bit(addrStr);
    
    args->clear();

    if (mMainWin) {
      kdDebug() << "AbBrowser already running." << endl;
      KWin::setActiveWindow(mMainWin->winId());
    } else {
      mMainWin = new KAddressBook;
      mMainWin->show();
    }
    
    if (!addr.isEmpty()) mMainWin->addEmail(addr);
  }

  return 0;
}

// the dummy argument is required, because KMail apparently sends an empty
// argument.
static KCmdLineOptions kmoptions[] =
{
  { "a", 0 , 0 },
  { "addr <email>", I18N_NOOP("Update entry with given email address"), 0 },
  { "+[argument]", I18N_NOOP("dummy argument"), 0},
  { 0, 0, 0}
};

int main(int argc, char *argv[])
{
  KLocale::setMainCatalogue("kaddressbook");
  KAboutData about("kaddressbook", I18N_NOOP("KAddressBook"),
                   "1.0",
                   "The KDE Address Book",
		   KAboutData::License_BSD,
                   "(c) 1997-2001, The KDE PIM Team");

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions( kmoptions );
  KUniqueApplication::addCmdLineOptions();
  
  if (!KAddressBookApp::start()) exit(0);

  KAddressBookApp app;

  return app.exec();
}

