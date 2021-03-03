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

#include <qvariant.h> // HP-UX compiler needs this here

#include "propertyeditor.h"
#include "pixmapchooser.h"
#include "formwindow.h"
#include "command.h"
#include "metadatabase.h"
#include <widgetdatabase.h>
#include "widgetfactory.h"
#include "globaldefs.h"
#include "defs.h"
#include "asciivalidator.h"
#include "paletteeditorimpl.h"
#include "multilineeditorimpl.h"

#include <limits.h>

#include <qpainter.h>
#include <qpalette.h>
#include <qapplication.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qfontdialog.h>
#include <qspinbox.h>
#include <qevent.h>
#include <qobjectlist.h>
#include <qlistbox.h>
#include <qfontdatabase.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaccel.h>
#include <qworkspace.h>

#include "../pics/arrow.xbm"
#include "../pics/uparrow.xbm"
#include "../pics/cross.xbm"
#include "../pics/wait.xbm"
#include "../pics/ibeam.xbm"
#include "../pics/sizeh.xbm"
#include "../pics/sizev.xbm"
#include "../pics/sizeb.xbm"
#include "../pics/sizef.xbm"
#include "../pics/sizeall.xbm"
#include "../pics/vsplit.xbm"
#include "../pics/hsplit.xbm"
#include "../pics/hand.xbm"
#include "../pics/no.xbm"

static QFontDatabase *fontDataBase = 0;

static void cleanupFontDatabase()
{
    delete fontDataBase;
    fontDataBase = 0;
}

static QStringList getFontList()
{
    if ( !fontDataBase ) {
	fontDataBase = new QFontDatabase;
	qAddPostRoutine( cleanupFontDatabase );
    }
    return fontDataBase->families();
}

/*!
  \class PropertyItem propertyeditor.h
  \brief Base class for all property items

  This is the base class for each property item for the
  PropertyList. A simple property item has just a name and a value to
  provide an editor for a datatype. But more complex datatypes might
  provide an expandable item for editing single parts of the
  datatype. See hasSubItems(), initChildren() for that.
*/

/*!  If this item should be a child of another property item, specify
  \a prop as the parent item.
*/

PropertyItem::PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : QListViewItem( l, after ), listview( l ), property( prop ), propertyName( propName )
{
    setSelectable( FALSE );
    open = FALSE;
    setText( 0, propertyName );
    changed = FALSE;
    setText( 1, "" );
    resetButton = 0;
}

PropertyItem::~PropertyItem()
{
    if ( resetButton )	
	delete resetButton->parentWidget();
    resetButton = 0;
}

void PropertyItem::toggle()
{
}

void PropertyItem::updateBackColor()
{
    if ( itemAbove() && this != listview->firstChild() ) {
	if ( ( ( PropertyItem*)itemAbove() )->backColor == backColor1 )
	    backColor = backColor2;
	else
	    backColor = backColor1;
    } else {
	backColor = backColor1;
    }
    if ( listview->firstChild() == this )
	backColor = backColor1;
}

QColor PropertyItem::backgroundColor()
{
    updateBackColor();
    if ( (QListViewItem*)this == listview->currentItem() )
	return selectedBack;
    return backColor;
}

/*!  If a subclass is a expandable item, this is called when the child
items should be created.
*/

void PropertyItem::createChildren()
{
}

/*!  If a subclass is a expandable item, this is called when the child
items should be initialized.
*/

void PropertyItem::initChildren()
{
}

void PropertyItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    int indent = 0;
    if ( column == 0 ) {
	indent = 20 + ( property ? 20 : 0 );
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	p->save();
	p->translate( indent, 0 );
    }

    if ( isChanged() && column == 0 ) {
	p->save();
	QFont f = p->font();
	f.setBold( TRUE );
	p->setFont( f );
    }

    if ( !hasCustomContents() || column != 1 ) {
	QListViewItem::paintCell( p, g, column, width - indent, align  );
    } else {
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	drawCustomContents( p, QRect( 0, 0, width, height() ) );
    }

    if ( isChanged() && column == 0 )
	p->restore();
    if ( column == 0 )
	p->restore();
    if ( hasSubItems() && column == 0 ) {
	p->save();
	p->setPen( cg.foreground() );
	p->setBrush( cg.base() );
	p->drawRect( 5, height() / 2 - 4, 9, 9 );
	p->drawLine( 7, height() / 2, 11, height() / 2 );
	if ( !isOpen() )
	    p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
	p->restore();
    }
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();

    if ( listview->currentItem() == this && column == 0 &&
	 !listview->hasFocus() && !listview->viewport()->hasFocus() )
	paintFocus( p, cg, QRect( 0, 0, width, height() ) );
}

void PropertyItem::paintBranches( QPainter * p, const QColorGroup & cg,
				  int w, int y, int h, GUIStyle s )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    QListViewItem::paintBranches( p, g, w, y, h, s );
}	

void PropertyItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
    p->save();
    QApplication::style().drawPanel( p, r.x(), r.y(), r.width(), r.height(), cg, TRUE, 1 );
    p->restore();
}

/*!  Subclasses which are expandable items have to return TRUE
  here. Default is FALSE.
*/

bool PropertyItem::hasSubItems() const
{
    return FALSE;
}

/*!  Returns the parent property item here if this is a child or 0
 otherwise.
 */

PropertyItem *PropertyItem::propertyParent() const
{
    return property;
}

bool PropertyItem::isOpen() const
{
    return open;
}

void PropertyItem::setOpen( bool b )
{
    if ( b == open )
	return;
    open = b;

    if ( !open ) {
	children.setAutoDelete( TRUE );
	children.clear();
	children.setAutoDelete( FALSE );
	qApp->processEvents();
	listview->updateEditorSize();
 	return;
    }

    createChildren();
    initChildren();
    qApp->processEvents();
    listview->updateEditorSize();
}

/*!  Subclasses have to show the editor of the item here
*/

void PropertyItem::showEditor()
{
    createResetButton();
    resetButton->parentWidget()->show();
}

/*!  Subclasses have to hide the editor of the item here
*/

void PropertyItem::hideEditor()
{
    createResetButton();
    resetButton->parentWidget()->hide();
}

/*!  This is called to init the value of the item. Reimplement in
  subclasses to init the editor
*/

void PropertyItem::setValue( const QVariant &v )
{
    val = v;
}

QVariant PropertyItem::value() const
{
    return val;
}

bool PropertyItem::isChanged() const
{
    return changed;
}

void PropertyItem::setChanged( bool b, bool updateDb )
{
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
    if ( updateDb )
	MetaDataBase::setPropertyChanged( listview->propertyEditor()->widget(), name(), changed );
    updateResetButtonState();
}

QString PropertyItem::name() const
{
    return propertyName;
}

void PropertyItem::createResetButton()
{
    if ( resetButton ) {
	resetButton->parentWidget()->lower();
	return;
    }
    QHBox *hbox = new QHBox( listview->viewport() );
    hbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    hbox->setLineWidth( 1 );
    resetButton = new QPushButton( hbox );
    resetButton->setPixmap( PixmapChooser::loadPixmap( "resetproperty.xpm", PixmapChooser::Mini ) );
    resetButton->setFixedWidth( resetButton->sizeHint().width() );
    hbox->layout()->setAlignment( Qt::AlignRight );
    listview->addChild( hbox );
    hbox->hide();
    QObject::connect( resetButton, SIGNAL( clicked() ),
		      listview, SLOT( resetProperty() ) );
    QToolTip::add( resetButton, PropertyEditor::tr( "Reset property to its default value" ) );
    QWhatsThis::add( resetButton, PropertyEditor::tr( "Click this button to reset the property to its default value" ) );
    updateResetButtonState();
}

