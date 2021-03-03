#include "legacyimport.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kseparator.h>
#include <kfiledialog.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kmessagebox.h>

KLegacyImport::KLegacyImport(QWidget *parent, const char *name)
    : QWizard(parent, name)
{
    firstPage = new QWidget(this);
    QGridLayout *layout = new QGridLayout(firstPage);
    QLabel *helpLbl = new QLabel(i18n("<QT><b>Legacy Theme Importer</b><P>\
KDE now supports utilizing older themes from other platforms in your new KDE \
desktop. The most common type of legacy theme is for the \
GTK toolkit, which is imported to KDE's Control Center with this utility.<P>\
In order to do this you must specify at least one item: the folder \
where the pixmaps for the theme is located. You may also have to specify the \
theme's gtkrc file if it's not in the same folder as the pixmaps. Both are usually \
in the folder \"gtk\" inside the theme's directory.\
</QT>"), firstPage);

    QLabel *descLbl = new QLabel(i18n("Step 1: Specify theme's pixmap folder:"),
                                 firstPage);

    browsePixBtn = new QPushButton(i18n("Browse"), firstPage);
    connect(browsePixBtn, SIGNAL(clicked()), this, SLOT(slotBrowsePixClicked()));
    pixDirEdit = new QLineEdit(firstPage);
    connect(pixDirEdit, SIGNAL(returnPressed()), this, SLOT(slotPixEdit()));

    layout->addMultiCellWidget(helpLbl, 0, 0, 0, 1);
    layout->addMultiCellWidget(new KSeparator(firstPage), 2, 2, 0, 1);
    layout->addMultiCellWidget(descLbl, 3, 3, 0, 1);
    layout->addWidget(pixDirEdit, 4, 0);
    layout->addWidget(browsePixBtn, 4, 1);
    layout->addRowSpacing(1, 32);
    layout->setRowStretch(1, 1);
    layout->setColStretch(0, 1);
    firstStep = true;

    addPage(firstPage, i18n("Specify pixmap directory"));

    secondPage = new QWidget(this);
    QGridLayout *nextlayout = new QGridLayout(secondPage);
    QLabel *gtkHelpLbl = new QLabel(i18n("<QT><b>Legacy Theme Importer</b><P>\
A gtkrc file was not found in the specified directory. If it exists somewhere \
other than where the pixmaps are located enter it here.</QT>"), secondPage);

    QLabel *gtkDescLbl = new QLabel(i18n("Step 2: Specify theme's gtkrc:"),
                                    secondPage);

    browseGtkrcBtn = new QPushButton(i18n("Browse"), secondPage);
    connect(browseGtkrcBtn, SIGNAL(clicked()), this, SLOT(slotBrowseGtkrcClicked()));
    gtkrcDirEdit = new QLineEdit(secondPage);
    connect(gtkrcDirEdit, SIGNAL(returnPressed()), this, SLOT(slotGtkrcEdit()));

    nextlayout->addMultiCellWidget(gtkHelpLbl, 0, 0, 0, 1);
    nextlayout->addMultiCellWidget(new KSeparator(secondPage), 2, 2, 0, 1);
    nextlayout->addMultiCellWidget(gtkDescLbl, 3, 3, 0, 1);
    nextlayout->addWidget(gtkrcDirEdit, 4, 0);
    nextlayout->addWidget(browseGtkrcBtn, 4, 1);
    nextlayout->addRowSpacing(1, 32);
    nextlayout->setRowStretch(1, 1);
    nextlayout->setColStretch(0, 1);

    addPage(secondPage, i18n("Specify gtkrc directory"));

    setNextEnabled(firstPage, false);


    KGlobal::dirs()->addResourceType("themes",
                                     KStandardDirs::kde_default("data") +
                                     "kstyle2/themes");

    setCaption(i18n("Legacy Import"));

}

void KLegacyImport::slotBrowsePixClicked()
{
    QString pixmapStr = KFileDialog::getExistingDirectory(QString::null, this,
                                                          i18n("GTK theme's folder"));
    if(!pixmapStr.isEmpty()){
        pixDirEdit->setText(pixmapStr);
        if(QFile::exists(pixmapStr + "/gtkrc")){
            gtkrcDirEdit->setText(pixmapStr + "/gtkrc");
            finished();
        }
        else
            setNextEnabled(firstPage, true);
    }
}

void KLegacyImport::slotPixEdit()
{
    QFileInfo fi(pixDirEdit->text());
    if(fi.exists() && fi.isDir()){
        if(QFile::exists(pixDirEdit->text() + "/gtkrc")){
            gtkrcDirEdit->setText(pixDirEdit->text() + "/gtkrc");
            finished();
        }
        else
            setNextEnabled(firstPage, true);
    }
    else
        KMessageBox::error(NULL, i18n("You did not enter a valid file!"));

}

void KLegacyImport::slotBrowseGtkrcClicked()
{
    QString gtkrcStr = KFileDialog::getOpenFileName(QString::null, "gtkrc",
                                                    this,
                                                    i18n("Gtkrc theme file"));
    if(!gtkrcStr.isEmpty())
        finished();
}


void KLegacyImport::slotGtkrcEdit()
{
    QFileInfo fi(gtkrcDirEdit->text());
    if(fi.exists() && fi.isFile()){
        finished();
    }
    else
        KMessageBox::error(NULL, i18n("You did not enter a valid file!"));

}

void KLegacyImport::finished()
{

    QString themeName = QInputDialog::getText(i18n("Theme title"),
                                              i18n("Please enter a short title for this theme"));
    if(themeName.isEmpty())
        kapp->exit();
    QString path = locateLocal("themes", "");
    qDebug(QString(path + themeName + ".themerc").latin1());
    KSimpleConfig config(path + themeName + ".themerc");
    config.setGroup("Misc");
    config.writeEntry("Name", themeName);
    config.writeEntry("Comment", i18n("Legacy widget theme"));
    config.writeEntry("PixmapPath", pixDirEdit->text());
    config.writeEntry("RCPath", gtkrcDirEdit->text());
    config.setGroup("KDE");
    config.writeEntry("widgetStyle", "klegacystyle.la");
    config.sync();
    KMessageBox::information(NULL, i18n("The theme is now installed and will be shown \
in KDE's Control Center."));
    kapp->quit();
}

int main(int argc, char **argv)
{
    KAboutData aboutData("klegacyimport", I18N_NOOP("KLegacyImport"),
                         "v0.1", I18N_NOOP("Importer for legacy themes"),
                         KAboutData::License_BSD,
                         "(c)2000 KDE project");
    aboutData.addAuthor("KDE project", 0, "");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app(argc, argv);
    KLegacyImport *tl = new KLegacyImport;
    app.setMainWidget(tl);
    //tl->resize(tl->sizeHint());
    tl->show();
    return(app.exec());
}

#include "legacyimport.moc"

