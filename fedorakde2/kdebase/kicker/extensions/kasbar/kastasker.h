// -*- c++ -*-

/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef KASTASKER_H
#define KASTASKER_H

#include "kasbar.h"

class KasTaskItem;
class KasStartupItem;
class TaskManager;
class Task;
class Startup;
class KPixmap;
class KasGroupItem;

/**
 * A KasBar that provides a taskbar using the TaskManager API.
 *
 * @version $Id: kastasker.h,v 1.9 2001/05/22 01:22:43 rich Exp $
 * @author Richard Moore, rich@kde.org
 */
class KasTasker : public KasBar
{
   Q_OBJECT

public:
   /**
    * Create a KasTasker widget.
    */
   KasTasker( Orientation o, QWidget* parent = 0, const char* name = 0 );

   /**
    * Create a KasTasker widget that is slaved to another KasTasker. The
    * created widget will inherit the settings of the parent, but will
    * not connect to the signals of the TaskManager.
    */
   KasTasker( Orientation o, KasTasker *master, QWidget* parent = 0, const char* name = 0 );
   virtual ~KasTasker();

   KasTaskItem *findItem( Task * );
   KasStartupItem *findItem( Startup *s );

   QBitmap *minIcon();
   QBitmap *maxIcon();
   QBitmap *shadeIcon();
   QPixmap *microShadeIcon();
   QPixmap *microMaxIcon();
   QPixmap *microMinIcon();

   bool thumbnailsEnabled() const { return enableThumbs_; }
   double thumbnailSize() const { return thumbnailSize_; }
   bool notifierEnabled() const { return enableNotifier_; }
   bool showModified() const { return showModified_; }
   bool showAllWindows() const { return showAllWindows_; }
   int thumbnailUpdateDelay() const { return thumbUpdateDelay_; }
   bool groupWindows() const { return groupWindows_; }

   // Internal grouping stuff
   KasGroupItem *convertToGroup( Task *t );
   void moveToMain( KasGroupItem *gi, Task *t );
   KasItem *maybeAddToGroup( Task *t );

public slots:
   void addTask( Task * );
   void removeTask( Task * );

   void addStartup( Startup * );
   void removeStartup( Startup * );

   void refreshAll();
   void refreshIconGeometry();

   void setNotifierEnabled( bool enable );
   void setThumbnailSize( double size );
   void setThumbnailSize( int percent );
   void setThumbnailsEnabled( bool enable );
   void setShowModified( bool enable );
   void setShowAllWindows( bool enable );
   void setThumbnailUpdateDelay( int secs );
   void setGroupWindows( bool enable );
 
private:
   KasTasker *master_;
   TaskManager *manager;
   QBitmap *minPix;
   QBitmap *maxPix;
   QBitmap *shadePix;
   QPixmap *microShadePix;
   QPixmap *microMaxPix;
   QPixmap *microMinPix;
   bool passive_;
   bool enableThumbs_;
   double thumbnailSize_;
   bool enableNotifier_;
   bool showModified_;
   bool showAllWindows_;
   int thumbUpdateDelay_;
   bool groupWindows_;
};

#endif // KASTASKER_H

