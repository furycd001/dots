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
#ifndef render_applet_h
#define render_applet_h

#include "render_replaced.h"

#include <qwidget.h>
#include <qmap.h>
#include <html_objectimpl.h>

class QScrollView;

namespace khtml {

class RenderApplet : public RenderWidget
{
public:
  RenderApplet(QScrollView *view,
               QMap<QString, QString> args, DOM::HTMLElementImpl *node);
    virtual ~RenderApplet();

    virtual const char *renderName() const { return "RenderApplet"; }

    virtual void layout();
    virtual short intrinsicWidth() const;
    virtual int intrinsicHeight() const;

private:
    void processArguments( QMap<QString, QString> args );

    DOM::HTMLElementImpl *m_applet;
};

class RenderEmptyApplet : public RenderWidget
{
public:
    RenderEmptyApplet(QScrollView *view);

    virtual const char *renderName() const { return "RenderEmptyApplet"; }

    virtual short intrinsicWidth() const;
    virtual int intrinsicHeight() const;
    virtual void layout();
};

};
#endif
