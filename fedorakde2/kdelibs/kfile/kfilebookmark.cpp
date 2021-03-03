/* This file is part of the KDE libraries
    Copyright (C) 1996, 1997, 1998 Martin R. Jones <mjones@kde.org>

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
//-----------------------------------------------------------------------------
//
// KDE HTML Bookmarks
//
// (c) Martin R. Jones 1996
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <qfile.h>
#include "kfilebookmark.h"

#include <klocale.h>
#include <kapp.h>

/* FIXME 
 * kfilebookmarks should be rewritten to use Qt, and not things like
 * strcasecmp
 */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

template class QList<KFileBookmark>;

//-----------------------------------------------------------------------------

KFileBookmark::KFileBookmark()
{
	children.setAutoDelete( true );
	type = URL;
}

KFileBookmark::KFileBookmark( const QString& _text, const QString& _url )
{
	children.setAutoDelete( true );
	text = _text;
	url = _url;
	type = URL;
}

void KFileBookmark::clear()
{
	KFileBookmark *bm;

	for ( bm = getChildren().first(); bm != NULL; bm = getChildren().next() )
	{
		bm->clear();
	}

	children.clear();
}

//-----------------------------------------------------------------------------

KFileBookmarkManager::KFileBookmarkManager()
{
}

void KFileBookmarkManager::read( const QString& filename )
{
	QFile file( filename );

	// rich
	myFilename= filename;

	if ( !file.open( IO_ReadOnly ) )
		return;

	root.clear();

	QString text;
	char buffer[256];

	do
	{
		file.readLine( buffer, 256 );
		text += QString::fromLatin1(buffer);
	}
	while ( !file.atEnd() );

	BookmarkTokenizer *ht = new BookmarkTokenizer();

	ht->begin();
	ht->write( text );
	ht->end();

	const char *str;

	while ( ht->hasMoreTokens() )
	{
		str = ht->nextToken();

		// Every tag starts with an escape character
		if ( str[0] == TAG_ESCAPE )
		{
			str++;
	
			if ( strncasecmp( str, "<title>", 7 ) == 0 )
			{
				QString t = QString::fromLatin1("");
				bool bend = false;

				do
				{
					if ( !ht->hasMoreTokens() )
						bend = true;
					else
					{
						str = ht->nextToken();
						if ( str[0] == TAG_ESCAPE &&
								strncasecmp( str + 1, "</title>", 8 ) == 0 )
							bend = true;
						else
							t += QString::fromLatin1(str);
					}
				}
				while ( !bend );

				title = t;
			}
			else if ( strncasecmp( str, "<DL>", 4 ) == 0 )
			{
				parse( ht, &root, "" );
			}
		}
    }

	delete ht;

	emit changed();
}

// parser based on HTML widget parser
//
const char *KFileBookmarkManager::parse( BookmarkTokenizer *ht, KFileBookmark *parent,
	const char *_end )
{
	KFileBookmark *bm = parent;
	QString text;
	const char *str;

	parent->setType( KFileBookmark::Folder );

	while ( ht->hasMoreTokens() )
	{
		str = ht->nextToken();

		if (str[0] == TAG_ESCAPE )
		{
			str++;

			if ( _end[0] != 0 && strcasecmp( str, _end ) == 0 )
			{
				return str;
			}
			else if ( strncasecmp( str, "<dl>", 4 ) == 0 )
			{
				parse( ht, bm, "</dl>" );
			}
			else if ( strncasecmp( str, "<dt>", 4 ) == 0 )
			{
				bm = new KFileBookmark;
				parent->getChildren().append( bm );
			}
			else if ( strncasecmp( str, "<a ", 3 ) == 0 )
			{
				const char *p = str + 3;

				while ( *p != '>' )
				{
					if ( strncasecmp( p, "href=", 5 ) == 0 )
					{
						p += 5;

						text = "";
						bool quoted = false;
						while ( ( *p != ' ' && *p != '>' ) || quoted )
						{
							if ( *p == '\"' )
								quoted = !quoted;
							else
								text += *p;
							p++;
						}

						bm->setURL( text );
			
						if ( *p == ' ' )
							p++;
					}
					else
					{
						const char *p2 = strchr( p, ' ' );
						if ( p2 == 0L )
							p2 = strchr( p, '>');
						else
							p2++;
						p = p2;
					}
				}

				text = "";
			}
			else if ( strncasecmp( str, "<H3", 3 ) == 0 )
			{
				text = "";
			}
			else if ( strncasecmp( str, "</H3>", 5 ) == 0 ||
						strncasecmp( str, "</a>", 4 ) == 0 )
			{
				bm->setText( text );
			}
		}
		else if ( str[0] )
		{
			text += QString::fromLatin1(str);
		}
	}

	return NULL;
}

