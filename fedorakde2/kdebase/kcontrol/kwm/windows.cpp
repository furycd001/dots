/*
 * windows.cpp
 *
 * Copyright (c) 1997 Patrick Dowler dowler@morgul.fsh.uvic.ca
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qwhatsthis.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlcdnumber.h>

#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <knuminput.h>
#include <kdialog.h>
#include <kapp.h>
#include <dcopclient.h>
#include <kdesktopwidget.h>

#include <kglobal.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "windows.h"
#include "geom.h"


// kwin config keywords
#define KWIN_FOCUS                 "FocusPolicy"
#define KWIN_PLACEMENT             "Placement"
#define KWIN_MOVE                  "MoveMode"
#define KWIN_MINIMIZE_ANIM         "AnimateMinimize"
#define KWIN_MINIMIZE_ANIM_SPEED   "AnimateMinimizeSpeed"
#define KWIN_RESIZE_OPAQUE         "ResizeMode"
#define KWIN_AUTORAISE_INTERVAL    "AutoRaiseInterval"
#define KWIN_AUTORAISE             "AutoRaise"
#define KWIN_CLICKRAISE            "ClickRaise"
#define KWIN_ANIMSHADE             "AnimateShade"
#define KWIN_MOVE_RESIZE_MAXIMIZED "MoveResizeMaximizedWindows"
#define KWIN_ALTTABMODE            "AltTabStyle"
#define KWIN_TRAVERSE_ALL          "TraverseAll"
#define KWIN_SHADEHOVER            "ShadeHover"
#define KWIN_SHADEHOVER_INTERVAL   "ShadeHoverInterval"
#define KWIN_XINERAMA              "XineramaEnabled"
#define KWIN_XINERAMA_MOVEMENT     "XineramaMovementEnabled"
#define KWIN_XINERAMA_PLACEMENT    "XineramaPlacementEnabled"
#define KWIN_XINERAMA_MAXIMIZE     "XineramaMaximizeEnabled"

/*
 * Some inter-widget spacing constants
 */
#define SPACE_XO 20
#define SPACE_YO 20
#define SPACE_XI 10
#define SPACE_YI 10

#define max(a,b) ((a > b) ? a:b)

// kwm config keywords
#define KWM_ELECTRIC_BORDER                  "ElectricBorder"
#define KWM_ELECTRIC_BORDER_DELAY            "ElectricBorderNumberOfPushes"
#define KWM_ELECTRIC_BORDER_MOVE_POINTER     "ElectricBorderPointerWarp"

//CT 15mar 98 - magics
#define KWM_BRDR_SNAP_ZONE                   "BorderSnapZone"
#define KWM_BRDR_SNAP_ZONE_DEFAULT           10
#define KWM_WNDW_SNAP_ZONE                   "WindowSnapZone"
#define KWM_WNDW_SNAP_ZONE_DEFAULT           10

#define MAX_BRDR_SNAP                          100
#define MAX_WNDW_SNAP                          100
#define MAX_EDGE_RES                         1000


KFocusConfig::~KFocusConfig ()
{
}

