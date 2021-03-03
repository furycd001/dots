/*****************************************************************
Copyright (c) 2000 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qvariant.h>
#include <qcolor.h>
#include "../../kdecore/kdatastream.h"
#include "../dcopclient.h"
#include "../dcopref.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static DCOPClient* dcop = 0;

void queryApplications()
{
    QCStringList apps = dcop->registeredApplications();
    for ( QCStringList::Iterator it = apps.begin(); it != apps.end(); ++it )
	if ( (*it) != dcop->appId() && (*it).left(9) != "anonymous" )
	    printf( "%s\n", (*it).data() );

    if ( !dcop->isAttached() )
    {
	qWarning( "server not accessible" );
        exit(1);
    }
}

void queryObjects( const char* app )
{
    bool ok = false;
    QCStringList objs = dcop->remoteObjects( app, &ok );
    for ( QCStringList::Iterator it = objs.begin(); it != objs.end(); ++it ) {
	if ( (*it) == "default" && ++it != objs.end() )
	    printf( "%s (default)\n", (*it).data() );
	else
	    printf( "%s\n", (*it).data() );
    }
    if ( !ok )
    {
	qWarning( "application '%s' not accessible", app );
	exit(1);
    }
}

void queryFunctions( const char* app, const char* obj )
{
    bool ok = false;
    QCStringList funcs = dcop->remoteFunctions( app, obj, &ok );
    for ( QCStringList::Iterator it = funcs.begin(); it != funcs.end(); ++it ) {
	printf( "%s\n", (*it).data() );
    }
    if ( !ok )
    {
	qWarning( "object '%s' in application '%s' not accessible", obj, app );
        exit(1);
    }
}


bool mkBool( const QString& s )
{
    if ( s.lower()  == "true" )
	return TRUE;
    if ( s.lower()  == "yes" )
	return TRUE;
    if ( s.lower()  == "on" )
	return TRUE;
    if ( s.toInt() != 0 )
	return TRUE;

    return FALSE;
}

QPoint mkPoint( const QString &str )
{
    const char *s = str.latin1();
    char *end;
    while(*s && !isdigit(*s)) s++;
    int x = strtol(s, &end, 10);
    s = (const char *)end;
    while(*s && !isdigit(*s)) s++;
    int y = strtol(s, &end, 10);
    return QPoint( x, y );
}

QSize mkSize( const QString &str )
{
    const char *s = str.latin1();
    char *end;
    while(*s && !isdigit(*s)) s++;
    int w = strtol(s, &end, 10);
    s = (const char *)end;
    while(*s && !isdigit(*s)) s++;
    int h = strtol(s, &end, 10);
    return QSize( w, h );
}

QRect mkRect( const QString &str )
{
    const char *s = str.latin1();
    char *end;
    while(*s && !isdigit(*s)) s++;
    int p1 = strtol(s, &end, 10);
    s = (const char *)end;
    bool legacy = (*s == 'x');
    while(*s && !isdigit(*s)) s++;
    int p2 = strtol(s, &end, 10);
    s = (const char *)end;
    while(*s && !isdigit(*s)) s++;
    int p3 = strtol(s, &end, 10);
    s = (const char *)end;
    while(*s && !isdigit(*s)) s++;
    int p4 = strtol(s, &end, 10);
    if (legacy)
    {
       return QRect( p3, p4, p1, p2 );
    }
    return QRect( p1, p2, p3, p4 );
}

QColor mkColor( const QString& s )
{
    QColor c;
    c.setNamedColor(s);
    return c;
}

QCString demarshal( QDataStream &stream, const QString &type )
{
    QCString result;

    if ( type == "int" )
    {
        int i;
        stream >> i;
        result.sprintf( "%d", i );
    } else if ( type == "uint" )
    {
        uint i;
        stream >> i;
        result.sprintf( "%d", i );
    } else if ( type == "long" )
    {
        long l;
        stream >> l;
        result.sprintf( "%ld", l );
    } else if ( type == "float" )
    {
        float f;
        stream >> f;
        result.sprintf( "%f", (double)f );
    } else if ( type == "double" )
    {
        double d;
        stream >> d;
        result.sprintf( "%f", d );
    } else if ( type == "bool" )
    {
        bool b;
        stream >> b;
        result = b ? "true" : "false";
    } else if ( type == "QString" )
    {
        QString s;
        stream >> s;
        result = s.local8Bit();
    } else if ( type == "QCString" )
    {
        stream >> result;
    } else if ( type == "QCStringList" )
    {
        return demarshal( stream, "QValueList<QCString>" );
    } else if ( type == "QStringList" )
    {
        return demarshal( stream, "QValueList<QString>" );
    } else if ( type == "QColor" )
    {
        QColor c;
        stream >> c;
        result = c.name().local8Bit();
    } else if ( type == "QSize" )
    {
        QSize s;
        stream >> s;
        result.sprintf( "%dx%d", s.width(), s.height() );
    } else if ( type == "QPoint" )
    {
        QPoint p;
        stream >> p;
        result.sprintf( "+%d+%d", p.x(), p.y() );
    } else if ( type == "QRect" )
    {
        QRect r;
        stream >> r;
        result.sprintf( "%dx%d+%d+%d", r.width(), r.height(), r.x(), r.y() );
    } else if ( type == "QVariant" )
    {
        Q_INT32 type;
        stream >> type;
        return demarshal( stream, QVariant::typeToName( (QVariant::Type)type ) );
    } else if ( type == "DCOPRef" )
    {
        DCOPRef r;
        stream >> r;
        result.sprintf( "DCOPRef(%s,%s)", r.app().data(), r.object().data() );
    } else if ( type.left( 11 ) == "QValueList<" )
    {
        if ( (uint)type.find( '>', 11 ) != type.length() - 1 )
            return result;

        QString nestedType = type.mid( 11, type.length() - 12 );

        if ( nestedType.isEmpty() )
            return result;

        Q_UINT32 count;
        stream >> count;

        Q_UINT32 i = 0;
        for (; i < count; ++i )
        {
            QCString arg = demarshal( stream, nestedType );
            if ( arg.isEmpty() )
                continue;

            result += arg;

            if ( i < count - 1 )
                result += '\n';
        }
    } else if ( type.left( 5 ) == "QMap<" )
    {
        int commaPos = type.find( ',', 5 );

        if ( commaPos == -1 )
            return result;

        if ( (uint)type.find( '>', commaPos ) != type.length() - 1 )
            return result;

        QString keyType = type.mid( 5, commaPos - 5 );
        QString valueType = type.mid( commaPos + 1, type.length() - commaPos - 2 );

        Q_UINT32 count;
        stream >> count;

        Q_UINT32 i = 0;
        for (; i < count; ++i )
        {
            QCString key = demarshal( stream, keyType );

            if ( key.isEmpty() )
                continue;

            QCString value = demarshal( stream, valueType );

            if ( value.isEmpty() )
                continue;

            result += key + "->" + value;

            if ( i < count - 1 )
                result += '\n';
        }
    }

    return result;

}

void callFunction( const char* app, const char* obj, const char* func, int argc, char** args )
{

    QString f = func; // Qt is better with unicode strings, so use one.
    int left = f.find( '(' );
    int right = f.find( ')' );

    if ( right <  left )
    {
	qWarning( "parentheses do not match" );
        exit(1);
    }

    if ( left < 0 ) {
	// try to get the interface from the server
	bool ok = false;
	QCStringList funcs = dcop->remoteFunctions( app, obj, &ok );
	QCString realfunc;
	if ( !ok && argc == 0 )
	    goto doit;
	if ( !ok )
        {
	    qWarning( "object not accessible" );
            exit(1);
        }
	for ( QCStringList::Iterator it = funcs.begin(); it != funcs.end(); ++it ) {
	    int l = (*it).find( '(' );
	    int s = (*it).find( ' ');
	    if ( s < 0 )
		s = 0;
	    else
		s++;

	    if ( l > 0 && (*it).mid( s, l - s ) == func ) {
		realfunc = (*it).mid( s );
		int a = (*it).contains(',');
		if ( ( a == 0 && argc == 0) || ( a > 0 && a + 1 == argc ) )
		    break;
	    }
	}
	if ( realfunc.isEmpty() )
	{
	    qWarning("no such function");
            exit(1);
	}
	f = realfunc;
	left = f.find( '(' );
	right = f.find( ')' );
    }

 doit:
    if ( left < 0 )
	f += "()";

    // This may seem expensive but is done only once per invocation
    // of dcop, so it should be OK.
    //
    //
    QStringList intTypes;
    intTypes << "int" << "unsigned" << "long" << "bool" ;

    QStringList types;
    if ( left >0 && left + 1 < right - 1) {
	types = QStringList::split( ',', f.mid( left + 1, right - left - 1) );
	for ( QStringList::Iterator it = types.begin(); it != types.end(); ++it ) {
	    QString lt = (*it).simplifyWhiteSpace();

	    int s = lt.find(' ');

	    // If there are spaces in the name, there may be two
	    // reasons: the parameter name is still there, ie.
	    // "QString URL" or it's a complicated int type, ie.
	    // "unsigned long long int bool".
	    //
	    //
	    if ( s > 0 )
	    {
		QStringList partl = QStringList::split(' ' , lt);
		
		// The zero'th part is -- at the very least -- a 
		// type part. Any trailing parts *might* be extra
		// int-type keywords, or at most one may be the
		// parameter name.
		//
		//
	    	s=1;

		while (s < partl.count() && intTypes.contains(partl[s]))
		{
			s++;
		}

		if (s<partl.count()-1)
		{
			qWarning("The argument `%s' seems syntactically wrong.",
				lt.latin1());
		}
		if (s==partl.count()-1)
		{
			partl.remove(partl.at(s));
		}

	    	lt = partl.join(" ");
		lt = lt.simplifyWhiteSpace();
	    }

	    (*it) = lt;
	}
	QString fc = f.left( left );
	fc += '(';
	bool first = TRUE;
	for ( QStringList::Iterator it = types.begin(); it != types.end(); ++it ) {
	    if ( !first )
		fc +=",";
	    first = FALSE;
	    fc += *it;
	}
	fc += ')';
	f = fc;
    }

    if ( (int) types.count() != argc ) {
	qWarning( "arguments do not match" );
	exit(1);
    }

    QByteArray data, replyData;
    QCString replyType;
    QDataStream arg(data, IO_WriteOnly);

    int i = 0;
    for ( QStringList::Iterator it = types.begin(); it != types.end(); ++it ) {
	QString type = *it;
	QString s = args[i++];
	if ( type == "int" )
	    arg << s.toInt();
	else if ( type == "uint" )
	    arg << s.toUInt();
	else if ( type == "unsigned" )
	    arg << s.toUInt();
	else if ( type == "unsigned int" )
	    arg << s.toUInt();
	else if ( type == "long" )
	    arg << s.toLong();
	else if ( type == "long int" )
	    arg << s.toLong();
	else if ( type == "unsigned long" )
	    arg << s.toULong();
	else if ( type == "unsigned long int" )
	    arg << s.toULong();
	else if ( type == "float" )
	    arg << s.toFloat();
	else if ( type == "double" )
	    arg << s.toDouble();
	else if ( type == "bool" )
	    arg << mkBool( s );
	else if ( type == "QString" )
	    arg << s;
	else if ( type == "QCString" )
	    arg << QCString( s.latin1() );
	else if ( type == "QColor" )
	    arg << mkColor( s );
	else if ( type == "QPoint" )
	    arg << mkPoint( s );
	else if ( type == "QSize" )
	    arg << mkSize( s );
	else if ( type == "QRect" )
	    arg << mkRect( s );
	else if ( type == "QVariant" ) {
	    if ( s == "true" || s == "false" )
		arg << QVariant( mkBool( s ), 42 );
	    else if ( s.left( 4 ) == "int(" )
		arg << QVariant( s.mid(4, s.length()-5).toInt() );
	    else if ( s.left( 7 ) == "QPoint(" )
		arg << QVariant( mkPoint( s.mid(7, s.length()-8) ) );
	    else if ( s.left( 6 ) == "QSize(" )
		arg << QVariant( mkSize( s.mid(6, s.length()-7) ) );
	    else if ( s.left( 6 ) == "QRect(" )
		arg << QVariant( mkRect( s.mid(6, s.length()-7) ) );
	    else if ( s.left( 7 ) == "QColor(" )
		arg << QVariant( mkColor( s.mid(7, s.length()-8) ) );
	    else
		arg << QVariant( s );
	} else {
	    qWarning( "cannot handle datatype '%s'", type.latin1() );
	    exit(1);
	}
    }

    if ( !dcop->call( app, obj, f.latin1(),  data, replyType, replyData) ) {
	qWarning( "call failed");
        exit(1);
    } else {
	QDataStream reply(replyData, IO_ReadOnly);

        if ( replyType != "void" && replyType != "ASYNC" )
        {
            QCString replyString = demarshal( reply, replyType );
            if ( replyString.isEmpty() )
                replyString.sprintf( "<%s>", replyType.data() );

            printf( "%s\n", replyString.data() );
        }
    }
}



int main( int argc, char** argv )
{

    if ( argc > 1 && argv[1][0] == '-' ) {
	fprintf( stderr, "Usage: dcop [ application [object [function [arg1] [arg2] [arg3] ... ] ] ] \n" );
	exit(0);
    }

    DCOPClient client;
    client.attach();
    dcop = &client;

    switch ( argc ) {
    case 0:
    case 1:
	queryApplications();
	break;
    case 2:
	queryObjects( argv[1] );
	break;
    case 3:
	queryFunctions( argv[1], argv[2] );
	break;
    case 4:
    default:
	callFunction( argv[1], argv[2], argv[3], argc - 4, &argv[4] );
	break;

    }

    return 0;
}
