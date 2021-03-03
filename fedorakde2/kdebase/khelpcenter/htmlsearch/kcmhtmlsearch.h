/**
 * kcmhtmlsearch.h
 *
 * Copyright (c) 2000 Matthias Hölzer-Klüpfel <hoelzer@kde.org>
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

#ifndef __kcmhtmlsearch_h__
#define __kcmhtmlsearch_h__


#include <kcmodule.h>


class QLineEdit;
class QCheckBox;
class QPushButton;
class KListBox;
class KProcess;
class KLanguageCombo;
class KURLRequester;

class KHTMLSearchConfig : public KCModule
{
  Q_OBJECT

public:

  KHTMLSearchConfig(QWidget *parent = 0L, const char *name = 0L);
  virtual ~KHTMLSearchConfig();
  
  void load();
  void save();
  void defaults();

  QString quickHelp() const;
  
  int buttons();

  
protected slots:

  void configChanged();
  void addClicked(); 
  void delClicked();
  void pathSelected(const QString &);
  void urlClicked(const QString&);
  void generateIndex();

  void indexTerminated(KProcess *proc);

      
private:

  void checkButtons();
  void loadLanguages();

  KURLRequester *htdigBin, *htsearchBin, *htmergeBin;
  QCheckBox *indexKDE, *indexMan, *indexInfo;
  QPushButton *addButton, *delButton, *runButton;
  KListBox *searchPaths;
  KLanguageCombo *language;

  KProcess *indexProc;

};

#endif