void PropertyItem::updateResetButtonState()
{
    if ( !resetButton )
	return;
    if ( propertyParent() || !WidgetFactory::canResetProperty( listview->propertyEditor()->widget(), name() ) )
	resetButton->setEnabled( FALSE );
    else
	resetButton->setEnabled( isChanged() );
}

/*!  Call this to place/resize the item editor correctly (normally
  call it from showEditor())
*/

void PropertyItem::placeEditor( QWidget *w )
{
    createResetButton();
    QRect r = listview->itemRect( this );
    if ( r == QRect( 0, 0, -1, -1 ) )
	listview->ensureItemVisible( this );
    r = listview->itemRect( this );
    r.setX( listview->header()->sectionPos( 1 ) );
    r.setWidth( listview->header()->sectionSize( 1 ) - 1 );
    r.setWidth( r.width() - resetButton->width() - 2 );
    r = QRect( listview->viewportToContents( r.topLeft() ), r.size() );
    w->resize( r.size() );
    listview->moveChild( w, r.x(), r.y() );
    resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
    listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
    resetButton->setFixedHeight( r.height() - 3 );
}

/*!  This should be called by subclasses if the use changed the value
  of the property and this value should be applied to the widget property
*/

void PropertyItem::notifyValueChange()
{
    if ( !propertyParent() ) {
	listview->valueChanged( this );
	setChanged( TRUE );
	if ( hasSubItems() )
	    initChildren();
    } else {
	propertyParent()->childValueChanged( this );
	setChanged( TRUE );
    }
}

/*!  If a subclass is a expandable item reimplement this as this is
  always called if a child item changed its value. So update the
  display of the item here then.
*/

void PropertyItem::childValueChanged( PropertyItem * )
{
}

/*!  When adding a child item, call this (normally from addChildren()
*/

void PropertyItem::addChild( PropertyItem *i )
{
    children.append( i );
}

int PropertyItem::childCount() const
{
    return children.count();
}

PropertyItem *PropertyItem::child( int i ) const
{
    // ARRRRRRRRG
    return ( (PropertyItem*)this )->children.at( i );
}

/*!  If the contents of the item is not displayable with a text, but
  you want to draw it yourself (using drawCustomContents()), return
  TRUE here.
*/

bool PropertyItem::hasCustomContents() const
{
    return FALSE;
}

/*!
  \sa hasCustomContents()
*/

void PropertyItem::drawCustomContents( QPainter *, const QRect & )
{
}

QString PropertyItem::currentItem() const
{
    return QString::null;
}

int PropertyItem::currentIntItem() const
{
    return -1;
}

void PropertyItem::setCurrentItem( const QString & )
{
}

void PropertyItem::setCurrentItem( int )
{
}

int PropertyItem::currentIntItemFromObject() const
{
    return -1;
}

QString PropertyItem::currentItemFromObject() const
{
    return QString::null;
}

void PropertyItem::setFocus( QWidget *w )
{
    QWorkspace *ws = 0;
    QWidget *wid = listview->propertyEditor();
    while ( wid ) {
	if ( wid->inherits( "QWorkspace" ) )
	    break;
	wid = wid->parentWidget();
    }
    if ( !wid )
	return;
    ws = (QWorkspace*)wid;
    if ( ws->activeWindow() == listview->propertyEditor() )
	w->setFocus();
}

// --------------------------------------------------------------

PropertyTextItem::PropertyTextItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, bool comment, bool multiLine, bool ascii, bool a )
    : PropertyItem( l, after, prop, propName ), withComment( comment ),
      hasMultiLines( multiLine ), asciiOnly( ascii ), accel( a )
{
    lin = 0;
    box = 0;
}

QLineEdit *PropertyTextItem::lined()
{
    if ( lin )
	return lin;
    if ( hasMultiLines ) {
	box = new QHBox( listview->viewport() );
	box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	box->setLineWidth( 2 );
	box->hide();
    }

    lin = 0;
    if ( hasMultiLines )
	lin = new QLineEdit( box );
    else
	lin = new QLineEdit( listview->viewport() );

    if ( asciiOnly )
	lin->setValidator( new AsciiValidator( lin, "ascii_validator" ) );
    if ( !hasMultiLines ) {
	lin->hide();
    } else {
	button = new QPushButton( tr("..."), box );
	button->setFixedWidth( 20 );
	connect( button, SIGNAL( clicked() ),
		 this, SLOT( getText() ) );
	lin->setFrame( FALSE );
    }
    connect( lin, SIGNAL( returnPressed() ),
	     this, SLOT( setValue() ) );
    connect( lin, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( setValue() ) );
    if ( PropertyItem::name() == "name" )
	connect( lin, SIGNAL( returnPressed() ),
		 listview->propertyEditor()->formWindow()->commandHistory(),
		 SLOT( checkCompressedCommand() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyTextItem::~PropertyTextItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
    delete (QHBox*)box;
    box = 0;
}

void PropertyTextItem::setChanged( bool b, bool updateDb )
{
    PropertyItem::setChanged( b, updateDb );
    if ( withComment && childCount() > 0 )
	( (PropertyTextItem*)PropertyItem::child( 0 ) )->lined()->setEnabled( b );
}

bool PropertyTextItem::hasSubItems() const
{
    return withComment;
}

void PropertyTextItem::childValueChanged( PropertyItem *child )
{
    MetaDataBase::setPropertyComment( listview->propertyEditor()->widget(), PropertyItem::name(), child->value().toString() );
}

static QString to_string( const QVariant &v, bool accel )
{
    if ( !accel )
	return v.toString();
    return QAccel::keyToString( v.toInt() );
}

void PropertyTextItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( TRUE );
	lined()->setText( to_string( value(), accel ) );
	lined()->blockSignals( FALSE );
    }
    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w= lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}

void PropertyTextItem::createChildren()
{
    PropertyTextItem *i = new PropertyTextItem( listview, this, this, tr( "comment" ), FALSE, FALSE );
    i->lined()->setEnabled( isChanged() );
    addChild( i );
}

void PropertyTextItem::initChildren()
{
    if ( !childCount() )
	return;
    PropertyItem *item = PropertyItem::child( 0 );
    if ( item )
	item->setValue( MetaDataBase::propertyComment( listview->propertyEditor()->widget(), PropertyItem::name() ) );
}

void PropertyTextItem::hideEditor()
{
    PropertyItem::hideEditor();
    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w = lined();

    w->hide();
}

void PropertyTextItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( TRUE );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( to_string( v, accel ) );
	lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( FALSE );
    }
    setText( 1, to_string( v, accel ) );
    PropertyItem::setValue( v );
}

void PropertyTextItem::setValue()
{
    setText( 1, lined()->text() );
    QVariant v;
    if ( accel )
	v = QAccel::stringToKey( lined()->text() );
    else
	v = lined()->text();
    PropertyItem::setValue( v );
    notifyValueChange();
}

void PropertyTextItem::getText()
{
    QString txt = TextEditor::getText( listview, value().toString() );
    if ( !txt.isEmpty() ) {
	setText( 1, txt );
	PropertyItem::setValue( txt );
	notifyValueChange();
	lined()->blockSignals( TRUE );
	lined()->setText( txt );
	lined()->blockSignals( FALSE );
    }
}

