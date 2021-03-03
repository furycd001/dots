/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 2000 Yves Arrouye <yves@realnames.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/* $Id: kurisearchfilter.h,v 1.2 2001/02/07 20:02:08 deller Exp $ */

#ifndef __KURISEARCHFILTER_H__
#define __KURISEARCHFILTER_H__

#include <dcopobject.h>
#include <klibloader.h>

#include <kurifilter.h>

class KInstance;

class KURISearchFilter : public KURIFilterPlugin, public DCOPObject
{
    K_DCOP
public:
    KURISearchFilter(QObject *parent = 0, const char *name = 0);
    ~KURISearchFilter();

    virtual bool filterURI( KURIFilterData& ) const;
    virtual KCModule *configModule(QWidget *parent = 0, const char *name = 0) const;
    virtual QString configName() const;

k_dcop:
    virtual void configure();
};

class KURISearchFilterFactory : public KLibFactory
{
    Q_OBJECT
public:
    KURISearchFilterFactory( QObject *parent = 0, const char *name = 0 );
    ~KURISearchFilterFactory();

    virtual QObject *create( QObject *parent = 0, const char *name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );
    static KInstance *instance();

private:
    static KInstance *s_instance;
};

#endif

