#include "katecmds.h"
#include <qregexp.h>
#include <qregexp3.h>
#include <qdatetime.h>
#include "katedocument.h"
#include <kdebug.h>

namespace KateCommands
{


bool InsertTime::execCmd(QString cmd, KateView *view)
{
	if (cmd.left(5) == "time")
	{
		view->insertText(QTime::currentTime().toString());
		return true;
	}

	return false;
}

static void replace(QString &s, const QString &needle, const QString &with)
{
	int pos=0;
	while (1)
	{
		pos=s.find(needle, pos);
		if (pos==-1) break;
		s.replace(pos, needle.length(), with);
		pos+=with.length();
	}

}

// stolen from QString::replace
static void replace(QString &s, QRegExp3 &rx, const QString &with)
{
	if (s.isEmpty()) return;
	int index = 0;
	int slen = with.length();
	int len;
	while (index < (int)s.length())
	{
		index = rx.search(s, index);
		len = rx.matchedLength();
		if ( index >= 0 )
		{
			s.replace(index, len, with);
			index += slen;
			if (!len)
				break;  // Avoid infinite loop on 0-length matches, e.g. [a-z]*
		}
		else
			break;
	}
}



static int backslashString(const QString &haystack, const QString &needle, int index)
{
	int len=haystack.length();
	int searchlen=needle.length();
	bool evenCount=true;
	while (index<len)
	{
		if (haystack[index]=='\\')
		{
			evenCount=!evenCount;
		}
		else
		{  // isn't a slash
			if (!evenCount)
			{
				if (haystack.mid(index, searchlen)==needle)
					return index-1;
			}
			evenCount=true;
		}
		index++;

	}
	
	return -1;
}


// exchange "\t" for the actual tab character, for example
static void exchangeAbbrevs(QString &str)
{
	// the format is (findreplace)*[nullzero]
	char *magic="a\x07t\t";

	while (*magic)
	{
		int index=0;
		char replace=magic[1];
		while ((index=backslashString(str, QChar(*magic), index))!=-1)
		{
			str.replace(index, 2, QChar(replace));
			index++;
		}
		magic++;
		magic++;
	}
}

QString SedReplace::sedMagic(QString textLine, QString find, QString rep, bool noCase, bool repeat)
{
	QRegExp3 matcher(find, noCase);

	int start=0;
	while (start!=-1)
	{
		start=matcher.search(textLine, start);
		
		if (start==-1) break;
		
		int length=matcher.matchedLength();


		// now set the backreferences in the replacement
		QStringList backrefs=matcher.capturedTexts();
		int refnum=1;

		QStringList::Iterator i = backrefs.begin();
		++i;

		for (; i!=backrefs.end(); ++i)
		{
			// I need to match "\\" or "", but not "\"
			QString number=QString::number(refnum);

			int index=0;
			while (index!=-1)
			{
				index=backslashString(rep, number, index);
				if (index>=0)
				{
					rep.replace(index, 2, *i);
					index+=(*i).length();
				}
			}
			
			refnum++;
		}

		textLine.replace(start, length, rep);
		if (!repeat) break;
		start+=rep.length();
	}

	replace(textLine, "\\\\", "\\");
	replace(textLine, "\\/", "/");
	
	return textLine;
}
	
	
static void setLineText(KateView *view, int line, const QString &text)
{
//	view->doc()->removeLine(line);
//	view->doc()->insertLine(text, line);
	view->doc()->replaceLine(text,line);
}

bool SedReplace::execCmd(QString cmd, KateView *view)
{
	kdDebug(13010)<<"SedReplace::execCmd()"<<endl;
	if (QRegExp("[$%]?s/.+/.*/[ig]*").find(cmd, 0)==-1)
		return false;
	
	bool fullFile=cmd[0]=='%';
	bool noCase=cmd[cmd.length()-1]=='i' || cmd[cmd.length()-2]=='i';
	bool repeat=cmd[cmd.length()-1]=='g' || cmd[cmd.length()-2]=='g';
	bool onlySelect=cmd[0]=='$';

	QRegExp3 splitter("^[$%]?s/((?:[^\\\\/]|\\\\[\\\\/\\$0-9tadDsSwW])*)/((?:[^\\\\/]|\\\\[\\\\/\\$0-9tadDsSwW])*)/[ig]*$");
	if (splitter.search(cmd)<0) return false;
	
	QString find=splitter.cap(1);
	kdDebug(13010)<< "SedReplace: find=" << find.latin1() <<endl;

	QString replace=splitter.cap(2);
	exchangeAbbrevs(replace);
	kdDebug(13010)<< "SedReplace: replace=" << replace.latin1() <<endl;
	
	
	if (fullFile)
	{
		int numLines=view->doc()->numLines();
		for (int line=0; line < numLines; line++)
		{
			QString text=view->textLine(line);
			text=sedMagic(text, find, replace, noCase, repeat);
			setLineText(view, line, text);
		}
	}
	else if (onlySelect)
	{
		// Not implemented
	}
	else
	{ // just this line
		QString textLine=view->currentTextLine();
		int line=view->currentLine();
		textLine=sedMagic(textLine, find, replace, noCase, repeat);
		setLineText(view, line, textLine);
	}
	return true;
}

bool Character::execCmd(QString cmd, KateView *view)
{
	// hex, octal, base 9+1
	QRegExp3 num("^char: *(0?x[0-9A-Fa-f]{1,4}|0[0-7]{1,6}|[0-9]{1,3})$");
	if (num.search(cmd)==-1) return false;

	cmd=num.cap(1);

	// identify the base
	
	unsigned short int number=0;
	int base=10;
	if (cmd[0]=='x' || cmd.left(2)=="0x")
	{
		cmd.replace(QRegExp("^0?x"), "");
		base=16;
	}
	else if (cmd[0]=='0')
		base=8;
	bool ok;
	number=cmd.toUShort(&ok, base);
	if (!ok || number==0) return false;
	if (number<=255)
	{
		char buf[2];
		buf[0]=(char)number;
		buf[1]=0;
		view->insertText(QString(buf));
	}
	else
	{ // do the unicode thing
		QChar c(number);
		view->insertText(QString(&c, 1));
	}
	
	return true;
}

bool Fifo::execCmd(QString cmd, KateView *view)
{

}

}

// vim: noet

