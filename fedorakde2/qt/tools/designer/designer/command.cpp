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

#include "command.h"
#include "formwindow.h"
#include "widgetfactory.h"
#include "propertyeditor.h"
#include "metadatabase.h"
#include <widgetdatabase.h>
#include "mainwindow.h"
#include "hierarchyview.h"
#include "formlist.h"

#include <qwidget.h>
#include <qmetaobject.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <qiconview.h>
#include <qmultilineedit.h>
#include <qstack.h>
#include <qheader.h>

CommandHistory::CommandHistory( int s )
    : current( -1 ), steps( s ), savedAt( -1 )
{
    history.setAutoDelete( TRUE );
    modified = FALSE;
    compressedCommand = 0;
}

void CommandHistory::addCommand( Command *cmd, bool tryCompress )
{
    if ( tryCompress ) {
	if ( !compressedCommand ||
	     compressedCommand->type() != cmd->type() ||
	     !compressedCommand->canMerge( cmd ) ) {
	    checkCompressedCommand();
	    compressedCommand = 0;
	}
	
	if ( compressedCommand ) {
	    compressedCommand->merge( cmd );
	    modified = TRUE;
	    modificationChanged( modified );
	    return;
	}
	compressedCommand = cmd;
    } else {
	checkCompressedCommand();
    }

    if ( current < (int)history.count() - 1 ) {
	if ( current < savedAt )
	    savedAt = -2;
	
	QList<Command> commands;
	commands.setAutoDelete( FALSE );

	for( int i = 0; i <= current; ++i ) {
	    commands.insert( i, history.at( 0 ) );
	    history.take( 0 );
	}

	commands.append( cmd );
	history.clear();
	history = commands;
	history.setAutoDelete( TRUE );
    } else {
	history.append( cmd );
    }

    if ( (int)history.count() > steps ) {
	savedAt--;
	history.removeFirst();
    } else {
	++current;
    }

    emitUndoRedo();
    modified = TRUE;
    modificationChanged( modified );
}

void CommandHistory::undo()
{
    checkCompressedCommand();
    compressedCommand = 0;
    if ( current > -1 ) {
	history.at( current )->unexecute();
	--current;
    }
    emitUndoRedo();
    modified = savedAt != current;
    modificationChanged( modified );
}

void CommandHistory::redo()
{
    checkCompressedCommand();
    compressedCommand = 0;
    if ( current > -1 ) {
	if ( current < (int)history.count() - 1 ) {
	    ++current;
	    history.at( current )->execute();
	}
    } else {
	if ( history.count() > 0 ) {
	    ++current;
	    history.at( current )->execute();
	}
    }
    emitUndoRedo();
    modified = savedAt != current;
    modificationChanged( modified );
}

void CommandHistory::emitUndoRedo()
{
    Command *undoCmd = 0;
    Command *redoCmd = 0;

    if ( current >= 0 && current < (int)history.count() )
	undoCmd = history.at( current );
    if ( current + 1 >= 0 && current + 1 < (int)history.count() )
	redoCmd = history.at( current + 1 );

    bool ua = (bool)undoCmd;
    QString uc;
    if ( ua )
	uc = undoCmd->name();
    bool ra = (bool)redoCmd;
    QString rc;
    if ( ra )
	rc = redoCmd->name();
    emit undoRedoChanged( ua, ra, uc, rc );
}

void CommandHistory::setModified( bool m )
{
    modified = m;
    if ( !modified )
	savedAt = current;
    modificationChanged( modified );
}

bool CommandHistory::isModified() const
{
    return modified;
}

void CommandHistory::checkCompressedCommand()
{
    if ( compressedCommand && compressedCommand->type() == Command::SetProperty ) {
	Command *c = compressedCommand;
	compressedCommand = 0;
	if ( !( (SetPropertyCommand*)c )->checkProperty() ) {
	    history.remove( current );
	    --current;
	    emitUndoRedo();
	}
    }
}

// ------------------------------------------------------------

Command::Command( const QString &n, FormWindow *fw )
    : cmdName( n ), formWin( fw )
{
}

Command::~Command()
{
}

QString Command::name() const
{
    return cmdName;
}

FormWindow *Command::formWindow() const
{
    return formWin;
}

void Command::merge( Command * )
{
}

bool Command::canMerge( Command * )
{
    return FALSE;
}

// ------------------------------------------------------------

ResizeCommand::ResizeCommand( const QString &n, FormWindow *fw,
			      QWidget *w, const QRect &oldr, const QRect &nr )
    : Command( n, fw ), widget( w ), oldRect( oldr ), newRect( nr )
{
}

