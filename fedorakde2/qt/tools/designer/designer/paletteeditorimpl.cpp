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

#include "paletteeditorimpl.h"
#include "previewstack.h"
#include "styledbutton.h"
#include "pixmapchooser.h"
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qtoolbutton.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qapplication.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qscrollview.h>

#include "mainwindow.h"
#include "formwindow.h"

/*!
    Class used by PaletteEditor for bold combobox items
*/

class BoldListBoxText : public QListBoxText
{
public:
    BoldListBoxText( QString text, QListBox* lb = 0 );

protected:
    virtual void paint( QPainter* );
};

BoldListBoxText::BoldListBoxText( QString text, QListBox* lb )
    : QListBoxText( lb )
{
    setText( text );
}

void BoldListBoxText::paint( QPainter* painter )
{
    QFont f = painter->font();
    f.setBold( TRUE );
    painter->setFont( f );

    QListBoxText::paint( painter );
}

static QWidget* target = 0;

/*!
    Constructs a PaletteEditor. By default, the apply-button will be hidden, but the
    static function using a target widget can be used to enable apply.
    \sa getPalette()
*/
PaletteEditor::PaletteEditor( FormWindow *fw, QWidget * parent, const char * name, bool modal, WFlags f )
    : PaletteEditorBase( parent, name, modal, f ), formWindow( fw )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    if ( !target ) {
	buttonApply->hide();
    } else {
	if ( !target->inherits( "QScrollView" ) )
	    setupBackgroundMode( target->backgroundMode() );
	else
	    setupBackgroundMode( ( (QScrollView*)target )->viewport()->backgroundMode() );
    }

    buttonActivePixmap->setEditor( StyledButton::PixmapEditor );
    buttonInactivePixmap->setEditor( StyledButton::PixmapEditor );
    buttonDisabledPixmap->setEditor( StyledButton::PixmapEditor );

    editPalette = target ? target->palette() : QApplication::palette();

    setPreviewPalette( editPalette );

    buttonMainColor->setColor( editPalette.active().color( QColorGroup::Button ) );
    buttonMainColor2->setColor( editPalette.active().color( QColorGroup::Background ) );

    onToggleBuildAll( target ? !target->ownPalette() : TRUE );

    buttonActivePixmap->setFormWindow( formWindow );
    buttonInactivePixmap->setFormWindow( formWindow );
    buttonDisabledPixmap->setFormWindow( formWindow );

    checkBuildActiveEffect->setChecked( FALSE );
    checkBuildActiveEffect->setChecked( TRUE );
    checkBuildInactiveEffect->setChecked( FALSE );
    checkBuildInactiveEffect->setChecked( TRUE );
    checkBuildDisabledEffect->setChecked( FALSE );
    checkBuildDisabledEffect->setChecked( TRUE );
}

PaletteEditor::~PaletteEditor()
{
}

/*!
    Applies current palette to target specified in static function
    \sa getPalette()
*/
void PaletteEditor::apply()
{
    if ( target )
	target->setPalette( editPalette );
}

/*!
    \reimp
*/
void PaletteEditor::onChooseMainColor()
{
    buildPalette();
}

void PaletteEditor::onChoose2ndMainColor()
{
    buildPalette();
}