// --------------------------------------------------------------

PropertyBoolItem::PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *PropertyBoolItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( FALSE, listview->viewport() );
    comb->hide();
    comb->insertItem( tr( "False" ) );
    comb->insertItem( tr( "True" ) );
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyBoolItem::~PropertyBoolItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyBoolItem::toggle()
{
    bool b = value().toBool();
    setValue( QVariant( !b, 0 ) );
    setValue();
}

void PropertyBoolItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( TRUE );
	if ( value().toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( FALSE );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyBoolItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyBoolItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( comb ) {
	combo()->blockSignals( TRUE );
	if ( v.toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( FALSE );
    }
    QString tmp = tr( "True" );
    if ( !v.toBool() )
	tmp = tr( "False" );
    setText( 1, tmp );
    PropertyItem::setValue( v );
}

void PropertyBoolItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    bool b = combo()->currentItem() == 0 ? (bool)FALSE : (bool)TRUE;
    PropertyItem::setValue( QVariant( b, 0 ) );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyIntItem::PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				  const QString &propName, bool s )
    : PropertyItem( l, after, prop, propName ), signedValue( s )
{
    spinBx = 0;
}

QSpinBox *PropertyIntItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    if ( signedValue )
	spinBx = new QSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
    else
	spinBx = new QSpinBox( 0, INT_MAX, 1, listview->viewport() );
    spinBx->hide();
    spinBx->installEventFilter( listview );
    QObjectList *ol = spinBx->queryList( "QLineEdit" );
    if ( ol && ol->first() )
	ol->first()->installEventFilter( listview );
    delete ol;
    connect( spinBx, SIGNAL( valueChanged( int ) ),
	     this, SLOT( setValue() ) );
    return spinBx;
}

PropertyIntItem::~PropertyIntItem()
{
    delete (QSpinBox*)spinBx;
    spinBx = 0;
}

void PropertyIntItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !spinBx ) {
	spinBox()->blockSignals( TRUE );
	if ( signedValue )
	    spinBox()->setValue( value().toInt() );
	else
	    spinBox()->setValue( value().toUInt() );
	spinBox()->blockSignals( FALSE );
    }
    placeEditor( spinBox() );
    if ( !spinBox()->isVisible()  || !spinBox()->hasFocus()  ) {
	spinBox()->show();
	setFocus( spinBox() );
    }
}

void PropertyIntItem::hideEditor()
{
    PropertyItem::hideEditor();
    spinBox()->hide();
}

void PropertyIntItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( spinBx ) {
	spinBox()->blockSignals( TRUE );
	if ( signedValue )
	    spinBox()->setValue( v.toInt() );
	else
	    spinBox()->setValue( v.toUInt() );
	spinBox()->blockSignals( FALSE );
    }

    if ( signedValue )
	    setText( 1, QString::number( v.toInt() ) );
    else
	    setText( 1, QString::number( v.toUInt() ) );
    PropertyItem::setValue( v );
}

void PropertyIntItem::setValue()
{
    if ( !spinBx )
	return;
    setText( 1, QString::number( spinBox()->value() ) );
    if ( signedValue )
	PropertyItem::setValue( spinBox()->value() );
    else
	PropertyItem::setValue( (uint)spinBox()->value() );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyListItem::PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, bool e )
    : PropertyItem( l, after, prop, propName ), editable( e )
{
    comb = 0;
    oldInt = -1;
}

QComboBox *PropertyListItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( editable, listview->viewport() );
    comb->hide();
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    if ( editable ) {
	QObjectList *ol = comb->queryList( "QLineEdit" );
	if ( ol && ol->first() )
	    ol->first()->installEventFilter( listview );
	delete ol;
    }
    return comb;
}

PropertyListItem::~PropertyListItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyListItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyListItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyListItem::setValue( const QVariant &v )
{
    if ( comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( v.toStringList() );
	combo()->blockSignals( FALSE );
    }
    setText( 1, v.toStringList().first() );
    PropertyItem::setValue( v );
}

void PropertyListItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    QStringList lst;
    for ( uint i = 0; i < combo()->listBox()->count(); ++i )
	lst << combo()->listBox()->item( i )->text();
    PropertyItem::setValue( lst );
    notifyValueChange();
    oldInt = currentIntItem();
    oldString = currentItem();
}

QString PropertyListItem::currentItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentText();
}

void PropertyListItem::setCurrentItem( const QString &s )
{
    if ( comb && currentItem().lower() == s.lower() )
 	return;

    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    for ( uint i = 0; i < combo()->listBox()->count(); ++i ) {
	if ( combo()->listBox()->item( i )->text().lower() == s.lower() ) {
	    combo()->setCurrentItem( i );
	    setText( 1, combo()->currentText() );
	    break;
	}
    }
    oldInt = currentIntItem();
    oldString = currentItem();
}

void PropertyListItem::setCurrentItem( int i )
{
    if ( comb && i == combo()->currentItem() )
	return;

    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    combo()->setCurrentItem( i );
    setText( 1, combo()->currentText() );
    oldInt = currentIntItem();
    oldString = currentItem();
}

int PropertyListItem::currentIntItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentItem();
}

int PropertyListItem::currentIntItemFromObject() const
{
    return oldInt;
}

QString PropertyListItem::currentItemFromObject() const
{
    return oldString;
}

// --------------------------------------------------------------

PropertyCoordItem::PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, Type t )
    : PropertyItem( l, after, prop, propName ), typ( t )
{
    lin = 0;

}

QLineEdit *PropertyCoordItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setReadOnly( TRUE );
    lin->installEventFilter( listview );
    lin->hide();
    return lin;
}

void PropertyCoordItem::createChildren()
{
    PropertyItem *i = this;
    if ( typ == Rect || typ == Point ) {
	i = new PropertyIntItem( listview, i, this, tr( "x" ), TRUE );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "y" ), TRUE );
	addChild( i );
    }
    if ( typ == Rect || typ == Size ) {
	i = new PropertyIntItem( listview, i, this, tr( "width" ), TRUE );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "height" ), TRUE );
	addChild( i );
    }
}

void PropertyCoordItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "x" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().x() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().x() );
	} else if ( item->name() == tr( "y" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().y() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().y() );
	} else if ( item->name() == tr( "width" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().width() );
	    else if ( typ == Size )
		item->setValue( val.toSize().width() );
	} else if ( item->name() == tr( "height" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().height() );
	    else if ( typ == Size )
		item->setValue( val.toSize().height() );
	}
    }
}

PropertyCoordItem::~PropertyCoordItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void PropertyCoordItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin )
	lined()->setText( text( 1 ) );
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus()  ) {
	lined()->show();
	setFocus( lined() );
    }
}

void PropertyCoordItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertyCoordItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    if ( typ == Rect )
	s = "[ " + QString::number( v.toRect().x() ) + ", " + QString::number( v.toRect().y() ) + ", " +
	    QString::number( v.toRect().width() ) + ", " + QString::number( v.toRect().height() ) + " ]";
    else if ( typ == Point )
	s = "[ " + QString::number( v.toPoint().x() ) + ", " +
	    QString::number( v.toPoint().y() ) + " ]";
    else if ( typ == Size )
	s = "[ " + QString::number( v.toSize().width() ) + ", " +
	    QString::number( v.toSize().height() ) + " ]";
    setText( 1, s );
    if ( lin )
	lined()->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyCoordItem::hasSubItems() const
{
    return TRUE;
}

