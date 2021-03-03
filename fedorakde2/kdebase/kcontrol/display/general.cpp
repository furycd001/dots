/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kcmdisplay.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * Copyright (C) 1997 Mark Donohoe
 *
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#include <stdlib.h>

#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlistview.h>
#include <qframe.h>
#include <qsizepolicy.h>
#include <qwhatsthis.h>
#include <qtextstream.h>
#include <qframe.h>

#include <dcopclient.h>

#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kthemebase.h>
#include <kwin.h>
#include <kdialog.h>
#include <kipc.h>
#include <kprocess.h>
#include <kcmodule.h>
#include <kiconloader.h>
#include <kglobalsettings.h>
#include <kdirwatch.h>
#include <kmessagebox.h>

#include "general.h"

#include <kdebug.h>

#include <X11/Xlib.h>

/**** DLL Interface for kcontrol ****/

extern void runRdb();

void applyGtkStyles(bool active)
{
   QString gtkkde = QDir::homeDirPath()+"/.gtkrc-kde";
   QCString gtkrc = getenv("GTK_RC_FILES");
   QStringList list = QStringList::split(':', QFile::decodeName(gtkrc));
   if (list.count() == 0)
   {
      list.append(QString::fromLatin1("/etc/gtk/gtkrc"));
      list.append(QDir::homeDirPath()+"/.gtkrc");
   }
   list.remove(gtkkde);
   if (active)
      list.append(gtkkde);

   // Pass env. var to kdeinit.
   QCString name = "GTK_RC_FILES";
   QCString value = QFile::encodeName(list.join(":"));
   QByteArray params;
   QDataStream stream(params, IO_WriteOnly);
   stream << name << value;
   kapp->dcopClient()->send("klauncher", "klauncher", "setLaunchEnv(QCString,QCString)", params);
}

void applyMultiHead(bool active)
{
   // Pass env. var to kdeinit.
   QCString name = "KDE_MULTIHEAD";
   QCString value = active ? "true" : "false";
   QByteArray params;
   QDataStream stream(params, IO_WriteOnly);
   stream << name << value;
   kapp->dcopClient()->send("klauncher", "klauncher", "setLaunchEnv(QCString,QCString)", params);
}

extern "C" {
    KCModule *create_style(QWidget *parent, const char *name) {
      KGlobal::locale()->insertCatalogue(QString::fromLatin1("kcmstyle"));
      return new KGeneral(parent, name);
    }

    void init_style() {
        KConfig config("kcmdisplayrc", true, true);
        config.setGroup("X11");
        if (config.readBoolEntry( "useResourceManager", true ))
        {
	  runRdb();
          applyGtkStyles(true);
        }
        else
        {
          applyGtkStyles(false);
        }

        if (!config.readBoolEntry( "disableMultihead", false ) &&
           (ScreenCount(qt_xdisplay()) > 1))
        {
          applyMultiHead(true);
        }
        else
        {
          applyMultiHead(false);
        }

        config.setGroup("KDE");
        // Enable/disable Qt anti-aliasing

        // Write some Qt root property.
#ifndef __osf__      // this crashes under Tru64 randomly -- will fix later
        QByteArray properties;
        QDataStream d(properties, IO_WriteOnly);
        d << kapp->palette() << KGlobalSettings::generalFont();
        Atom a = XInternAtom(qt_xdisplay(), "_QT_DESKTOP_PROPERTIES", false);

	// do it for all root windows - multihead support
	int screen_count = ScreenCount(qt_xdisplay());
	for (int i = 0; i < screen_count; i++)
	    XChangeProperty(qt_xdisplay(),  RootWindow(qt_xdisplay(), i),
			    a, a, 8, PropModeReplace,
			    (unsigned char*) properties.data(), properties.size());
#endif
    }
}


/**** KThemeListBox ****/


// mosfet's style stuff 04/26/99 (mosfet)
KThemeListBox::KThemeListBox(QWidget *parent, const char *name)
    : QListView(parent, name)
{
    KConfig kconfig("kstylerc");
    kconfig.setGroup("KDE");
    defName = QString::fromLatin1("KDE default");
    curTheme = kconfig.readEntry("currentTheme");

    addColumn(i18n("Name"));
    addColumn(i18n("Description"));
    setAllColumnsShowFocus(true);
    KGlobal::dirs()->addResourceType("themes", KStandardDirs::kde_default("data") + "kstyle2/themes");

    if (curTheme.isEmpty())
       curTheme = locate("themes", "default.themerc");

    connect(KDirWatch::self(), SIGNAL(dirty(const QString&)),
            this, SLOT(rescan()));
    localThemeDir = locateLocal("themes","");
    KDirWatch::self()->addDir(localThemeDir);
    KDirWatch::self()->startScan();

    rescan();
}


