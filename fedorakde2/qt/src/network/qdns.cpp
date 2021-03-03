/****************************************************************************
** $Id: qt/src/network/qdns.cpp   2.3.2   edited 2001-10-14 $
**
** Implementation of QDns class.
**
** Created : 991122
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qdns.h"

#ifndef QT_NO_DNS

#include "qdatetime.h"
#include "qdict.h"
#include "qlist.h"
#include "qstring.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qvector.h"
#include "qstrlist.h"
#include "qptrdict.h"
#include "qfile.h"
#include "qtextstream.h"


//#define DEBUG_QDNS


static Q_UINT16 id; // ### seeded started by now()


static QDateTime * originOfTime = 0;


static void cleanup()
{
    delete originOfTime;
    originOfTime = 0;
}

static Q_UINT32 now()
{
    if ( originOfTime )
	return originOfTime->secsTo( QDateTime::currentDateTime() );

    originOfTime = new QDateTime( QDateTime::currentDateTime() );
    ::id = originOfTime->time().msec() * 60 + originOfTime->time().second();
    qAddPostRoutine( cleanup );
    return 0;
}


static QList<QHostAddress> * ns = 0;
static QStrList * domains = 0;

static void doResInit();


class QDnsPrivate {
public:
    QDnsPrivate() : startQueryTimer(FALSE) {}
    ~QDnsPrivate() {}
private:
    bool startQueryTimer;

    friend class QDns;
};


class QDnsRR;
class QDnsDomain;



// QDnsRR is the class used to store a single RR.  QDnsRR can store
// all of the supported RR types.  a QDnsRR is always cached.

// QDnsRR is mostly constructed from the outside.  a but hacky, but
// permissible since the entire class is internal.

class QDnsRR {
public:
    QDnsRR( const QString & label );
    ~QDnsRR();

public:
    QDnsDomain * domain;
    QDns::RecordType t;
    bool nxdomain;
    bool current;
    Q_UINT32 expireTime;
    Q_UINT32 deleteTime;
    // somewhat space-wasting per-type data
    // a / aaaa
    QHostAddress address;
    // cname / mx / srv / ptr
    QString target;
    // mx / srv
    Q_UINT16 priority;
    // srv
    Q_UINT16 weight;
    Q_UINT16 port;
    // txt
    QString text; // could be overloaded into target...
private:

};


class QDnsDomain {
public:
    QDnsDomain( const QString & label );
    ~QDnsDomain();

    static void add( const QString & label, QDnsRR * );
    static QList<QDnsRR> * cached( const QDns * );

    void take( QDnsRR * );

    void sweep( Q_UINT32 thisSweep );

    bool isEmpty() const { return rrs == 0 || rrs->isEmpty(); }

    QString name() const { return l; }

public:
    QString l;
    QList<QDnsRR> * rrs;
};


class QDnsQuery: public QTimer { // this inheritance is a very evil hack
public:
    QDnsQuery():
	id( 0 ), t( QDns::None ), step(0), started(0),
	dns( new QPtrDict<void>(17) ) {}
    Q_UINT16 id;
    QDns::RecordType t;
    QString l;

    uint step;
    Q_UINT32 started;

    QPtrDict<void> * dns;
};



class QDnsAnswer {
public:
    QDnsAnswer( QDnsQuery * );
    QDnsAnswer( const QByteArray &, QDnsQuery * );
    ~QDnsAnswer();

    void parse();
    void notify();

    bool ok;

private:
    QDnsQuery * query;

    Q_UINT8 * answer;
    int size;
    int pp;

    QList<QDnsRR> * rrs;

    // convenience
    int next;
    int ttl;
    QString label;
    QDnsRR * rr;

    QString readString();
    void parseA();
    void parseAaaa();
    void parseMx();
    void parseSrv();
    void parseCname();
    void parsePtr();
    void parseTxt();
    void parseNs();
};


QDnsRR::QDnsRR( const QString & label )
    : domain( 0 ), t( QDns::None ),
      nxdomain( FALSE ), current( FALSE ),
      expireTime( 0 ), deleteTime( 0 ),
      priority( 0 ), weight( 0 ), port( 0 )
{
    QDnsDomain::add( label, this );
}


// not supposed to be deleted except by QDnsDomain
QDnsRR::~QDnsRR()
{
    // nothing is necessary
}


// this one just sticks in a NXDomain
QDnsAnswer::QDnsAnswer( QDnsQuery * query_ )
{
    ok = TRUE;

    answer = 0;
    size = 0;
    query = query_;
    pp = 0;
    rrs = new QList<QDnsRR>;
    rrs->setAutoDelete( FALSE );
    next = size;
    ttl = 0;
    label = QString::null;
    rr = 0;

    QDnsRR * newrr = new QDnsRR( query->l );
    newrr->t = query->t;
    newrr->deleteTime = query->started + 10;
    newrr->expireTime = query->started + 10;
    newrr->nxdomain = TRUE;
    newrr->current = TRUE;
    rrs->append( newrr );
}


QDnsAnswer::QDnsAnswer( const QByteArray& answer_,
			QDnsQuery * query_ )
{
    ok = TRUE;

    answer = (Q_UINT8 *)(answer_.data());
    size = (int)answer_.size();
    query = query_;
    pp = 0;
    rrs = new QList<QDnsRR>;
    rrs->setAutoDelete( FALSE );
    next = size;
    ttl = 0;
    label = QString::null;
    rr = 0;
}


QDnsAnswer::~QDnsAnswer()
{
    if ( !ok && rrs ) {
	QListIterator<QDnsRR> it( *rrs );
	QDnsRR * tmprr;
	while( (tmprr=it.current()) != 0 ) {
	    ++it;
	    tmprr->t = QDns::None; // will be deleted soonish
	}
    }
}


QString QDnsAnswer::readString()
{
    int p = pp;
    QString r = QString::null;
    Q_UINT8 b;
    for (;;) {
	b = 128;
	if ( p >= 0 && p < size )
	    b = answer[p];

	switch( b >> 6 ) {
	case 0:
	    p++;
	    if ( b == 0 ) {
		if ( p > pp )
		    pp = p;
		return r.isNull() ? QString( "." ) : r;
	    }
	    if ( !r.isNull() )
		r += '.';
	    while( b-- > 0 )
		r += QChar( answer[p++] );
	    break;
	default:
	    ok = FALSE;
	    return QString::null;
	case 3:
	    int q = ( (answer[p] & 0x3f) << 8 ) + answer[p+1];
	    if ( q >= pp || q >= p ) {
		ok = FALSE;
		return QString::null;
	    }
	    if ( p >= pp )
		pp = p + 2;
	    p = q;
	    break;
	}
    }
#if defined(Q_SPURIOUS_NON_VOID_WARNING)
    return QString::null;
#endif
}



void QDnsAnswer::parseA()
{
    if ( next != pp + 4 ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw %d bytes long IN A for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->t = QDns::A;
    rr->address = QHostAddress( ( answer[pp+0] << 24 ) +
				( answer[pp+1] << 16 ) +
				( answer[pp+2] <<  8 ) +
				( answer[pp+3] ) );
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN A %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}


void QDnsAnswer::parseAaaa()
{
    if ( next != pp + 16 ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw %d bytes long IN Aaaa for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->t = QDns::Aaaa;
    rr->address = QHostAddress( answer+pp );
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN Aaaa %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}



void QDnsAnswer::parseMx()
{
    if ( next < pp + 2 ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw %d bytes long IN MX for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    pp += 2;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad string in MX for %s", label.ascii() );
#endif
	return;
    }
    rr->t = QDns::Mx;
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN MX %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->target.ascii(), ttl );
#endif
}


void QDnsAnswer::parseSrv()
{
    if ( next < pp + 6 ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw %d bytes long IN SRV for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    rr->weight = (answer[pp+2] << 8) + answer[pp+3];
    rr->port = (answer[pp+4] << 8) + answer[pp+5];
    pp += 6;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad string in SRV for %s", label.ascii() );
#endif
	return;
    }
    rr->t = QDns::Srv;
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN SRV %d %d %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->weight, rr->port, rr->target.ascii(), ttl );
#endif
}


void QDnsAnswer::parseCname()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->t = QDns::Cname;
    rr->target = target;
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN CNAME %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void QDnsAnswer::parseNs()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    // parse, but ignore

#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN NS %s (ttl %d)", label.ascii(),
	    target.ascii(), ttl );
#endif
}


void QDnsAnswer::parsePtr()
{
    QString target = readString().lower();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad PTR for for %s", label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->t = QDns::Ptr;
    rr->target = target;
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN PTR %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void QDnsAnswer::parseTxt()
{
    QString text = readString();
    if ( !ok ) {
#if defined(DEBUG_QDNS)
	qDebug( "QDns: saw bad TXT for for %s", label.ascii() );
#endif
	return;
    }

    rr = new QDnsRR( label );
    rr->t = QDns::Txt;
    rr->text = text;
#if defined(DEBUG_QDNS)
    qDebug( "QDns: saw %s IN TXT \"%s\" (ttl %d)", label.ascii(),
	    rr->text.ascii(), ttl );
#endif
}


void QDnsAnswer::parse()
{
    // okay, do the work...
    if ( (answer[2] & 0x78) != 0 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: asnwer to wrong query type (%d)", answer[1] );
#endif
	ok = FALSE;
	return;
    }

    // AA
    bool aa = (answer[2] & 4) != 0;

    // TC
    if ( (answer[2] & 2) != 0 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: truncated answer; pressing on" );
#endif
    }

    // RD
    bool rd = (answer[2] & 1) != 0;

    // we don't test RA
    // we don't test the MBZ fields

    if ( (answer[3] & 0x0f) == 3 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: saw NXDomain for %s", query->l.ascii() );
#endif
	// NXDomain.  cache that for one minute.
	rr = new QDnsRR( query->l );
	rr->t = query->t;
	rr->deleteTime = query->started + 60;
	rr->expireTime = query->started + 60;
	rr->nxdomain = TRUE;
	rr->current = TRUE;
	rrs->append( rr );
	return;
    }

    if ( (answer[3] & 0x0f) != 0 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: error code %d", answer[3] & 0x0f );
#endif
	ok = FALSE;
	return;
    }

    int qdcount = ( answer[4] << 8 ) + answer[5];
    int ancount = ( answer[6] << 8 ) + answer[7];
    int nscount = ( answer[8] << 8 ) + answer[9];
    int adcount = (answer[10] << 8 ) +answer[11];

    pp = 12;

    // read query
    while( qdcount > 0 && pp < size ) {
	// should I compare the string against query->l?
	(void)readString();
	if ( !ok )
	    return;
	pp += 4;
	qdcount--;
    }

    // answers and stuff
    int rrno = 0;
    // if we parse the answer completely, but there are no answers,
    // ignore the entire thing.
    int answers = 0;
    while( ( rrno < ancount ||
	     ( ok && answers >0 && rrno < ancount + nscount + adcount ) ) &&
	   pp < size ) {
	label = readString().lower();
	if ( !ok )
	    return;
	int rdlength = 0;
	if ( pp + 10 <= size )
	    rdlength = ( answer[pp+8] << 8 ) + answer[pp+9];
	if ( pp + 10 + rdlength > size ) {
#if defined(DEBUG_QDNS)
	    qDebug( "DNS Manager: ran out of stuff to parse (%d+%d>%d (%d)",
		    pp, rdlength, size, rrno < ancount );
#endif
	    // if we're still in the AN section, we should go back and
	    // at least down the TTLs.  probably best to invalidate
	    // the results.
	    // the rrs list is good for this
	    ok = ( rrno < ancount );
	    return;
	}
	uint type, clas;
	type = ( answer[pp+0] << 8 ) + answer[pp+1];
	clas = ( answer[pp+2] << 8 ) + answer[pp+3];
	ttl = ( answer[pp+4] << 24 ) + ( answer[pp+5] << 16 ) +
	      ( answer[pp+6] <<  8 ) + answer[pp+7];
	pp = pp + 10;
	if ( clas != 1 ) {
#if defined(DEBUG_QDNS)
	    qDebug( "DNS Manager: class %d (not internet) for %s",
		    clas, label.isNull() ? "." : label.ascii() );
#endif
	} else {
	    next = pp + rdlength;
	    rr = 0;
	    switch( type ) {
	    case 1:
		parseA();
		break;
	    case 28:
		parseAaaa();
		break;
	    case 15:
		parseMx();
		break;
	    case 33:
		parseSrv();
		break;
	    case 5:
		parseCname();
		break;
	    case 12:
		parsePtr();
		break;
	    case 16:
		parseTxt();
		break;
	    case 2:
		parseNs();
		break;
	    default:
		// something we don't know
#if defined(DEBUG_QDNS)
		qDebug( "DNS Manager: type %d for %s", type,
			label.isNull() ? "." : label.ascii() );
#endif
		break;
	    }
	    if ( rr ) {
		rr->deleteTime = 0;
		if ( ttl > 0 )
		    rr->expireTime = query->started + ttl;
		else
		    rr->expireTime = query->started + 20;
		if ( rrno < ancount ) {
		    answers++;
		    rr->deleteTime = rr->expireTime;
		}
		rr->current = TRUE;
		rrs->append( rr );
	    }
	}
	if ( !ok )
	    return;
	pp = next;
	next = size;
	rrno++;
    }
    if ( answers == 0 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: answer contained no answers" );
#endif
	ok = ( aa && rd );
    }

    // now go through the list and mark all the As that are referenced
    // by something we care about.  we want to cache such As.
    rrs->first();
    QDict<void> used( 17 );
    used.setAutoDelete( FALSE );
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr->target.length() && rr->deleteTime > 0 && rr->current )
	    used.insert( rr->target, (void*)42 );
	if ( ( rr->t == QDns::A || rr->t == QDns::Aaaa ) &&
	     used.find( rr->domain->name() ) != 0 )
	    rr->deleteTime = rr->expireTime;
    }

    // next, for each RR, delete any older RRs that are equal to it
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr && rr->domain && rr->domain->rrs ) {
	    QList<QDnsRR> * drrs = rr->domain->rrs;
	    drrs->first();
	    QDnsRR * older;
	    while( (older=drrs->current()) != 0 ) {
		if ( older != rr &&
		     older->t == rr->t &&
		     older->nxdomain == rr->nxdomain &&
		     older->address == rr->address &&
		     older->target == rr->target &&
		     older->priority == rr->priority &&
		     older->weight == rr->weight &&
		     older->port == rr->port &&
		     older->text == rr->text ) {
		    // well, it's equal, but it's not the same. so we kill it,
		    // but use its expiry time.
#if defined(DEBUG_QDNS)
		    qDebug( "killing off old %d for %s, expire was %d",
			   older->t, older->domain->name().latin1(),
			   rr->expireTime );
#endif
		    older->t = QDns::None;
		    rr->expireTime = QMAX( older->expireTime, rr->expireTime );
		    rr->deleteTime = QMAX( older->deleteTime, rr->deleteTime );
		    older->deleteTime = 0;
#if defined(DEBUG_QDNS)
		    qDebug( "    adjusted expire is %d", rr->expireTime );
#endif
		}
		drrs->next();
	    }
	}
    }

#if defined(DEBUG_QDNS)
    //qDebug( "DNS Manager: ()" );
#endif
}


class QDnsUgleHack: public QDns {
public:
    void ugle( bool emitAnyway=FALSE );
};


void QDnsAnswer::notify()
{
    if ( !rrs || !ok || !query || !query->dns )
	return;

    QPtrDict<void> notified;
    notified.setAutoDelete( FALSE );

    QPtrDictIterator<void> it( *query->dns );
    QDns * dns;
    it.toFirst();
    while( (dns=(QDns*)(it.current())) != 0 ) {
	++it;
	if ( notified.find( (void*)dns ) == 0 &&
	     query->dns->find( (void*)dns ) != 0 ) {
	    notified.insert( (void*)dns, (void*)42 );
	    if ( rrs->count() == 0 ) {
#if defined(DEBUG_QDNS)
		qDebug( "DNS Manager: found no answers!" );
#endif
		((QDnsUgleHack*)dns)->ugle( TRUE );
	    } else {
		QStringList n = dns->qualifiedNames();
		if ( n.contains(query->l) )
		    ((QDnsUgleHack*)dns)->ugle();
#if defined(DEBUG_QDNS)
		else
		    qDebug( "DNS Manager: DNS thing %s not notified for %s",
			    dns->label().ascii(), query->l.ascii() );
#endif
	    }
	}
    }
}


//
//
// QDnsManager
//
//


class QDnsManager: public QDnsSocket {
private:
public: // just to silence the moronic g++.
    QDnsManager();
    ~QDnsManager();
public:
    static QDnsManager * manager();

    QDnsDomain * domain( const QString & );

    void transmitQuery( QDnsQuery * );
    void transmitQuery( int );

    // reimplementation of the slots
    void cleanCache();
    void retransmit();
    void answer();

public:
    QVector<QDnsQuery> queries;
    QDict<QDnsDomain> cache;
    QSocketDevice * socket;
};



static QDnsManager * globalManager;


QDnsManager * QDnsManager::manager()
{
    if ( !globalManager )
	new QDnsManager();
    return globalManager;
}


void QDnsUgleHack::ugle( bool emitAnyway)
{
    if ( emitAnyway || !isWorking() ) {
#if defined( DEBUG_QDNS )
	qDebug( "DNS Manager: status change for %s (type %d)",
		label().ascii(), recordType() );
#endif
	emit resultsReady();
    }
}


QDnsManager::QDnsManager()
    : QDnsSocket( qApp, "Internal DNS manager" ),
      queries( QVector<QDnsQuery>( 0 ) ),
      cache( QDict<QDnsDomain>( 83, FALSE ) ),
      socket( new QSocketDevice( QSocketDevice::Datagram ) )
{
    cache.setAutoDelete( TRUE );
    globalManager = this;

    QTimer * sweepTimer = new QTimer( this );
    sweepTimer->start( 1000 * 60 * 3 );
    connect( sweepTimer, SIGNAL(timeout()),
	     this, SLOT(cleanCache()) );

    QSocketNotifier * rn = new QSocketNotifier( socket->socket(),
						QSocketNotifier::Read,
						this, "dns socket watcher" );
    socket->setAddressReusable( FALSE );
    socket->setBlocking( FALSE );
    connect( rn, SIGNAL(activated(int)),
	     this, SLOT(answer()) );

    if ( !ns )
	doResInit();

    // O(n*n) stuff here.  but for 3 and 6, O(n*n) with a low k should
    // be perfect.  the point is to eliminate any duplicates that
    // might be hidden in the lists.
    QList<QHostAddress> * ns = new QList<QHostAddress>;

    ::ns->first();
    QHostAddress * h;
    while( (h=::ns->current()) != 0 ) {
	ns->first();
	while( ns->current() != 0 && !(*ns->current() == *h) )
	    ns->next();
	if ( !ns->current() ) {
	    ns->append( new QHostAddress(*h) );
#if defined(DEBUG_QDNS)
	    qDebug( "using name server %s", h->toString().latin1() );
	} else {
	    qDebug( "skipping address %s", h->toString().latin1() );
#endif
	}
	::ns->next();
    }

    delete ::ns;
    ::ns = ns;
    ::ns->setAutoDelete( TRUE );

    QStrList * domains = new QStrList( TRUE );

    ::domains->first();
    char * s;
    while( (s=::domains->current()) != 0 ) {
	domains->first();
	while( domains->current() != 0 && qstrcmp( domains->current(), s ) )
	    domains->next();
	if ( !domains->current() ) {
	    domains->append( s );
#if defined(DEBUG_QDNS)
	    qDebug( "searching domain %s", s );
	} else {
	    qDebug( "skipping domain %s", s );
#endif
	}
	::domains->next();
    }

    delete ::domains;
    ::domains = domains;
    ::domains->setAutoDelete( TRUE );
}


QDnsManager::~QDnsManager()
{
    if ( globalManager )
	globalManager = 0;
}

static Q_UINT32 lastSweep = 0;

void QDnsManager::cleanCache()
{
    bool again = FALSE;
    QDictIterator<QDnsDomain> it( cache );
    QDnsDomain * d;
    Q_UINT32 thisSweep = now();
#if defined(DEBUG_QDNS)
    qDebug( "QDnsManager::cleanCache(: Called, time is %u, last was %u",
	   thisSweep, lastSweep );
#endif

    while( (d=it.current()) != 0 ) {
	++it;
	d->sweep( thisSweep ); // after this, d may be empty
	if ( !again )
	    again = !d->isEmpty();
    }
    if ( !again )
	delete this;
    lastSweep = thisSweep;
}


void QDnsManager::retransmit()
{
    const QObject * o = sender();
    if ( o == 0 || globalManager == 0 || this != globalManager )
	return;
    uint q = 0;
    while( q < queries.size() && queries[q] != o )
	q++;
    if ( q < queries.size() )
	transmitQuery( q );
}


void QDnsManager::answer()
{
    QByteArray a( 16383 ); // large enough for anything, one suspects
    int r = socket->readBlock( a.data(), a.size() );
#if defined(DEBUG_QDNS)
    qDebug("DNS Manager: answer arrived: %d bytes from %s:%d", r,
	   socket->peerAddress().toString().ascii(), socket->peerPort() );
#endif
    if ( r < 12 )
	return;
    // maybe we should check that the answer comes from port 53 on one
    // of our name servers...
    a.resize( r );

    Q_UINT16 aid = (((Q_UINT8)a[0]) << 8) + ((Q_UINT8)a[1]);
    uint i = 0;
    while( i < queries.size() &&
	   !( queries[i] && queries[i]->id == aid ) )
	i++;
    if ( i == queries.size() ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: bad id (0x%04x) %d", aid, i );
#endif
	return;
    }

    // at this point queries[i] is whatever we asked for.

    if ( ( (Q_UINT8)(a[2]) & 0x80 ) == 0 ) {
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: received a query" );
#endif
	return;
    }

    QDnsQuery * q = queries[i];
    queries.take( i );
    QDnsAnswer answer( a, q );
    answer.parse();
    answer.notify();
    if ( answer.ok )
	delete q;
    else
	queries.insert( i, q );
}


void QDnsManager::transmitQuery( QDnsQuery * query_ )
{
    if ( !query_ )
	return;

    uint i = 0;
    while( i < queries.size() && queries[i] != 0 )
	i++;
    if ( i == queries.size() )
	queries.resize( i+1 );
    queries.insert( i, query_ );
    transmitQuery( i );
}


void QDnsManager::transmitQuery( int i )
{
    if ( i < 0 || i >= (int)queries.size() )
	return;
    QDnsQuery * q = queries[i];

    if ( q && q->step > 8 ) {
	// okay, we've run out of retransmissions, let's kill it off and say
	// name-server-is-naughty
	queries.take( i );
#if defined(DEBUG_QDNS)
	qDebug( "DNS Manager: giving up on query 0x%04x", q->id );
#endif
	// we fake an NXDomain with a very short life time
	QDnsAnswer answer( q );
	answer.notify();
	delete q;
	QTimer::singleShot( 1000*10, QDnsManager::manager(), SLOT(cleanCache()) );
	// and don't process anything more
	return;
    }

    QByteArray p( 12 + q->l.length() + 2 + 4 );
    if ( p.size() > 500 )
	return; // way over the limit, so don't even try

    // header
    // id
    p[0] = (q->id & 0xff00) >> 8;
    p[1] =  q->id & 0x00ff;
    p[2] = 1; // recursion desired, rest is 0
    p[3] = 0; // all is 0
    // one query
    p[4] = 0;
    p[5] = 1;
    // no answers, name servers or additional data
    p[6] = p[7] = p[8] = p[9] = p[10] = p[11] = 0;

    // the name is composed of several components.  each needs to be
    // written by itself... so we write...
    // oh, and we assume that there's no funky characters in there.
    int pp = 12;
    uint lp = 0;
    while( lp < q->l.length() ) {
	int le = q->l.find( '.', lp );
	if ( le < 0 )
	    le = q->l.length();
	QString component = q->l.mid( lp, le-lp );
	p[pp++] = component.length();
	int cp;
	for( cp=0; cp < (int)component.length(); cp++ )
	    p[pp++] = component[cp].latin1();
	lp = le + 1;
    }
    // final null
    p[pp++] = 0;
    // query type
    p[pp++] = 0;
    switch( q->t ) {
    case QDns::A:
	p[pp++] = 1;
	break;
    case QDns::Aaaa:
	p[pp++] = 28;
	break;
    case QDns::Mx:
	p[pp++] = 15;
	break;
    case QDns::Srv:
	p[pp++] = 33;
	break;
    case QDns::Cname:
	p[pp++] = 5;
	break;
    case QDns::Ptr:
	p[pp++] = 12;
	break;
    case QDns::Txt:
	p[pp++] = 16;
	break;
    default:
	p[pp++] = (char)255; // any
	break;
    }
    // query class (always internet)
    p[pp++] = 0;
    p[pp++] = 1;

    if ( !ns || ns->isEmpty() )
	return;

    socket->writeBlock( p.data(), pp, *ns->at( q->step % ns->count() ), 53 );
#if defined(DEBUG_QDNS)
    qDebug( "issuing query 0x%04x (%d) about %s type %d to %s",
	    q->id, q->step, q->l.ascii(), q->t,
	    ns->at( q->step % ns->count() )->toString().ascii() );
#endif
    if ( ns->count() > 1 && q->step == 0 ) {
	// if it's the first time, send nonrecursive queries to the
	// other name servers too.
	p[2] = 0;
	QHostAddress * server;
	while( (server=ns->next()) != 0 ) {
	    socket->writeBlock( p.data(), pp, *server, 53 );
#if defined(DEBUG_QDNS)
	    qDebug( "copying query to %s", server->toString().ascii() );
#endif
	}
    }
    q->step++;
    // some testing indicates that normal dns queries take up to 0.6
    // seconds.  the graph becomes steep around that point, and the
    // number of errors rises... so it seems good to retry at that
    // point.
    q->start( q->step < ns->count() ? 600 : 1500, TRUE );
}


QDnsDomain * QDnsManager::domain( const QString & label )
{
    QDnsDomain * d = cache.find( label );
    if ( !d ) {
	d = new QDnsDomain( label );
	cache.insert( label, d );
    }
    return d;
}


//
//
// the QDnsDomain class looks after and coordinates queries for QDnsRRs for
// each domain, and the cached QDnsRRs.  (A domain, in DNS terminology, is
// a node in the DNS.  "no", "trolltech.com" and "lupinella.troll.no" are
// all domains.)
//
//


// this is ONLY to be called by QDnsManager::domain().  noone else.
QDnsDomain::QDnsDomain( const QString & label )
{
    l = label;
    rrs = 0;
}


QDnsDomain::~QDnsDomain()
{
    delete rrs;
    rrs = 0;
}


void QDnsDomain::add( const QString & label, QDnsRR * rr )
{
    QDnsDomain * d = QDnsManager::manager()->domain( label );
    if ( !d->rrs ) {
	d->rrs = new QList<QDnsRR>;
	d->rrs->setAutoDelete( TRUE );
    }
    d->rrs->append( rr );
    rr->domain = d;
}


QList<QDnsRR> * QDnsDomain::cached( const QDns * r )
{
    QList<QDnsRR> * l = new QList<QDnsRR>;

    // test at first if you have to start a query at all
    if ( r->recordType() == QDns::A ) {
	if ( r->label().lower() == "localhost" ) {
	    // undocumented hack:
	    QDnsRR *rrTmp = new QDnsRR( r->label() );
	    rrTmp->t = QDns::A;
	    rrTmp->address = QHostAddress( 0x7f000001 );
	    rrTmp->current = TRUE;
	    l->append( rrTmp );
	    return l;
	}
	QHostAddress tmp;
	if ( tmp.setAddress(r->label()) && tmp.isIp4Addr() ) {
	    QDnsRR *rrTmp = new QDnsRR( r->label() );
	    rrTmp->t = QDns::A;
	    rrTmp->address = tmp;
	    rrTmp->current = TRUE;
	    l->append( rrTmp );
	    return l;
	}
    }
    if ( r->recordType() == QDns::Aaaa ) {
	QHostAddress tmp;
	if ( tmp.setAddress(r->label()) && !tmp.isIp4Addr() ) {
	// ### if ( tmp.setAddress(r->label()) && tmp.isIp6Addr() ) {
	// ### this would make also sense
	    QDnsRR *rrTmp = new QDnsRR( r->label() );
	    rrTmp->t = QDns::Aaaa;
	    rrTmp->address = tmp;
	    rrTmp->current = TRUE;
	    l->append( rrTmp );
	    return l;
	}
    }

    // if you reach this point, you have to do the query
    QDnsManager * m = QDnsManager::manager();
    QStringList n = r->qualifiedNames();
    QValueListIterator<QString> it = n.begin();
    QValueListIterator<QString> end = n.end();
    bool nxdomain;
    int cnamecount = 0;
    while( it != end ) {
	QString s = *it;
	it++;
	nxdomain = FALSE;
#if defined(DEBUG_QDNS)
	qDebug( "looking at cache for %s (%s %d)",
		s.ascii(), r->label().ascii(), r->recordType() );
#endif
	QDnsDomain * d = m->domain( s );
	if ( d->rrs )
	    d->rrs->first();
	QDnsRR * rr;
	bool answer = FALSE;
	while( d->rrs && (rr=d->rrs->current()) != 0 ) {
	    if ( rr->t == QDns::Cname && r->recordType() != QDns::Cname &&
		 !rr->nxdomain && cnamecount < 16 ) {
		// cname.  if the code is ugly, that may just
		// possibly be because the concept is.
#if defined(DEBUG_QDNS)
		qDebug( "found cname from %s to %s",
			r->label().ascii(), rr->target.ascii() );
#endif
		s = rr->target;
		d = m->domain( s );
		if ( d->rrs )
		    d->rrs->first();
		it = end;
		// we've elegantly moved over to whatever the cname
		// pointed to.  well, not elegantly.  let's remember
		// that we've done something, anyway, so we can't be
		// fooled into an infinte loop as well.
		cnamecount++;
	    } else {
		if ( rr->t == r->recordType() ) {
		    if ( rr->nxdomain )
			nxdomain = TRUE;
		    else
			answer = TRUE;
		    l->append( rr );
		    if ( rr->deleteTime <= lastSweep ) {
			// we're returning something that'll be
			// deleted soon.  we assume that if the client
			// wanted it twice, it'll want it again, so we
			// ask the name server again right now.
			QDnsQuery * query = new QDnsQuery;
			query->started = now();
			query->id = ++::id;
			query->t = rr->t;
			query->l = rr->domain->name();
			// note that here, we don't bother about
			// notification. but we do bother about
			// timeouts: we make sure to use high timeouts
			// and few tramsissions.
			query->step = ns->count();
			QObject::connect( query, SIGNAL(timeout()),
					  QDnsManager::manager(),
					  SLOT(retransmit()) );
			QDnsManager::manager()->transmitQuery( query );
		    }
		}
		d->rrs->next();
	    }
	}
	// if we found a positive result, return quickly
	if ( answer && l->count() ) {
#if defined(DEBUG_QDNS)
	    qDebug( "found %d records for %s",
		    l->count(), r->label().ascii() );
	    l->first();
	    while( l->current() ) {
		qDebug( "  type %d target %s address %s",
		       l->current()->t,
		       l->current()->target.latin1(),
		       l->current()->address.toString().latin1() );
		l->next();
	    }
#endif
	    l->first();
	    return l;
	}

#if defined(DEBUG_QDNS)
	if ( nxdomain )
	    qDebug( "found NXDomain %s", s.ascii() );
#endif

	if ( !nxdomain ) {
	    // if we didn't, and not a negative result either, perhaps
	    // we need to transmit a query.
	    uint q = 0;
	    while ( q < m->queries.size() &&
		    ( m->queries[q] == 0 ||
		      m->queries[q]->t != r->recordType() ||
		      m->queries[q]->l != s ) )
		q++;
	    // we haven't done it before, so maybe we should.  but
	    // wait - if it's an unqualified name, only ask when all
	    // the other alternatives are exhausted.
	    if ( q == m->queries.size() && ( s.find( '.' ) >= 0 ||
					     l->count() >= n.count()-1 ) ) {
		QDnsQuery * query = new QDnsQuery;
		query->started = now();
		query->id = ++::id;
		query->t = r->recordType();
		query->l = s;
		query->dns->replace( (void*)r, (void*)r );
		QObject::connect( query, SIGNAL(timeout()),
				  QDnsManager::manager(), SLOT(retransmit()) );
		QDnsManager::manager()->transmitQuery( query );
	    } else if ( q < m->queries.size() ) {
		// if we've found an earlier query for the same
		// domain/type, subscribe to its answer
		m->queries[q]->dns->replace( (void*)r, (void*)r );
	    }
	}
    }
    l->first();
    return l;
}


void QDnsDomain::sweep( Q_UINT32 thisSweep )
{
    if ( !rrs )
	return;

    QDnsRR * rr;
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	if ( !rr->deleteTime )
	    rr->deleteTime = thisSweep; // will hit next time around

#if defined(DEBUG_QDNS)
	qDebug( "QDns::sweep: %s type %d expires %u %u - %s / %s",
	       rr->domain->name().latin1(), rr->t,
	       rr->expireTime, rr->deleteTime,
	       rr->target.latin1(), rr->address.toString().latin1());
#endif
	if ( rr->current == FALSE ||
	     rr->t == QDns::None ||
	     rr->deleteTime <= thisSweep ||
	     rr->expireTime <= thisSweep )
	    rrs->remove();
	else
	    rrs->next();
    }

    if ( rrs->isEmpty() ) {
	delete rrs;
	rrs = 0;
    }
}




// the itsy-bitsy little socket class I don't really need except for
// so I can subclass and reimplement the slots.


QDnsSocket::QDnsSocket( QObject * parent, const char * name )
    : QObject( parent, name )
{
    // nothing
}


QDnsSocket::~QDnsSocket()
{
    // nothing
}


void QDnsSocket::cleanCache()
{
    // nothing
}


void QDnsSocket::retransmit()
{
    // nothing
}


void QDnsSocket::answer()
{
    // nothing
}


/*! \class QDns qdns.h

  \brief The QDns class provides asynchronous DNS lookups.

  \module network

  Both Windows and UNIX provides synchronous DNS lookups; Windows
  provides some asynchronous support too.  Neither OS provides
  asynchronous support for anything other than hostname-to-address
  mapping.

  QDns rectifies that, by providing asynchronous caching lookups for
  the record types that we expect modern GUI applications to need in
  the near future.

  The class is a bit hard to use (although much simpler than the
  native APIs); QSocket provides much simpler TCP connection
  facilities.  The aim of QDns is to provide a correct and small API
  to the DNS: Nothing more.  (Correctness implies that the DNS
  information is correctly cached, and correctly timed out.)

  The API is made up of a constructor, functions to set the DNS node
  (the domain in DNS terminology) and record type (setLabel() and
  setRecordType()), the corresponding getters, an isWorking() function
  to determine whether QDns is working or reading, a resultsReady()
  signal, and finally query functions for the result.

  There is one query function for each RecordType, namely addresses(),
  mailServers(), servers(), hostNames() and texts(). There are also two
  generic query functions: canonicalName() return the name you'll presumably
  end up using (the exact meaning of that depends on the record type)
  and qualifiedNames() returns a list of the fully qualified names
  label() maps to.

  \sa QSocket
*/