void PropertyCoordItem::childValueChanged( PropertyItem *child )
{
    if ( typ == Rect ) {
	QRect r = value().toRect();
	if ( child->name() == tr( "x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == tr( "y" ) )
	    r.setY( child->value().toInt() );
	else if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    } else if ( typ == Point ) {
	QPoint r = value().toPoint();
	if ( child->name() == tr( "x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == tr( "y" ) )
	    r.setY( child->value().toInt() );
	setValue( r );
    } else if ( typ == Size ) {
	QSize r = value().toSize();
	if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    }
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyPixmapItem::PropertyPixmapItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new QHBox( listview->viewport() );
    box->hide();
    pixPrev = new QLabel( box );
    pixPrev->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) );
    pixPrev->setBackgroundColor( pixPrev->colorGroup().color( QColorGroup::Base ) );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    box->setLineWidth( 2 );
    pixPrev->setFrameStyle( QFrame::NoFrame );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getPixmap() ) );
}
PropertyPixmapItem::~PropertyPixmapItem()
{
    delete (QHBox*)box;
}

void PropertyPixmapItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() )
	box->show();
}

void PropertyPixmapItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPixmapItem::setValue( const QVariant &v )
{
    QString s;
    pixPrev->setPixmap( v.toPixmap() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPixmapItem::getPixmap()
{
    QPixmap pix = qChoosePixmap( listview, listview->propertyEditor()->formWindow(), value().toPixmap() );
    if ( !pix.isNull() ) {
	setValue( pix );
	notifyValueChange();
    }
}

bool PropertyPixmapItem::hasCustomContents() const
{
    return TRUE;
}

void PropertyPixmapItem::drawCustomContents( QPainter *p, const QRect &r )
{
    QPixmap pix( value().toPixmap() );
    if ( !pix.isNull() ) {
	p->save();
	p->setClipRect( QRect( QPoint( (int)(p->worldMatrix().dx() + r.x()),
				       (int)(p->worldMatrix().dy() + r.y()) ),
			       r.size() ) );
	p->drawPixmap( r.x(), r.y() + ( r.height() - pix.height() ) / 2, pix );
	p->restore();
    }
}


// --------------------------------------------------------------

PropertyColorItem::PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName, bool children )
    : PropertyItem( l, after, prop, propName ), withChildren( children )
{
    box = new QHBox( listview->viewport() );
    box->hide();
    colorPrev = new QFrame( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    box->setLineWidth( 2 );
    colorPrev->setFrameStyle( QFrame::Plain | QFrame::Box );
    colorPrev->setLineWidth( 2 );
    QPalette pal = colorPrev->palette();
    QColorGroup cg = pal.active();
    cg.setColor( QColorGroup::Foreground, cg.color( QColorGroup::Base ) );
    pal.setActive( cg );
    pal.setInactive( cg );
    pal.setDisabled( cg );
    colorPrev->setPalette( pal );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getColor() ) );
}

void PropertyColorItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyIntItem( listview, i, this, tr( "Red" ), TRUE );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Green" ), TRUE );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Blue" ), TRUE );
    addChild( i );
}

void PropertyColorItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Red" ) )
	    item->setValue( val.toColor().red() );
	else if ( item->name() == tr( "Green" ) )
	    item->setValue( val.toColor().green() );
	else if ( item->name() == tr( "Blue" ) )
	    item->setValue( val.toColor().blue() );
    }
}

PropertyColorItem::~PropertyColorItem()
{
    delete (QHBox*)box;
}

void PropertyColorItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() )
	box->show();
}

void PropertyColorItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyColorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    setText( 1, v.toColor().name() );
    colorPrev->setBackgroundColor( v.toColor() );
    PropertyItem::setValue( v );
}

bool PropertyColorItem::hasSubItems() const
{
    return withChildren;
}

void PropertyColorItem::childValueChanged( PropertyItem *child )
{
    QColor c( val.toColor() );
    if ( child->name() == tr( "Red" ) )
	c.setRgb( child->value().toInt(), c.green(), c.blue() );
    else if ( child->name() == tr( "Green" ) )
	c.setRgb( c.red(), child->value().toInt(), c.blue() );
    else if ( child->name() == tr( "Blue" ) )
	c.setRgb( c.red(), c.green(), child->value().toInt() );
    setValue( c );
    notifyValueChange();
}

void PropertyColorItem::getColor()
{
    QColor c = QColorDialog::getColor( val.asColor(), listview );
    if ( c.isValid() ) {
	setValue( c );
	notifyValueChange();
    }
}

bool PropertyColorItem::hasCustomContents() const
{
    return TRUE;
}

void PropertyColorItem::drawCustomContents( QPainter *p, const QRect &r )
{
    p->save();
    p->setPen( QPen( black, 1 ) );
    p->setBrush( val.toColor() );
    p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
    p->restore();
}

// --------------------------------------------------------------

PropertyFontItem::PropertyFontItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new QHBox( listview->viewport() );
    box->hide();
    lined = new QLineEdit( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    box->setLineWidth( 2 );
    lined->setFrame( FALSE );
    lined->setReadOnly( TRUE );
    box->setFocusProxy( lined );
    box->installEventFilter( listview );
    lined->installEventFilter( listview );
    button->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getFont() ) );
}

void PropertyFontItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, tr( "Family" ), FALSE );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Point Size" ), TRUE );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Bold" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Italic" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Underline" ) );
    addChild( i );
    i = new PropertyBoolItem( listview, i, this, tr( "Strikeout" ) );
    addChild( i );
}

void PropertyFontItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Family" ) ) {
	    ( (PropertyListItem*)item )->setValue( getFontList() );
	    ( (PropertyListItem*)item )->setCurrentItem( val.toFont().family() );
	} else if ( item->name() == tr( "Point Size" ) )
	    item->setValue( val.toFont().pointSize() );
	else if ( item->name() == tr( "Bold" ) )
	    item->setValue( QVariant( val.toFont().bold(), 0 ) );
	else if ( item->name() == tr( "Italic" ) )
	    item->setValue( QVariant( val.toFont().italic(), 0 ) );
	else if ( item->name() == tr( "Underline" ) )
	    item->setValue( QVariant( val.toFont().underline(), 0 ) );
	else if ( item->name() == tr( "Strikeout" ) )
	    item->setValue( QVariant( val.toFont().strikeOut(), 0 ) );
    }
}

PropertyFontItem::~PropertyFontItem()
{
    delete (QHBox*)box;
}

void PropertyFontItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() || !lined->hasFocus() ) {
	box->show();
	setFocus( lined );
    }
}

void PropertyFontItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyFontItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;

    setText( 1, v.toFont().family() + "-" + QString::number( v.toFont().pointSize() ) );
    lined->setText( v.toFont().family() + "-" + QString::number( v.toFont().pointSize() ) );
    PropertyItem::setValue( v );
}

void PropertyFontItem::getFont()
{
    bool ok = FALSE;
    QFont f = QFontDialog::getFont( &ok, val.toFont(), listview );
    if ( ok && f != val.toFont() ) {
	setValue( f );
	notifyValueChange();
    }
}

