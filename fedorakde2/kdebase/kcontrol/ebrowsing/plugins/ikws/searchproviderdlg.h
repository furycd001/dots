/*
 * Copyright (c) 2000 Malte Starostik <malte.starostik@t-online.de>
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


#ifndef __SEARCHPROVIDERDLG_H___
#define __SEARCHPROVIDERDLG_H___

#include <kdialog.h>

class KLineEdit;
class KComboBox;
class QPushButton;
class SearchProvider;

class SearchProviderDialog : public KDialog {
    Q_OBJECT

public:
    SearchProviderDialog(SearchProvider *provider, QWidget *parent = 0, const char *name = 0);

    SearchProvider *provider() { return m_provider; }

protected slots:
    void slotChanged();
    virtual void accept();

private:
    SearchProvider *m_provider;

    KLineEdit *m_name;
    KLineEdit *m_query;
    KLineEdit *m_keys;
    KComboBox *m_charset;

    QPushButton *m_ok;
    QPushButton *m_cancel;
};

#endif