KThemeListBox::~KThemeListBox()
{
    KDirWatch::self()->removeDir(localThemeDir);
}


void KThemeListBox::readTheme(const QString &file)
{
    KSimpleConfig config(file, true);
    if (!config.hasGroup("KDE")) return;

    config.setGroup("Misc");
    QString name = config.readEntry("Name");
    if (name.isEmpty())
    {
       name = file;
       int i = name.findRev('/');
       if (i != -1)
          name = name.mid(i+1);
       i = name.find('.');
       if (i > 0)
          name.truncate(i);
    }
    QString desc = config.readEntry("Comment", i18n("No description available."));
    config.setGroup( "KDE" );

    QString style = config.readEntry( "widgetStyle" );
    QListViewItem *item = new QListViewItem(this, name, desc, file);
    if (file == curTheme) {
        curItem = item;
        setSelected(item, true);
        ensureItemVisible(item);
    }
    if (name == defName)
        defItem = item;
}


void KThemeListBox::defaults()
{
    setSelected(defItem, true);
    ensureItemVisible( defItem );
}


void KThemeListBox::load()
{
    if (0 != curItem) {
        setSelected(curItem, true);
        ensureItemVisible( curItem );
    }
}

void KThemeListBox::rescan()
{
    clear();
    curItem = 0;
    defItem = 0;

    QStringList list = KGlobal::dirs()->findAllResources("themes", "*.themerc", true, true);
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++)
        readTheme(*it);

    if (!currentItem() )
        setSelected(firstChild(), true);

    ensureItemVisible(currentItem());
}


void KThemeListBox::save()
{
    if (currentItem()->text(2) == curTheme)
        return;
    // check for legacy theme
    KSimpleConfig configTest(currentItem()->text(2));
    configTest.setGroup("Misc");
    if(configTest.hasKey("RCPath")){
        kdDebug () << "Legacy theme" << endl;
        QFile input(configTest.readEntry("RCPath"));
        if(!input.open(IO_ReadOnly)){
            kdDebug() << "Could not open input file!" << endl;
            return;
        }
        QFile output(QDir::home().absPath() + "/.gtkrc");
        if(!output.open(IO_WriteOnly)){
            kdDebug () << "Could not open output file!" << endl;
            return;
        }
        QTextStream outStream(&output);
        QTextStream inStream(&input);
        outStream << "pixmap_path \"" << configTest.readEntry("PixmapPath") << "\""<< endl;
        while(!inStream.atEnd())
            outStream << inStream.readLine() << endl;
        input.close();
        output.close();
    }

    curItem = currentItem();
    curTheme = curItem->text(2);
    KThemeBase::applyConfigFile(currentItem()->text(2));

// This section can be removed when kdelibs 2.1 is released.
    KConfig kconfig("kstylerc");
    kconfig.setGroup("KDE");
    kconfig.writeEntry("currentTheme", curTheme);
    kconfig.sync();
// End section
}


/**** KGeneral ****/

