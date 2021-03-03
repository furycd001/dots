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

#include "paletteeditor.h"

class FormWindow;

class PaletteEditor : public PaletteEditorBase
{
    Q_OBJECT
public:
    PaletteEditor( FormWindow *fw, QWidget * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0 );
    ~PaletteEditor();

    static QPalette getPalette( bool *ok, const QPalette &pal, BackgroundMode mode = PaletteBackground, 
				QWidget* parent = 0, const char* name = 0, FormWindow *fw = 0 );
    static void getPalette( bool *ok, QWidget* target, QWidget* parent = 0, const char* name = 0 );

protected slots:
    void apply();

    void onActiveCentral( int );
    void onActiveEffect( int );
    void onInactiveCentral( int );
    void onInactiveEffect( int );
    void onDisabledCentral( int );
    void onDisabledEffect( int );

    void onChooseMainColor();
    void onChoose2ndMainColor();
    void onChooseActiveCentralColor();
    void onChooseActiveEffectColor();
    void onChooseActivePixmap();
    void onChooseInactiveCentralColor();
    void onChooseInactiveEffectColor();
    void onChooseInactivePixmap();
    void onChooseDisabledCentralColor();
    void onChooseDisabledEffectColor();
    void onChooseDisabledPixmap();

    void onToggleBuildAll( bool );
    void onToggleBuildInactive( bool );
    void onToggleBuildDisabled( bool );
    void onToggleActiveBuildEffects( bool );
    void onToggleInactiveBuildEffects( bool );
    void onToggleDisabledBuildEffects( bool );

    void onTabPage( const QString& );

protected:
    void mapToActiveCentralRole( const QColor& );
    void mapToActiveEffectRole( const QColor& );
    void mapToActivePixmapRole( const QPixmap& );
    void mapToInactiveCentralRole( const QColor& );
    void mapToInactiveEffectRole( const QColor& );
    void mapToInactivePixmapRole( const QPixmap& );
    void mapToDisabledCentralRole( const QColor& );
    void mapToDisabledEffectRole( const QColor& );
    void mapToDisabledPixmapRole( const QPixmap& );


    void buildPalette();
    void buildActiveEffect();
    void buildInactive();
    void buildInactiveEffect();
    void buildDisabled();
    void buildDisabledEffect();

private:
    void setPreviewPalette( const QPalette& );
    void updateStyledButtons();
    void setupBackgroundMode( BackgroundMode );

    QPalette pal() const;
    void setPal( const QPalette& );

    QColorGroup::ColorRole centralFromItem( int );
    QColorGroup::ColorRole effectFromItem( int );
    QPalette editPalette;
    QPalette previewPalette;

    FormWindow *formWindow;
};
