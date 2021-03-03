/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt GUI Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include <qvaluelist.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qapplication.h>

// STEP1: Include header files of the widgets for which a description
// should be created here. If you have a widget which is defined in
// the file mycustomwidget.h in /home/joedeveloper/src, write here
//
// #include "/home/joedeveloper/src/mycustomwidget.h"
//
// Now go on to STEP2!

// -----------------------------

struct Widget
{
    QWidget *w; // the widget
    QString include; // header file
    QString location; // "global" for include <...> or "local" include "..."
};

static QString makeIndent( int indent )
{
    QString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static QString entitize( const QString &s )
{
    QString s2 = s;
    s2 = s2.replace( QRegExp( "\"" ), "&quot;" );
    s2 = s2.replace( QRegExp( "&" ), "&amp;" );
    s2 = s2.replace( QRegExp( ">" ), "&gt;" );
    s2 = s2.replace( QRegExp( "<" ), "&lt;" );
    s2 = s2.replace( QRegExp( "'" ), "&apos;" );
    return s2;
}

static QString convert_type( const QString &s )
{
    QString str( s );
    if ( str[ 0 ] == 'Q' )
	str.remove( 0, 1 );
    str[ 0 ] = str[ 0 ].upper();
    return str;
}

static void createDescription( const QValueList<Widget> &l, QTextStream &ts )
{
    int indent = 0;
    ts << "<!DOCTYPE CW><CW>" << endl;
    ts << makeIndent( indent ) << "<customwidgets>" << endl;
    indent++;

    for ( QValueList<Widget>::ConstIterator it = l.begin(); it != l.end(); ++it ) {
	Widget w = *it;
	ts << makeIndent( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<class>" << w.w->className() << "</class>" << endl;
	ts << makeIndent( indent ) << "<header location=\"" << w.location << "\">" << w.include << "</header>" << endl;
	ts << makeIndent( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << w.w->sizeHint().width() << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << w.w->sizeHint().height() << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizehint>" << endl;
	ts << makeIndent( indent ) << "<container>" << w.w->inherits( "QGroupBox" ) << "</container>" << endl;
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hordata>" << (int)w.w->sizePolicy().horData() << "</hordata>" << endl;
	ts << makeIndent( indent ) << "<verdata>" << (int)w.w->sizePolicy().verData() << "</verdata>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	
	QStrList sigs = w.w->metaObject()->signalNames( TRUE );
	if ( !sigs.isEmpty() ) {
	    for ( int i = 0; i < (int)sigs.count(); ++i )
		ts << makeIndent( indent ) << "<signal>" << entitize( sigs.at( i ) ) << "</signal>" << endl;
	}
	QStrList slts = w.w->metaObject()->slotNames( TRUE );
	if ( !slts.isEmpty() ) {
	    for ( int i = 0; i < (int)slts.count(); ++i ) {
		QMetaData::Access data = w.w->metaObject()->slot_access( i, TRUE );
		if ( data == QMetaData::Private )
		    continue;
		ts << makeIndent( indent ) << "<slot access=\""
		   << ( data == QMetaData::Protected ? "protected" : "public" )
		   << "\">" << entitize( slts.at( i ) ) << "</slot>" << endl;
	    }
	}	
	QStrList props = w.w->metaObject()->propertyNames( TRUE );
	if ( !props.isEmpty() ) {
	    for ( int i = 0; i < (int)props.count(); ++i ) {
		const QMetaProperty *p = w.w->metaObject()->property( props.at( i ), TRUE );
		if ( !p )
		    continue;
		if ( !p->writable() || !p->designable() )
		    continue;
		ts << makeIndent( indent ) << "<property type=\"" << convert_type( p->type() ) << "\">" << entitize( p->name() ) << "</property>" << endl;
	    }
	}
	indent--;
	ts << makeIndent( indent ) << "</customwidget>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</customwidgets>" << endl;
    ts << "</CW>" << endl;
}

int main( int argc, char **argv )
{
    if ( argc < 2 )
	return -1;
    QString fn = argv[1];
    QFile f( fn );
    if ( !f.open( IO_WriteOnly ) )
	return -1;
    QTextStream ts( &f );
    QApplication a( argc, argv );

    QValueList<Widget> wl;

    // STEP2: Instantiate all widgets for which a description should
    // be created here and add them to the list wl. If your custom widget
    // is e.g. called MyCustomWidget you would write here
    //
    // Widget w;
    // w.w = new MyCustomWidget( 0, 0 );
    // w.include = "mycustomwidget.h";
    // w.location = "global";
    // wl.append( w );
    //
    // After that compile the program, link it with your custom widget
    // (library or object file) and run it like this:
    // (unix): ./createcw mywidgets.cw
    // (win32): createcw mywidgets.cw
    //
    // After that you can import this description file into the Qt
    // Designer using the Custom-Widget Dialog (See
    // Tools->Custom->Edit Custom Widgets... in the Qt Designer)
    // and use these custom widget there in your forms.



    // ----------------------------------------------

    createDescription( wl, ts );
    f.close();
    return 0;
}