KGeneral::KGeneral(QWidget *parent, const char *name)
    : KCModule(parent, name)
{
    m_bChanged = false;
    m_bStyleDirty = false;
    m_bToolbarsDirty = false;
    m_bEffectsDirty = false;
    m_bMacStyleDirty = false;
    useRM = true;
    macStyle = false;

    config = new KConfig("kcmdisplayrc");
    QBoxLayout *topLayout = new QVBoxLayout( this, KDialog::marginHint(),
                         KDialog::spacingHint() );

    // my little style list (mosfet 04/26/99)
    QBoxLayout *lay = new QHBoxLayout;
    topLayout->addLayout(lay);

    QGroupBox *themeBox = new QGroupBox(1, Horizontal,
                    i18n("Widget style and theme"),
                    this);
    lay->addWidget(themeBox);

    themeList = new KThemeListBox(themeBox);
    connect(themeList, SIGNAL(currentChanged(QListViewItem*)),
            SLOT(slotChangeStylePlugin(QListViewItem*)));
    QWhatsThis::add( themeBox, i18n("Here you can choose from a list of"
      " predefined widget styles (e.g. the way buttons are drawn) which"
      " may or may not be combined with a theme (additional information"
      " like a marble texture or a gradient).") );

    QWidget *dummy = new QWidget(themeBox);
    QHBoxLayout *btnLay = new QHBoxLayout(dummy);

    QPushButton *btnImport = new QPushButton(i18n("&Import GTK themes..."), dummy);
    btnImport->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
    QWhatsThis::add(btnImport, i18n("This launches KDE GTK style importer "
        "which allows you to convert legacy GTK pixmap themes to KDE widget styles."));

    QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );

    btnLay->addWidget(btnImport);
    btnLay->addItem(spacer);

    connect(btnImport, SIGNAL(clicked()), SLOT(slotRunImporter()));

    // Drawing settings
    styles = new QGroupBox ( i18n( "Other settings for drawing" ), this );
    topLayout->addWidget(styles, 10);
    QBoxLayout *vlay = new QVBoxLayout (styles, KDialog::marginHint(),
                    KDialog::spacingHint());
    vlay->addSpacing(styles->fontMetrics().lineSpacing());

    cbMac = new QCheckBox( i18n( "&Menubar on top of the screen in "
                                 "the style of MacOS" ), styles);
    QWhatsThis::add( cbMac, i18n("If this option is selected, applications"
      " won't have their menubar attached to their own window anymore."
      " Instead, there is one menu bar at the top of the screen which shows"
      " the menu of the currently active application. Maybe you know this"
      " behavior from MacOS.") );

    connect( cbMac, SIGNAL( clicked() ), SLOT( slotMacStyle()  )  );
    vlay->addWidget( cbMac, 10 );

    cbRes = new QCheckBox( i18n( "&Apply fonts and colors to non-KDE apps" ), styles);
    connect( cbRes, SIGNAL( clicked() ), SLOT( slotUseResourceManager()  )  );
    vlay->addWidget( cbRes, 10 );

    QWhatsThis::add( cbRes, i18n("If this option is selected, KDE will try"
      " to force your preferences regarding colors and fonts even on non-KDE"
      " applications. While this works fine with most applications, it <em>may</em>"
      " give strange results sometimes.") );

    tbStyle = new QButtonGroup( i18n( "Style options for toolbars" ), this);

    topLayout->addWidget(tbStyle, 10);

    vlay = new QVBoxLayout( tbStyle, 10 );
    vlay->addSpacing( tbStyle->fontMetrics().lineSpacing() );

    QGridLayout *grid = new QGridLayout(4, 2);
    vlay->addLayout( grid );

    tbIcon   = new QRadioButton( i18n( "&Icons only" ), tbStyle);
    tbText   = new QRadioButton( i18n( "&Text only" ), tbStyle);
    tbAside  = new QRadioButton( i18n( "Text a&side icons" ), tbStyle);
    tbUnder  = new QRadioButton( i18n( "Text &under icons" ), tbStyle);

    QWhatsThis::add( tbIcon, i18n("Shows only icons on toolbar buttons. Best option for low resolutions.") );
    QWhatsThis::add( tbText, i18n("Shows only text on toolbar buttons.") );
    QWhatsThis::add( tbAside, i18n("Shows icons and text on toolbar buttons. Text is aligned aside the icon.") );
    QWhatsThis::add( tbUnder, i18n("Shows icons and text on toolbar buttons. Text is aligned below the icon.") );

    tbHilite = new QCheckBox( i18n( "&Highlight buttons under mouse" ), tbStyle);
    tbTransp = new QCheckBox( i18n( "Tool&bars are transparent when"
                " moving" ), tbStyle);

    QWhatsThis::add( tbHilite, i18n("If this option is selected, toolbar buttons"
      " will change their color when the mouse cursor is moved over them.") );
    QWhatsThis::add( tbTransp, i18n("If this option is selected, only the skeleton of"
      " a toolbar will be shown when moving that toolbar.") );

    connect( tbIcon , SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );
    connect( tbText , SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );
    connect( tbAside, SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );
    connect( tbUnder, SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );
    connect( tbHilite, SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );
    connect( tbTransp, SIGNAL( clicked() ), SLOT( slotChangeTbStyle()  )  );

    grid->addWidget(tbIcon, 0, 0);
    grid->addWidget(tbText, 1, 0);
    grid->addWidget(tbAside, 2, 0);
    grid->addWidget(tbUnder, 3, 0);

    grid->addWidget(tbHilite, 0, 1);
    grid->addWidget(tbTransp, 1, 1);

    QBoxLayout *hlay = new QHBoxLayout;
    topLayout->addLayout(hlay);

    effectStyleMenu = new QButtonGroup( i18n( "Effect options" ), this);
    hlay->addWidget(effectStyleMenu, 10);