void ResizeCommand::execute()
{
    widget->setGeometry( newRect );
    formWindow()->updateSelection( widget );
    formWindow()->emitUpdateProperties( widget );
    if ( WidgetFactory::layoutType( widget ) != WidgetFactory::NoLayout )
	formWindow()->updateChildSelections( widget );
}

void ResizeCommand::unexecute()
{
    widget->setGeometry( oldRect );
    formWindow()->updateSelection( widget );
    formWindow()->emitUpdateProperties( widget );
    if ( WidgetFactory::layoutType( widget ) != WidgetFactory::NoLayout )
	formWindow()->updateChildSelections( widget );
}

// ------------------------------------------------------------

InsertCommand::InsertCommand( const QString &n, FormWindow *fw,
			      QWidget *w, const QRect &g )
    : Command( n, fw ), widget( w ), geometry( g )
{
}

void InsertCommand::execute()
{
    if ( geometry.size() == QSize( 0, 0 ) ) {
	widget->move( geometry.topLeft() );
	widget->adjustSize();
    } else {
	QSize s = geometry.size().expandedTo( widget->minimumSize() );
	s = s.expandedTo( widget->minimumSizeHint() );
	QRect r( geometry.topLeft(), s );
	widget->setGeometry( r );
    }
    widget->show();
    formWindow()->widgets()->insert( widget, widget );
    formWindow()->clearSelection( FALSE );
    formWindow()->selectWidget( widget );
    formWindow()->mainWindow()->objectHierarchy()->widgetInserted( widget );
}

void InsertCommand::unexecute()
{
    widget->hide();
    formWindow()->selectWidget( widget, FALSE );
    formWindow()->widgets()->remove( widget );
    formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( widget );
 }

// ------------------------------------------------------------

MoveCommand::MoveCommand( const QString &n, FormWindow *fw,
			  const QWidgetList &w,
			  const QValueList<QPoint> op,
			  const QValueList<QPoint> np,
			  QWidget *opr, QWidget *npr )
    : Command( n, fw ), widgets( w ), oldPos( op ), newPos( np ),
      oldParent( opr ), newParent( npr )
{
    widgets.setAutoDelete( FALSE );
}

void MoveCommand::merge( Command *c )
{
    MoveCommand *cmd = (MoveCommand*)c;
    newPos = cmd->newPos;
}

bool MoveCommand::canMerge( Command *c )
{
    MoveCommand *cmd = (MoveCommand*)c;
    return widgets == cmd->widgets;
}


void MoveCommand::execute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout ) {
	    if ( newParent && oldParent && newParent != oldParent ) {
		QPoint pos = newParent->mapFromGlobal( w->mapToGlobal( QPoint( 0,0 ) ) );
		w->reparent( newParent, pos, TRUE );
		formWindow()->raiseSelection( w );
		formWindow()->raiseChildSelections( w );
		formWindow()->widgetChanged( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
	    }
	    w->move( newPos[ widgets.at() ] );
	}
	formWindow()->updateSelection( w );
	formWindow()->updateChildSelections( w );
	formWindow()->emitUpdateProperties( w );
    }
}

void MoveCommand::unexecute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout ) {
	    if ( newParent && oldParent && newParent != oldParent ) {
		QPoint pos = oldParent->mapFromGlobal( w->mapToGlobal( QPoint( 0,0 ) ) );
		w->reparent( oldParent, pos, TRUE );
		formWindow()->raiseSelection( w );
		formWindow()->raiseChildSelections( w );
		formWindow()->widgetChanged( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
		formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
	    }
	    w->move( oldPos[ widgets.at() ] );
	}
	formWindow()->updateSelection( w );
	formWindow()->updateChildSelections( w );
	formWindow()->emitUpdateProperties( w );
    }
}

// ------------------------------------------------------------

DeleteCommand::DeleteCommand( const QString &n, FormWindow *fw,
			      const QWidgetList &w )
    : Command( n, fw ), widgets( w )
{
    widgets.setAutoDelete( FALSE );
}

