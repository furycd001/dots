/*
 *  $Id$
 *  Proxy Auto Configuration
 *  
 *  Copyright (C) 2000 Malte Starostik <malte.starostik@t-online.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#ifndef _KPAC_H_
#define _KPAC_H_

class KURL;

class KPAC
{
public:
    /**
     * Returns the proxy for the @p url or QString::null
     * if the request should be done unproxied
     */
    virtual QString proxyForURL(const KURL &url) = 0;
    /**
     * Loads the PAC-script
     * @param url URL of the script.
     */
    virtual bool init(const KURL &url) = 0;
    /**
     * Tries to discover a PAC-script and loads it.
     */
    virtual bool discover() = 0;
    /**
     * Marks @p proxy as down. If the config script returns
     * alternative proxies or allows a direct connection
     * as fallback, this proxy will not be returned for
     * a while.
     */
    virtual void badProxy(const QString &proxy) = 0;
};

#endif

