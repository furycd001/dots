/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kdirnotify_h__
#define __kdirnotify_h__

#include <dcopobject.h>
#include <kurl.h>

/**
 * An abstract class that receives notifications of added and removed files
 * in any directory, local or remote.
 * The information comes from the konqueror/kdesktop instance where the
 * operation was done, and can interest KDirListers, bookmark handlers, etc.
 */
class KDirNotify : public DCOPObject
{
   K_DCOP
protected:
  KDirNotify();
  virtual ~KDirNotify() {}

public:
k_dcop:
  /**
   * Notify that files have been added in @p directory
   * Note: this is ASYNC so that it can be used with a broadcast
   */
  virtual ASYNC FilesAdded( const KURL & directory ) = 0;

  /**
   * Notify that files have been deleted.
   * Note: this is ASYNC so that it can be used with a broadcast
   */
  virtual ASYNC FilesRemoved( const KURL::List & fileList ) = 0;

  /**
   * Notify that files have been changed.
   * At the moment, this is only used for new icon, but it could be
   * used for size etc. as well.
   * Note: this is ASYNC so that it can be used with a broadcast
   */
  virtual ASYNC FilesChanged( const KURL::List & fileList ) = 0;

  /**
   * Notify that a file has been renamed.
   * Note: this is ASYNC so that it can be used with a broadcast
   */
  virtual ASYNC FileRenamed( const KURL &src, const KURL &dst );

private:
  // @internal
  static int s_serial;
};

#endif
