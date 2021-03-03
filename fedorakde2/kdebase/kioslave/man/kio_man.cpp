/*  This file is part of the KDE libraries
    Copyright (c) 2000 Matthias Hoelzer-Kluepfel <mhk@caldera.de>

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdict.h>
#include <qcstring.h>
#include <qlist.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kprocess.h>
#include <klocale.h>


#include "kio_man.h"
#include "kio_man.moc"
#include <zlib.h>
#include <man2html.h>
#include <assert.h>
#include <kfilterbase.h>
#include <kfilterdev.h>
#include <qmap.h>

using namespace KIO;

MANProtocol *MANProtocol::_self = 0;

bool parseUrl(const QString& _url, QString &title, QString &section)
{
    section = "";

    QString url = _url;
    if (url.at(0) == '/') {
        if (KStandardDirs::exists(url)) {
            title = url;
            return true;
        } else
            kdDebug(7107) << url << " does not exist" << endl;
    }

    while (url.at(0) == '/')
        url.remove(0,1);

    title = url;

    int pos = url.find('(');
    if (pos < 0)
        return true;

    title = title.left(pos);

    section = url.mid(pos+1);
    section = section.left(section.length()-1);

    return true;
}


MANProtocol::MANProtocol(const QCString &pool_socket, const QCString &app_socket)
    : QObject(), SlaveBase("man", pool_socket, app_socket)
{
    assert(!_self);
    _self = this;
    common_dir = KGlobal::dirs()->findResourceDir( "html", "en/common/kde-common.css" );
}

MANProtocol *MANProtocol::self() { return _self; }

MANProtocol::~MANProtocol()
{
    _self = 0;
}

QStringList MANProtocol::findPages(const QString &section, const QString &title)
{
    checkManPaths();
    QStringList list;
    if (title.at(0) == '/') {
       list.append(title);
       return list;
    }

    QString mansection = "man*";
    if (!section.isEmpty())
        mansection = QString("man%1").arg(section);
    QStringList languages = KGlobal::locale()->languageList();
    for (QStringList::ConstIterator it = languages.begin(); it != languages.end(); ++it) {
        list += KGlobal::dirs()->findAllResources("manpath", QString("%1/%2/%3.*").arg(*it).arg(mansection).arg(title));
    }
    list += KGlobal::dirs()->findAllResources("manpath", QString("%1/%2.*").arg(mansection).arg(title));
    QStringList::Iterator it = list.begin();
    while (it != list.end()) {
        QString file = (*it).mid((*it).findRev('/') + 1);
        kdDebug() << file << endl;
//        assert(file[title.length()] == '.');
        if (file[title.length()] != '.')
        {
            it = list.remove(it);
            continue;
        };
        file = file.mid(title.length() + 1);
        if (!file[0].isNumber())
            it = list.remove(it);
        else
            ++it;
    }
    return list;
}

void MANProtocol::output(const char *insert)
{
    if (insert)
        output_string += insert;
    if (!insert || output_string.length() > 2000) {
        // TODO find out the language of the man page and put the right common dir in
        output_string.replace( QRegExp( "KDE_COMMON_DIR" ), QString( "file:%1/en/common" ).arg( common_dir ).local8Bit());
        //kdDebug(7107) << "output " << output_string << endl;
        data(output_string);
        output_string.truncate(0);
    }
}

// called by man2html
char *read_man_page(const char *filename)
{
    return MANProtocol::self()->readManPage(filename);
}

// called by man2html
void output_real(const char *insert)
{
    MANProtocol::self()->output(insert);
}

void MANProtocol::get(const KURL& url )
{
    kdDebug(7107) << "GET " << url.url() << endl;

    QString title, section;

    if (!parseUrl(url.path(), title, section))
    {
        showMainIndex();
        return;
    }

    // see if an index was requested
    if (url.query().isEmpty() && (title.isEmpty() || title == "/" || title == "."))
    {
        if (section == "index" || section.isEmpty())
            showMainIndex();
        else
            showIndex(section);
        return;
    }

    // tell the mimetype
    mimeType("text/html");

    QStringList foundPages=findPages(section, title);
    if (foundPages.count()==0)
    {
       outputError(i18n("No man page matching to %1 found.").arg(title));
    }
    else if (foundPages.count()>1)
    {
       outputMatchingPages(foundPages);
    }
    //yes, we found exactly one man page
    else
    {
       QCString filename=QFile::encodeName(foundPages[0]);

       char *buf = readManPage(filename);
       if (!buf)
       {
          outputError(i18n("Open of %1 failed.").arg(title));
          finished();
          return;
       }
       // will call output_real
       scan_man_page(buf);
       delete [] buf;

       output(0); // flush

       // tell we are done
       data(QByteArray());
    };
    finished();
}

char *MANProtocol::readManPage(const char *_filename)
{
    QCString filename = _filename;

    if (QDir::isRelativePath(filename)) {
        kdDebug(7107) << "relative " << filename << endl;
        filename = QDir::cleanDirPath(lastdir + "/" + filename).utf8();
        if (!KStandardDirs::exists(filename)) { // exists perhaps with suffix
            lastdir = filename.left(filename.findRev('/'));
            QDir mandir(lastdir);
            mandir.setNameFilter(filename.mid(filename.findRev('/') + 1) + ".*");
            filename = lastdir + "/" + QFile::encodeName(mandir.entryList().first());
        }
        kdDebug(7107) << "resolved to " << filename << endl;
    }
    lastdir = filename.left(filename.findRev('/'));

    QFile raw(filename);
    KFilterBase *f = KFilterBase::findFilterByFileName(filename);

    QIODevice *fd= KFilterDev::createFilterDevice(f,&raw);

    if (!fd->open(IO_ReadOnly))
    {
       delete f;
       delete fd;
       return 0;
    }
    char buffer[1025];
    int n;
    QCString text;
    while ( ( n = fd->readBlock(buffer, 1024) ) )
    {
        buffer[n] = 0;
        text += buffer;
    }
    kdDebug(7107) << "read " << text.length() << endl;
    fd->close();

    delete fd;
    delete f;

    int l = text.length();
    char *buf = new char[l + 4];
    memcpy(buf + 1, text.data(), l);
    buf[0]=buf[l]='\n';
    buf[l+1]=buf[l+2]='\0';

    return buf;
}


void MANProtocol::outputError(const QString& errmsg)
{
    QCString output;

    QTextStream os(output, IO_WriteOnly);
    // QTextSream on a QCString needs to be told explicitely to use local8Bit conversion !
    os.setEncoding(QTextStream::Locale);

    os << "<html>" << endl;
    os << i18n("<head><title>Man output</title></head>") << endl;
    os << i18n("<body bgcolor=#ffffff><h1>KDE Man Viewer Error</h1>") << errmsg << "</body>" << endl;
    os << "</html>" << endl;

    data(output);
}

void MANProtocol::outputMatchingPages(const QStringList &matchingPages)
{
    QCString output;

    QTextStream os(output, IO_WriteOnly);
    os.setEncoding(QTextStream::Locale);

    os << "<html>\n<head><title>\n";
    os << i18n("Man output");
    os <<"</title></head>\n<body bgcolor=#ffffff><h1>";
    os << i18n("There is more than one matching man page.");
    os << "</h1>\n<ul>";
    for (QStringList::ConstIterator it = matchingPages.begin(); it != matchingPages.end(); ++it)
       os<<"<li><a href=man:"<<QFile::encodeName(*it)<<">"<< *it <<"</a><br>\n<br>\n";
    os<< "</ul>\n</body>\n</html>"<<endl;

    data(output);
    finished();
}

void MANProtocol::stat( const KURL& url)
{
    kdDebug(7107) << "ENTERING STAT " << url.url();

    QString title, section;

    if (!parseUrl(url.path(), title, section))
    {
        error(KIO::ERR_MALFORMED_URL, url.url());
        return;
    }

    kdDebug(7107) << "URL " << url.url() << " parsed to title='" << title << "' section=" << section << endl;


    UDSEntry entry;
    UDSAtom atom;

    atom.m_uds = UDS_NAME;
    atom.m_long = 0;
    atom.m_str = title;
    entry.append(atom);

    atom.m_uds = UDS_FILE_TYPE;
    atom.m_str = "";
    atom.m_long = S_IFREG;
    entry.append(atom);

    atom.m_uds = UDS_URL;
    atom.m_long = 0;
    QString newUrl = "man:"+title;
    if (!section.isEmpty())
        newUrl += QString("(%1)").arg(section);
    atom.m_str = newUrl;
    entry.append(atom);

    atom.m_uds = UDS_MIME_TYPE;
    atom.m_long = 0;
    atom.m_str = "text/html";
    entry.append(atom);

    statEntry(entry);

    finished();
}


extern "C"
{

    int kdemain( int argc, char **argv ) {

        KInstance instance("kio_man");

        kdDebug(7107) <<  "STARTING " << getpid() << endl;

        if (argc != 4)
        {
            fprintf(stderr, "Usage: kio_man protocol domain-socket1 domain-socket2\n");
            exit(-1);
        }

        MANProtocol slave(argv[2], argv[3]);
        slave.dispatchLoop();

        kdDebug(7107) << "Done" << endl;

        return 0;
    }

}

void MANProtocol::mimetype(const KURL & /*url*/)
{
    mimeType("text/html");
    finished();
}

