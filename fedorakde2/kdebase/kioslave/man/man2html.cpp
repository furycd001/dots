/*
** This program was written by Richard Verhoeven (NL:5482ZX35)
** at the Eindhoven University of Technology. Email: rcb5@win.tue.nl
**
** Permission is granted to distribute, modify and use this program as long
** as this comment is not removed or changed.
*/

/*
 * man2html-linux-1.0/1.1
 * This version modified for Redhat/Caldera linux - March 1996.
 * Michael Hamilton <michael@actrix.gen.nz>.
 *
 * man2html-linux-1.2
 * Added support for BSD mandoc pages - I didn't have any documentation
 * on the mandoc macros, so I may have missed some.
 * Michael Hamilton <michael@actrix.gen.nz>.
 *
 * vh-man2html-1.3
 * Renamed to avoid confusion (V for Verhoeven, H for Hamilton).
 *
 * vh-man2html-1.4
 * Now uses /etc/man.config
 * Added support for compressed pages.
 * Added "length-safe" string operations for client input parameters.
 * More secure, -M secured, and client input string lengths checked.
 *
 */

/*
** If you want to use this program for your WWW server, adjust the line
** which defines the CGIBASE or compile it with the -DCGIBASE='"..."' option.
**
** You have to adjust the built-in manpath to your local system. Note that
** every directory should start and end with the '/' and that the first
** directory should be "/" to allow a full path as an argument.
**
** The program first check if PATH_INFO contains some information.
** If it does (t.i. man2html/some/thing is used), the program will look
** for a manpage called PATH_INFO in the manpath.
**
** Otherwise the manpath is searched for the specified command line argument,
** where the following options can be used:
**
** name      name of manpage (csh, printf, xv, troff)
** section   the section (1 2 3 4 5 6 7 8 9 n l 1v ...)
** -M path   an extra directory to look for manpages (replaces "/")
**
** If man2html finds multiple manpages that satisfy the options, an index
** is displayed and the user can make a choice. If only one page is
** found, that page will be displayed.
**
** man2html will add links to the converted manpages. The function add_links
** is used for that. At the moment it will add links as follows, where
**     indicates what should match to start with:
** ^^^
** Recognition           Item            Link
** ----------------------------------------------------------
** name(*)               Manpage         ../man?/name.*
**     ^
** name@hostname         Email address   mailto:name@hostname
**     ^
** method://string       URL             method://string
**       ^^^
** www.host.name         WWW server      http://www.host.name
** ^^^^
** ftp.host.name         FTP server      ftp://ftp.host.name
** ^^^^
** <file.h>              Include file    file:/usr/include/file.h
**      ^^^
**
** Since man2html does not check if manpages, hosts or email addresses exist,
** some links might not work. For manpages, some extra checks are performed
** to make sure not every () pair creates a link. Also out of date pages
** might point to incorrect places.
**
** The program will not allow users to get system specific files, such as
** /etc/passwd. It will check that "man" is part of the specified file and
** that  "/../" isn't. Even if someone manages to get such file, man2html will
** handle it like a manpage and will usually not produce any output (or crash).
**
** If you find any bugs when normal manpages are converted, please report
** them to me (rcb5@win.tue.nl) after you have checked that man(1) can handle
** the manpage correct.
**
** Known bugs and missing features:
**
**  * Equations are not converted at all.
**  * Tables are converted but some features are not possible in html.
**  * The tabbing environment is converted by counting characters and adding
**    spaces. This might go wrong (outside <PRE>)
**  * Some pages look beter if man2html works in troff mode, especially pages
**    with tables. You can decide at compile time which made you want to use.
**
**    -DNROFF=0     troff mode
**    -DNROFF=1     nroff mode   (default)
**
**    if you install both modes, you should compile with the correct CGIBASE.
**  * Some manpages rely on the fact that troff/nroff is used to convert
**    them and use features which are not descripted in the man manpages.
**    (definitions, calculations, conditionals, requests). I can't guarantee
**    that all these features work on all manpages. (I didn't have the
**    time to look through all the available manpages.)
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include <qstring.h>
#include <qlist.h>
#include "man2html.h"

#define NULL_TERMINATED(n) ((n) + 1)

#define HUGE_STR_MAX  10000
#define LARGE_STR_MAX 2000
#define MED_STR_MAX   500
#define SMALL_STR_MAX 100
#define TINY_STR_MAX  10

#define MAX_ZCATS     10	/* Max number of zcat style programs */
#define MAX_WORDLIST  100

#ifndef NROFF
#define NROFF 1
#endif

#define DOCTYPE "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"

/* BSD mandoc Bl/El lists to HTML list types */
#define BL_DESC_LIST   1
#define BL_BULLET_LIST 2
#define BL_ENUM_LIST   4

/* BSD mandoc Bd/Ed example(?) blocks */
#define BD_LITERAL  1
#define BD_INDENT   2

static char *stralloc(int len)
{
  /* allocate enough for len + NULL */
  char *news = new char [len+1];
  if (!news) {
    fprintf(stderr, "man2html: out of memory\n");
    exit(EXIT_FAILURE);
  }
  return news;
}

static char *strmaxcpy(char *to, const char *from, int n)
{				/* Assumes space for n plus a null */
  int len = strlen(from);
  strncpy(to, from, n);
  to[(len <= n) ? len : n] = '\0';
  return to;
}

static char *strlimitcpy(char *to, char *from, int n, int limit)
{                               /* Assumes space for limit plus a null */
  int len = n > limit ? limit : n;
  strmaxcpy(to, from, len);
  to[len] = '\0';
  return to;
}

/* below this you should not change anything unless you know a lot
** about this program or about troff.
*/

struct STRDEF {
    int nr,slen;
    char *st;
    STRDEF *next;
};

struct INTDEF {
    int nr;
    int val;
    int incr;
    INTDEF *next;
};

static char NEWLINE[2]="\n";

static STRDEF *chardef, *strdef, *defdef;
static INTDEF *intdef;

#define V(A,B) ((A)*256+(B))

/* default: print code */


/* static char eqndelimopen=0, eqndelimclose=0; */
static char escapesym='\\', nobreaksym='\'', controlsym='.', fieldsym=0, padsym=0;

static char *buffer=NULL;
static int buffpos=0, buffmax=0;
static int scaninbuff=0;
static int itemdepth=0;
static int section=0;
static int dl_set[20]= { 0 };
static int still_dd=0;
static int tabstops[20] = { 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96 };
static int maxtstop=12;
static int curpos=0;

static char *scan_troff(char *c, int san, char **result);
static char *scan_troff_mandoc(char *c, int san, char **result);

static char **argument=NULL;

static char charb[TINY_STR_MAX];

static const char *expand_char(int nr)
{
  STRDEF *h;
  h=chardef;
  if (!nr) return NULL;
  while (h)
      if (h->nr==nr) {
	  curpos+=h->slen;
	  return h->st;
      } else
	  h=h->next;
  charb[0]=nr/256;
  charb[1]=nr%256;
  charb[2]='\0';
  if (charb[0] == '<') {	/* Fix up <= */
    charb[4] = charb[1];
    strncpy(charb, "&lt;", 4);
    charb[5] = '\0';
  }
  curpos+=2;
  return charb;
}

static const char *expand_string(int nr)
{
  STRDEF *h=strdef;
  if (!nr) return NULL;
  while (h)
      if (h->nr==nr) {
	  curpos+=h->slen;
	  return h->st;
    } else
	h=h->next;
  return NULL;
}

static char outbuffer[NULL_TERMINATED(HUGE_STR_MAX)];
static int obp=0;
static int no_newline_output=0;
static int newline_for_fun=0;
static int output_possible=0;
static int out_length=0;

static const char *includedirs[] = {
    "/usr/include",
    "/usr/include/sys",
    "/usr/local/include",
    "/opt/local/include",
    "/usr/ccs",
    "/usr/X11R6/include",
    "/usr/openwin/include",
    0
};


static void add_links(char *c)
{
    /*
    ** Add the links to the output.
    ** At the moment the following are recognized:
    **
    ** name(*)                 -> ../man?/name.*
    ** method://string         -> method://string
    ** www.host.name           -> http://www.host.name
    ** ftp.host.name           -> ftp://ftp.host.name
    ** name@host               -> mailto:name@host
    ** <name.h>                -> file:/usr/include/name.h   (guess)
    **
    ** Other possible links to add in the future:
    **
    ** /dir/dir/file  -> file:/dir/dir/file
    */
    int i,j,nr;
    char *f, *g,*h;
    char *idtest[6]; /* url, mailto, www, ftp, manpage */
    out_length+=strlen(c);
    /* search for (section) */
    nr=0;
    idtest[0]=strstr(c+1,"://");
    idtest[1]=strchr(c+1,'@');
    idtest[2]=strstr(c,"www.");
    idtest[3]=strstr(c,"ftp.");
    idtest[4]=strchr(c+1,'(');
    idtest[5]=strstr(c+1,".h&gt;");
    for (i=0; i<6 && !nr; i++) nr = (idtest[i]!=NULL);
    while (nr) {
	j=-1;
	for (i=0; i<6; i++)
	    if (idtest[i] && (j<0 || idtest[i]<idtest[j])) j=i;
	switch (j) {
	case 5: { /* <name.h> */
	    f=idtest[5];
	    h=f+2;
	    g=f;
	    while (g>c && g[-1]!=';') g--;
            bool wrote_include = false;

            if (g!=c) {

                QCString dir;
                QCString file(g, h - g + 1);
                file = file.stripWhiteSpace();
                for (int index = 0; includedirs[index]; index++) {
                    QCString str = QCString(includedirs[index]) + "/" + file;
                    if (!access(str, R_OK)) {
                        dir = includedirs[index];
                        break;
                    }
                }
                if (!dir.isEmpty()) {

                    char t;
                    t=*g;
                    *g=0;
                    output_real(c);
                    *g=t;*h=0;

                    QCString str;
                    str.sprintf("<A HREF=\"file:%s/%s\">%s</A>&gt;", dir.data(), file.data(), file.data());
                    output_real(str.data());
                    c=f+6;
                    wrote_include = true;
                }

            }

            if (!wrote_include) {
                f[5]=0;
                output_real(c);
                f[5]=';';
                c=f+5;
            }
        }
        break;
	case 4: /* manpage */
	    f=idtest[j];
	    /* check section */
	    g=strchr(f,')');
	    if (g && f-g<6 && (isalnum(f[-1]) || f[-1]=='>') &&
		((isdigit(f[1]) && f[1]!='0' &&
		  (f[2]==')' || (isalpha(f[2]) && f[3]==')') || f[2]=='X')) ||
		 (f[2]==')' && (f[1]=='n' || f[1]=='l')))) {
		/* this might be a link */
		h=f-1;
		/* skip html makeup */
		while (h>c && *h=='>') {
		    while (h!=c && *h!='<') h--;
		    if (h!=c) h--;
		}
		if (isalnum(*h)) {
		    char t,sec,subsec, *e;
		    e=h+1;
		    sec=f[1];
		    subsec=f[2];
		    if ((subsec=='X' && f[3]!=')')|| subsec==')') subsec='\0';
		    while (h>c && (isalnum(h[-1]) || h[-1]=='_'
				    || h[-1]==':' || h[-1]=='-' || h[-1]=='.'))
			h--;
		    t=*h;
		    *h='\0';
                    output_real(c);
		    *h=t;
		    t=*e;
		    *e='\0';
                    QCString str;
		    if (subsec)
                        str.sprintf("<A HREF=\"man:/%s(%c,%c))\">%s</A>", h, sec, tolower(subsec), h);
		    else
                        str.sprintf("<A HREF=\"man:/%s(%c)\">%s</A>", h, sec, h);
                    output_real(str.data());
		    *e=t;
		    c=e;
		}
	    }
	    *f='\0';
            output_real(c);
	    *f='(';
	    idtest[4]=f-1;
	    c=f;
	    break; /* manpage */
	case 3: /* ftp */
	case 2: /* www */
	    g=f=idtest[j];
	    while (*g && (isalnum(*g) || *g=='_' || *g=='-' || *g=='+' ||
			  *g=='.')) g++;
	    if (g[-1]=='.') g--;
	    if (g-f>4) {
		char t;
		t=*f; *f='\0';
                output_real(c);
		*f=t; t=*g;*g='\0';
                QCString str;
                str.sprintf("<A HREF=\"%s://%s\">%s</A>", ((j==3)?"ftp":"http"), f, f);
                output_real(str.data());
		*g=t;
		c=g;
	    } else {
		f[3]='\0';
                output_real(c);
		c=f+3;
		f[3]='.';
	    }
	    break;
	case 1: /* mailto */
	    g=f=idtest[1];
	    while (g>c && (isalnum(g[-1]) || g[-1]=='_' || g[-1]=='-' ||
			   g[-1]=='+' || g[-1]=='.' || g[-1]=='%')) g--;
	    h=f+1;
	    while (*h && (isalnum(*h) || *h=='_' || *h=='-' || *h=='+' ||
			  *h=='.')) h++;
	    if (*h=='.') h--;
	    if (h-f>4 && f-g>1) {
		char t;
		t=*g;
		*g='\0';
                output_real(c);
		*g=t;t=*h;*h='\0';
                QCString str;
                str.sprintf("<A HREF=\"mailto:%s\">%s</A>", g, g);
                output_real(str.data());
		*h=t;
		c=h;
	    } else {
		*f='\0';
                output_real(c);
		*f='@';
		idtest[1]=c;
		c=f;
	    }
	    break;
	case 0: /* url */
	    g=f=idtest[0];
	    while (g>c && isalpha(g[-1]) && islower(g[-1])) g--;
	    h=f+3;
	    while (*h && !isspace(*h) && *h!='<' && *h!='>' && *h!='"' &&
		   *h!='&') h++;
	    if (f-g>2 && f-g<7 && h-f>3) {
		char t;
		t=*g;
		*g='\0';
                output_real(c);
		*g=t; t=*h; *h='\0';
                QCString str;
                str.sprintf("<A HREF=\"%s\">%s</A>", g, g);
                output_real(str.data());
		*h=t;
		c=h;
	    } else {
		f[1]='\0';
                output_real(c);
		f[1]='/';
		c=f+1;
	    }
	    break;
	default:
	    break;
	}
	nr=0;
	if (idtest[0] && idtest[0]<c) idtest[0]=strstr(c+1,"://");
	if (idtest[1] && idtest[1]<c) idtest[1]=strchr(c+1,'@');
	if (idtest[2] && idtest[2]<c) idtest[2]=strstr(c,"www.");
	if (idtest[3] && idtest[3]<c) idtest[3]=strstr(c,"ftp.");
	if (idtest[4] && idtest[4]<c) idtest[4]=strchr(c+1,'(');
	if (idtest[5] && idtest[5]<c) idtest[5]=strstr(c+1,".h&gt;");
	for (i=0; i<6 && !nr; i++) nr = (idtest[i]!=NULL);
    }
    output_real(c);
}