bool PropertyFontItem::hasSubItems() const
{
    return TRUE;
}

void PropertyFontItem::childValueChanged( PropertyItem *child )
{
    QFont f = val.toFont();
    if ( child->name() == tr( "Family" ) )
	f.setFamily( ( (PropertyListItem*)child )->currentItem() );
    else if ( child->name() == tr( "Point Size" ) )
	f.setPointSize( child->value().toInt() );
    else if ( child->name() == tr( "Bold" ) )
	f.setBold( child->value().toBool() );
    else if ( child->name() == tr( "Italic" ) )
	f.setItalic( child->value().toBool() );
    else if ( child->name() == tr( "Underline" ) )
	f.setUnderline( child->value().toBool() );
    else if ( child->name() == tr( "Strikeout" ) )
	f.setStrikeOut( child->value().toBool() );
    setValue( f );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertySizePolicyItem::PropertySizePolicyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
						const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

QLineEdit *PropertySizePolicyItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->hide();
    lin->setReadOnly( TRUE );
    return lin;
}

void PropertySizePolicyItem::createChildren()
{
    QStringList lst;
    lst << "Fixed" << "Minimum" << "Maximum" << "Preferred" << "MinimumExpanding" << "Expanding";

    PropertyItem *i = this;
    i = new PropertyListItem( listview, i, this, tr( "hSizeType" ), FALSE );
    i->setValue( lst );
    addChild( i );
    i = new PropertyListItem( listview, i, this, tr( "vSizeType" ), FALSE );
    i->setValue( lst );
    addChild( i );
}

void PropertySizePolicyItem::initChildren()
{
    PropertyItem *item = 0;
    QSizePolicy sp = val.toSizePolicy();
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "hSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.horData() ) );
	else if ( item->name() == tr( "vSizeType" ) )
	    ( (PropertyListItem*)item )->setCurrentItem( size_type_to_int( sp.verData() ) );
    }
}

PropertySizePolicyItem::~PropertySizePolicyItem()
{
    delete (QLineEdit*)lin;
}

void PropertySizePolicyItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus() )
	lined()->show();
}

void PropertySizePolicyItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertySizePolicyItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;

    QString s = tr( "%1/%2" );
    s = s.arg( size_type_to_string( v.toSizePolicy().horData() ) ).
	arg( size_type_to_string( v.toSizePolicy().verData() ) );
    setText( 1, s );
    lined()->setText( s );
    PropertyItem::setValue( v );
}

void PropertySizePolicyItem::childValueChanged( PropertyItem *child )
{
    QSizePolicy sp = val.toSizePolicy();
    if ( child->name() == tr( "hSizeType" ) )
	sp.setHorData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    else if ( child->name() == tr( "vSizeType" ) )
	sp.setVerData( int_to_size_type( ( ( PropertyListItem*)child )->currentIntItem() ) );
    setValue( sp );
    notifyValueChange();
}

bool PropertySizePolicyItem::hasSubItems() const
{
    return TRUE;
}

// --------------------------------------------------------------

PropertyPaletteItem::PropertyPaletteItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    box = new QHBox( listview->viewport() );
    box->hide();
    palettePrev = new QLabel( box );
    button = new QPushButton( "...", box );
    button->setFixedWidth( 20 );
    box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    box->setLineWidth( 2 );
    palettePrev->setFrameStyle( QFrame::NoFrame );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getPalette() ) );
}
PropertyPaletteItem::~PropertyPaletteItem()
{
    delete (QHBox*)box;
}

void PropertyPaletteItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() )
	box->show();
}

void PropertyPaletteItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyPaletteItem::setValue( const QVariant &v )
{
    QString s;
    palettePrev->setPalette( v.toPalette() );
    PropertyItem::setValue( v );
    repaint();
}

void PropertyPaletteItem::getPalette()
{
    bool ok = FALSE;
    QWidget *w = listview->propertyEditor()->widget();
    if ( w->inherits( "QScrollView" ) )
	w = ( (QScrollView*)w )->viewport();
    QPalette pal = PaletteEditor::getPalette( &ok, val.toPalette(),
#if defined(QT_NON_COMMERCIAL)
					      w->backgroundMode(), listview->topLevelWidget(),
#else
					      w->backgroundMode(), listview,
#endif
 					      "choose_palette", listview->propertyEditor()->formWindow() );
    if ( !ok )
	return;
    setValue( pal );
    notifyValueChange();
}

bool PropertyPaletteItem::hasCustomContents() const
{
    return TRUE;
}

void PropertyPaletteItem::drawCustomContents( QPainter *p, const QRect &r )
{
    QPalette pal( value().toPalette() );
    p->save();
    p->setClipRect( QRect( QPoint( (int)(p->worldMatrix().dx() + r.x()),
				   (int)(p->worldMatrix().dy() + r.y()) ),
			   r.size() ) );
    QRect r2( r );
    r2.setX( r2.x() + 2 );
    r2.setY( r2.y() + 2 );
    r2.setWidth( r2.width() - 3 );
    r2.setHeight( r2.height() - 3 );
    p->setPen( QPen( black, 1 ) );
    p->setBrush( pal.active().background() );
    p->drawRect( r2 );
    p->restore();
}

// --------------------------------------------------------------