QString sectionName(const QString& section)
{
    if (section == "1")
        return i18n("User Commands");
    else if (section == "2")
        return i18n("System Calls");
    else if (section == "3")
        return i18n("Subroutines");
    else if (section == "4")
        return i18n("Devices");
    else if (section == "5")
        return i18n("File Formats");
    else if (section == "6")
        return i18n("Games");
    else if (section == "7")
        return i18n("Miscellaneous");
    else if (section == "8")
        return i18n("System Administration");
    else if (section == "9")
        return i18n("Kernel");
    else if (section == "n")
        return i18n("New");

    return QString::null;
}


void MANProtocol::showMainIndex()
{
    QCString output;

    QTextStream os(output, IO_WriteOnly);
    // QTextSream on a QCString needs to be told explicitely to use local8Bit conversion !
    os.setEncoding(QTextStream::Locale);

    // print header
    os << "<html>" << endl;
    os << i18n("<head><title>UNIX Manual Index</title></head>") << endl;
    os << i18n("<body bgcolor=#ffffff><h1>UNIX Manual Index</h1>") << endl;

    QString sectList = getenv("MANSECT");
    if (sectList.isEmpty())
        sectList = "1:2:3:4:5:6:7:8:9:n";
    QStringList sections = QStringList::split(':', sectList);

    os << "<table>" << endl;

    QStringList::ConstIterator it;
    for (it = sections.begin(); it != sections.end(); ++it)
        os << "<tr><td><a href=\"man:(" << *it << ")\">" << i18n("Section ") << *it << "</a></td><td>&nbsp;</td><td> " << sectionName(*it) << "</td></tr>" << endl;

    os << "</table>" << endl;

    // print footer
    os << "</body></html>" << endl;

    data(output);
    finished();
}


