 /*
  This file is or will be part of KDE desktop environment

  Copyright 1998 Sven Radej <sven@lisa.exp.univie.ac.at>

  It is licensed under GPL version 2.

  If it is part of KDE libraries than this file is licensed under
  LGPL version 2.
 */

#ifndef _KDIRWATCHTEST_H_
#define _KDIRWATCHTEST_H_

#include <stdlib.h>
#include <stdio.h>
#include <qobject.h>

#include "kdirwatch.h"
#include "kapp.h"

class myTest : public QObject 
{
   Q_OBJECT
public:
   myTest() { };
public slots:
   void dirty(const QString &a) { printf("Dirty: %s\n", a.ascii()); };
   void fileDirty(const QString& f) { printf("File-Dirty: %s\n", f.ascii()); }
   void deleted(const QString& f) { printf("Deleted: %s\n", f.ascii()); }
};

#endif
