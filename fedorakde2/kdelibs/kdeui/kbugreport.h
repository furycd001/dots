/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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
#ifndef _KBUGREPORT_H__
#define _KBUGREPORT_H__

#include <kdialogbase.h>

class QMultiLineEdit;
class QLineEdit;
class QHButtonGroup;
class KProcess;
class KAboutData;
class KBugReportPrivate;

/**
 * A dialog box for sending bug reports.
 * All the information needed by the dialog box
 * (program name, version, bug-report address, etc.)
 * comes from the @ref KAboutData class.
 * Make sure you create an instance of KAboutData and pass it
 * to @ref KCmdLineArgs.
 *
 * @short A dialog box for sending bug reports.
 * @author David Faure <faure@kde.org>
 */
class KBugReport : public KDialogBase
{
  Q_OBJECT
public:
  /**
   * Creates a bug-report dialog.
   * Note that you shouldn't have to do this manually,
   * since @ref KHelpMenu takes care of the menu item
   * for "Report Bug..." and of creating a KBugReport dialog.
   */
  KBugReport( QWidget * parent = 0L, bool modal=true, const KAboutData *aboutData = 0L );
  /**
   * Destructor
   */
  virtual ~KBugReport();

protected slots:
  /**
   * "Configure email" has been clicked - this calls kcmshell System/email
   */
  virtual void slotConfigureEmail();
  /**
   * Sets the "From" field from the e-mail configuration
   * Called at creation time, but also after "Configure email" is closed.
   */
  virtual void slotSetFrom();
  /**
   * The URL-Label "http://bugs.kde.org/" was clicked.
   */
  virtual void slotUrlClicked(const QString &);
  /**
   * OK has been clicked
   */
  virtual void slotOk( void );
  /**
   * Cancel has been clicked
   */
  virtual void slotCancel();

  // app combo changed
  void appChanged(int);
  void updateURL();

protected:
  QString text();
  bool sendBugReport();

  KProcess * m_process;
  const KAboutData * m_aboutData;

  QMultiLineEdit * m_lineedit;
  QLineEdit * m_subject;
  QLabel * m_from;
  QLabel * m_version;
  QString m_strVersion;
  QHButtonGroup * m_bgSeverity;
  QPushButton * m_configureEmail;

private:
  KBugReportPrivate *d;
};

#endif

