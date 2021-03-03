//
// KDE Display color scheme setup module
//
// Copyright (c)  Mark Donohoe 1997
//
// Converted to a kcc module by Matthias Hoelzer 1997
// Ported to Qt-2.0 by Matthias Ettrich 1999
// Ported to kcontrol2 by Geert Jansen 1999
// Made maintable by Waldo Bastian 2000

#include <assert.h>
#include <config.h>
#include <stdlib.h>
#include <unistd.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <klineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qcombobox.h>
#include <klistbox.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qwhatsthis.h>

#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kcursor.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kstddirs.h>
#include <kipc.h>
#include <kcolordlg.h>
#include <kcolorbtn.h>
#include <kbuttonbox.h>

#include <X11/Xlib.h>

#include "colorscm.h"
#include "widgetcanvas.h"


/**** DLL Interface ****/

extern "C" {
    KCModule *create_colors(QWidget *parent, const char *name) {
    KGlobal::locale()->insertCatalogue("kcmcolors");
    return new KColorScheme(parent, name);
    }
}

/**** KColorScheme ****/

KColorScheme::KColorScheme(QWidget *parent, const char *name)
    : KCModule(parent, name)
{
    m_bChanged = false;
    nSysSchemes = 2;

    KConfig *cfg = new KConfig("kcmdisplayrc");
    cfg->setGroup("X11");
    useRM = cfg->readBoolEntry("useResourceManager", true);
    delete cfg;

    cs = new WidgetCanvas( this );
    cs->setCursor( KCursor::handCursor() );

    // LAYOUT

    QGridLayout *topLayout = new QGridLayout( this, 2, 2, 10 );
    topLayout->setRowStretch(0,0);
    topLayout->setRowStretch(1,0);
    topLayout->setColStretch(0,1);
    topLayout->setColStretch(1,1);

    cs->setFixedHeight(160);
    cs->setMinimumWidth(440);

    QWhatsThis::add( cs, i18n("This is a preview of the color settings which"
       " will be applied if you click \"Apply\" or \"OK\". You can click on"
       " different parts of this preview image. The widget name in the"
       " \"Widget color\" box will change to reflect the part of the preview"
       " image you clicked.") );

    connect( cs, SIGNAL( widgetSelected( int ) ),
         SLOT( slotWidgetColor( int ) ) );
    connect( cs, SIGNAL( colorDropped( int, const QColor&)),
         SLOT( slotColorForWidget( int, const QColor&)));
    topLayout->addMultiCellWidget( cs, 0, 0, 0, 1 );

    QGroupBox *group = new QGroupBox( i18n("Color Scheme"), this );
    topLayout->addWidget( group, 1, 0 );
    QBoxLayout *groupLayout = new QVBoxLayout( group, 10 );
    groupLayout->addSpacing(10);

    sList = new KListBox( group );
    readSchemeNames();
    sList->setCurrentItem( 0 );
    connect(sList, SIGNAL(highlighted(int)), SLOT(slotPreviewScheme(int)));
    groupLayout->addWidget(sList);

    QWhatsThis::add( sList, i18n("This is a list of predefined color schemes,"
       " including any that you may have created. You can preview an existing"
       " color scheme by selecting it from the list. The current scheme will"
       " be replaced by the selected color scheme.<p>"
       " Warning: if you have not yet applied any changes you may have made"
       " to the current scheme, those changes will be lost if you select"
       " another color scheme.") );

    addBt = new QPushButton(i18n("&Save scheme..."), group);
    connect(addBt, SIGNAL(clicked()), SLOT(slotAdd()));
    groupLayout->addWidget(addBt, 10);

    QWhatsThis::add( addBt, i18n("Press this button if you want to save"
       " the current color settings as a color scheme. You will be"
       " prompted for a name.") );

    removeBt = new QPushButton(i18n("&Remove scheme"), group);
    removeBt->setEnabled(FALSE);
    connect(removeBt, SIGNAL(clicked()), SLOT(slotRemove()));

    QWhatsThis::add( removeBt, i18n("Press this button to remove the selected"
       " color scheme. Note that this button is disabled if you do not have"
       " permission to delete the color scheme.") );

    groupLayout->addWidget( removeBt, 10 );

    QBoxLayout *stackLayout = new QVBoxLayout;
    topLayout->addLayout(stackLayout, 1, 1);

    group = new QGroupBox(i18n("Widget color"), this);
    stackLayout->addWidget(group);
    groupLayout = new QVBoxLayout(group, 10);
    groupLayout->addSpacing(10);

    wcCombo = new QComboBox(false, group);
    for(int i=0; i < CSM_LAST;i++)
    {
       wcCombo->insertItem(QString::null);
    }

    setColorName(i18n("Inactive title bar") , CSM_Inactive_title_bar);
    setColorName(i18n("Inactive title text"), CSM_Inactive_title_text);
    setColorName(i18n("Inactive title blend"), CSM_Inactive_title_blend);
    setColorName(i18n("Active title bar"), CSM_Active_title_bar);
    setColorName(i18n("Active title text"), CSM_Active_title_text);
    setColorName(i18n("Active title blend"), CSM_Active_title_blend);
    setColorName(i18n("Window background"), CSM_Background);
    setColorName(i18n("Window text"), CSM_Text);
    setColorName(i18n("Select background"), CSM_Select_background);
    setColorName(i18n("Select text"), CSM_Select_text);
    setColorName(i18n("Standard Background"), CSM_Standard_background);
    setColorName(i18n("Standard Text"), CSM_Standard_text);
    setColorName(i18n("Button background"), CSM_Button_background);
    setColorName(i18n("Button text"), CSM_Button_text);
    setColorName(i18n("Active title button"), CSM_Active_title_button);
    setColorName(i18n("Inactive title button"), CSM_Inactive_title_button);
    setColorName(i18n("Link"), CSM_Link);
    setColorName(i18n("Followed Link"), CSM_Followed_Link);
    setColorName(i18n("Alternate background in lists"), CSM_Alternate_background);

    wcCombo->adjustSize();
    connect(wcCombo, SIGNAL(activated(int)), SLOT(slotWidgetColor(int)));
    groupLayout->addWidget(wcCombo);

    QWhatsThis::add( wcCombo, i18n("Click here to select an element of"
       " the KDE desktop whose color you want to change. You may either"
       " choose the \"widget\" here, or click on the corresponding part"
       " of the preview image above.") );

    colorButton = new KColorButton( group );
    connect( colorButton, SIGNAL( changed(const QColor &)),
             SLOT(slotSelectColor(const QColor &)));

    groupLayout->addWidget( colorButton );

    QWhatsThis::add( colorButton, i18n("Click here to bring up a dialog"
       " box where you can choose a color for the \"widget\" selected"
       " in the above list.") );

    group = new QGroupBox(  i18n("Contrast"), this );
    stackLayout->addWidget(group);

    QVBoxLayout *groupLayout2 = new QVBoxLayout(group, 10);
    groupLayout2->addSpacing(10);
    groupLayout = new QHBoxLayout;
    groupLayout2->addLayout(groupLayout);

    sb = new QSlider( QSlider::Horizontal,group,"Slider" );
    sb->setRange( 0, 10 );
    sb->setFocusPolicy( QWidget::StrongFocus );
    connect(sb, SIGNAL(valueChanged(int)), SLOT(sliderValueChanged(int)));

    QWhatsThis::add(sb, i18n("Use this slider to change the contrast level"
       " of the current color scheme. Contrast does not affect all of the"
       " colors, only the edges of 3D objects."));

    QLabel *label = new QLabel(sb, i18n("Low Contrast", "Low"), group);
    groupLayout->addWidget(label);
    groupLayout->addWidget(sb, 10);
    label = new QLabel(group);
    label->setText(i18n("High Contrast", "High"));
    groupLayout->addWidget( label );

    load();
}