// removed the LCD display over the slider - this is not good GUI design :) RNolden 051701
KFocusConfig::KFocusConfig (KConfig *_config, QWidget * parent, const char *name)
    : KCModule (parent, name), config(_config)
{
    QString wtstr;
    QBoxLayout *lay = new QVBoxLayout (this, KDialog::marginHint(),
                                       KDialog::spacingHint());

    //iTLabel = new QLabel(i18n("  Allowed overlap:\n"
    //                         "(% of desktop space)"),
    //             plcBox);
    //iTLabel->setAlignment(AlignTop|AlignHCenter);
    //pLay->addWidget(iTLabel,1,1);

    //interactiveTrigger = new QSpinBox(0, 500, 1, plcBox);
    //pLay->addWidget(interactiveTrigger,1,2);

    //pLay->addRowSpacing(2,KDialog::spacingHint());

    //lay->addWidget(plcBox);

    // focus policy
    fcsBox = new QButtonGroup(i18n("Focus policy"),this);

    QGridLayout *fLay = new QGridLayout(fcsBox,5,3,
                                        KDialog::marginHint(),
                                        KDialog::spacingHint());
    fLay->addRowSpacing(0,fontMetrics().lineSpacing());
    fLay->setColStretch(0,0);
    fLay->setColStretch(1,1);
    fLay->setColStretch(2,1);


    focusCombo =  new QComboBox(false, fcsBox);
    focusCombo->insertItem(i18n("Click to focus"), CLICK_TO_FOCUS);
    focusCombo->insertItem(i18n("Focus follows mouse"), FOCUS_FOLLOWS_MOUSE);
    focusCombo->insertItem(i18n("Focus under mouse"), FOCUS_UNDER_MOUSE);
    focusCombo->insertItem(i18n("Focus strictly under mouse"), FOCUS_STRICTLY_UNDER_MOUSE);
    fLay->addMultiCellWidget(focusCombo,1,1,0,1);

    // FIXME, when more policies have been added to KWin
    QWhatsThis::add( focusCombo, i18n("The focus policy is used to determine the active window, i.e."
                                      " the window you can work in. <ul>"
                                      " <li><em>Click to focus:</em> A window becomes active when you click into it. This is the behavior"
                                      " you might know from other operating systems.</li>"
                                      " <li><em>Focus follows mouse:</em> Moving the mouse pointer actively on to a"
                                      " normal window activates it. Very practical if you are using the mouse a lot.</li>"
                                      " <li><em>Focus under mouse:</em> The window that happens to be under the"
                                      " mouse pointer becomes active. If the mouse points nowhere, the last window"
                                      " that was under the mouse has focus. </li>"
                                      " <li><em>Focus strictly under mouse:</em> This is even worse than"
                                      " 'Focus under mouse'. Only the window under the mouse pointer is"
                                      " active. If the mouse points nowhere, nothing has focus. "
                                      " </ul>"
                                      " Note that 'Focus under mouse' and 'Focus strictly under mouse' are not"
                                      " particularly useful. They are only provided for old-fashioned"
                                      " die-hard UNIX people ;-)"
                         ) );

    connect(focusCombo, SIGNAL(activated(int)),this,
            SLOT(setAutoRaiseEnabled()) );

    // autoraise delay

    autoRaiseOn = new QCheckBox(i18n("Auto Raise"), fcsBox);
    fLay->addWidget(autoRaiseOn,2,0);
    connect(autoRaiseOn,SIGNAL(toggled(bool)), this, SLOT(autoRaiseOnTog(bool)));

    clickRaiseOn = new QCheckBox(i18n("Click Raise"), fcsBox);
    fLay->addWidget(clickRaiseOn,4,0);

    connect(clickRaiseOn,SIGNAL(toggled(bool)), this, SLOT(clickRaiseOnTog(bool)));

    alabel = new QLabel(i18n("Delay (ms)"), fcsBox);
    alabel->setAlignment(AlignVCenter|AlignHCenter);
    fLay->addWidget(alabel,3,0,AlignLeft);

    autoRaise = new KIntNumInput(500, fcsBox);
    autoRaise->setRange(0, 3000, 100, true);
    autoRaise->setSteps(100,100);
    fLay->addMultiCellWidget(autoRaise,3,3,1,2);

    fLay->addColSpacing(0,QMAX(autoRaiseOn->sizeHint().width(),
                               clickRaiseOn->sizeHint().width()) + 15);

    QWhatsThis::add( autoRaiseOn, i18n("If Auto Raise is enabled, a window in the background will automatically"
                                       " come to the front when the mouse pointer has been over it for some time.") );
    wtstr = i18n("This is the delay after which the window that the mouse pointer is over will automatically"
                 " come to the front.");
    QWhatsThis::add( autoRaise, wtstr );
//    QWhatsThis::add( s, wtstr );
    QWhatsThis::add( alabel, wtstr );

    QWhatsThis::add( clickRaiseOn, i18n("When this option is enabled, your windows will be brought to the"
                                        " front when you click somewhere into the window contents.") );

    lay->addWidget(fcsBox);

    kbdBox = new QButtonGroup(i18n("Keyboard"), this);
    QGridLayout *kLay = new QGridLayout(kbdBox, 3, 3,
                                        KDialog::marginHint(),
                                        KDialog::spacingHint());
    kLay->addRowSpacing(0,10);
    QLabel *altTabLabel = new QLabel( i18n("Walk through windows mode:"), kbdBox);
    kLay->addWidget(altTabLabel, 1, 0);
    kdeMode = new QRadioButton(i18n("KDE"), kbdBox);
    kLay->addWidget(kdeMode, 1, 1);
    cdeMode = new QRadioButton(i18n("CDE"), kbdBox);
    kLay->addWidget(cdeMode, 1, 2);

    wtstr = i18n("Keep the Alt key pressed and hit the Tab key repeatedly to walk"
                 " through the windows on the current desktop (the Alt+Tab"
                 " combination can be reconfigured). The two different modes mean:<ul>"
                 "<li><b>KDE</b>: a nice widget is shown, displaying the icons of all windows to"
                 " walk through and the title of the currently selected one;"
                 "<li><b>CDE</b>: the focus is passed to a new window each time Tab is hit."
                 " No fancy widget.</li></ul>");
    QWhatsThis::add( altTabLabel, wtstr );
    QWhatsThis::add( kdeMode, wtstr );
    QWhatsThis::add( cdeMode, wtstr );

    traverseAll = new QCheckBox( i18n( "Traverse windows on all desktops" ), kbdBox );
    kLay->addMultiCellWidget( traverseAll, 2, 2, 0, 2 );

    wtstr = i18n( "Leave this option disabled if you want to limit walking through"
                  " windows to the current desktop." );
    QWhatsThis::add( traverseAll, wtstr );

    lay->addWidget(kbdBox);

    lay->addStretch();

    // Any changes goes to slotChanged()
    connect(focusCombo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
    connect(fcsBox, SIGNAL(clicked(int)), this, SLOT(slotChanged()));
    connect(autoRaise, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()));
    connect(kdeMode, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect(cdeMode, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect(traverseAll, SIGNAL(clicked()), this, SLOT(slotChanged()));

    load();
}

// many widgets connect to this slot
void KFocusConfig::slotChanged()
{
    emit changed(true);
}

int KFocusConfig::getFocus()
{
    return focusCombo->currentItem();
}

void KFocusConfig::setFocus(int foc)
{
    focusCombo->setCurrentItem(foc);

    // this will disable/hide the auto raise delay widget if focus==click
    setAutoRaiseEnabled();
}

void KFocusConfig::setAutoRaiseInterval(int tb)
{
    autoRaise->setValue(tb);
}

int KFocusConfig::getAutoRaiseInterval()
{
    return autoRaise->value();
}

void KFocusConfig::setAutoRaise(bool on)
{
    autoRaiseOn->setChecked(on);
}

void KFocusConfig::setClickRaise(bool on)
{
    clickRaiseOn->setChecked(on);
}

void KFocusConfig::setAutoRaiseEnabled()
{
    // the auto raise related widgets are: autoRaise, alabel, s, sec
    if ( focusCombo->currentItem() != CLICK_TO_FOCUS )
    {
        clickRaiseOn->setEnabled(true);
        clickRaiseOnTog(clickRaiseOn->isChecked());
        autoRaiseOn->setEnabled(true);
        autoRaiseOnTog(autoRaiseOn->isChecked());
    }
    else
    {
        autoRaiseOn->setEnabled(false);
        autoRaiseOnTog(false);
        clickRaiseOn->setEnabled(false);
        clickRaiseOnTog(false);
    }
}


// CT 13mar98 interactiveTrigger configured by this slot
// void KFocusConfig::ifPlacementIsInteractive( )
// {
//   if( placementCombo->currentItem() == INTERACTIVE_PLACEMENT) {
//     iTLabel->setEnabled(true);
//     interactiveTrigger->show();
//   }
//   else {
//     iTLabel->setEnabled(false);
//     interactiveTrigger->hide();
//   }
// }
//CT

//CT 23Oct1998 make AutoRaise toggling much clear
void KFocusConfig::autoRaiseOnTog(bool a) {
    autoRaise->setEnabled(a);
    alabel->setEnabled(a);
    clickRaiseOn->setEnabled( !a );
    if ( a )
        clickRaiseOn->setChecked( TRUE );

}
//CT

void KFocusConfig::clickRaiseOnTog(bool ) {
}

void KFocusConfig::setAltTabMode(bool a) {
    kdeMode->setChecked(a);
    cdeMode->setChecked(!a);
}

void KFocusConfig::setTraverseAll(bool a) {
    traverseAll->setChecked(a);
}

void KFocusConfig::load( void )
{
    QString key;

    config->setGroup( "Windows" );

    key = config->readEntry(KWIN_FOCUS);
    if( key == "ClickToFocus")
        setFocus(CLICK_TO_FOCUS);
    else if( key == "FocusFollowsMouse")
        setFocus(FOCUS_FOLLOWS_MOUSE);
    else if(key == "FocusUnderMouse")
        setFocus(FOCUS_UNDER_MOUSE);
    else if(key == "FocusStrictlyUnderMouse")
        setFocus(FOCUS_STRICTLY_UNDER_MOUSE);

    int k = config->readNumEntry(KWIN_AUTORAISE_INTERVAL,0);
    setAutoRaiseInterval(k);

    key = config->readEntry(KWIN_AUTORAISE);
    setAutoRaise(key == "on");
    key = config->readEntry(KWIN_CLICKRAISE);
    setClickRaise(key != "off");
    setAutoRaiseEnabled();      // this will disable/hide the auto raise delay widget if focus==click

    key = config->readEntry(KWIN_ALTTABMODE, "KDE");
    setAltTabMode(key == "KDE");

    config->setGroup( "TabBox" );
    setTraverseAll( config->readBoolEntry(KWIN_TRAVERSE_ALL, false ));

    config->setGroup("Desktops");
}

void KFocusConfig::save( void )
{
    int v;

    config->setGroup( "Windows" );

    v = getFocus();
    if (v == CLICK_TO_FOCUS)
        config->writeEntry(KWIN_FOCUS,"ClickToFocus");
    else if (v == FOCUS_UNDER_MOUSE)
        config->writeEntry(KWIN_FOCUS,"FocusUnderMouse");
    else if (v == FOCUS_STRICTLY_UNDER_MOUSE)
        config->writeEntry(KWIN_FOCUS,"FocusStrictlyUnderMouse");
    else
        config->writeEntry(KWIN_FOCUS,"FocusFollowsMouse");

    v = getAutoRaiseInterval();
    if (v <0) v = 0;
    config->writeEntry(KWIN_AUTORAISE_INTERVAL,v);

    if (autoRaiseOn->isChecked())
        config->writeEntry(KWIN_AUTORAISE, "on");
    else
        config->writeEntry(KWIN_AUTORAISE, "off");

    if (clickRaiseOn->isChecked())
        config->writeEntry(KWIN_CLICKRAISE, "on");
    else
        config->writeEntry(KWIN_CLICKRAISE, "off");

    if (kdeMode->isChecked())
        config->writeEntry(KWIN_ALTTABMODE, "KDE");
    else
        config->writeEntry(KWIN_ALTTABMODE, "CDE");

    config->setGroup( "TabBox" );
    config->writeEntry( KWIN_TRAVERSE_ALL , traverseAll->isChecked());

    config->setGroup("Desktops");
}

void KFocusConfig::defaults()
{
    setFocus(CLICK_TO_FOCUS);
    setAutoRaise(false);
    setClickRaise(false);
    setAltTabMode(true);
    setTraverseAll( false );
}

KAdvancedConfig::~KAdvancedConfig ()
{
}

KAdvancedConfig::KAdvancedConfig (KConfig *_config, QWidget *parent, const char *name)
    : KCModule (parent, name), config(_config)
{
    QString wtstr;
    QBoxLayout *lay = new QVBoxLayout (this, KDialog::marginHint(),
                                       KDialog::spacingHint());

    windowsBox = new QButtonGroup(i18n("Windows"), this);

    QBoxLayout *wLay = new QVBoxLayout (windowsBox,KDialog::marginHint(),
                                        KDialog::spacingHint());
    wLay->addSpacing(fontMetrics().lineSpacing());

    QBoxLayout *bLay = new QVBoxLayout;
    wLay->addLayout(bLay);

    opaque = new QCheckBox(i18n("Display content in moving windows"), windowsBox);
    bLay->addWidget(opaque);
    QWhatsThis::add( opaque, i18n("Enable this option if you want a window's content to be fully shown"
                                  " while moving it, instead of just showing a window 'skeleton'. The result may not be satisfying"
                                  " on slow machines without graphic acceleration.") );

    resizeOpaqueOn = new QCheckBox(i18n("Display content in resizing windows"), windowsBox);
    bLay->addWidget(resizeOpaqueOn);
    QWhatsThis::add( resizeOpaqueOn, i18n("Enable this option if you want a window's content to be shown"
                                          " while resizing it, instead of just showing a window 'skeleton'. The result may not be satisfying"
                                          " on slow machines.") );

    QGridLayout *rLay = new QGridLayout(2,3);
    bLay->addLayout(rLay);
    rLay->setColStretch(0,0);
    rLay->setColStretch(1,1);

    minimizeAnimOn = new QCheckBox(i18n("Animate Minimize and Restore"),
                                   windowsBox);
    QWhatsThis::add( minimizeAnimOn, i18n("Enable this option if you want an animation shown when"
                                          " windows are minimized or restored." ) );
    rLay->addWidget(minimizeAnimOn,0,0);

    minimizeAnimSlider = new QSlider(0,10,10,0,QSlider::Horizontal, windowsBox);
    minimizeAnimSlider->setSteps(10,1);
    rLay->addMultiCellWidget(minimizeAnimSlider,0,0,1,2);

    minimizeAnimSlowLabel= new QLabel(i18n("Slow"),windowsBox);
    minimizeAnimSlowLabel->setAlignment(AlignTop|AlignLeft);
    rLay->addWidget(minimizeAnimSlowLabel,1,1);

    minimizeAnimFastLabel= new QLabel(i18n("Fast"),windowsBox);
    minimizeAnimFastLabel->setAlignment(AlignTop|AlignRight);
    rLay->addWidget(minimizeAnimFastLabel,1,2);

    wtstr = i18n("Here you can set the speed of the animation shown when windows are"
                 " minimized and restored. ");
    QWhatsThis::add( minimizeAnimSlider, wtstr );
    QWhatsThis::add( minimizeAnimSlowLabel, wtstr );
    QWhatsThis::add( minimizeAnimFastLabel, wtstr );

    //CT 17Dec2000 - maybe restore this button box when placement becomes again what was in KDE-1
    // placement policy --- CT 19jan98, 13mar98 ---
    //plcBox = new QButtonGroup(i18n("Placement policy"),this);

    //QGridLayout *pLay = new QGridLayout(plcBox,3,3,
    //                    KDialog::marginHint(),
    //                    KDialog::spacingHint());
    //pLay->addRowSpacing(0,fontMetrics().lineSpacing());


    moveResizeMaximized = new QCheckBox( i18n("Allow Moving and Resizing of maximized windows"), windowsBox);
    bLay->addWidget(moveResizeMaximized);
    QWhatsThis::add(moveResizeMaximized, i18n("When enabled, this feature activates the border of maximized windows"
                                              " and allows you to move or resize them,"
                                              " just like for normal windows"));


    rLay = new QGridLayout(1,3);
    bLay->addLayout(rLay);
    rLay->setColStretch(0,1);
    rLay->setColStretch(1,3);
    rLay->setColStretch(2,2);

    QLabel *plcLabel = new QLabel(i18n("Placement:"),windowsBox);

    placementCombo = new QComboBox(false, windowsBox);
    placementCombo->insertItem(i18n("Smart"), SMART_PLACEMENT);
    placementCombo->insertItem(i18n("Cascade"), CASCADE_PLACEMENT);
    placementCombo->insertItem(i18n("Random"), RANDOM_PLACEMENT);
    // CT: disabling is needed as long as functionality misses in kwin
    //placementCombo->insertItem(i18n("Interactive"), INTERACTIVE_PLACEMENT);
    //placementCombo->insertItem(i18n("Manual"), MANUAL_PLACEMENT);
    placementCombo->setCurrentItem(SMART_PLACEMENT);

    // FIXME, when more policies have been added to KWin
    wtstr = i18n("The placement policy determines where a new window"
                 " will appear on the desktop. For now, there are three different policies:"
                 " <ul><li><em>Smart</em> will try to achieve a minimum overlap of windows</li>"
                 " <li><em>Cascade</em> will cascade the windows</li>"
                 " <li><em>Random</em> will use a random position</li></ul>") ;

    QWhatsThis::add( plcLabel, wtstr);
    QWhatsThis::add( placementCombo, wtstr);

    plcLabel->setBuddy(placementCombo);
    rLay->addWidget(plcLabel, 0, 0);
    rLay->addWidget(placementCombo, 0, 1);

    bLay->addSpacing(10);

    lay->addWidget(windowsBox);

    //iTLabel = new QLabel(i18n("  Allowed overlap:\n"
    //                         "(% of desktop space)"),
    //             plcBox);
    //iTLabel->setAlignment(AlignTop|AlignHCenter);
    //pLay->addWidget(iTLabel,1,1);

    //interactiveTrigger = new QSpinBox(0, 500, 1, plcBox);
    //pLay->addWidget(interactiveTrigger,1,2);

    //pLay->addRowSpacing(2,KDialog::spacingHint());

    //lay->addWidget(plcBox);

    shBox = new QButtonGroup(i18n("Shading"), this);
    QGridLayout *shLay = new QGridLayout(shBox, 3, 3,
                                         KDialog::marginHint(),
                                         KDialog::spacingHint());

    shLay->addRowSpacing(0,fontMetrics().lineSpacing());
    animateShade = new QCheckBox(i18n("Animate"), shBox);
    QWhatsThis::add(animateShade, i18n("Animate the action of reducing the window to its titlebar (shading)"
                                       " as well as the expansion of a shaded window") );
    shLay->addWidget(animateShade, 1, 0);

    shadeHoverOn = new QCheckBox(i18n("Enable Hover"), shBox);
    shLay->addWidget(shadeHoverOn, 2, 0);

    connect(shadeHoverOn, SIGNAL(toggled(bool)), this, SLOT(shadeHoverChanged(bool)));

    shlabel = new QLabel(i18n("Delay (ms)"), shBox);
    shlabel->setAlignment(AlignVCenter | AlignHCenter);
    shLay->addWidget(shlabel, 3, 0, AlignLeft);

    shadeHover = new KIntNumInput(500, shBox);
    shadeHover->setRange(0, 3000, 100, true);
    shadeHover->setSteps(100, 100);
    shLay->addMultiCellWidget(shadeHover, 3, 3, 1, 2);

    QWhatsThis::add(shadeHoverOn, i18n("If Shade Hover is enabled, a shaded window will un-shade automatically "
                                       "when the mouse pointer has been over the title bar for some time."));

    wtstr = i18n("Sets the time in milliseconds before the window unshades "
                "when the mouse pointer goes over the shaded window.");
    QWhatsThis::add(shadeHover, wtstr);
    QWhatsThis::add(shlabel, wtstr);

    lay->addWidget(shBox);

    // Any changes goes to slotChanged()
    connect(opaque, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect(resizeOpaqueOn, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect(minimizeAnimOn, SIGNAL(clicked() ), SLOT(slotChanged()));
    connect(minimizeAnimSlider, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()));
    connect(moveResizeMaximized, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
    connect(placementCombo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
    connect(animateShade, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
    connect(shadeHoverOn, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
    connect(shadeHover, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()));

    //copied from kcontrol/konq/kwindesktop, aleXXX
//CT disabled <i>sine die</i>
//  ElectricBox = new QVButtonGroup(i18n("Active desktop borders"),
//                 this);
//  ElectricBox->setMargin(15);
//
//  enable= new QCheckBox(i18n("Enable &active desktop borders"), ElectricBox);
//  QWhatsThis::add( enable, i18n("If this option is enabled, moving the mouse to a screen border"
//    " will change your desktop. This is e.g. useful if you want to drag windows from one desktop"
//    " to the other.") );
//
//  movepointer = new QCheckBox(i18n("&Move pointer towards center after switch"),
//                              ElectricBox);
//  QWhatsThis::add( movepointer, i18n("If this option is enabled, after switching desktops using"
//    " the active borders feature the mouse pointer will be moved to the middle of the screen. This"
//    " way you don't repeatedly change desktops because the mouse pointer is still on the border"
//    " of the screen.") );
//
//  delays = new KIntNumInput(10, ElectricBox);
//  delays->setRange(0, MAX_EDGE_RES/10, 10, true);
//  delays->setLabel(i18n("&Desktop switch delay:"));
//  QWhatsThis::add( delays, i18n("Here you can set a delay for switching desktops using the active"
//    " borders feature. Desktops will be switched after the mouse has been touching a screen border"
//    " for the specified number of seconds.") );
//
//  connect( enable, SIGNAL(clicked()), this, SLOT(setEBorders()));
//
//  lay->addWidget(ElectricBox);
//
//  // Electric borders are not in kwin yet => disable controls
//  enable->setEnabled(false);
//  movepointer->setEnabled(false);
//  delays->setEnabled(false);
    //CT 15mar98 - add EdgeResistance, BorderAttractor, WindowsAttractor config
    MagicBox = new QVButtonGroup(i18n("Magic borders"), this);
    MagicBox->setMargin(15);

    BrdrSnap = new KIntNumInput(10, MagicBox);
    BrdrSnap->setRange( 0, MAX_BRDR_SNAP);
    BrdrSnap->setLabel(i18n("&Border snap zone:"));
    BrdrSnap->setSuffix(i18n(" pixels"));
    BrdrSnap->setSteps(1,1);
    QWhatsThis::add( BrdrSnap, i18n("Here you can set the snap zone for screen borders, i.e."
                                    " the 'strength' of the magnetic field which will make windows snap to the border when"
                                    " moved near it.") );

    WndwSnap = new KIntNumInput(10, MagicBox);
    WndwSnap->setRange( 0, MAX_WNDW_SNAP);
    WndwSnap->setLabel(i18n("&Window snap zone:"));
    WndwSnap->setSuffix( i18n(" pixels"));
    BrdrSnap->setSteps(1,1);
    QWhatsThis::add( WndwSnap, i18n("Here you can set the snap zone for windows, i.e."
                                    " the 'strength' of the magnetic field which will make windows snap to each other when"
                                    " they're moved near another window.") );

    OverlapSnap=new QCheckBox(i18n("Snap windows only when &overlapping"),MagicBox);
    QWhatsThis::add( OverlapSnap, i18n("Here you can set that windows will be only"
                                       " snapped if you try to overlap them, i.e. they won't be snapped if the windows"
                                       " comes only near another window or border.") );

    lay->addWidget(MagicBox);
#ifdef HAVE_XINERAMA
    xineramaBox = new QVButtonGroup(i18n("Xinerama"), this);

    xineramaEnable = new QCheckBox(i18n("Enable Xinerama Support"), xineramaBox);
    QWhatsThis::add(xineramaEnable, i18n("Enable support for Xinerama."));
    connect(xineramaEnable, SIGNAL(toggled(bool)), this, SLOT(setXinerama(bool)));
    xineramaMovementEnable = new QCheckBox(i18n("Enable Window Resistance Support"), xineramaBox);
    QWhatsThis::add(xineramaMovementEnable, i18n("Turn on resistance when moving a window from one physical screen to the other."));
    xineramaPlacementEnable = new QCheckBox(i18n("Enable Window Placement Support"), xineramaBox);
    QWhatsThis::add(xineramaPlacementEnable, i18n("This option opens new windows on the physical screen on which the cursor is present."));
    xineramaMaximizeEnable = new QCheckBox(i18n("Enable Window Maximize Support"), xineramaBox);
    QWhatsThis::add(xineramaMaximizeEnable, i18n("When this option is turned on, windows will only maximize up to the physical screen size."));

    lay->addWidget(xineramaBox);
#endif
    lay->addStretch();
    load();

    connect( BrdrSnap, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()));
    connect( WndwSnap, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()));
    connect( OverlapSnap, SIGNAL(clicked()), this, SLOT(slotChanged()));
#ifdef HAVE_XINERAMA
    connect( xineramaEnable, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect( xineramaMovementEnable, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect( xineramaPlacementEnable, SIGNAL(clicked()), this, SLOT(slotChanged()));
    connect( xineramaMaximizeEnable, SIGNAL(clicked()), this, SLOT(slotChanged()));
#endif

    load();
}

// many widgets connect to this slot
void KAdvancedConfig::slotChanged()
{
    emit changed(true);
}

int KAdvancedConfig::getMove()
{
    if (opaque->isChecked())
        return OPAQUE;
    else
        return TRANSPARENT;
}

void KAdvancedConfig::setMove(int trans)
{
    if (trans == TRANSPARENT)
        opaque->setChecked(false);
    else
        opaque->setChecked(true);
}

// placement policy --- CT 31jan98 ---
int KAdvancedConfig::getPlacement()
{
    return placementCombo->currentItem();
}

void KAdvancedConfig::setPlacement(int plac)
{
    placementCombo->setCurrentItem(plac);
}

bool KAdvancedConfig::getMinimizeAnim()
{
    return minimizeAnimOn->isChecked();
}

int KAdvancedConfig::getMinimizeAnimSpeed()
{
    return minimizeAnimSlider->value();
}

void KAdvancedConfig::setMinimizeAnim(bool anim, int speed)
{
    minimizeAnimOn->setChecked( anim );
    minimizeAnimSlider->setValue(speed);
    minimizeAnimSlider->setEnabled( anim );
    minimizeAnimSlowLabel->setEnabled( anim );
    minimizeAnimFastLabel->setEnabled( anim );
}

int KAdvancedConfig::getResizeOpaque()
{
    if (resizeOpaqueOn->isChecked())
        return RESIZE_OPAQUE;
    else
        return RESIZE_TRANSPARENT;
}

void KAdvancedConfig::setResizeOpaque(int opaque)
{
    if (opaque == RESIZE_OPAQUE)
        resizeOpaqueOn->setChecked(true);
    else
        resizeOpaqueOn->setChecked(false);
}

// CT 13mar98 interactiveTrigger configured by this slot
// void KAdvancedConfig::ifPlacementIsInteractive( )
// {
//   if( placementCombo->currentItem() == INTERACTIVE_PLACEMENT) {
//     iTLabel->setEnabled(true);
//     interactiveTrigger->show();
//   }
//   else {
//     iTLabel->setEnabled(false);
//     interactiveTrigger->hide();
//   }
// }
//CT

void KAdvancedConfig::setShadeHover(bool on) {
    shadeHoverOn->setChecked(on);
    shadeHover->setEnabled(on);
    shlabel->setEnabled(on);
}

void KAdvancedConfig::setShadeHoverInterval(int k) {
    shadeHover->setValue(k);
}

int KAdvancedConfig::getShadeHoverInterval() {

    return shadeHover->value();
}

void KAdvancedConfig::shadeHoverChanged(bool a) {
    shadeHover->setEnabled(a);
    shlabel->setEnabled(a);
}

void KAdvancedConfig::setXinerama(bool on) {
#ifdef HAVE_XINERAMA
    if (KApplication::desktop()->isVirtualDesktop())
        xineramaEnable->setChecked(on);
    else
        xineramaEnable->setEnabled(false);

    xineramaMovementEnable->setEnabled(on);
    xineramaPlacementEnable->setEnabled(on);
    xineramaMaximizeEnable->setEnabled(on);
#endif
}

void KAdvancedConfig::setAnimateShade(bool a) {
    animateShade->setChecked(a);
}

void KAdvancedConfig::setMoveResizeMaximized(bool a) {
    moveResizeMaximized->setChecked(a);
}

void KAdvancedConfig::load( void )
{
    QString key;

    config->setGroup( "Windows" );

    key = config->readEntry(KWIN_MOVE, "Opaque");
    if( key == "Transparent")
        setMove(TRANSPARENT);
    else if( key == "Opaque")
        setMove(OPAQUE);

    //CT 17Jun1998 - variable animation speed from 0 (none!!) to 10 (max)
    int anim = 1;
    if (config->hasKey(KWIN_MINIMIZE_ANIM_SPEED)) {
        anim = config->readNumEntry(KWIN_MINIMIZE_ANIM_SPEED);
        if( anim < 1 ) anim = 0;
        if( anim > 10 ) anim = 10;
        setMinimizeAnim( config->readBoolEntry(KWIN_MINIMIZE_ANIM, true ), anim );
    }
    else{
        config->writeEntry(KWIN_MINIMIZE_ANIM, true );
        config->writeEntry(KWIN_MINIMIZE_ANIM_SPEED, 5);
        setMinimizeAnim(true, 5);
    }

    // DF: please keep the default consistent with kwin (options.cpp line 145)
    key = config->readEntry(KWIN_RESIZE_OPAQUE, "Opaque");
    if( key == "Opaque")
        setResizeOpaque(RESIZE_OPAQUE);
    else if ( key == "Transparent")
        setResizeOpaque(RESIZE_TRANSPARENT);

    // placement policy --- CT 19jan98 ---
    key = config->readEntry(KWIN_PLACEMENT);
    //CT 13mar98 interactive placement
//   if( key.left(11) == "interactive") {
//     setPlacement(INTERACTIVE_PLACEMENT);
//     int comma_pos = key.find(',');
//     if (comma_pos < 0)
//       interactiveTrigger->setValue(0);
//     else
//       interactiveTrigger->setValue (key.right(key.length()
//                           - comma_pos).toUInt(0));
//     iTLabel->setEnabled(true);
//     interactiveTrigger->show();
//   }
//   else {
//     interactiveTrigger->setValue(0);
//     iTLabel->setEnabled(false);
//     interactiveTrigger->hide();
    if( key == "Random")
        setPlacement(RANDOM_PLACEMENT);
    else if( key == "Cascade")
        setPlacement(CASCADE_PLACEMENT); //CT 31jan98
    //CT 31mar98 manual placement
    else if( key == "manual")
        setPlacement(MANUAL_PLACEMENT);

    else
        setPlacement(SMART_PLACEMENT);
//  }

    setAnimateShade(config->readBoolEntry(KWIN_ANIMSHADE, true));

    setMoveResizeMaximized(config->readBoolEntry(KWIN_MOVE_RESIZE_MAXIMIZED, true));

    key = config->readEntry(KWIN_SHADEHOVER, "off");
    setShadeHover(key == "on");

    int k = config->readNumEntry(KWIN_SHADEHOVER_INTERVAL, 250);
    setShadeHoverInterval(k);

#ifdef HAVE_XINERAMA
    key = config->readEntry(KWIN_XINERAMA, "off");
    setXinerama(key == "on");

    key = config->readEntry(KWIN_XINERAMA_MOVEMENT, "off");
    xineramaMovementEnable->setChecked(key == "on");

    key = config->readEntry(KWIN_XINERAMA_PLACEMENT, "off");
    xineramaPlacementEnable->setChecked(key == "on");

    key = config->readEntry(KWIN_XINERAMA_MAXIMIZE, "off");
    xineramaMaximizeEnable->setChecked(key == "on");
#endif

    //copied from kcontrol/konq/kwindesktop, aleXXX
    int v;
    config->setGroup( "Windows" );

/* Electric borders are not in kwin yet (?)
  v = config->readNumEntry(KWM_ELECTRIC_BORDER);
  setElectricBorders(v != -1);

  v = config->readNumEntry(KWM_ELECTRIC_BORDER_DELAY);
  setElectricBordersDelay(v);

  //CT 17mar98 re-allign this reading with the one in kwm  ("on"/"off")
  // matthias: this is obsolete now. Should be fixed in 1.1 with NoWarp, MiddleWarp, FullWarp
  key = config->readEntry(KWM_ELECTRIC_BORDER_MOVE_POINTER);
  if (key == "MiddleWarp")
    setElectricBordersMovePointer(TRUE);
*/
    //CT 15mar98 - magics
    v = config->readNumEntry(KWM_BRDR_SNAP_ZONE, KWM_BRDR_SNAP_ZONE_DEFAULT);
    if (v > MAX_BRDR_SNAP) setBorderSnapZone(MAX_BRDR_SNAP);
    else if (v < 0) setBorderSnapZone (0);
    else setBorderSnapZone(v);

    v = config->readNumEntry(KWM_WNDW_SNAP_ZONE, KWM_WNDW_SNAP_ZONE_DEFAULT);
    if (v > MAX_WNDW_SNAP) setWindowSnapZone(MAX_WNDW_SNAP);
    else if (v < 0) setWindowSnapZone (0);
    else setWindowSnapZone(v);
    //CT ---

    OverlapSnap->setChecked(config->readBoolEntry("SnapOnlyWhenOverlapping",false));

}

void KAdvancedConfig::save( void )
{
    int v;

    config->setGroup( "Windows" );

    v = getMove();
    if (v == TRANSPARENT)
        config->writeEntry(KWIN_MOVE,"Transparent");
    else
        config->writeEntry(KWIN_MOVE,"Opaque");


    // placement policy --- CT 31jan98 ---
    v =getPlacement();
    if (v == RANDOM_PLACEMENT)
        config->writeEntry(KWIN_PLACEMENT, "Random");
    else if (v == CASCADE_PLACEMENT)
        config->writeEntry(KWIN_PLACEMENT, "Cascade");
//CT 13mar98 manual and interactive placement
//   else if (v == MANUAL_PLACEMENT)
//     config->writeEntry(KWIN_PLACEMENT, "Manual");
//   else if (v == INTERACTIVE_PLACEMENT) {
//       QString tmpstr = QString("Interactive,%1").arg(interactiveTrigger->value());
//       config->writeEntry(KWIN_PLACEMENT, tmpstr);
//   }
    else
        config->writeEntry(KWIN_PLACEMENT, "Smart");

//CT - 17Jun1998
    config->writeEntry(KWIN_MINIMIZE_ANIM, getMinimizeAnim());
    config->writeEntry(KWIN_MINIMIZE_ANIM_SPEED, getMinimizeAnimSpeed());

    if ( getMinimizeAnim() > 0 )
        config->writeEntry("AnimateMinimize", true );


    v = getResizeOpaque();
    if (v == RESIZE_OPAQUE)
        config->writeEntry(KWIN_RESIZE_OPAQUE, "Opaque");
    else
        config->writeEntry(KWIN_RESIZE_OPAQUE, "Transparent");

    config->writeEntry(KWIN_ANIMSHADE, animateShade->isChecked());

    config->writeEntry(KWIN_MOVE_RESIZE_MAXIMIZED, moveResizeMaximized->isChecked());

    if (shadeHoverOn->isChecked())
        config->writeEntry(KWIN_SHADEHOVER, "on");
    else
        config->writeEntry(KWIN_SHADEHOVER, "off");

    v = getShadeHoverInterval();
    if (v<0) v = 0;
    config->writeEntry(KWIN_SHADEHOVER_INTERVAL, v);

#ifdef HAVE_XINERAMA
    if (xineramaEnable->isChecked())
        config->writeEntry(KWIN_XINERAMA, "on");
    else
        config->writeEntry(KWIN_XINERAMA, "off");

    if (xineramaMovementEnable->isChecked())
        config->writeEntry(KWIN_XINERAMA_MOVEMENT, "on");
    else
        config->writeEntry(KWIN_XINERAMA_MOVEMENT, "off");

    if (xineramaPlacementEnable->isChecked())
        config->writeEntry(KWIN_XINERAMA_PLACEMENT, "on");
    else
        config->writeEntry(KWIN_XINERAMA_PLACEMENT, "off");

    if (xineramaMaximizeEnable->isChecked())
        config->writeEntry(KWIN_XINERAMA_MAXIMIZE, "on");
    else
        config->writeEntry(KWIN_XINERAMA_MAXIMIZE, "off");
#endif

  //copied from kcontrol/konq/kwindesktop, aleXXX
  config->setGroup( "Windows" );

/* Electric borders are not in kwin yet
  int v = getElectricBordersDelay()>10?80*getElectricBordersDelay():800;
  if (getElectricBorders())
    config->writeEntry(KWM_ELECTRIC_BORDER,v);
  else
    config->writeEntry(KWM_ELECTRIC_BORDER,-1);


  config->writeEntry(KWM_ELECTRIC_BORDER_DELAY,getElectricBordersDelay());

  bv = getElectricBordersMovePointer();
  config->writeEntry(KWM_ELECTRIC_BORDER_MOVE_POINTER,bv?"MiddleWarp":"NoWarp");
*/

  //CT 15mar98 - magics
  config->writeEntry(KWM_BRDR_SNAP_ZONE,getBorderSnapZone());

  config->writeEntry(KWM_WNDW_SNAP_ZONE,getWindowSnapZone());

  config->writeEntry("SnapOnlyWhenOverlapping",OverlapSnap->isChecked());

}

void KAdvancedConfig::defaults()
{
    setMove(OPAQUE);
    setResizeOpaque(RESIZE_TRANSPARENT);
    setPlacement(SMART_PLACEMENT);
    setMoveResizeMaximized(true);

    //copied from kcontrol/konq/kwindesktop, aleXXX
    setWindowSnapZone(KWM_WNDW_SNAP_ZONE_DEFAULT);
    setBorderSnapZone(KWM_BRDR_SNAP_ZONE_DEFAULT);
    OverlapSnap->setChecked(false);

}

void KAdvancedConfig::setEBorders()
{
    delays->setEnabled(enable->isChecked());
    movepointer->setEnabled(enable->isChecked());
}

bool KAdvancedConfig::getElectricBorders()
{
    return  enable->isChecked();
}

int KAdvancedConfig::getElectricBordersDelay()
{
    return delays->value();
}

bool KAdvancedConfig::getElectricBordersMovePointer()
{
    return movepointer->isChecked();
}

void KAdvancedConfig::setElectricBordersMovePointer(bool move){

  if(move){
    movepointer->setEnabled(true);
    movepointer->setChecked(true);
  }
  else{
    movepointer->setEnabled(false);
    movepointer->setChecked(false);
  }

  movepointer->setEnabled(enable->isChecked());

}

void KAdvancedConfig::setElectricBorders(bool b){
    enable->setChecked(b);
    setEBorders();
}

void KAdvancedConfig::setElectricBordersDelay(int delay)
{
    delays->setValue(delay);
}


int KAdvancedConfig::getBorderSnapZone() {
  return BrdrSnap->value();
}

void KAdvancedConfig::setBorderSnapZone(int pxls) {
  BrdrSnap->setValue(pxls);
}

int KAdvancedConfig::getWindowSnapZone() {
  return WndwSnap->value();
}

void KAdvancedConfig::setWindowSnapZone(int pxls) {
  WndwSnap->setValue(pxls);
}



#include "windows.moc"
