/*
  Copyright (c) 2000,2001 Matthias Elter <elter@kde.org>
 
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

#ifndef __aboutwidget_h__
#define __aboutwidget_h__

#include <qwidget.h>
#include <qlistview.h>

class ModuleInfo;
class QPixmap;
class KPixmap;

class AboutWidget : public QWidget
{  
  Q_OBJECT    
  
public:   
  AboutWidget(QWidget *parent, const char *name=0, QListViewItem* category=0);

    /**
     * initialize the pixmaps and preprocess the PixmapEffect
     */
    static void initPixmaps();

    /**
     * Free the pixmaps again. They will be reloaded on the next use.
     * make sure to free them or you will lose QPixmaps to the X server
     * on exit!
     * This function is safe to call is init() hasn't been called or failed
     */
    static void freePixmaps();

    /**
     * Set a new category without creating a new AboutWidget if there is
     * one visible already (reduces flicker)
     */
    void setCategory( QListViewItem* category = 0 );

signals:
    void moduleSelected(const QString &);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    /**
     * Update the pixmap to be shown. Called from resizeEvent and from
     * setCategory.
     */
    void updatePixmap();
    
    // For performance reasons we make the pixmaps static so they won't
    // be reloaded every time again!
    static QPixmap *_part1;
    static QPixmap *_part2;
    static QPixmap *_part3;

    // Also for performance reasons we apply the KPixmapEffect only once
    static KPixmap *_part3Effect;
    
    QPixmap _buffer, _linkBuffer;
    QRect _linkArea;
    bool    _moduleList;
    QListViewItem* _category;
    struct ModuleLink;
    QList<ModuleLink> _moduleLinks;
    ModuleLink *_activeLink;
};

#endif
