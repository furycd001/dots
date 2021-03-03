/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "mainwindow.h"
#include "defs.h"
#include "formwindow.h"
#include "widgetdatabase.h"
#include "widgetfactory.h"
#include "propertyeditor.h"
#include "qmetaobject.h"
#include "qaction.h"
#include "metadatabase.h"
#include "resource.h"
#include "pixmapchooser.h"
#include "config.h"
#include "hierarchyview.h"
#include "editslotsimpl.h"
#include "newformimpl.h"
#include "formlist.h"
#include "help.h"
#include "connectionviewerimpl.h"
#include "customwidgeteditorimpl.h"
#include "preferences.h"
#include "styledbutton.h"
#include "formsettingsimpl.h"
#include "about.h"
#include "import.h"
#include "multilineeditorimpl.h"
#include "createtemplate.h"

#include <qinputdialog.h>
#if defined(HAVE_KDE)
#include <ktoolbar.h>
#include <kmenubar.h>
#else
#include <qtoolbar.h>
#include <qmenubar.h>
#endif
#include <qpixmap.h>
#include <qbuttongroup.h>
#include <qapplication.h>
#include <qworkspace.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qbuffer.h>
#include <qdir.h>
#include <qstyle.h>
#include <qmotifstyle.h>
#include <qcdestyle.h>
#include <qplatinumstyle.h>
#include <qwindowsstyle.h>
#include <qsgistyle.h>
#include <qmotifplusstyle.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstatusbar.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qtoolbutton.h>
#include <qobjectlist.h>
#include <qurl.h>
#include <qwhatsthis.h>
#include <qwizard.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <stdlib.h>

static int forms = 0;

static const char * whatsthis_image[] = {
    "16 16 3 1",
    " 	c None",
    "o	c #000000",
    "a	c #000080",
    "o        aaaaa  ",
    "oo      aaa aaa ",
    "ooo    aaa   aaa",
    "oooo   aa     aa",
    "ooooo  aa     aa",
    "oooooo  a    aaa",
    "ooooooo     aaa ",
    "oooooooo   aaa  ",
    "ooooooooo aaa   ",
    "ooooo     aaa   ",
    "oo ooo          ",
    "o  ooo    aaa   ",
    "    ooo   aaa   ",
    "    ooo         ",
    "     ooo        ",
    "     ooo        "};

const char toolbarHelp[] = "<p>Toolbars contain a number of buttons to "
"provide quick access to often used functions.%1"
"<br>Click on the toolbar handle to hide the toolbar, "
"or drag and place the toolbar to a different location.</p>";

MainWindow *MainWindow::self = 0;

MainWindow::MainWindow( bool asClient )
#if defined(HAVE_KDE)
    : KMainWindow( 0, "mainwindow", WType_TopLevel | WDestructiveClose ),
#else
    : QMainWindow( 0, "mainwindow", WType_TopLevel | WDestructiveClose ),
#endif
      grd( 10, 10 ), sGrid( TRUE ), snGrid( TRUE ), restoreConfig( TRUE ), splashScreen( TRUE ),
      docPath( "$QTDIR/doc/html" ), client( asClient )
{
    self = this;
    setIcon( PixmapChooser::loadPixmap( "logo" ) );

    actionGroupTools = 0;
    prefDia = 0;
    windowMenu = 0;
    actionWindowPropertyEditor = 0;
    hierarchyView = 0;

    statusBar()->clear();
#if defined(QT_NON_COMMERCIAL)
    statusBar()->addWidget( new QLabel(tr("Ready - This is the non-commercial version of Qt - For commercial evaluation purposes, use the help menu to register with Trolltech."), 
	statusBar()), 1 );
#else
    statusBar()->addWidget( new QLabel("Ready", statusBar()), 1 );
#endif

    setupMDI();
    setupMenuBar();

    setupFileActions();
    setupEditActions();
#if defined(HAVE_KDE)
    layoutToolBar = new KToolBar( this );
    ( (KToolBar*)layoutToolBar )->setFullSize( FALSE );
#else
    layoutToolBar = new QToolBar( this );
#endif
    addToolBar( layoutToolBar, tr( "Layout" ) );
    setupToolActions();
    setupLayoutActions();
    setupPreviewActions();
    setupWindowActions();
    setupHelpActions();
    setupRMBMenus();

    emit hasActiveForm( FALSE );

    lastPressWidget = 0;
    qApp->installEventFilter( this );

    setupPropertyEditor();
    setupHierarchyView();
    setupFormList();

    QSize as( qApp->desktop()->size() );
    as -= QSize( 30, 30 );
    resize( QSize( 1000, 800 ).boundedTo( as ) );

    connect( qApp->clipboard(), SIGNAL( dataChanged() ),
	     this, SLOT( clipboardChanged() ) );
    clipboardChanged();
    layoutChilds = FALSE;
    layoutSelected = FALSE;
    breakLayout = FALSE;
    backPix = TRUE;

    readConfig();

    // hack to make WidgetFactory happy (so it knows QWidget and QDialog for resetting properties)
    QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWidget" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QDialog" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QLabel" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QTabWidget" ), this, 0, FALSE );
    delete w;
    w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QFrame" ), this, 0, FALSE );
    delete w;

    statusBar()->setSizeGripEnabled( TRUE );
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMDI()
{
    QVBox *vbox = new QVBox( this );
    setCentralWidget( vbox );
    vbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    vbox->setMargin( 1 );
    vbox->setLineWidth( 1 );
    workspace = new QWorkspace( vbox );
    workspace->setBackgroundMode( PaletteDark );
    workspace->setBackgroundPixmap( PixmapChooser::loadPixmap( "background.png", PixmapChooser::NoSize ) );
    connect( workspace, SIGNAL( windowActivated( QWidget * ) ),
	     this, SLOT( activeWindowChanged( QWidget * ) ) );
    lastActiveFormWindow = 0;
    workspace->setAcceptDrops( TRUE );
}

void MainWindow::setupMenuBar()
{
    menubar = menuBar();
}

static QIconSet createIconSet( const QString &name )
{
    QIconSet ic( PixmapChooser::loadPixmap( name, PixmapChooser::Small ) );
    ic.setPixmap( PixmapChooser::loadPixmap( name, PixmapChooser::Disabled ), QIconSet::Small, QIconSet::Disabled );
    return ic;
}