static int current_font=0;
static int current_size=0;
static int fillout=1;

static void out_html(const char *c)
{
  if (!c) return;

  char *c2 = qstrdup(c);
  char *c3 = c2;

  if (no_newline_output) {
      int i=0;
      no_newline_output=1;
      while (c2[i]) {
	  if (!no_newline_output) c2[i-1]=c2[i];
	  if (c2[i]=='\n') no_newline_output=0;
	  i++;
      }
      if (!no_newline_output) c2[i-1]=0;
  }
  if (scaninbuff) {
      while (*c2) {
	  if (buffpos>=buffmax) {
	      char *h = new char[buffmax*2];
	      if (!h) exit(1);
              memcpy(h, buffer, buffmax);
              delete [] buffer;
	      buffer=h;
	      buffmax=buffmax*2;
	  }
	  buffer[buffpos++]=*c2++;
      }
  } else
      if (output_possible) {
	  while (*c2) {
	      outbuffer[obp++]=*c2;
	      if (*c=='\n' || obp >= HUGE_STR_MAX) {
		  outbuffer[obp]='\0';
		  add_links(outbuffer);
		  obp=0;
	      }
	      c2++;
	  }
      }
  delete [] c3;
}

#define FO0 ""
#define FC0 ""
#define FO1 "<span class=\"parameter\">"
#define FC1 "</span>"
#define FO2 "<span class=\"option\">"
#define FC2 "</span>"
#define FO3 "<TT>"
#define FC3 "</TT>"

static const char *switchfont[16] = { ""     , FC0 FO1, FC0 FO2, FC0 FO3,
			 FC1 FO0, ""     , FC1 FO2, FC1 FO3,
			 FC2 FO0, FC2 FO1, ""     , FC2 FO3,
			 FC3 FO0, FC3 FO1, FC3 FO2, ""      };

static const char *change_to_font(int nr)
{
  int i;
  switch (nr) {
  case '0': nr++;
  case '1': case '2': case '3': case '4': nr=nr-'1'; break;
  case V('C','W'): nr=3; break;
  case 'L': nr=3; break;
  case 'B': nr=2; break;
  case 'I': nr=1; break;
  case 'P': case 'R': nr=0; break;
  case 0: case 1: case 2: case 3: break;
  default: nr=0; break;
  }
  i= current_font*4+nr%4;
  current_font=nr%4;
  return switchfont[i];
}

static char sizebuf[200];

static const char *change_to_size(int nr)
{
  int i;
  switch (nr) {
  case '0': case '1': case '2': case '3': case '4': case '5': case '6':
  case '7': case '8': case '9': nr=nr-'0'; break;
  case '\0': break;
  default: nr=current_size+nr; if (nr>9) nr=9; if (nr< -9) nr=-9; break;
  }
  if (nr==current_size) return "";
  i=current_font;
  sizebuf[0]='\0';
  strcat(sizebuf, change_to_font(0));
  if (current_size) strcat(sizebuf, "</FONT>");
  current_size=nr;
  if (nr) {
    int l;
    strcat(sizebuf, "<FONT SIZE=\"");
    l=strlen(sizebuf);
    if (nr>0) sizebuf[l++]='+'; else sizebuf[l++]='-',nr=-nr;
    sizebuf[l++]=nr+'0';
    sizebuf[l++]='"';
    sizebuf[l++]='>';
    sizebuf[l]='\0';
  }
  strcat(sizebuf, change_to_font(i));
  return sizebuf;
}

/* static int asint=0; */
static int intresult=0;

#define SKIPEOL while (*c && *c++!='\n')

static int skip_escape=0;
static int single_escape=0;

static char *scan_escape(char *c)
{
    const char *h=NULL;
    char b[5];
    INTDEF *intd;
    int exoutputp,exskipescape;
    int i,j;

    intresult=0;
    switch (*c) {
    case 'e': h="\\"; curpos++;break;
    case '0':
    case ' ': h="&nbsp;";curpos++; break;
    case '|': h=""; break;
    case '"': SKIPEOL; c--; h=""; break;
    case '$':
	if (argument) {
	    c++;
	    i=(*c -'1');
	    if (i < 0 || i > (int)strlen(*argument) || !(h=argument[i])) h="";
	}
	break;
    case 'z':
	c++;
	if (*c=='\\') { c=scan_escape(c+1); c--;h=""; }
	else {
	    b[0]=*c;
	    b[1]='\0';
	    h="";
	}
	break;
    case 'k': c++; if (*c=='(') c+=2;
    case '^':
    case '!':
    case '%':
    case 'a':
    case 'd':
    case 'r':
    case 'u':
    case '\n':
        h="";
        break;
    case '&':
        b[0] = c[1];
        b[1] = 0;
        out_html(b);
        c++;
        h="";
        break;
    case '(':
	c++;
	i= c[0]*256+c[1];
	c++;
	h = expand_char(i);
	break;
    case '*':
	c++;
	if (*c=='(') {
	    c++;
	    i= c[0]*256+c[1];
	    c++;
	} else
	    i= *c *256+' ';
	h = expand_string(i);
	break;
    case 'f':
	c++;
	if (*c=='\\') {
	    c++;
	    c=scan_escape(c);
	    c--;
	    i=intresult;
	} else 	if (*c != '(')
	    i=*c;
	else {
	    c++;
	    i=c[0]*256+c[1];
	    c++;
	}
	if (!skip_escape) h=change_to_font(i); else h="";
	break;
    case 's':
	c++;
	j=0;i=0;
	if (*c=='-') {j= -1; c++;} else if (*c=='+') {j=1; c++;}
	if (*c=='0') c++; else if (*c=='\\') {
	    c++;
	    c=scan_escape(c);
	    i=intresult; if (!j) j=1;
	} else
	    while (isdigit(*c) && (!i || (!j && i<4))) i=i*10+(*c++)-'0';
	if (!j) { j=1; if (i) i=i-10; }
	if (!skip_escape) h=change_to_size(i*j); else h="";
	c--;
	break;
    case 'n':
	c++;
	j=0;
	switch (*c) {
	case '+': j=1; c++; break;
	case '-': j=-1; c++; break;
	default: break;
	}
	if (*c=='(') {
	    c++;
	    i=V(c[0],c[1]);
	    c=c+1;
	} else {
	    i=V(c[0],' ');
	}
	intd=intdef;
	while (intd && intd->nr!=i) intd=intd->next;
	if (intd) {
	    intd->val=intd->val+j*intd->incr;
	    intresult=intd->val;
	} else {
	    switch (i) {
	    case V('.','s'): intresult=current_size; break;
	    case V('.','f'): intresult=current_font; break;
	    default: intresult=0; break;
	    }
	}
	h="";
	break;
    case 'w':
	c++;
	i=*c;
	c++;
	exoutputp=output_possible;
	exskipescape=skip_escape;
	output_possible=0;
	skip_escape=1;
	j=0;
	while (*c!=i) {
	    j++;
	    if (*c==escapesym) c=scan_escape(c+1); else c++;
	}
	output_possible=exoutputp;
	skip_escape=exskipescape;
	intresult=j;
	break;
    case 'l': h="<HR>"; curpos=0;
    case 'b':
    case 'v':
    case 'x':
    case 'o':
    case 'L':
    case 'h':
	c++;
	i=*c;
	c++;
	exoutputp=output_possible;
	exskipescape=skip_escape;
	output_possible=0;
	skip_escape=1;
	while (*c != i)
	    if (*c==escapesym) c=scan_escape(c+1);
	    else c++;
	output_possible=exoutputp;
	skip_escape=exskipescape;
	break;
    case 'c': no_newline_output=1; break;
    case '{': newline_for_fun++; h="";break;
    case '}': if (newline_for_fun) newline_for_fun--; h="";break;
    case 'p': h="<BR>\n";curpos=0; break;
    case 't': h="\t";curpos=(curpos+8)&0xfff8; break;
    case '<': h="&lt;";curpos++; break;
    case '>': h="&gt;";curpos++; break;
    case '\\': if (single_escape) { c--; break;}
    default: b[0]=*c; b[1]=0; h=b; curpos++; break;
    }
    c++;
    if (!skip_escape) out_html(h);
    return c;
}

class TABLEROW;

class TABLEITEM {
public:
    TABLEITEM(TABLEROW *row);
    ~TABLEITEM() {
        delete [] contents;
    }
    void setContents(const char *_contents) {
        delete [] contents;
        contents = qstrdup(_contents);
    }
    const char *getContents() const { return contents; }

    void init() {
        delete [] contents;
        contents = 0;
        size = 0;
        align = 0;
        valign = 0;
        colspan = 1;
        rowspan = 1;
        font = 0;
        vleft = 0;
        vright = 0;
        space = 0;
        width = 0;
    }

    void copyLayout(const TABLEITEM *orig) {
        size = orig->size;
        align = orig->align;
        valign = orig->valign;
        colspan = orig->colspan;
        rowspan = orig->rowspan;
        font = orig->font;
        vleft = orig->vleft;
        vright = orig->vright;
        space = orig->space;
        width = orig->width;
    }

public:
    int size,align,valign,colspan,rowspan,font,vleft,vright,space,width;

private:
    char *contents;
    TABLEROW *_parent;
};

class TABLEROW {
    char *test;
public:
    TABLEROW() {
        test = new char;
        items.setAutoDelete(true);
        prev = 0; next = 0;
    }
    ~TABLEROW() {
        delete test;

    }
    int length() const { return items.count(); }
    bool has(int index) {
        return (index >= 0) && (index < (int)items.count());
    }
    TABLEITEM &at(int index) {
        return *items.at(index);
    }