/*!
  Constructs a DNS query object with invalid settings both for the
  label and the search type.
*/

QDns::QDns()
{
    d = new QDnsPrivate;
    t = None;
}




/*!
  Constructs a DNS query object that will return \a rr
  information about \a label.

  The DNS lookup is started the next time the application enters the event
  loop. When the result is found the signal resultsReady() is emmitted.

  \a rr defaults to \c A, IPv4 addresses.
*/

QDns::QDns( const QString & label, QDns::RecordType rr )
{
    d = new QDnsPrivate;
    t = rr;
    setLabel( label );
    setStartQueryTimer(); // start query the next time we enter event loop
}



/*!
  Constructs a DNS query object that will return \a rr information about
  \a address.  The label is set to the IN-ADDR.ARPA domain name. This is useful
  in combination with the Ptr record type (i.e. if you want to look up a
  hostname for a given address).

  The DNS lookup is started the next time the application enters the event
  loop. When the result is found the signal resultsReady() is emmitted.

  \a rr defaults to \c Ptr, that maps addresses to hostnames.
*/

QDns::QDns( const QHostAddress & address, QDns::RecordType rr )
{
    d = new QDnsPrivate;
    t = rr;
    setLabel( address );
    setStartQueryTimer(); // start query the next time we enter event loop
}