KColorScheme::~KColorScheme()
{
}

void KColorScheme::setColorName( const QString &name, int id )
{
    wcCombo->changeItem(name, id);
    cs->addToolTip( id, name );
}

void KColorScheme::load()
{
    KConfig *config = KGlobal::config();
    config->setGroup("KDE");
    sCurrentScheme = config->readEntry("colorScheme");

    sList->setCurrentItem(findSchemeByName(sCurrentScheme));
    readScheme(0);

    cs->drawSampleWidgets();
    slotWidgetColor(0);
    sb->blockSignals(true);
    sb->setValue(cs->contrast);
    sb->blockSignals(false);

    m_bChanged = false;
    emit changed(false);
}


void KColorScheme::save()
{
    if (!m_bChanged)
    return;

    KConfig *cfg = KGlobal::config();
    cfg->setGroup( "General" );
    cfg->writeEntry("background", cs->back, true, true);
    cfg->writeEntry("selectBackground", cs->select, true, true);
    cfg->writeEntry("foreground", cs->txt, true, true);
    cfg->writeEntry("windowForeground", cs->windowTxt, true, true);
    cfg->writeEntry("windowBackground", cs->window, true, true);
    cfg->writeEntry("selectForeground", cs->selectTxt, true, true);
    cfg->writeEntry("buttonBackground", cs->button, true, true);
    cfg->writeEntry("buttonForeground", cs->buttonTxt, true, true);
    cfg->writeEntry("linkColor", cs->link, true, true);
    cfg->writeEntry("visitedLinkColor", cs->visitedLink, true, true);
	cfg->writeEntry("alternateBackground", cs->alternateBackground, true, true);

    cfg->setGroup( "WM" );
    cfg->writeEntry("activeForeground", cs->aTxt, true, true);
    cfg->writeEntry("inactiveBackground", cs->iaTitle, true, true);
    cfg->writeEntry("inactiveBlend", cs->iaBlend, true, true);
    cfg->writeEntry("activeBackground", cs->aTitle, true, true);
    cfg->writeEntry("activeBlend", cs->aBlend, true, true);
    cfg->writeEntry("inactiveForeground", cs->iaTxt, true, true);
    cfg->writeEntry("activeTitleBtnBg", cs->aTitleBtn, true, true);
    cfg->writeEntry("inactiveTitleBtnBg", cs->iTitleBtn, true, true);

    cfg->setGroup( "KDE" );
    cfg->writeEntry("contrast", cs->contrast, true, true);
    cfg->writeEntry("colorScheme", sCurrentScheme, true, true);
    cfg->sync();

    // KDE-1.x support
    KSimpleConfig *config =
    new KSimpleConfig( QCString(::getenv("HOME")) + "/.kderc" );
    config->setGroup( "General" );
    config->writeEntry("background", cs->back );
    config->writeEntry("selectBackground", cs->select );
    config->writeEntry("foreground", cs->txt, true, true);
    config->writeEntry("windowForeground", cs->windowTxt );
    config->writeEntry("windowBackground", cs->window );
    config->writeEntry("selectForeground", cs->selectTxt );
    config->sync();
    delete config;

    QApplication::setOverrideCursor( waitCursor );
    QStringList args;
    args.append("style");
    kapp->kdeinitExecWait("kcminit2", args);
    QApplication::restoreOverrideCursor();
    QApplication::flushX();

    // Notify all KDE applications
    KIPC::sendMessageAll(KIPC::PaletteChanged);

    m_bChanged = false;
    emit changed(false);
}