    TABLEROW *copyLayout() const;

    void addItem(TABLEITEM *item) {
        items.append(item);
    }
    TABLEROW *prev, *next;

private:
    QList<TABLEITEM> items;
};

TABLEITEM::TABLEITEM(TABLEROW *row) : contents(0), _parent(row) {
     init();
     _parent->addItem(this);
}

TABLEROW *TABLEROW::copyLayout() const {
    TABLEROW *newrow = new TABLEROW();

    QListIterator<TABLEITEM> it(items);
    for ( ; it.current(); ++it) {
        TABLEITEM *newitem = new TABLEITEM(newrow);
        newitem->copyLayout(it.current());
    }
    return newrow;
}

static const char *tableopt[]= { "center", "expand", "box", "allbox",
                                 "doublebox", "tab", "linesize",
                                 "delim", NULL };
static int tableoptl[] = { 6,6,3,6,9,3,8,5,0};


static void clear_table(TABLEROW *table)
{
    TABLEROW *tr1,*tr2;

    tr1=table;
    while (tr1->prev) tr1=tr1->prev;
    while (tr1) {
	tr2=tr1;
	tr1=tr1->next;
	delete tr2;
    }
}

static char *scan_expression(char *c, int *result);

static char *scan_format(char *c, TABLEROW **result, int *maxcol)
{
    TABLEROW *layout, *currow;
    TABLEITEM *curfield;
    int i,j;
    if (*result) {
	clear_table(*result);
    }
    layout= currow=new TABLEROW();
    curfield=new TABLEITEM(currow);
    while (*c && *c!='.') {
	switch (*c) {
	case 'C': case 'c': case 'N': case 'n':
	case 'R': case 'r': case 'A': case 'a':
	case 'L': case 'l': case 'S': case 's':
	case '^': case '_':
	    if (curfield->align)
		curfield=new TABLEITEM(currow);
	    curfield->align=toupper(*c);
	    c++;
	    break;
	case 'i': case 'I': case 'B': case 'b':
	    curfield->font = toupper(*c);
	    c++;
	    break;
	case 'f': case 'F':
	    c++;
	    curfield->font = toupper(*c);
	    c++;
	    if (!isspace(*c)) c++;
	    break;
	case 't': case 'T': curfield->valign='t'; c++; break;
	case 'p': case 'P':
	    c++;
	    i=j=0;
	    if (*c=='+') { j=1; c++; }
	    if (*c=='-') { j=-1; c++; }
	    while (isdigit(*c)) i=i*10+(*c++)-'0';
	    if (j) curfield->size= i*j; else curfield->size=j-10;
	    break;
	case 'v': case 'V':
	case 'w': case 'W':
	    c=scan_expression(c+2,&curfield->width);
	    break;
	case '|':
	    if (curfield->align) curfield->vleft++;
	    else curfield->vright++;
	    c++;
	    break;
	case 'e': case 'E':
	    c++;
	    break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    i=0;
	    while (isdigit(*c)) i=i*10+(*c++)-'0';
	    curfield->space=i;
	    break;
	case ',': case '\n':
	    currow->next=new TABLEROW();
	    currow->next->prev=currow;
	    currow=currow->next;
	    currow->next=NULL;
	    curfield=new TABLEITEM(currow);
	    c++;
	    break;
	default:
	    c++;
	    break;
	}
    }
    if (*c=='.') while (*c++!='\n');
    *maxcol=0;
    currow=layout;
    while (currow) {
	i=currow->length();
	if (i>*maxcol) *maxcol=i;
	currow=currow->next;
    }
    *result=layout;
    return c;
}

static TABLEROW *next_row(TABLEROW *tr)
{
    if (tr->next) {
	tr=tr->next;
	if (!tr->next)
            return next_row(tr);
        return tr;
    } else {
	tr->next = tr->copyLayout();
        tr->next->prev = tr;
	return tr->next;
    }
}

static char itemreset[20]="\\fR\\s0";

#define FORWARDCUR  do { curfield++; } while (currow->has(curfield) &&  currow->at(curfield).align=='S');

static char *scan_table(char *c)
{
    char *h;
    char *g;
    int center=0, expand=0, box=0, border=0, linesize=1;
    int i,j,maxcol=0, finished=0;
    int oldfont, oldsize,oldfillout;
    char itemsep='\t';
    TABLEROW *layout=NULL, *currow;
    int curfield = -1;
    while (*c++!='\n');
    h=c;
    if (*h=='.') return c-1;
    oldfont=current_font;
    oldsize=current_size;
    oldfillout=fillout;
    out_html(change_to_font(0));
    out_html(change_to_size(0));
    if (!fillout) {
	fillout=1;
	out_html("</PRE>");
    }
    while (*h && *h!='\n') h++;
    if (h[-1]==';') {
	/* scan table options */
	while (c<h) {
	    while (isspace(*c)) c++;
	    for (i=0; tableopt[i] && strncmp(tableopt[i],c,tableoptl[i]);i++);
	    c=c+tableoptl[i];
	    switch (i) {
	    case 0: center=1; break;
	    case 1: expand=1; break;
	    case 2: box=1; break;
	    case 3: border=1; break;
	    case 4: box=2; break;
	    case 5: while (*c++!='('); itemsep=*c++; break;
	    case 6: while (*c++!='('); linesize=0;
		while (isdigit(*c)) linesize=linesize*10+(*c++)-'0';
		break;
	    case 7: while (*c!=')') c++;
	    default: break;
	    }
	    c++;
	}
	c=h+1;
    }
    /* scan layout */
    c=scan_format(c,&layout, &maxcol);
//    currow=layout;
    currow=next_row(layout);
    curfield=0;
    i=0;
    while (!finished && *c) {
	/* search item */
	h=c;
	if ((*c=='_' || *c=='=') && (c[1]==itemsep || c[1]=='\n')) {
	    if (c[-1]=='\n' && c[1]=='\n') {
		if (currow->prev) {
		    currow->prev->next=new TABLEROW();
		    currow->prev->next->next=currow;
		    currow->prev->next->prev=currow->prev;
		    currow->prev=currow->prev->next;
		} else {
		    currow->prev=layout=new TABLEROW();
		    currow->prev->prev=NULL;
		    currow->prev->next=currow;
		}
		TABLEITEM *newitem = new TABLEITEM(currow->prev);
		newitem->align=*c;
		newitem->colspan=maxcol;
		curfield=0;
		c=c+2;
	    } else {
		if (currow->has(curfield)) {
		    currow->at(curfield).align=*c;
                    FORWARDCUR;
		}
		if (c[1]=='\n') {
		    currow=next_row(currow);
		    curfield=0;
		}
		c=c+2;
	    }
	} else if (*c=='T' && c[1]=='{') {
	    h=c+2;
	    c=strstr(h,"\nT}");
	    c++;
	    *c='\0';
	    g=NULL;
	    scan_troff(h,0,&g);
	    scan_troff(itemreset, 0, &g);
	    *c='T';
	    c+=3;
	    if (currow->has(curfield)) {
		currow->at(curfield).setContents(g);
                FORWARDCUR;
	    }
            delete [] g;

	    if (c[-1]=='\n') {
		currow=next_row(currow);
		curfield=0;
	    }
	} else if (*c=='.' && c[1]=='T' && c[2]=='&' && c[-1]=='\n') {
	    TABLEROW *hr;
	    while (*c++!='\n');
	    hr=currow;
	    currow=currow->prev;
	    hr->prev=NULL;
	    c=scan_format(c,&hr, &i);
	    hr->prev=currow;
	    currow->next=hr;
	    currow=hr;
	    next_row(currow);
	    curfield=0;
	} else if (*c=='.' && c[1]=='T' && c[2]=='E' && c[-1]=='\n') {
	    finished=1;
	    while (*c++!='\n');
	    if (currow->prev)
		currow->prev->next=NULL;
	    currow->prev=NULL;
            clear_table(currow);
            currow = 0;
        } else if (*c=='.' && c[-1]=='\n' && !isdigit(c[1])) {
	    /* skip troff request inside table (usually only .sp ) */
	    while (*c++!='\n');
	} else {
	    h=c;
	    while (*c && (*c!=itemsep || c[-1]=='\\') &&
		   (*c!='\n' || c[-1]=='\\')) c++;
	    i=0;
	    if (*c==itemsep) {i=1; *c='\n'; }
	    if (h[0]=='\\' && h[2]=='\n' &&
		(h[1]=='_' || h[1]=='^')) {
		if (currow->has(curfield)) {
		    currow->at(curfield).align=h[1];
                    FORWARDCUR;
		}
		h=h+3;
	    } else {
		g=NULL;
		h=scan_troff(h,1,&g);
		scan_troff(itemreset,0, &g);
		if (currow->has(curfield)) {
		    currow->at(curfield).setContents(g);
                    FORWARDCUR;
		}
                delete [] g;
	    }
	    if (i) *c=itemsep;
	    c=h;
	    if (c[-1]=='\n') {
		currow=next_row(currow);
		curfield=0;
	    }
	}
    }
    /* calculate colspan and rowspan */
    currow=layout;
    while (currow->next) currow=currow->next;
    while (currow) {
        int ti = 0, ti1 = 0, ti2 = -1;
        TABLEROW *prev = currow->prev;
        if (!prev)
            break;

	while (prev->has(ti1)) {
	    if (currow->has(ti))
                switch (currow->at(ti).align) {
                    case 'S':
                        if (currow->has(ti2)) {
                            currow->at(ti2).colspan++;
                            if (currow->at(ti2).rowspan<prev->at(ti1).rowspan)
                                currow->at(ti2).rowspan=prev->at(ti1).rowspan;
                        }
                        break;
                    case '^':
                        if (prev->has(ti1)) prev->at(ti1).rowspan++;
                    default:
                        if (ti2 < 0) ti2=ti;
                        else {
                            do {
                                ti2++;
                            } while (currow->has(ti2) && currow->at(ti2).align=='S');
                        }
                        break;
                }
            ti++;
            if (ti1 >= 0) ti1++;
        }
        currow=currow->prev;
    }
    /* produce html output */
    if (center) out_html("<CENTER>");
    if (box==2) out_html("<TABLE BORDER><TR><TD>");
    out_html("<TABLE");
    if (box || border) {
        out_html(" BORDER");
        if (!border) out_html("><TR><TD><TABLE");
        if (expand) out_html(" WIDTH=\"100%\"");
    }
    out_html(">\n");
    currow=layout;
    while (currow) {
        j=0;
        out_html("<TR VALIGN=top>");
	curfield=0;
	while (currow->has(curfield)) {
	    if (currow->at(curfield).align!='S' && currow->at(curfield).align!='^') {
		out_html("<TD");
		switch (currow->at(curfield).align) {
                    case 'N':
                        currow->at(curfield).space+=4;
                    case 'R':
                        out_html(" ALIGN=right");
                        break;
                    case 'C':
                        out_html(" ALIGN=center");
                    default:
                        break;
		}
		if (!currow->at(curfield).valign && currow->at(curfield).rowspan>1)
		    out_html(" VALIGN=center");
		if (currow->at(curfield).colspan>1) {
		    char buf[5];
		    out_html(" COLSPAN=");
		    sprintf(buf, "%i", currow->at(curfield).colspan);
		    out_html(buf);
		}
		if (currow->at(curfield).rowspan>1) {
		    char buf[5];
		    out_html(" ROWSPAN=");
		    sprintf(buf, "%i", currow->at(curfield).rowspan);
		    out_html(buf);
		}
		j=j+currow->at(curfield).colspan;
		out_html(">");
		if (currow->at(curfield).size) out_html(change_to_size(currow->at(curfield).size));
		if (currow->at(curfield).font) out_html(change_to_font(currow->at(curfield).font));
		switch (currow->at(curfield).align) {
		case '=': out_html("<HR><HR>"); break;
		case '_': out_html("<HR>"); break;
		default:
		    out_html(currow->at(curfield).getContents());
		    break;
		}
		if (currow->at(curfield).space)
		    for (i=0; i<currow->at(curfield).space;i++) out_html("&nbsp;");
		if (currow->at(curfield).font) out_html(change_to_font(0));
		if (currow->at(curfield).size) out_html(change_to_size(0));
		if (j>=maxcol && currow->at(curfield).align>'@' && currow->at(curfield).align!='_')
		    out_html("<BR>");
		out_html("</TD>");
	    }
	    curfield++;
	}
	out_html("</TR>\n");
	currow=currow->next;
    }

    clear_table(layout);

    if (box && !border) out_html("</TABLE>");
    out_html("</TABLE>");
    if (box==2) out_html("</TABLE>");
    if (center) out_html("</CENTER>\n");
    else out_html("\n");
    if (!oldfillout) out_html("<PRE>");
    fillout=oldfillout;
    out_html(change_to_size(oldsize));
    out_html(change_to_font(oldfont));
    return c;
}

