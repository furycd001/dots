/****************************************************************************
** $Id: qt/examples/xmlquotes/quoteparser.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#include "quoteparser.h"

QuoteHandler::QuoteHandler()
{
}


QuoteHandler::~QuoteHandler()
{
}


QStringList QuoteHandler::quotes()
{
    return quoteList;
}


QString QuoteHandler::errorProtocol()
{
    return errorProt;
}


bool QuoteHandler::startDocument()
{
    // at the beginning of parsing: do some initialization
    quoteList.clear();
    errorProt = "";
    state = StateInit;

    return TRUE;
}


bool QuoteHandler::startElement( const QString&, const QString&, const QString& qName, const QXmlAttributes& atts )
{
    // do different actions depending on the name of the tag and the
    // state you are in
    if        ( qName == "quotations" && state == StateInit ) {
	state = StateDocument;
    } else if ( qName == "quote"      && state == StateDocument ) {
	state = StateQuote;
	// add a new quote to the end of the list
	QString tmp;
	quoteList.append( tmp );
	// add some nice header
	quoteList.last() += "<b>Saying " +
	    QString::number( quoteList.count() ) +
	    "</b><br>"
	    "<hr><br><br>"
	    "<big>";
	// a quote can have a author and a reference
	author = atts.value( "author" );
	reference = atts.value( "reference" );
    } else if ( qName == "line"       && state == StateQuote ) {
	state = StateLine;
    } else if ( qName == "heading"    && state == StateQuote ) {
	state = StateHeading;
	// headings should be bold
	quoteList.last() += "<b>";
    } else if ( qName == "p"          && state == StateQuote ) {
	state = StateP;
	// start tag
	quoteList.last() += "</p>";
    } else {
	// error
	return FALSE;
    }
    return TRUE;
}


bool QuoteHandler::endElement( const QString&, const QString&, const QString& )
{
    // "pop" the state and do some actions
    switch ( state ) {
	case StateQuote:
	    state = StateDocument;
	    // add closing tags and author and reference
	    quoteList.last() += "</big><br><br>"
				"<center><i>-- ";
	    if ( author.isEmpty() )
		quoteList.last() += "Unknown";
	    else
		quoteList.last() += author;
	    if ( !reference.isEmpty() )
		quoteList.last() += ", \"" + reference + "\"";
	    quoteList.last() += "</i></center><i>";
	    break;
	case StateLine:
	    state = StateQuote;
	    // force line break
	    quoteList.last() += "<br>";
	    break;
	case StateHeading:
	    state = StateQuote;
	    // closing tag
	    quoteList.last() += "</b><br>";
	    break;
	case StateP:
	    state = StateQuote;
	    // closing tag
	    quoteList.last() += "</p>";
	    break;
	default:
	    // do nothing
	    break;
    }
    return TRUE;
}


bool QuoteHandler::characters( const QString& ch )
{
    // we are not interested in whitespaces
    QString ch_simplified = ch.simplifyWhiteSpace();
    if ( ch_simplified.isEmpty() )
	return TRUE;

    switch ( state ) {
	case StateQuote:
	case StateLine:
	case StateHeading:
	case StateP:
	    quoteList.last() += ch_simplified;
	    break;
	default:
	    return FALSE;
    }

    return TRUE;
}


QString QuoteHandler::errorString()
{
    return "the document is not in the quote file format";
}


bool QuoteHandler::fatalError( const QXmlParseException& exception )
{
    errorProt += QString( "fatal parsing error: %1 in line %2, column %3\n" )
	.arg( exception.message() )
	.arg( exception.lineNumber() )
	.arg( exception.columnNumber() );

    return QXmlDefaultHandler::fatalError( exception );
}
