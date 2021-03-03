/*
 * klangbutton.h - Button with language selection drop down menu.
 *                 Derived from the KLangCombo class by Hans Petter Bieker.
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *           (c) 2001      Martijn Klingens   <mklingens@yahoo.com>
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


#ifndef __KLANGBUTTON_H__
#define __KLANGBUTTON_H__

#include "kpushbutton.h"

/*
 * Extended QPushButton that shows a menu with submenu for language selection.
 * Essentially just a combo box with a 2-D dataset, but using a real
 * QComboBox will produce ugly results.
 *
 * Combined version of KTagCombo and KLanguageCombo but using a QPushButton
 * instead.
 */
class KLanguageButton : public QPushButton
{
  Q_OBJECT

public:
  KLanguageButton(QWidget *parent=0, const char *name=0);
  ~KLanguageButton();

  void insertItem( const QIconSet& icon, const QString &text,
                   const QString &tag, const QString &submenu = QString::null,
                   int index = -1 );
  void insertItem( const QString &text, const QString &tag,
                   const QString &submenu = QString::null, int index = -1 );
  void insertSeparator( const QString &submenu = QString::null,
                        int index = -1 );
  void insertSubmenu( const QString &text, const QString &tag,
                      const QString &submenu = QString::null, int index = -1);

  int count() const;
  void clear();
  
  void insertLanguage( const QString& path, const QString& name,
                       const QString& sub = QString::null,
                       const QString &submenu = QString::null, int index = -1);
  
  /*
   * Tag of the selected item
   */
  QString currentTag() const;
  QString tag( int i ) const;
  bool containsTag( const QString &str ) const;

  /*
   * Set the current item
   */
  int currentItem() const;
  void setCurrentItem( int i );
  void setCurrentItem( const QString &code );

signals:
  void activated( int index );
  void highlighted( int index );

private slots:
  void slotActivated( int );

private:
  // work space for the new class
  QStringList *m_tags;  
  QPopupMenu  *m_popup, *m_oldPopup;
  int         m_current;
};

#endif