static char *scan_expression(char *c, int *result)
{
    int value=0,value2,sign=1,opex=0;
    char oper='c';

    if (*c=='!') {
	c=scan_expression(c+1, &value);
	value= (!value);
    } else if (*c=='n') {
	c++;
	value=NROFF;
    } else if (*c=='t') {
	c++;
	value=1-NROFF;
    } else if (*c=='\'' || *c=='"' || *c<' ' || (*c=='\\' && c[1]=='(')) {
	/* ?string1?string2?
	** test if string1 equals string2.
	*/
	char *st1=NULL, *st2=NULL, *h;
	char *tcmp=NULL;
	char sep;
	sep=*c;
	if (sep=='\\') {
	    tcmp=c;
	    c=c+3;
	}
	c++;
	h=c;
	while (*c!= sep && (!tcmp || strncmp(c,tcmp,4))) c++;
	*c='\n';
	scan_troff(h, 1, &st1);
	*c=sep;
	if (tcmp) c=c+3;
	c++;
	h=c;
	while (*c!=sep && (!tcmp || strncmp(c,tcmp,4))) c++;
	*c='\n';
	scan_troff(h,1,&st2);
	*c=sep;
	if (!st1 && !st2) value=1;
	else if (!st1 || !st2) value=0;
	else value=(!strcmp(st1, st2));
	delete [] st1;
        delete [] st2;
	if (tcmp) c=c+3;
	c++;
    } else {
	while (*c && !isspace(*c) && *c!=')' && opex >= 0) {
	    opex=0;
	    switch (*c) {
	    case '(':
		c=scan_expression(c+1, &value2);
		value2=sign*value2;
		opex=1;
		break;
	    case '.':
	    case '0': case '1':
	    case '2': case '3':
	    case '4': case '5':
	    case '6': case '7':
	    case '8': case '9': {
		int num=0,denum=1;
		value2=0;
		while (isdigit(*c)) value2=value2*10+((*c++)-'0');
		if (*c=='.' && isdigit(c[1])) {
		    c++;
		    while (isdigit(*c)) {
			num=num*10+((*c++)-'0');
			denum=denum*10;
		    }
		}
		if (isalpha(*c)) {
		    /* scale indicator */
		    switch (*c) {
		    case 'i': /* inch -> 10pt */
			value2=value2*10+(num*10+denum/2)/denum;
			num=0;
			break;
		    default:
			break;
		    }
		    c++;
		}
		value2=value2+(num+denum/2)/denum;
		value2=sign*value2;
		opex=1;
                if (*c=='.')
                    opex = -1;

	    }
            break;
	    case '\\':
		c=scan_escape(c+1);
		value2=intresult*sign;
		if (isalpha(*c)) c++; /* scale indicator */
		opex=1;
		break;
	    case '-':
		if (oper) { sign=-1; c++; break; }
	    case '>':
	    case '<':
	    case '+':
	    case '/':
	    case '*':
	    case '%':
	    case '&':
	    case '=':
	    case ':':
		if (c[1]=='=') oper=(*c++) +16; else oper=*c;
		c++;
		break;
	    default: c++; break;
	    }
	    if (opex > 0) {
		sign=1;
		switch (oper) {
		case 'c': value=value2; break;
		case '-': value=value-value2; break;
		case '+': value=value+value2; break;
		case '*': value=value*value2; break;
		case '/': if (value2) value=value/value2; break;
		case '%': if (value2) value=value%value2; break;
		case '<': value=(value<value2); break;
		case '>': value=(value>value2); break;
		case '>'+16: value=(value>=value2); break;
		case '<'+16: value=(value<=value2); break;
		case '=': case '='+16: value=(value==value2); break;
		case '&': value = (value && value2); break;
		case ':': value = (value || value2); break;
		default: fprintf(stderr, "man2html: unknown operator %c.\n", oper);
		}
		oper=0;
	    }
	}
	if (*c==')') c++;
    }
    *result=value;
    return c;
}

static void trans_char(char *c, char s, char t)
{
    char *sl=c;
    int slash=0;
    while (*sl!='\n' || slash) {
	if (!slash) {
	    if (*sl==escapesym)
		slash=1;
	    else if (*sl==s)
		*sl=t;
	} else slash=0;
	sl++;
    }
}

static char *fill_words(char *c, char *words[], int *n)
{
    char *sl=c;
    int slash=0;
    int skipspace=0;
    *n=0;
    words[*n]=sl;
    while (*sl && (*sl!='\n' || slash)) {
	if (!slash) {
	    if (*sl=='"') {
		*sl='\a';
		skipspace=!skipspace;
	    } else if (*sl==escapesym) {
		slash=1;
                if (sl[1]=='\n')
                    *sl='\a';
	    } else if ((*sl==' ' || *sl=='\t') && !skipspace) {
		*sl='\n';
		if (words[*n]!=sl) (*n)++;
		words[*n]=sl+1;
	    }
	} else {
	    if (*sl=='"') {
		sl--;
		*sl='\n';
		if (words[*n]!=sl) (*n)++;
		sl++;
		while (*sl && *sl !='\n') sl++;
		words[*n]=sl;
		sl--;
	    }
	    slash=0;
	}
	sl++;
    }
    if (sl!=words[*n]) (*n)++;
    return sl;
}

static const char *abbrev_list[] = {
    "GSBG", "Getting Started ",
    "SUBG", "Customizing SunOS",
    "SHBG", "Basic Troubleshooting",
    "SVBG", "SunView User's Guide",
    "MMBG", "Mail and Messages",
    "DMBG", "Doing More with SunOS",
    "UNBG", "Using the Network",
    "GDBG", "Games, Demos &amp; Other Pursuits",
    "CHANGE", "SunOS 4.1 Release Manual",
    "INSTALL", "Installing SunOS 4.1",
    "ADMIN", "System and Network Administration",
    "SECUR", "Security Features Guide",
    "PROM", "PROM User's Manual",
    "DIAG", "Sun System Diagnostics",
    "SUNDIAG", "Sundiag User's Guide",
    "MANPAGES", "SunOS Reference Manual",
    "REFMAN", "SunOS Reference Manual",
    "SSI", "Sun System Introduction",
    "SSO", "System Services Overview",
    "TEXT", "Editing Text Files",
    "DOCS", "Formatting Documents",
    "TROFF", "Using <B>nroff</B> and <B>troff</B>",
    "INDEX", "Global Index",
    "CPG", "C Programmer's Guide",
    "CREF", "C Reference Manual",
    "ASSY", "Assembly Language Reference",
    "PUL", "Programming Utilities and Libraries",
    "DEBUG", "Debugging Tools",
    "NETP", "Network Programming",
    "DRIVER", "Writing Device Drivers",
    "STREAMS", "STREAMS Programming",
    "SBDK", "SBus Developer's Kit",
    "WDDS", "Writing Device Drivers for the SBus",
    "FPOINT", "Floating-Point Programmer's Guide",
    "SVPG", "SunView 1 Programmer's Guide",
    "SVSPG", "SunView 1 System Programmer's Guide",
    "PIXRCT", "Pixrect Reference Manual",
    "CGI", "SunCGI Reference Manual",
    "CORE", "SunCore Reference Manual",
    "4ASSY", "Sun-4 Assembly Language Reference",
    "SARCH", "<FONT SIZE=\"-1\">SPARC</FONT> Architecture Manual",
    "KR", "The C Programming Language",
    NULL, NULL };

static const char *lookup_abbrev(char *c)
{
    int i=0;

    if (!c) return "";
    while (abbrev_list[i] && strcmp(c,abbrev_list[i])) i=i+2;
    if (abbrev_list[i]) return abbrev_list[i+1];
    else return c;
}

static const char *section_list[] = {
    "1", "User Commands ",
    "1C", "User Commands",
    "1G", "User Commands",
    "1S", "User Commands",
    "1V", "User Commands ",
    "2", "System Calls",
    "2V", "System Calls",
    "3", "C Library Functions",
    "3C", "Compatibility Functions",
    "3F", "Fortran Library Routines",
    "3K", "Kernel VM Library Functions",
    "3L", "Lightweight Processes Library",
    "3M", "Mathematical Library",
    "3N", "Network Functions",
    "3R", "RPC Services Library",
    "3S", "Standard I/O Functions",
    "3V", "C Library Functions",
    "3X", "Miscellaneous Library Functions",
    "4", "Devices and Network Interfaces",
    "4F", "Protocol Families",
    "4I", "Devices and Network Interfaces",
    "4M", "Devices and Network Interfaces",
    "4N", "Devices and Network Interfaces",
    "4P", "Protocols",
    "4S", "Devices and Network Interfaces",
    "4V", "Devices and Network Interfaces",
    "5", "File Formats",
    "5V", "File Formats",
    "6", "Games and Demos",
    "7", "Environments, Tables, and Troff Macros",
    "7V", "Environments, Tables, and Troff Macros",
    "8", "Maintenance Commands",
    "8C", "Maintenance Commands",
    "8S", "Maintenance Commands",
    "8V", "Maintenance Commands",
    "L", "Local Commands",
/* for Solaris:
    "1", "User Commands",
    "1B", "SunOS/BSD Compatibility Package Commands",
    "1b", "SunOS/BSD Compatibility Package Commands",
    "1C", "Communication Commands ",
    "1c", "Communication Commands",
    "1F", "FMLI Commands ",
    "1f", "FMLI Commands",
    "1G", "Graphics and CAD Commands ",
    "1g", "Graphics and CAD Commands ",
    "1M", "Maintenance Commands",
    "1m", "Maintenance Commands",
    "1S", "SunOS Specific Commands",
    "1s", "SunOS Specific Commands",
    "2", "System Calls",
    "3", "C Library Functions",
    "3B", "SunOS/BSD Compatibility Library Functions",
    "3b", "SunOS/BSD Compatibility Library Functions",
    "3C", "C Library Functions",
    "3c", "C Library Functions",
    "3E", "C Library Functions",
    "3e", "C Library Functions",
    "3F", "Fortran Library Routines",
    "3f", "Fortran Library Routines",
    "3G", "C Library Functions",
    "3g", "C Library Functions",
    "3I", "Wide Character Functions",
    "3i", "Wide Character Functions",
    "3K", "Kernel VM Library Functions",
    "3k", "Kernel VM Library Functions",
    "3L", "Lightweight Processes Library",
    "3l", "Lightweight Processes Library",
    "3M", "Mathematical Library",
    "3m", "Mathematical Library",
    "3N", "Network Functions",
    "3n", "Network Functions",
    "3R", "Realtime Library",
    "3r", "Realtime Library",
    "3S", "Standard I/O Functions",
    "3s", "Standard I/O Functions",
    "3T", "Threads Library",
    "3t", "Threads Library",
    "3W", "C Library Functions",
    "3w", "C Library Functions",
    "3X", "Miscellaneous Library Functions",
    "3x", "Miscellaneous Library Functions",
    "4", "File Formats",
    "4B", "SunOS/BSD Compatibility Package File Formats",
    "4b", "SunOS/BSD Compatibility Package File Formats",
    "5", "Headers, Tables, and Macros",
    "6", "Games and Demos",
    "7", "Special Files",
    "7B", "SunOS/BSD Compatibility Special Files",
    "7b", "SunOS/BSD Compatibility Special Files",
    "8", "Maintenance Procedures",
    "8C", "Maintenance Procedures",
    "8c", "Maintenance Procedures",
    "8S", "Maintenance Procedures",
    "8s", "Maintenance Procedures",
    "9", "DDI and DKI",
    "9E", "DDI and DKI Driver Entry Points",
    "9e", "DDI and DKI Driver Entry Points",
    "9F", "DDI and DKI Kernel Functions",
    "9f", "DDI and DKI Kernel Functions",
    "9S", "DDI and DKI Data Structures",
    "9s", "DDI and DKI Data Structures",
    "L", "Local Commands",
*/
    NULL, "Misc. Reference Manual Pages",
    NULL, NULL
};

