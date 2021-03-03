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

#ifndef FORMWINDOW_H
#define FORMWINDOW_H

#include "command.h"
#include "metadatabase.h"
#include "sizehandle.h"

#include <qwidget.h>
#include <qptrdict.h>
#include <qpixmap.h>
#include <qarray.h>
#include <qwidgetlist.h>
#include <qmap.h>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QPainter;
class QLabel;
class MainWindow;
class QTimer;
class QFocusEvent;
class QCloseEvent;
class Resource;
class QResizeEvent;
class BreakLayoutCommand;
class QPixmap;
class QSizeGrip;

#if defined(_CC_MSVC_) || defined(Q_FULL_TEMPLATE_INSTANTIATION)
#include "orderindicator.h"
#else
class OrderIndicator;
#endif

class FormWindow : public QWidget
{
    Q_OBJECT

public:
    FormWindow( MainWindow *mw, QWidget *parent, const char *name = 0 );
    FormWindow( QWidget *parent, const char *name = 0 );
    ~FormWindow();

    void init();
    virtual void setMainWindow( MainWindow *w );

    virtual QString fileName() const;
    virtual void setFileName( const QString &fn );

    virtual QPoint grid() const;
    virtual QPoint gridPoint( const QPoint &p );

    virtual CommandHistory *commandHistory();

    virtual void undo();
    virtual void redo();
    virtual QString copy();
    virtual void paste( const QString &cb, QWidget *parent );
    virtual void lowerWidgets();
    virtual void raiseWidgets();
    virtual void checkAccels();

    virtual void layoutHorizontal();
    virtual void layoutVertical();
    virtual void layoutGrid();

    virtual void layoutHorizontalContainer( QWidget *w );
    virtual void layoutVerticalContainer( QWidget *w );
    virtual void layoutGridContainer( QWidget *w );

    virtual void breakLayout( QWidget *w );

    virtual void selectWidget( QWidget *w, bool select = TRUE );
    virtual void selectAll();
    virtual void updateSelection( QWidget *w );
    virtual void raiseSelection( QWidget *w );
    virtual void repaintSelection( QWidget *w );
    virtual void clearSelection( bool changePropertyDisplay = TRUE );
    virtual void selectWidgets();
    bool isWidgetSelected( QWidget *w );
    virtual void updateChildSelections( QWidget *w );
    virtual void raiseChildSelections( QWidget *w );

    virtual void emitUpdateProperties( QWidget *w );
    virtual void emitShowProperties( QWidget *w = 0 );
    virtual void emitSelectionChanged();

    virtual void setPropertyShowingBlocked( bool b );
    bool isPropertyShowingBlocked() const;

    virtual QLabel *sizePreview() const;
    virtual void checkPreviewGeometry( QRect &r );

    virtual QPtrDict<QWidget> *widgets();
    virtual QWidgetList selectedWidgets() const;

    virtual QWidget *designerWidget( QObject *o ) const;

    virtual void handleMousePress( QMouseEvent *e, QWidget *w );
    virtual void handleMouseRelease( QMouseEvent *e, QWidget *w );
    virtual void handleMouseDblClick( QMouseEvent *e, QWidget *w );
    virtual void handleMouseMove( QMouseEvent *e, QWidget *w );
    virtual void handleKeyPress( QKeyEvent *e, QWidget *w );
    virtual void handleKeyRelease( QKeyEvent *e, QWidget *w );

    virtual void updateUndoInfo();

    virtual MainWindow *mainWindow() const;

    virtual void save( const QString &filename );
    virtual void insertWidget( QWidget *w, bool checkName = FALSE );
    virtual void removeWidget( QWidget *w );
    virtual void deleteWidgets();
    virtual void editAdjustSize();
    virtual void editConnections();

    virtual int numSelectedWidgets() const;
    virtual int numVisibleWidgets() const;

    virtual bool hasInsertedChildren( QWidget *w ) const;

    virtual QWidget *currentWidget() const { return propertyWidget; }
    virtual bool unify( QWidget *w, QString &s, bool changeIt );