void MANProtocol::checkManPaths()
{
    static bool inited = false;

    if (inited)
        return;

    inited = true;

    QStringList manPaths;

    // add paths from /etc/man.conf
    QRegExp manpath("^MANPATH\\s");
    QFile mc("/etc/man.conf");             // ???
    if (!mc.exists())
    	mc.setName("/etc/manpath.config"); // SuSE, Debian
    if (!mc.exists())
   	mc.setName("/etc/man.config");  // Mandrake
    if (mc.open(IO_ReadOnly))
    {
        QTextStream is(&mc);

        while (!is.eof())
	{
            QString line = is.readLine();
            if (manpath.find(line, 0) == 0)
	    {
                QString path = line.mid(8).stripWhiteSpace();
                KGlobal::dirs()->addResourceDir("manpath", path);
	    }
	}

        mc.close();
    }

    static const char *manpaths[] = {
                    "/usr/X11/man/",
                    "/usr/X11R6/man/",
                    "/usr/man/",
                    "/usr/local/man/",
                    "/usr/exp/man/",
                    "/usr/openwin/man/",
		    "/usr/dt/man/",
		    "/opt/freetool/man",
		    "/opt/local/man",
                    "/usr/tex/man/",
                    "/usr/www/man/",
                    "/usr/lang/man/",
                    "/usr/gnu/man/",
                    "/usr/share/man",
                    "/usr/motif/man/",
                    "/usr/titools/man/",
                    "/usr/sunpc/man/",
                    "/usr/ncd/man/",
                    "/usr/newsprint/man/",
                    NULL };

    int index = 0;
    while (manpaths[index]) {
        KGlobal::dirs()->addResourceDir("manpath", manpaths[index++]);
    }

    // add MANPATH paths
    QString envPath = getenv("MANPATH");
    if (!envPath.isEmpty()) {
        manPaths = QStringList::split(':', envPath);
        for (QStringList::ConstIterator it = manPaths.begin();
             it != manPaths.end(); ++it)
            KGlobal::dirs()->addResourceDir("manpath", *it);
    }

}