//	effectStyle->setExclusive(false);
	
    vlay = new QVBoxLayout( effectStyleMenu, 10 );
    vlay->addSpacing( effectStyleMenu->fontMetrics().lineSpacing() );

//    grid = new QGridLayout(5, 2);
//    vlay->addLayout( grid );

    effPlainMenu = new QRadioButton( i18n( "No m&enu effect" ), effectStyleMenu );
    effFadeMenu = new QRadioButton( i18n( "&Fade menus" ), effectStyleMenu );
    effAnimateMenu = new QRadioButton( i18n( "Anima&te menus"), effectStyleMenu );
    QFrame* Line1 = new QFrame( effectStyleMenu, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    effAnimateCombo = new QCheckBox( i18n( "Animate &combo boxes"), effectStyleMenu );

    QWhatsThis::add( effFadeMenu, i18n( "If this option is selected, menus open immediately." ) );
    QWhatsThis::add( effFadeMenu, i18n( "If this option is selected, menus fade in slowly." ) );
    QWhatsThis::add( effAnimateMenu, i18n( "If this option is selected, menus are animated to grow to their full size." ) );
    QWhatsThis::add( effAnimateCombo, i18n( "If this option is selected, combo boxes are animated to grow to their full size.") );

    connect( effPlainMenu, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) ) ;
    connect( effFadeMenu, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) ) ;
    connect( effAnimateMenu, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );



    connect( effAnimateCombo, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );
    vlay->addWidget( effPlainMenu );
    vlay->addWidget( effFadeMenu );
    vlay->addWidget( effAnimateMenu );
    vlay->addWidget( Line1);
    vlay->addWidget( effAnimateCombo );


    effectStyleTooltip = new QButtonGroup( i18n( "Tooltips" ), this);
    hlay->addWidget(effectStyleTooltip, 10);
   QVBoxLayout*  vlay1 = new QVBoxLayout( effectStyleTooltip, 10 );
    vlay1->addSpacing( effectStyleTooltip->fontMetrics().lineSpacing() );

    effNoTooltip = new QCheckBox( effectStyleTooltip);
    effNoTooltip->setChecked(effectNoTooltip);
    effNoTooltip->setText( i18n( "&Disable tool tips"));
    effPlainTooltip = new QRadioButton( i18n( "&Plain tool tips"), effectStyleTooltip );
    effFadeTooltip = new QRadioButton( i18n( "Fade tool t&ips"), effectStyleTooltip );
    effAnimateTooltip = new QRadioButton( i18n( "&Rollout tool t&ips"), effectStyleTooltip );
    QWhatsThis::add( effNoTooltip, i18n( "If this option is selected, tooltips are disabled in all applications." ) );
    QWhatsThis::add( effPlainTooltip, i18n( "If this option is selected, tooltips appear with no effect." ) );
    QWhatsThis::add( effFadeTooltip, i18n( "If this option is selected, tooltips fade in slowly." ) );
    QWhatsThis::add( effAnimateTooltip, i18n( "If this option is selected, tooltips roll out slowly." ) );
    connect( effNoTooltip, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );
    connect( effPlainTooltip, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );
    connect( effFadeTooltip, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );
    connect( effAnimateTooltip, SIGNAL( clicked() ), SLOT( slotChangeEffectStyle() ) );
    vlay1->addWidget( effNoTooltip );
    vlay1->addWidget( effPlainTooltip );
    vlay1->addWidget( effFadeTooltip );
    vlay1->addWidget( effAnimateTooltip );
	vlay1->addStretch();

    if(effectNoTooltip){
    	effFadeTooltip->setEnabled(false);
    	effPlainTooltip->setEnabled(false);
    	effAnimateTooltip->setEnabled(false);
    }

    topLayout->addStretch( 100 );
    load();
}