    virtual bool isCustomWidgetUsed( MetaDataBase::CustomWidget *w );

    virtual QPoint mapToForm( const QWidget* w, const QPoint&  ) const;

    bool isMainContainer( QWidget *w ) const;
    QWidget *mainContainer() const { return mContainer; }
    void setMainContainer( QWidget *w );

    void paintGrid( QWidget *w, QPaintEvent *e );

    bool savePixmapInline() const;
    QString pixmapLoaderFunction() const;
    void setSavePixmapInline( bool b );
    void setPixmapLoaderFunction( const QString &func );

    void setToolFixed() { toolFixed = TRUE; }

public slots:
    virtual void widgetChanged( QWidget *w );
    virtual void currentToolChanged();
    virtual void visibilityChanged();

signals:
    void showProperties( QWidget *w );
    void updateProperties( QWidget *w );
    void undoRedoChanged( bool undoAvailable, bool redoAvailable,
			  const QString &undoCmd, const QString &redoCmd );
    void selectionChanged();
    void modificationChanged( bool m, FormWindow *fw );
    void fileNameChanged( const QString &s, FormWindow *fw );

protected:
    virtual void closeEvent( QCloseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void resizeEvent( QResizeEvent *e );

private:
    enum RectType { Insert, Rubber };

    virtual void beginUnclippedPainter( bool doNot );
    virtual void endUnclippedPainter();
    virtual void drawConnectLine();
    virtual void drawSizePreview( const QPoint &pos, const QString& text );

    virtual void insertWidget();
    virtual void moveSelectedWidgets( int dx, int dy );

    virtual void startRectDraw( const QPoint &p, const QPoint &global, QWidget *w, RectType t );
    virtual void continueRectDraw( const QPoint &p, const QPoint &global, QWidget *w, RectType t );
    virtual void endRectDraw();

    virtual void checkSelectionsForMove( QWidget *w );
    virtual BreakLayoutCommand *breakLayoutCommand( QWidget *w );

    virtual bool allowMove( QWidget *w );

    virtual void saveBackground();
    virtual void restoreConnectionLine();
    virtual void restoreRect( const QRect &rect ) ;

    virtual void showOrderIndicators();
    virtual void updateOrderIndicators();
    virtual void repositionOrderIndicators();
    virtual void hideOrderIndicators();

    virtual QWidget *containerAt( const QPoint &pos, QWidget *notParentOf );

private slots:
    virtual void invalidCheckedSelections();
    virtual void updatePropertiesTimerDone();
    virtual void showPropertiesTimerDone();
    virtual void selectionChangedTimerDone();
    virtual void modificationChanged( bool m );
    virtual void windowsRepaintWorkaroundTimerTimeout();

private:
    int currTool;
    bool oldRectValid, widgetPressed, drawRubber, checkedSelectionsForMove;
    QRect currRect;
    QPoint rectAnchor;
    QPainter *unclippedPainter;
    QPoint sizePreviewPos;
    QPixmap sizePreviewPixmap;
    MainWindow *mainwindow;
    QList<WidgetSelection> selections;
    QPtrDict<WidgetSelection> usedSelections;
    QRect widgetGeom, rubber;
    QPoint oldPressPos, origPressPos;
    CommandHistory commands;
    QMap<ulong, QPoint> moving;
    QWidget *insertParent, *propertyWidget;
    QLabel *sizePreviewLabel;
    QTimer *checkSelectionsTimer;
    QPtrDict<QWidget> insertedWidgets;
    QString fname;
    bool propShowBlocked;
    QTimer* updatePropertiesTimer, *showPropertiesTimer, *selectionChangedTimer,
    *windowsRepaintWorkaroundTimer;
    QPoint connectStartPos, currentConnectPos;
    QObject *connectSender, *connectReceiver;
    QString filename;
    QPixmap *buffer;
    QList<OrderIndicator> orderIndicators;
    QWidgetList orderedWidgets;
    QWidgetList stackedWidgets;
    QWidget *mContainer;
    bool pixInline;
    QString pixLoader;
    bool toolFixed;

};

#endif