void PaletteEditor::onChooseActiveCentralColor()
{
    mapToActiveCentralRole( buttonActiveCentral->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseActiveEffectColor()
{
    mapToActiveEffectRole( buttonActiveEffect->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseActivePixmap()
{
    if ( buttonActivePixmap->pixmap() )
	mapToActivePixmapRole( * buttonActivePixmap->pixmap() );

    updateStyledButtons();
}

void PaletteEditor::onChooseInactiveCentralColor()
{
    mapToInactiveCentralRole( buttonInactiveCentral->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseInactiveEffectColor()
{
    mapToInactiveEffectRole( buttonInactiveEffect->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseInactivePixmap()
{
    if ( buttonInactivePixmap->pixmap() )
	mapToInactivePixmapRole( *buttonInactivePixmap->pixmap() );

    updateStyledButtons();
}

void PaletteEditor::onChooseDisabledCentralColor()
{
    mapToDisabledCentralRole( buttonDisabledCentral->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseDisabledEffectColor()
{
    mapToDisabledEffectRole( buttonDisabledEffect->color() );

    updateStyledButtons();
}

void PaletteEditor::onChooseDisabledPixmap()
{
    if ( buttonDisabledPixmap->pixmap() )
	mapToDisabledPixmapRole( *buttonDisabledPixmap->pixmap() );

    updateStyledButtons();
}

void PaletteEditor::onToggleBuildAll( bool on )
{
    groupActiveCentral->setEnabled( !on );
    groupActiveEffect->setEnabled( !on );

    groupInactiveCentral->setEnabled( !on ? !checkBuildInactive->isChecked() : !on );
    groupInactiveEffect->setEnabled( !on ? !checkBuildInactive->isChecked() : !on );
    groupInactiveAuto->setEnabled( !on );

    groupDisabledCentral->setEnabled( !on ? !checkBuildDisabled->isChecked() : !on );
    groupDisabledEffect->setEnabled( !on ? !checkBuildDisabled->isChecked() : !on );
    groupDisabledAuto->setEnabled( !on );

    if (on)
	buildPalette();
}

void PaletteEditor::onToggleActiveBuildEffects( bool on )
{
    if (on)
	buildActiveEffect();
}

void PaletteEditor::onToggleBuildInactive( bool on )
{
    groupInactiveCentral->setEnabled( !on );
    groupInactiveEffect->setEnabled( !on );

    if (on)
	buildInactive();
}

void PaletteEditor::onToggleInactiveBuildEffects( bool on )
{
    if (on)
	buildInactiveEffect();
}

void PaletteEditor::onToggleBuildDisabled( bool on )
{
    groupDisabledCentral->setEnabled( !on );
    groupDisabledEffect->setEnabled( !on );

    if (on)
	buildDisabled();
}

void PaletteEditor::onToggleDisabledBuildEffects( bool on )
{
    if (on)
	buildDisabledEffect();
}

void PaletteEditor::onTabPage( const QString& /*page*/ )
{
    setPreviewPalette( editPalette );
}

QColorGroup::ColorRole PaletteEditor::centralFromItem( int item )
{
    switch( item )
	{
	case 0:
	    return QColorGroup::Background;
	case 1:
	    return QColorGroup::Foreground;
	case 2:
	    return QColorGroup::Button;
	case 3:
	    return QColorGroup::Base;
	case 4:
	    return QColorGroup::Text;
	case 5:
	    return QColorGroup::BrightText;
	case 6:
	    return QColorGroup::ButtonText;
	case 7:
	    return QColorGroup::Highlight;
	case 8:
	    return QColorGroup::HighlightedText;
	default:
	    return QColorGroup::NColorRoles;
	}
}

QColorGroup::ColorRole PaletteEditor::effectFromItem( int item )
{
    switch( item )
	{
	case 0:
	    return QColorGroup::Light;
	case 1:
	    return QColorGroup::Midlight;
	case 2:
	    return QColorGroup::Mid;
	case 3:
	    return QColorGroup::Dark;
	case 4:
	    return QColorGroup::Shadow;
	default:
	    return QColorGroup::NColorRoles;
	}
}

void PaletteEditor::onActiveCentral( int item )
{
    buttonActiveCentral->setColor( editPalette.active().color( centralFromItem(item) ) );
    QPixmap* p = editPalette.active().brush( centralFromItem(item) ).pixmap();
    if ( p )
	buttonActivePixmap->setPixmap( *p );
    else
	buttonActivePixmap->setPixmap( QPixmap() );
}

void PaletteEditor::onActiveEffect( int item )
{
    buttonActiveEffect->setColor( editPalette.active().color( effectFromItem(item) ) );
}

void PaletteEditor::onInactiveCentral( int item )
{
    buttonInactiveCentral->setColor( editPalette.inactive().color( centralFromItem(item) ) );
    QPixmap* p = editPalette.inactive().brush( centralFromItem(item) ).pixmap();
    if ( p )
	buttonInactivePixmap->setPixmap( *p );
    else
	buttonInactivePixmap->setPixmap( QPixmap() );
}

void PaletteEditor::onInactiveEffect( int item )
{
    buttonInactiveEffect->setColor( editPalette.inactive().color( effectFromItem(item) ) );
}

void PaletteEditor::onDisabledCentral( int item )
{
    buttonDisabledCentral->setColor( editPalette.disabled().color( centralFromItem(item) ) );
    QPixmap* p = editPalette.disabled().brush( centralFromItem(item) ).pixmap();
    if ( p )
	buttonDisabledPixmap->setPixmap( *p );
    else
	buttonDisabledPixmap->setPixmap( QPixmap() );
}

void PaletteEditor::onDisabledEffect( int item )
{
    buttonDisabledEffect->setColor( editPalette.disabled().color( effectFromItem(item) ) );
}

void PaletteEditor::mapToActiveCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.active();
    cg.setColor( centralFromItem(comboActiveCentral->currentItem()), c );
    editPalette.setActive( cg );

    if ( checkBuildActiveEffect->isChecked() )
	buildActiveEffect();
    if ( checkBuildInactive->isChecked() )
	buildInactive();
    if ( checkBuildDisabled->isChecked() )
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToActiveEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.active();
    cg.setColor( effectFromItem(comboActiveEffect->currentItem()), c );
    editPalette.setActive( cg );

    if ( checkBuildInactive->isChecked() )
	buildInactive();
    if ( checkBuildDisabled->isChecked() )
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToActivePixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboActiveCentral->currentItem());
    QColorGroup cg = editPalette.active();
    if (  !pm.isNull()  )
	cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, QBrush( cg.color( role ) ) );
    editPalette.setActive( cg );

    if ( checkBuildActiveEffect->isChecked() )
	buildActiveEffect();
    if ( checkBuildInactive->isChecked() )
	buildInactive();
    if ( checkBuildDisabled->isChecked() )
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToInactiveCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.inactive();
    cg.setColor( centralFromItem(comboInactiveCentral->currentItem()), c );
    editPalette.setInactive( cg );

    if ( checkBuildInactiveEffect->isChecked() )
	buildInactiveEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToInactiveEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.inactive();
    cg.setColor( effectFromItem(comboInactiveEffect->currentItem()), c );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToInactivePixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboInactiveCentral->currentItem());
    QColorGroup cg = editPalette.inactive();
    if (  !pm.isNull()  )
	cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, QBrush( cg.color( role ) ) );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToDisabledCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.disabled();
    cg.setColor( centralFromItem(comboDisabledCentral->currentItem()), c );
    editPalette.setDisabled( cg );

    if ( checkBuildDisabledEffect->isChecked() )
	buildDisabledEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToDisabledEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.disabled();
    cg.setColor( effectFromItem(comboDisabledEffect->currentItem()), c );
    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditor::mapToDisabledPixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboDisabledCentral->currentItem());
    QColorGroup cg = editPalette.disabled();
    if (  !pm.isNull()  )
	cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, QBrush( cg.color( role ) ) );

    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditor::buildPalette()
{
    int i;
    QColorGroup cg;
    QColor btn = buttonMainColor->color();
    QColor back = buttonMainColor2->color();
    QPalette automake( btn, back );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), automake.active().color( centralFromItem(i) ) );

    editPalette.setActive( cg );
    buildActiveEffect();

    cg = editPalette.inactive();

    QPalette temp( editPalette.active().color( QColorGroup::Button ),
		   editPalette.active().color( QColorGroup::Background ) );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.inactive().color( centralFromItem(i) ) );

    editPalette.setInactive( cg );
    buildInactiveEffect();

    cg = editPalette.disabled();

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.disabled().color( centralFromItem(i) ) );

    editPalette.setDisabled( cg );
    buildDisabledEffect();

    updateStyledButtons();
}

