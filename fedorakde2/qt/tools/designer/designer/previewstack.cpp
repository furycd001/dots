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

#include <qvariant.h>  // HP-UX compiler needs this here
#include "previewstack.h"
#include "widgetpreview.h"
#include "checkboxpreview.h"
#include "radiopreview.h"
#include "texteditpreview.h"
#include "listviewpreview.h"
#include <qlayout.h>
#include <qobjectlist.h>
#include <qapplication.h>

static const int pc = 5;

PreviewStack::PreviewStack( QWidget* parent, const char* name )
: QWidgetStack( parent, name )
{
    int i;
    QWidget* page[ pc ];
    QBoxLayout* layout[ pc ];

    for ( i = 0; i < pc; i++) {
	page[i] = new QWidget( );
	layout[i] = new QVBoxLayout( page[i] );
	addWidget( page[i], i );
    }

    layout[0]->addWidget( new WidgetPreview( page[0] ), 1, AlignHCenter );
    layout[1]->addWidget( new CheckboxPreview( page[1] ), 1, AlignHCenter );
    layout[2]->addWidget( new RadioPreview( page[2] ), 1, AlignHCenter );
    layout[3]->addWidget( new TextEditPreview( page[3] ), 1, AlignHCenter );
    layout[4]->addWidget( new ListViewPreview( page[4] ), 1, AlignHCenter );

    raiseWidget(0);
}

PreviewStack::~PreviewStack()
{
}

void PreviewStack::nextWidget()
{
    int newid = id(visibleWidget()) + 1;
    raiseWidget( newid % pc );
}

void PreviewStack::previousWidget()
{
    int newid = id(visibleWidget()) - 1;
    raiseWidget( newid < 0 ? pc-1 : newid );
}

/*!
    \reimp
*/

void PreviewStack::setPreviewPalette( const QPalette& p )
{
    QObjectList* list = queryList("QWidget");
    QObjectListIt it(*list);

    for ( ; it.current(); ++it)
        ((QWidget*) it.current())->setPalette( p );
}
