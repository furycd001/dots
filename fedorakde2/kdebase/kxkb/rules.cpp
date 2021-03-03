#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>


#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <config.h>

#include "rules.h"


KeyRules::KeyRules(QString rule, QString path)
{
  // try to load the right files
  if (!path.isEmpty())
    {
      loadRules(QString("%1/%2.lst").arg(path).arg(rule));
      loadRules(QString("%1/%2-%3.lst").arg(path).arg(rule).arg(KGlobal::locale()->language()));
    }
  
  loadRules(QString("/usr/X11R6/lib/X11/xkb/rules/%1.lst").arg(rule));
  loadRules(QString("/usr/X11R6/lib/X11/xkb/rules/%1-%2.lst").arg(rule).arg(KGlobal::locale()->language()));

  loadEncodings(QString("/usr/X11R6/lib/X11/locale/locale.alias"));
}


void KeyRules::loadRules(QString file)
{
  QFile f(file);
  if (f.open(IO_ReadOnly))
    {
      QTextStream ts(&f);

      QString line;
      bool model=false, layout=false;
      while (!ts.eof())
	{
	  line = ts.readLine().stripWhiteSpace();

	  if (!model && line.left(7) == "! model")
	    {
	      model = true;
	      continue;
	    }
	  if (!layout && line.left(8) == "! layout")
	    {
	      layout = true;
	      continue;
	    }

	  if (model)
	    if (line.isEmpty())
	      model = false;
	    else
	      {
		int pos = line.find(QRegExp("\\s"));
		if (pos > 0)		  
		  {
		    _models.remove(line.left(pos));		    
		    _models.insert(line.left(pos), strdup(line.mid(pos).stripWhiteSpace().latin1()));
		  }
	      }
	  
	  if (layout)
	    if (line.isEmpty())
	      layout = false;
	    else
	      {
		int pos = line.find(QRegExp("\\s"));
		if (pos > 0)
		  {
		    _layouts.remove(line.left(pos));
		    _layouts.insert(line.left(pos), strdup(line.mid(pos).stripWhiteSpace().latin1()));
		  }
	      }
	}

      f.close();
    }
}

// some handcoded ones, because the X11 locale file doesn't get them correctly, or in case
// the locale file wasn't found
static struct {
    const char * locale;
    const char * encoding;
    unsigned int group;
} encs[] = {
    { "be", "ISO8859-1", 0 },
    { "br", "ISO8859-1", 0 },
    { "bg", "ISO8859-5", 1 },
    { "ca", "ISO8859-1", 0 },
    { "cs", "ISO8859-2", 0 },
    { "cz", "ISO8859-2", 0 },
    { "de", "ISO8859-1", 0 },
    { "de_CH", "ISO8859-1", 0 },
    { "dk", "ISO8859-1", 0 },
    { "ee", "ISO8859-15", 0 },
    { "en_US", "ISO8859-1", 0 },
    { "el", "ISO8859-7", 1 },
    { "es", "ISO8859-1", 0 },
    { "eo", "ISO8859-3", 0 },
    { "fi", "ISO8859-1", 0 },
    { "fr", "ISO8859-1", 0 },
    { "fr_CH", "ISO8859-1", 0 },
    { "he", "ISO8859-8-i", 1 },
    { "hu", "ISO8859-2", 0 },
    { "hr", "ISO8859-2", 0 },
    { "il", "ISO8859-8-i", 1 },
    { "it", "ISO8859-1", 0 },
    { "kl", "ISO8859-1", 0 },
    { "lt", "ISO8859-13", 1 },
    { "mk", "ISO8859-5", 1 },
    { "nl", "ISO8859-1", 0 },
    { "no", "ISO8859-1", 0 },
    { "pl", "ISO8859-2", 0 },
    { "pt", "ISO8859-1", 0 },
    { "ro", "ISO8859-2", 0 },
    { "ru", "KOI8-R", 1 },
    { "ru_UA", "KOI8-U", 0 },
    { "se", "ISO8859-1", 0 },
    { "sk", "ISO8859-2", 0 },
    { "th", "ISO8859-11", 1 },
    { "us", "ISO8859-1", 0 },
    { "ua", "KOI8-U", 1 },
    { "uk_UA", "KOI8-U", 1 },
    { 0, 0, 0 }
};

void KeyRules::loadEncodings(QString file)
{
  QFile f(file);
  if (f.open(IO_ReadOnly))
    {
      QTextStream ts(&f);

      QString line;
      while (!ts.eof()) {
	  line = ts.readLine().simplifyWhiteSpace();

	    if (line[0] == '#' || line.isEmpty())
		continue;

	    int pos = line.find(' ');
	    if (pos > 0) {
		_encodings.remove(line.left(pos));
		int pos2 = line.find('.', pos);
		_encodings.insert(line.left(pos), strdup(line.mid(pos2+1).stripWhiteSpace().latin1()));
	    }
      }

      f.close();
    }

  int i = 0;
  while ( encs[i].encoding != 0 ) {
      _encodings.remove(encs[i].locale);
      _encodings.insert(encs[i].locale, encs[i].encoding);
      _group.insert(encs[i].locale, &encs[i].group);
      i++;
  }
}


QStringList KeyRules::rules(QString path)
{
  QStringList result;

  if (path.isEmpty())
    path = "/usr/X11R6/lib/X11/xkb/rules";
  
  QDir dir(path);
  dir.setFilter(QDir::Files);
  QStringList list = dir.entryList();
  for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
    if ((*it).right(4) != ".lst")
      result << *it;
  
  return result;
}