KGeneral::~KGeneral()
{
    delete config;
}


void KGeneral::slotChangeStylePlugin(QListViewItem *)
{
    m_bStyleDirty = true;
    m_bChanged = true;
    emit changed(true);
}


void KGeneral::slotChangeTbStyle()
{
    if (tbIcon->isChecked() )
        tbUseText = "IconOnly";
    else if (tbText->isChecked() )
        tbUseText = "TextOnly";
    else if (tbAside->isChecked() )
        tbUseText = "IconTextRight";
    else if (tbUnder->isChecked() )
        tbUseText = "IconTextBottom";
    else
        tbUseText = "IconOnly";

    tbUseHilite = tbHilite->isChecked();
    tbMoveTransparent = tbTransp->isChecked();

    m_bToolbarsDirty = true;
    m_bChanged = true;
    emit changed(true);
}


void KGeneral::slotChangeEffectStyle()
{
    effectFadeMenu = effFadeMenu->isChecked();
    effectAnimateMenu = effAnimateMenu->isChecked();

    effectFadeTooltip = effFadeTooltip->isChecked();
    effectAnimateTooltip = effAnimateTooltip->isChecked();
    effectNoTooltip = effNoTooltip->isChecked();
    effectAnimateCombo = effAnimateCombo->isChecked();

    if(effectNoTooltip){
    	effFadeTooltip->setEnabled(false);
    	effPlainTooltip->setEnabled(false);
    	effAnimateTooltip->setEnabled(false);
    }
    else{
    	effFadeTooltip->setEnabled(true);
    	effPlainTooltip->setEnabled(true);
    	effAnimateTooltip->setEnabled(true);
    }

    m_bEffectsDirty = true;
    m_bChanged = true;
    emit changed(true);
}

void KGeneral::slotUseResourceManager()
{
    useRM = cbRes->isChecked();

    m_bChanged = true;
    emit changed(true);
}

void KGeneral::slotMacStyle()
{
    macStyle = cbMac->isChecked();

    m_bMacStyleDirty = true;
    m_bChanged = true;
    emit changed(true);
}

void KGeneral::readSettings()
{
    config->setGroup("KDE");
    QString str = config->readEntry( "widgetStyle", "Platinum" );
    if ( str == "Platinum" )
    applicationStyle = WindowsStyle;
    else if ( str == "Windows 95" )
    applicationStyle = WindowsStyle;
    else
    applicationStyle = MotifStyle;
    macStyle = config->readBoolEntry( "macStyle", false);
    effectAnimateMenu = config->readBoolEntry( "EffectAnimateMenu", false );
    effectFadeMenu = config->readBoolEntry( "EffectFadeMenu", false );
    effectAnimateCombo = config->readBoolEntry( "EffectAnimateCombo", false );
    effectFadeTooltip = config->readBoolEntry( "EffectFadeTooltip", false );
    effectAnimateTooltip = config->readBoolEntry("EffectAnimateTooltip",false);
    effectNoTooltip = config->readBoolEntry("EffectNoTooltip",false);

    config->setGroup( "Toolbar style" );
    tbUseText = config->readEntry( "IconText", "IconOnly");
    int val = config->readNumEntry( "Highlighting", 1);
    tbUseHilite = val? true : false;
    tbMoveTransparent = config->readBoolEntry( "TransparentMoving", true);

    config->setGroup("X11");
    useRM = config->readBoolEntry( "useResourceManager", true );
}

void KGeneral::slotRunImporter()
{
    KProcess *themeImporter = new KProcess();
    themeImporter->setExecutable("klegacyimport");
    connect(themeImporter, SIGNAL(processExited(KProcess *)), themeList,SLOT(rescan()));
    themeImporter->start();
}

