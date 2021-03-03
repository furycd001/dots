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

#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include <qvariant.h>
#include <qiconset.h>
#include <qstring.h>
#include <qintdict.h>
#include <qtabwidget.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qpainter.h>
#include <qevent.h>
#include <qobjectlist.h>
#include <qlabel.h>
#include <qwizard.h>
#include <qptrdict.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include "metadatabase.h"

class QWidget;
class QLayout;
class FormWindow;

class WidgetFactory : public Qt
{
public:
    enum LayoutType {
	HBox,
	VBox,
	Grid,
	NoLayout
    };

    static QWidget *create( int id, QWidget *parent, const char *name = 0, bool init = TRUE,
			    const QRect *rect = 0, Qt::Orientation orient = Qt::Horizontal );
    static QLayout *createLayout( QWidget *widget, QLayout* layout, LayoutType type );
    static void deleteLayout( QWidget *widget );

    static LayoutType layoutType( QWidget *w );
    static LayoutType layoutType( QWidget *w, QLayout *&layout );
    static LayoutType layoutType( QLayout *layout );
    static QWidget *layoutParent( QLayout *layout );

    static QWidget* containerOfWidget( QWidget *w );
    static QWidget* widgetOfContainer( QWidget *w );

    static bool isPassiveInteractor( QObject* o );
    static const char* classNameOf( QObject* o );

    static void initChangedProperties( QObject *o );

    static bool hasSpecialEditor( int id );
    static bool hasItems( int id );
    static void editWidget( int id, QWidget *parent, QWidget *editWidget, FormWindow *fw );

    static bool canResetProperty( QWidget *w, const QString &propName );
    static bool resetProperty( QWidget *w, const QString &propName );
    static QVariant defaultValue( QWidget *w, const QString &propName );
    static QString defaultCurrentItem( QWidget *w, const QString &propName );

    static QVariant property( QWidget *w, const char *name );

private:
    static QWidget *createWidget( const QString &className, QWidget *parent, const char *name, bool init,
				  const QRect *r = 0, Qt::Orientation orient = Qt::Horizontal );
    static QWidget *createCustomWidget( QWidget *parent, const char *name, MetaDataBase::CustomWidget *w );

};


class QDesignerTabWidget : public QTabWidget
{
    Q_OBJECT
    Q_PROPERTY( int currentPage READ currentPage WRITE setCurrentPage STORED false DESIGNABLE true )
    Q_PROPERTY( QString pageTitle READ pageTitle WRITE setPageTitle STORED false DESIGNABLE true )
    Q_PROPERTY( QCString pageName READ pageName WRITE setPageName STORED false DESIGNABLE true )
public:
    QDesignerTabWidget( QWidget *parent, const char *name ) : QTabWidget( parent, name ) {}

    int currentPage() const;
    void setCurrentPage( int i );
    QString pageTitle() const;
    void setPageTitle( const QString& title );
    QCString pageName() const;
    void setPageName( const QCString& name );

    int count() const;
    QTabBar *tabBar() const { return QTabWidget::tabBar(); }

};

class QDesignerWizard : public QWizard
{
    Q_OBJECT
    Q_PROPERTY( int currentPage READ currentPageNum WRITE setCurrentPage STORED false DESIGNABLE true )
    Q_PROPERTY( QString pageTitle READ pageTitle WRITE setPageTitle STORED false DESIGNABLE true )
    Q_PROPERTY( QCString pageName READ pageName WRITE setPageName STORED false DESIGNABLE true )
public:
    QDesignerWizard( QWidget *parent, const char *name ) : QWizard( parent, name ) {}

    int currentPageNum() const;
    void setCurrentPage( int i );
    QString pageTitle() const;
    void setPageTitle( const QString& title );
    QCString pageName() const;
    void setPageName( const QCString& name );
    int pageNum( QWidget *page );
    void addPage( QWidget *p, const QString & );
    void removePage( QWidget *p );
    void insertPage( QWidget *p, const QString &t, int index );
    bool isPageRemoved( QWidget *p ) { return (bool)removedPages.find( p ); }

    void reject() {}

private:
    struct Page
    {
	Page( QWidget *a, const QString &b ) : p( a ), t( b ) {}
	Page() : p( 0 ), t( QString::null ) {}
	QWidget *p;
	QString t;
    };
    QPtrDict<QWidget> removedPages;

};

class QLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    QLayoutWidget( QWidget *parent, const char *name ) : QWidget( parent, name ), sp( QWidget::sizePolicy() ) {}

    QSizePolicy sizePolicy() const;

protected:
    void paintEvent( QPaintEvent * );
    bool event( QEvent * );
    void updateSizePolicy();
    QSizePolicy sp;

};


class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    CustomWidget( QWidget *parent, const char *name, MetaDataBase::CustomWidget *cw )
	: QWidget( parent, name ), cusw( cw ) {
	    alwaysExpand = parentWidget() && parentWidget()->inherits( "FormWindow" );
	    setSizePolicy( cw->sizePolicy );
    }

    QSize sizeHint() const {
	QSize sh = cusw->sizeHint;
	if ( sh.isValid() )
	    return sh;
	return QWidget::sizeHint();
    }

    QString realClassName() { return cusw->className; }
    MetaDataBase::CustomWidget *customWidget() const { return cusw; }

