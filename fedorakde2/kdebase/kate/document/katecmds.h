
#ifndef _KATECMDS_H
#define _KATECMDS_H

#include "katecmd.h"


/**
 * this namespace will be maintained by Charles Samuels <charles@kde.org>
 * so we're going to be using this indentation style here.
 *
 * Respect my style, and I'll respect your's!
 **/
namespace KateCommands
{

/**
 * This is by Christoph Cullmann
 **/
class InsertTime : public KateCmdParser
{
public:
	InsertTime(KateDocument *doc=0) : KateCmdParser(doc) { }

	bool execCmd(QString cmd=0, KateView *view=0);
};

/**
 * -- Charles Samuels <charles@kde.org> 
 * Support vim/sed find and replace
 * s/search/replace/ find search, replace with replace on this line
 * %s/search/replace/ do the same to the whole file
 * s/search/replace/i do the S. and R., but case insensitively
 * $s/search/replace/ do the search are replacement to the selection only
 *
 * $s/// is currently unsupported
 **/
class SedReplace : public KateCmdParser
{
public:
	SedReplace(KateDocument *doc=0) : KateCmdParser(doc) { }

	bool execCmd(QString cmd=0, KateView *view=0);
private:
	static QString sedMagic(QString textLine, QString find, QString replace, bool noCase, bool repeat);
};

/**
 * insert a unicode or ascii character
 * base 9+1: 1234
 * hex: 0x1234 or x1234
 * octal: 01231
 *
 * prefixed with "char:"
 **/
class Character : public KateCmdParser
{
public:
	Character(KateDocument *doc=0) : KateCmdParser(doc) { }

	bool execCmd(QString cmd=0, KateView *view=0);
};

class Fifo : public KateCmdParser
{
public:
	Fifo(KateDocument *doc=0) : KateCmdParser(doc) { }

	bool execCmd(QString cmd=0, KateView *view=0);
};

}

// vim: noet

#endif

