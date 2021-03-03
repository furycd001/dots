#include <kapp.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <kconfig.h>

int main(int argc, char **argv)
{
  KApplication a(argc, argv, "whatever", false);
  KStandardDirs t;
  KConfig config; // to add custom entries - a bit tricky :/

  t.saveLocation("icon");

  QString s = t.findResource("icon", "xv.xpm");
  if (!s.isNull()) kdDebug() << s << endl;

  QStringList list = t.findAllResources("data", "kfind/toolbar", true);
  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
    kdDebug() << "data " << (*it).ascii() << endl;
  }

  list = t.findAllResources("config", "kcmdisplayrc");
  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
    kdDebug() << "config " << (*it).ascii() << endl;
  }

  list = t.findAllResources("config", "kcmdisplayrc", false, true);
  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
    kdDebug() << "config2 " << (*it).ascii() << endl;
  }


  list = t.findAllResources("html", "en/*/index.html", true);
  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
    kdDebug() << "docs " << (*it).ascii() << endl;
  }

  list = t.findDirs("data", "kwin");
  for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
    kdDebug() << "kwin dirs " << (*it).ascii() << endl;
  }

  kdDebug() << "hit " << t.findResourceDir("config", "kcmdisplayrc") << endl;
}