//#define _USE_OLD_CODE

#ifdef _USE_OLD_CODE
#warning "using old code"
#else

// Define this, if you want to compile with qsort from stdlib.h
// else the Qt Heapsort will be used.
// Note, qsort seems to be a bit faster (~10%) on a large man section
// eg. man section 3
#define _USE_QSORT

// Setup my own structure, with char pointers.
// from now on only pointers are copied, no strings
//
// containing the whole path string,
// the beginning of the man page name
// and the length of the name
struct man_index_t {
    const char *manpath;  // the full path including man file
    const char *manpage_begin;  // pointer to the begin of the man file name in the path
    int manpage_len; // len of the man file name
};
typedef man_index_t *man_index_ptr;

#ifdef _USE_QSORT
int compare_man_index(const void *s1, const void *s2)
{
    struct man_index_t *m1 = *(struct man_index_t **)s1;
    struct man_index_t *m2 = *(struct man_index_t **)s2;
    int i;
    // Compare the names of the pages
    // with the shorter length.
    // Man page names are not '\0' terminated, so
    // this is a bit tricky
    if ( m1->manpage_len > m2->manpage_len)
    {
	i = qstrncmp( m1->manpage_begin,
		      m2->manpage_begin,
		      m2->manpage_len);
	if (!i)
	    return 1;
	return i;
    }

    if ( m1->manpage_len < m2->manpage_len)
    {
	i = strncmp( m1->manpage_begin,
		     m2->manpage_begin,
		     m1->manpage_len);
	if (!i)
	    return -1;
	return i;
    }

    return strncmp( m1->manpage_begin,
		    m2->manpage_begin,
		    m1->manpage_len);
}

#else /* !_USE_QSORT */
#warning using heapsort
// Set up my own man page list,
// with a special compare function to sort itself
typedef QList<struct man_index_t> QManIndexListBase;
typedef QListIterator<struct man_index_t> QManIndexListIterator;

class QManIndexList : public QManIndexListBase
{
public:
private:
    int compareItems( QCollection::Item s1, QCollection::Item s2 )
	{
	    struct man_index_t *m1 = (struct man_index_t *)s1;
	    struct man_index_t *m2 = (struct man_index_t *)s2;
	    int i;
	    // compare the names of the pages
	    // with the shorter length
	    if (m1->manpage_len > m2->manpage_len)
	    {
		i = qstrncmp(m1->manpage_begin,
			     m2->manpage_begin,
			     m2->manpage_len);
		if (!i)
		    return 1;
		return i;
	    }

	    if (m1->manpage_len > m2->manpage_len)
	    {

		i = qstrncmp(m1->manpage_begin,
			     m2->manpage_begin,
			     m1->manpage_len);
		if (!i)
		    return -1;
		return i;
	    }

	    return qstrncmp(m1->manpage_begin,
			    m2->manpage_begin,
			    m1->manpage_len);
	}
};

