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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "metadatabase.h"

#if defined(HAVE_KDE)
#include <kmainwindow.h>
#else
#include <qmainwindow.h>
#endif

#include <qmap.h>
#include <qguardedptr.h>

class PropertyEditor;
class QWorkspace;
class QMenuBar;
class FormWindow;
class QAction;
class QActionGroup;
class QPopupMenu;
class HierarchyView;
class QCloseEvent;
class FormList;
class Help;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
#include <qtoolbar.h>
#else
class QToolBar;
#endif
class Preferences;

#if defined(HAVE_KDE)
#define QMainWindow KMainWindow
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

#undef QMainWindow

public:
    MainWindow( bool asClient );
    ~MainWindow();

    HierarchyView *objectHierarchy() const;
    FormList *formlist() const;

    void resetTool();
    int currentTool() const;

    FormWindow *formWindow();

    bool unregisterClient( FormWindow *w );
    QWidget *isAFormWindowChild( QObject *o ) const;

    void insertFormWindow( FormWindow *fw );
    QWorkspace *workSpace() const;

    void popupFormWindoMenu( const QPoint &gp, FormWindow *fw );
    void popupWidgetMenu( const QPoint &gp, FormWindow *fw, QWidget *w );

    QPopupMenu *setupNormalHierarchyMenu( QWidget *parent );
    QPopupMenu *setupTabWidgetHierarchyMenu( QWidget *parent, const char *addSlot, const char *removeSlot );

    void openFile( const QString &fn, bool validFileName = TRUE );
    bool isCustomWidgetUsed( MetaDataBase::CustomWidget *w );

    void setGrid( const QPoint &p );
    void setShowGrid( bool b );
    void setSnapGrid( bool b );
    QPoint grid() const { return grd; }
    bool showGrid() const { return sGrid; }
    bool snapGrid() const { return snGrid && sGrid; }

    QString documentationPath() const;

    static MainWindow *self;
    void saveAllTemp();

    QString templatePath() const { return templPath; }
    
public slots:
    void showProperties( QWidget *w );
    void updateProperties( QWidget *w );
    void showDialogHelp();

signals:
    void currentToolChanged();
    void hasActiveForm( bool );
    void formModified( bool );

protected:
    bool eventFilter( QObject *o, QEvent *e );
    void closeEvent( QCloseEvent *e );

private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void fileSaveAll();
    void fileCreateTemplate();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editLower();
    void editRaise();
    void editAdjustSize();
    void editLayoutHorizontal();
    void editLayoutVertical();
    void editLayoutGrid();
    void editLayoutContainerHorizontal();
    void editLayoutContainerVertical();
    void editLayoutContainerGrid();
    void editBreakLayout();
    void editAccels();
    void editSlots();
    void editConnections();
    void editFormSettings();
    void editPreferences();

    void previewForm();
    void previewFormMotif();
    void previewFormWindows();
    void previewFormCDE();
    void previewFormPlatinum();
    void previewFormSGI();
    void previewFormMotifPlus();

    void windowPropertyEditor( bool );
    void windowHierarchyView( bool );
    void windowFormList( bool );

    void toolsCustomWidget();

    void helpContents();
    void helpManual();
    void helpAbout();
    void helpAboutQt();
    void helpRegister();
    void propertyEditorHidden();
    void hierarchyViewHidden();
    void formListHidden();

    void activeWindowChanged( QWidget *w );
    void updateUndoRedo( bool, bool, const QString &, const QString & );

    void toolSelected( QAction* );

    void clipboardChanged();
    void selectionChanged();

    void chooseDocPath();
    void windowsMenuActivated( int id );
    void setupWindowActions();

    void closeAllForms();
    void createNewTemplate();

private:
    void setupMDI();
    void setupMenuBar();
    void setupEditActions();
    void setupToolActions();
    void setupLayoutActions();
    void setupFileActions();
    void setupPreviewActions();
    void setupHelpActions();
    void setupRMBMenus();

    void setupPropertyEditor();
    void setupHierarchyView();
    void setupFormList();
    QWidget* previewFormInternal( QStyle* style = 0, QPalette* pal = 0 );

    FormWindow *insertFormWindow( int type );

    void writeConfig();
    void readConfig();

    void setupRMBProperties( QValueList<int> &ids, QMap<QString, int> &props, QWidget *w );
    void handleRMBProperties( int id, QMap<QString, int> &props, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, QWidget *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, QWidget *w );
    void setupRMBSpecialCommands( QValueList<int> &ids, QMap<QString, int> &commands, FormWindow *w );
    void handleRMBSpecialCommands( int id, QMap<QString, int> &commands, FormWindow *w );
    bool closeForm( FormWindow *fw );

    bool openEditor( QWidget *w );
    void rebuildCustomWidgetGUI();

    QStringList getUiFiles( const QString &profile );

    void checkTempFiles();

    void openHelpForDialog( const QString &dia );

private:
    PropertyEditor *propertyEditor;
    HierarchyView *hierarchyView;
    FormList *formList;
    QGuardedPtr<Help> help;
    QWidget *lastPressWidget;
    QWorkspace *workspace;
#if defined(HAVE_KDE)
    KMenuBar *menubar;
#else
    QMenuBar *menubar;
#endif
    FormWindow *lastActiveFormWindow;
    bool breakLayout, layoutChilds, layoutSelected;
    QPoint grd;
    bool sGrid, snGrid;
    bool restoreConfig;
    bool backPix;
    bool splashScreen;
    QString docPath;

    QAction *actionWindowPropertyEditor;
    QAction *actionEditUndo, *actionEditRedo, *actionEditCut, *actionEditCopy,
    *actionEditPaste, *actionEditDelete,
    *actionEditAdjustSize,
    *actionEditHLayout, *actionEditVLayout, *actionEditGridLayout,
    *actionEditSelectAll, *actionEditBreakLayout, *actionEditSlots, *actionEditConnections,
    *actionEditLower, *actionEditRaise;
    QActionGroup *actionGroupTools;
    QAction* actionPointerTool, *actionConnectTool, *actionOrderTool;
    QAction* actionCurrentTool;
    QAction *actionHelpContents, *actionHelpAbout, *actionHelpAboutQt, *actionHelpWhatsThis;
#if defined(QT_NON_COMMERCIAL)
    QAction *actionHelpRegister;
#endif
    QAction *actionWindowHierarchyView, *actionWindowFormList, *actionHelpManual;
    QAction *actionToolsCustomWidget, *actionEditPreferences;
    QAction *actionWindowTile, *actionWindowCascade, *actionWindowClose, *actionWindowCloseAll;
    QAction *actionWindowNext, *actionWindowPrevious;
    QAction *actionEditFormSettings, *actionEditAccels;

    QPopupMenu *rmbWidgets;
    QPopupMenu *rmbFormWindow;
    QPopupMenu *customWidgetMenu, *windowMenu;
    QToolBar *customWidgetToolBar, *layoutToolBar;

    Preferences *prefDia;
    QMap<QString,QString> propertyDocumentation;
    QRect propGeom, flGeom, hvGeom;
    bool client;
    QString templPath;
    
};

#endif