static const char *section_name(char *c)
{
    int i=0;

    if (!c) return "";
    while (section_list[i] && strcmp(c,section_list[i])) i=i+2;
    if (section_list[i+1]) return section_list[i+1];
    else return c;
}

static char *skip_till_newline(char *c)
{
    int lvl=0;

    while (*c && (*c!='\n' || lvl>0)) {
	if (*c=='\\') {
	    c++;
	    if (*c=='}') lvl--; else if (*c=='{') lvl++;
	}
	c++;
    }
    if (*c) c++;
    if (lvl<0 && newline_for_fun) {
	newline_for_fun = newline_for_fun+lvl;
	if (newline_for_fun<0) newline_for_fun=0;
    }
    return c;
}

static int ifelseval=0;

static char *scan_request(char *c)
{
				  /* BSD Mandoc stuff */
    static int mandoc_synopsis=0; /* True if we are in the synopsis section */
    static int mandoc_command=0;  /* True if this is mandoc page */
    static int mandoc_bd_options; /* Only copes with non-nested Bd's */

    int i,j,mode=0;
    char *h;
    char *wordlist[MAX_WORDLIST];
    int words;
    char *sl;
    STRDEF *owndef;
    while (*c==' ' || *c=='\t') c++;
    if (c[0]=='\n') return c+1;
    if (c[1]=='\n') j=1; else j=2;
    while (c[j]==' ' || c[j]=='\t') j++;
    if (c[0]==escapesym) {
	/* some pages use .\" .\$1 .\} */
	/* .\$1 is too difficult/stuppid */
	if (c[1]=='$') c=skip_till_newline(c);
	else
	    c = scan_escape(c+1);
    } else {
	i=V(c[0],c[1]);
	switch (i) {
	case V('a','b'):
	    h=c+j;
	    while (*h && *h !='\n') h++;
	    *h='\0';
	    if (scaninbuff && buffpos) {
		buffer[buffpos]='\0';
		puts(buffer);
	    }
	    /* fprintf(stderr, "%s\n", c+2); */
            return 0;
	    break;
	case V('d','i'):
	    {
		STRDEF *de;
		/* int oldcurpos=curpos; */
		c=c+j;
		i=V(c[0],c[1]);
		if (*c=='\n') { c++;break; }
		while (*c && *c!='\n') c++;
		c++;
		h=c;
		while (*c && strncmp(c,".di",3)) while (*c && *c++!='\n');
		*c='\0';
		de=strdef;
		while (de && de->nr !=i) de=de->next;
		if (!de) {
		    de=new STRDEF();
		    de->nr=i;
		    de->slen=0;
		    de->next=strdef;
		    de->st=NULL;
		    strdef=de;
		} else {
		    delete [] de->st;
		    de->slen=0;
		    de->st=NULL;
		}
		scan_troff(h,0,&de->st);
		if (*c) *c='.';
		while (*c && *c++!='\n');
		break;
	    }
	case V('d','s'):
	    mode=1;
	case V('a','s'):
	    {
		STRDEF *de;
		int oldcurpos=curpos;
		c=c+j;
		i=V(c[0],c[1]);
		j=0;
		while (c[j] && c[j]!='\n') j++;
		if (j<3) { c=c+j; break; }
		if (c[1]==' ') c=c+1; else c=c+2;
		while (isspace(*c)) c++;
		if (*c=='"') c++;
		de=strdef;
		while (de && de->nr != i) de=de->next;
		single_escape=1;
		curpos=0;
		if (!de) {
		    char *h;
		    de=new STRDEF();
		    de->nr=i;
		    de->slen=0;
		    de->next=strdef;
		    de->st=NULL;
		    strdef=de;
		    h=NULL;
		    c=scan_troff(c, 1, &h);
		    de->st=h;
		    de->slen=curpos;
		} else {
		    if (mode) {
			char *h=NULL;
			c=scan_troff(c, 1, &h);
			delete [] de->st;
			de->slen=0;
			de->st=h;
		    } else
			c=scan_troff(c,1,&de->st);
		    de->slen+=curpos;
		}
		single_escape=0;
		curpos=oldcurpos;
	    }
	    break;
	case V('b','r'):
	    if (still_dd) out_html("<DD>");
	    else out_html("<BR>\n");
	    curpos=0;
	    c=c+j;
	    if (c[0]==escapesym) { c=scan_escape(c+1); }
	    c=skip_till_newline(c);break;
	case V('c','2'):
	    c=c+j;
	    if (*c!='\n') { nobreaksym=*c; }
	    else nobreaksym='\'';
	    c=skip_till_newline(c);
	    break;
	case V('c','c'):
	    c=c+j;
	    if (*c!='\n') { controlsym=*c; }
	    else controlsym='.';
	    c=skip_till_newline(c);
	    break;
	case V('c','e'):
	    c=c+j;
	    if (*c=='\n') { i=1; }
	    else {
		i=0;
		while ('0'<=*c && *c<='9') {
		    i=i*10+*c-'0';
		    c++;
		}
	    }
	    c=skip_till_newline(c);
	    /* center next i lines */
	    if (i>0) {
		out_html("<CENTER>\n");
		while (i && *c) {
		    char *line=NULL;
		    c=scan_troff(c,1, &line);
		    if (line && strncmp(line, "<BR>", 4)) {
			out_html(line);
			out_html("<BR>\n");
                        delete [] line;
			i--;
		    }
		}
		out_html("</CENTER>\n");
		curpos=0;
	    }
	    break;
	case V('e','c'):
	    c=c+j;
	    if (*c!='\n') { escapesym=*c; }
	    else escapesym='\\';
	    break;
	    c=skip_till_newline(c);
	case V('e','o'):
	    escapesym='\0';
	    c=skip_till_newline(c);
	    break;
	case V('e','x'):
	    return 0;
	    break;
	case V('f','c'):
	    c=c+j;
	    if  (*c=='\n') {
		fieldsym=padsym='\0';
	    } else {
		fieldsym=c[0];
		padsym=c[1];
	    }
	    c=skip_till_newline(c);
	    break;
	case V('f','i'):
	    if (!fillout) {
		out_html(change_to_font(0));
		out_html(change_to_size('0'));
		out_html("</PRE>\n");
	    }
	    curpos=0;
	    fillout=1;
	    c=skip_till_newline(c);
	    break;
	case V('f','t'):
	    c=c+j;
	    if (*c=='\n') {
		out_html(change_to_font(0));
	    } else {
		if (*c==escapesym) {
		    int fn;
		    c=scan_expression(c, &fn);
		    c--;
		    out_html(change_to_font(fn));
		} else {
		    out_html(change_to_font(*c));
		    c++;
		}
	    }
	    c=skip_till_newline(c);
	    break;
	case V('e','l'):
	    /* .el anything : else part of if else */
	    if (ifelseval) {
		c=c+j;
		c[-1]='\n';
		c=scan_troff(c,1,NULL);
	    } else
		c=skip_till_newline(c+j);
	    break;
	case V('i','e'):
	    /* .ie c anything : then part of if else */
	case V('i','f'):
	    /* .if c anything
	     * .if !c anything
	     * .if N anything
	     * .if !N anything
	     * .if 'string1'string2' anything
	     * .if !'string1'string2' anything
	     */
	    c=c+j;
	    c=scan_expression(c, &i);
	    ifelseval=!i;
	    if (i) {
		*c='\n';
		c++;
		c=scan_troff(c,1,NULL);
	    } else
		c=skip_till_newline(c);
	    break;
	case V('i','g'):
	    {
		const char *endwith="..\n";
		i=3;
		c=c+j;
		if (*c!='\n' && *c != '\\') { /* Not newline or comment */
		    endwith=c-1;i=1;
		    c[-1]='.';
		    while (*c && *c!='\n') c++,i++;
		}
		c++;
		while (*c && strncmp(c,endwith,i)) while (*c++!='\n');
		while (*c && *c++!='\n');
		break;
	    }
	case V('n','f'):
	    if (fillout) {
		out_html(change_to_font(0));
		out_html(change_to_size('0'));
		out_html("<PRE>\n");
	    }
	    curpos=0;
	    fillout=0;
	    c=skip_till_newline(c);
	    break;
	case V('p','s'):
	    c=c+j;
	    if (*c=='\n') {
		out_html(change_to_size('0'));
	    } else {
		j=0;i=0;
		if (*c=='-') { j= -1;c++; } else if (*c=='+') { j=1;c++;}
		c=scan_expression(c, &i);
		if (!j) { j=1; if (i>5) i=i-10; }
		out_html(change_to_size(i*j));
	    }
	    c=skip_till_newline(c);
	    break;
	case V('s','p'):
	    c=c+j;
	    if (fillout) out_html("<P>"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	case V('s','o'):
	    {
		/* FILE *f; */
		char *buf;
		char *name=NULL;
		curpos=0;
		c=c+j;
		if (*c=='/') {
		    h=c;
		} else {
		    h=c-3;
		    h[0]='.';
		    h[1]='.';
		    h[2]='/';
		}
		while (*c!='\n') c++;
		*c='\0';
		scan_troff(h,1, &name);
		if (name[3]=='/') h=name+3; else h=name;
                /* this works alright, except for section 3 */
                buf=read_man_page(h);
                if (!buf) {

                    fprintf(stderr, "man2html: unable to open or read file %s.\n",
                            h);
                    out_html("<BLOCKQUOTE>"
                             "man2html: unable to open or read file.\n");
                    out_html(h);
                    out_html("</BLOCKQUOTE>\n");
                }
                else {
                    scan_troff(buf+1,0,NULL);
                }
                if (buf) delete [] buf;
                if (name) delete [] name;

		*c++='\n';
		break;
	    }
	case V('t','a'):
	    c=c+j;
	    j=0;
	    while (*c!='\n') {
		sl=scan_expression(c, &tabstops[j]);
		if (j>0 && (*c=='-' || *c=='+')) tabstops[j]+=tabstops[j-1];
		c=sl;
		while (*c==' ' || *c=='\t') c++;
		j++;
	    }
	    maxtstop=j;
	    curpos=0;
	    break;
	case V('t','i'):
	    /*while (itemdepth || dl_set[itemdepth]) {
		out_html("</DL>\n");
		if (dl_set[itemdepth]) dl_set[itemdepth]=0;
		else itemdepth--;
	    }*/
	    out_html("<BR>\n");
	    c=c+j;
	    c=scan_expression(c, &j);
	    for (i=0; i<j; i++) out_html("&nbsp;");
	    curpos=j;
	    c=skip_till_newline(c);
	    break;
	case V('t','m'):
	    c=c+j;
	    h=c;
	    while (*c!='\n') c++;
	    *c='\0';
	    /* fprintf(stderr,"%s\n", h); */
	    *c='\n';
	    break;
	case V('B',' '):
	case V('B','\n'):
	case V('I',' '):
	case V('I','\n'):
            /* parse one line in a certain font */
	    out_html(change_to_font(*c));
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('O','P'):  /* groff manpages use this construction */
            /* .OP a b : [ <B>a</B> <I>b</I> ] */
	    mode=1;
	    c[0]='B'; c[1]='I';
	    out_html(change_to_font('R'));
	    out_html("[");
	    curpos++;
	case V('B','R'):
	case V('B','I'):
	case V('I','B'):
	case V('I','R'):
	case V('R','B'):
	case V('R','I'):
	    {
		char font[2] ;
		font[0] = c[0]; font[1] = c[1];
		c=c+j;
		if (*c=='\n') c++;
		sl=fill_words(c, wordlist, &words);
		c=sl+1;
		/* .BR name (section)
		** indicates a link. It will be added in the output routine.
		*/
		for (i=0; i<words; i++) {
		    if (mode) { out_html(" "); curpos++; }
		    wordlist[i][-1]=' ';
		    out_html(change_to_font(font[i&1]));
		    scan_troff(wordlist[i],1,NULL);
		}
		out_html(change_to_font('R'));
		if (mode) { out_html(" ]"); curpos++;}
		out_html(NEWLINE); if (!fillout) curpos=0; else curpos++;
	    }
	    break;
	case V('D','T'):
	    for (j=0;j<20; j++) tabstops[j]=(j+1)*8;
	    maxtstop=20;
	    c=skip_till_newline(c); break;
	case V('I','P'):
	    sl=fill_words(c+j, wordlist, &words);
	    c=sl+1;
            if (!dl_set[itemdepth]) {
		out_html("<DL COMPACT>\n");
		dl_set[itemdepth]=1;
	    }
	    out_html("<DT>");
            if (words) {
		scan_troff(wordlist[0], 1,NULL);
	    }
	    out_html("<DD>");
	    curpos=0;
	    break;
	case V('T','P'):
	    if (!dl_set[itemdepth]) {
		out_html("<P><DL COMPACT>\n");
		dl_set[itemdepth]=1;
	    }
	    out_html("<DT>");
	    c=skip_till_newline(c);
	    /* somewhere a definition ends with '.TP' */
	    if (!*c) still_dd=1; else {
		c=scan_troff(c,1,NULL);
		out_html("<DD>");
	    }
	    curpos=0;
	    break;
	case V('I','X'):
            /* general index */
            c=skip_till_newline(c);
	    break;
        case V('P',' '):
        case V('P','\n'):
	case V('L','P'):
	case V('P','P'):
	    if (dl_set[itemdepth]) {
		out_html("</DL>\n");
		dl_set[itemdepth]=0;
	    }
	    if (fillout) out_html("<P>\n"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	case V('H','P'):
	    if (!dl_set[itemdepth]) {
		out_html("<DL COMPACT>");
		dl_set[itemdepth]=1;
	    }
	    out_html("<DT>\n");
	    still_dd=1;
	    c=skip_till_newline(c);
	    curpos=0;
	    break;
	case V('P','D'):
	    c=skip_till_newline(c);
	    break;
	case V('R','s'):	/* BSD mandoc */
	case V('R','S'):
	    sl=fill_words(c+j, wordlist, &words);
	    j=1;
	    if (words>0) scan_expression(wordlist[0], &j);
	    if (j>=0) {
		itemdepth++;
		dl_set[itemdepth]=0;
		out_html("<DL COMPACT><DT><DD>");
		c=skip_till_newline(c);
		curpos=0;
		break;
	    }
	case V('R','e'):	/* BSD mandoc */
	case V('R','E'):
	    if (itemdepth > 0) {
		if (dl_set[itemdepth]) out_html("</DL>");
		out_html("</DL>\n");
		itemdepth--;
	    }
	    c=skip_till_newline(c);
	    curpos=0;
	    break;
	case V('S','B'):
	    out_html(change_to_size(-1));
	    out_html(change_to_font('B'));
	    c=scan_troff(c+j, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(change_to_size('0'));
	    break;
	case V('S','M'):
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html(change_to_size(-1));
	    trans_char(c,'"','\a');
	    c=scan_troff(c,1,NULL);
	    out_html(change_to_size('0'));
	    break;
	case V('S','s'):	/* BSD mandoc */
	    mandoc_command = 1;
	case V('S','S'):
	    mode=1;
	case V('S','h'):	/* BSD mandoc */
				/* hack for fallthru from above */
	    mandoc_command = !mode || mandoc_command;
	case V('S','H'):
	    c=c+j;
	    if (*c=='\n') c++;
	    while (itemdepth || dl_set[itemdepth]) {
		out_html("</DL>\n");
		if (dl_set[itemdepth]) dl_set[itemdepth]=0;
		else if (itemdepth > 0) itemdepth--;
	    }
	    out_html(change_to_font(0));
	    out_html(change_to_size(0));
	    if (!fillout) {
		fillout=1;
		out_html("</PRE>");
	    }
	    trans_char(c,'"', '\a');
	    /* &nbsp; for mosaic users */
            if (section) {
                out_html("</div>\n");
                section=0;
            }
	    if (mode) out_html("\n<H3>");
	    else out_html("\n<H2>");
	    mandoc_synopsis = strncmp(c, "SYNOPSIS", 8) == 0;
	    c = mandoc_command ? scan_troff_mandoc(c,1,NULL) : scan_troff(c,1,NULL);
	    if (mode) out_html("</H3>\n");
	    else out_html("</H2>\n");
            out_html("<div>\n");

            section=1;
	    curpos=0;
	    break;
        case V('S','x'): // reference to a section header
	    out_html(change_to_font('B'));
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
            break;
	case V('T','S'):
	    c=scan_table(c);
	    break;
	case V('D','t'):	/* BSD mandoc */
	    mandoc_command = 1;
	case V('T','H'):
	    if (!output_possible) {
		sl = fill_words(c+j, wordlist, &words);
		if (words>1) {
		    for (i=1; i<words; i++) wordlist[i][-1]='\0';
		    *sl='\0';
		    output_possible=1;
		    out_html( DOCTYPE"<HTML><HEAD><TITLE>Manpage of ");
		    out_html( wordlist[0]);
		    out_html( "</TITLE>\n");
                    out_html( "<link rel=\"stylesheet\" href=\"KDE_COMMON_DIR/kde-default.css\" type=\"text/css\">\n" );
                    out_html( "</HEAD>\n\n" );
                    out_html("<BODY BGCOLOR=\"#FFFFFF\">\n\n" );
                    out_html("<div id=\"headline\" style=\"position : absolute; height : 85px; z-index :\n" );
                    out_html("100; background : transparent; text-align : center; text-transform:\n" );
                    out_html("smallcaps; width : 100%; top : 0px; left : 0px; width : 100%; color :\n" );
                    out_html("#000000;\">\n" );
                    out_html("\n" );
                    out_html("<H1>" );
                    out_html( wordlist[0] );
                    out_html("</H1>\n" );
                    out_html("</div>\n" );
                    out_html("<div id=\"navbackground\" style=\"position : absolute; width : 100%; height\n" );
                    out_html(": 124px; background-image : url('KDE_COMMON_DIR/doctop2.png'); z-index : 5; left\n" );
                    out_html(": 0px; top : 0px; padding : 0px;\"> <div id=\"bulb1\" style=\"padding : 0px;\n" );
                    out_html("position : absolute; z-index : 15; width : 150px; height : 85px; top :\n" );
                    out_html("0px; left : 0px; background : url('KDE_COMMON_DIR/doctop1.png') repeat;\"></div>\n" );
                    out_html("<div id=\"gradient\" style=\"position : absolute; width : 275px; height :\n" );
                    out_html("85px; z-index : 19px; top : 0px; padding : 0px; left : 150px;\n" );
                    out_html("background-image : url('KDE_COMMON_DIR/doctop1a.png'); background-repeat :\n" );
                    out_html("no-repeat; background-color : transparent; visibility : visible;\"></div>\n" );
                    out_html("\n" );
                    out_html("<div id=\"bulb-bit\" style=\"position : absolute; width : 100%; height :\n" );
                    out_html("25px; top : 85px; left : 0px; background-image :\n" );
                    out_html("url('KDE_COMMON_DIR/doctop1b.png'); background-repeat : no-repeat;\n" );
                    out_html("background-color : transparent; z-index : 5;\"></div></div>\n" );
                    out_html("<h1>" );
                    out_html( wordlist[0] );
                    out_html( "</h1>\n" );
                    out_html("\n" );
                    out_html("Section: " );
                    if (words>4) out_html(wordlist[4]);
		    else
			out_html(section_name(wordlist[1]));
		    out_html(" (");
		    out_html(wordlist[1]);
                    out_html(")\n");
		    *sl='\n';
		    if (mandoc_command) out_html("<BR>BSD mandoc<BR>");
		}
		c=sl+1;
	    } else c=skip_till_newline(c);
	    curpos=0;
	    break;
       case V('T','X'): {
	    sl=fill_words(c+j, wordlist, &words);
	    *sl='\0';
	    out_html(change_to_font('I'));
	    if (words>1) wordlist[1][-1]='\0';
	    const char *c2=lookup_abbrev(wordlist[0]);
	    curpos+=strlen(c2);
	    out_html(c2);
	    out_html(change_to_font('R'));
	    if (words>1)
		out_html(wordlist[1]);
	    *sl='\n';
	    c=sl+1;
          }
          break;
	case V('r','m'):
            /* .rm xx : Remove request, macro or string */
	case V('r','n'):
            /* .rn xx yy : Rename request, macro or string xx to yy */
	    {
		STRDEF *de;
		c=c+j;
		i=V(c[0],c[1]);
		c=c+2;
		while (isspace(*c) && *c!='\n') c++;
		j=V(c[0],c[1]);
		while (*c && *c!='\n') c++;
		c++;
		de=strdef;
		while (de && de->nr!=j) de=de->next;
		if (de) {
		    delete [] de->st;
                    de->st=0;
		    de->nr=0;
		}
		de=strdef;
		while (de && de->nr!=i) de=de->next;
		if (de) de->nr=j;
		break;
	    }
	case V('n','x'):
            /* .nx filename : next file. */
	case V('i','n'):
            /* .in +-N : Indent */
	    c=skip_till_newline(c);
	    break;
	case V('n','r'):
            /* .nr R +-N M: define and set number register R by +-N;
	    **  auto-increment by M
	    */
	    {
		INTDEF *intd;
		c=c+j;
		i=V(c[0],c[1]);
		c=c+2;
		intd=intdef;
		while (intd && intd->nr!=i) intd=intd->next;
		if (!intd) {
		    intd = new INTDEF();
		    intd->nr=i;
		    intd->val=0;
		    intd->incr=0;
		    intd->next=intdef;
		    intdef=intd;
		}
		while (*c==' ' || *c=='\t') c++;
		c=scan_expression(c,&intd->val);
		if (*c!='\n') {
		    while (*c==' ' || *c=='\t') c++;
		    c=scan_expression(c,&intd->incr);
		}
		c=skip_till_newline(c);
		break;
	    }
	case V('a','m'):
            /* .am xx yy : append to a macro. */
            /* define or handle as .ig yy */
	    mode=1;
	case V('d','e'):
            /* .de xx yy : define or redefine macro xx; end at .yy (..) */
            /* define or handle as .ig yy */
	    {
		STRDEF *de;
		int olen=0;
		c=c+j;
		sl=fill_words(c, wordlist, &words);
		i=V(c[0],c[1]);j=2;
		if (words==1) {
                    wordlist[1] = qstrdup("..");
                } else {
		    wordlist[1]--;
		    wordlist[1][0]='.';
		    j=3;
		}
		c=sl+1;
		sl=c;
		while (*c && strncmp(c,wordlist[1],j)) c=skip_till_newline(c);
		de=defdef;
		while (de && de->nr!= i) de=de->next;
		if (mode && de) olen=strlen(de->st);
		j=olen+c-sl;
		h = stralloc(j*2+4);
		if (h) {
		    for (j=0; j<olen; j++)
			h[j]=de->st[j];
		    if (!j || h[j-1]!='\n')
			h[j++]='\n';
		    while (sl!=c) {
			if (sl[0]=='\\' && sl[1]=='\\') {
			    h[j++]='\\'; sl++;
			} else
			    h[j++]=*sl;
			sl++;
		    }
		    h[j]=0;
		    if (de) {
                        delete [] de->st;
			de->st=h;
		    } else {
			de = new STRDEF();
			de->nr=i;
			de->next=defdef;
			de->st=h;
			defdef=de;
		    }
		}
                if (words==1) {
                    delete [] wordlist[1];
                }
	    }
	    c=skip_till_newline(c);
	    break;
	case V('B','l'):	/* BSD mandoc */
	  {
	    char list_options[NULL_TERMINATED(MED_STR_MAX)];
	    char *nl = strchr(c,'\n');
	    c=c+j;
	    if (dl_set[itemdepth]) {  /* These things can nest. */
	        itemdepth++;
	    }
	    if (nl) {		  /* Parse list options */
	        strlimitcpy(list_options, c, nl - c, MED_STR_MAX);
	    }
	    if (strstr(list_options, "-bullet")) { /* HTML Unnumbered List */
	        dl_set[itemdepth] = BL_BULLET_LIST;
	        out_html("<UL>\n");
	    }
	    else if (strstr(list_options, "-enum")) { /* HTML Ordered List */
	        dl_set[itemdepth] = BL_ENUM_LIST;
	        out_html("<OL>\n");
	    }
	    else {		  /* HTML Descriptive List */
	        dl_set[itemdepth] = BL_DESC_LIST;
	        out_html("<DL COMPACT>\n");
	    }
	    if (fillout) out_html("<P>\n"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	  }
	case V('E','l'):	/* BSD mandoc */
	    c=c+j;
	    if (dl_set[itemdepth] & BL_DESC_LIST) {
		out_html("</DL>\n");
	    }
	    else if (dl_set[itemdepth] & BL_BULLET_LIST) {
		out_html("</UL>\n");
	    }
	    else if (dl_set[itemdepth] & BL_ENUM_LIST) {
		out_html("</OL>\n");
	    }
	    dl_set[itemdepth]=0;
	    if (itemdepth > 0) itemdepth--;
	    if (fillout) out_html("<P>\n"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	case V('I','t'):	/* BSD mandoc */
	    c=c+j;
	    if (strncmp(c, "Xo", 2) == 0 && isspace(*(c+2))) {
	        c = skip_till_newline(c);
	    }
	    if (dl_set[itemdepth] & BL_DESC_LIST) {
	        out_html("<DT>");
		out_html(change_to_font('B'));
	        if (*c=='\n') {	  /* Don't allow embedded comms after a newline */
		    c++;
		    c=scan_troff(c,1,NULL);
		}
		else {		  /* Do allow embedded comms on the same line. */
		    c=scan_troff_mandoc(c,1,NULL);
		}
		out_html(change_to_font('R'));
		out_html(NEWLINE);
		out_html("<DD>");
	    }
	    else if (dl_set[itemdepth] & (BL_BULLET_LIST | BL_ENUM_LIST)) {
	        out_html("<LI>");
		c=scan_troff_mandoc(c,1,NULL);
		out_html(NEWLINE);
	    }
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('B','k'):	/* BSD mandoc */
	case V('E','k'):	/* BSD mandoc */
	case V('D','d'):	/* BSD mandoc */
	case V('O','s'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('B','t'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    out_html(" is currently in beta test.");
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('B','x'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html("BSD ");
	    c=scan_troff_mandoc(c, 1, NULL);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('D','l'):	/* BSD mandoc */
	    c=c+j;
	    out_html(NEWLINE);
	    out_html("<BLOCKQUOTE>");
	    out_html(change_to_font('L'));
	    if (*c=='\n') c++;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html("</BLOCKQUOTE>");
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('B','d'):	/* BSD mandoc */
	  {			/* Seems like a kind of example/literal mode */
	    char bd_options[NULL_TERMINATED(MED_STR_MAX)];
	    char *nl = strchr(c,'\n');
	    c=c+j;
	    if (nl) {
	      strlimitcpy(bd_options, c, nl - c, MED_STR_MAX);
	    }
	    out_html(NEWLINE);
	    mandoc_bd_options = 0; /* Remember options for terminating Bl */
	    if (strstr(bd_options, "-offset indent")) {
	        mandoc_bd_options |= BD_INDENT;
	        out_html("<BLOCKQUOTE>\n");
	    }
	    if (   strstr(bd_options, "-literal")
		|| strstr(bd_options, "-unfilled")) {
	        if (fillout) {
		    mandoc_bd_options |= BD_LITERAL;
		    out_html(change_to_font(0));
		    out_html(change_to_size('0'));
		    out_html("<PRE>\n");
		}
		curpos=0;
		fillout=0;
	    }
	    c=skip_till_newline(c);
	    break;
	  }
	case V('E','d'):	/* BSD mandoc */
	    if (mandoc_bd_options & BD_LITERAL) {
	        if (!fillout) {
		    out_html(change_to_font(0));
		    out_html(change_to_size('0'));
		    out_html("</PRE>\n");
		}
	    }
	    if (mandoc_bd_options & BD_INDENT)
	        out_html("</BLOCKQUOTE>\n");
	    curpos=0;
	    fillout=1;
	    c=skip_till_newline(c);
	    break;
	case V('B','e'):	/* BSD mandoc */
	    c=c+j;
	    if (fillout) out_html("<P>"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	case V('X','r'):	/* BSD mandoc */
	    {
	      /* Translate xyz 1 to xyz(1)
	       * Allow for multiple spaces.  Allow the section to be missing.
	       */
	      char buff[NULL_TERMINATED(MED_STR_MAX)];
	      char *bufptr;
	      trans_char(c,'"','\a');
	      bufptr = buff;
	      c = c+j;
	      if (*c == '\n') c++; /* Skip spaces */
	      while (isspace(*c) && *c != '\n') c++;
	      while (isalnum(*c)) { /* Copy the xyz part */
		*bufptr = *c;
		bufptr++; if (bufptr >= buff + MED_STR_MAX) break;
		c++;
	      }
	      while (isspace(*c) && *c != '\n') c++;	/* Skip spaces */
	      if (isdigit(*c)) { /* Convert the number if there is one */
		*bufptr = '(';
		bufptr++;
		if (bufptr < buff + MED_STR_MAX) {
		  while (isalnum(*c)) {
		    *bufptr = *c;
		    bufptr++; if (bufptr >= buff + MED_STR_MAX) break;
		    c++;
		  }
		  if (bufptr < buff + MED_STR_MAX) {
		    *bufptr = ')';
		    bufptr++;
		  }
		}
	      }

	      while (*c != '\n') { /* Copy the remainder */
		if (!isspace(*c)) {
		  *bufptr = *c;
		  bufptr++; if (bufptr >= buff + MED_STR_MAX) break;
		}
		c++;
	      }
	      *bufptr = '\n';
              bufptr[1] = 0;
	      scan_troff_mandoc(buff, 1, NULL);

	      out_html(NEWLINE);
	      if (fillout) curpos++; else curpos=0;
	    }
	    break;
	case V('F','l'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    out_html("-");
	    if (*c!='\n') {
	        out_html(change_to_font('B'));
	        c=scan_troff_mandoc(c, 1, NULL);
	        out_html(change_to_font('R'));
            }
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('P','a'):	/* BSD mandoc */
	case V('P','f'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('P','p'):	/* BSD mandoc */
	    if (fillout) out_html("<P>\n"); else {
		out_html(NEWLINE);
		NEWLINE[0]='\n';
	    }
	    curpos=0;
	    c=skip_till_newline(c);
	    break;
	case V('D','q'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html("``");
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html("''");
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('O','p'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html(change_to_font('R'));
	    out_html("[");
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html("]");
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('O','o'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html(change_to_font('R'));
	    out_html("[");
	    c=scan_troff_mandoc(c, 1, NULL);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('O','c'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html("]");
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('P','q'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html("(");
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(")");
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('Q','l'):	/* BSD mandoc */
	  {			/* Single quote first word in the line */
	    char *sp;
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    sp = c;
	    do {		/* Find first whitespace after the
				 * first word that isn't a mandoc macro
				 */
	      while (*sp && isspace(*sp)) sp++;
	      while (*sp && !isspace(*sp)) sp++;
	    } while (*sp && isupper(*(sp-2)) && islower(*(sp-1)));

				/* Use a newline to mark the end of text to
				 * be quoted
				 */
	    if (*sp) *sp = '\n';
	    out_html("`");	/* Quote the text */
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html("'");
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	  }
	case V('S','q'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html("`");
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html("'");
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('A','r'):	/* BSD mandoc */
            /* parse one line in italics */
	    out_html(change_to_font('I'));
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') {	/* An empty Ar means "file ..." */
	        out_html("file ...");
	    }
	    else {
	        c=scan_troff_mandoc(c, 1, NULL);
	    }
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('A','d'):	/* BSD mandoc */
	case V('E','m'):	/* BSD mandoc */
	case V('V','a'):	/* BSD mandoc */
	case V('X','c'):	/* BSD mandoc */
            /* parse one line in italics */
	    out_html(change_to_font('I'));
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('N','d'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html(" - ");
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('N','m'):	/* BSD mandoc */
	  {
	    static char mandoc_name[NULL_TERMINATED(SMALL_STR_MAX)] = "";
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (mandoc_synopsis) {    /* Break lines only in the Synopsis.
				       * The Synopsis section seems to be treated
				       * as a special case - Bummer!
				       */
	        static int count = 0; /* Don't break on the first Nm */
	        if (count) {
		    out_html("<BR>");
		}
		else {
		    char *end = strchr(c, '\n');
		    if (end) {	/* Remember the name for later. */
		        strlimitcpy(mandoc_name, c, end - c, SMALL_STR_MAX);
		    }
		}
		count++;
	    }
	    out_html(change_to_font('B'));
	    while (*c == ' '|| *c == '\t') c++;
	    if (*c == '\n') {	/* If Nm has no argument, use one from an earlier
				 * Nm command that did have one.  Hope there aren't
				 * too many commands that do this.
				 */
	        out_html(mandoc_name);
	    }
	    else {
	        c=scan_troff_mandoc(c, 1, NULL);
	    }
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	  }
	case V('C','d'):	/* BSD mandoc */
	case V('C','m'):	/* BSD mandoc */
	case V('I','c'):	/* BSD mandoc */
	case V('M','s'):	/* BSD mandoc */
	case V('O','r'):	/* BSD mandoc */
	case V('S','y'):	/* BSD mandoc */
            /* parse one line in bold */
	    out_html(change_to_font('B'));
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('D','v'):	/* BSD mandoc */
	case V('E','v'):	/* BSD mandoc */
	case V('F','r'):	/* BSD mandoc */
	case V('L','i'):	/* BSD mandoc */
	case V('N','o'):	/* BSD mandoc */
	case V('N','s'):	/* BSD mandoc */
	case V('T','n'):	/* BSD mandoc */
	case V('n','N'):	/* BSD mandoc */
	    trans_char(c,'"','\a');
	    c=c+j;
	    if (*c=='\n') c++;
	    out_html(change_to_font('B'));
	    c=scan_troff_mandoc(c, 1, NULL);
	    out_html(change_to_font('R'));
	    out_html(NEWLINE);
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('%','A'):	/* BSD mandoc biblio stuff */
	case V('%','D'):
	case V('%','N'):
	case V('%','O'):
	case V('%','P'):
	case V('%','Q'):
	case V('%','V'):
	    c=c+j;
	    if (*c=='\n') c++;
	    c=scan_troff(c, 1, NULL); /* Don't allow embedded mandoc coms */
	    if (fillout) curpos++; else curpos=0;
	    break;
	case V('%','B'):
	case V('%','J'):
	case V('%','R'):
	case V('%','T'):
	    c=c+j;
	    out_html(change_to_font('I'));
	    if (*c=='\n') c++;
	    c=scan_troff(c, 1, NULL); /* Don't allow embedded mandoc coms */
	    out_html(change_to_font('R'));
	    if (fillout) curpos++; else curpos=0;
	    break;
	default:
            /* search macro database of self-defined macros */
	    owndef = defdef;
	    while (owndef && owndef->nr!=i) owndef=owndef->next;
	    if (owndef) {
		char **oldargument;
		int deflen;
		int onff;
		sl=fill_words(c+j, wordlist, &words);
		c=sl+1;
		*sl='\0';
		for (i=1;i<words; i++) wordlist[i][-1]='\0';
		for (i=0; i<words; i++) {
		    char *h=NULL;
		    if (mandoc_command) {
		      scan_troff_mandoc(wordlist[i],1,&h);
		    }
		    else {
		      scan_troff(wordlist[i],1,&h);
		    }
		    wordlist[i] = qstrdup(h);
                    delete [] h;
		}
		for (i=words;i<20; i++) wordlist[i]=NULL;
		deflen = strlen(owndef->st);
		for (i=0; (owndef->st[deflen+2+i]=owndef->st[i]); i++);
		oldargument=argument;
		argument=wordlist;
		onff=newline_for_fun;
		if (mandoc_command) {
		  scan_troff_mandoc(owndef->st+deflen+2, 0, NULL);
		}
		else {
                    scan_troff(owndef->st+deflen+2, 0, NULL);
		}
		newline_for_fun=onff;
		argument=oldargument;
		for (i=0; i<words; i++) delete [] wordlist[i];
		*sl='\n';
	    }
	    else if (mandoc_command &&
		     ((isupper(*c) && islower(*(c+1)))
		      || (islower(*c) && isupper(*(c+1))))
		     ) {	/* Let through any BSD mandoc commands that haven't
				 * been delt with.
				 * I don't want to miss anything out of the text.
				 */
	        char buf[4];
		strncpy(buf,c,2);
		buf[2] = ' ';
		buf[3] = '\0';
		out_html(buf);	/* Print the command (it might just be text). */
	        c=c+j;
	        trans_char(c,'"','\a');
		if (*c=='\n') c++;
		out_html(change_to_font('R'));
		c=scan_troff(c, 1, NULL);
		out_html(NEWLINE);
		if (fillout) curpos++; else curpos=0;
	    }
	    else {
		c=skip_till_newline(c);
	    }
	    break;
	}
    }
    if (fillout) { out_html(NEWLINE); curpos++; }
    NEWLINE[0]='\n';
    return c;
}
/*
static void flush(void)
{
}
*/
static int contained_tab=0;
static int mandoc_line=0;	/* Signals whether to look for embedded mandoc
				 * commands.
				 */

static char *scan_troff(char *c, int san, char **result)
{   /* san : stop at newline */
    char *h;
    char intbuff[NULL_TERMINATED(MED_STR_MAX)];
    int ibp=0;
    /* int i; */
#define FLUSHIBP  if (ibp) { intbuff[ibp]=0; out_html(intbuff); ibp=0; }
    char *exbuffer;
    int exbuffpos, exbuffmax, exscaninbuff, exnewline_for_fun;
    int usenbsp=0;

    exbuffer=buffer;
    exbuffpos=buffpos;
    exbuffmax=buffmax;
    exnewline_for_fun=newline_for_fun;
    exscaninbuff=scaninbuff;
    newline_for_fun=0;
    if (result) {
	if (*result) {
	    buffer=*result;
	    buffpos=strlen(buffer);
	    buffmax=buffpos;
	} else {
            buffer = stralloc(LARGE_STR_MAX);
            buffpos=0;
            buffmax=LARGE_STR_MAX;
	}
	scaninbuff=1;
    }
    h=c;
    /* start scanning */

    while (h && *h && (!san || newline_for_fun || *h!='\n')) {

	if (*h==escapesym) {
	    h++;
	    FLUSHIBP;
	    h = scan_escape(h);
	} else if (*h==controlsym && h[-1]=='\n') {
	    h++;
	    FLUSHIBP;
	    h = scan_request(h);
	    if (san && h[-1]=='\n') h--;
	} else if (mandoc_line
		   && *(h) && isupper(*(h))
		   && *(h+1) && islower(*(h+1))
		   && *(h+2) && isspace(*(h+2))) {
	    /* BSD imbedded command eg ".It Fl Ar arg1 Fl Ar arg2" */
	    FLUSHIBP;
	    h = scan_request(h);
	    if (san && h[-1]=='\n') h--;
	} else if (*h==nobreaksym && h[-1]=='\n') {
	    h++;
	    FLUSHIBP;
	    h = scan_request(h);
	    if (san && h[-1]=='\n') h--;
	} else {
	    /* int mx; */
	    if (still_dd && isalnum(*h) && h[-1]=='\n') {
		/* sometimes a .HP request is not followed by a .br request */
		FLUSHIBP;
		out_html("<DD>");
		curpos=0;
		still_dd=0;
	    }
	    switch (*h) {
	    case '&':
		intbuff[ibp++]='&';
		intbuff[ibp++]='a';
		intbuff[ibp++]='m';
		intbuff[ibp++]='p';
		intbuff[ibp++]=';';
		curpos++;
		break;
	    case '<':
		intbuff[ibp++]='&';
		intbuff[ibp++]='l';
		intbuff[ibp++]='t';
		intbuff[ibp++]=';';
		curpos++;
		break;
	    case '>':
		intbuff[ibp++]='&';
		intbuff[ibp++]='g';
		intbuff[ibp++]='t';
		intbuff[ibp++]=';';
		curpos++;
		break;
	    case '"':
		intbuff[ibp++]='&';
		intbuff[ibp++]='q';
		intbuff[ibp++]='u';
		intbuff[ibp++]='o';
		intbuff[ibp++]='t';
		intbuff[ibp++]=';';
		curpos++;
		break;
	    case '\n':
		if (h[-1]=='\n' && fillout) {
		    intbuff[ibp++]='<';
		    intbuff[ibp++]='P';
		    intbuff[ibp++]='>';
		}
		if (contained_tab && fillout) {
		    intbuff[ibp++]='<';
		    intbuff[ibp++]='B';
		    intbuff[ibp++]='R';
		    intbuff[ibp++]='>';
		}
		contained_tab=0;
		curpos=0;
		usenbsp=0;
		intbuff[ibp++]='\n';
		break;
	    case '\t':
		{
		    int curtab=0;
		    contained_tab=1;
		    FLUSHIBP;
		    /* like a typewriter, not like TeX */
		    tabstops[19]=curpos+1;
		    while (curtab<maxtstop && tabstops[curtab]<=curpos)
			curtab++;
		    if (curtab<maxtstop) {
			if (!fillout) {
			    while (curpos<tabstops[curtab]) {
				intbuff[ibp++]=' ';
				if (ibp>480) { FLUSHIBP; }
				curpos++;
			    }
			} else {
			    out_html("<TT>");
			    while (curpos<tabstops[curtab]) {
				out_html("&nbsp;");
				curpos++;
			    }
			    out_html("</TT>");
			}
		    }
		}
		break;
	    default:
		if (*h==' ' && (h[-1]=='\n' || usenbsp)) {
		    FLUSHIBP;
		    if (!usenbsp && fillout) {
			out_html("<BR>");
			curpos=0;
		    }
		    usenbsp=fillout;
		    if (usenbsp) out_html("&nbsp;"); else intbuff[ibp++]=' ';
		} else if (*h>31 && *h<127) intbuff[ibp++]=*h;
		else if (((unsigned char)(*h))>127) {
                    intbuff[ibp++]=*h;

#if 0
		    intbuff[ibp++]='&';
		    intbuff[ibp++]='#';
		    intbuff[ibp++]='0'+((unsigned char)(*h))/100;
		    intbuff[ibp++]='0'+(((unsigned char)(*h))%100)/10;
		    intbuff[ibp++]='0'+((unsigned char)(*h))%10;
		    intbuff[ibp++]=';';
#endif

		}
		curpos++;
		break;
	    }
	    if (ibp > (MED_STR_MAX - 20)) FLUSHIBP;
	    h++;
	}
    }
    FLUSHIBP;
    if (buffer) buffer[buffpos]='\0';
    if (san && *h) h++;
    newline_for_fun=exnewline_for_fun;
    if (result) {
	*result = buffer;
	buffer=exbuffer;
	buffpos=exbuffpos;
	buffmax=exbuffmax;
	scaninbuff=exscaninbuff;
    }

    return h;
}


static char *scan_troff_mandoc(char *c, int san, char **result)
{
    char *ret;
    char *end = c;
    int oldval = mandoc_line;
    mandoc_line = 1;
    while (*end && *end != '\n') {
        end++;
    }

    if (end > c + 2
        && ispunct(*(end - 1))
	&& isspace(*(end - 2)) && *(end - 2) != '\n') {
      /* Don't format lonely punctuation E.g. in "xyz ," format
       * the xyz and then append the comma removing the space.
       */
        *(end - 2) = '\n';
	ret = scan_troff(c, san, result);
        *(end - 2) = *(end - 1);
        *(end - 1) = ' ';
    }
    else {
	ret = scan_troff(c, san, result);
    }
    mandoc_line = oldval;
    return ret;
}

void scan_man_page(const char *man_page)
{
    if (!man_page)
        return;

    output_possible = false;
    int strLength = strlen(man_page);
    char *buf = new char[strLength + 2];
    strcpy(buf+1, man_page);
    buf[0] = '\n';

    scan_troff(buf+1,0,NULL);
    while (itemdepth || dl_set[itemdepth]) {
        out_html("</DL>\n");
        if (dl_set[itemdepth]) dl_set[itemdepth]=0;
        else if (itemdepth > 0) itemdepth--;
    }

    out_html(change_to_font(0));
    out_html(change_to_size(0));
    if (!fillout) {
	fillout=1;
	out_html("</PRE>");
    }
    out_html(NEWLINE);
    if (section) {
        out_html("<div style=\"margin-left: 2cm\">\n");
        section = 0;
    }
    if (output_possible) {
        output_real( "<div id=\"bottom-nav\" style=\"position : relative; width : 100%;\n");
        output_real( "height : 185px; left : 0px; right : 0px; top : 0px; margin-top: 100px;\n");
        output_real( "background-image : url('KDE_COMMON_DIR/bottom1.png'); background-repeat :\n");
        output_real( "repeat-x; background-color : transparent; margin-left: 0px;\n");
        output_real( "margin-right: 0px; z-index : 25;\">\n");
        output_real( "<img src=\"KDE_COMMON_DIR/bottom2.png\" align=\"right\" height=\"59\" width=\"227\" alt=\"KDE Logo\">\n");
        output_real( "<div id=\"navtable2\" style=\"width : 100%; margin-left: 0px; margin-right:\n");
        output_real( "0px; z-index : 15; background-color : transparent;\"></div>\n");
        output_real( "</div>  \n");
	output_real("</BODY>\n</HTML>\n");
    }
    delete [] buf;

    // reinit static variables for reuse
    STRDEF *cursor = defdef;
    while (cursor) {
        defdef = cursor->next;
        if (cursor->st)
            delete [] cursor->st;
        delete cursor;
        cursor = defdef;
    }
    defdef = 0;

    cursor = strdef;
    while (cursor) {
        strdef = cursor->next;
        if (cursor->st)
            delete [] cursor->st;
        delete cursor;
        cursor = strdef;
    }
    strdef = 0;

    cursor = chardef;
    while (cursor) {
        chardef = cursor->next;
        delete [] cursor->st;
        delete cursor;
        cursor = chardef;
    }
    chardef = 0;

    INTDEF *cursor2 = intdef;
    while (cursor2) {
        intdef = cursor2->next;
        delete cursor2;
        cursor2 = intdef;
    }
    intdef = 0;

    delete [] buffer;
    buffer = 0;

    escapesym='\\';
    nobreaksym='\'';
    controlsym='.';
    fieldsym=0;
    padsym=0;

    buffpos=0;
    buffmax=0;
    scaninbuff=0;
    itemdepth=0;
    for (int i = 0; i < 20; i++)
        dl_set[i] = 0;
    still_dd=0;
    for (int i = 0; i < 12; i++)
        tabstops[i] = (i+1)*8;
    maxtstop=12;
    curpos=0;

    argument = 0;

}

#ifdef SIMPLE_MAN2HTML
void output_real(const char *insert)
{
    printf("%s", insert);
}

char *read_man_page(const char *filename)
{
    int man_pipe = 0;
    char *man_buf = NULL;

    FILE *man_stream = NULL;
    struct stat stbuf;
    size_t buf_size;
    if (stat(filename, &stbuf) == -1) {
        return NULL;
    }
    if (!S_ISREG(stbuf.st_mode)) {
        return NULL;
    }
    buf_size = stbuf.st_size;
    man_buf = stralloc(buf_size+5);
    man_pipe = 0;
    man_stream = fopen(filename, "r");
    if (man_stream) {
        man_buf[0] = '\n';
        if (fread(man_buf+1, 1, buf_size, man_stream) == buf_size) {
            man_buf[buf_size] = '\n';
            man_buf[buf_size + 1] = man_buf[buf_size + 2] = '\0';
        }
        else {
            man_buf = NULL;
        }
        fclose(man_stream);
    }
    return man_buf;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;
    if (chdir(argv[1])) {
        char *buf = read_man_page(argv[1]);
        if (buf) {
            scan_man_page(buf);
            delete [] buf;
        }
    } else {
        DIR *dir = opendir(".");
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            fprintf(stderr, "converting %s\n", ent->d_name);
            char *buf = read_man_page(ent->d_name);
            if (buf) {
                scan_man_page(buf);
                delete [] buf;
            }
        }
        closedir(dir);
    }
    return 0;
}


#endif
