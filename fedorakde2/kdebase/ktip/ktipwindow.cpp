/*****************************************************************

Copyright (c) 2000, Matthias Hoelzer-Kluepfel

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


#include <qapplication.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <ktextbrowser.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qhbox.h>

#include <kapp.h>
#include <kuniqueapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kseparator.h>
#include <kstddirs.h>
#include <kwin.h>
#include <kdesktopwidget.h>


#include "ktipwindow.moc"


TipWindow::TipWindow()
  : QDialog(0,0,false)
{
    KWin::setState( winId(), NET::StaysOnTop );
    setCaption(i18n("Kandalf's useful tips"));

    QVBoxLayout *vbox = new QVBoxLayout(this, 4);

    QHBox *hbox = new QHBox(this);
    hbox->setSpacing(0);
    hbox->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    vbox->addWidget(hbox);

    QHBox *tl = new QHBox(hbox);
    tl->setMargin(7);
    tl->setBackgroundColor(QColor(49, 121, 172));

    QHBox *topLeft = new QHBox(tl);
    topLeft->setMargin(15);
    QColor bgColor(213, 222, 238);
    topLeft->setBackgroundColor(bgColor);

    text = new KTextBrowser(topLeft);
    text->mimeSourceFactory()->addFilePath(KGlobal::dirs()->findResourceDir("data", "kdewizard/pics")+"kdewizard/pics/");
    text->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    text->setHScrollBarMode(QScrollView::AlwaysOff);
    QStringList icons = KGlobal::dirs()->resourceDirs("icon");
    QStringList::Iterator it;
    for (it = icons.begin(); it != icons.end(); ++it)
        text->mimeSourceFactory()->addFilePath(*it);
    QColorGroup colors;
    colors.setBrush(QColorGroup::Base, QBrush(bgColor));
    colors.setColor(QColorGroup::Highlight, QColor(49, 121, 172));
    colors.setColor(QColorGroup::HighlightedText, Qt::white);
    text->setPaperColorGroup(colors);

    QLabel *l = new QLabel(hbox);
    l->setPixmap(locate("data", "kdewizard/pics/wizard_small.png"));
    l->setBackgroundColor(QColor(49, 121, 172));
    l->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    KSeparator* sep = new KSeparator( KSeparator::HLine, this);
    vbox->addWidget(sep);

    QHBoxLayout *hbox2 = new QHBoxLayout(vbox, 4);

    startup = new QCheckBox(i18n("Run on startup"), this);
    hbox2->addWidget(startup, 1);

    prev = new QPushButton(i18n("&Previous"), this);
    hbox2->addWidget(prev);

    next = new QPushButton(i18n("&Next"), this);
    hbox2->addWidget(next);

    ok = new QPushButton(i18n("&Close"), this);
    ok->setDefault(true);
    hbox2->addWidget(ok);

    connect(next, SIGNAL(clicked()), this, SLOT(nextTip()));
    connect(prev, SIGNAL(clicked()), this, SLOT(prevTip()));
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

    connect(startup, SIGNAL(clicked()), this, SLOT(startupClicked()));

    resize(550, 230);
    QSize sh = sizeHint();
    QRect rect = KApplication::desktop()->screenGeometry();
    move(rect.x() + (rect.width() - sh.width())/2,
         rect.y() + (rect.height() - sh.height())/2);

    loadTips();

    KConfig *config = new KConfig("kdewizardrc", true);
    config->setGroup("General");
    startup->setChecked(config->readBoolEntry("TipsOnStart", true));
    delete config;

    if (tips.count()) {
        current = kapp->random() % tips.count();
    } else {
        prev->setEnabled(false);
        next->setEnabled(false);
    }
}


void TipWindow::done( int )
{
    kapp->quit();
}


void TipWindow::startupClicked()
{
    KConfig *config = new KConfig("kdewizardrc");
    config->setGroup("General");
    config->writeEntry("TipsOnStart", startup->isChecked());
    delete config;
}


// if you change something here, please update the script
// preparetips, which depends on extracting exactly the same
// text as done here.
void TipWindow::loadTips()
{
    QString fname;

    fname = locate("data", QString("kdewizard/tips"));

    if (fname.isEmpty())
        return;

    tips.clear();

    QFile f(fname);
    if (f.open(IO_ReadOnly))
    {
        QTextStream ts(&f);

        QString line, tag, tip;
        bool inTip = false;
        while (!ts.eof())
	{
            line = ts.readLine();
            tag = line.stripWhiteSpace().lower();

            if (tag == "<html>")
	    {
                inTip = true;
                tip = QString::null;
                continue;
	    }

            if (inTip)
	    {
                if (tag == "</html>")
		{
                    tips.append(tip);
                    inTip = false;
		}
                else
                    tip.append(line).append("\n");
	    }

	}

        f.close();
    }
}


void TipWindow::nextTip()
{
    if (tips.count()==0)
        return;
    current += 1;
    if (current >= (int) tips.count())
        current = 0;
    text->setText(QString("<html>%1</html>").arg(i18n(tips[current].latin1())));
}


void TipWindow::prevTip()
{
    if (tips.count()==0)
        return;
    current -= 1;
    if (current < 0)
        current = tips.count()-1;
    text->setText(QString("<html>%1</html>").arg(i18n(tips[current].latin1())));
}


TipApp::TipApp() : KUniqueApplication()
{
    window = new TipWindow;
    window->show();

    setMainWidget(window);

    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));
}


TipApp::~TipApp()
{
    delete window;
}


static const char *description = I18N_NOOP("Kandalf's tips");

int main(int argc, char *argv[])
{
    KAboutData aboutData("ktip", I18N_NOOP("KTip"),
                         "0.2", description, KAboutData::License_GPL,
                         "(c) 1998-2000, KDE Developers");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start())
        exit(-1);

    TipApp app;

    return app.exec();
}
