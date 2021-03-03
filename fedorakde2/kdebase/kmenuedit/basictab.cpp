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


#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qhbox.h>

#include <klocale.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kicondialog.h>
#include <kdesktopfile.h>
#include <kurlrequester.h>
#include <kfiledialog.h>
#include "khotkeys.h"


#include "basictab.h"
#include "basictab.moc"

BasicTab::BasicTab( QWidget *parent, const char *name )
  : QWidget(parent, name)
{
    QGridLayout *layout = new QGridLayout(this, 6, 2,
					  KDialog::marginHint(),
					  KDialog::spacingHint());

    // general group
    QGroupBox *general_group = new QGroupBox(this);
    QGridLayout *grid = new QGridLayout(general_group, 4, 2,
					KDialog::marginHint(),
					KDialog::spacingHint());

    // setup labels
    grid->addWidget(new QLabel(i18n("Name"), general_group), 0, 0);
    grid->addWidget(new QLabel(i18n("Comment"), general_group), 1, 0);
    grid->addWidget(new QLabel(i18n("Command"), general_group), 2, 0);
    grid->addWidget(new QLabel(i18n("Type"), general_group), 3, 0);

    // setup line inputs
    _nameEdit = new KLineEdit(general_group);
    _commentEdit = new KLineEdit(general_group);
    _execEdit = new KURLRequester(general_group);
    _typeEdit = new KLineEdit(general_group);

    // connect line inputs
    connect(_nameEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    connect(_commentEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    connect(_execEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    connect(_typeEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));

    // add line inputs to the grid
    grid->addWidget(_nameEdit, 0, 1);
    grid->addWidget(_commentEdit, 1, 1);
    grid->addWidget(_execEdit, 2, 1);
    grid->addWidget(_typeEdit, 3, 1);

    // add the general group to the main layout
    layout->addMultiCellWidget(general_group, 0, 0, 0, 1);

    // path group
    _path_group = new QGroupBox(this);
    QVBoxLayout *vbox = new QVBoxLayout(_path_group, KDialog::marginHint(),
					KDialog::spacingHint());

    QHBox *hbox = new QHBox(_path_group);
    (void) new QLabel(i18n("Work Path"), hbox);
    hbox->setSpacing(KDialog::spacingHint());

    _pathEdit = new KURLRequester(hbox);
    _pathEdit->fileDialog()->setMode(KFile::Directory | KFile::LocalOnly);
    connect(_pathEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    vbox->addWidget(hbox);
    layout->addWidget(_path_group, 1, 0);

    // setup icon button
    _iconButton = new KIconButton(this);
    _iconButton->setFixedSize(52,52);
    connect(_iconButton, SIGNAL(clicked()), SIGNAL(changed()));
    layout->addWidget(_iconButton, 1, 1);

    // terminal group
    _term_group = new QGroupBox(this);
    vbox = new QVBoxLayout(_term_group, KDialog::marginHint(),
			   KDialog::spacingHint());

    _terminalCB = new QCheckBox(i18n("Run in terminal"), _term_group);
    connect(_terminalCB, SIGNAL(clicked()), SLOT(termcb_clicked()));
    vbox->addWidget(_terminalCB);

    hbox = new QHBox(_term_group);
    (void) new QLabel(i18n("Terminal Options"), hbox);
    hbox->setSpacing(KDialog::spacingHint());
    _termOptEdit = new KLineEdit(hbox);
    connect(_termOptEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    vbox->addWidget(hbox);
    layout->addMultiCellWidget(_term_group, 2, 2, 0, 1);

    _termOptEdit->setEnabled(false);

    // uid group
    _uid_group = new QGroupBox(this);
    vbox = new QVBoxLayout(_uid_group, KDialog::marginHint(),
			   KDialog::spacingHint());

    _uidCB = new QCheckBox(i18n("Run as a different user"), _uid_group);
    connect(_uidCB, SIGNAL(clicked()), SLOT(uidcb_clicked()));
    vbox->addWidget(_uidCB);

    hbox = new QHBox(_uid_group);
    (void) new QLabel(i18n("Username"), hbox);
    hbox->setSpacing(KDialog::spacingHint());
    _uidEdit = new KLineEdit(hbox);
    connect(_uidEdit, SIGNAL(textChanged(const QString&)),
	    SLOT(slotChanged(const QString&)));
    vbox->addWidget(hbox);
    layout->addMultiCellWidget(_uid_group, 3, 3, 0, 1);

    _uidEdit->setEnabled(false);

    layout->setRowStretch(0, 2);

    // key binding group
    general_group_keybind = new QGroupBox(this);
    layout->addMultiCellWidget( general_group_keybind, 4, 4, 0, 1 );
    // dummy widget in order to make it look a bit better
    layout->addWidget( new QWidget(this), 5, 0 );
    layout->setRowStretch( 1, 4 );
    QGridLayout *grid_keybind = new QGridLayout(general_group_keybind, 3, 1,
					KDialog::marginHint(),
					KDialog::spacingHint());

    grid_keybind->addWidget(new QLabel(i18n("Current key"), general_group_keybind), 0, 0);
    _keyEdit = new KLineEdit(general_group_keybind);
    _keyEdit->setReadOnly( true );
    _keyEdit->setText( "" );
    QPushButton* _keyButton = new QPushButton( i18n( "Change" ),
                                               general_group_keybind );
    connect( _keyButton, SIGNAL( clicked()), this, SLOT( keyButtonPressed()));
    grid_keybind->addWidget(_keyEdit, 0, 1);
    grid_keybind->addWidget(_keyButton, 0, 2 );
    if( !KHotKeys::present())
        setEnabled( false ); // disable the whole tab if no KHotKeys found
    _khotkeysNeedsSave = false;


    //disable all group at the begining.
    //because there is not file selected.
    _nameEdit->setEnabled(false);
    _commentEdit->setEnabled(false);
    _execEdit->setEnabled(false);
    _typeEdit->setEnabled(false);
    _path_group->setEnabled(false);
    _term_group->setEnabled(false);
    _uid_group->setEnabled(false);
    // key binding part
    general_group_keybind->setEnabled( false );

    connect( this, SIGNAL( changed()), SLOT( slotChanged()));
}

void BasicTab::setDesktopFile(const QString& desktopFile)
{
    _desktopFile = desktopFile;
    // key binding part
    _khotkeysNeedsSave = false;

    KDesktopFile df(desktopFile);

    _nameEdit->setText(df.readName());
    _commentEdit->setText(df.readComment());
    _iconButton->setIcon(df.readIcon());

    // is desktopFile a .desktop file?
    bool isDF = desktopFile.find(".desktop") > 0;

    // set only basic attributes if it is not a .desktop file
    _nameEdit->setEnabled(true);
    _commentEdit->setEnabled(true);
    _execEdit->setEnabled(isDF);
    _typeEdit->setEnabled(isDF);
    _path_group->setEnabled(isDF);
    _term_group->setEnabled(isDF);
    _uid_group->setEnabled(isDF);

    // key binding part
    if( desktopFile.find(".desktop") > 0 )
    {
        if( KHotKeys::present())
        {
            general_group_keybind->setEnabled( true );
            _keyEdit->setText( KHotKeys::getMenuEntryShortcut(
                _desktopFile ));
        }
    }
    else
    {
        general_group_keybind->setEnabled( false ); // not a menu entry - no shortcut
        _keyEdit->setText("");
    }
   // clean all disabled fields and return if it is not a .desktop file
    if (!isDF) {
          _execEdit->lineEdit()->setText("");
	  _typeEdit->setText("");
	  _pathEdit->lineEdit()->setText("");
	  _termOptEdit->setText("");
	  _uidEdit->setText("");
	  _terminalCB->setChecked(false);
	  _uidCB->setChecked(false);
          return;
    }

    _execEdit->lineEdit()->setText(df.readEntry("Exec"));
    _typeEdit->setText(df.readType());
    _pathEdit->lineEdit()->setText(df.readPath());
    _termOptEdit->setText(df.readEntry("TerminalOptions"));
    _uidEdit->setText(df.readEntry("X-KDE-Username"));

    if(df.readNumEntry("Terminal", 0) == 1)
	_terminalCB->setChecked(true);
    else
	_terminalCB->setChecked(false);

    _uidCB->setChecked(df.readBoolEntry("X-KDE-SubstituteUID", false));

    _termOptEdit->setEnabled(_terminalCB->isChecked());
    _uidEdit->setEnabled(_uidCB->isChecked());
}

void BasicTab::apply( bool desktopFileNeedsSave )
{
    // key binding part
    if( KHotKeys::present() && _khotkeysNeedsSave )
        {
        KHotKeys::changeMenuEntryShortcut( _desktopFile, _keyEdit->text());
        }
    _khotkeysNeedsSave = false;

    if( !desktopFileNeedsSave )
        return;
    QString local = locateLocal("apps", _desktopFile);

    KDesktopFile df(local);
    df.writeEntry("Name", _nameEdit->text());
    df.writeEntry("Comment", _commentEdit->text());
    df.writeEntry("Icon", _iconButton->icon());

    if(_desktopFile.find(".desktop") < 0)
	{
	    df.sync();
	    return;
	}

    df.writeEntry("Exec", _execEdit->lineEdit()->text());
    df.writeEntry("Type", _typeEdit->text());
    df.writeEntry("Path", _pathEdit->lineEdit()->text());

    if (_terminalCB->isChecked())
	df.writeEntry("Terminal", 1);
    else
	df.writeEntry("Terminal", 0);

    df.writeEntry("TerminalOptions", _termOptEdit->text());
    df.writeEntry("X-KDE-SubstituteUID", _uidCB->isChecked());
    df.writeEntry("X-KDE-Username", _uidEdit->text());

    df.sync();
}

void BasicTab::reset()
{
    if(_desktopFile != "")
	setDesktopFile(_desktopFile);

    // key binding part
    _khotkeysNeedsSave = false;
}

void BasicTab::slotChanged(const QString&)
{
    emit changed();
}

void BasicTab::slotChanged()
{
    emit changed( true );
}

void BasicTab::termcb_clicked()
{
    _termOptEdit->setEnabled(_terminalCB->isChecked());
    emit changed();
}

void BasicTab::uidcb_clicked()
{
    _uidEdit->setEnabled(_uidCB->isChecked());
    emit changed();
}

// key bindign method
void BasicTab::keyButtonPressed()
{
    if( !KHotKeys::present())
        return;
    QString new_shortcut = KHotKeys::editMenuEntryShortcut( _desktopFile,
        _keyEdit->text(), false );
    if( new_shortcut == _keyEdit->text())
        return;
    _keyEdit->setText( new_shortcut );
    emit changed( false );
    _khotkeysNeedsSave = true;
}