void MainWindow::setupEditActions()
{
    actionEditUndo = new QAction( tr("Undo"), createIconSet( "undo.xpm" ),tr("&Undo: Not Available"), CTRL + Key_Z, this, 0 );
    actionEditUndo->setStatusTip( tr( "Reverses the last action" ) );
    actionEditUndo->setWhatsThis( tr( "Reverses the last action" ) );
    connect( actionEditUndo, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    actionEditUndo->setEnabled( FALSE );

    actionEditRedo = new QAction( tr( "Redo" ), createIconSet("redo.xpm"), tr( "&Redo: Not Available" ), CTRL + Key_Y, this, 0 );
    actionEditRedo->setStatusTip( tr( "Redoes the last undone operation") );
    actionEditRedo->setWhatsThis( tr("Redoes the last undone operation") );
    connect( actionEditRedo, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    actionEditRedo->setEnabled( FALSE );

    actionEditCut = new QAction( tr( "Cut" ), createIconSet("editcut.xpm"), tr( "Cu&t" ), CTRL + Key_X, this, 0 );
    actionEditCut->setStatusTip( tr( "Cuts the selected widgets and puts them on the clipboard" ) );
    actionEditCut->setWhatsThis( tr( "Cuts the selected widgets and puts them on the clipboard" ) );
    connect( actionEditCut, SIGNAL( activated() ), this, SLOT( editCut() ) );
    actionEditCut->setEnabled( FALSE );

    actionEditCopy = new QAction( tr( "Copy" ), createIconSet("editcopy.xpm"), tr( "&Copy" ), CTRL + Key_C, this, 0 );
    actionEditCopy->setStatusTip( tr( "Copies the selected widgets to the clipboard" ) );
    actionEditCopy->setWhatsThis( tr( "Copies the selected widgets to the clipboard" ) );
    connect( actionEditCopy, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    actionEditCopy->setEnabled( FALSE );

    actionEditPaste = new QAction( tr( "Paste" ), createIconSet("editpaste.xpm"), tr( "&Paste" ), CTRL + Key_V, this, 0 );
    actionEditPaste->setStatusTip( tr( "Pastes clipboard contents" ) );
    actionEditPaste->setWhatsThis( tr( "Pastes the widgets on the clipboard into the formwindow" ) );
    connect( actionEditPaste, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    actionEditPaste->setEnabled( FALSE );

    actionEditDelete = new QAction( tr( "Delete" ), QPixmap(), tr( "&Delete" ), Key_Delete, this, 0 );
    actionEditDelete->setStatusTip( tr( "Deletes the selected widgets" ) );
    actionEditDelete->setWhatsThis( tr( "Deletes the selected widgets" ) );
    connect( actionEditDelete, SIGNAL( activated() ), this, SLOT( editDelete() ) );
    actionEditDelete->setEnabled( FALSE );

    actionEditSelectAll = new QAction( tr( "Select All" ), QPixmap(), tr( "Select &All" ), CTRL + Key_A, this, 0 );
    actionEditSelectAll->setStatusTip( tr( "Selects all widgets" ) );
    actionEditSelectAll->setWhatsThis( tr( "Selects all widgets in the current form" ) );
    connect( actionEditSelectAll, SIGNAL( activated() ), this, SLOT( editSelectAll() ) );
    actionEditSelectAll->setEnabled( TRUE );

    actionEditRaise = new QAction( tr( "Bring to Front" ), createIconSet("editraise.xpm"), tr( "Bring to &Front" ), 0, this, 0 );
    actionEditRaise->setStatusTip( tr( "Raises the selected widgets" ) );
    actionEditRaise->setWhatsThis( tr( "Raises the selected widgets" ) );
    connect( actionEditRaise, SIGNAL( activated() ), this, SLOT( editRaise() ) );
    actionEditRaise->setEnabled( FALSE );

    actionEditLower = new QAction( tr( "Send to Back" ), createIconSet("editlower.xpm"), tr( "Send to &Back" ), 0, this, 0 );
    actionEditLower->setStatusTip( tr( "Lowers the selected widgets" ) );
    actionEditLower->setWhatsThis( tr( "Lowers the selected widgets" ) );
    connect( actionEditLower, SIGNAL( activated() ), this, SLOT( editLower() ) );
    actionEditLower->setEnabled( FALSE );

    actionEditAccels = new QAction( tr( "Check Accelerators" ), QPixmap(),
				    tr( "Check Accele&rators" ), CTRL + Key_R, this, 0 );
    actionEditAccels->setStatusTip( tr("Checks if the accelerators used in the form are unique") );
    actionEditAccels->setWhatsThis( tr("<b>Check Accelerators</b>"
				       "<p>Checks if the accelerators used in the form are unique. If this "
				       "is not the case, the desiner helps you to fix that problem.</p>") );
    connect( actionEditAccels, SIGNAL( activated() ), this, SLOT( editAccels() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditAccels, SLOT( setEnabled(bool) ) );


    actionEditSlots = new QAction( tr( "Slots" ), createIconSet("editslots.xpm"),
				   tr( "S&lots..." ), 0, this, 0 );
    actionEditSlots->setStatusTip( tr("Opens a dialog to edit slots") );
    actionEditSlots->setWhatsThis( tr("<b>Edit slots</b>"
				      "<p>Opens a dialog where slots of the current form can be added and changed. "
				      "The slots will be virtual in the generated C++ source, and you may wish to "
				      "reimplement them in subclasses.</p>") );
    connect( actionEditSlots, SIGNAL( activated() ), this, SLOT( editSlots() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditSlots, SLOT( setEnabled(bool) ) );

    actionEditConnections = new QAction( tr( "Connections" ), createIconSet("connecttool.xpm"),
					 tr( "Co&nnections..." ), 0, this, 0 );
    actionEditConnections->setStatusTip( tr("Opens a dialog to edit connections") );
    actionEditConnections->setWhatsThis( tr("<b>Edit connections</b>"
					    "<p>Opens a dialog where the connections of the current form can be "
					    "changed.</p>") );
    connect( actionEditConnections, SIGNAL( activated() ), this, SLOT( editConnections() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditConnections, SLOT( setEnabled(bool) ) );

    actionEditFormSettings = new QAction( tr( "Form Settings" ), QPixmap(),
					  tr( "&Form Settings..." ), 0, this, 0 );
    actionEditFormSettings->setStatusTip( tr("Opens a dialog to change the settings of the form") );
    actionEditFormSettings->setWhatsThis( tr("<b>Edit settings of the form</b>"
					     "<p>Opens a dialog to change the classname and add comments to the current formwindow.</p>") );
    connect( actionEditFormSettings, SIGNAL( activated() ), this, SLOT( editFormSettings() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), actionEditFormSettings, SLOT( setEnabled(bool) ) );

    actionEditPreferences = new QAction( tr( "Preferences" ), QPixmap(),
					 tr( "P&references..." ), 0, this, 0 );
    actionEditPreferences->setStatusTip( tr("Opens a dialog to change preferences") );
    actionEditPreferences->setWhatsThis( tr("<b>Change preferences</b>"
					    "<p>The settings will be saved on exit. They will be restored "
					    "the next time the Designer starts if \"Restore last Workspace\" "
					    "has been selected.</p>") );
    connect( actionEditPreferences, SIGNAL( activated() ), this, SLOT( editPreferences() ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this );
#endif
    QWhatsThis::add( tb, tr( "<b>The Edit toolbar</b>%1").arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "Edit" ) );
    actionEditUndo->addTo( tb );
    actionEditRedo->addTo( tb );
    tb->addSeparator();
    actionEditCut->addTo( tb );
    actionEditCopy->addTo( tb );
    actionEditPaste->addTo( tb );
#if 0
    tb->addSeparator();
    actionEditLower->addTo( tb );
    actionEditRaise->addTo( tb );
#endif

    QPopupMenu *menu = new QPopupMenu( this );
    menubar->insertItem( tr( "&Edit" ), menu );
    actionEditUndo->addTo( menu );
    actionEditRedo->addTo( menu );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );
    actionEditSelectAll->addTo( menu );
    actionEditAccels->addTo( menu );
#if 0
    menu->insertSeparator();
    actionEditLower->addTo( menu );
    actionEditRaise->addTo( menu );
#endif
    menu->insertSeparator();
    actionEditSlots->addTo( menu );
    actionEditConnections->addTo( menu );
    actionEditFormSettings->addTo( menu );
    menu->insertSeparator();
    actionEditPreferences->addTo( menu );
}

void MainWindow::setupLayoutActions()
{
    if ( !actionGroupTools ) {
	actionGroupTools = new QActionGroup( this );
	actionGroupTools->setExclusive( TRUE );
	connect( actionGroupTools, SIGNAL( selected(QAction*) ), this, SLOT( toolSelected(QAction*) ) );
    }

    actionEditAdjustSize = new QAction( tr( "Adjust Size" ), createIconSet("adjustsize.xpm"),
					tr( "Adjust &Size" ), CTRL + Key_J, this, 0 );
    actionEditAdjustSize->setStatusTip(tr("Adjusts the size of the selected widget") );
    actionEditAdjustSize->setWhatsThis(tr("<b>Adjust the size</b>"
					  "<p>Calculates an appropriate size for the selected widget. This function "
					  "is disabled if the widget is part of a layout, and the layout will "
					  "control the widget\'s geometry.</p>") );
    connect( actionEditAdjustSize, SIGNAL( activated() ), this, SLOT( editAdjustSize() ) );
    actionEditAdjustSize->setEnabled( FALSE );

    actionEditHLayout = new QAction( tr( "Lay Out Horizontally" ), createIconSet("edithlayout.xpm"),
				     tr( "Lay Out &Horizontally" ), CTRL + Key_H, this, 0 );
    actionEditHLayout->setStatusTip(tr("Lays out the selected widgets horizontally") );
    actionEditHLayout->setWhatsThis(tr("<b>Layout widgets horizontally</b>"
				       "<p>The selected widgets will be laid out horizontally. "
				       "If only one widget is selected, its child-widgets will be laid out.</p>") );
    connect( actionEditHLayout, SIGNAL( activated() ), this, SLOT( editLayoutHorizontal() ) );
    actionEditHLayout->setEnabled( FALSE );

    actionEditVLayout = new QAction( tr( "Lay Out Vertically" ), createIconSet("editvlayout.xpm"),
				     tr( "Lay Out &Vertically" ), CTRL + Key_L, this, 0 );
    actionEditVLayout->setStatusTip(tr("Lays out the selected widgets vertically") );
    actionEditVLayout->setWhatsThis(tr("<b>Layout widgets vertically</b>"
				       "<p>The selected widgets will be laid out vertically. "
				       "If only one widget is selected, its child-widgets will be laid out.</p>") );
    connect( actionEditVLayout, SIGNAL( activated() ), this, SLOT( editLayoutVertical() ) );
    actionEditVLayout->setEnabled( FALSE );

    actionEditGridLayout = new QAction( tr( "Lay Out in a Grid" ), createIconSet("editgrid.xpm"),
					tr( "Lay Out in a &Grid" ), CTRL + Key_G, this, 0 );
    actionEditGridLayout->setStatusTip(tr("Lays out the selected widgets in a grid") );
    actionEditGridLayout->setWhatsThis(tr("<b>Layout widgets in a grid</b>"
					  "<p>The selected widgets will be laid out in a grid."
					  "If only one widget is selected, its child-widgets will be laid out.</p>") );
    connect( actionEditGridLayout, SIGNAL( activated() ), this, SLOT( editLayoutGrid() ) );
    actionEditGridLayout->setEnabled( FALSE );

    actionEditBreakLayout = new QAction( tr( "Break Layout" ), createIconSet("editbreaklayout.xpm"),
					 tr( "&Break Layout" ), CTRL + Key_B, this, 0 );
    actionEditBreakLayout->setStatusTip(tr("Breaks the selected layout") );
    actionEditBreakLayout->setWhatsThis(tr("<b>Break the layout</b>"
					   "<p>The selected layout or the layout of the selected widget "
					   "will be removed.</p>") );
    connect( actionEditBreakLayout, SIGNAL( activated() ), this, SLOT( editBreakLayout() ) );

    int id = WidgetDatabase::idFromClassName( "Spacer" );
    QAction* a = new QAction( actionGroupTools, QString::number( id ).latin1() );
    a->setToggleAction( TRUE );
    a->setText( WidgetDatabase::className( id ) );
    a->setMenuText( tr( "Add ") + WidgetDatabase::className( id ) );
    a->setIconSet( WidgetDatabase::iconSet( id ) );
    a->setToolTip( WidgetDatabase::toolTip( id ) );
    a->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::toolTip( id )) );
    a->setWhatsThis( QString("<b>A %1</b><p>%2</p>"
			     "<p>Click to insert a single %3,"
			     "or double click to keep the tool selected.")
	.arg(WidgetDatabase::toolTip( id ))
	.arg(WidgetDatabase::whatsThis( id ))
	.arg(WidgetDatabase::toolTip( id ) ));

    QWhatsThis::add( layoutToolBar, tr( "<b>The Layout toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    actionEditAdjustSize->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    actionEditHLayout->addTo( layoutToolBar );
    actionEditVLayout->addTo( layoutToolBar );
    actionEditGridLayout->addTo( layoutToolBar );
    actionEditBreakLayout->addTo( layoutToolBar );
    layoutToolBar->addSeparator();
    a->addTo( layoutToolBar );

    QPopupMenu *menu = new QPopupMenu( this );
    menubar->insertItem( tr( "&Layout" ), menu );
    actionEditAdjustSize->addTo( menu );
    menu->insertSeparator();
    actionEditHLayout->addTo( menu );
    actionEditVLayout->addTo( menu );
    actionEditGridLayout->addTo( menu );
    actionEditBreakLayout->addTo( menu );
    menu->insertSeparator();
    a->addTo( menu );
}

void MainWindow::setupToolActions()
{
    if ( !actionGroupTools ) {
	actionGroupTools = new QActionGroup( this );
	actionGroupTools->setExclusive( TRUE );
	connect( actionGroupTools, SIGNAL( selected(QAction*) ), this, SLOT( toolSelected(QAction*) ) );
    }

    actionPointerTool = new QAction( tr("Pointer"), createIconSet("pointer.xpm"), tr("&Pointer"),  Key_F2,
				     actionGroupTools, QString::number(POINTER_TOOL).latin1(), TRUE );
    actionPointerTool->setStatusTip( tr("Selects the pointer tool") );
    actionPointerTool->setWhatsThis( tr("<b>The pointer tool</b>"
					"<p>The default tool used to select and move widgets on your form. "
					"For some widgets, a double-click opens a dialog where you can enter "
					"the value for the basic property. A context menu with often used "
					"commands is available for all form elements.</p>") );

    actionConnectTool = new QAction( tr("Connect Signal/Slots"), createIconSet("connecttool.xpm"),
				     tr("&Connect Signal/Slots"),  Key_F3,
				     actionGroupTools, QString::number(CONNECT_TOOL).latin1(), TRUE );
    actionConnectTool->setStatusTip( tr("Selects the connection tool") );
    actionConnectTool->setWhatsThis( tr("<b>Connect signals and slots</b>"
					"<p>Create a connection by dragging with the LMB from the widget "
					"emitting a signal to the receiver, and connect the signal and slot "
					"in the opening dialog.</p>"
					"<p>Double click on this tool to keep it selected.</p>") );

    actionOrderTool = new QAction( tr("Tab Order"), createIconSet("ordertool.xpm"),
				   tr("Tab &Order"),  Key_F4,
				   actionGroupTools, QString::number(ORDER_TOOL).latin1(), TRUE );
    actionOrderTool->setStatusTip( tr("Selects the tab order tool") );
    actionOrderTool->setWhatsThis( tr("<b>Change the tab order</b>"
				      "<p>Click on one widget after the other to change the order in which "
				      "they receive the keyboard focus. A double-click on an item will make "
				      "it the first item in the chain and restart the ordering.</p>") );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this );
#endif
    QWhatsThis::add( tb, tr( "<b>The Tools toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
					
    addToolBar( tb, tr( "Tools" ), QMainWindow::Top, TRUE );
    actionPointerTool->addTo( tb );
    actionConnectTool->addTo( tb );
    actionOrderTool->addTo( tb );

    QPopupMenu *mmenu = new QPopupMenu( this );
    menubar->insertItem( tr( "&Tools" ), mmenu );
    actionPointerTool->addTo( mmenu );
    actionConnectTool->addTo( mmenu );
    actionOrderTool->addTo( mmenu );
    mmenu->insertSeparator();

    customWidgetToolBar = 0;
    customWidgetMenu = 0;

    actionToolsCustomWidget = new QAction( tr("Custom Widgets"),
					   createIconSet( "customwidget.xpm" ), tr("Edit &Custom Widgets..."), 0, this, 0 );
    actionToolsCustomWidget->setStatusTip( tr("Opens a dialog to change the custom widgets") );
    actionToolsCustomWidget->setWhatsThis( tr("<b>Change custom widgets</b>"
					      "<p>You can add your own widgets into forms by providing classname "
					      "and name of the header file. You can add properties as well as "
					      "signals and slots to integrate them into the designer, "
					      "and provide a pixmap which will be used to represent the widget on the form.</p>") );

    connect( actionToolsCustomWidget, SIGNAL( activated() ), this, SLOT( toolsCustomWidget() ) );

    for ( int j = 0; j < WidgetDatabase::numWidgetGroups(); ++j ) {
	QString grp = WidgetDatabase::widgetGroup( j );
	if ( !WidgetDatabase::isGroupVisible( grp ) )
	    continue;
#if defined(HAVE_KDE)
	KToolBar *tb = new KToolBar( this );
	tb->setFullSize( FALSE );
#else
	QToolBar *tb = new QToolBar( this );
#endif
	bool plural = grp[(int)grp.length()-1] == 's';
	if ( plural ) {
	    QWhatsThis::add( tb, tr( "<b>The %1</b>%2" ).arg(grp).arg(tr(toolbarHelp).
						arg( tr(" Click on a button to insert a single widget, "
						"or double click to insert multiple %1.") ).arg(grp)) );
	} else {
	    QWhatsThis::add( tb, tr( "<b>The %1 Widgets</b>%2" ).arg(grp).arg(tr(toolbarHelp).
						arg( tr(" Click on a button to insert a single %1 widget, "
						"or double click to insert multiple widgets.") ).arg(grp)) );
	}
	addToolBar( tb, grp );
	QPopupMenu *menu = new QPopupMenu( this );
	mmenu->insertItem( grp, menu );
	
	if ( grp == "Custom" ) {
	    if ( !customWidgetMenu )
		actionToolsCustomWidget->addTo( menu );
	    else
		menu->insertSeparator();
	    customWidgetMenu = menu;
	    customWidgetToolBar = tb;
	}

	for ( int i = 0; i < WidgetDatabase::count(); ++i ) {
	    if ( WidgetDatabase::group( i ) != grp )
		continue; // only widgets, i.e. not forms and temp stuff
	    QAction* a = new QAction( actionGroupTools, QString::number( i ).latin1() );
	    a->setToggleAction( TRUE );
	    if ( WidgetDatabase::className( i )[0] == 'Q' )
		a->setText( WidgetDatabase::className( i ).mid(1) );
	    else
		a->setText( WidgetDatabase::className( i ) );
	    QString ttip = WidgetDatabase::toolTip( i );
	    a->setIconSet( WidgetDatabase::iconSet( i ) );
	    a->setToolTip( ttip );
	    if ( !WidgetDatabase::isWhatsThisLoaded() )
		WidgetDatabase::loadWhatsThis( documentationPath() );
	    a->setStatusTip( tr( "Insert a %1").arg(WidgetDatabase::className( i )) );
	
	    QString whats = QString("<b>A %1</b>").arg( WidgetDatabase::className( i ) );
	    if ( !WidgetDatabase::whatsThis( i ).isEmpty() )
	    whats += QString("<p>%1</p>").arg(WidgetDatabase::whatsThis( i ));
	    a->setWhatsThis( whats + tr("<p>Double click on this tool to keep it selected.</p>") );
	
	    if ( grp != "KDE" )
		a->addTo( tb );
	    a->addTo( menu );
	}
    }

    if ( !customWidgetToolBar ) {
#if defined(HAVE_KDE)
	KToolBar *tb = new KToolBar( this );
	tb->setFullSize( FALSE );
#else
	QToolBar *tb = new QToolBar( this );
#endif
	QWhatsThis::add( tb, tr( "<b>The Custom Widgets toolbar</b>%1"
				 "<p>Select <b>Edit Custom Widgets...</b> in the <b>Tools->Custom</b> menu to "
				 "add and change custom widgets</p>" ).arg(tr(toolbarHelp).
				 arg( tr(" Click on the buttons to insert a single widget, "
				 "or double click to insert multiple widgets.") )) );
	addToolBar( tb, "Custom" );
	customWidgetToolBar = tb;
	QPopupMenu *menu = new QPopupMenu( this );
	mmenu->insertItem( "Custom", menu );
	customWidgetMenu = menu;
	customWidgetToolBar->hide();
	actionToolsCustomWidget->addTo( customWidgetMenu );
	customWidgetMenu->insertSeparator();
    }

    resetTool();
}

void MainWindow::setupFileActions()
{
#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this );
#endif
    QWhatsThis::add( tb, tr( "<b>The File toolbar</b>%1" ).arg(tr(toolbarHelp).arg("")) );
    addToolBar( tb, tr( "File" ) );
    QPopupMenu *menu = new QPopupMenu( this );
    menubar->insertItem( tr( "&File" ), menu );

    QAction *a = 0;

    a = new QAction( this, 0 );
    a->setText( tr( "New" ) );
    a->setMenuText( tr( "&New" ) );
    a->setIconSet( createIconSet("filenew.xpm") );
    a->setAccel( CTRL + Key_N );
    a->setStatusTip( tr( "Creates a new form" ) );
    a->setWhatsThis( tr("<b>Create a new form</b>"
		        "<p>Select a template for the new form or start with an empty form.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( tb );
    a->addTo( menu );

    a = new QAction( this, 0 );
    a->setText( tr( "Open" ) );
    a->setMenuText( tr( "&Open..." ) );
    a->setIconSet( createIconSet("fileopen.xpm") );
    a->setAccel( CTRL + Key_O );
    a->setStatusTip( tr( "Opens an existing form") );
    a->setWhatsThis( tr("<b>Open a User-Interface (ui) file</b>"
			"<p>Use the filedialog to select the file you want to "
			"open. You can also use Drag&Drop to open multiple files.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( tb );
    a->addTo( menu );

    a = new QAction( this, 0 );
    a->setText( tr( "Save" ) );
    a->setMenuText( tr( "&Save" ) );
    a->setIconSet( createIconSet("filesave.xpm") );
    a->setAccel( CTRL + Key_S );
    a->setStatusTip( tr( "Saves the current form" ) );
    a->setWhatsThis( tr("<b>Save the current form</b>"
			"<p>A filedialog will open if there is no filename already "
			"provided, otherwise the old name will be used.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
#if 0 // #### Reggie: I don't like it
    connect( this, SIGNAL( formModified(bool) ), a, SLOT( setEnabled(bool) ) );
#endif
    menu->insertSeparator();
    a->addTo( tb );
    a->addTo( menu );

    a = new QAction( this, 0 );
    a->setText( tr( "Save As" ) );
    a->setMenuText( tr( "Save &As..." ) );
    a->setStatusTip( tr( "Saves the current form with a new filename" ) );
    a->setWhatsThis( tr( "Save the current form with a new filename" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( this, 0 );
    a->setText( tr( "Save All" ) );
    a->setMenuText( tr( "Sa&ve All" ) );
    a->setStatusTip( tr( "Saves all open forms" ) );
    a->setWhatsThis( tr( "Save all open forms" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAll() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    menu->insertSeparator();

    a = new QAction( this, 0 );
    a->setText( tr( "Create Template" ) );
    a->setMenuText( tr( "&Create Template..." ) );
    a->setStatusTip( tr( "Creates a new template" ) );
    a->setWhatsThis( tr( "Creates a new template" ) );
    connect( a, SIGNAL( activated() ), this, SLOT( fileCreateTemplate() ) );
    a->addTo( menu );

    menu->insertSeparator();

    a = new QAction( this, 0 );
    a->setText( tr( "Exit" ) );
    a->setMenuText( tr( "E&xit" ) );
    a->setStatusTip( tr( "Quits the application and prompts to save changed forms" ) );
    a->setWhatsThis( tr( "<b>Exit the designer</b>"
			 "<p>The Qt Designer will ask if you want to save changed forms before "
			 "the application closes.</p>") );
    connect( a, SIGNAL( activated() ), qApp, SLOT( closeAllWindows() ) );
    a->addTo( menu );
}

void MainWindow::setupPreviewActions()
{
    QAction* a = 0;
    QPopupMenu *menu = new QPopupMenu( this );
    menubar->insertItem( tr( "&Preview" ), menu );
		
    a = new QAction( tr( "Preview Form" ), createIconSet("previewform.xpm"),
				     tr( "Preview &Form" ), 0, this, 0 );
    a->setAccel( CTRL + Key_T );
    a->setStatusTip( tr("Opens a preview") );
    a->setWhatsThis( tr("<b>Open a preview</b>"
			"<p>Use the preview to test the design and "
			"signal-slot connections of the current form.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewForm() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    menu->insertSeparator();

    a = new QAction( tr( "Preview Form in Motif Style" ), createIconSet("previewform.xpm"),
				     tr( "Preview Form in &Motif Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in Motif style") );
    a->setWhatsThis( tr("<b>Open a preview in Motif style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use the Motif "
			"Look&Feel used as the default style on most UNIX-Systems.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormMotif() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( tr( "Preview Form in Windows Style" ), createIconSet("previewform.xpm"),
				     tr( "Preview Form in &Windows Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in Windows style") );
    a->setWhatsThis( tr("<b>Open a preview in Windows style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use the Windows "
			"Look&Feel used as the default style on Windows-Systems.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormWindows() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( tr( "Preview Form in Platinum Style" ), createIconSet("previewform.xpm"),
				     tr( "Preview Form in &Platinum Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in Platinum style") );
    a->setWhatsThis( tr("<b>Open a preview in Platinum style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use the Platinum "
			"Look&Feel resembling a Macinosh-like GUI style.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormPlatinum() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( tr( "Preview Form in CDE Style" ), createIconSet("previewform.xpm"),
				     tr( "Preview Form in &CDE Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in CDE style") );
    a->setWhatsThis( tr("<b>Open a preview in CDE style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use the CDE "
			"Look&Feel which is similar to some versions of the "
			"Common Desktop Environment.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormCDE() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( tr( "Preview Form in SGI Style" ), createIconSet("previewform.xpm"),
		     tr( "Preview Form in &SGI Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in SGI style") );
    a->setWhatsThis( tr("<b>Open a preview in SGI style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use the Motif "
			"Look&Feel used as the default style on SGI-systems.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormSGI() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );

    a = new QAction( tr( "Preview Form in Motif Plus Style" ), createIconSet("previewform.xpm"),
		     tr( "Preview Form in Motif &Plus Style" ), 0, this, 0 );
    a->setStatusTip( tr("Opens a preview in Motif Plus style") );
    a->setWhatsThis( tr("<b>Open a preview in Motif Plus style.</b>"
			"<p>Use the preview to test the design and signal-slot "
			"connections of the form. The preview will use an advanced "
			"Motif Look&Feel as used by the GIMP toolkit (GTK) on Linux.</p>") );
    connect( a, SIGNAL( activated() ), this, SLOT( previewFormMotifPlus() ) );
    connect( this, SIGNAL( hasActiveForm(bool) ), a, SLOT( setEnabled(bool) ) );
    a->addTo( menu );
}

void MainWindow::setupWindowActions()
{
    if ( !actionWindowPropertyEditor ) {
	actionWindowPropertyEditor = new QAction( tr( "Property Editor" ), tr( "Property &Editor" ), 0, this, 0, TRUE );
	actionWindowPropertyEditor->setStatusTip( tr("Toggles the Property Editor") );
	actionWindowPropertyEditor->setWhatsThis( tr("<b>Toggle the Property Editor</b>"
						     "<p>Use the property editor to change the attributes of the "
						     "widgets in your form.</p>") );
	connect( actionWindowPropertyEditor, SIGNAL( toggled(bool) ), this, SLOT( windowPropertyEditor(bool) ) );

	actionWindowHierarchyView = new QAction( tr( "Object Hierarchy" ), tr( "Object &Hierarchy" ), 0, this, 0, TRUE );
	actionWindowHierarchyView->setStatusTip( tr("Toggles the Object Hierarchy view") );
	actionWindowHierarchyView->setWhatsThis( tr("<b>Toggle the Object Hierarchy view</b>"
						    "<p>The object hierarchy gives a quick overview about the relations "
						    "between the widgets in your form.</p>") );
	connect( actionWindowHierarchyView, SIGNAL( toggled(bool) ), this, SLOT( windowHierarchyView(bool) ) );

	actionWindowFormList = new QAction( tr( "Form List" ), tr( "&Form List" ), 0, this, 0, TRUE );
	actionWindowFormList->setStatusTip( tr("Toggles the Form List") );
	actionWindowFormList->setWhatsThis( tr("<b>Toggle the Form List</b>"
					       "<p>The Form List displays the filenames of all open forms, and a flag indicates "
					       "which forms have been changed.</p>") );
	connect( actionWindowFormList, SIGNAL( toggled(bool) ), this, SLOT( windowFormList(bool) ) );

	actionWindowTile = new QAction( tr( "Tile" ), tr( "&Tile" ), 0, this );
	actionWindowTile->setStatusTip( tr("Arranges all windows tiled") );
	actionWindowTile->setWhatsThis( tr("Arrange all windows tiled") );
	connect( actionWindowTile, SIGNAL( activated() ), workspace, SLOT( tile() ) );
	actionWindowCascade = new QAction( tr( "Cascade" ), tr( "&Cascade" ), 0, this );
	actionWindowCascade->setStatusTip( tr("Arrange all windows cascaded") );
	actionWindowCascade->setWhatsThis( tr("Arrange all windows cascaded") );
	connect( actionWindowCascade, SIGNAL( activated() ), workspace, SLOT( cascade() ) );

	actionWindowClose = new QAction( tr( "Close" ), tr( "Cl&ose" ), CTRL + Key_F4, this );
	actionWindowClose->setStatusTip( tr( "Closes the active window") );
	actionWindowClose->setWhatsThis( tr( "Close the active window") );
	connect( actionWindowClose, SIGNAL( activated() ), workspace, SLOT( closeActiveWindow() ) );

	actionWindowCloseAll = new QAction( tr( "Close All" ), tr( "Close Al&l" ), 0, this );
	actionWindowCloseAll->setStatusTip( tr( "Closes all form windows") );
	actionWindowCloseAll->setWhatsThis( tr( "Close all form windows") );
	connect( actionWindowCloseAll, SIGNAL( activated() ), this, SLOT( closeAllForms() ) );

    	actionWindowNext = new QAction( tr( "Next" ), tr( "Ne&xt" ), CTRL + Key_F6, this );
	actionWindowNext->setStatusTip( tr( "Activates the next window" ) );
	actionWindowNext->setWhatsThis( tr( "Activate the next window" ) );
	connect( actionWindowNext, SIGNAL( activated() ), workspace, SLOT( activateNextWindow() ) );

    	actionWindowPrevious = new QAction( tr( "Previous" ), tr( "Pre&vious" ), CTRL + SHIFT + Key_F6, this );
	actionWindowPrevious->setStatusTip( tr( "Activates the previous window" ) );
	actionWindowPrevious->setWhatsThis( tr( "Activate the previous window" ) );
	connect( actionWindowPrevious, SIGNAL( activated() ), workspace, SLOT( activatePreviousWindow() ) );
    }

    if ( !windowMenu ) {
	windowMenu = new QPopupMenu( this );
	menubar->insertItem( tr( "&Window" ), windowMenu );
	connect( windowMenu, SIGNAL( aboutToShow() ),
		 this, SLOT( setupWindowActions() ) );
    } else {
	windowMenu->clear();
    }

    actionWindowClose->addTo( windowMenu );
    actionWindowCloseAll->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowNext->addTo( windowMenu );
    actionWindowPrevious->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowTile->addTo( windowMenu );
    actionWindowCascade->addTo( windowMenu );
    windowMenu->insertSeparator();
    actionWindowPropertyEditor->addTo( windowMenu );
    actionWindowHierarchyView->addTo( windowMenu );
    actionWindowFormList->addTo( windowMenu );
    QWidgetList windows = workspace->windowList();
    if ( windows.count() && formWindow() )
	windowMenu->insertSeparator();
    int j = 0;
    for ( int i = 0; i < int( windows.count() ); ++i ) {
	QWidget *w = windows.at( i );
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	j++;
	QString itemText;
	if ( j < 10 )
	    itemText = QString("&%1 ").arg( j );
	itemText += w->name();
	
	int id = windowMenu->insertItem( itemText, this, SLOT( windowsMenuActivated( int ) ) );
	windowMenu->setItemParameter( id, i );
	windowMenu->setItemChecked( id, workspace->activeWindow() == windows.at( i ) );
    }
}

void MainWindow::setupHelpActions()
{
    actionHelpContents = new QAction( tr( "Contents" ), tr( "&Contents" ), Key_F1, this, 0 );
    actionHelpContents->setStatusTip( tr("Opens the online help") );
    actionHelpContents->setWhatsThis( tr("<b>Open the online help</b>"
					 "<p>Use the online help to get detailed information "
					 "about selected components. Press the F1 key to open "
					 "context sensitive help on the selected item or property.</p>") );
    connect( actionHelpContents, SIGNAL( activated() ), this, SLOT( helpContents() ) );

    actionHelpManual = new QAction( tr( "Manual" ), tr( "&Manual" ), CTRL + Key_M, this, 0 );
    actionHelpManual->setStatusTip( tr("Opens the Qt Designer manual") );
    actionHelpManual->setWhatsThis( tr("<b>Open the Qt Designer manual</b>"
					 "<p>Use the Qt Designer Manual to get help about how to use the Qt Designer.</p>") );
    connect( actionHelpManual, SIGNAL( activated() ), this, SLOT( helpManual() ) );

    actionHelpAbout = new QAction( tr("About"), QPixmap(), tr("&About..."), 0, this, 0 );
    actionHelpAbout->setStatusTip( tr("Displays information about this product") );
    actionHelpAbout->setWhatsThis( tr("Get information about this product") );
    connect( actionHelpAbout, SIGNAL( activated() ), this, SLOT( helpAbout() ) );

    actionHelpAboutQt = new QAction( tr("About Qt"), QPixmap(), tr("About &Qt..."), 0, this, 0 );
    actionHelpAboutQt->setStatusTip( tr("Displays information about the Qt Toolkit") );
    actionHelpAboutQt->setWhatsThis( tr("Get information about the Qt Toolkit") );
    connect( actionHelpAboutQt, SIGNAL( activated() ), this, SLOT( helpAboutQt() ) );
#if defined(QT_NON_COMMERCIAL)
    actionHelpRegister = new QAction( tr("Register Qt"), QPixmap(), tr("&Register Qt..."), 0, this, 0 );
    actionHelpRegister->setStatusTip( tr("Launches web browser with evaluation form at www.trolltech.com") );
    actionHelpRegister->setWhatsThis( tr("Register with Trolltech") );
    connect( actionHelpRegister, SIGNAL( activated() ), this, SLOT( helpRegister() ) );
#endif
    actionHelpWhatsThis = new QAction( tr("What's This?"), QIconSet( whatsthis_image, whatsthis_image ),
				       tr("What's This?"), SHIFT + Key_F1, this, 0 );
    actionHelpWhatsThis->setStatusTip( tr("\"What's This?\" context sensitive help") );
    actionHelpWhatsThis->setWhatsThis( tr("<b>That's me!</b>"
					  "<p>In What's This?-Mode, the mouse cursor shows an arrow with a questionmark, "
					  "and you can click on the interface elements to get a short "
					  "description of what they do and how to use them. In dialogs, "
					  "this feature can be accessed using the context help button in the titlebar.</p>") );
    connect( actionHelpWhatsThis, SIGNAL( activated() ), this, SLOT( whatsThis() ) );

#if defined(HAVE_KDE)
    KToolBar *tb = new KToolBar( this );
    tb->setFullSize( FALSE );
#else
    QToolBar *tb = new QToolBar( this );
#endif
    QWhatsThis::add( tb, tr( "<b>The Help toolbar</b>%1" ).arg(tr(toolbarHelp).arg("") ));
    addToolBar( tb, tr( "Help" ) );
    actionHelpWhatsThis->addTo( tb );

    QPopupMenu *menu = new QPopupMenu( this );
    menubar->insertSeparator();
    menubar->insertItem( tr( "&Help" ), menu );
    actionHelpContents->addTo( menu );
    actionHelpManual->addTo( menu );
    menu->insertSeparator();
    actionHelpAbout->addTo( menu );
    actionHelpAboutQt->addTo( menu );
#if defined(QT_NON_COMMERCIAL)
    actionHelpRegister->addTo( menu );
#endif
    menu->insertSeparator();
    actionHelpWhatsThis->addTo( menu );
}

void MainWindow::setupPropertyEditor()
{
    propertyEditor = new PropertyEditor( workspace );
    propertyEditor->resize( 300, 600 );
    QWhatsThis::add( propertyEditor, tr("<b>The Property Editor</b>"
					"<p>You can change the appearance and behaviour of the selected widget in the "
					"property editor.</p>"
					"<p>You can set properties for components and forms at design time and see the "
					"changes immediately. Each property has its own editor which you can use to enter "
					"new values, open a special dialog or select values from a predefined list. "
					"Use <b>F1</b> to get detailed help for the selected property.</p>"
					"<p>You can resize the columns of the editor by dragging the separators of the list "
					"header.</p>") );
    propGeom = QRect( 0, 0, 300, 600 );
    connect( propertyEditor, SIGNAL( hidden() ),
	     this, SLOT( propertyEditorHidden() ) );
    actionWindowPropertyEditor->setOn( TRUE );
}

void MainWindow::setupHierarchyView()
{
    if ( hierarchyView )
	return;
    hierarchyView = new HierarchyView( workSpace() );
    hierarchyView->setCaption( tr( "Object Hierarchy" ) );
    hierarchyView->resize( 300, 500 );
    hvGeom = QRect( -1, -1, 300, 500 );
    QWhatsThis::add( hierarchyView, tr("<b>The Hierarchy View</b>"
				      "<p>The object hierarchy gives a quick overview about the relations "
				      "between the widgets in your form. You can use the clipboard functions using "
				      "a context menu for each item in the view.</p>"
				      "<p>The columns can be resized by dragging the separator in the list header.</p>" ) );
    connect( hierarchyView, SIGNAL( hidden() ),
	     this, SLOT( hierarchyViewHidden() ) );
    actionWindowHierarchyView->setOn( FALSE );
    hierarchyView->hide();
}

void MainWindow::setupFormList()
{
    formList = new FormList( workSpace(), this );
    formList->setCaption( tr( "Forms" ) );
    formList->resize( 600, 300 );
    flGeom = QRect( -1, -1, 600, 300 );
    QWhatsThis::add( formList, tr("<b>The Form List</b>"
				  "<p>The Form List displays the filenames of all open forms, and a flag indicates "
				  "which forms have been changed.</p>"
				  "<p>The columns can be resized by dragging the separator in the list header.</p>") );
    connect( formList, SIGNAL( hidden() ),
	     this, SLOT( formListHidden() ) );
    actionWindowFormList->setOn( FALSE );
    formList->hide();
}

void MainWindow::setupRMBMenus()
{
    rmbWidgets = new QPopupMenu( this );
    actionEditCut->addTo( rmbWidgets );
    actionEditCopy->addTo( rmbWidgets );
    actionEditPaste->addTo( rmbWidgets );
    actionEditDelete->addTo( rmbWidgets );
#if 0
    rmbWidgets->insertSeparator();
    actionEditLower->addTo( rmbWidgets );
    actionEditRaise->addTo( rmbWidgets );
#endif
    rmbWidgets->insertSeparator();
    actionEditAdjustSize->addTo( rmbWidgets );
    actionEditHLayout->addTo( rmbWidgets );
    actionEditVLayout->addTo( rmbWidgets );
    actionEditGridLayout->addTo( rmbWidgets );
    actionEditBreakLayout->addTo( rmbWidgets );
    rmbWidgets->insertSeparator();
    actionEditConnections->addTo( rmbWidgets );

    rmbFormWindow = new QPopupMenu( this );
    actionEditPaste->addTo( rmbFormWindow );
    actionEditSelectAll->addTo( rmbFormWindow );
    actionEditAccels->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditAdjustSize->addTo( rmbFormWindow );
    actionEditHLayout->addTo( rmbFormWindow );
    actionEditVLayout->addTo( rmbFormWindow );
    actionEditGridLayout->addTo( rmbFormWindow );
    actionEditBreakLayout->addTo( rmbFormWindow );
    rmbFormWindow->insertSeparator();
    actionEditSlots->addTo( rmbFormWindow );
    actionEditConnections->addTo( rmbFormWindow );
    actionEditFormSettings->addTo( rmbFormWindow );
}

void MainWindow::toolSelected( QAction* action )
{
    actionCurrentTool = action;
    emit currentToolChanged();
    if ( formWindow() )
	formWindow()->commandHistory()->emitUndoRedo();
}

int MainWindow::currentTool() const
{
    if ( !actionCurrentTool )
	return POINTER_TOOL;
    return QString::fromLatin1(actionCurrentTool->name()).toInt();
}

static void unifyFormName( FormWindow *fw, QWorkspace *workspace )
{
    QStringList lst;
    QWidgetList windows = workspace->windowList();
    for ( QWidget *w =windows.first(); w; w = windows.next() ) {
	if ( w == fw )
	    continue;
	lst << w->name();
    }

    if ( lst.findIndex( fw->name() ) == -1 )
	return;
    QString origName = fw->name();
    QString n = origName;
    int i = 1;
    while ( lst.findIndex( n ) != -1 ) {
	n = origName + QString::number( i++ );
    }
    fw->setName( n );
    fw->setCaption( n );
}

void MainWindow::fileNew()
{
    statusBar()->message( tr("Select a template for the new form...") );
    NewForm dlg( this, templatePath() );
    if ( dlg.exec() == QDialog::Accepted ) {
	NewForm::Form f = dlg.formType();
	if ( f != NewForm::Custom ) {
	    insertFormWindow( f )->setFocus();
	} else {
	    QString filename = dlg.templateFile();
	    if ( !filename.isEmpty() && QFile::exists( filename ) ) {
		Resource resource( this );
		if ( !resource.load( filename ) ) {
		    QMessageBox::information( this, tr("Load Template"),
			tr("Couldn't load form description from template "+ filename ) );
		    return;
		}
		if ( formWindow() )
		    formWindow()->setFileName( QString::null );
		unifyFormName( formWindow(), workspace );
	    }
	}
    }
    statusBar()->clear();
}

void MainWindow::fileOpen()
{
    statusBar()->message( tr( "Select a file...") );
    QString filename = QFileDialog::getOpenFileName( QString::null, Import::filters(), this );
    if ( !filename.isEmpty() ) {
	if ( filename.right( 4 ) == ".pro" ) {
	    QStringList lst = getUiFiles( filename );
	    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
		QString fn = QUrl( QFileInfo( filename ).dirPath(), *it ).path();
		openFile( fn );
	    }
	} else if ( filename.right( 3 ) == ".ui" ) {
	    openFile( filename );
	} else {
	    statusBar()->message( tr( "Importing %1 using a 3rd party converter, this might take some time...").arg( filename ) );
	    (void)new Import( filename, this ); // deletes itself
	}
    }
}

void MainWindow::openFile( const QString &filename, bool validFileName )
{
    if ( filename.isEmpty() )
	return;

    bool makeNew = FALSE;

    if ( !QFile::exists( filename ) ) {
	makeNew = TRUE;
    } else {
	QFile f( filename );
	f.open( IO_ReadOnly );
	QTextStream ts( &f );
	makeNew = ts.read().length() < 2;
    }
    if ( !makeNew ) {
	statusBar()->message( tr( "Reading file %1...").arg( filename ) );
	if ( QFile::exists( filename ) ) {
	    QApplication::setOverrideCursor( WaitCursor );
	    Resource resource( this );
	    bool b = resource.load( filename ) && (FormWindow*)resource.widget();
	    if ( !validFileName && resource.widget() )
		( (FormWindow*)resource.widget() )->setFileName( QString::null );
	    QApplication::restoreOverrideCursor();
	    qApp->processEvents();
	    if ( b ) {
		rebuildCustomWidgetGUI();
		statusBar()->message( tr( "File %1 opened.").arg( filename ), 3000 );
	    } else {
		statusBar()->message( tr( "Failed to load file %1").arg( filename ), 5000 );
		QMessageBox::information( this, tr("Load File"), tr("Couldn't load file %1").arg( filename ) );	
	    }
	} else {
	    statusBar()->clear();
	}
    } else {
	fileNew();
	if ( formWindow() )
	    formWindow()->setFileName( filename );
    }
}

bool MainWindow::fileSave()
{
    if ( !formWindow() )
	return FALSE;
    if ( formWindow()->fileName().isEmpty() ) {
	return fileSaveAs();
    } else {
	QApplication::setOverrideCursor( WaitCursor );
	formWindow()->save( formWindow()->fileName() );
	QApplication::restoreOverrideCursor();
    }
    return TRUE;
}

bool MainWindow::fileSaveAs()
{
    statusBar()->message( tr( "Enter a filename..." ) );
    if ( !formWindow() )
	return FALSE;
    FormWindow *fw = formWindow();
    QString filename = QFileDialog::getSaveFileName( QString::null, tr( "Qt User-Interface Files (*.ui);;All Files (*)" ),
						     this );
    if ( filename.isEmpty() )
	return FALSE;
    QFileInfo fi( filename );
    if ( fi.extension() != "ui" )
	filename += ".ui";
    fw->setFileName( filename );
    fileSave();
    return TRUE;
}

void MainWindow::fileSaveAll()
{
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	w->setFocus();
	qApp->processEvents();
	fileSave();
    }
}

static bool inSaveAllTemp = FALSE;
void MainWindow::saveAllTemp()
{
    if ( inSaveAllTemp )
	return;
    inSaveAllTemp = TRUE;
    statusBar()->message( tr( "Qt Designer is crashing - saving work as good as possible..." ) );
    QWidgetList windows = workSpace()->windowList();
    QString baseName = QString( getenv( "HOME" ) ) + "/.designer/saved-form-";
    int i = 1;
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	
	QString fn = baseName + QString::number( i++ ) + ".ui";
	( (FormWindow*)w )->setFileName( fn );
	( (FormWindow*)w )->save( fn );
    }
    inSaveAllTemp = FALSE;
}

void MainWindow::fileCreateTemplate()
{
    CreateTemplate dia( this, 0, TRUE );

    for ( int i = 0; i < WidgetDatabase::numWidgetGroups(); ++i ) {
	if ( WidgetDatabase::isContainer( i ) && WidgetDatabase::className( i ) != "QTabWidget" )
	    dia.listClass->insertItem( WidgetDatabase::className( i ) );
    }
    QList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	if ( w->isContainer )
	    dia.listClass->insertItem( w->className );
    }

    dia.editName->setText( tr( "NewTemplate" ) );
    connect( dia.buttonCreate, SIGNAL( clicked() ),
	     this, SLOT( createNewTemplate() ) );
    dia.exec();
}

void MainWindow::editUndo()
{
    if ( formWindow() )
 	formWindow()->undo();
}

void MainWindow::editRedo()
{
    if ( formWindow() )
	formWindow()->redo();
}

void MainWindow::editCut()
{
    editCopy();
    editDelete();
}

void MainWindow::editCopy()
{
    if ( formWindow() )
	qApp->clipboard()->setText( formWindow()->copy() );
}

void MainWindow::editPaste()
{
    if ( !formWindow() )
	return;

    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 ) {
	w = l.first();
	if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	     ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) &&
	       w != formWindow()->mainContainer() ) )
	    w = formWindow()->mainContainer();
    }

    if ( w && WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
	formWindow()->paste( qApp->clipboard()->text(), WidgetFactory::containerOfWidget( w ) );
	hierarchyView->widgetInserted( 0 );
	formWindow()->commandHistory()->setModified( TRUE );
    } else {
	// #### should we popup a messagebox here which says that
	// nothing has been pasted because you can't paste into a
	// laid out widget? (RS)
    }
}

void MainWindow::editDelete()
{
    if ( formWindow() )
	formWindow()->deleteWidgets();
}

void MainWindow::editSelectAll()
{
    if ( formWindow() )
	formWindow()->selectAll();
}


void MainWindow::editLower()
{
    if ( formWindow() )
	formWindow()->lowerWidgets();
}

void MainWindow::editRaise()
{
    if ( formWindow() )
	formWindow()->raiseWidgets();
}

void MainWindow::editAdjustSize()
{
    if ( formWindow() )
	formWindow()->editAdjustSize();
}

void MainWindow::editLayoutHorizontal()
{
    if ( layoutChilds )
	editLayoutContainerHorizontal();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutHorizontal();
}

void MainWindow::editLayoutVertical()
{
    if ( layoutChilds )
	editLayoutContainerVertical();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutVertical();
}

void MainWindow::editLayoutGrid()
{
    if ( layoutChilds )
	editLayoutContainerGrid();
    else if ( layoutSelected && formWindow() )
	formWindow()->layoutGrid();
}

void MainWindow::editLayoutContainerHorizontal()
{
    if ( !formWindow() )
	return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutHorizontalContainer( w );
}

void MainWindow::editLayoutContainerVertical()
{
    if ( !formWindow() )
	return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutVerticalContainer( w  );
}

void MainWindow::editLayoutContainerGrid()
{
    if ( !formWindow() )
	return;
    QWidget *w = formWindow()->mainContainer();
    QWidgetList l( formWindow()->selectedWidgets() );
    if ( l.count() == 1 )
	w = l.first();
    if ( w )
	formWindow()->layoutGridContainer( w  );
}

void MainWindow::editBreakLayout()
{
    if ( !formWindow() || !breakLayout )
	return;
    QWidget *w = formWindow()->mainContainer();
    if ( formWindow()->currentWidget() )
	w = formWindow()->currentWidget();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	 w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
	formWindow()->breakLayout( w );
	return;
    } else {
	QWidgetList widgets = formWindow()->selectedWidgets();
	for ( w = widgets.first(); w; w = widgets.next() ) {
	    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
		 w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
		break;
	}
	if ( w ) {
	    formWindow()->breakLayout( w );
	    return;
	}
    }

    w = formWindow()->mainContainer();
    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ||
	 w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	formWindow()->breakLayout( w );
}

void MainWindow::editAccels()
{
    if ( !formWindow() )
	return;
    formWindow()->checkAccels();
}

void MainWindow::editSlots()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit slots of current form..." ) );
    EditSlots dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editConnections()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit connections in current form..." ) );
    ConnectionViewer dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editFormSettings()
{
    if ( !formWindow() )
	return;

    statusBar()->message( tr( "Edit settings of current form..." ) );
    FormSettings dlg( this, formWindow() );
    dlg.exec();
    statusBar()->clear();
}

void MainWindow::editPreferences()
{
    statusBar()->message( tr( "Edit preferences..." ) );
    Preferences *dia = new Preferences( this, 0, TRUE );
    prefDia = dia;
    connect( dia->helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    dia->buttonColor->setEditor( StyledButton::ColorEditor );
    dia->buttonPixmap->setEditor( StyledButton::PixmapEditor );
    dia->checkBoxShowGrid->setChecked( sGrid );
    dia->checkBoxGrid->setChecked( snGrid );
    dia->spinGridX->setValue( grid().x() );
    dia->spinGridY->setValue( grid().y() );
    dia->checkBoxWorkspace->setChecked( restoreConfig );
    dia->checkBoxBigIcons->setChecked( usesBigPixmaps() );
    dia->checkBoxBigIcons->hide(); // ##### disabled for now
    dia->checkBoxTextLabels->setChecked( usesTextLabel() );
    dia->buttonColor->setColor( workspace->backgroundColor() );
    if ( workspace->backgroundPixmap() )
	dia->buttonPixmap->setPixmap( *workspace->backgroundPixmap() );
    if ( backPix )
	dia->radioPixmap->setChecked( TRUE );
    else
	dia->radioColor->setChecked( TRUE );
    dia->checkBoxSplash->setChecked( splashScreen );
    dia->editDocPath->setText( docPath );
    connect( dia->buttonDocPath, SIGNAL( clicked() ),
	     this, SLOT( chooseDocPath() ) );

    if ( dia->exec() == QDialog::Accepted ) {
	setSnapGrid( dia->checkBoxGrid->isChecked() );
	setShowGrid( dia->checkBoxShowGrid->isChecked() );
	setGrid( QPoint( dia->spinGridX->value(),
			 dia->spinGridY->value() ) );
	restoreConfig = dia->checkBoxWorkspace->isChecked();
	setUsesBigPixmaps( FALSE /*dia->checkBoxBigIcons->isChecked()*/ ); // ### disable for now
	setUsesTextLabel( dia->checkBoxTextLabels->isChecked() );
	if ( help ) {
	    help->setUsesBigPixmaps( FALSE /*dia->checkBoxBigIcons->isChecked()*/ ); // ### same here
	    help->setUsesTextLabel( dia->checkBoxTextLabels->isChecked() );
	}
	if ( dia->radioPixmap->isChecked() && dia->buttonPixmap->pixmap() ) {
	    workspace->setBackgroundPixmap( *dia->buttonPixmap->pixmap() );
	    backPix = TRUE;
	} else {
	    workspace->setBackgroundColor( dia->buttonColor->color() );
	    backPix = FALSE;
	}
	splashScreen = dia->checkBoxSplash->isChecked();
	docPath = dia->editDocPath->text();
    }
    delete dia;
    prefDia = 0;
    statusBar()->clear();
}

QWidget* MainWindow::previewFormInternal( QStyle* style, QPalette* palet )
{
    QApplication::setOverrideCursor( WaitCursor );
    if ( currentTool() == ORDER_TOOL )
	resetTool();

    FormWindow *fw = formWindow();
    if ( !fw )
	return 0;
    QCString s;
    QBuffer buffer( s );
    buffer.open( IO_WriteOnly );
    Resource resource( this );
    resource.setWidget( fw );
    QValueList<Resource::Image> images;
    resource.save( &buffer, FALSE, &images );

    buffer.close();
    buffer.open( IO_ReadOnly );
    Resource preview( style, palet );
    if ( preview.load( &buffer, &images ) && preview.widget() ) {
	preview.widget()->move( fw->mapToGlobal( QPoint(0,0) ) );
	((MainWindow*)preview.widget() )->setWFlags( WDestructiveClose );
	QApplication::restoreOverrideCursor();
	return preview.widget();
    }
    QApplication::restoreOverrideCursor();
    return 0;
}

void MainWindow::previewForm()
{
    QWidget* w = previewFormInternal();
    if ( w )
	w->show();
}

void MainWindow::previewFormMotif()
{
    static QMotifStyle* style = 0;
    if ( !style )
	style = new QMotifStyle;
    QPalette p( QColor( 192, 192, 192 ) );
    QWidget* w = previewFormInternal( style, &p );
    if (! w )
	return;
    w->show();
}

void MainWindow::previewFormWindows()
{
    static QWindowsStyle* style = 0;
    if ( !style )
	style = new QWindowsStyle;
    QPalette p( QColor( 212, 208, 200 ) );
    QWidget* w = previewFormInternal( style, &p );
    if ( w )
	w->show();
}

void MainWindow::previewFormPlatinum()
{
    static QPlatinumStyle* style = 0;
    if ( !style )
	style = new QPlatinumStyle;
    QPalette p( QColor( 220, 220, 220 ) );
    QWidget* w = previewFormInternal( style, &p );
    if ( w )
	w->show();
}

void MainWindow::previewFormCDE()
{
    static QCDEStyle* style = 0;
    if ( !style )
	style = new QCDEStyle;
    QPalette p( QColor( 75, 123, 130 ) );
    p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
    p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
    p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
    p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
    QWidget* w = previewFormInternal( style, &p );
    if ( !w )
	return;
    w->setFont( QFont( "times", QApplication::font().pointSize() ) );
    w->show();
}

void MainWindow::previewFormSGI()
{
    static QSGIStyle* style = 0;
    if ( !style )
	style = new QSGIStyle;
    QPalette p( QColor( 220, 220, 220 ) );
    QWidget* w = previewFormInternal( style, &p );
    if ( !w )
	return;
    QFont font( QApplication::font() );
    font.setPointSize( 14 );
    w->setFont( font );
    w->show();
}

void MainWindow::previewFormMotifPlus()
{
    static QMotifPlusStyle* style = 0;
    if ( !style )
	style = new QMotifPlusStyle;

    QColor gtkfg(0x00, 0x00, 0x00);
    QColor gtkdf(0x75, 0x75, 0x75);
    QColor gtksf(0xff, 0xff, 0xff);
    QColor gtkbs(0xff, 0xff, 0xff);
    QColor gtkbg(0xd6, 0xd6, 0xd6);
    QColor gtksl(0x00, 0x00, 0x9c);
    QColorGroup active(gtkfg,            // foreground
		       gtkbg,            // button
		       gtkbg.light(),    // light
		       gtkbg.dark(142),  // dark
		       gtkbg.dark(110),  // mid
		       gtkfg,            // text
		       gtkfg,            // bright text
		       gtkbs,            // base
		       gtkbg),           // background
	disabled(gtkdf,            // foreground
		 gtkbg,            // button
		 gtkbg.light(), // light
		 gtkbg.dark(156),  // dark
		 gtkbg.dark(110),  // mid
		 gtkdf,            // text
		 gtkdf,            // bright text
		 gtkbs,            // base
		 gtkbg);           // background

    QPalette pal(active, disabled, active);

    pal.setColor(QPalette::Active, QColorGroup::Highlight,
		 gtksl);
    pal.setColor(QPalette::Active, QColorGroup::HighlightedText,
		 gtksf);
    pal.setColor(QPalette::Inactive, QColorGroup::Highlight,
		 gtksl);
    pal.setColor(QPalette::Inactive, QColorGroup::HighlightedText,
		 gtksf);
    pal.setColor(QPalette::Disabled, QColorGroup::Highlight,
		 gtksl);
    pal.setColor(QPalette::Disabled, QColorGroup::HighlightedText,
		 gtkdf);

    QWidget* w = previewFormInternal( style, &pal );
    if (! w )
	return;
    w->show();
}

static void correctGeometry( QWidget *wid, QWidget *workspace )
{
    int x = wid->parentWidget()->x(), y = wid->parentWidget()->y();
    if ( wid->parentWidget()->x() > workspace->width() )
	x = QMAX( 0, workspace->width() - wid->parentWidget()->width() );
    if ( wid->parentWidget()->y() > workspace->height() )
	y = QMAX( 0, workspace->height() - wid->parentWidget()->height() );
    wid->parentWidget()->move( x, y );
    int w = wid->parentWidget()->width(), h = wid->parentWidget()->height();
    if ( x + w > workspace->width() )
	w = workspace->width() - x;
    if ( h + y > workspace->height() )
	h = workspace->height() - y;
    wid->parentWidget()->resize( w, h );
}

void MainWindow::windowPropertyEditor( bool showIt )
{
    if ( !propertyEditor )
	setupPropertyEditor();
    if ( showIt ) {
	correctGeometry( propertyEditor, workspace );
	propertyEditor->show();
	propertyEditor->setFocus();
    } else {
	propertyEditor->hide();
    }
}

void MainWindow::windowHierarchyView( bool showIt )
{
    if ( !hierarchyView )
	setupHierarchyView();
    if ( showIt ) {
	correctGeometry( hierarchyView, workspace );
	hierarchyView->show();
	hierarchyView->setFocus();
    } else {
	hierarchyView->hide();
    }
}

void MainWindow::windowFormList( bool showIt )
{
    if ( !formList )
	setupFormList();
    if ( showIt ) {
	correctGeometry( formList, workspace );
	formList->show();
	formList->setFocus();
    } else {
	formList->hide();
    }
}

void MainWindow::toolsCustomWidget()
{
    statusBar()->message( tr( "Edit custom widgets..." ) );
    CustomWidgetEditor edit( this, this );
    edit.exec();
    rebuildCustomWidgetGUI();
    statusBar()->clear();
}

void MainWindow::helpContents()
{
    if ( !help ) {
	help = new Help( documentationPath(), this, "help" );
	help->setSource( "book1.html" );
	help->setUsesBigPixmaps( FALSE /*usesBigPixmaps()*/ ); // ## disabled for now
	help->setUsesTextLabel( usesTextLabel() );
    }


    if ( propertyDocumentation.isEmpty() ) {
	QString indexFile = documentationPath() + "/propertyindex";
	QFile f( indexFile );
	if ( f.open( IO_ReadOnly ) ) {
	    QTextStream ts( &f );
	    while ( !ts.eof() ) {
		QString s = ts.readLine();
		int from = s.find( "\"" );
		if ( from == -1 )
		    continue;
		int to = s.findRev( "\"" );
		if ( to == -1 )
		    continue;
		propertyDocumentation[ s.mid( from + 1, to - from - 1 ) ] = s.mid( to + 2 );
	    }
	    f.close();
	} else {
	    QMessageBox::critical( this, tr( "Error" ), tr( "Couldn't find the Qt documentation property index file!\n"
					    "Define the correct documentation path in the preferences dialog." ) );
	}
    }
    help->show();
    help->raise();

    if ( propertyEditor->widget() ) {
	QString source;
	if ( workspace->activeWindow() == propertyEditor && !propertyEditor->currentProperty().isEmpty() ) {
	    QMetaObject* mo = propertyEditor->metaObjectOfCurrentProperty();
	    QString s;
	    QString cp = propertyEditor->currentProperty();
	    if ( cp == "layoutMargin" ) {
		source = propertyDocumentation[ "QLayout/margin" ];
	    } else if ( cp == "layoutSpacing" ) {
		source = propertyDocumentation[ "QLayout/spacing" ];
	    } else if ( cp == "toolTip" ) {
		source = "qtooltip.html#details";
	    } else if ( mo && qstrcmp( mo->className(), "Spacer" ) == 0 ) {
		if ( cp != "name" )
		    source = "qsizepolicy.html#SizeType";
		else
		    source = propertyDocumentation[ "QObject/name" ];
	    } else {
		while ( mo && !propertyDocumentation.contains( ( s = QString( mo->className() ) + "/" + cp ) ) )
		    mo = mo->superClass();
		if ( mo )
		    source = propertyDocumentation[s];
	    }
	}
	
	QString classname =  WidgetFactory::classNameOf( propertyEditor->widget() );
	if ( source.isEmpty() ) {
	    if ( classname.lower() == "spacer" )
		source = "qspaceritem.html#details";
	    else if ( classname == "QLayoutWidget" )
		source = "layout.html";
	    else
		source = QString( WidgetFactory::classNameOf( propertyEditor->widget() ) ).lower() + ".html#details";
	}
	
	if ( !source.isEmpty() )
	    help->setSource( source );
    }

}

void MainWindow::helpManual()
{
    if ( !help ) {
	help = new Help( documentationPath(), this, "help" );
	help->setSource( "book1.html" );
	help->setUsesBigPixmaps( FALSE /*usesBigPixmaps()*/ ); // ### disbaled for now
	help->setUsesTextLabel( usesTextLabel() );
    }
    help->setSource( "book1.html" );
    help->show();
    help->raise();
}

void MainWindow::helpAbout()
{
    AboutDialog dlg( this, 0, TRUE );
    dlg.exec();
}

void MainWindow::helpAboutQt()
{
    QMessageBox::aboutQt( this, "Qt Designer" );
}

#if defined(_WS_WIN_)
#include <qt_windows.h>
#include <qprocess.h>
#endif

void MainWindow::helpRegister()
{
#if defined(_WS_WIN_)
    HKEY key;
    HKEY subkey;
    long res;
    DWORD type;
    DWORD size = 255;
    QString command;
    QString sub( "htmlfile\\shell" );
#if defined(UNICODE)
    if ( QApplication::winVersion() & Qt::WV_NT_based ) {
	unsigned char data[256];
	res = RegOpenKeyExW( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
	res = RegOpenKeyExW( key, (TCHAR*)qt_winTchar( sub, TRUE ), 0, KEY_READ, &subkey );
	res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
	command = qt_winQString( data ) + "\\command";
	size = 255;
	res = RegOpenKeyExW( subkey, (TCHAR*)qt_winTchar( command, TRUE ), 0, KEY_READ, &subkey );
	res = RegQueryValueExW( subkey, NULL, NULL, &type, data, &size );
	command = qt_winQString( data );
    } else
#endif
    {
	unsigned char data[256];
	res = RegOpenKeyExA( HKEY_CLASSES_ROOT, NULL, 0, KEY_READ, &key );
	res = RegOpenKeyExA( key, sub.local8Bit(), 0, KEY_READ, &subkey );
	res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
	command = QString::fromLocal8Bit( (const char*) data ) + "\\command";
	size = 255;
	res = RegOpenKeyExA( subkey, command.local8Bit(), 0, KEY_READ, &subkey );
	res = RegQueryValueExA( subkey, NULL, NULL, &type, data, &size );
	command = QString::fromLocal8Bit( (const char*) data );
    }

    res = RegCloseKey( subkey );
    res = RegCloseKey( key );

    QProcess process( command + " www.trolltech.com/products/download/eval/evaluation.html" );
    if ( !process.start() )
	QMessageBox::information( this, "Register Qt", "Launching your web browser failed.\n"
				        "To register Qt, point your browser to www.trolltech.com/products/download/eval/evaluation.html" );
#endif
}

void MainWindow::showProperties( QWidget *w )
{
    setupHierarchyView();
    FormWindow *fw = (FormWindow*)isAFormWindowChild( w );
    if ( fw ) {
	propertyEditor->setWidget( w, fw );
	hierarchyView->setFormWindow( fw, w );
    } else {
	propertyEditor->setWidget( 0, 0 );
	hierarchyView->setFormWindow( 0, 0 );
    }

    if ( currentTool() == POINTER_TOOL )
	fw->setFocus();
}

void MainWindow::resetTool()
{
    actionPointerTool->setOn( TRUE );
}

void MainWindow::updateProperties( QWidget * )
{
    if ( propertyEditor )
	propertyEditor->refetchData();
}

bool MainWindow::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e || !o->isWidgetType() )
	return QMainWindow::eventFilter( o, e );

    QWidget *w = 0;
    bool passiveInteractor = WidgetFactory::isPassiveInteractor( o );
    switch ( e->type() ) {
    case QEvent::MouseButtonPress:
	if ( o && o->inherits( "QSizeGrip" ) )
	    break;
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	if ( !w->hasFocus() )
	    w->setFocus();
	if ( !passiveInteractor || currentTool() != ORDER_TOOL )
	    ( (FormWindow*)w )->handleMousePress( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	lastPressWidget = (QWidget*)o;
	if ( passiveInteractor )
	    QTimer::singleShot( 0, formWindow(), SLOT( visibilityChanged() ) );
	if ( currentTool() == CONNECT_TOOL )
	    return TRUE;
	return !passiveInteractor;
    case QEvent::MouseButtonRelease:
	lastPressWidget = 0;
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	if ( !passiveInteractor )
	    ( (FormWindow*)w )->handleMouseRelease( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	if ( passiveInteractor ) {
	    selectionChanged();
	    QTimer::singleShot( 0, formWindow(), SLOT( visibilityChanged() ) );
	}
	return !passiveInteractor;
    case QEvent::MouseMove:
	if ( lastPressWidget != (QWidget*)o ||
	     ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) ) )
	    break;
	if ( !passiveInteractor )
	    ( (FormWindow*)w )->handleMouseMove( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	return !passiveInteractor;
    case QEvent::KeyPress:
	if ( ( (QKeyEvent*)e )->key() == Key_Escape && currentTool() != POINTER_TOOL ) {
	    resetTool();
	    return FALSE;
	}
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	( (FormWindow*)w )->handleKeyPress( (QKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	if ( ((QKeyEvent*)e)->isAccepted() )
	    return TRUE;
	break;
    case QEvent::MouseButtonDblClick:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) ) {
	    if ( o && o->inherits( "QToolButton" ) && ( ( QToolButton*)o )->isOn() &&
		 o->parent() && o->parent()->inherits( "QToolBar" ) && formWindow() )
		formWindow()->setToolFixed();
	    break;
	}
	if ( currentTool() == ORDER_TOOL ) {
	    ( (FormWindow*)w )->handleMouseDblClick( (QMouseEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	    return TRUE;
	}
	return openEditor( ( (FormWindow*)w )->designerWidget( o ) );
    case QEvent::KeyRelease:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	( (FormWindow*)w )->handleKeyRelease( (QKeyEvent*)e, ( (FormWindow*)w )->designerWidget( o ) );
	if ( ((QKeyEvent*)e)->isAccepted() )
	    return TRUE;
	break;
    case QEvent::Hide:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	if ( ( (FormWindow*)w )->isWidgetSelected( (QWidget*)o ) )
	    ( (FormWindow*)w )->selectWidget( (QWidget*)o, FALSE );
	break;
    case QEvent::Enter:
    case QEvent::Leave:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	return TRUE;
    case QEvent::Resize:
    case QEvent::Move:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	if ( WidgetFactory::layoutType( (QWidget*)o->parent() ) != WidgetFactory::NoLayout ) {
	    ( (FormWindow*)w )->updateSelection( (QWidget*)o );
	    if ( e->type() != QEvent::Resize )
		( (FormWindow*)w )->updateChildSelections( (QWidget*)o );
	}
	break;
    case QEvent::Close:
	if ( o->inherits( "FormWindow" ) ) {
	    if ( !closeForm( (FormWindow*)o ) ) {
		( (QCloseEvent*)e )->ignore();
	    } else {
		( (QCloseEvent*)e )->accept();
		unregisterClient( (FormWindow*)o );
	    }
	    return TRUE;
	}
	break;
    case QEvent::DragEnter:
	if ( o == workSpace() || o == formlist() || o == formlist()->viewport() ) {
	    formlist()->contentsDragEnterEvent( (QDragEnterEvent*)e );
	    return TRUE;
	}
	break;
    case QEvent::DragMove:
	if ( o == workSpace() || o == formlist() || o == formlist()->viewport() ) {
	    formlist()->contentsDragMoveEvent( (QDragMoveEvent*)e );
	    return TRUE;
	}
	break;
    case QEvent::Drop:
	if ( o == workSpace() || o == formlist() || o == formlist()->viewport() ) {
	    formlist()->contentsDropEvent( (QDropEvent*)e );
	    return TRUE;
	}
	break;
    case QEvent::Show:
	if ( o != this )
	    break;
	if ( ((QShowEvent*)e)->spontaneous() )
	    break;
	QApplication::sendPostedEvents( workspace, QEvent::ChildInserted );
	showEvent( (QShowEvent*)e );
	if ( hvGeom.topLeft() == QPoint( -1, -1 ) ) {
	    hvGeom.setX( workSpace()->width() - hierarchyView->width() - 5 );
	    hvGeom.setY( 0 );
	}
	if ( flGeom.topLeft() == QPoint( -1, -1 ) ) {
	    flGeom.setX( workSpace()->width() - formList->width() - 5 );
	    flGeom.setY( workSpace()->height() - formList->height() - 30 );
	}
	hierarchyView->parentWidget()->setGeometry( hvGeom );
	formList->parentWidget()->setGeometry( flGeom );
	propertyEditor->parentWidget()->setGeometry( propGeom );
	correctGeometry( propertyEditor, workspace );
	correctGeometry( hierarchyView, workspace );
	correctGeometry( formList, workspace );
	checkTempFiles();
	return TRUE;
	break;
    case QEvent::Wheel:
	if ( !( w = isAFormWindowChild( o ) ) || o->inherits( "SizeHandle" ) || o->inherits( "OrderIndicator" ) )
	    break;
	return TRUE;
    default:
	return QMainWindow::eventFilter( o, e );
    }

    return QMainWindow::eventFilter( o, e );
}

QWidget *MainWindow::isAFormWindowChild( QObject *o ) const
{
    if ( o->parent() && o->parent()->inherits( "QWizard" ) && !o->inherits( "QPushButton" ) )
	return 0;
    while ( o ) {
	if ( o->inherits( "FormWindow" ) )
	    return (QWidget*)o;
	o = o->parent();
    }
    return 0;
}

FormWindow *MainWindow::formWindow()
{
    if ( workspace->activeWindow() ) {
	FormWindow *fw = 0;
	if ( workspace->activeWindow()->inherits( "FormWindow" ) )
	    fw = (FormWindow*)workspace->activeWindow();
	else if ( lastActiveFormWindow &&
		    workspace->windowList().find( lastActiveFormWindow ) != -1)
	    fw = lastActiveFormWindow;
	return fw;
    }
    return 0;
}

FormWindow* MainWindow::insertFormWindow( int type )
{
    QString n = tr( "Form%1" ).arg( ++forms );
    FormWindow *fw = 0;
    fw = new FormWindow( this, workspace, n );
    if ( type == NewForm::Widget ) {
	QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWidget" ), fw, n.latin1() );
	fw->setMainContainer( w );
    } else if ( type == NewForm::Dialog ) {
	QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QDialog" ), fw, n.latin1() );
	fw->setMainContainer( w );
    } else if ( type == NewForm::Wizard ) {
	QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QWizard" ), fw, n.latin1() );
	fw->setMainContainer( w );
    }
    fw->setCaption( n );
    fw->resize( 600, 480 );
    MetaDataBase::addEntry( fw );
    insertFormWindow( fw );
    return fw;
}

void MainWindow::insertFormWindow( FormWindow *fw )
{
    if ( fw )
	QWhatsThis::add( fw, tr( "<b>The Form Window</b>"
			       "<p>Use the different tools to add widgets or to change the layout "
			       "and behaviour of the components in your form. Select one or multiple "
			       "widgets and move them, or resize a single widget using the handles.</p>"
			       "<p>Changes in the <b>Property Editor</b> can be seen at design time, "
			       "and you can open a preview of your form in different styles.</p>"
			       "<p>You can change the grid resolution, or turn the grid off in the "
			       "<b>Preferences</b> dialog in the <b>Edit</b> menu."
			       "<p>You can have several forms open, and all open forms are listed "
			       "in the <b>Form List</b>.") );

    connect( fw, SIGNAL( showProperties( QWidget * ) ),
 	     this, SLOT( showProperties( QWidget * ) ) );
    connect( fw, SIGNAL( updateProperties( QWidget * ) ),
 	     this, SLOT( updateProperties( QWidget * ) ) );
    connect( this, SIGNAL( currentToolChanged() ),
	     fw, SLOT( currentToolChanged() ) );
    connect( fw, SIGNAL( selectionChanged() ),
	     this, SLOT( selectionChanged() ) );
    connect( fw, SIGNAL( undoRedoChanged( bool, bool, const QString &, const QString & ) ),
	     this, SLOT( updateUndoRedo( bool, bool, const QString &, const QString & ) ) );
    connect( fw, SIGNAL( fileNameChanged( const QString &, FormWindow * ) ),
	     formlist(), SLOT( fileNameChanged( const QString &, FormWindow * ) ) );
    connect( fw, SIGNAL( modificationChanged( bool, FormWindow * ) ),
	     formlist(), SLOT( modificationChanged( bool, FormWindow * ) ) );
    connect( fw, SIGNAL( modificationChanged( bool, FormWindow * ) ),
	     this, SIGNAL( formModified( bool ) ) );

    formlist()->addForm( fw );

    fw->show();
    fw->currentToolChanged();
    if ( fw->caption().isEmpty() && qstrlen( fw->name() )  )
	fw->setCaption( fw->name() );
    fw->mainContainer()->setCaption( fw->caption() );
    activeWindowChanged( fw );
}

bool MainWindow::unregisterClient( FormWindow *w )
{
    propertyEditor->closed( w );
    objectHierarchy()->closed( w );
    formList->closed( w );
    if ( w == lastActiveFormWindow )
	lastActiveFormWindow = 0;
	
    return TRUE;
}

void MainWindow::propertyEditorHidden()
{
    actionWindowPropertyEditor->setOn( FALSE );
}

void MainWindow::hierarchyViewHidden()
{
    actionWindowHierarchyView->setOn( FALSE );
}

void MainWindow::formListHidden()
{
    actionWindowFormList->setOn( FALSE );
}

void MainWindow::activeWindowChanged( QWidget *w )
{
    if ( w && w->inherits( "FormWindow" ) ) {
	lastActiveFormWindow = (FormWindow*)w;
	lastActiveFormWindow->updateUndoInfo();
	emit hasActiveForm( TRUE );
	if ( formWindow() ) {
	    formWindow()->emitShowProperties();
	    emit formModified( formWindow()->commandHistory()->isModified() );
	    if ( currentTool() != POINTER_TOOL )
		formWindow()->clearSelection();
	}
	formlist()->activeFormChanged( (FormWindow*)w );
    } else if ( w == propertyEditor ) {
	propertyEditor->resetFocus();
    } else if ( !lastActiveFormWindow ) {
	emit hasActiveForm( FALSE );
	actionEditUndo->setEnabled( FALSE );
	actionEditRedo->setEnabled( FALSE );
    }
    selectionChanged();
}

static QString textNoAccel( const QString& text)
{
    QString t = text;
    int i;
    while ( (i = t.find('&') )>= 0 ) {
	t.remove(i,1);
    }
    return t;
}


void MainWindow::updateUndoRedo( bool undoAvailable, bool redoAvailable,
				 const QString &undoCmd, const QString &redoCmd )
{
    actionEditUndo->setEnabled( undoAvailable );
    actionEditRedo->setEnabled( redoAvailable );
    if ( !undoCmd.isEmpty() )
	actionEditUndo->setMenuText( tr( "&Undo: %1" ).arg( undoCmd ) );
    else
	actionEditUndo->setMenuText( tr( "&Undo: Not Available" ) );
    if ( !redoCmd.isEmpty() )
	actionEditRedo->setMenuText( tr( "&Redo: %1" ).arg( redoCmd ) );
    else
	actionEditRedo->setMenuText( tr( "&Redo: Not Available" ) );

    actionEditUndo->setToolTip( textNoAccel( actionEditUndo->menuText()) );
    actionEditRedo->setToolTip( textNoAccel( actionEditRedo->menuText()) );

    if ( currentTool() == ORDER_TOOL ) {
	actionEditUndo->setEnabled( FALSE );
	actionEditRedo->setEnabled( FALSE );
    }
}

QWorkspace *MainWindow::workSpace() const
{
    return workspace;
}

void MainWindow::popupFormWindoMenu( const QPoint & gp, FormWindow *fw )
{
    QValueList<int> ids;
    QMap<QString, int> commands;

    setupRMBSpecialCommands( ids, commands, fw );
    setupRMBProperties( ids, commands, fw );

    qApp->processEvents();
    int r = rmbFormWindow->exec( gp );

    handleRMBProperties( r, commands, fw );
    handleRMBSpecialCommands( r, commands, fw );

    for ( QValueList<int>::Iterator i = ids.begin(); i != ids.end(); ++i )
	rmbFormWindow->removeItem( *i );
}

void MainWindow::popupWidgetMenu( const QPoint &gp, FormWindow * /*fw*/, QWidget * w)
{
    QValueList<int> ids;
    QMap<QString, int> commands;

    setupRMBSpecialCommands( ids, commands, w );
    setupRMBProperties( ids, commands, w );

    qApp->processEvents();
    int r = rmbWidgets->exec( gp );

    handleRMBProperties( r, commands, w );
    handleRMBSpecialCommands( r, commands, w );

    for ( QValueList<int>::Iterator i = ids.begin(); i != ids.end(); ++i )
	rmbWidgets->removeItem( *i );
}

void MainWindow::setupRMBProperties( QValueList<int> &ids, QMap<QString, int> &props, QWidget *w )
{
    const QMetaProperty* text = w->metaObject()->property( "text", TRUE );
    if ( text && qstrcmp( text->type(), "QString") != 0 )
	text = 0;
    const QMetaProperty* title = w->metaObject()->property( "title", TRUE );
    if ( title && qstrcmp( title->type(), "QString") != 0 )
	title = 0;
    const QMetaProperty* pagetitle = w->metaObject()->property( "pageTitle", TRUE );
    if ( pagetitle && qstrcmp( pagetitle->type(), "QString") != 0 )
	pagetitle = 0;
    const QMetaProperty* pixmap = w->metaObject()->property( "pixmap", TRUE );
    if ( pixmap && qstrcmp( pixmap->type(), "QPixmap") != 0 )
	pixmap = 0;

    if ( text && text->designable() ||
	 title && title->designable() ||
	 pagetitle && pagetitle->designable() ||
	 pixmap && pixmap->designable() ) {
	int id = 0;
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator(0);
	if ( pixmap && pixmap->designable() ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Choose Pixmap..."), -1, 0) );
	    props.insert( "pixmap", id );
	}
	if ( text && text->designable() && !w->inherits( "QMultiLineEdit" ) ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Text..."), -1, 0) );
	    props.insert( "text", id );
	}
	if ( title && title->designable() ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Title..."), -1, 0) );
	    props.insert( "title", id );
	}
	if ( pagetitle && pagetitle->designable() ) {
	    ids << ( id = rmbWidgets->insertItem( tr("Edit Page Title..."), -1, 0) );
	    props.insert( "pagetitle", id );
	}
    }
}

void MainWindow::setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, QWidget *w )
{
    int id;

    if ( w->inherits( "QTabWidget" ) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	if ( ( (QDesignerTabWidget*)w )->count() > 1) {
	    ids << ( id = rmbWidgets->insertItem( tr("Remove Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}
	ids << ( id = rmbWidgets->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );
    }
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	if ( ids.isEmpty() )
	    ids << rmbWidgets->insertSeparator( 0 );
	ids << ( id = rmbWidgets->insertItem( tr("Edit..."), -1, 0 ) );
	commands.insert( "edit", id );
    }
}

void MainWindow::setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, FormWindow *fw )
{
    int id;

    if ( fw->mainContainer()->inherits( "QWizard" ) ) {
	if ( ids.isEmpty() )
	    ids << rmbFormWindow->insertSeparator( 0 );
	if ( ( (QWizard*)fw->mainContainer() )->pageCount() > 1) {
	    ids << ( id = rmbFormWindow->insertItem( tr("Remove Page"), -1, 0 ) );
	    commands.insert( "remove", id );
	}
	ids << ( id = rmbFormWindow->insertItem( tr("Add Page"), -1, 0 ) );
	commands.insert( "add", id );
    }
}

void MainWindow::handleRMBProperties( int id, QMap<QString, int> &props, QWidget *w )
{
    if ( id == props[ "text" ] ) {
	bool ok = FALSE;
	QString text;
	if ( w->inherits( "QTextView" ) || w->inherits( "QLabel" ) ) {
	    text = TextEditor::getText( this, w->property("text").toString() );
	    ok = !text.isEmpty();
	} else {
	    text = QInputDialog::getText( tr("Text"), tr( "New text" ), w->property("text").toString(), &ok, this );
	}
	if ( ok ) {
	    QString pn( tr( "Set 'text' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "text", w->property( "text" ),
							      text, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "text", TRUE );
	}
    } else if ( id == props[ "title" ] ) {
	bool ok = FALSE;
	QString title = QInputDialog::getText( tr("Title"), tr( "New title" ), w->property("title").toString(), &ok, this );
	if ( ok ) {
	    QString pn( tr( "Set 'title' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "title", w->property( "title" ),
							      title, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "title", TRUE );
	}
    } else if ( id == props[ "pagetitle" ] ) {
	bool ok = FALSE;
	QString text = QInputDialog::getText( tr("Page Title"), tr( "New page title" ), w->property("pageTitle").toString(), &ok, this );
	if ( ok ) {
	    QString pn( tr( "Set 'pageTitle' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "pageTitle", w->property( "pageTitle" ),
							      text, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "pageTitle", TRUE );
	}
    } else if ( id == props[ "pixmap" ] ) {
	QPixmap oldPix = w->property( "pixmap" ).toPixmap();
	QPixmap pix = qChoosePixmap( this, formWindow(), oldPix );
	if ( !pix.isNull() ) {
	    QString pn( tr( "Set 'pixmap' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "pixmap", w->property( "pixmap" ),
							      pix, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "pixmap", TRUE );
	}
    }
}

void MainWindow::handleRMBSpecialCommands( int id, QMap<QString, int> &commands, QWidget *w )
{
    if ( w->inherits( "QTabWidget" ) ) {
	QTabWidget *tw = (QTabWidget*)w;
	if ( id == commands[ "add" ] ) {
	    AddTabPageCommand *cmd = new AddTabPageCommand( tr( "Add Page to %1" ).arg( tw->name() ), formWindow(),
							    tw, "Tab" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( tw->currentPage() ) {
		QDesignerTabWidget *dtw = (QDesignerTabWidget*)tw;
		DeleteTabPageCommand *cmd = new DeleteTabPageCommand( tr( "Remove Page %1 of %2" ).
								      arg( dtw->pageTitle() ).arg( tw->name() ),
								      formWindow(), tw, tw->currentPage() );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	}
    }
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	if ( id == commands[ "edit" ] )
	    WidgetFactory::editWidget( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ), this, w, formWindow() );
    }
}

void MainWindow::handleRMBSpecialCommands( int id, QMap<QString, int> &commands, FormWindow *fw )
{
    if ( fw->mainContainer()->inherits( "QWizard" ) ) {
	QWizard *wiz = (QWizard*)fw->mainContainer();
	if ( id == commands[ "add" ] ) {
	    AddWizardPageCommand *cmd = new AddWizardPageCommand( tr( "Add Page to %1" ).arg( wiz->name() ), formWindow(),
								  wiz, "Page" );
	    formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	} else if ( id == commands[ "remove" ] ) {
	    if ( wiz->currentPage() ) {
		QDesignerWizard *dw = (QDesignerWizard*)wiz;
		DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( tr( "Remove Page %1 of %2" ).
									    arg( dw->pageTitle() ).arg( wiz->name() ),
									    formWindow(), wiz, wiz->currentPage() );
		formWindow()->commandHistory()->addCommand( cmd );
		cmd->execute();
	    }
	}
    }
}

void MainWindow::clipboardChanged()
{
    QString text( qApp->clipboard()->text() );
    QString start( "<!DOCTYPE UI-SELECTION>" );
    actionEditPaste->setEnabled( text.left( start.length() ) == start );
}

void MainWindow::selectionChanged()
{
    layoutChilds = FALSE;
    layoutSelected = FALSE;
    breakLayout = FALSE;
    if ( !formWindow() ) {
	actionEditCut->setEnabled( FALSE );
	actionEditCopy->setEnabled( FALSE );
	actionEditDelete->setEnabled( FALSE );
	actionEditAdjustSize->setEnabled( FALSE );
	actionEditHLayout->setEnabled( FALSE );
	actionEditVLayout->setEnabled( FALSE );
	actionEditGridLayout->setEnabled( FALSE );
	actionEditBreakLayout->setEnabled( FALSE );
	actionEditLower->setEnabled( FALSE );
	actionEditRaise->setEnabled( FALSE );
 	actionEditAdjustSize->setEnabled( FALSE );
	return;
    }
	
    int selectedWidgets = formWindow()->numSelectedWidgets();
    bool enable = selectedWidgets > 0;
    actionEditCut->setEnabled( enable );
    actionEditCopy->setEnabled( enable );
    actionEditDelete->setEnabled( enable );
    actionEditLower->setEnabled( enable );
    actionEditRaise->setEnabled( enable );

    actionEditAdjustSize->setEnabled( FALSE );

    enable = FALSE;
    QWidgetList widgets = formWindow()->selectedWidgets();
    if ( selectedWidgets > 1 ) {
	int unlaidout = 0;
	int laidout = 0;
	for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	    if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
		unlaidout++;
	    else
		laidout++;
	}
	actionEditHLayout->setEnabled( unlaidout > 1 );
	actionEditVLayout->setEnabled( unlaidout > 1 );
	actionEditGridLayout->setEnabled( unlaidout > 1 );
	actionEditBreakLayout->setEnabled( laidout > 0 );
 	actionEditAdjustSize->setEnabled( laidout > 0 );
	layoutSelected = unlaidout > 1;
	breakLayout = laidout > 0;
    } else if ( selectedWidgets == 1 ) {
	QWidget *w = widgets.first();
	bool isContainer = WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ||
			   w == formWindow()->mainContainer();
 	actionEditAdjustSize->setEnabled( !w->parentWidget() ||
 					  WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout );
	
	if ( !isContainer ) {
	    actionEditHLayout->setEnabled( FALSE );
	    actionEditVLayout->setEnabled( FALSE );
	    actionEditGridLayout->setEnabled( FALSE );
	    if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
		actionEditBreakLayout->setEnabled( TRUE );
		breakLayout = TRUE;
	    } else {
		actionEditBreakLayout->setEnabled( FALSE );
	    }
	} else {
	    if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
		if ( !formWindow()->hasInsertedChildren( w ) ) {
		    actionEditHLayout->setEnabled( FALSE );
		    actionEditVLayout->setEnabled( FALSE );
		    actionEditGridLayout->setEnabled( FALSE );
		    actionEditBreakLayout->setEnabled( FALSE );
		} else {
		    actionEditHLayout->setEnabled( TRUE );
		    actionEditVLayout->setEnabled( TRUE );
		    actionEditGridLayout->setEnabled( TRUE );
		    actionEditBreakLayout->setEnabled( FALSE );
		    layoutChilds = TRUE;
		}
		if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout ) {
		    actionEditBreakLayout->setEnabled( TRUE );
		    breakLayout = TRUE;
		}
	    } else {
		actionEditHLayout->setEnabled( FALSE );
		actionEditVLayout->setEnabled( FALSE );
		actionEditGridLayout->setEnabled( FALSE );
		actionEditBreakLayout->setEnabled( TRUE );
		breakLayout = TRUE;
	    }
	}
    } else if ( selectedWidgets == 0 && formWindow() ) {
 	actionEditAdjustSize->setEnabled( TRUE );
	QWidget *w = formWindow()->mainContainer();
	if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout ) {
	    if ( !formWindow()->hasInsertedChildren( w ) ) {
		actionEditHLayout->setEnabled( FALSE );
		actionEditVLayout->setEnabled( FALSE );
		actionEditGridLayout->setEnabled( FALSE );
		actionEditBreakLayout->setEnabled( FALSE );
	    } else {
		actionEditHLayout->setEnabled( TRUE );
		actionEditVLayout->setEnabled( TRUE );
		actionEditGridLayout->setEnabled( TRUE );
		actionEditBreakLayout->setEnabled( FALSE );
		layoutChilds = TRUE;
	    }
	} else {
	    actionEditHLayout->setEnabled( FALSE );
	    actionEditVLayout->setEnabled( FALSE );
	    actionEditGridLayout->setEnabled( FALSE );
	    actionEditBreakLayout->setEnabled( TRUE );
	    breakLayout = TRUE;
	}
    } else {
	actionEditHLayout->setEnabled( FALSE );
	actionEditVLayout->setEnabled( FALSE );
	actionEditGridLayout->setEnabled( FALSE );
	actionEditBreakLayout->setEnabled( FALSE );
    }
}

static QString fixArgs( const QString &s2 )
{
    QString s = s2;
    return s.replace( QRegExp( "," ), ";" );
}

void MainWindow::writeConfig()
{
    QString fn = QDir::homeDirPath() + "/.designerrc";
    Config config( fn );
    config.setGroup( "General" );
    config.writeEntry( "RestoreWorkspace", restoreConfig );
    config.writeEntry( "SplashScreen", splashScreen );
    config.writeEntry( "DocPath", docPath );
    config.setGroup( "Grid" );
    config.writeEntry( "Snap", snGrid );
    config.writeEntry( "Show", sGrid );
    config.writeEntry( "x", grid().x() );
    config.writeEntry( "y", grid().y() );
    config.setGroup( "Background" );
    config.writeEntry( "UsePixmap", backPix );
    config.writeEntry( "Color", (int)workspace->backgroundColor().rgb() );
    if ( workspace->backgroundPixmap() )
	workspace->backgroundPixmap()->save( QDir::home().absPath() + "/.designer/" + "background.xpm", "XPM" );
    config.setGroup( "Geometries" );
    config.writeEntry( "MainwindowX", x() );
    config.writeEntry( "MainwindowY", y() );
    config.writeEntry( "MainwindowWidth", width() );
    config.writeEntry( "MainwindowHeight", height() );
    config.writeEntry( "PropertyEditorX", propertyEditor->parentWidget()->x() );
    config.writeEntry( "PropertyEditorY", propertyEditor->parentWidget()->y() );
    config.writeEntry( "PropertyEditorWidth", propertyEditor->parentWidget()->width() );
    config.writeEntry( "PropertyEditorHeight", propertyEditor->parentWidget()->height() );
    config.writeEntry( "HierarchyViewX", hierarchyView->parentWidget()->x() );
    config.writeEntry( "HierarchyViewY", hierarchyView->parentWidget()->y() );
    config.writeEntry( "HierarchyViewWidth", hierarchyView->parentWidget()->width() );
    config.writeEntry( "HierarchyViewHeight", hierarchyView->parentWidget()->height() );
    config.writeEntry( "FormListX", formList->parentWidget()->x() );
    config.writeEntry( "FormListY", formList->parentWidget()->y() );
    config.writeEntry( "FormListWidth", formList->parentWidget()->width() );
    config.writeEntry( "FormListHeight", formList->parentWidget()->height() );
    config.setGroup( "View" );
    config.writeEntry( "TextLabels", usesTextLabel() );
    config.writeEntry( "BigIcons", usesBigPixmaps() );
    config.writeEntry( "PropertyEditor", actionWindowPropertyEditor->isOn() );
    config.writeEntry( "HierarchyView", actionWindowHierarchyView->isOn() );
    config.writeEntry( "FormList", actionWindowFormList->isOn() );

#ifndef Q_TEMPLATE_NEEDS_CLASS_DECLARATION
    QList<QToolBar> tbl;
    ToolBarDock da[] = { Left, Right, Top, Bottom, Minimized };
    QMap< QString, int > docks;
    QMap< QString, int > indices;
    QMap< QString, int > nls;
    QMap< QString, int > eos;
    int j = 0;
    int i;
    for ( i = 0; i < 5; ++i ) {
	tbl = toolBars( da[ i ] );
	QToolBar *tb = tbl.first();
	while ( tb ) {
	    ToolBarDock dock;
	    int index;
	    bool nl;
	    int extraOffset;
	    if ( getLocation( tb, dock, index, nl, extraOffset ) ) {
		docks[ QString::number( j ) + tb->label() ] = dock;
		indices[ QString::number( j ) + tb->label() ] = index;
		nls[ QString::number( j ) + tb->label() ] = nl;
		eos[ QString::number( j ) + tb->label() ] = extraOffset;
		++j;
	    }
	    tb = tbl.next();
	}
    }
    QFile file( fn + "tb" );
    file.open( IO_WriteOnly );
    QDataStream s( &file );
    s << docks;
    s << indices;
    s << nls;
    s << eos;
#endif
    
    config.setGroup( "CustomWidgets" );

    QList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    config.writeEntry( "num", (int)lst->count() );
    j = 0;
    QDir::home().mkdir( ".designer" );
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	QStringList l;
	l << w->className;
	l << w->includeFile;
	l << QString::number( (int)w->includePolicy );
	l << QString::number( w->sizeHint.width() );
	l << QString::number( w->sizeHint.height() );
	l << QString::number( w->lstSignals.count() );
	for ( QValueList<QCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
	    l << QString( fixArgs( *it ) );
	l << QString::number( w->lstSlots.count() );
	for ( QValueList<MetaDataBase::Slot>::Iterator it2 = w->lstSlots.begin(); it2 != w->lstSlots.end(); ++it2 ) {
	    l << fixArgs( (*it2).slot );
	    l << (*it2).access;
	}
	l << QString::number( w->lstProperties.count() );
	for ( QValueList<MetaDataBase::Property>::Iterator it3 = w->lstProperties.begin(); it3 != w->lstProperties.end(); ++it3 ) {
	    l << (*it3).property;
	    l << (*it3).type;
	}
	l << QString::number( size_type_to_int( w->sizePolicy.horData() ) );
	l << QString::number( size_type_to_int( w->sizePolicy.verData() ) );
	l << QString::number( (int)w->isContainer );
	config.writeEntry( "Widget" + QString::number( j++ ), l, ',' );
	w->pixmap->save( QDir::home().absPath() + "/.designer/" + w->className, "XPM" );
    }

    config.setGroup( "Import" );
    config.writeEntry( "numFilters", Import::numFilters() );
    for ( i = 0; i < Import::numFilters(); i++ ) {
	config.writeEntry( "wildcard[" + QString::number( i ) + "]", Import::wildcard( i ) );
	config.writeEntry( "command[" + QString::number( i ) + "]", Import::command( i ) );
    }

    config.write();
}

static QString fixArgs2( const QString &s2 )
{
    QString s = s2;
    return s.replace( QRegExp( ";" ), "," );
}

void MainWindow::readConfig()
{
    QString fn = QDir::homeDirPath() + "/.designerrc";
    if ( !QFile::exists( fn ) ) {
	fn = "/etc/designerrc";
	if ( !QFile::exists( fn ) )
	    return;
    }
    Config config( fn );
    config.setGroup( "General" );
    restoreConfig = config.readBoolEntry( "RestoreWorkspace", TRUE );
    docPath = config.readEntry( "DocPath", docPath );
    templPath = config.readEntry( "TemplatePath", QString::null );
    config.setGroup( "Import" );
    int num = config.readNumEntry( "numFilters" );
    int i;
    for ( i = 0; i < num; ++i ) {
	QString wildcard = config.readEntry( "wildcard[" + QString::number( i ) + "]" );
	QString command = config.readEntry( "command[" + QString::number( i ) + "]" );
	Import::addImportEntry( wildcard, command );
    }
    config.setGroup( "General" );
    if ( restoreConfig ) {
	splashScreen = config.readBoolEntry( "SplashScreen", TRUE );
	config.setGroup( "Background" );
	backPix = config.readBoolEntry( "UsePixmap", TRUE );
	if ( backPix ) {
	    QPixmap pix;
	    pix.load( QDir::home().absPath() + "/.designer/" + "background.xpm" );
	    if ( !pix.isNull() )
		workspace->setBackgroundPixmap( pix );
	} else {
	    workspace->setBackgroundColor( QColor( (QRgb)config.readNumEntry( "Color" ) ) );
	}
	config.setGroup( "Grid" );
	sGrid = config.readBoolEntry( "Show", TRUE );
	snGrid = config.readBoolEntry( "Snap", TRUE );
	grd.setX( config.readNumEntry( "x", 10 ) );
	grd.setY( config.readNumEntry( "y", 10 ) );
	config.setGroup( "Geometries" );
	QRect r( pos(), size() );
	r.setX( config.readNumEntry( "MainwindowX", r.x() ) );
	r.setY( config.readNumEntry( "MainwindowY", r.y() ) );
	r.setWidth( config.readNumEntry( "MainwindowWidth", r.width() ) );
	r.setHeight( config.readNumEntry( "MainwindowHeight", r.height() ) );
	resize( r.size() );
	move( r.topLeft() );
	config.setGroup( "Geometries" );
	r.setX( config.readNumEntry( "PropertyEditorX", r.x() ) );
	r.setY( config.readNumEntry( "PropertyEditorY", r.y() ) );
	r.setWidth( config.readNumEntry( "PropertyEditorWidth", r.width() ) );
	r.setHeight( config.readNumEntry( "PropertyEditorHeight", r.height() ) );
	propertyEditor->parentWidget()->setGeometry( r );
	propGeom = r;
	r.setX( config.readNumEntry( "HierarchyViewX", r.x() ) );
	r.setY( config.readNumEntry( "HierarchyViewY", r.y() ) );
	r.setWidth( config.readNumEntry( "HierarchyViewWidth", r.width() ) );
	r.setHeight( config.readNumEntry( "HierarchyViewHeight", r.height() ) );
	hierarchyView->parentWidget()->setGeometry( r );
	hvGeom = r;
	r.setX( config.readNumEntry( "FormListX", r.x() ) );
	r.setY( config.readNumEntry( "FormListY", r.y() ) );
	r.setWidth( config.readNumEntry( "FormListWidth", r.width() ) );
	r.setHeight( config.readNumEntry( "FormListHeight", r.height() ) );
	formList->parentWidget()->setGeometry( r );
	flGeom = r;
	config.setGroup( "View" );
	setUsesTextLabel( config.readBoolEntry( "TextLabels", FALSE ) );
	setUsesBigPixmaps( FALSE /*config.readBoolEntry( "BigIcons", FALSE )*/ ); // ### disabled for now
	actionWindowPropertyEditor->setOn( config.readBoolEntry( "PropertyEditor", FALSE ) );
	actionWindowHierarchyView->setOn( config.readBoolEntry( "HierarchyView", FALSE ) );
	actionWindowFormList->setOn( config.readBoolEntry( "FormList", FALSE ) );
    }

    config.setGroup( "CustomWidgets" );
    num = config.readNumEntry( "num" );
    for ( i = 0; i < num; ++i ) {
	MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
	QStringList l = config.readListEntry( "Widget" + QString::number( i ), ',' );
	w->className = l[ 0 ];
	w->includeFile = l[ 1 ];
	w->includePolicy = (MetaDataBase::CustomWidget::IncludePolicy)l[ 2 ].toInt();
	w->sizeHint.setWidth( l[ 3 ].toInt() );
	w->sizeHint.setHeight( l[ 4 ].toInt() );
	uint c = 5;
	if ( l.count() > c ) {
	    int numSignals = l[ c ].toInt();
	    c++;
	    for ( int j = 0; j < numSignals; ++j, c++ )
		w->lstSignals.append( fixArgs2( l[ c ] ).latin1() );
	}
	if ( l.count() > c ) {
	    int numSlots = l[ c ].toInt();
	    c++;
	    for ( int j = 0; j < numSlots; ++j ) {
		MetaDataBase::Slot slot;
		slot.slot = fixArgs2( l[ c ] );
		c++;
		slot.access = l[ c ];
		c++;
		w->lstSlots.append( slot );
	    }
	}
	if ( l.count() > c ) {
	    int numProperties = l[ c ].toInt();
	    c++;
	    for ( int j = 0; j < numProperties; ++j ) {
		MetaDataBase::Property prop;
		prop.property = l[ c ];
		c++;
		prop.type = l[ c ];
		c++;
		w->lstProperties.append( prop );
	    }
	} if ( l.count() > c ) {
	    QSizePolicy::SizeType h, v;
	     h = int_to_size_type( l[ c++ ].toInt() );
	     v = int_to_size_type( l[ c++ ].toInt() );
	     w->sizePolicy = QSizePolicy( h, v );
	}
	if ( l.count() > c ) {
	    w->isContainer = (bool)l[ c++ ].toInt();
	}
	w->pixmap = new QPixmap( PixmapChooser::loadPixmap( QDir::home().absPath() + "/.designer/" + w->className ) );
	MetaDataBase::addCustomWidget( w );
    }
    if ( num > 0 )
	rebuildCustomWidgetGUI();

    if ( !restoreConfig )
	return;

    QFile f( fn + "tb" );
    bool tbconfig = f.open( IO_ReadOnly );
    QMap< QString, int > docks;
    QMap< QString, int > indices;
    QMap< QString, int > nls;
    QMap< QString, int > eos;
    if ( tbconfig ) {
	QDataStream s( &f );
	s >> docks;
	s >> indices;
	s >> nls;
	s >> eos;
    }

    if ( tbconfig ) {
	QMap< QString, int >::Iterator dit, iit;
	QMap< QString, int >::Iterator nit, eit;
	dit = docks.begin();
	iit = indices.begin();
	nit = nls.begin();
	eit = eos.begin();
	QObjectList *l = queryList( "QToolBar" );
	if ( !l )
	    return;
	for ( ; dit != docks.end(); ++dit, ++iit, ++nit, ++eit ) {
	    QString n = dit.key();
	    while ( n[ 0 ].isNumber() )
		n.remove( 0, 1 );
	    QToolBar *tb = 0;
	    for ( tb = (QToolBar*)l->first(); tb; tb = (QToolBar*)l->next() ) {
		if ( tb->label() == n )
		    break;
	    }
	    if ( tb )
		moveToolBar( tb, (ToolBarDock)*dit, (bool)*nit, *iit, *eit );
	}
	delete l;
    }
}

HierarchyView *MainWindow::objectHierarchy() const
{
    if ( !hierarchyView )
	( (MainWindow*)this )->setupHierarchyView();
    return hierarchyView;
}

QPopupMenu *MainWindow::setupNormalHierarchyMenu( QWidget *parent )
{
    QPopupMenu *menu = new QPopupMenu( parent );

    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

QPopupMenu *MainWindow::setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot )
{
    QPopupMenu *menu = new QPopupMenu( parent );

    menu->insertItem( tr( "Add Page" ), parent, addSlot );
    menu->insertItem( tr( "Remove Page" ), parent, removeSlot );
    menu->insertSeparator();
    actionEditCut->addTo( menu );
    actionEditCopy->addTo( menu );
    actionEditPaste->addTo( menu );
    actionEditDelete->addTo( menu );

    return menu;
}

void MainWindow::closeEvent( QCloseEvent *e )
{
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	if ( !closeForm( (FormWindow*)w ) ) {
	    e->ignore();
	    return;
	}
    }
    hide();
    if ( help )
	help->close();
    writeConfig();
    e->accept();

    if ( client ) {
	QDir home( QDir::homeDirPath() );
	home.remove( ".designerpid" );
    }
}

bool MainWindow::closeForm( FormWindow *fw )
{
    bool modified = FALSE;
    modified = fw->commandHistory()->isModified();
    if ( modified ) {
	switch ( QMessageBox::warning( this, tr( "Save Form" ),
				       tr( "Save changes of form '%1'?" ).arg( fw->name() ),
				       tr( "&Yes" ), tr( "&No" ), tr( "&Cancel" ), 0, 2 ) ) {
	case 0: // save
	    fw->setFocus();
	    qApp->processEvents();
	    if ( !fileSave() )
		return FALSE;
	    break;
	case 1: // don't save
	    break;
	case 2: // cancel
	    return FALSE;
	default:
	    break;
	}
    }	
    return TRUE;
}

FormList *MainWindow::formlist() const
{
    if ( !formList )
	( (MainWindow*)this )->setupFormList();
    return formList;
}

bool MainWindow::openEditor( QWidget *w )
{
    if ( WidgetFactory::hasSpecialEditor( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ) {
	statusBar()->message( tr( "Edit %1..." ).arg( w->className() ) );
	WidgetFactory::editWidget( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ), this, w, formWindow() );
	statusBar()->clear();
	return TRUE;
    }

    const QMetaProperty* text = w->metaObject()->property( "text", TRUE );
    const QMetaProperty* title = w->metaObject()->property( "title", TRUE );
    if ( text && text->designable() ) {
	bool ok = FALSE;
	QString text;
	if ( w->inherits( "QTextView" ) || w->inherits( "QLabel" ) ) {
	    text = TextEditor::getText( this, w->property("text").toString() );
	    ok = !text.isEmpty();
	} else {
	    text = QInputDialog::getText( tr("Text"), tr( "New text" ), w->property("text").toString(), &ok, this );
	}
	if ( ok ) {
	    QString pn( tr( "Set 'text' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "text", w->property( "text" ),
							      text, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "text", TRUE );
	}
	return TRUE;
    }
    if ( title && title->designable() ) {
	bool ok = FALSE;
	QString text;
	text = QInputDialog::getText( tr("Title"), tr( "New title" ), w->property("title").toString(), &ok, this );
	if ( ok ) {
	    QString pn( tr( "Set 'title' of '%2'" ).arg( w->name() ) );
	    SetPropertyCommand *cmd = new SetPropertyCommand( pn, formWindow(), w, propertyEditor,
							      "title", w->property( "title" ),
							      text, QString::null, QString::null );
	    cmd->execute();
	    formWindow()->commandHistory()->addCommand( cmd );
	    MetaDataBase::setPropertyChanged( w, "title", TRUE );
	}
	return TRUE;
    }

    return TRUE;
}

void MainWindow::rebuildCustomWidgetGUI()
{
    customWidgetToolBar->clear();
    customWidgetMenu->clear();
    int count = 0;
    QList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();

    actionToolsCustomWidget->addTo( customWidgetMenu );
    customWidgetMenu->insertSeparator();

    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	QAction* a = new QAction( actionGroupTools, QString::number( w->id ).latin1() );
	a->setToggleAction( TRUE );
	a->setText( w->className );
	a->setIconSet( *w->pixmap );
	a->setStatusTip( tr( "Insert a " +w->className + " (custom widget)" ) );
	a->setWhatsThis( tr("<b>" + w->className + " (custom widget)</b>"
			    "<p>Select <b>Edit Custom Widgets...</b> in the <b>Tools->Custom</b> menu to "
			    "add and change the custom widgets. You can add properties as well as "
			    "signals and slots to integrate them into the designer, "
			    "and provide a pixmap which will be used to represent the widget on the form.</p>") );
	
	a->addTo( customWidgetToolBar );
	a->addTo( customWidgetMenu);
	count++;
    }

    if ( count == 0 )
	customWidgetToolBar->hide();
    else
	customWidgetToolBar->show();
}

bool MainWindow::isCustomWidgetUsed( MetaDataBase::CustomWidget *wid )
{
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( w->inherits( "FormWindow" ) ) {
	    if ( ( (FormWindow*)w )->isCustomWidgetUsed( wid ) )
		return TRUE;
	}
    }
    return FALSE;
}

void MainWindow::setGrid( const QPoint &p )
{
    if ( p == grd )
	return;
    grd = p;
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setShowGrid( bool b )
{
    if ( b == sGrid )
	return;
    sGrid = b;
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	( (FormWindow*)w )->mainContainer()->update();
    }
}

void MainWindow::setSnapGrid( bool b )
{
    if ( b == snGrid )
	return;
    snGrid = b;
}

QString MainWindow::documentationPath() const
{
    QString result = docPath;

    if ( docPath[0] == '$' ) {
	int fs = docPath.find('/');
	if ( fs == -1 )
	    fs = docPath.find('\\');

	if ( fs > -1 ) {
	    result = docPath.mid( 1, fs-1 );
	} else {
	    fs=docPath.length();
	    result = docPath.right(fs-1);
	}
	result = getenv(result.latin1()) + docPath.right( docPath.length()-fs );
    }

    return result;
}

void MainWindow::chooseDocPath()
{
    if ( !prefDia )
	return;
    QString fn = QFileDialog::getExistingDirectory( QString::null, this );
    if ( !fn.isEmpty() )
	prefDia->editDocPath->setText( fn );
}

void MainWindow::windowsMenuActivated( int id )
{
    QWidget* w = workspace->windowList().at( id );
    if ( w )
	w->setFocus();
}

void MainWindow::closeAllForms()
{
    QWidgetList windows = workSpace()->windowList();
    for ( QWidget *w = windows.first(); w; w = windows.next() ) {
	if ( !w->inherits( "FormWindow" ) )
	    continue;
	w->close();
    }
}

void MainWindow::createNewTemplate()
{
    CreateTemplate *dia = (CreateTemplate*)sender()->parent();
    QString fn = dia->editName->text();
    QString cn = dia->listClass->currentText();
    if ( fn.isEmpty() || cn.isEmpty() ) {
	QMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
	return;
    }
    fn.prepend( QString( getenv( "QTDIR" ) ) + "/tools/designer/templates/" );
    fn.append( ".ui" );
    QFile f( fn );
    if ( !f.open( IO_WriteOnly ) ) {
	QMessageBox::information( this, tr( "Create Template" ), tr( "Couldn't create the template" ) );
	return;
    }
    QTextStream ts( &f );

    ts << "<!DOCTYPE UI><UI>" << endl;
    ts << "<widget>" << endl;
    ts << "<class>" << cn << "</class>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>name</name>" << endl;
    ts << "    <cstring>" << cn << "Form</cstring>" << endl;
    ts << "</property>" << endl;
    ts << "<property stdset=\"1\">" << endl;
    ts << "    <name>geometry</name>" << endl;
    ts << "    <rect>" << endl;
    ts << "        <width>300</width>" << endl;
    ts << "        <height>400</height>" << endl;
    ts << "    </rect>" << endl;
    ts << "</property>" << endl;
    ts << "</widget>" << endl;
    ts << "</UI>" << endl;

    dia->editName->setText( tr( "NewTemplate" ) );

    f.close();
}

QStringList MainWindow::getUiFiles( const QString &profile )
{
    QFile f( profile );
    if ( !f.exists() || !f.open( IO_ReadOnly ) )
	return QStringList();
    QTextStream ts( &f );
    QString contents = ts.read();
    f.close();

    int i = contents.find( "INTERFACES" );
    if ( i == -1 )
	return QStringList();
	
    QString part = contents.mid( i + QString( "INTERFACES" ).length() );
    QStringList lst;
    bool inName = FALSE;
    QString currName;
    for ( i = 0; i < (int)part.length(); ++i ) {
	QChar c = part[ i ];
	if ( ( c.isLetter() || c.isDigit() || c == '.' ) &&
	     c != ' ' && c != '\t' && c != '\n' && c != '=' && c != '\\' ) {
	    if ( !inName )
		currName = QString::null;
	    currName += c;
	    inName = TRUE;
	} else {
	    if ( inName ) {
		inName = FALSE;
		if ( currName.right( 3 ).lower() == ".ui" )
		    lst.append( currName );
	    }
	}
    }

    return lst;
}

void MainWindow::checkTempFiles()
{
    QString baseName = QString( getenv( "HOME" ) ) + "/.designer/saved-form-";
    if ( !QFile::exists( baseName + "1.ui" ) )
	return;
    QString s = QString( getenv( "HOME" ) ) + "/.designer";
    QDir d( s );
    d.setNameFilter( "*.ui" );
    QStringList lst = d.entryList();
    QApplication::restoreOverrideCursor();
    bool load = QMessageBox::information( this, tr( "Restoring last session" ),
					  tr( "The Qt Designer found some temporary saved files, which have been\n"
					      "written when the Qt Designer crashed last time. Do you want to\n"
					      "load these files?" ), tr( "&Yes" ), tr( "&No" ) ) == 0;
    QApplication::setOverrideCursor( waitCursor );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	if ( load )
	    openFile( QString( getenv( "HOME" ) ) + "/.designer/" + *it, FALSE );
	d.remove( *it );
    }
}

void MainWindow::openHelpForDialog( const QString &dia )
{
    QString manualdir = QString( getenv( "QTDIR" ) ) + "/tools/designer/manual/book1.html";
    if ( !QFile::exists( manualdir ) )
	manualdir = QString( getenv( "QTDIR" ) ) + "/doc/html/designer/book1.html";
    QFile file( manualdir );
    if ( !file.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &file );
    QString text = ts.read();

    int i = text.find( dia );
    if ( i == -1 )
	return;
    i = text.findRev( "href=\"", i );
    if ( i == -1 )
	return;
    int end = text.find( "\"", i + 8 );
    i += QString( "href=\"" ).length();
    QString page = text.mid( i, end - i );

    if ( !help ) {
	help = new Help( documentationPath(), this, "help" );
	help->setSource( page );
	help->setUsesBigPixmaps( FALSE /*usesBigPixmaps()*/ ); // ### disbaled for now
	help->setUsesTextLabel( usesTextLabel() );
    }
    help->setSource( page );
    help->show();
    help->raise();
}

void MainWindow::showDialogHelp()
{
    QWidget *w = (QWidget*)sender();
    w = w->topLevelWidget();

    // dialog classname to documentation title mapping
    if ( w->inherits( "AboutDialog" ) )
	openHelpForDialog( "The About Dialog" );
    else if ( w->inherits( "ConnectionEditorBase" ) )
	openHelpForDialog( "The Connection Editor Dialog (Edit Connections)" );
    else if ( w->inherits( "ConnectionViewerBase" ) )
	openHelpForDialog( "The Connection Viewer Dialog (Edit Connections)" );
    else if ( w->inherits( "CustomWidgetEditorBase" ) )
	openHelpForDialog( "The Edit Custom Widgets Dialog" );
    else if ( w->inherits( "IconViewEditorBase" ) )
	openHelpForDialog( "The Edit Icon View Dialog" );
    else if ( w->inherits( "ListBoxEditorBase" ) )
	openHelpForDialog( "The Edit List Box Dialog" );
    else if ( w->inherits( "ListViewEditorBase" ) )
	openHelpForDialog( "The Edit List View Dialog" );
    else if ( w->inherits( "MultiLineEditorBase" ) )
	openHelpForDialog( "The Edit Multiline Edit Dialog" );
    else if ( w->inherits( "EditSlotsBase" ) )
	openHelpForDialog( "The Edit Slots Dialog" );
    else if ( w->inherits( "FormSettingsBase" ) )
	openHelpForDialog( "The Form Settings Dialog" );
    else if ( w->inherits( "HelpDialogBase" ) )
	openHelpForDialog( "The Help Dialog" );
    else if ( w->inherits( "NewFormBase" ) )
	openHelpForDialog( "The New Form Dialog" );
    else if ( w->inherits( "PaletteEditorBase" ) )
	openHelpForDialog( "The Palette Editor Dialog" );
    else if ( w->inherits( "PixmapFunction" ) )
	openHelpForDialog( "The Pixmap Function Dialog" );
    else if ( w->inherits( "Preferences" ) )
	openHelpForDialog( "The Preferences Dialog" );
    else if ( w->inherits( "TopicChooserBase" ) )
	openHelpForDialog( "The Topic Chooser Dialog" );
}