void DeleteCommand::execute()
{
    formWindow()->setPropertyShowingBlocked( TRUE );
    connections.clear();
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->hide();
	QString s = w->name();
	s.prepend( "qt_dead_widget_" );
	w->setName( s );
	formWindow()->selectWidget( w, FALSE );
	formWindow()->widgets()->remove( w );
	QValueList<MetaDataBase::Connection> conns = MetaDataBase::connections( formWindow(), w );
	connections.insert( w, conns );
	QValueList<MetaDataBase::Connection>::Iterator it = conns.begin();
	for ( ; it != conns.end(); ++it ) {
	    MetaDataBase::removeConnection( formWindow(), (*it).sender,
					    (*it).signal, (*it).receiver, (*it).slot );
	}
    }
    formWindow()->setPropertyShowingBlocked( FALSE );
    formWindow()->emitShowProperties();
    formWindow()->mainWindow()->objectHierarchy()->widgetsRemoved( widgets );

}

void DeleteCommand::unexecute()
{
    formWindow()->setPropertyShowingBlocked( TRUE );
    formWindow()->clearSelection( FALSE );
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->show();
	QString s = w->name();
	s.remove( 0, QString( "qt_dead_widget_" ).length() );
	w->setName( s );
	formWindow()->widgets()->insert( w, w );
	formWindow()->selectWidget( w );
	QValueList<MetaDataBase::Connection> conns = *connections.find( w );
	QValueList<MetaDataBase::Connection>::Iterator it = conns.begin();
	for ( ; it != conns.end(); ++it ) {
	    MetaDataBase::addConnection( formWindow(), (*it).sender,
					 (*it).signal, (*it).receiver, (*it).slot );
	}
    }
    formWindow()->setPropertyShowingBlocked( FALSE );
    formWindow()->emitShowProperties();
    formWindow()->mainWindow()->objectHierarchy()->widgetsInserted( widgets );
}

// ------------------------------------------------------------

SetPropertyCommand::SetPropertyCommand( const QString &n, FormWindow *fw,
					QWidget *w, PropertyEditor *e,
					const QString &pn, const QVariant &ov,
					const QVariant &nv, const QString &ncut,
					const QString &ocut, bool reset )
    : Command( n, fw ), widget( w ), editor( e ), propName( pn ),
      oldValue( ov ), newValue( nv ), oldCurrentItemText( ocut ), newCurrentItemText( ncut ),
      wasChanged( TRUE ), isResetCommand( reset )
{
    wasChanged = MetaDataBase::isPropertyChanged( w, propName );
}


void SetPropertyCommand::execute()
{
    if ( !wasChanged )
	MetaDataBase::setPropertyChanged( widget, propName, TRUE );
    if ( isResetCommand ) {
	MetaDataBase::setPropertyChanged( widget, propName, FALSE );
	if ( WidgetFactory::resetProperty( widget, propName ) ) {
	    if ( !formWindow()->isWidgetSelected( widget ) && widget != formWindow() )
		formWindow()->selectWidget( widget );
	    if ( editor->widget() != widget )
		editor->setWidget( widget, formWindow() );
	    editor->propertyList()->setCurrentProperty( propName );
	    PropertyItem *i = (PropertyItem*)editor->propertyList()->currentItem();
	    if ( !i )
		return;
	    i->setValue( widget->property( propName ) );
	    i->setChanged( FALSE );
	    editor->refetchData();
	    editor->emitWidgetChanged();
	    return;
	}
    }
    setProperty( newValue, newCurrentItemText );
}

void SetPropertyCommand::unexecute()
{
    if ( !wasChanged )
	MetaDataBase::setPropertyChanged( widget, propName, FALSE );
    if ( isResetCommand )
	MetaDataBase::setPropertyChanged( widget, propName, TRUE );
    setProperty( oldValue, oldCurrentItemText );
}

bool SetPropertyCommand::canMerge( Command *c )
{
    SetPropertyCommand *cmd = (SetPropertyCommand*)c;
    const QMetaProperty *p = widget->metaObject()->property( propName, TRUE );
    if ( !p ) {
	if ( propName == "toolTip" || propName == "whatsThis" )
	    return TRUE;
	if ( widget->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)widget )->customWidget();
	    if ( !cw )
		return FALSE;
	    for ( QValueList<MetaDataBase::Property>::Iterator it = cw->lstProperties.begin(); it != cw->lstProperties.end(); ++it ) {
		if ( QString( (*it ).property ) == propName ) {
		    if ( (*it).type == "String" || (*it).type == "CString" || (*it).type == "Int" || (*it).type == "UInt" )
			return TRUE;
		}
	    }
	}
	return FALSE;
    }
    QVariant::Type t = QVariant::nameToType( p->type() );
    return ( cmd->propName == propName &&
	     t == QVariant::String || t == QVariant::CString || t == QVariant::Int || t == QVariant::UInt );
}

void SetPropertyCommand::merge( Command *c )
{
    SetPropertyCommand *cmd = (SetPropertyCommand*)c;
    newValue = cmd->newValue;
    newCurrentItemText = cmd->newCurrentItemText;
}

