/*
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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
#include <qtabbar.h>
#include <qlistbox.h>
#include <qtabwidget.h>
#include <qpushbutton.h>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#include <kdb/plugin.h>
#include <kdb/dbengine.h>
#include <kdb/connectiondlg.h>

#include <connectionconfig.h>

#include <connectionconfig.moc>

using namespace KDB;

ConnectionConfig::ConnectionConfig( QWidget* parent, const char* name )
  : QWidget( parent, name )
{
  QGridLayout* layout = new QGridLayout( this, 4, 2, 15, 7 );

  m_connectionList = new DBListView( this, 0L, true, DBListView::Manual );
  layout->addMultiCellWidget( m_connectionList, 0, 4, 0, 0 );
  connect( m_connectionList, SIGNAL( currentChanged( QListViewItem* ) ), this, SLOT( slotItemSelected( QListViewItem* ) ) );

  m_btnAdd = new QPushButton( i18n( "Add..." ), this );
  m_btnAdd->setEnabled( false );
  layout->addWidget( m_btnAdd, 0, 1 );
  connect( m_btnAdd, SIGNAL( clicked() ), SLOT( slotAdd() ) );

  m_btnEdit = new QPushButton( i18n( "Edit..." ), this );
  m_btnEdit->setEnabled( false );
  layout->addWidget( m_btnEdit, 1, 1 );
  connect( m_btnEdit, SIGNAL( clicked() ), SLOT( slotEdit() ) );

  m_btnRemove = new QPushButton( i18n( "Remove" ), this );
  m_btnRemove->setEnabled( false );
  layout->addWidget( m_btnRemove, 2, 1 );
  connect( m_btnRemove, SIGNAL( clicked() ), SLOT( slotRemove() ) );

  layout->setRowStretch( 3, 1 );
  layout->setColStretch( 0, 1 );

  for( PluginIterator it = DBENGINE->beginPlugins(); it != 0L ; ++it )
    m_connectionList->addPlugin( *it );
}

void ConnectionConfig::load()
{
}

void ConnectionConfig::save()
{
}

void ConnectionConfig::defaults()
{
}

void ConnectionConfig::slotAdd()
{
  kdDebug(20020) << "ConnectionConfig::slotAdd" << endl;

  Connection *connection = ConnectionDialog::createConnection( this );

  if( connection )
  {
    m_connectionList->addConnection( connection );
    emit changed();
  }
}

void ConnectionConfig::slotEdit()
{
  kdDebug(20020) << "ConnectionConfig::slotEdit" << endl;

  //if( ConnectionDialog::editConnection( connection, this ) )
    emit changed();
}

void ConnectionConfig::slotRemove()
{
  kdDebug(20020) << "ConnectionConfig::slotRemove" << endl;

  QListViewItem *item = m_connectionList->currentItem();

  if( item )
  {
    //m_connectionList->removeItem( id );
    emit changed();
  }
}

void ConnectionConfig::slotItemSelected( QListViewItem *i )
{
    DBListViewItem *item = static_cast<DBListViewItem *>(i);

    if( item->itemObject()->inherits("KDB::Plugin") )
    {
        m_btnAdd->setEnabled( true );
        m_btnEdit->setEnabled( false );
        m_btnRemove->setEnabled( false );
    }
    else
    {
        m_btnAdd->setEnabled( false );
        m_btnEdit->setEnabled( true );
        m_btnRemove->setEnabled( true );
    }
}