void KColorScheme::defaults()
{
    readScheme(1);
    sList->setCurrentItem(1);

    cs->drawSampleWidgets();
    slotWidgetColor(0);
    sb->blockSignals(true);
    sb->setValue(cs->contrast);
    sb->blockSignals(false);

    m_bChanged = true;
    emit changed(true);
}

QString KColorScheme::quickHelp() const
{
    return i18n("<h1>Colors</h1> This module allows you to choose"
       " the color scheme used for the KDE desktop. The different"
       " elements of the desktop, such as title bars, menu text, etc.,"
       " are called \"widgets\". You can choose the widget whose"
       " color you want to change by selecting it from a list, or by"
       " clicking on a graphical representation of the desktop.<p>"
       " You can save color settings as complete color schemes,"
       " which can also be modified or deleted. KDE comes with several"
       " predefined color schemes on which you can base your own.<p>"
       " All KDE applications will obey the selected color scheme."
       " Non-KDE applications may also obey some or all of the color"
       " settings. See the \"Style\" control module for more details.");
}


void KColorScheme::sliderValueChanged( int val )
{
    cs->contrast = val;
    cs->drawSampleWidgets();

    sCurrentScheme = QString::null;

    m_bChanged = true;
    emit changed(true);
}


void KColorScheme::slotSave( )
{
    sCurrentScheme = sFileList[ sList->currentItem() ];
    KSimpleConfig *config = new KSimpleConfig(sCurrentScheme );
    int i = sCurrentScheme.findRev('/');
    if (i >= 0)
      sCurrentScheme = sCurrentScheme.mid(i+1);

    config->setGroup("Color Scheme" );
    config->writeEntry("background", cs->back );
    config->writeEntry("selectBackground", cs->select );
    config->writeEntry("foreground", cs->txt );
    config->writeEntry("activeForeground", cs->aTxt );
    config->writeEntry("inactiveBackground", cs->iaTitle );
    config->writeEntry("inactiveBlend", cs->iaBlend );
    config->writeEntry("activeBackground", cs->aTitle );
    config->writeEntry("activeBlend", cs->aBlend );
    config->writeEntry("inactiveForeground", cs->iaTxt );
    config->writeEntry("windowForeground", cs->windowTxt );
    config->writeEntry("windowBackground", cs->window );
    config->writeEntry("selectForeground", cs->selectTxt );
    config->writeEntry("contrast", cs->contrast );
    config->writeEntry("buttonForeground", cs->buttonTxt );
    config->writeEntry("buttonBackground", cs->button );
    config->writeEntry("activeTitleBtnBg", cs->aTitleBtn);
    config->writeEntry("inactiveTitleBtnBg", cs->iTitleBtn);
    config->writeEntry("linkColor", cs->link);
    config->writeEntry("visitedLinkColor", cs->visitedLink);
	config->writeEntry("alternateBackground", cs->alternateBackground);

    delete config;
}