void KGeneral::showSettings()
{
    cbRes->setChecked(useRM);
    cbMac->setChecked(macStyle);

    tbHilite->setChecked(tbUseHilite);
    tbTransp->setChecked(tbMoveTransparent);


    tbIcon->setChecked( true ); // we need a default
    tbAside->setChecked( tbUseText == "IconTextRight" );
    tbText->setChecked( tbUseText == "TextOnly" );
    tbUnder->setChecked( tbUseText == "IconTextBottom" );

    effPlainMenu->setChecked(!effectAnimateMenu && !effectFadeMenu);
    effAnimateMenu->setChecked( effectAnimateMenu );
    effFadeMenu->setChecked( effectFadeMenu );
    effAnimateCombo->setChecked( effectAnimateCombo );
    effPlainTooltip->setChecked(!effectAnimateTooltip && !effectFadeTooltip);
    effFadeTooltip->setChecked( effectFadeTooltip );
    effAnimateTooltip->setChecked( effectAnimateTooltip );
    effNoTooltip->setChecked( effectNoTooltip );
    if(effectNoTooltip){
    	effFadeTooltip->setEnabled(false);
    	effPlainTooltip->setEnabled(false);
    	effAnimateTooltip->setEnabled(false);
    }
    else{
    	effFadeTooltip->setEnabled(true);
    	effPlainTooltip->setEnabled(true);
    	effAnimateTooltip->setEnabled(true);
    }
}


void KGeneral::defaults()
{
    useRM = true;
    macStyle = false;
    tbUseText = "IconOnly";
    tbUseHilite = true;
    tbMoveTransparent = true;

    effectAnimateMenu = false;
    effectFadeMenu = false;
    effectAnimateCombo = false;
    effectFadeTooltip = false;
	effectNoTooltip = false;
	
    themeList->defaults();
    showSettings();

    m_bChanged = true;
    emit changed(true);
}

QString KGeneral::quickHelp() const
{
    return i18n("<h1>Style</h1> In this module you can configure how"
      " your KDE applications will look.<p>"
      " Styles and themes affect the way buttons, menus etc. are drawn"
      " in KDE.  Think of styles as plugins that provide a general look"
      " for KDE and of themes as a way to fine-tune those styles. E.g."
      " you might use the \"System\" style but use a theme that provides"
      " a color gradient or a marble texture.<p>"
      " Apart from styles and themes you can configure here the behavior"
      " of menubars and toolbars.");
}


void KGeneral::load()
{
    readSettings();
    showSettings();

    themeList->load();

    m_bChanged = false;
    emit changed(false);
}


void KGeneral::save()
{
    themeList->save();

    if (!m_bChanged)
    return;

    config->setGroup("KDE");
    config->writeEntry("macStyle", macStyle, true, true);
    config->writeEntry("EffectAnimateMenu", effectAnimateMenu, true, true);
    config->writeEntry("EffectFadeMenu", effectFadeMenu, true, true);
    config->writeEntry("EffectAnimateCombo", effectAnimateCombo, true, true);
    config->writeEntry("EffectFadeTooltip", effectFadeTooltip, true, true);
	config->writeEntry("EffectAnimateTooltip", effectAnimateTooltip, true, true);
	config->writeEntry("EffectNoTooltip", effectNoTooltip, true, true);
	
    config->setGroup("Toolbar style");
    config->writeEntry("IconText", tbUseText, true, true);
    config->writeEntry("Highlighting", (int) tbUseHilite, true, true);
    config->writeEntry("TransparentMoving", (int) tbMoveTransparent,
        true, true);

    config->setGroup("X11");
    config->writeEntry("useResourceManager", useRM);
    config->sync();

    if (useRM) {
      QApplication::setOverrideCursor( waitCursor );
      runRdb();
      QApplication::restoreOverrideCursor();
    }
    applyGtkStyles(useRM);

    if ( m_bStyleDirty )
        KIPC::sendMessageAll(KIPC::StyleChanged);
    else if ( m_bMacStyleDirty )
        kapp->dcopClient()->send("kdesktop", "KDesktopIface", "configure()", QByteArray());
    if ( m_bToolbarsDirty )
        KIPC::sendMessageAll(KIPC::ToolbarStyleChanged, 0 /* we could use later for finer granularity */);
    if ( m_bEffectsDirty ){
        KIPC::sendMessageAll(KIPC::SettingsChanged);
		kapp->dcopClient()->send("kwin*", "", "reconfigure()", "");
	}
    QApplication::syncX();

    m_bChanged = false;
    m_bStyleDirty = false;
    m_bToolbarsDirty = false;
    m_bEffectsDirty = false;
    m_bMacStyleDirty = false;
    emit changed(false);
}


#include "general.moc"