// write bookmarks file
//
void KFileBookmarkManager::write( const QString& filename )
{
	QFile file( filename );

	if ( !file.open( IO_WriteOnly ) )
		return;

	// rich
	myFilename= filename;

	QTextStream stream( &file );

	stream << "<!DOCTYPE KDEHELP-Bookmark-file>" << endl;
	stream << i18n("<!-- Do not edit this file -->") << endl;
	stream << "<TITLE>" << title << "</TITLE>" << endl;

	stream << "<H1>" << title << "</H1>" << endl;

	stream << "<DL><p>" << endl;

	writeFolder( stream, &root );

	stream << "</DL><P>" << endl;
}

// write the contents of a folder (recursive)
//
void KFileBookmarkManager::writeFolder( QTextStream &stream, KFileBookmark *parent )
{
	KFileBookmark *bm;

	for ( bm = parent->getChildren().first(); bm != NULL;
			bm = parent->getChildren().next() )
	{
		if ( bm->getType() == KFileBookmark::URL )
		{
			stream << "<DT><A HREF=\"" << bm->getURL() << "\">"
				<< bm->getText() << "</A>" << endl;
		}
		else
		{
			stream << "<DT><H3>" << bm->getText() << "</H3>" << endl;
			stream << "<DL><P>" << endl;
			writeFolder( stream, bm );
			stream << "</DL><P>" << endl;
		}
	}
}

KFileBookmark *KFileBookmarkManager::getBookmark( int id )
{
	int currId = 0;

	return findBookmark( &root, id, currId );
}

KFileBookmark *KFileBookmarkManager::findBookmark( KFileBookmark *parent, int id,
	int &currId )
{
	KFileBookmark *bm;

	for ( bm = parent->getChildren().first(); bm != NULL;
			bm = parent->getChildren().next() )
	{
		if ( bm->getType() == KFileBookmark::URL )
		{
			if ( currId == id )
				return bm;
			currId++;
		}
		else
		{
			KFileBookmark *retbm;
			if ( ( retbm = findBookmark( bm, id, currId ) ) != NULL )
				return retbm;
		}
	}

	return NULL;
}

void KFileBookmarkManager::add( const QString& _text, const QString& _url )
{
	root.getChildren().append( new KFileBookmark( _text, _url ) );

	emit changed();
}

// rich
bool KFileBookmarkManager::remove(int i)
{
  bool result= false;
  if (i >= 0) {
    root.getChildren().remove(i);
    emit changed();
    result= true;
  }
  return result;
}

// rich
void KFileBookmarkManager::rename(int i, const QString& s)
{
  KFileBookmark *b;

  if (i > 0) {
    b= root.getChildren().at(i);
    b->setText(s);
    emit changed();
  }
}

// rich
bool KFileBookmarkManager::moveUp(int i)
{
  KFileBookmark *b;
  bool result= false;

  if (i > 0) {
    b= root.getChildren().take(i);
    root.getChildren().insert(i-1, b);
    emit changed();
    result= true;
  }
  return result;
}

// rich
bool KFileBookmarkManager::moveDown(int i)
{
  KFileBookmark *b;
  uint j= i;
  bool result= false;

  if (j < (root.getChildren().count() -1)) {
    b= root.getChildren().take(i);
    root.getChildren().insert(i+1, b);
    emit changed();
    result= true;
  }
  return result;
}

// rich
void KFileBookmarkManager::reread()
{
  read(myFilename);
}

// rich
void KFileBookmarkManager::write()
{
  write(myFilename);
}

#include "kfilebookmark.moc"
