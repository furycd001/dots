/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)

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
#ifndef _KDEBUGDIALOG
#define _KDEBUGDIALOG

#include <kabstractdebugdialog.h>

class QLineEdit;
class QComboBox;
class QLabel;
class QGroupBox;
class QCheckBox;
class QPushButton;

class KConfig;

/**
 * Control debug/warning/error/fatal output of KDE applications
 *
 * This dialog allows control of debugging output for all KDE apps.
 *
 * @author Kalle Dalheimer (kalle@kde.org)
 * @version $Id: kdebugdialog.h,v 1.6 2000/07/12 13:42:43 kalle Exp $
 */
class KDebugDialog : public KAbstractDebugDialog
{
  Q_OBJECT

public:
  KDebugDialog( QStringList areaList, QWidget *parent=0, const char *name=0, bool modal=true );
  virtual ~KDebugDialog();

  void save();

protected slots:
  void slotDebugAreaChanged( const QString & );
  void slotDestinationChanged(int);

private:
  QComboBox* pDebugAreas;
  QGroupBox* pInfoGroup;
  QLabel* pInfoLabel1;
  QComboBox* pInfoCombo;
  QLabel* pInfoLabel2;
  QLineEdit* pInfoFile;
  QLabel* pInfoLabel3;
  QLineEdit* pInfoShow;
  QGroupBox* pWarnGroup;
  QLabel* pWarnLabel1;
  QComboBox* pWarnCombo;
  QLabel* pWarnLabel2;
  QLineEdit* pWarnFile;
  QLabel* pWarnLabel3;
  QLineEdit* pWarnShow;
  QGroupBox* pErrorGroup;
  QLabel* pErrorLabel1;
  QComboBox* pErrorCombo;
  QLabel* pErrorLabel2;
  QLineEdit* pErrorFile;
  QLabel* pErrorLabel3;
  QLineEdit* pErrorShow;
  QGroupBox* pFatalGroup;
  QLabel* pFatalLabel1;
  QComboBox* pFatalCombo;
  QLabel* pFatalLabel2;
  QLineEdit* pFatalFile;
  QLabel* pFatalLabel3;
  QLineEdit* pFatalShow;

  QCheckBox* pAbortFatal;

private:
  // Disallow assignment and copy-construction
  KDebugDialog( const KDebugDialog& );
  KDebugDialog& operator= ( const KDebugDialog& );
};

#endif