void KColorScheme::slotRemove()
{
    uint ind = sList->currentItem();
    if (unlink(QFile::encodeName(sFileList[ind]).data())) {
    KMessageBox::error( 0,
          i18n("This color scheme could not be removed.\n"
           "Perhaps you do not have permission to alter the file\n"
            "system where the color scheme is stored." ));
    return;
    }

    sList->removeItem(ind);
    sFileList.remove(sFileList.at(ind));
}


/*
 * Add a local color scheme.
 */
void KColorScheme::slotAdd()
{
    QString sName;
    if (sList->currentItem() >= nSysSchemes)
       sName = sList->currentText();
    SaveScm *ss = new SaveScm( 0,  "save scheme", sName );

    QString sFile;

    bool valid = false;
    int exists = -1;

    while (!valid)
    {
        if (ss->exec() != QDialog::Accepted)
            return;

        sName = ss->nameLine->text();
        sName = sName.simplifyWhiteSpace();
        if (sName.isEmpty())
            return;
        sFile = sName;

        int i = 0;

        // Capitalise each word
        sName.at(0) = sName.at(0).upper();
        while (1)
        {
            i = sName.find(" ", i);
            if (i == -1)
                break;
            if (++i >= (int) sName.length())
                break;
            sName.at(i) = sName.at(i).upper();
        }

        exists = -1;
        // Check if it's already there
        for (i=0; i < (int) sList->count(); i++)
        {
            if (sName == sList->text(i))
            {
                exists = i;
                int result = KMessageBox::warningContinueCancel( 0,
                   i18n("A color scheme with the name '%1' already exists.\n"
                        "Do you want to overwrite it?\n").arg(sName),
		   i18n("Save color scheme"),
                   i18n("Overwrite"));
                if (result == KMessageBox::Cancel)
                    break;
            }
        }
        if (i == (int) sList->count())
            valid = true;
    }

    disconnect(sList, SIGNAL(highlighted(int)), this,
        SLOT(slotPreviewScheme(int)));

    if (exists != -1)
    {
       sList->setFocus();
       sList->setCurrentItem(exists);
    }
    else
    {
       sList->insertItem(sName);
       sList->setFocus();
       sList->setCurrentItem(sList->count() - 1);
       sFile = KGlobal::dirs()->saveLocation("data", "kdisplay2/color-schemes/") + sFile + ".kcsrc";
       sFileList.append(sFile);

       KSimpleConfig *config = new KSimpleConfig(sFile);
       config->setGroup( "Color Scheme");
       config->writeEntry("Name", sName);
       delete config;
    }
    slotSave();

    connect(sList, SIGNAL(highlighted(int)), SLOT(slotPreviewScheme(int)));
    slotPreviewScheme(sList->currentItem());
}

