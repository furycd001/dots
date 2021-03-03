/**
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#include <config.h>
#include <klocale.h>

#include <kdebug.h>

#include "render_applet.h"
#include "../khtmlview.h"
#include "../khtml_part.h"

#include <qlabel.h>
#include <qscrollview.h>

#include <java/kjavaappletwidget.h>
#include <misc/htmltags.h>

using namespace khtml;
using namespace DOM;

RenderApplet::RenderApplet(QScrollView *view,
                           QMap<QString, QString> args, HTMLElementImpl *applet)
    : RenderWidget(view)
{
    // init RenderObject attributes
    setInline(true);
    m_applet = applet;

    KJavaAppletContext *context = 0;
    KHTMLView *_view = static_cast<KHTMLView*>(view);
    if ( _view ) {
        KHTMLPart *part = _view->part();
        context = part->createJavaContext();
    }

    if ( context ) {
        //kdDebug(6100) << "RenderApplet::RenderApplet, setting QWidget" << endl;
        setQWidget( new KJavaAppletWidget(context, view->viewport()) );
        processArguments(args);
    }
}

RenderApplet::~RenderApplet()
{
}

short RenderApplet::intrinsicWidth() const
{
    int rval = 150;

    if( m_widget )
        rval = ((KJavaAppletWidget*)(m_widget))->sizeHint().width();

    //kdDebug(6100) << "RenderApplet::intrinsicWidth(), returning " << rval << endl;
    return rval;
}

int RenderApplet::intrinsicHeight() const
{
    int rval = 150;

    if( m_widget )
        rval = m_widget->sizeHint().height();

    //kdDebug(6100) << "RenderApplet::intrinsicHeight(), returning " << rval << endl;
    return rval;
}

void RenderApplet::layout()
{
    //kdDebug(6100) << "RenderApplet::layout" << endl;

    if(layouted()) return;

    calcWidth();
    calcHeight();

    KJavaAppletWidget *tmp = static_cast<KJavaAppletWidget*>(m_widget);
    if ( tmp ) {
        NodeImpl *child = m_applet->firstChild();

        while(child) {

            if(child->id() == ID_PARAM) {
                HTMLParamElementImpl *p = static_cast<HTMLParamElementImpl *>(child);
                if(tmp->applet())
                    tmp->applet()->setParameter( p->name(), p->value());
            }
            child = child->nextSibling();
        }
        //kdDebug(6100) << "setting applet widget to size: " << m_width << ", " << m_height << endl;
        m_widget->resize(m_width-marginLeft()-marginRight()-paddingLeft()-paddingRight(),
                         m_height-marginTop()-marginBottom()-paddingTop()-paddingBottom());
        tmp->showApplet();
    }

    setLayouted();
}

void RenderApplet::processArguments(QMap<QString, QString> args)
{
    KJavaAppletWidget *w = static_cast<KJavaAppletWidget*>(m_widget);
    KJavaApplet* applet = w ? w->applet() : 0;

    if ( applet ) {
        applet->setBaseURL( args[QString::fromLatin1("baseURL") ] );
        applet->setAppletClass( args[QString::fromLatin1("code") ] );

        if( !args[QString::fromLatin1("codeBase") ].isEmpty() )
            applet->setCodeBase( args[QString::fromLatin1("codeBase") ] );

        if( !args[QString::fromLatin1("name") ].isNull() )
            applet->setAppletName( args[QString::fromLatin1("name") ] );
        else
            applet->setAppletName( args[QString::fromLatin1("code") ] );

        if( !args[QString::fromLatin1("archive") ].isEmpty() )
            applet->setArchives( args[QString::fromLatin1("archive") ] );
    }
}

RenderEmptyApplet::RenderEmptyApplet(QScrollView *view)
  : RenderWidget( view )
{
    // init RenderObject attributes
    setInline(true);

    QLabel* label = new QLabel(i18n("Java Applet is not loaded. (Java interpreter disabled)"), view->viewport());
    label->setAlignment( Qt::AlignCenter | Qt::WordBreak );
    setQWidget(label);
}

short RenderEmptyApplet::intrinsicWidth() const
{
    return (m_widget ? m_widget->sizeHint().width() : 150);
}

int RenderEmptyApplet::intrinsicHeight() const
{
    return (m_widget ? m_widget->sizeHint().height() : 150);
}

void RenderEmptyApplet::layout()
{
    if(layouted())
        return;

    calcWidth();
    calcHeight();

    if(m_widget)
    {
        //kdDebug(6100) << "RenderEmptyApplet::layout, m_width = " << m_width << ", m_height = " << m_height << endl;
        m_widget->resize(m_width-marginLeft()-marginRight()-paddingLeft()-paddingRight(),
                         m_height-marginTop()-marginBottom()-paddingTop()-paddingBottom());
    }

    setLayouted();
}
