// -*- c++ -*-
/* This file is part of the KDE libraries
    Copyright (C) 1997, 1998 Richard Moore <rich@kde.org>
                  1998 Stephan Kulow <coolo@kde.org>
                  1998 Daniel Grana <grana@ie.iwi.unibe.ch>
    
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

#ifndef KFILEDIALOGCONF_H
#define KFILEDIALOGCONF_H

class QRadioButton;
class QCheckBox;


#include <kdialogbase.h>

/**
 * KFileDialog configuration widget.
 *
 * @short This widget allows users to configure KFileDialog
 * @author Richard Moore (rich@kde.org)
 * @version $Id$
 */
class KFileDialogConfigure : public QWidget
{
  Q_OBJECT

  public:
    KFileDialogConfigure(QWidget *parent= 0, const char *name= 0);

  public slots:
    void saveConfiguration();

  protected:  
    QCheckBox *myShowStatusLine;

  private:
    class KFileDialogConfigurePrivate;
    KFileDialogConfigurePrivate *d;
};


class KFileDialogConfigureDlg : public KDialogBase
{
  Q_OBJECT

  public:
    KFileDialogConfigureDlg( QWidget *parent, const char *name );

  private:
    void setupConfigPage( const QString &title );
    void setupAboutPage( const QString &title );

};

#endif // KFILEDIALOGCONF_H
