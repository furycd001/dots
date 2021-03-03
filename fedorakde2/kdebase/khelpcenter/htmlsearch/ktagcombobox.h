/*
 * ktagcombobox.h - A combobox with support for submenues, icons and tags
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __KTAGCOMBOBOX_H__
#define __KTAGCOMBOBOX_H__

#include <qcombobox.h>

/*
 * This class should be just like qcombobox, but it should be possible
 * to have have a QIconSet for each entry, and each entry should have a tag.
 *
 * It has also support for sub menues.
 */
class KTagComboBox : public QComboBox
{
  Q_OBJECT

public:
  KTagComboBox(QWidget *parent=0, const char *name=0);
  ~KTagComboBox();

  void insertItem(const QIconSet& icon, const QString &text, const QString &tag, const QString &submenu = QString::null, int index=-1 );
  void insertItem(const QString &text, const QString &tag, const QString &submenu = QString::null, int index=-1 );
  void insertSeparator(const QString &submenu = QString::null, int index=-1 );
  void insertSubmenu(const QString &text, const QString &tag, const QString &submenu = QString::null, int index=-1);

  int count() const;
  void clear();

  /*
   * Tag of the selected item
   */
  QString currentTag() const;
  QString tag ( int i ) const;
  bool containsTag (const QString &str ) const;

  /*
   * Set the current item
   */
  int currentItem() const;
  void setCurrentItem(int i);
  void setCurrentItem(const QString &code);

  // widget stuff
  virtual void setFont( const QFont & );

signals:
  void activated( int index );
  void highlighted( int index );

private slots:
  void internalActivate( int );
  void internalHighlight( int );

protected:
  void paintEvent( QPaintEvent * );
  void mousePressEvent( QMouseEvent * );
  void keyPressEvent( QKeyEvent *e );
  void popupMenu();

private:
  // work space for the new class
  QStringList *tags;  
  QPopupMenu *popup, *old_popup;
  int current;
};

#endif