/*! Destroys the query object and frees its allocated resources. */

QDns::~QDns()
{
    uint q = 0;
    QDnsManager * m = QDnsManager::manager();
    while( q < m->queries.size() ) {
	QDnsQuery * query=m->queries[q];
	if ( query && query->dns )
	    (void)query->dns->take( (void*) this );
	q++;
    }

    delete d;
    d = 0;
}




/*!
  Sets this query object to query for information about \a label.

  This does not change the recordType(), but its isWorking() most
  likely changes as a result.

  The DNS lookup is started the next time the application enters the event
  loop. When the result is found the signal resultsReady() is emmitted.
*/

void QDns::setLabel( const QString & label )
{
    l = label;
    n.clear();

    if ( l.length() > 1 && l[(int)l.length()-1] == '.' ) {
	n.append( l.left( l.length()-1 ).lower() );
    } else {
	int i = l.length();
	int dots = 0;
	const int maxDots = 2;
	while( i && dots < maxDots ) {
	    if ( l[--i] == '.' )
		dots++;
	}
	if ( dots < maxDots ) {
	    (void)QDnsManager::manager();
	    QStrListIterator it( *domains );
	    const char * dom;
	    while( (dom=it.current()) != 0 ) {
		++it;
		n.append( l.lower() + "." + dom );
	    }
	}
	n.append( l.lower() );
    }
    setStartQueryTimer(); // start query the next time we enter event loop
#if defined(DEBUG_QDNS)
    qDebug( "QDns::setLabel: %d address(es) for %s", n.count(), l.ascii() );
    int i = 0;
    for( i = 0; i < (int)n.count(); i++ )
	qDebug( "QDns::setLabel: %d: %s", i, n[i].ascii() );
#endif
}