bool SetPropertyCommand::checkProperty()
{
    if ( propName == "name" ) {
	QString s = newValue.toString();
	if ( !formWindow()->unify( widget, s, FALSE ) ) {
	    QMessageBox::information( formWindow()->mainWindow(),
				      FormWindow::tr( "Set 'name' property" ),
				      FormWindow::tr( "The name of a widget has to be unique!\n"
						      "'%1' is already used in the form '%2',\n"
						      "so the name has been changed back to '%3'." ).
				      arg( newValue.toString() ).
				      arg( formWindow()->name() ).
				      arg( oldValue.toString() ));
	    setProperty( oldValue, oldCurrentItemText, FALSE );
	    return FALSE;
	}
    }
    return TRUE;
}

void SetPropertyCommand::setProperty( const QVariant &v, const QString &currentItemText, bool select )
{
    if ( !formWindow()->isWidgetSelected( widget ) && !formWindow()->isMainContainer( widget ) && select )
	formWindow()->selectWidget( widget );
    if ( editor->widget() != widget && select )
	editor->setWidget( widget, formWindow() );
    if ( select )
	editor->propertyList()->setCurrentProperty( propName );

    const QMetaProperty *p = widget->metaObject()->property( propName, TRUE );
    if ( !p ) {
	if ( propName == "hAlign" ) {
	    p = widget->metaObject()->property( "alignment", TRUE );
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~( AlignLeft  | AlignHCenter | AlignRight );
	    align |= p->keyToValue( currentItemText );
	    widget->setProperty( "alignment", QVariant( align ) );
	} else if ( propName == "vAlign" ) {
	    p = widget->metaObject()->property( "alignment", TRUE );
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~( AlignTop | AlignVCenter | AlignBottom );
	    align |= p->keyToValue( currentItemText );
	    widget->setProperty( "alignment", QVariant( align ) );
	} else if ( propName == "wordwrap" ) {
	    int align = widget->property( "alignment" ).toInt();
	    align &= ~WordBreak;
	    if ( v.toBool() )
		align |= WordBreak;
	    widget->setProperty( "alignment", QVariant( align ) );
	} else if ( propName == "layoutSpacing" ) {
	    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( editor->widget() ), v.toInt() );
	} else if ( propName == "layoutMargin" ) {
	    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( editor->widget() ), v.toInt() );
	} else if ( propName == "toolTip" || propName == "whatsThis" ) {
	    MetaDataBase::setFakeProperty( editor->widget(), propName, v );
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)widget )->customWidget();
	    if ( cw ) {
		MetaDataBase::setFakeProperty( editor->widget(), propName, v );
	    }
	}
	editor->refetchData();
	editor->emitWidgetChanged();
	( ( PropertyItem* )editor->propertyList()->currentItem() )->setChanged( MetaDataBase::isPropertyChanged( widget, propName ) );
	return;
    }

    if ( p->isSetType() ) {
	;
    } else if ( p->isEnumType() ) {
	widget->setProperty( propName, p->keyToValue( currentItemText ) );
    } else {
	QVariant ov;
	if ( propName == "name" )
	    ov = widget->property( propName );
	widget->setProperty( propName, v );
	if ( propName == "cursor" )
	    MetaDataBase::setCursor( widget, v.toCursor() );
	if ( propName == "name" ) {
	    formWindow()->mainWindow()->objectHierarchy()->namePropertyChanged( widget, ov );
	    if ( formWindow()->isMainContainer( widget ) ) {
		formWindow()->mainWindow()->formlist()->nameChanged( (FormWindow*)widget );
		formWindow()->setName( v.toCString() );
	    }
	}
	if ( propName == "caption" ) {
	    if ( formWindow()->isMainContainer( widget ) )
		formWindow()->setCaption( v.toString() );
	}
	if ( propName == "icon" ) {
	    if ( formWindow()->isMainContainer( widget ) )
		formWindow()->setIcon( v.toPixmap() );
	}
    }
    editor->refetchData();
    if ( editor->propertyList()->currentItem() && select ) {
	( ( PropertyItem* )editor->propertyList()->currentItem() )->showEditor();
	( ( PropertyItem* )editor->propertyList()->currentItem() )->setChanged( MetaDataBase::isPropertyChanged( widget, propName ) );
    }
    editor->emitWidgetChanged();
}

// ------------------------------------------------------------