QColor &KColorScheme::color(int index)
{
    switch(index) {
    case CSM_Inactive_title_bar:
    return cs->iaTitle;
    case CSM_Inactive_title_text:
    return cs->iaTxt;
    case CSM_Inactive_title_blend:
    return cs->iaBlend;
    case CSM_Active_title_bar:
    return cs->aTitle;
    case CSM_Active_title_text:
    return cs->aTxt;
    case CSM_Active_title_blend:
    return cs->aBlend;
    case CSM_Background:
    return cs->back;
    case CSM_Text:
    return cs->txt;
    case CSM_Select_background:
    return cs->select;
    case CSM_Select_text:
    return cs->selectTxt;
    case CSM_Standard_background:
    return cs->window;
    case CSM_Standard_text:
    return cs->windowTxt;
    case CSM_Button_background:
    return cs->button;
    case CSM_Button_text:
    return cs->buttonTxt;
    case CSM_Active_title_button:
    return cs->aTitleBtn;
    case CSM_Inactive_title_button:
    return cs->iTitleBtn;
    case CSM_Link:
    return cs->link;
    case CSM_Followed_Link:
    return cs->visitedLink;
    case CSM_Alternate_background:
    return cs->alternateBackground;
    }

    assert(0); // Should never be here!
    return cs->iaTxt; // Silence compiler
}


void KColorScheme::slotSelectColor(const QColor &col)
{
    int selection;
    selection = wcCombo->currentItem();

    color(selection) = col;

    cs->drawSampleWidgets();

    sCurrentScheme = QString::null;

    m_bChanged = true;
    emit changed(true);
}


void KColorScheme::slotWidgetColor(int indx)
{
    if (wcCombo->currentItem() != indx)
    wcCombo->setCurrentItem( indx );

    QColor col = color(indx);
    colorButton->setColor( col );
    colorPushColor = col;
}



void KColorScheme::slotColorForWidget(int indx, const QColor& col)
{
    slotWidgetColor(indx);
    slotSelectColor(col);
}


/*
 * Read a color scheme into "cs".
 *
 * KEEP IN SYNC with thememgr!
 */