/*!
  Sets this query object to query for information about the address \a address.
  The label is set to the IN-ADDR.ARPA domain name. This is useful in
  combination with the Ptr record type (i.e. if you want to look up a hostname
  for a given address.

  This does not change the recordType(), but its isWorking() most
  likely changes as a result.

  The DNS lookup is started the next time the application enters the event
  loop. When the result is found the signal resultsReady() is emmitted.
*/

void QDns::setLabel( const QHostAddress & address )
{
    setLabel( toInAddrArpaDomain( address ) );
}


/*!
  \fn QStringList QDns::qualifiedNames() const

  Returns a list of the fully qualified names label() maps to.
*/


/*! \fn QString QDns::label() const

  Returns the domain name for which this object returns information.

  \sa setLabel()
*/

/*! \enum QDns::RecordType

  This enum type defines the record types QDns can handle.  The DNS
  provides many more; these are the ones we've judged to be in current
  use, useful for GUI programs and important enough to support right
  away:

  <ul>

  <li> \c None - no information.  This exists only so that QDns can
  have a default.

  <li> \c A - IPv4 addresses.  By far the most common type.

  <li> \c Aaaa - Ipv6 addresses.  So far mostly unused.

  <li> \c Mx - Mail eXchanger names.  Used for mail delivery.

  <li> \c Srv - SeRVer names.  Generic record type for finding
  servers.  So far mostly unused.

  <li> \c Cname - canonical name.  Maps from nicknames to the true
  name (the canonical name) for a host.

  <li> \c Ptr - name PoinTeR.  Maps from IPv4 or IPv6 addresses to hostnames.

  <li> \c Txt - arbitrary text for domains.

  </ul>

  We expect that some support for the
  <a href="http://www.dns.net/dnsrd/rfc/rfc2535.html">RFC-2535</a>
  extensions will be added in future versions.
*/