PropertyCursorItem::PropertyCursorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
					const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *PropertyCursorItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( FALSE, listview->viewport() );
    comb->hide();
    QBitmap cur;

    cur = QBitmap(arrow_width, arrow_height, arrow_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Arrow"), ArrowCursor);

    cur = QBitmap(uparrow_width, uparrow_height, uparrow_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Up-Arrow"), UpArrowCursor );

    cur = QBitmap(cross_width, cross_height, cross_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Cross"), CrossCursor );

    cur = QBitmap(wait_width, wait_height, wait_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Waiting"), WaitCursor );

    cur = QBitmap(ibeam_width, ibeam_height, ibeam_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("iBeam"), IbeamCursor );

    cur = QBitmap(sizev_width, sizev_height, sizev_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Size Vertical"), SizeVerCursor );

    cur = QBitmap(sizeh_width, sizeh_height, sizeh_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Size Horizontal"), SizeHorCursor );

    cur = QBitmap(sizef_width, sizef_height, sizef_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Size Slash"), SizeBDiagCursor );

    cur = QBitmap(sizeb_width, sizeb_height, sizeb_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Size Backslash"), SizeFDiagCursor );

    cur = QBitmap(sizeall_width, sizeall_height, sizeall_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Size All"), SizeAllCursor );

    cur = QBitmap( 25, 25, 1 );
    cur.setMask( cur );
    comb->insertItem( cur, tr("Blank"), BlankCursor );

    cur = QBitmap(vsplit_width, vsplit_height, vsplit_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Split Vertical"), SplitVCursor );

    cur = QBitmap(hsplit_width, hsplit_height, hsplit_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Split Horizontal"), SplitHCursor );

    cur = QBitmap(hand_width, hand_height, hand_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Pointing Hand"), PointingHandCursor );

    cur = QBitmap(no_width, no_height, no_bits, TRUE);
    cur.setMask( cur );
    comb->insertItem( cur, tr("Forbidden"), ForbiddenCursor );

    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyCursorItem::~PropertyCursorItem()
{
    delete (QComboBox*)comb;
}

void PropertyCursorItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->setCurrentItem( (int)value().toCursor().shape() );
	combo()->blockSignals( FALSE );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible() || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyCursorItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyCursorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    combo()->blockSignals( TRUE );
    combo()->setCurrentItem( (int)v.toCursor().shape() );
    combo()->blockSignals( FALSE );
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( v );
}

void PropertyCursorItem::setValue()
{
    if ( !comb )
	return;
    if ( QVariant( QCursor( combo()->currentItem() ) ) == val )
	return;
    setText( 1, combo()->currentText() );
    PropertyItem::setValue( QCursor( combo()->currentItem() ) );
    notifyValueChange();
}

// --------------------------------------------------------------

/*!
  \class PropertyList propertyeditor.h
  \brief PropertyList is a QListView derived class which is used for editing widget properties

  This class is used for widget properties. It has to be child of a
  PropertyEditor.

  To initialize it for editing a widget call setupProperties() which
  iterates through the properties of the current widget (see
  PropertyEditor::widget()) and builds the list.

  To update the item values, refetchData() can be called.

  If the value of an item has been changed by the user, and this
  change should be applied to the widget's property, valueChanged()
  has to be called.

  To set the value of an item, setPropertyValue() has to be called.
*/

PropertyList::PropertyList( PropertyEditor *e )
    : QListView( e ), editor( e )
{
    header()->setMovingEnabled( FALSE );
    viewport()->setBackgroundMode( PaletteBackground );
    setResizePolicy( QScrollView::Manual );
    addColumn( tr( "Property" ) );
    addColumn( tr( "Value" ) );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateEditorSize() ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
    connect( this, SIGNAL( pressed( QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( itemPressed( QListViewItem *, const QPoint &, int ) ) );
    connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
	     this, SLOT( toggleOpen( QListViewItem * ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
}

void PropertyList::resizeEvent( QResizeEvent *e )
{
    QListView::resizeEvent( e );
    QSize vs = viewportSize( 0, contentsHeight() );

    int os = header()->sectionSize( 1 );
    int ns = vs.width() - header()->sectionSize( 0 );
    if ( ns < 16 )
	ns = 16;
	
    header()->resizeSection( 1, ns );
    header()->repaint( header()->width() - header()->sectionSize( 1 ), 0, header()->sectionSize( 1 ), header()->height() );

    int elipsis = fontMetrics().width("...") + 10;
    viewport()->repaint( header()->sectionPos(1) + os - elipsis, 0, elipsis, viewport()->height(), FALSE );
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

static QVariant::Type type_to_variant( const QString &s )
{
    if ( s == "Invalid " )
	return QVariant::Invalid;
    if ( s == "Map" )
	return QVariant::Map;
    if ( s == "List" )
	return QVariant::List;
    if ( s == "String" )
	return QVariant::String;
    if ( s == "StringList" )
	return QVariant::StringList;
    if ( s == "Font" )
	return QVariant::Font;
    if ( s == "Pixmap" )
	return QVariant::Pixmap;
    if ( s == "Brush" )
	return QVariant::Brush;
    if ( s == "Rect" )
	return QVariant::Rect;
    if ( s == "Size" )
	return QVariant::Size;
    if ( s == "Color" )
	return QVariant::Color;
    if ( s == "Palette" )
	return QVariant::Palette;
    if ( s == "ColorGroup" )
	return QVariant::ColorGroup;
    if ( s == "IconSet" )
	return QVariant::IconSet;
    if ( s == "Point" )
	return QVariant::Point;
    if ( s == "Image" )
	return QVariant::Image;
    if ( s == "Int" )
	return QVariant::Int;
    if ( s == "UInt" )
	return QVariant::UInt;
    if ( s == "Bool" )
	return QVariant::Bool;
    if ( s == "Double" )
	return QVariant::Double;
    if ( s == "CString" )
	return QVariant::CString;
    if ( s == "PointArray" )
	return QVariant::PointArray;
    if ( s == "Region" )
	return QVariant::Region;
    if ( s == "Bitmap" )
	return QVariant::Bitmap;
    if ( s == "Cursor" )
	return QVariant::Cursor;
    if ( s == "SizePolicy" )
	return QVariant::SizePolicy;
    return QVariant::Invalid;	
}

/*!  Sets up the property list by adding an item for each designable
property of the widget which is just edited.
*/

void PropertyList::setupProperties()
{
    if ( !editor->widget() )
	return;
    bool allProperties = !editor->widget()->inherits( "Spacer" );
    QStrList lst = editor->widget()->metaObject()->propertyNames( allProperties );
    PropertyItem *item = 0;
    QMap<QString, bool> unique;
    QWidget *w = editor->widget();
    QStringList valueSet;
    bool parentHasLayout =
	!editor->formWindow()->isMainContainer( w ) && w->parentWidget() &&
	WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout;
    for ( QListIterator<char> it( lst ); it.current(); ++it ) {
	const QMetaProperty* p = editor->widget()->metaObject()->property( it.current(), allProperties );
	if ( !p )
	    continue;
	if ( unique.contains( QString::fromLatin1( it.current() ) ) )
	    continue;
	unique.insert( QString::fromLatin1( it.current() ), TRUE );
	if ( editor->formWindow()->isMainContainer( editor->widget() ) ) {
	    if ( qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	} else { // hide some toplevel-only stuff
	    if ( qstrcmp( p->name(), "icon" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "iconText" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "caption" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "sizeIncrement" ) == 0 )
		continue;
	    if ( qstrcmp( p->name(), "baseSize" ) == 0 )
		continue;
	    if ( parentHasLayout && qstrcmp( p->name(), "geometry" ) == 0 )
		continue;
	    if ( w->inherits( "QLayoutWidget" ) || w->inherits( "Spacer" ) ) {
		if ( qstrcmp( p->name(), "sizePolicy" ) == 0 )
		    continue;	
	    }
	}
	if ( qstrcmp( p->name(), "minimumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "minimumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumHeight" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "maximumWidth" ) == 0 )
	    continue;
	if ( qstrcmp( p->name(), "buttonGroupId" ) == 0 ) { // #### remove this when designable in Q_PROPERTY can take a function (isInButtonGroup() in this case)
	    if ( !editor->widget()->parentWidget() ||
		 !editor->widget()->parentWidget()->inherits( "QButtonGroup" ) )
		continue;
	}
	
	
	if ( p->designable() ) {
	    if ( p->isSetType() ) {
		if ( QString( p->name() ) == "alignment" ) {
		    QStringList lst;
		    lst << p->valueToKey( AlignLeft )
			<< p->valueToKey( AlignHCenter )
			<< p->valueToKey( AlignRight );
		    item = new PropertyListItem( this, item, 0, "hAlign", FALSE );
		    item->setValue( lst );
		    setPropertyValue( item );
		    if ( MetaDataBase::isPropertyChanged( editor->widget(), "hAlign" ) )
			item->setChanged( TRUE, FALSE );
		    if ( !editor->widget()->inherits( "QMultiLineEdit" ) ) {
			lst.clear();
			lst << p->valueToKey( AlignTop )
			    << p->valueToKey( AlignVCenter )
			    << p->valueToKey( AlignBottom );
			item = new PropertyListItem( this, item, 0, "vAlign", FALSE );
			item->setValue( lst );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "vAlign" ) )
			    item->setChanged( TRUE, FALSE );
			item = new PropertyBoolItem( this, item, 0, "wordwrap" );
			setPropertyValue( item );
			if ( MetaDataBase::isPropertyChanged( editor->widget(), "wordwrap" ) )
			    item->setChanged( TRUE, FALSE );
		    }
		} else {
		    qWarning( "Sets except 'alignment' not supported yet.... %s.", p->name() );
		}
	    } else if ( p->isEnumType() ) {
		QStrList l = p->enumKeys();
		QStringList lst;
		for ( uint i = 0; i < l.count(); ++i ) {
		    QString k = l.at( i );
		    // filter out enum-masks
		    if ( k[0] == 'M' && k[1].category() == QChar::Letter_Uppercase )
			continue;
		    lst << l.at( i );
		}
		item = new PropertyListItem( this, item, 0, p->name(), FALSE );
		item->setValue( lst );
	    } else {
		QVariant::Type t = QVariant::nameToType( p->type() );
		if ( !addPropertyItem( item, p->name(), t ) )
		    continue;
	    }
	}
	if ( item && !p->isSetType() ) {
	    if ( valueSet.findIndex( item->name() ) == -1 ) {
		setPropertyValue( item );
		valueSet << item->name();
	    }
	    if ( MetaDataBase::isPropertyChanged( editor->widget(), p->name() ) )
		item->setChanged( TRUE, FALSE );
	}
    }

    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ) {
	item = new PropertyIntItem( this, item, 0, "layoutSpacing", TRUE );
	setPropertyValue( item );
	item->setChanged( TRUE );
	item = new PropertyIntItem( this, item, 0, "layoutMargin", TRUE );
	setPropertyValue( item );
	item->setChanged( TRUE );
    }


    if ( !w->inherits( "Spacer" ) && !w->inherits( "QLayoutWidget" ) ) {
	item = new PropertyTextItem( this, item, 0, "toolTip", TRUE, FALSE );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "toolTip" ) )
	    item->setChanged( TRUE, FALSE );
	item = new PropertyTextItem( this, item, 0, "whatsThis", TRUE, TRUE );
	setPropertyValue( item );
	if ( MetaDataBase::isPropertyChanged( editor->widget(), "whatsThis" ) )
	    item->setChanged( TRUE, FALSE );
    }

    if ( w->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
	if ( cw ) {
	    for ( QValueList<MetaDataBase::Property>::Iterator it = cw->lstProperties.begin(); it != cw->lstProperties.end(); ++it ) {
		if ( unique.contains( QString( (*it).property ) ) )
		    continue;
		unique.insert( QString( (*it).property ), TRUE );
		addPropertyItem( item, (*it).property, type_to_variant( (*it).type ) );
		setPropertyValue( item );
		if ( MetaDataBase::isPropertyChanged( editor->widget(), (*it).property ) )
		    item->setChanged( TRUE, FALSE );
	    }
	}
    }

    setCurrentItem( firstChild() );

    updateEditorSize();
    updateEditorSize();
}

bool PropertyList::addPropertyItem( PropertyItem *&item, const QCString &name, QVariant::Type t )
{
    switch ( t ) {
    case QVariant::String:
	item = new PropertyTextItem( this, item, 0, name, TRUE, editor->widget()->inherits( "QLabel" ) || editor->widget()->inherits( "QTextView" ) );
	break;
    case QVariant::CString:
	item = new PropertyTextItem( this, item, 0, name, FALSE, FALSE, TRUE );
	break;
    case QVariant::Bool:
	item = new PropertyBoolItem( this, item, 0, name );
	break;
    case QVariant::Font:
	item = new PropertyFontItem( this, item, 0, name );
	break;
    case QVariant::Int:
	if ( name == "accel" )
	    item = new PropertyTextItem( this, item, 0, name, FALSE, FALSE, FALSE, TRUE );
	else
	    item = new PropertyIntItem( this, item, 0, name, TRUE );
	break;
    case QVariant::UInt:
	item = new PropertyIntItem( this, item, 0, name, FALSE );
	break;
    case QVariant::StringList:
	item = new PropertyListItem( this, item, 0, name, TRUE );
	break;
    case QVariant::Rect:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Rect );
	break;
    case QVariant::Point:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Point );
	break;
    case QVariant::Size:
	item = new PropertyCoordItem( this, item, 0, name, PropertyCoordItem::Size );
	break;
    case QVariant::Color:
	item = new PropertyColorItem( this, item, 0, name, TRUE );
	break;
    case QVariant::Pixmap:
	item = new PropertyPixmapItem( this, item, 0, name );
	break;
    case QVariant::SizePolicy:
	item = new PropertySizePolicyItem( this, item, 0, name );
	break;
    case QVariant::Palette:
	item = new PropertyPaletteItem( this, item, 0, name );
	break;
    case QVariant::Cursor:
	item = new PropertyCursorItem( this, item, 0, name );
	break;
    default:
	return FALSE;
    }
    return TRUE;
}

void PropertyList::paintEmptyArea( QPainter *p, const QRect &r )
{
    p->fillRect( r, backColor2 );
}

void PropertyList::setCurrentItem( QListViewItem *i )
{
    if ( currentItem() )
	( (PropertyItem*)currentItem() )->hideEditor();
    QListView::setCurrentItem( i );
    ( (PropertyItem*)currentItem() )->showEditor();
}

void PropertyList::updateEditorSize()
{
    QSize s( header()->sectionPos(1) + header()->sectionSize(1), height() );
    QResizeEvent e( s, size() );
    resizeEvent( &e );
    viewport()->repaint( s.width(), 0, width() - s.width(), height(), FALSE );
}

/*!  This has to be called if the value if \a i should be set as
  property to the currently edited widget.
*/

void PropertyList::valueChanged( PropertyItem *i )
{
    if ( !editor->widget() )
	return;
    QString pn( tr( "Set '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), WidgetFactory::property( editor->widget(), i->name() ),
						      i->value(), i->currentItem(), i->currentItemFromObject() );
    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, TRUE );
}

void PropertyList::itemPressed( QListViewItem *i, const QPoint &p, int c )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( !pi->hasSubItems() )
	return;

    if ( c == 0 && viewport()->mapFromGlobal( p ).x() < 20 )
	toggleOpen( i );
}

void PropertyList::toggleOpen( QListViewItem *i )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( pi->hasSubItems() ) {
	pi->setOpen( !pi->isOpen() );
    } else {
	pi->toggle();
    }
}

bool PropertyList::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    PropertyItem *i = (PropertyItem*)currentItem();
    if ( o != this &&e->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent*)e;
	if ( ( ke->key() == Key_Up || ke->key() == Key_Down ) &&
	     ( o != this || o != viewport() ) &&
	     !( ke->state() & ControlButton ) ) {
	    QApplication::sendEvent( this, (QKeyEvent*)e );
	    return TRUE;
	} else if ( ( !o->inherits( "QLineEdit" ) ||
		      ( o->inherits( "QLineEdit" ) && ( (QLineEdit*)o )->isReadOnly() ) ) &&
		    i && i->hasSubItems() ) {
	    if ( !i->isOpen() &&
		 ( ke->key() == Key_Plus ||
		   ke->key() == Key_Right ))
		i->setOpen( TRUE );
	    else if ( i->isOpen() &&
		 ( ke->key() == Key_Minus ||
		   ke->key() == Key_Left ) )
		i->setOpen( FALSE );
	} else if ( ( ke->key() == Key_Return || ke->key() == Key_Enter ) && o->inherits( "QComboBox" ) ) {
	    QKeyEvent ke2( QEvent::KeyPress, Key_Space, 0, 0 );
	    QApplication::sendEvent( o, &ke2 );
	    return TRUE;
	}
    } else if ( e->type() == QEvent::FocusOut && o->inherits( "QLineEdit" ) ) {
  	QTimer::singleShot( 100, editor->formWindow()->commandHistory(), SLOT( checkCompressedCommand() ) );
    }

    return QListView::eventFilter( o, e );
}

/*!  This method re-initializes each item of the property list.
*/

void PropertyList::refetchData()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	PropertyItem *i = (PropertyItem*)it.current();
	if ( !i->propertyParent() )
	    setPropertyValue( i );
	if ( i->hasSubItems() )
	    i->initChildren();
	bool changed = MetaDataBase::isPropertyChanged( editor->widget(), i->name() );
	if ( changed != i->isChanged() )
	    i->setChanged( changed, FALSE );
    }
    updateEditorSize();
}

/*!  This method initializes the value of the item \a i to the value
  of the corresponding property.
*/

void PropertyList::setPropertyValue( PropertyItem *i )
{
    const QMetaProperty *p = editor->widget()->metaObject()->property( i->name(), TRUE );
    if ( !p ) {
	if ( i->name() == "hAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->property( "alignment", TRUE );
	    align &= AlignLeft | AlignHCenter | AlignRight;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).first() );
	} else if ( i->name() == "vAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->property( "alignment", TRUE );
	    align &= AlignTop | AlignVCenter | AlignBottom;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).first() );
	} else if ( i->name() == "wordwrap" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		i->setValue( QVariant( TRUE, 0 ) );
	    else
		i->setValue( QVariant( FALSE, 0 ) );
	} else if ( i->name() == "layoutSpacing" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::spacing( WidgetFactory::containerOfWidget( editor->widget() ) ) );
	} else if ( i->name() == "layoutMargin" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::margin( WidgetFactory::containerOfWidget( editor->widget() ) ) );
	} else if ( i->name() == "toolTip" || i->name() == "whatsThis" ) {
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)editor->widget() )->customWidget();
	    if ( !cw )
		return;
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	}
	return;
    }
    if ( p->isSetType() )
	;
    else if ( p->isEnumType() )
	( (PropertyListItem*)i )->setCurrentItem( p->valueToKey( editor->widget()->property( i->name() ).toInt() ) );
    else
	i->setValue( editor->widget()->property( i->name() ) );
}