void KColorScheme::readScheme( int index )
{
    KConfigBase* config;

    // define some KDE2 default colors
    QColor kde2Blue;
    if (QPixmap::defaultDepth() > 8)
      kde2Blue.setRgb(10, 95, 137);
    else
      kde2Blue.setRgb(0, 0, 192);

    QColor widget(220, 220, 220);

    QColor button;
    if (QPixmap::defaultDepth() > 8)
      button.setRgb(228, 228, 228);
    else
      button.setRgb(220, 220, 220);

    QColor link(0, 0, 192);
    QColor visitedLink(128, 0,128);
    QColor alternate(240, 240, 240);

    // note: keep default color scheme in sync with default Current Scheme
    if (index == 1) {
      sCurrentScheme  = "<default>";
      cs->back        = widget;
      cs->txt         = black;
      cs->select      = kde2Blue;
      cs->selectTxt   = white;
      cs->window      = white;
      cs->windowTxt   = black;
      cs->iaTitle     = widget;
      cs->iaTxt       = black;
      cs->iaBlend     = widget;
      cs->aTitle      = kde2Blue;
      cs->aTxt        = white;
      cs->aBlend      = kde2Blue;
      cs->button      = button;
      cs->buttonTxt   = black;
      cs->aTitleBtn   = cs->back;
      cs->iTitleBtn   = cs->back;
      cs->link        = link;
      cs->visitedLink = visitedLink;
      cs->alternateBackground = alternate;

      cs->contrast    = 7;

      return;
    }

    if (index == 0) {
      // Current scheme
      config = KGlobal::config();
      config->setGroup("General");
    } else {
      // Open scheme file
      sCurrentScheme = sFileList[ index ];
      config = new KSimpleConfig(sCurrentScheme, true);
      config->setGroup("Color Scheme");
      int i = sCurrentScheme.findRev('/');
      if (i >= 0)
        sCurrentScheme = sCurrentScheme.mid(i+1);
    }

    // note: defaults should be the same as the KDE default
    cs->txt = config->readColorEntry( "foreground", &black );
    cs->back = config->readColorEntry( "background", &widget );
    cs->select = config->readColorEntry( "selectBackground", &kde2Blue );
    cs->selectTxt = config->readColorEntry( "selectForeground", &white );
    cs->window = config->readColorEntry( "windowBackground", &white );
    cs->windowTxt = config->readColorEntry( "windowForeground", &black );
    cs->button = config->readColorEntry( "buttonBackground", &button );
    cs->buttonTxt = config->readColorEntry( "buttonForeground", &black );
    cs->link = config->readColorEntry( "linkColor", &link );
    cs->visitedLink = config->readColorEntry( "visitedLinkColor", &visitedLink );
    alternate = cs->window.dark(106);
    int h, s, v;
    cs->window.hsv( &h, &s, &v );
    if (v > 128)
        alternate = cs->window.dark(106);
    else if (cs->window != black)
        alternate = cs->window.light(106);
    else
        alternate = Qt::darkGray;

    cs->alternateBackground = config->readColorEntry( "alternateBackground", &alternate );

    if (index == 0)
      config->setGroup( "WM" );

    cs->iaTitle = config->readColorEntry("inactiveBackground", &widget);
    cs->iaTxt = config->readColorEntry("inactiveForeground", &black);
    cs->iaBlend = config->readColorEntry("inactiveBlend", &widget);
    cs->aTitle = config->readColorEntry("activeBackground", &kde2Blue);
    cs->aTxt = config->readColorEntry("activeForeground", &white);
    cs->aBlend = config->readColorEntry("activeBlend", &kde2Blue);
    // hack - this is all going away. For now just set all to button bg
    cs->aTitleBtn = config->readColorEntry("activeTitleBtnBg", &cs->back);
    cs->iTitleBtn = config->readColorEntry("inactiveTitleBtnBg", &cs->back);

    if (index == 0)
      config->setGroup( "KDE" );

    cs->contrast = config->readNumEntry( "contrast", 7 );
    if (index != 0)
      delete config;
}


/*
 * Get all installed color schemes.
 */
