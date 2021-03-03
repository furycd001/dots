/* This file is part of the KDE project
 *
 * Copyright (C) 2001 George Staikos <staikos@kde.org>
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
 */

#ifndef _KSSLCERTCHAIN_H
#define _KSSLCERTCHAIN_H

#include <qlist.h>

class QString;
class QCString;
class KSSL;
class KSSLCertChainPrivate;
class QStringList;

#include <ksslcertificate.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


class KSSLCertChain {
friend class KSSL;
friend class KSSLPeerInfo;

public:
  KSSLCertChain();
  ~KSSLCertChain();

  bool isValid();

  KSSLCertChain *replicate();
  void setChain(void *stack_of_x509);
  void setChain(QList<KSSLCertificate>& chain);
  void setChain(QStringList chain);
  QList<KSSLCertificate> getChain();
  int depth();
  void *rawChain() { return _chain; }


private:
  KSSLCertChainPrivate *d;
  void *_chain;
};


#endif