/*!
  Sets this object to query for \a rr records.

  The DNS lookup is started the next time the application enters the event
  loop. When the result is found the signal resultsReady() is emmitted.

  \sa RecordType
*/

void QDns::setRecordType( RecordType rr )
{
    t = rr;
    setStartQueryTimer(); // start query the next time we enter event loop
}

/*!
  Private slot for starting the query.
*/
void QDns::startQuery()
{
    // isWorking() starts the query (if necessary)
    if ( !isWorking() ) {
	emit resultsReady();
    }
    d->startQueryTimer = FALSE;
}

/*!
  The three functions QDns::QDns( QString, RecordType ), QDns::setLabel()
  and QDns::setRecordType() may start a DNS lookup. This function handles
  setting up the single shot timer.
*/
void QDns::setStartQueryTimer()
{
    if ( !d->startQueryTimer ) {
	// start the query the next time we enter event loop
	QTimer::singleShot( 0, this, SLOT(startQuery()) );
	d->startQueryTimer = TRUE;
    }
}

/*!
  Transform a host address to the IN-ADDR.ARPA domain name.
*/
QString QDns::toInAddrArpaDomain( const QHostAddress &address )
{
    if ( address.isIp4Addr() ) {
	Q_UINT32 i = address.ip4Addr();
	QString s;
	s.sprintf( "%d.%d.%d.%d.IN-ADDR.ARPA",
		i & 0xff, (i >> 8) & 0xff, (i>>16) & 0xff, (i>>24) & 0xff );
	return s;
    } else {
	qWarning( "QDns: IPv6 addresses not supported for this operation yet" );
	return QString::null;
    }
}