void PaletteEditor::buildActiveEffect()
{
    QColorGroup cg = editPalette.active();
    QColor btn = cg.color( QColorGroup::Button );

    QPalette temp( btn, btn );

    for (int i = 0; i<5; i++)
	cg.setColor( effectFromItem(i), temp.active().color( effectFromItem(i) ) );

    editPalette.setActive( cg );
    setPreviewPalette( editPalette );

    updateStyledButtons();
}

void PaletteEditor::buildInactive()
{
    editPalette.setInactive( editPalette.active() );
    buildInactiveEffect();
}

void PaletteEditor::buildInactiveEffect()
{
    QColorGroup cg = editPalette.inactive();

    QColor light, midlight, mid, dark, shadow;
    QColor btn = cg.color( QColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( QColorGroup::Light, light );
    cg.setColor( QColorGroup::Midlight, midlight );
    cg.setColor( QColorGroup::Mid, mid );
    cg.setColor( QColorGroup::Dark, dark );
    cg.setColor( QColorGroup::Shadow, shadow );

    editPalette.setInactive( cg );
    setPreviewPalette( editPalette );
    updateStyledButtons();
}

void PaletteEditor::buildDisabled()
{
    QColorGroup cg = editPalette.active();
    cg.setColor( QColorGroup::ButtonText, darkGray );
    cg.setColor( QColorGroup::Foreground, darkGray );
    editPalette.setDisabled( cg );

    buildDisabledEffect();
}

void PaletteEditor::buildDisabledEffect()
{
    QColorGroup cg = editPalette.disabled();

    QColor light, midlight, mid, dark, shadow;
    QColor btn = cg.color( QColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( QColorGroup::Light, light );
    cg.setColor( QColorGroup::Midlight, midlight );
    cg.setColor( QColorGroup::Mid, mid );
    cg.setColor( QColorGroup::Dark, dark );
    cg.setColor( QColorGroup::Shadow, shadow );

    editPalette.setDisabled( cg );
    setPreviewPalette( editPalette );
    updateStyledButtons();
}

void PaletteEditor::setPreviewPalette( const QPalette& pal )
{
    QColorGroup cg;

    switch ( tabWidget->currentPageIndex() ) {
    case 0:
	cg = pal.active();
	break;
    case 1:
	cg = pal.inactive();
	break;
    case 2:
	cg = pal.disabled();
	break;
    }
    previewPalette.setActive( cg );
    previewPalette.setInactive( cg );
    previewPalette.setDisabled( cg );
    previewActive->setPreviewPalette( previewPalette );
    previewInactive->setPreviewPalette( previewPalette );
    previewDisabled->setPreviewPalette( previewPalette );
}

void PaletteEditor::updateStyledButtons()
{
    buttonMainColor->setColor( editPalette.active().color( QColorGroup::Button ));
    buttonMainColor2->setColor( editPalette.active().color( QColorGroup::Background ));
    buttonActiveCentral->setColor( editPalette.active().color( centralFromItem( comboActiveCentral->currentItem() ) ) );
    buttonActiveEffect->setColor( editPalette.active().color( effectFromItem( comboActiveEffect->currentItem() ) ) );
    QPixmap* pm = editPalette.active().brush( centralFromItem( comboActiveCentral->currentItem() ) ).pixmap();
    if ( pm && !pm->isNull() )
	buttonActivePixmap->setPixmap( *pm );
    buttonInactiveCentral->setColor( editPalette.inactive().color( centralFromItem( comboInactiveCentral->currentItem() ) ) );
    buttonInactiveEffect->setColor( editPalette.inactive().color( effectFromItem( comboInactiveEffect->currentItem() ) ) );
    pm = editPalette.inactive().brush( centralFromItem( comboActiveCentral->currentItem() ) ).pixmap();
    if ( pm && !pm->isNull() )
	buttonInactivePixmap->setPixmap( *pm );
    buttonDisabledCentral->setColor( editPalette.disabled().color( centralFromItem( comboDisabledCentral->currentItem() ) ) );
    buttonDisabledEffect->setColor( editPalette.disabled().color( effectFromItem( comboDisabledEffect->currentItem() ) ) );
    pm = editPalette.disabled().brush( centralFromItem( comboActiveCentral->currentItem() ) ).pixmap();
    if ( pm && !pm->isNull() )
	buttonDisabledPixmap->setPixmap( *pm );
}

void PaletteEditor::setPal( const QPalette& pal )
{
    checkBuildPalette->setChecked( FALSE );
    editPalette = pal;
    setPreviewPalette( pal );
    updateStyledButtons();
}

QPalette PaletteEditor::pal() const
{
    return editPalette;
}

void PaletteEditor::setupBackgroundMode( BackgroundMode mode )
{
    int initRole = 0;

    switch( mode ) {
    case PaletteBackground:
	initRole = 0;
	break;
    case PaletteForeground:
	initRole = 1;
	break;
    case PaletteButton:
	initRole = 2;
	break;
    case PaletteBase:
	initRole = 3;
	break;
    case PaletteText:
	initRole = 4;
	break;
    case PaletteBrightText:
	initRole = 5;
	break;
    case PaletteButtonText:
	initRole = 6;
	break;
    case PaletteHighlight:
	initRole = 7;
	break;
    case PaletteHighlightedText:
	initRole = 8;
	break;
    case PaletteLight:
	initRole = 9;
	break;
    case PaletteMidlight:
	initRole = 10;
	break;
    case PaletteDark:
	initRole = 11;
	break;
    case PaletteMid:
	initRole = 12;
	break;
    case PaletteShadow:
	initRole = 13;
	break;
    default:
	initRole = -1;
	break;
    }

    if ( initRole > -1 ) {
	if (initRole > 8 ) {
	    comboActiveEffect->setCurrentItem( initRole - 9 );
	    comboInactiveEffect->setCurrentItem( initRole - 9 );
	    comboDisabledEffect->setCurrentItem( initRole - 9 );
	    if ( comboActiveEffect->listBox() ) {
		QString text = comboActiveEffect->currentText();
		comboActiveEffect->listBox()->changeItem( new BoldListBoxText( text ), initRole - 9 );
		comboInactiveEffect->listBox()->changeItem( new BoldListBoxText( text ), initRole - 9 );
		comboDisabledEffect->listBox()->changeItem( new BoldListBoxText( text ), initRole - 9 );
	    }
	} else {
	    comboActiveCentral->setCurrentItem( initRole );
	    comboInactiveCentral->setCurrentItem( initRole );
	    comboDisabledCentral->setCurrentItem( initRole );
	    if ( comboActiveCentral->listBox() ) {
		QString text = comboActiveCentral->currentText();
		comboActiveCentral->listBox()->changeItem( new BoldListBoxText( text ), initRole );
		comboInactiveCentral->listBox()->changeItem( new BoldListBoxText( text ), initRole );
		comboDisabledCentral->listBox()->changeItem( new BoldListBoxText( text ), initRole );
	    }
	}
    }
}

QPalette PaletteEditor::getPalette( bool *ok, const QPalette &init, BackgroundMode mode, QWidget* parent, const char* name, FormWindow *fw )
{
    target = 0;
    PaletteEditor* dlg = new PaletteEditor( fw, parent, name, TRUE );
    dlg->setupBackgroundMode( mode );
    dlg->setCaption( PaletteEditor::tr( "Change palette" ) );

    if ( init != QPalette() )
        dlg->setPal( init );
    int resultCode = dlg->exec();

    QPalette result = init;
    if ( resultCode == QDialog::Accepted ) {
	if ( ok )
	    *ok = TRUE;
	result = dlg->pal();
    } else {
	if ( ok )
	    *ok = FALSE;
    }
    delete dlg;
    return result;
}

void PaletteEditor::getPalette( bool *ok, QWidget* t, QWidget* parent, const char* name )
{
    target = t;
    PaletteEditor* dlg = new PaletteEditor( 0, parent, name, TRUE );
    dlg->setCaption( PaletteEditor::tr( "Change palette" ) );
    int resultCode = dlg->exec();
    if ( resultCode == QDialog::Accepted ) {
	if ( ok )
	    *ok = TRUE;
	target->setPalette( dlg->pal() );
    } else {
	if ( ok )
	    *ok = FALSE;
    }

    delete dlg;
}
