/* This file is part of the KDE project
 *
 * Copyright (C) 2000 George Staikos <staikos@kde.org>
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
 
#include "ksslconnectioninfo.h"

 
KSSLConnectionInfo::KSSLConnectionInfo() {
  clean(); 
}

 
KSSLConnectionInfo::~KSSLConnectionInfo() {
 
}


void KSSLConnectionInfo::clean() {
  m_iCipherUsedBits = 0;
  m_iCipherBits = 0;
  m_cipherName = "";
}


const QString& KSSLConnectionInfo::getCipherVersion() const {
  return m_cipherVersion;
}


const QString& KSSLConnectionInfo::getCipherDescription() const {
  return m_cipherDescription;
}


const QString& KSSLConnectionInfo::getCipher() const {
  return m_cipherName;
}


int KSSLConnectionInfo::getCipherUsedBits() const {
  return m_iCipherUsedBits;
}


int KSSLConnectionInfo::getCipherBits() const {
  return m_iCipherBits;
}