#endif /* !_USE_QSORT */
#endif /* !_USE_OLD_CODE */




void MANProtocol::showIndex(const QString& section)
{
    QCString output;

    QTextStream os(output, IO_WriteOnly);
    // QTextSream on a QCString needs to be told explicitely to use local8Bit conversion !
    os.setEncoding(QTextStream::Locale);

    // print header
    os << "<html>" << endl;
    os << i18n("<head><title>UNIX Manual Index</title></head>") << endl;
    os << i18n("<body bgcolor=#ffffff><h1>Index for Section %1: %2</h1>").arg(section).arg(sectionName(section)) << endl;

    // compose list of search paths -------------------------------------------------------------

    checkManPaths();
    infoMessage(i18n("Generating Index"));

    // search for the man pages
    QStringList pages = KGlobal::dirs()->
                        findAllResources("manpath",
                                         QString("man%1/*").arg(section), true);

    /* pages += KGlobal::dirs()->
             findAllResources("manpath",
                              QString("sman%1/*").arg(section), true);
*/

    // print out the list
    os << "<table>" << endl;

#ifdef _USE_OLD_CODE
    pages.sort();

    QMap<QString, QString> pagemap;

    QStringList::ConstIterator page;
    for (page = pages.begin(); page != pages.end(); ++page)
    {
	QString fileName = *page;

        // skip compress extension
        if (fileName.right(4) == ".bz2")
        {
            fileName.truncate(fileName.length()-4);
        }
        else if (fileName.right(3) == ".gz")
        {
            fileName.truncate(fileName.length()-3);
        }
        else if (fileName.right(2) == ".Z")
        {
            fileName.truncate(fileName.length()-2);
        }

        // strip section
        int pos = fileName.findRev('.');
        if ((pos > 0) && (fileName.mid(pos).find(section) > 0))
            fileName = fileName.left(pos);

        pos = fileName.findRev('/');
        if (pos > 0)
            fileName = fileName.mid(pos+1);

        if (!fileName.isEmpty())
            pagemap[fileName] = *page;

    }

    for (QMap<QString,QString>::ConstIterator it = pagemap.begin();
	 it != pagemap.end(); ++it)
    {
	os << "<tr><td><a href=\"man:" << it.data() << "\">\n"
	   << it.key() << "</a></td><td>&nbsp;</td><td> "
	   << i18n("no idea yet") << "</td></tr>"  << endl;
    }

#else /* ! _USE_OLD_CODE */

#ifdef _USE_QSORT

    int listlen = pages.count();
    man_index_ptr *indexlist = new man_index_ptr[listlen];
    listlen = 0;

#else /* !_USE_QSORT */

    QManIndexList manpages;
    manpages.setAutoDelete(TRUE);

#endif /* _USE_QSORT */

    QStringList::ConstIterator page;
    for (page = pages.begin(); page != pages.end(); ++page)
    {
	// I look for the beginning of the man page name
	// i.e. "bla/pagename.3.gz" by looking for the last "/"
	// Then look for the end of the name with the next "."
	// If the len of the name is >0,
	// store it in the list structure, to be sorted later

        char *manpage_end;
        struct man_index_t *manindex = new man_index_t;
	manindex->manpath = (*page).latin1();

	manindex->manpage_begin = strrchr(manindex->manpath, '/');
	if (manindex->manpage_begin)
	{
	    manindex->manpage_begin++;
	    assert(manindex->manpage_begin >= manindex->manpath);
	}
	else
	{
	    manindex->manpage_begin = manindex->manpath;
	    assert(manindex->manpage_begin >= manindex->manpath);
	}

	manpage_end = strchr(manindex->manpage_begin, '.');
	if (NULL == manpage_end)
	{
	    // no '.' ending ???
	    // set the pointer past the end of the filename
	    manindex->manpage_len = (*page).length();
	    manindex->manpage_len -= (manindex->manpage_begin - manindex->manpath);
	    assert(manindex->manpage_len >= 0);
	}
	else
	{
	    manindex->manpage_len = (manpage_end - manindex->manpage_begin);
	    assert(manindex->manpage_len >= 0);
	}

	if (0 < manindex->manpage_len)
	{

#ifdef _USE_QSORT

	    indexlist[listlen] = manindex;
	    listlen++;

#else /* !_USE_QSORT */

	    manpages.append(manindex);

#endif /* _USE_QSORT */

	}
    }

    //
    // Now do the sorting on the page names
    // and the printout afterwards
    // While printing avoid duplicate man page names
    //

    struct man_index_t dummy_index = {0l,0l,0};
    struct man_index_t *last_index = &dummy_index;

#ifdef _USE_QSORT

    // sort and print
    qsort(indexlist, listlen, sizeof(struct man_index_t *), compare_man_index);

    for (int i=0; i<listlen; i++)
    {
	struct man_index_t *manindex = indexlist[i];

	// qstrncmp():
	// "last_man" has already a \0 string ending, but
	// "manindex->manpage_begin" has not,
	// so do compare at most "manindex->manpage_len" of the strings.
	if (last_index->manpage_len == manindex->manpage_len &&
	    !qstrncmp(last_index->manpage_begin,
		      manindex->manpage_begin,
		      manindex->manpage_len)
	    )
	{
	    continue;
	}
	os << "<tr><td><a href=\"man:"
	   << manindex->manpath << "\">\n";
	// !!!!!!!!!!!!!!!!!!!!!!
	//
	// WARNING
	//
	// here I do modify the const QString from "pages".
	// I assume, they are not used afterwards anyways
	//
	// Maybe I could use a QTextStream::WriteRaw(const char *, uint len)
	// too, but how about locale encoded Filenames ??
	//
	// !!!!!!!!!!!!!!!!!!!!!!
	((char *)manindex->manpage_begin)[manindex->manpage_len] = '\0';
	os << manindex->manpage_begin
	   << "</a></td><td>&nbsp;</td><td> "
	   << i18n("no idea yet")
	   << "</td></tr>"  << endl;
	last_index = manindex;
    }

    for (int i=0; i<listlen; i++)
	delete indexlist[i];

    delete [] indexlist;

#else /* !_USE_QSORT */

    manpages.sort(); // using

    for (QManIndexListIterator mit(manpages);
	 mit.current();
	 ++mit )
    {
	struct man_index_t *manindex = mit.current();

	// qstrncmp():
	// "last_man" has already a \0 string ending, but
	// "manindex->manpage_begin" has not,
	// so do compare at most "manindex->manpage_len" of the strings.
	if (last_index->manpage_len == manindex->manpage_len &&
	    !qstrncmp(last_index->manpage_begin,
		      manindex->manpage_begin,
		      manindex->manpage_len)
	    )
	{
	    continue;
	}

	os << "<tr><td><a href=\"man:"
	   << manindex->manpath << "\">\n";
	// !!!!!!!!!!!!!!!!!!!!!!
	//
	// WARNING
	//
	// here I do modify the const QString from "pages".
	// I assume, they are not used afterwards anyways
	//
	// Maybe I could use a QTextStream::WriteRaw(const char *, uint len)
	// too, but how about locale encoded Filenames ??
	//
	// !!!!!!!!!!!!!!!!!!!!!!
	manindex->manpage_begin[manindex->manpage_len] = '\0';
	os << manindex->manpage_begin
	   << "</a></td><td>&nbsp;</td><td> "
	   << i18n("no idea yet")
	   << "</td></tr>"  << endl;
	last_index = manindex;
    }
#endif /* _USE_QSORT */
#endif /* _USE_OLD_CODE */

    os << "</table>" << endl;

    // print footer
    os << "</body></html>" << endl;

    infoMessage(QString::null);
    data(output);
    finished();
}