LayoutHorizontalCommand::LayoutHorizontalCommand( const QString &n, FormWindow *fw,
						  QWidget *parent, QWidget *layoutBase,
						  const QWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase )
{
}

void LayoutHorizontalCommand::execute()
{
    formWindow()->clearSelection( FALSE );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutHorizontalCommand::unexecute()
{
    formWindow()->clearSelection( FALSE );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutVerticalCommand::LayoutVerticalCommand( const QString &n, FormWindow *fw,
					      QWidget *parent, QWidget *layoutBase,
					      const QWidgetList &wl )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase )
{
}

void LayoutVerticalCommand::execute()
{
    formWindow()->clearSelection( FALSE );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutVerticalCommand::unexecute()
{
    formWindow()->clearSelection( FALSE );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

LayoutGridCommand::LayoutGridCommand( const QString &n, FormWindow *fw,
				      QWidget *parent, QWidget *layoutBase,
				      const QWidgetList &wl, int xres, int yres )
    : Command( n, fw ), layout( wl, parent, fw, layoutBase, QSize( xres, yres ) )
{
}

void LayoutGridCommand::execute()
{
    formWindow()->clearSelection( FALSE );
    layout.doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

void LayoutGridCommand::unexecute()
{
    formWindow()->clearSelection( FALSE );
    layout.undoLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
}

// ------------------------------------------------------------

BreakLayoutCommand::BreakLayoutCommand( const QString &n, FormWindow *fw,
					QWidget *layoutBase, const QWidgetList &wl )
    : Command( n, fw ), lb( layoutBase ), widgets( wl )
{
    WidgetFactory::LayoutType lay = WidgetFactory::layoutType( layoutBase );
    spacing = MetaDataBase::spacing( layoutBase );
    margin = MetaDataBase::margin( layoutBase );
    layout = 0;
    if ( lay == WidgetFactory::HBox )
	layout = new HorizontalLayout( wl, layoutBase, fw, layoutBase, FALSE );
    else if ( lay == WidgetFactory::VBox )
	layout = new VerticalLayout( wl, layoutBase, fw, layoutBase, FALSE );
    else if ( lay == WidgetFactory::Grid )
	layout = new GridLayout( wl, layoutBase, fw, layoutBase, QSize( fw->grid().x(), fw->grid().y()), FALSE );
}

void BreakLayoutCommand::execute()
{
    if ( !layout )
	return;
    formWindow()->clearSelection( FALSE );
    layout->breakLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
    for ( QWidget *w = widgets.first(); w; w = widgets.next() )
	w->resize( QMAX( 16, w->width() ), QMAX( 16, w->height() ) );
}

void BreakLayoutCommand::unexecute()
{
    if ( !layout )
	return;
    formWindow()->clearSelection( FALSE );
    layout->doLayout();
    formWindow()->mainWindow()->objectHierarchy()->rebuild();
    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( lb ), spacing );
    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( lb ), margin );
}

// ------------------------------------------------------------

MacroCommand::MacroCommand( const QString &n, FormWindow *fw,
			    const QList<Command> &cmds )
    : Command( n, fw ), commands( cmds )
{
}

void MacroCommand::execute()
{
    for ( Command *c = commands.first(); c; c = commands.next() )
	c->execute();
}

void MacroCommand::unexecute()
{
    for ( Command *c = commands.last(); c; c = commands.prev() )
	c->unexecute();
}

// ------------------------------------------------------------

AddTabPageCommand::AddTabPageCommand( const QString &n, FormWindow *fw,
				      QTabWidget *tw, const QString &label )
    : Command( n, fw ), tabWidget( tw ), tabLabel( label )
{
    tabPage = new QDesignerWidget( formWindow(), tabWidget, "tab" );
    tabPage->hide();
    index = -1;
    MetaDataBase::addEntry( tabPage );
}

void AddTabPageCommand::execute()
{
    if ( index == -1 )
	index = ( (QDesignerTabWidget*)tabWidget )->count();
    tabWidget->insertTab( tabPage, tabLabel, index );
    tabWidget->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

void AddTabPageCommand::unexecute()
{
    tabWidget->removePage( tabPage );
    tabPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

// ------------------------------------------------------------

DeleteTabPageCommand::DeleteTabPageCommand( const QString &n, FormWindow *fw,
					    QTabWidget *tw, QWidget *page )
    : Command( n, fw ), tabWidget( tw ), tabPage( page )
{
    tabLabel = ( (QDesignerTabWidget*)tabWidget )->pageTitle();
    index = ( (QDesignerTabWidget*)tabWidget )->currentPage();
}

void DeleteTabPageCommand::execute()
{
    tabWidget->removePage( tabPage );
    tabPage->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

void DeleteTabPageCommand::unexecute()
{
    tabWidget->insertTab( tabPage, tabLabel, index );
    tabWidget->showPage( tabPage );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->tabsChanged( tabWidget );
}

// ------------------------------------------------------------

AddWizardPageCommand::AddWizardPageCommand( const QString &n, FormWindow *fw,
					    QWizard *w, const QString &label )
    : Command( n, fw ), wizard( w ), pageLabel( label )
{
    page = new QDesignerWidget( formWindow(), wizard, "page" );
    page->hide();
    index = -1;
    MetaDataBase::addEntry( page );
}

void AddWizardPageCommand::execute()
{
    if ( index == -1 )
	index = wizard->pageCount();
    ( (QDesignerWizard*)wizard )->insertPage( page, pageLabel, index );
    ( (QDesignerWizard*)wizard )->setCurrentPage( ( (QDesignerWizard*)wizard )->pageNum( page ) );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void AddWizardPageCommand::unexecute()
{
    wizard->removePage( page );
    page->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

// ------------------------------------------------------------

DeleteWizardPageCommand::DeleteWizardPageCommand( const QString &n, FormWindow *fw,
						  QWizard *w, QWidget *p )
    : Command( n, fw ), wizard( w ), page( p )
{
    pageLabel = ( (QDesignerWizard*)wizard )->pageTitle();
    index = ( (QDesignerWizard*)wizard )->currentPageNum();
}

void DeleteWizardPageCommand::execute()
{
    wizard->removePage( page );
    page->hide();
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

void DeleteWizardPageCommand::unexecute()
{
    ( (QDesignerWizard*)wizard )->insertPage( page, pageLabel, index );
    ( (QDesignerWizard*)wizard )->setCurrentPage( ( (QDesignerWizard*)wizard )->pageNum( page ) );
    formWindow()->emitUpdateProperties( formWindow()->currentWidget() );
    formWindow()->mainWindow()->objectHierarchy()->pagesChanged( wizard );
}

// ------------------------------------------------------------

AddConnectionCommand::AddConnectionCommand( const QString &name, FormWindow *fw,
					    MetaDataBase::Connection c )
    : Command( name, fw ), connection( c )
{
}

void AddConnectionCommand::execute()
{
    MetaDataBase::addConnection( formWindow(), connection.sender, connection.signal, connection.receiver, connection.slot );
}

void AddConnectionCommand::unexecute()
{
    MetaDataBase::removeConnection( formWindow(), connection.sender, connection.signal, connection.receiver, connection.slot );
}

// ------------------------------------------------------------

RemoveConnectionCommand::RemoveConnectionCommand( const QString &name, FormWindow *fw,
						  MetaDataBase::Connection c )
    : Command( name, fw ), connection( c )
{
}

void RemoveConnectionCommand::execute()
{
    MetaDataBase::removeConnection( formWindow(), connection.sender, connection.signal, connection.receiver, connection.slot );
}

void RemoveConnectionCommand::unexecute()
{
    MetaDataBase::addConnection( formWindow(), connection.sender, connection.signal, connection.receiver, connection.slot );
}

// ------------------------------------------------------------

AddSlotCommand::AddSlotCommand( const QString &name, FormWindow *fw, const QCString &s, const QString &a )
    : Command( name, fw ), slot( s ), access( a )
{
}

void AddSlotCommand::execute()
{
    MetaDataBase::addSlot( formWindow(), slot, access );
}

void AddSlotCommand::unexecute()
{
    MetaDataBase::removeSlot( formWindow(), slot, access );
}

// ------------------------------------------------------------

RemoveSlotCommand::RemoveSlotCommand( const QString &name, FormWindow *fw, const QCString &s, const QString &a )
    : Command( name, fw ), slot( s ), access( a )
{
}

void RemoveSlotCommand::execute()
{
    MetaDataBase::removeSlot( formWindow(), slot, access );
}

void RemoveSlotCommand::unexecute()
{
    MetaDataBase::addSlot( formWindow(), slot, access );
}

// ------------------------------------------------------------

LowerCommand::LowerCommand( const QString &name, FormWindow *fw, const QWidgetList &w )
    : Command( name, fw ), widgets( w )
{
}

void LowerCommand::execute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->lower();
	formWindow()->raiseSelection( w );
    }
	
}

void LowerCommand::unexecute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->raise();
	formWindow()->raiseSelection( w );
    }
}

// ------------------------------------------------------------

RaiseCommand::RaiseCommand( const QString &name, FormWindow *fw, const QWidgetList &w )
    : Command( name, fw ), widgets( w )
{
}

void RaiseCommand::execute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->raise();
	formWindow()->raiseSelection( w );
    }
	
}

void RaiseCommand::unexecute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->lower();
	formWindow()->raiseSelection( w );
    }
}

