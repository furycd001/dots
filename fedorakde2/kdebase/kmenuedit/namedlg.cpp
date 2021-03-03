/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <qlabel.h>
#include <qhbox.h>

#include <kglobal.h>
#include <klocale.h>
#include <klineedit.h>

#include "namedlg.h"

NameDialog::NameDialog( QWidget *parent, const char *name )
    : KDialogBase( parent, name, true, "text", Ok|Cancel, Ok, true )
{
    QHBox *page = makeHBoxMainWidget();
    (void)  new QLabel( i18n("Name:"), page);

    _lineedit = new KLineEdit(page);
    _lineedit->setMinimumWidth(fontMetrics().maxWidth()*20);
    _lineedit->setFocus();
}

QString NameDialog::text()
{
    if (_lineedit)
	return _lineedit->text();
    else
	return QString::null;
}

void NameDialog::setText(const QString& s)
{
    if(_lineedit)
	_lineedit->setText(s);
}
