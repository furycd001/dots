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

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include "searchproviderdlg.h"
#include "searchprovider.h"

SearchProviderDialog::SearchProviderDialog(SearchProvider *provider, QWidget *parent, const char *name)
    : KDialog(parent, name, true),
      m_provider(provider)
{
    // GUI init
    QGridLayout *layout = new QGridLayout(this, 9, 2, KDialog::marginHint(), KDialog::spacingHint());

    QLabel *label = new QLabel(i18n("Search &Provider Name:"), this);
    layout->addMultiCellWidget(label, 0, 0, 0, 1);
    label->setBuddy(m_name = new KLineEdit(this));
    layout->addMultiCellWidget(m_name, 1, 1, 0, 1);
    QString whatsThis = i18n("Enter the human readable name of the search provider here.");
    QWhatsThis::add(label, whatsThis);
    QWhatsThis::add(m_name, whatsThis);

    label = new QLabel(i18n("Search &URI:"), this);
    layout->addMultiCellWidget(label, 2, 2, 0, 1);
    label->setBuddy(m_query = new KLineEdit(this));
    m_query->setMinimumWidth(kapp->fontMetrics().width('x') * 60);
    layout->addMultiCellWidget(m_query, 3, 3, 0, 1);
    whatsThis = i18n("Enter the URI that is used to do a search on the search engine here. The text to be searched for can be specified as \\1.");
    QWhatsThis::add(label, whatsThis);
    QWhatsThis::add(m_query, whatsThis);

    label = new QLabel(i18n("UR&I Shortcuts:"), this);
    layout->addMultiCellWidget(label, 4, 4, 0, 1);
    label->setBuddy(m_keys = new KLineEdit(this));
    layout->addMultiCellWidget(m_keys, 5, 5, 0, 1);
    whatsThis = i18n("The shortcuts entered here can be used as a pseudo-URI scheme in KDE. For example, the shortcut <em>av</em> can be used as in <em>av</em>:<em>my search</em>.");
    QWhatsThis::add(label, whatsThis);
    QWhatsThis::add(m_keys, whatsThis);

    label = new QLabel(i18n("&Charset:"), this);
    layout->addMultiCellWidget(label, 6, 6, 0, 1);
    label->setBuddy(m_charset = new KComboBox(this));
    layout->addMultiCellWidget(m_charset, 7, 7, 0, 1);
    whatsThis = i18n("Select the character set that will be used to encode your search query.");
    QWhatsThis::add(label, whatsThis);
    QWhatsThis::add(m_charset, whatsThis);

    QHBoxLayout *Layout2 = new QHBoxLayout;
    Layout2->setSpacing( KDialog::spacingHint() );
    Layout2->setMargin( KDialog::marginHint() );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_2 );

    m_ok = new QPushButton(i18n("&OK"), this);
    Layout2->addWidget(m_ok);

    m_cancel = new QPushButton(i18n("&Cancel"), this);
    Layout2->addWidget(m_cancel);

    layout->addLayout( Layout2, 8, 0 );

    connect(m_name, SIGNAL(textChanged(const QString &)), SLOT(slotChanged()));
    connect(m_query, SIGNAL(textChanged(const QString &)), SLOT(slotChanged()));
    connect(m_keys, SIGNAL(textChanged(const QString &)), SLOT(slotChanged()));
    connect(m_ok, SIGNAL(clicked()), SLOT(accept()));
    connect(m_cancel, SIGNAL(clicked()), SLOT(reject()));

    // Data init
    QStringList charsets = KGlobal::charsets()->availableEncodingNames();
    charsets.prepend(i18n("Default"));
    m_charset->insertStringList(charsets);
    
    if (m_provider)
    {
        setPlainCaption(i18n("Modify Search Provider"));
        m_name->setText(m_provider->name());
        m_query->setText(m_provider->query());
        m_keys->setText(m_provider->keys().join(","));
        m_charset->setCurrentItem(m_provider->charset().isEmpty() ? 0 : charsets.findIndex(m_provider->charset()));
        m_name->setEnabled(false);
        m_query->setFocus();
    }
    else
    {
        setPlainCaption(i18n("New Search Provider"));
        m_name->setFocus();
    }
}

void SearchProviderDialog::slotChanged()
{
    m_ok->setEnabled(!(m_name->text().isEmpty()
                       || m_keys->text().isEmpty()
                       || m_query->text().isEmpty()));
    m_ok->setDefault(true);
}

void SearchProviderDialog::accept()
{
    if ((m_query->text().find("\\1") == -1)
        && KMessageBox::warningContinueCancel(0,
            i18n("The URI does not contain a \\1 placeholder for the user query.\n"
                 "This means that the same page is always going to be visited, \n"
                 "regardless of what the user types..."),
            QString::null, i18n("Keep It")) == KMessageBox::Cancel)
        return;

    if (!m_provider)
        m_provider = new SearchProvider;
    m_provider->setName(m_name->text().stripWhiteSpace());
    m_provider->setQuery(m_query->text().stripWhiteSpace());
    m_provider->setKeys(QStringList::split(",", m_keys->text().stripWhiteSpace()));
    m_provider->setCharset(m_charset->currentItem() ? m_charset->currentText() : QString::null);
    KDialog::accept();
}

#include "searchproviderdlg.moc"
