/*****************************************************************

Copyright (c) 2001 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <qdir.h>
#include <qfileinfo.h>

#include <kapp.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>

#include "konsole_mnu.moc"

PanelKonsoleMenu::PanelKonsoleMenu(QWidget *parent, const char *name)
    : PanelMenu(locate("data", "konsole/"), parent, name) {}

void PanelKonsoleMenu::initialize()
{
    if (init) clear();
    init = true;

    QStringList list = KGlobal::dirs()->findAllResources("data", "konsole/*.desktop", false, true);

    int id = 0;

    sessionList.clear();
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
    {
        KSimpleConfig conf(*it, true /* read only */);
        conf.setDesktopGroup();
        QString text = conf.readEntry("Comment");
        if (text.isEmpty() || conf.readEntry("Type") != "KonsoleApplication")
            continue;

        insertItem(SmallIconSet(conf.readEntry("Icon", "konsole")), text, id++);
        QFileInfo fi(*it);
        sessionList.append(fi.baseName());
    }

    screenList.clear();
    QCString screenDir = getenv("SCREENDIR");
    if (screenDir.isEmpty())
        screenDir = QFile::encodeName(QDir::homeDirPath()) + "/.screen/";
    QStringList sessions;
    // Can't use QDir as it doesn't support FIFOs :(
    DIR *dir = opendir(screenDir);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)))
        {
            QCString path = screenDir + "/" + entry->d_name;
            struct stat st;
            if (stat(path, &st) != 0)
                continue;
            int fd;
            if (S_ISFIFO(st.st_mode) && !(st.st_mode & 0111) && // xbit == attached
                (fd = open(path, O_WRONLY | O_NONBLOCK)) != -1)
            {
                ::close(fd);
                screenList.append(QFile::decodeName(entry->d_name));
                insertItem(SmallIconSet("konsole"),
                    i18n("Screen is a program controlling screens!", "Screen at %1").arg(entry->d_name), id++);
            }
        }
        closedir(dir);
    }
}

void PanelKonsoleMenu::slotExec(int id)
{
    if (id >= 0)
    {
        kapp->propagateSessionManager();
        QStringList args;
        if (static_cast<unsigned int>(id) < sessionList.count())
        {
            args << "--type";
            args << sessionList[id];
        }
        else
        {
            args << "-e";
            args << "screen";
            args << "-r";
            args << screenList[id - sessionList.count()];
        }
        KApplication::kdeinitExec("konsole2", args);
    }
}