void KColorScheme::readSchemeNames()
{
    // Always a current and a default scheme
    sList->insertItem( i18n("Current scheme"), 0 );
    sFileList.append( "Not a  kcsrc file" );
    sList->insertItem( i18n("KDE default"), 1 );
    sFileList.append( "Not a kcsrc file" );
    nSysSchemes = 2;

    // Global + local schemes
    QStringList list = KGlobal::dirs()->findAllResources("data",
            "kdisplay2/color-schemes/*.kcsrc", false, true);

    // Put local schemes into localList
    QStringList localList;
    QStringList::Iterator it;
    for (it = list.begin(); it != list.end(); it++) {
    QFileInfo fi(*it);
    if (fi.isWritable()) {
        localList.append(*it);
        it = list.remove(it);
        it--;
    }
    }

    // And add them
    for (it = list.begin(); it != list.end(); it++) {
    KSimpleConfig *config = new KSimpleConfig(*it, true);
    config->setGroup("Color Scheme");
    QString str = config->readEntry("Name");
    if (str.isEmpty()) {
       str =  config->readEntry("name");
       if (str.isEmpty())
          continue;
    }
    sList->insertItem(str);
    sFileList.append(*it);
    nSysSchemes++;
    delete config;
    }

    // Now repeat for local files
    for (it = localList.begin(); it != localList.end(); it++) {
    KSimpleConfig *config = new KSimpleConfig((*it), true);
    config->setGroup("Color Scheme");
    QString str = config->readEntry("Name");
    if (str.isEmpty()) {
       str =  config->readEntry("name");
       if (str.isEmpty())
          continue;
    }
    sList->insertItem(str);
    sFileList.append(*it);
    delete config;
    }
}

/*
 * Find scheme based on filename
 */
int KColorScheme::findSchemeByName(const QString &scheme)
{
   if (scheme.isEmpty())
      return 0;
   if (scheme == "<default>")
      return 1;

   QString search = scheme;
   int i = search.findRev('/');
   if (i >= 0)
     search = search.mid(i+1);

   i = nSysSchemes;
   while (i < sFileList.count())
   {
      if (sFileList[i].contains(search))
         return i;
      i++;
   }

   return 0;
}


void KColorScheme::slotPreviewScheme(int indx)
{
    readScheme(indx);

    // Set various appropriate for the scheme

    cs->drawSampleWidgets();
    sb->blockSignals(true);
    sb->setValue(cs->contrast);
    sb->blockSignals(false);
    slotWidgetColor(0);
    if (indx < nSysSchemes)
       removeBt->setEnabled(false);
    else
       removeBt->setEnabled(true);

    m_bChanged = (indx != 0);
    emit changed(m_bChanged);
}


/* this function should dissappear: colorscm should work directly on a Qt palette, since
   this will give us much more cusomization with qt-2.0.
   */
QPalette KColorScheme::createPalette()
{
    QColorGroup disabledgrp(cs->windowTxt, cs->back, cs->back.light(150),
                cs->back.dark(), cs->back.dark(120), cs->back.dark(120),
                cs->window);

    QColorGroup colgrp(cs->windowTxt, cs->back, cs->back.light(150),
               cs->back.dark(), cs->back.dark(120), cs->txt, cs->window);

    colgrp.setColor(QColorGroup::Highlight, cs->select);
    colgrp.setColor(QColorGroup::HighlightedText, cs->selectTxt);
    colgrp.setColor(QColorGroup::Button, cs->button);
    colgrp.setColor(QColorGroup::ButtonText, cs->buttonTxt);
    return QPalette( colgrp, disabledgrp, colgrp);
}


/**** SaveScm ****/

SaveScm::SaveScm( QWidget *parent, const char *name, const QString &def )
    : KDialogBase( parent, name, true, i18n("Save color scheme"), Ok|Cancel, Ok, true )
{
    QWidget *page = new QWidget(this);
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    nameLine = new KLineEdit( page );
    nameLine->setFocus();
    nameLine->setMaxLength(18);
    nameLine->setFixedHeight( nameLine->sizeHint().height() );
    nameLine->setText(def);
    nameLine->selectAll();

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( nameLine,
     i18n( "Enter a name for the color scheme:\n"), page);

    tmpQLabel->setAlignment( AlignLeft | AlignBottom | ShowPrefix );
    tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );
    tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() );

    topLayout->addWidget( tmpQLabel );
    topLayout->addWidget( nameLine );
    topLayout->addStretch( 10 );
}


#include "colorscm.moc"