/*!
  \fn QDns::RecordType QDns::recordType() const

  Returns the record type of this query object.

  \sa setRecordType() RecordType
*/

/*!
  \fn void QDns::resultsReady()

  This signal is emitted when results are available for one of
  the qualifiedNames().
*/

/*!
  Returns TRUE if QDns is doing a lookup for this object, and FALSE
  if this object has the information it wants.

  QDns emits the resultsReady() signal when the status changes to FALSE.
*/

bool QDns::isWorking() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::isWorking (%s, %d)", l.ascii(), t );
#endif
    if ( t == None )
	return FALSE;

    QList<QDnsRR> * ll = QDnsDomain::cached( this );
    int queries = n.count();
    while( ll->current() != 0 ) {
	if ( ll->current()->nxdomain )
	    queries--;
	else
	    return FALSE;
	ll->next();
    }

    if ( queries <= 0 )
	return FALSE;
    return TRUE;
}


/*!
  Returns a list of the addresses for this name if this QDns object
  has a recordType() of \a QDns::A or \a QDns::Aaaa and the answer is
  available, or an empty list else.

  As a special case, if label() is a valid numeric IP address, this function
  returns that address.
*/

QValueList<QHostAddress> QDns::addresses() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::addresses (%s)", l.ascii() );
#endif
    QValueList<QHostAddress> result;
    if ( t != A && t != Aaaa )
	return result;

    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain )
	    result.append( rr->address );
	cached->next();
    }
    delete cached;
    return result;
}


