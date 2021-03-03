/*
 * klanguagebutton.cpp - Adds some methods for inserting languages.
 *
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.trolltech.com/
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

#define INCLUDE_MENUITEM_DEF
#include <qpopupmenu.h>
#include <qiconset.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>

#include "klanguagebutton.h"
#include "klanguagebutton.moc"

#include <kdebug.h>


static inline void checkInsertPos( QPopupMenu *popup, const QString & str,
                                   int &index )
{
  if ( index == -2 )
    index = popup->count();
  if ( index != -1 )
    return;

  int a = 0;
  int b = popup->count();
  while ( a <= b )
  {
    int w = ( a + b ) / 2;

    int id = popup->idAt( w );
    int j = str.compare( popup->text( id ) );

    if ( j > 0 )
      a = w + 1;
    else
      b = w - 1;
  }

  index = a; // it doesn't really matter ... a == b here.
}

static inline QPopupMenu * checkInsertIndex( QPopupMenu *popup,
                            const QStringList *tags, const QString &submenu )
{
  int pos = tags->findIndex( submenu );

  QPopupMenu *pi = 0;
  if ( pos != -1 )
  {
    QMenuItem *p = popup->findItem( pos );
    pi = p ? p->popup() : 0;
  }
  if ( !pi )
    pi = popup;

  return pi;
}


KLanguageButton::~KLanguageButton()
{
}

KLanguageButton::KLanguageButton( QWidget * parent, const char *name )
: QPushButton( parent, name ),
	m_popup( 0 ),
	m_oldPopup( 0 )
{
  m_tags = new QStringList;

  clear();
}

void KLanguageButton::insertItem( const QIconSet& icon, const QString &text,
                      const QString &tag, const QString &submenu, int index )
{
  QPopupMenu *pi = checkInsertIndex( m_popup, m_tags, submenu );
  checkInsertPos( pi, text, index );
  pi->insertItem( icon, text, count(), index );
  m_tags->append( tag );
}

void KLanguageButton::insertItem( const QString &text, const QString &tag,
                                  const QString &submenu, int index )
{
  QPopupMenu *pi = checkInsertIndex( m_popup, m_tags, submenu );
  checkInsertPos( pi, text, index );
  pi->insertItem( text, count(), index );
  m_tags->append( tag );
}

void KLanguageButton::insertSeparator( const QString &submenu, int index )
{
  QPopupMenu *pi = checkInsertIndex( m_popup, m_tags, submenu );
  pi->insertSeparator( index );
  m_tags->append( QString::null );
}

void KLanguageButton::insertSubmenu( const QString &text, const QString &tag,
                                     const QString &submenu, int index )
{
  QPopupMenu *pi = checkInsertIndex( m_popup, m_tags, submenu );
  QPopupMenu *p = new QPopupMenu( pi );
  checkInsertPos( pi, text, index );
  pi->insertItem( text, p, count(), index );
  m_tags->append( tag );
  connect( p, SIGNAL( activated( int ) ),
                        SLOT( slotActivated( int ) ) );
  connect( p, SIGNAL( highlighted( int ) ), this,
                        SIGNAL( highlighted( int ) ) );
}

void KLanguageButton::insertLanguage( const QString& path, const QString& name,
                        const QString& sub, const QString &submenu, int index )
{
  QString output = name + QString::fromLatin1( " (" ) + path +
                   QString::fromLatin1( ")" );
  QPixmap flag( locate( "locale", sub + path +
                QString::fromLatin1( "/flag.png" ) ) );
  insertItem( QIconSet( flag ), output, path, submenu, index );
}

void KLanguageButton::slotActivated( int index )
{
  // Update caption and iconset:
  if ( m_current == index )
    return;

  setCurrentItem( index );

  // Forward event from popup menu as if it was emitted from this widget:
  emit activated( index );
}

int KLanguageButton::count() const
{
  return m_tags->count();
}

void KLanguageButton::clear()
{
  m_tags->clear();

  delete m_oldPopup;
  m_oldPopup = m_popup;
  m_popup = new QPopupMenu( this );

  setPopup( m_popup );

  connect( m_popup, SIGNAL( activated( int ) ),
                        SLOT( slotActivated( int ) ) );
  connect( m_popup, SIGNAL( highlighted( int ) ),
                        SIGNAL( highlighted( int ) ) );

  setText( QString::null );
  setIconSet( QIconSet() );
}

/*void KLanguageButton::changeLanguage( const QString& name, int i )
{
  if ( i < 0 || i >= count() )
    return;
  QString output = name + QString::fromLatin1( " (" ) + tag( i ) +
                   QString::fromLatin1( ")" );
  changeItem( output, i );
}*/

bool KLanguageButton::containsTag( const QString &str ) const
{
  return m_tags->contains( str ) > 0;
}

QString KLanguageButton::currentTag() const
{
  return *m_tags->at( currentItem() );
}

QString KLanguageButton::tag( int i ) const
{
  if ( i < 0 || i >= count() )
  {
    kdDebug() << "KLanguageButton::tag(), unknown tag " << i << endl;
    return QString::null;
  }
  return *m_tags->at( i );
}

int KLanguageButton::currentItem() const
{
  return m_current;
}

void KLanguageButton::setCurrentItem( int i )
{
  if ( i < 0 || i >= count() )
    return;
  m_current = i;

  setText( m_popup->text( m_current ) );
  QIconSet *icon = m_popup->iconSet( m_current );
  if( icon )
    setIconSet( *icon );
  else
    setIconSet( QPixmap() );
}

void KLanguageButton::setCurrentItem( const QString &code )
{
  int i = m_tags->findIndex( code );
  if ( code.isNull() )
    i = 0;
  if ( i != -1 )
    setCurrentItem( i );
}