protected:
    void paintEvent( QPaintEvent *e );

    MetaDataBase::CustomWidget *cusw;
    bool alwaysExpand;

};


class Line : public QFrame
{
    Q_OBJECT

    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_OVERRIDE( int frameWidth DESIGNABLE false )
    Q_OVERRIDE( Shape frameShape DESIGNABLE false )
    Q_OVERRIDE( QRect frameRect DESIGNABLE false )
    Q_OVERRIDE( QRect contentsRect DESIGNABLE false )
public:
    Line( QWidget *parent, const char *name )
	: QFrame( parent, name, WMouseNoMask ) {
	    setFrameStyle( HLine | Sunken );
    }

    void setOrientation( Orientation orient ) {
	if ( orient == Horizontal )
	    setFrameShape( HLine );
	else
	    setFrameShape( VLine );
    }
    Orientation orientation() const {
	return frameShape() == HLine ? Horizontal : Vertical;
    }
};

class QDesignerLabel : public QLabel
{
    Q_OBJECT

    Q_PROPERTY( QCString buddy READ buddyWidget WRITE setBuddyWidget )

public:
    QDesignerLabel( QWidget *parent = 0, const char *name = 0 )
	: QLabel( parent, name ) { myBuddy = 0; }

    void setBuddyWidget( const QCString &b ) {
	myBuddy = b;
	updateBuddy();
    }
    QCString buddyWidget() const {
	return myBuddy;
    };

protected:
    void showEvent( QShowEvent *e ) {
	QLabel::showEvent( e );
	updateBuddy();
    }


private:
    void updateBuddy();

    QCString myBuddy;

};

class QDesignerWidget : public QWidget
{
    Q_OBJECT

public:
    QDesignerWidget( FormWindow *fw, QWidget *parent, const char *name )
	: QWidget( parent, name ), formwindow( fw ) {}

protected:
    void paintEvent( QPaintEvent *e );

private:
    FormWindow *formwindow;

};

class QDesignerDialog : public QDialog
{
    Q_OBJECT

public:
    QDesignerDialog( FormWindow *fw, QWidget *parent, const char *name )
	: QDialog( parent, name ), formwindow( fw ) {}

protected:
    void paintEvent( QPaintEvent *e );

private:
    FormWindow *formwindow;

};

class QDesignerToolButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )
    
public:
    QDesignerToolButton( QWidget *parent, const char *name )
	: QToolButton( parent, name ) {}
    
    bool isInButtonGroup() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ); 
    }
    int buttonGroupId() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ? ( (QButtonGroup*)parentWidget() )->id( (QButton*)this ) : -1; 
    }
    void setButtonGroupId( int id ) { 
	if ( parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ) {
	    ( (QButtonGroup*)parentWidget() )->remove( this );
	    ( (QButtonGroup*)parentWidget() )->insert( this, id );    
	}
    }
};

class QDesignerRadioButton : public QRadioButton
{
    Q_OBJECT
    Q_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )
    
public:
    QDesignerRadioButton( QWidget *parent, const char *name )
	: QRadioButton( parent, name ) {}
    
    bool isInButtonGroup() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ); 
    }
    int buttonGroupId() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ? ( (QButtonGroup*)parentWidget() )->id( (QButton*)this ) : -1; 
    }
    void setButtonGroupId( int id ) { 
	if ( parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ) {
	    ( (QButtonGroup*)parentWidget() )->remove( this );
	    ( (QButtonGroup*)parentWidget() )->insert( this, id );    
	}
    }
    
};

class QDesignerPushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )
    
public:
    QDesignerPushButton( QWidget *parent, const char *name )
	: QPushButton( parent, name ) {}
    
    bool isInButtonGroup() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ); 
    }
    int buttonGroupId() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ? ( (QButtonGroup*)parentWidget() )->id( (QButton*)this ) : -1; 
    }
    void setButtonGroupId( int id ) { 
	if ( parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ) {
	    ( (QButtonGroup*)parentWidget() )->remove( this );
	    ( (QButtonGroup*)parentWidget() )->insert( this, id );    
	}
    }
    
};

class QDesignerCheckBox : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY( int buttonGroupId READ buttonGroupId WRITE setButtonGroupId )
    
public:
    QDesignerCheckBox( QWidget *parent, const char *name )
	: QCheckBox( parent, name ) {}
    
    bool isInButtonGroup() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ); 
    }
    int buttonGroupId() const { 
	return parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ? ( (QButtonGroup*)parentWidget() )->id( (QButton*)this ) : -1; 
    }
    void setButtonGroupId( int id ) { 
	if ( parentWidget() && parentWidget()->inherits( "QButtonGroup" ) ) {
	    ( (QButtonGroup*)parentWidget() )->remove( this );
	    ( (QButtonGroup*)parentWidget() )->insert( this, id );    
	}
    }
    
};

#endif