// ### the \fn in the documentation is not nice but qdoc wants it...
/*!
  \fn QValueList<MailServer> QDns::mailServers() const

  Returns a list of mail servers if the record type is \c Mx. The struct
  \c QDns::MailServer contains the following variables:
  <ul>
  <li> \c QString QDns::MailServer::name
  <li> \c Q_UINT16 QDns::MailServer::priority
  </ul>
*/
QValueList<QDns::MailServer> QDns::mailServers() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::mailServers (%s)", l.ascii() );
#endif
    QValueList<QDns::MailServer> result;
    if ( t != Mx )
	return result;

    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    MailServer ms( rr->target, rr->priority );
	    result.append( ms );
	}
	cached->next();
    }
    delete cached;
    return result;
}


// ### the \fn in the documentation is not nice but qdoc wants it...
/*!
  \fn QValueList<Server> QDns::servers() const

  Returns a list of servers if the record type is \c Srv. The struct \c
  QDns::Server contains the following variables:
  <ul>
  <li> \c QString QDns::Server::name
  <li> \c Q_UINT16 QDns::Server::priority
  <li> \c Q_UINT16 QDns::Server::weight
  <li> \c Q_UINT16 QDns::Server::port
  </ul>
*/
QValueList<QDns::Server> QDns::servers() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::servers (%s)", l.ascii() );
#endif
    QValueList<QDns::Server> result;
    if ( t != Srv )
	return result;

    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    Server s( rr->target, rr->priority, rr->weight, rr->port );
	    result.append( s );
	}
	cached->next();
    }
    delete cached;
    return result;
}


// #### QStringList or QString as return value?
/*!
  Returns a list of host names if the record type is \c Ptr.
*/
QStringList QDns::hostNames() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::hostNames (%s)", l.ascii() );
#endif
    QStringList result;
    if ( t != Ptr )
	return result;

    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    QString str( rr->target );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
  Returns a list of texts if the record type is \c Txt.
*/
QStringList QDns::texts() const
{
#if defined(DEBUG_QDNS)
    qDebug( "QDns::texts (%s)", l.ascii() );
#endif
    QStringList result;
    if ( t != Txt )
	return result;

    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    QString str( rr->text );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
  Returns the canonical name for this DNS node.  (This works
  regardless of what recordType() is set to.)

  If the canonical name isn't known, this function returns a null
  string.

  The canonical name of a DNS node is its full name, or the full name of
  the target of its CNAME.  For example, if l.trolltech.com is a CNAME to
  lupinella.troll.no, and the search path for QDns is "trolltech.com", then
  the canonical name for all of "lupinella", "l", "lupinella.troll.no."
  and "l.trolltech.com" is "lupinella.troll.no.".
*/

QString QDns::canonicalName() const
{
    QList<QDnsRR> * cached = QDnsDomain::cached( this );

    QDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain && rr->domain ) {
	    delete cached;
	    return rr->target;
	}
	cached->next();
    }
    delete cached;
    return QString::null;
}


#if defined(_OS_UNIX_)

// include this stuff late, so any defines won't hurt.  funkily,
// struct __res_state is part of the api.  normally not used, it says.
// but we use it, to get various information.

#include <sys/types.h>
#include <netinet/in.h>
#if defined (_OS_SCO_) || defined (_OS_AIX_) || defined(_OS_FREEBSD_)
# define class c_class
#endif
#include <arpa/nameser.h>
#if defined (_OS_SCO_) || defined (_OS_AIX_) || defined(_OS_FREEBSD_)
# undef class
#endif
#include <resolv.h>

#if defined (_OS_SOLARIS_) || defined (_OS_HPUX_)
// According to changelog 23685, this was introduced to fix a problem under
// Solaris. According to qt-bugs/arc-09/19264 it's needed for HPUX as well.
extern "C" int res_init();
#endif

// if various defines aren't there, we'll set them safely.

#if !defined(MAXNS)
#define MAXNS 1
#endif

static void doResInit()
{
    if ( ns )
	return;
    ns = new QList<QHostAddress>;
    ns->setAutoDelete( TRUE );
    domains = new QStrList( TRUE );
    domains->setAutoDelete( TRUE );

    res_init();
    int i;
    // find the name servers to use
    for( i=0; i < MAXNS && i < _res.nscount; i++ ) {
	ns->append( new QHostAddress(
		             ntohl( _res.nsaddr_list[i].sin_addr.s_addr ) ) );
    }
#if defined(MAXDFLSRCH)
    for( i=0; i < MAXDFLSRCH; i++ )
	if ( _res.dnsrch[i] && *(_res.dnsrch[i]) )
	    domains->append( QString::fromLatin1( _res.dnsrch[i] ).lower() );
#endif
    if ( *_res.defdname )
	domains->append( QString::fromLatin1( _res.defdname ).lower() );
#if defined(SANE_OPERATING_SYSTEM)
    // never defined, for obvious reasons, but should be
    res_close();
#endif



    QFile hosts( QString::fromLatin1( "/etc/hosts" ) );
    if ( hosts.open( IO_ReadOnly ) ) {
	// read the /etc/hosts file, creating long-life A and PTR RRs
	// for the things we find.
	QTextStream i( &hosts );
	QString line;
	while( !i.atEnd() ) {
	    line = i.readLine().simplifyWhiteSpace().lower();
	    uint n = 0;
	    while( n < line.length() && line[(int)n] != '#' )
		n++;
	    line.truncate( n );
	    n = 0;
	    while( n < line.length() && !line[(int)n].isSpace() )
		n++;
	    QString ip = line.left( n );
	    QHostAddress a;
	    a.setAddress( ip );
	    if ( a.isIp4Addr() ) {
		bool first = TRUE;
		line = line.mid( n+1 );
		n = 0;
		while( n < line.length() && !line[(int)n].isSpace() )
		    n++;
		QString hostname = line.left( n );
		// ### in case of bad syntax, hostname is invalid. do we care?
		if ( n ) {
		    QDnsRR * rr = new QDnsRR( hostname );
		    rr->t = QDns::A;
		    rr->address = a;
		    rr->deleteTime = UINT_MAX;
		    rr->expireTime = UINT_MAX;
		    rr->current = TRUE;
		    if ( first ) {
			first = FALSE;
			QString arpa;
			// ### maybe this should go in QHostAddress?
			arpa.sprintf( "%d.%d.%d.%d.in-addr.arpa",
				      ( a.ip4Addr() >> 0 ) & 0xff,
				      ( a.ip4Addr() >> 8 ) & 0xff,
				      ( a.ip4Addr() >>16 ) & 0xff,
				      ( a.ip4Addr() >>24 ) & 0xff );
			QDnsRR * ptr = new QDnsRR( arpa );
			ptr->t = QDns::Ptr;
			ptr->target = hostname;
			ptr->deleteTime = UINT_MAX;
			ptr->expireTime = UINT_MAX;
			ptr->current = TRUE;
		    }
		}
	    }
	}
    }
}