// ------------------------------------------------------------

PasteCommand::PasteCommand( const QString &n, FormWindow *fw,
			      const QWidgetList &w )
    : Command( n, fw ), widgets( w )
{
}

void PasteCommand::execute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->show();
	formWindow()->selectWidget( w );
	formWindow()->widgets()->insert( w, w );
	formWindow()->mainWindow()->objectHierarchy()->widgetInserted( w );
    }
}

void PasteCommand::unexecute()
{
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	w->hide();
	formWindow()->selectWidget( w, FALSE );
	formWindow()->widgets()->remove( w );
	formWindow()->mainWindow()->objectHierarchy()->widgetRemoved( w );
    }
 }

// ------------------------------------------------------------

TabOrderCommand::TabOrderCommand( const QString &n, FormWindow *fw,
				  const QWidgetList &ol, const QWidgetList &nl )
    : Command( n, fw ), oldOrder( ol ), newOrder( nl )
{
}

void TabOrderCommand::merge( Command *c )
{
    TabOrderCommand *cmd = (TabOrderCommand*)c;
    newOrder = cmd->newOrder;
}

bool TabOrderCommand::canMerge( Command * )
{
    return TRUE;
}

void TabOrderCommand::execute()
{
    MetaDataBase::setTabOrder( formWindow(), newOrder );
}