void PropertyList::setCurrentProperty( const QString &n )
{
    if ( currentItem() && currentItem()->text( 0 ) == n ||
	 currentItem() && ( (PropertyItem*)currentItem() )->propertyParent() &&
	 ( (PropertyItem*)currentItem() )->propertyParent()->text( 0 ) == n )
	return;

    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 0 ) == n ) {
	    setCurrentItem( it.current() );
	    break;
	}
    }
}

PropertyEditor *PropertyList::propertyEditor() const
{
    return editor;
}

void PropertyList::resetProperty()
{
    if ( !currentItem() )
	return;
    PropertyItem *i = (PropertyItem*)currentItem();
    if ( !MetaDataBase::isPropertyChanged( editor->widget(), i->PropertyItem::name() ) )
	return;
    QString pn( tr( "Reset '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
						      editor->widget(), editor,
						      i->name(), i->value(),
						      WidgetFactory::defaultValue( editor->widget(), i->name() ),
						      WidgetFactory::defaultCurrentItem( editor->widget(), i->name() ),
						      i->currentItem(), TRUE );
    cmd->execute();
    editor->formWindow()->commandHistory()->addCommand( cmd, FALSE );
    if ( i->hasSubItems() )
	i->initChildren();
}

// --------------------------------------------------------------

/*!
  \class PropertyEditor propertyeditor.h
  \brief PropertyEdior toplevel window

  This is the toplevel window of the property editor which contains a
  listview for editing properties.
*/

PropertyEditor::PropertyEditor( QWidget *parent )
    : QVBox( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
			WStyle_StaysOnTop | WStyle_Tool |WStyle_MinMax | WStyle_SysMenu )
{
    setCaption( tr( "Property Editor" ) );
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    wid = 0;
    formwindow = 0;
    listview = new PropertyList( this );
}

QWidget *PropertyEditor::widget() const
{
    return wid;
}

void PropertyEditor::setWidget( QWidget *w, FormWindow *fw )
{
    if ( w == wid ) {
	bool ret = TRUE;
	if ( WidgetFactory::layoutType( wid ) != WidgetFactory::NoLayout ) {
	    QListViewItemIterator it( listview );
	    ret = FALSE;
	    while ( it.current() ) {
		if ( it.current()->text( 0 ) == "layoutSpacing" || it.current()->text( 0 ) == "layoutMargin" ) {
		    ret = TRUE;
		    break;
		}
		++it;
	    }
	}
	if ( ret )
	    return;
    }

    if ( !w || !fw ) {
	setCaption( tr( "Property Editor" ) );
	clear();
	wid = 0;
	formwindow = 0;
	return;
    }

    wid = w;
    formwindow = fw;
    setCaption( tr( "Property Editor (%1)" ).arg( formwindow->name() ) );
    listview->viewport()->setUpdatesEnabled( FALSE );
    listview->setUpdatesEnabled( FALSE );
    clear();
    listview->viewport()->setUpdatesEnabled( TRUE );
    listview->setUpdatesEnabled( TRUE );
    setup();
}

void PropertyEditor::clear()
{
    listview->setContentsPos( 0, 0 );
    listview->clear();
}

void PropertyEditor::setup()
{
    listview->viewport()->setUpdatesEnabled( FALSE );
    listview->setupProperties();
    listview->viewport()->setUpdatesEnabled( TRUE );
    qApp->processEvents();
    listview->updateEditorSize();
}

void PropertyEditor::refetchData()
{
    listview->refetchData();
}

void PropertyEditor::emitWidgetChanged()
{
    if ( formwindow && wid )
	formwindow->widgetChanged( wid );
}

void PropertyEditor::closed( FormWindow *w )
{
    if ( w == formwindow ) {
	formwindow = 0;
	wid = 0;
	clear();
    }
}

void PropertyEditor::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

PropertyList *PropertyEditor::propertyList() const
{
    return listview;
}

FormWindow *PropertyEditor::formWindow() const
{
    return formwindow;
}

QString PropertyEditor::currentProperty() const
{
    if ( !wid )
	return QString::null;
    if ( ( (PropertyItem*)listview->currentItem() )->propertyParent() )
	return ( (PropertyItem*)listview->currentItem() )->propertyParent()->name();
    return ( (PropertyItem*)listview->currentItem() )->name();
}

QString PropertyEditor::classOfCurrentProperty() const
{
    if ( !wid )
	return QString::null;
    QObject *o = wid;
    QString curr = currentProperty();
    QMetaObject *mo = o->metaObject();
    while ( mo ) {
	QStrList props = mo->propertyNames( FALSE );
	if ( props.find( curr.latin1() ) != -1 )
	    return mo->className();
	mo = mo->superClass();
    }
    return QString::null;
}

QMetaObject* PropertyEditor::metaObjectOfCurrentProperty() const
{
    if ( !wid )
	return 0;
    return wid->metaObject();
}

void PropertyEditor::resetFocus()
{
    if ( listview->currentItem() )
	( (PropertyItem*)listview->currentItem() )->showEditor();
}