#elif defined(_OS_WIN32_)

#include <windows.h>

// the following typedefs are needed for GetNetworkParams() API call
#ifndef IP_TYPES_INCLUDED
#define MAX_HOSTNAME_LEN    128
#define MAX_DOMAIN_NAME_LEN 128
#define MAX_SCOPE_ID_LEN    256
typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;
typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;
typedef struct {
    char HostName[MAX_HOSTNAME_LEN + 4] ;
    char DomainName[MAX_DOMAIN_NAME_LEN + 4];
    PIP_ADDR_STRING CurrentDnsServer;
    IP_ADDR_STRING DnsServerList;
    UINT NodeType;
    char ScopeId[MAX_SCOPE_ID_LEN + 4];
    UINT EnableRouting;
    UINT EnableProxy;
    UINT EnableDns;
} FIXED_INFO, *PFIXED_INFO;
#endif
typedef DWORD (WINAPI *GNP)( PFIXED_INFO, PULONG );

//
// We need to get information about DNS etc. from the Windows
// registry.  We don't worry about Unicode strings here.
//

static QString getWindowsRegString( HKEY key, const char *subKey )
{
    QString s;
    char  buf[512];
    DWORD bsz = sizeof(buf);
    int r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)buf, &bsz );
    if ( r == ERROR_SUCCESS ) {
	s = buf;
    } else if ( r == ERROR_MORE_DATA ) {
	char *ptr = new char[bsz+1];
	r = RegQueryValueExA( key, subKey, 0, 0, (LPBYTE)ptr, &bsz );
	if ( r == ERROR_SUCCESS )
	    s = ptr;
	delete [] ptr;
    }
    return s;
}


static void doResInit()
{
    char separator;

    if ( ns )
	return;
    ns = new QList<QHostAddress>;
    ns->setAutoDelete( TRUE );
    domains = new QStrList( TRUE );
    domains->setAutoDelete( TRUE );

    QString domainName, nameServer, searchList;
    HKEY k1, k2;

    bool gotNetworkParams = FALSE;
    if ( QApplication::winVersion() == Qt::WV_98 ||
	 QApplication::winVersion() == Qt::WV_2000 ) {
	// for 98 and 2000 try the API call GetNetworkParams()
	HINSTANCE hinstLib = LoadLibraryA( "iphlpapi" ); // we don't need Unicode here
	if ( hinstLib != 0 ) {
	    GNP getNetworkParams = (GNP) GetProcAddress( hinstLib, "GetNetworkParams" );
	    if ( getNetworkParams != 0 ) {
		ULONG l = 0;
		DWORD res;
		res = getNetworkParams( 0, &l );
		if ( res == ERROR_BUFFER_OVERFLOW ) {
		    FIXED_INFO *finfo = (FIXED_INFO*)new char[l];
		    res = getNetworkParams( finfo, &l );
		    if ( res == ERROR_SUCCESS ) {
			domainName = finfo->DomainName;
			nameServer = "";
			IP_ADDR_STRING *dnsServer = &finfo->DnsServerList;
			while ( dnsServer != 0 ) {
			    nameServer += dnsServer->IpAddress.String;
			    dnsServer = dnsServer->Next;
			    if ( dnsServer != 0 )
				nameServer += " ";
			}
			searchList = "";
			separator = ' ';
			gotNetworkParams = TRUE;
		    }
		    delete[] finfo;
		}
	    }
	    FreeLibrary( hinstLib );
	}
	if ( !gotNetworkParams )
	    qWarning( "QDns: call GetNetworkParams() was unsuccessful!" );
    }
    if ( !gotNetworkParams ) {
	// this is for NT
	int r = RegOpenKeyExA( HKEY_LOCAL_MACHINE,
			       "System\\CurrentControlSet\\Services\\Tcpip\\"
			       "Parameters",
			       0, KEY_READ, &k1 );
	if ( r == ERROR_SUCCESS ) {
	    domainName = getWindowsRegString( k1, "Domain" );
	    nameServer = getWindowsRegString( k1, "NameServer" );
	    searchList = getWindowsRegString( k1, "SearchList" );
	    separator = ' ';
	} else {
	    // this is for 95/98
	    int r = RegOpenKeyExA( HKEY_LOCAL_MACHINE,
				   "System\\CurrentControlSet\\Services\\VxD\\"
				   "MSTCP",
				   0, KEY_READ, &k2 );
	    if ( r == ERROR_SUCCESS ) {
		domainName = getWindowsRegString( k2, "Domain" );
		nameServer = getWindowsRegString( k2, "NameServer" );
		searchList = getWindowsRegString( k2, "SearchList" );
		separator = ',';
	    } else {
		// Could not access the TCP/IP parameters
		domainName = "";
		nameServer = "127.0.0.1";
		searchList = "";
		separator = ' ';
	    }
	    RegCloseKey( k2 );
	}
	RegCloseKey( k1 );
    }

    nameServer = nameServer.simplifyWhiteSpace();
    int first, last;
    first = 0;
    do {
	last = nameServer.find( separator, first );
	if ( last < 0 )
	    last = nameServer.length();
	QDns tmp( nameServer.mid( first, last-first ), QDns::A );
	QValueList<QHostAddress> address = tmp.addresses();
	int i = address.count();
	while( i )
	    ns->append( new QHostAddress(address[--i]) );
	first = last+1;
    } while( first < (int)nameServer.length() );

    searchList = searchList + " " + domainName;
    searchList = searchList.simplifyWhiteSpace().lower();
    first = 0;
    do {
	last = searchList.find( separator, first );
	if ( last < 0 )
	    last = searchList.length();
	domains->append( qstrdup( searchList.mid( first, last-first ) ) );
	first = last+1;
    } while( first < (int)searchList.length() );
}

#endif

#endif