void TabOrderCommand::unexecute()
{
    MetaDataBase::setTabOrder( formWindow(), oldOrder );
}

// ------------------------------------------------------------

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
bool PopulateListBoxCommand::Item::operator==( const PopulateListBoxCommand::Item& ) const
{
    return FALSE;
}
#endif

PopulateListBoxCommand::PopulateListBoxCommand( const QString &n, FormWindow *fw,
						QListBox *lb, const QValueList<Item> &items )
    : Command( n, fw ), newItems( items ), listbox( lb )
{
    QListBoxItem *i = 0;
    for ( i = listbox->firstItem(); i; i = i->next() ) {
	Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	oldItems.append( item );
    }
}

void PopulateListBoxCommand::execute()
{
    listbox->clear();
    for ( QValueList<Item>::Iterator it = newItems.begin(); it != newItems.end(); ++it ) {
	Item i = *it;
	if ( !i.pix.isNull() )
	    (void)new QListBoxPixmap( listbox, i.pix, i.text );
	else
	    (void)new QListBoxText( listbox, i.text );
    }
}

void PopulateListBoxCommand::unexecute()
{
    listbox->clear();
    for ( QValueList<Item>::Iterator it = oldItems.begin(); it != oldItems.end(); ++it ) {
	Item i = *it;
	if ( !i.pix.isNull() )
	    (void)new QListBoxPixmap( listbox, i.pix, i.text );
	else
	    (void)new QListBoxText( listbox, i.text );
    }
}

// ------------------------------------------------------------

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
bool PopulateIconViewCommand::Item::operator==( const PopulateIconViewCommand::Item& ) const
{
    return FALSE;
}
#endif

PopulateIconViewCommand::PopulateIconViewCommand( const QString &n, FormWindow *fw,
						  QIconView *iv, const QValueList<Item> &items )
    : Command( n, fw ), newItems( items ), iconview( iv )
{
    QIconViewItem *i = 0;
    for ( i = iconview->firstItem(); i; i = i->nextItem() ) {
	Item item;
	if ( i->pixmap() )
	    item.pix = *i->pixmap();
	item.text = i->text();
	oldItems.append( item );
    }
}

void PopulateIconViewCommand::execute()
{
    iconview->clear();
    for ( QValueList<Item>::Iterator it = newItems.begin(); it != newItems.end(); ++it ) {
	Item i = *it;
	(void)new QIconViewItem( iconview, i.text, i.pix );
    }
}

void PopulateIconViewCommand::unexecute()
{
    iconview->clear();
    for ( QValueList<Item>::Iterator it = oldItems.begin(); it != oldItems.end(); ++it ) {
	Item i = *it;
	(void)new QIconViewItem( iconview, i.text, i.pix );
    }
}

// ------------------------------------------------------------

