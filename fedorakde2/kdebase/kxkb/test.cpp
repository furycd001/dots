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

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kapp.h>
#include <kcmdlineargs.h>

#include "rules.h"
#include "pixmap.h"
#include <kinstance.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdio.h>
#include <errno.h>
#include <ksimpleconfig.h>

#include <X11/Xlib.h>
#define explicit int_explicit        // avoid compiler name clash in XKBlib.h
#include <X11/XKBlib.h>

#if 0

extern FILE *xkbparserin;
extern int xkbparserlex();

class KeyMapConfig : public KSimpleConfig
{
public:
    KeyMapConfig(const QString &filename) : KSimpleConfig(filename) {}
    KEntryMap keymap() const { return internalEntryMap("KeyboardMap"); }
};

int main(int argc, char **argv)
{
    KLocale::setMainCatalogue("kcmlayout");
    KAboutData d("converter", I18N_NOOP("sss"), "1.0",
                 I18N_NOOP("sss"),
                 KAboutData::License_GPL, "(c) 2000 Stephan Kulow");
    d.addAuthor("Stephan Kulow", I18N_NOOP("Author"), "coolo@kde.org");

    KCmdLineArgs::init(argc, argv, &d);
    // KCmdLineArgs::addCmdLineOptions(options);
//    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication a("ss");

    KGlobal::dirs()->addResourceType("xkb", KStandardDirs::kde_default("data") + "kxkb/");
    KGlobal::dirs()->addResourceDir("xkb", "/usr/X11R6/lib/X11/xkb");

    QStringList list = KGlobal::dirs()->findAllResources("xkb", "keymap/*", true);
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
        kdDebug() << *it << endl;
        /* Open the file for lex */
        xkbparserin = fopen(QFile::encodeName(*it), "r");

        if (xkbparserin == NULL)
        {
            fprintf(stderr, "cannot open %s: %s\n", QFile::encodeName(*it).data() , strerror(errno));
            continue;
        }
        /* Call the parser on the file */
        xkbparserlex();
        fclose(xkbparserin);
    }
    KGlobal::dirs()->addResourceType("kmaps", "share/apps/kikbd");
    list = KGlobal::dirs()->findAllResources("kmaps", "*.kimap", true);
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
    {
        kdDebug() << *it << endl;
        KeyMapConfig cfg(*it);
        KEntryMap m = cfg.keymap();
        for (KEntryMapConstIterator it2 = m.begin(); it2 != m.end(); ++it2)
        {
            if (it2.key().mKey.left(6) != "keysym")
                continue;
            QStringList keys = QStringList::split(',', QString((*it2).mValue));
            if (keys.count() > 5) {
                kdDebug() << "more than 4 items: " << keys.count() << endl;
            }
            for (QStringList::ConstIterator it3 = keys.begin(); it3 != keys.end(); ++it3) {
                KeySym s;
                if ((*it3).left(2) == "0x") {
                    unsigned int hex;
                    if (sscanf((*it3).latin1(), "%x", &s) != 1)
                        s = NoSymbol;
                    fprintf(stderr, "%x - %s\n", s, XKeysymToString(s));
                } else
                    s = XStringToKeysym((*it3).latin1());
                if (s == NoSymbol)
                    kdDebug() << "illegal " << *it3 << endl;
            }

        }
    }

    return 1;
}

void mapinfo_cb(const char *map_name)
{
//    kdDebug() << map_name << endl;
}

void filename_cb(const char *)
{
}

#endif

