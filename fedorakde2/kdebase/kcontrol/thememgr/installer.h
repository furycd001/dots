/*
 * installer.h
 *
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
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
#ifndef INSTALLER_H
#define INSTALLER_H

#include <qpoint.h>
#include <kcmodule.h>
#include <klistbox.h>
#include <kurl.h>
#include <qmap.h>

class QGridLayout;
class QPushButton;
class QLabel;
class QMultiLineEdit;
class ThemeListBox;

class Installer : public KCModule
{
  Q_OBJECT
public:
  Installer(QWidget *parent=0, const char *aName=0, bool aInit=FALSE);
  ~Installer();

  virtual void load();
  virtual void save();
  virtual void defaults();

  /** Find item in listbox. Returns item index or -1 if not found */
  virtual int findItem(const QString text) const;

protected slots:
  virtual void slotAdd();
  virtual void slotSaveAs();
  virtual void slotCreate();
  virtual void slotRemove();
  virtual void slotThemeChanged();
  virtual void slotSetTheme(int);
  void slotFilesDropped(const KURL::List &urls);

protected:
  /** Scan Themes directory for available theme packages */
  virtual void readThemesList(void);
  /** add a theme to the list, returns the list index */
  int addTheme(const QString &path);
  void addNewTheme(const KURL &srcURL);

private:
  bool mGui;
  QGridLayout *mGrid;
  ThemeListBox *mThemesList;
  QPushButton *mBtnCreate, *mBtnSaveAs, *mBtnAdd, *mBtnRemove;
  QMultiLineEdit *mText;
  QLabel *mPreview;
};

class ThemeListBox: public KListBox
{
  Q_OBJECT
public:
  ThemeListBox(QWidget *parent);
    QMap<QString, QString> text2path;

signals:
  void filesDropped(const KURL::List &urls);

protected:
  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void mouseMoveEvent(QMouseEvent *e);

protected slots:
  void slotMouseButtonPressed(int button, QListBoxItem *item, const QPoint &p);

private:
  QString mDragFile;
  QPoint mOldPos;

};


#endif /*INSTALLER_H*/