PopulateListViewCommand::PopulateListViewCommand( const QString &n, FormWindow *fw,
						  QListView *lv, QListView *from )
    : Command( n, fw ), listview( lv )
{
    newItems = new QListView();
    newItems->hide();
    transferItems( from, newItems );
    oldItems = new QListView();
    oldItems->hide();
    transferItems( listview, oldItems );
}

void PopulateListViewCommand::execute()
{
    listview->clear();
    transferItems( newItems, listview );
}

void PopulateListViewCommand::unexecute()
{
    listview->clear();
    transferItems( oldItems, listview );
}

void PopulateListViewCommand::transferItems( QListView *from, QListView *to )
{
    QHeader *header = to->header();
    while ( header->count() )
	to->removeColumn( 0 );
    QHeader *h2 = from->header();
    for ( int i = 0; i < h2->count(); ++i ) {
	to->addColumn( h2->label( i ) );
	if ( h2->iconSet( i ) && !h2->iconSet( i )->pixmap().isNull() )
	    header->setLabel( i, *h2->iconSet( i ), h2->label( i ) );
	header->setResizeEnabled( h2->isResizeEnabled( i ), i );
	header->setClickEnabled( h2->isClickEnabled( i ), i );
    }

    QListViewItemIterator it( from );
    QStack<QListViewItem> fromParents, toParents;
    fromParents.push( 0 );
    toParents.push( 0 );
    QStack<QListViewItem> toLasts;
    QListViewItem *fromLast = 0;
    toLasts.push( 0 );
    int cols = from->columns();
    to->setSorting( -1 );
    from->setSorting( -1 );
    for ( ; it.current(); ++it ) {
	QListViewItem *i = it.current();
	if ( i->parent() == fromParents.top() ) {
	    QListViewItem *pi = toParents.top();
	    QListViewItem *ni = 0;
	    if ( pi )
		ni = new QListViewItem( pi, toLasts.top() );
	    else
		ni = new QListViewItem( to, toLasts.top() );
	    for ( int c = 0; c < cols; ++c ) {
		ni->setText( c, i->text( c ) );
		if ( i->pixmap( c ) )
		    ni->setPixmap( c, *i->pixmap( c ) );
	    }
	    toLasts.pop();
	    toLasts.push( ni );
	    if ( pi )
		pi->setOpen( TRUE );
	} else {
	    if ( i->parent() == fromLast ) {
		fromParents.push( fromLast );
		toParents.push( toLasts.top() );
		toLasts.push( 0 );
		QListViewItem *pi = toParents.top();
		QListViewItem *ni = 0;
		if ( pi )
		    ni = new QListViewItem( pi );
		else
		    ni = new QListViewItem( to );
		for ( int c = 0; c < cols; ++c ) {
		    ni->setText( c, i->text( c ) );
		    if ( i->pixmap( c ) )
			ni->setPixmap( c, *i->pixmap( c ) );
		}
		toLasts.pop();
		toLasts.push( ni );
		if ( pi )
		    pi->setOpen( TRUE );
	    } else {
		while ( fromParents.top() != i->parent() ) {
		    fromParents.pop();
		    toParents.pop();
		    toLasts.pop();
		}
		
		QListViewItem *pi = toParents.top();
		QListViewItem *ni = 0;
		if ( pi )
		    ni = new QListViewItem( pi, toLasts.top() );
		else
		    ni = new QListViewItem( to, toLasts.top() );
		for ( int c = 0; c < cols; ++c ) {
		    ni->setText( c, i->text( c ) );
		    if ( i->pixmap( c ) )
			ni->setPixmap( c, *i->pixmap( c ) );
		}
		if ( pi )
		    pi->setOpen( TRUE );
		toLasts.pop();
		toLasts.push( ni );
	    }
	}
	fromLast = i;
    }
}



// ------------------------------------------------------------

PopulateMultiLineEditCommand::PopulateMultiLineEditCommand( const QString &n, FormWindow *fw,
							    QMultiLineEdit *mle, const QString &txt )
    : Command( n, fw ), newText( txt ), mlined( mle )
{
    oldText = mlined->text();
    wasChanged = MetaDataBase::isPropertyChanged( mlined, "text" );
}

void PopulateMultiLineEditCommand::execute()
{
    mlined->setText( newText );
    MetaDataBase::setPropertyChanged( mlined, "text", TRUE );
    formWindow()->emitUpdateProperties( mlined );
}

void PopulateMultiLineEditCommand::unexecute()
{
    mlined->setText( oldText );
    MetaDataBase::setPropertyChanged( mlined, "text", wasChanged );
    formWindow()->emitUpdateProperties( mlined );
}
