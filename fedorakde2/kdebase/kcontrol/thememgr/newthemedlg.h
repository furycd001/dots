/*
 * newthemedlg.h
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
#ifndef NEW_THEME_DLG_H
#define NEW_THEME_DLG_H

#include <kdialogbase.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qimage.h>

class QLabel;
class QGridLayout;

class NewThemeDlg: public KDialogBase
{
  Q_OBJECT
public:
  NewThemeDlg(QWidget *parent);
  virtual ~NewThemeDlg();

  QString fileName(void) const { return mEdtFilename->text(); }
  QString themeName(void) const { return mEdtName->text(); }
  QString author(void) const { return mEdtAuthor->text(); }
  QString email(void) const { return mEdtEmail->text(); }
  QString homepage(void) const { return mEdtHomepage->text(); }
  QImage preview() const { return mPreview; }

protected:
  virtual QLineEdit* newLine(const QString& lbl, int cols);
  virtual void setValues(void);

protected slots:
  void slotSnapshot();

protected:
  int mGridRow;
  QLineEdit *mEdtFilename;
  QLineEdit *mEdtName;
  QLineEdit *mEdtAuthor;
  QLineEdit *mEdtEmail;
  QLineEdit *mEdtHomepage;
  QLabel *mPreviewLabel;
  QImage mPreview;
  QGridLayout* mGrid;
};

class SnapshotDlg: public KDialogBase
{
  Q_OBJECT
public:
  SnapshotDlg(QWidget *parent);
protected slots:
  void slotCountdown();

protected:
  QTimer mTimer;
  QLabel *mLabel;
  int mSeconds;
};


#endif /*NEW_THEME_DLG_H*/
