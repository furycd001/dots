/* $Id: printf.c,v 2.8.2.1 2001/09/23 11:26:36 ossi Exp $ */
/*

Copyright 1988, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * printf.c - working horse of error.c
 *
 */

/* ########## printf core implementation with some extensions ########## */
/*
 * How to use the extensions:
 * - put ' or " in the flags field to quote a string with this char and
 *   escape special characters (only available, if PRINT_QUOTES is defined)
 * - put \\ in the flags field to quote special characters and leading and
 *   trailing spaces (only available, if PRINT_QUOTES is defined)
 * - arrays (only available, if PRINT_ARRAYS is defined)
 *   - the array modifier [ comes after the maximal field width specifier
 *   - these modifiers expect an argument:
 *     - * -> number of elements
 *	 otherwise the array terminates at a -1 for ints and 0 for strings
 *     - (, ) -> array pre-/suf-fix; default ""
 *     - <, > -> element pre-/suf-fix; default ""
 *     - | -> element separator; default " "
 *   - these modifiers expect no argument:
 *     - : -> print '<number of elements>: ' before an array
 *     - , -> short for | ","
 *     - { -> short for ( "{" ) " }" < " " | ""
 *   - the pointer to the array is the last argument to the format
 *
 * NOTE: this file is meant to be included, not linked, 
 * so it can be used in the helper programs without much voodoo.
 */

/**************************************************************
 * (C) 2001 Oswald Buddenhagen <ossi@kde.org>
 * Partially stolen from OpenSSH's OpenBSD compat directory.
 * (C) Patrick Powell, Brandon Long, Thomas Roessler, 
 *     Michael Elkins, Ben Lindstrom
 **************************************************************/

#include <ctype.h>

/* format flags - Bits */
#define DP_F_MINUS	(1 << 0)
#define DP_F_PLUS	(1 << 1)
#define DP_F_SPACE	(1 << 2)
#define DP_F_NUM	(1 << 3)
#define DP_F_ZERO	(1 << 4)
#define DP_F_UPCASE	(1 << 5)
#define DP_F_UNSIGNED	(1 << 6)
#define DP_F_SQUOTE	(1 << 7)
#define DP_F_DQUOTE	(1 << 8)
#define DP_F_BACKSL	(1 << 9)
#define DP_F_ARRAY	(1 << 10)
#define DP_F_COLON	(1 << 11)

/* Conversion Flags */
#define DP_C_INT	0
#define DP_C_BYTE	1
#define DP_C_SHORT	2
#define DP_C_LONG	3
#define DP_C_STR	10

#ifndef USE_CONST
# define const
#endif

typedef void (*OutCh)(void *bp, char c);


static void
fmtint (OutCh dopr_outch, void *bp,
	long value, int base, int min, int max, int flags)
{
    unsigned long uvalue;
    char convert[20];
    const char *ctab;
    int signvalue = 0;
    int place = 0;
    int spadlen = 0;		/* amount to space pad */
    int zpadlen = 0;		/* amount to zero pad */

    if (max < 0)
	max = 0;

    uvalue = value;

    if (!(flags & DP_F_UNSIGNED)) {
	if (value < 0) {
	    signvalue = '-';
	    uvalue = -value;
	} else if (flags & DP_F_PLUS)	/* Do a sign (+/i) */
	    signvalue = '+';
	else if (flags & DP_F_SPACE)
	    signvalue = ' ';
    }

    ctab = (flags & DP_F_UPCASE) ? "0123456789ABCDEF" : "0123456789abcdef";
    do {
	convert[place++] = ctab[uvalue % (unsigned) base];
	uvalue = (uvalue / (unsigned) base);
    } while (uvalue);

    zpadlen = max - place;
    spadlen = min - (max > place ? max : place) - 
	      (signvalue ? 1 : 0) - ((flags & DP_F_NUM) ? 2 : 0);
    if (zpadlen < 0)
	zpadlen = 0;
    if (spadlen < 0)
	spadlen = 0;
    if (flags & DP_F_ZERO) {
	zpadlen = zpadlen > spadlen ? zpadlen : spadlen;
	spadlen = 0;
    }
    if (flags & DP_F_MINUS)
	spadlen = -spadlen;	/* Left Justifty */


    /* Spaces */
    while (spadlen > 0) {
	dopr_outch (bp, ' ');
	--spadlen;
    }

    /* Sign */
    if (signvalue)
	dopr_outch (bp, signvalue);

    /* Prefix */
    if (flags & DP_F_NUM) {
	dopr_outch (bp, '0');
	dopr_outch (bp, 'x');
    }

    /* Zeros */
    if (zpadlen > 0) {
	while (zpadlen > 0) {
	    dopr_outch (bp, '0');
	    --zpadlen;
	}
    }

    /* Digits */
    while (place > 0)
	dopr_outch (bp, convert[--place]);

    /* Left Justified spaces */
    while (spadlen < 0) {
	dopr_outch (bp, ' ');
	++spadlen;
    }
}

static void
fmtstr (OutCh dopr_outch, void *bp,
	const char *value, int flags, int min, int max)
{
    int padlen, strln, curcol;
#ifdef PRINT_QUOTES
    int lastcol;
#endif
    char ch;

    if (!value) {
#ifdef PRINT_QUOTES
	if (flags & (DP_F_SQUOTE | DP_F_DQUOTE)) {
	    flags &= ~(DP_F_SQUOTE | DP_F_DQUOTE);
	    value = "NULL";
	} else
#endif
	    value = "(null)";
    }

    for (strln = 0; (unsigned) strln < (unsigned) max && value[strln]; strln++);
    padlen = min - strln;
    if (padlen < 0)
	padlen = 0;
    if (flags & DP_F_MINUS)
	padlen = -padlen;	/* Left Justify */

    for (; padlen > 0; padlen--)
	dopr_outch (bp, ' ');
#ifdef PRINT_QUOTES
    if (flags & DP_F_SQUOTE)
	dopr_outch (bp, '\'');
    else if (flags & DP_F_DQUOTE)
	dopr_outch (bp, '"');
    else if (flags & DP_F_BACKSL)
	for (lastcol = strln; lastcol && value[lastcol - 1] == ' '; lastcol--);
#endif
    for (curcol = 0; curcol < strln; curcol++) {
	ch = value[curcol];
#ifdef PRINT_QUOTES
	if (flags & (DP_F_SQUOTE | DP_F_DQUOTE | DP_F_BACKSL)) {
	    switch (ch) {
	    case '\r': ch = 'r'; break;
	    case '\n': ch = 'n'; break;
	    case '\t': ch = 't'; break;
	    case '\a': ch = 'a'; break;
	    case '\b': ch = 'b'; break;
	    case '\v': ch = 'v'; break;
	    case '\f': ch = 'f'; break;
	    default:
		if (ch < 32 || 
		    ((unsigned char) ch >= 0x7f && (unsigned char) ch < 0xa0)) 
		{
		    dopr_outch (bp, '\\');
		    fmtint (dopr_outch, bp, (unsigned char)ch, 8, 3, 3, DP_F_ZERO);
		    continue;
		} else {
		    if ((ch == '\'' && (flags & DP_F_SQUOTE)) ||
			(ch == '"' && (flags & DP_F_DQUOTE)) ||
			(ch == ' ' && (flags & DP_F_BACKSL) && 
			 (!curcol || curcol >= lastcol)) ||
			ch == '\\')
			dopr_outch (bp, '\\');
		    dopr_outch (bp, ch);
		    continue;
		}
	    }
	    dopr_outch (bp, '\\');
	}
#endif
	dopr_outch (bp, ch);
    }
#ifdef PRINT_QUOTES
    if (flags & DP_F_SQUOTE)
	dopr_outch (bp, '\'');
    else if (flags & DP_F_DQUOTE)
	dopr_outch (bp, '"');
#endif
    for (; padlen < 0; padlen++)
	dopr_outch (bp, ' ');
}

static void
DoPr (OutCh dopr_outch, void *bp, const char *format, va_list args)
{
    const char *strvalue;
#ifdef PRINT_ARRAYS
    const char *arpr, *arsf, *arepr, *aresf, *aresp;
    void *arptr;
#endif
    unsigned long value;
    int radix, min, max, flags, cflags;
#ifdef PRINT_ARRAYS
    int arlen;
    unsigned aridx;
#endif
    char ch;
#define NCHR if (!(ch = *format++)) return

#if 0	/* gcc's flow analyzer is not the smartest ... */
    arpr = arsf = arepr = aresf = aresp = 0;
    arlen = 0;
#endif
    for (;;) {
	flags = cflags = min = 0;
	max = -1;   
	for (;;) {
	    NCHR;
	    if (ch == '%')
		break;
	    dopr_outch (bp, ch);
	}
	for (;;) {
	    NCHR;
	    switch (ch) {
	    case '#': flags |= DP_F_NUM; continue;
	    case '-': flags |= DP_F_MINUS; continue;
	    case '+': flags |= DP_F_PLUS; continue;
	    case ' ': flags |= DP_F_SPACE; continue;
	    case '0': flags |= DP_F_ZERO; continue;
#ifdef PRINT_QUOTES
	    case '"': flags |= DP_F_DQUOTE; continue;
	    case '\'': flags |= DP_F_SQUOTE; continue;
	    case '\\': flags |= DP_F_BACKSL; continue;
#endif
	    }
	    break;
	}
	for (;;) {
	    if (isdigit ((unsigned char) ch)) {
		min = 10 * min + (ch - '0');
		NCHR;
		continue;
	    } else if (ch == '*') {
		min = va_arg (args, int);
		NCHR;
	    }
	    break;
	}
	if (ch == '.') {
	    max = 0;
	    for (;;) {
		NCHR;
		if (isdigit ((unsigned char) ch)) {
		    max = 10 * max + (ch - '0');
		    continue;
		} else if (ch == '*') {
		    max = va_arg (args, int);
		    NCHR;
		}
		break;
	    }
	}
#ifdef PRINT_ARRAYS
	if (ch == '[') {
	    flags |= DP_F_ARRAY;
	    arlen = -1;
	    arpr = arsf = arepr = aresf = "", aresp = " ";
	    for (;;) {
		NCHR;
		switch (ch) {
		case ':': flags |= DP_F_COLON; continue;
		case '*': arlen = va_arg (args, int); continue;
		case '(': arpr = va_arg (args, char *); continue;
		case ')': arsf = va_arg (args, char *); continue;
		case '<': arepr = va_arg (args, char *); continue;
		case '>': aresf = va_arg (args, char *); continue;
		case '|': aresp = va_arg (args, char *); continue;
		case ',': aresp = ","; continue;
		case '{': arpr = "{"; arsf = " }"; arepr = " "; aresp = ""; continue;
		}
		break;
	    }
	}
#endif
	for (;;) {
	    switch (ch) {
	    case 'h':
		cflags = DP_C_SHORT;
		NCHR;
		if (ch == 'h') {
		    cflags = DP_C_BYTE;
		    NCHR;
		}
		continue;
	    case 'l':
		cflags = DP_C_LONG;
		NCHR;
		continue;
	    }
	    break;
	}
	switch (ch) {
	case '%':
	    dopr_outch (bp, ch);
	    break;
	case 'c':
	    dopr_outch (bp, va_arg (args, int));
	    break;
	case 's':
#ifdef PRINT_ARRAYS
	    cflags = DP_C_STR;
	    goto printit;
#else
	    strvalue = va_arg (args, char *);
	    fmtstr (dopr_outch, bp, strvalue, flags, min, max);
	    break;
#endif
	case 'u':
	    flags |= DP_F_UNSIGNED;
	case 'd':
	case 'i':
	    radix = 10;
	    goto printit;
	case 'X':
	    flags |= DP_F_UPCASE;
	case 'x':
	    flags |= DP_F_UNSIGNED;
	    radix = 16;
	  printit:
#ifdef PRINT_ARRAYS
	    if (flags & DP_F_ARRAY) {
		if (!(arptr = va_arg (args, void *)))
		    fmtstr (dopr_outch, bp, 
			    arpr ? "NULL" : "((null))", 0, 0, -1);
		else {
		    if (arlen == -1) {
			arlen = 0;
			switch (cflags) {
			case DP_C_STR: while (((char **)arptr)[arlen]) arlen++; break;
			case DP_C_BYTE: while (((unsigned char *)arptr)[arlen] != (unsigned char)-1) arlen++; break;
			case DP_C_SHORT: while (((unsigned short int *)arptr)[arlen] != (unsigned short int)-1) arlen++; break;
			case DP_C_LONG: while (((unsigned long int *)arptr)[arlen] != (unsigned long int)-1) arlen++; break;
			default: while (((unsigned int *)arptr)[arlen] != (unsigned int)-1) arlen++; break;
			}
		    }
		    if (flags & DP_F_COLON) {
			fmtint (dopr_outch, bp, (long)arlen, 10, 0, -1, DP_F_UNSIGNED);
			dopr_outch (bp, ':');
			dopr_outch (bp, ' ');
		    }
		    fmtstr (dopr_outch, bp, arpr, 0, 0, -1);
		    for (aridx = 0; aridx < (unsigned)arlen; aridx++) {
			if (aridx)
			    fmtstr (dopr_outch, bp, aresp, 0, 0, -1);
			fmtstr (dopr_outch, bp, arepr, 0, 0, -1);
			if (cflags == DP_C_STR) {
			    strvalue = ((char **)arptr)[aridx];
			    fmtstr (dopr_outch, bp, strvalue, flags, min, max);
			} else {
			    switch (cflags) {
			    case DP_C_BYTE: value = ((unsigned char *)arptr)[aridx]; break;
			    case DP_C_SHORT: value = ((unsigned short int *)arptr)[aridx]; break;
			    case DP_C_LONG: value = ((unsigned long int *)arptr)[aridx]; break;
			    default: value = ((unsigned int *)arptr)[aridx]; break;
			    }
			    fmtint (dopr_outch, bp, value, radix, min, max, flags);
			}
			fmtstr (dopr_outch, bp, aresf, 0, 0, -1);
		    }
		    fmtstr (dopr_outch, bp, arsf, 0, 0, -1);
		}
	    } else {
		if (cflags == DP_C_STR) {
		    strvalue = va_arg (args, char *);
		    fmtstr (dopr_outch, bp, strvalue, flags, min, max);
		} else {
#endif
		    switch (cflags) {
		    case DP_C_LONG: value = va_arg (args, unsigned long int); break;
		    default: value = va_arg (args, unsigned int); break;
		    }
		    fmtint (dopr_outch, bp, value, radix, min, max, flags);
#ifdef PRINT_ARRAYS
		}
	    }
#endif
	    break;
	case 'p':
	    value = (long) va_arg (args, void *);
	    fmtint (dopr_outch, bp, value, 16, sizeof (long) * 2 + 2,
		    max, flags | DP_F_UNSIGNED | DP_F_ZERO | DP_F_NUM);
	    break;
	}
    }
}

/* ########## end of printf core implementation ########## */


/*
 * Logging function for xdm and helper programs.
 */
#ifndef NO_LOGGER

#ifdef USE_SYSLOG
# include <syslog.h>
# ifdef LOG_NAME
#  define InitLog() openlog(LOG_NAME, LOG_PID, LOG_DAEMON)
# else
#  define InitLog() openlog(prog, LOG_PID, LOG_DAEMON)
# endif
static int lognums[] = { LOG_DEBUG, LOG_INFO, LOG_ERR, LOG_CRIT };
#else
# include <stdio.h>
# include <time.h>	/* XXX this will break on stone-age systems */
# ifndef Time_t
#  define Time_t time_t
# endif
# define InitLog() while(0)
static char *lognams[] = { "debug", "info", "error", "panic" };

static void
logTime (char *dbuf)
{
    Time_t tim;
    (void) time (&tim);
    strftime (dbuf, 20, "%b %e %H:%M:%S", localtime (&tim));
}
#endif

#ifdef LOG_LOCAL
# define STATIC static
#else
# define STATIC
#endif

STATIC void
LogOutOfMem (const char *fkt)
{
#ifdef USE_SYSLOG
    syslog (LOG_CRIT, "Out of memory in %s()", fkt);
#else
    char dbuf[20];
    logTime (dbuf);
    fprintf (stderr, "%s "
# ifdef LOG_NAME
	LOG_NAME "[%d]: Out of memory in %s()\n", dbuf, 
# else
	"%s[%d]: Out of memory in %s()\n", dbuf, prog, 
# endif
	(int)getpid(), fkt);
    fflush (stderr);
#endif
}

typedef struct {
    char *buf;
    int clen, blen, type;
    char lmbuf[100];
} OCLBuf;

static void
OutChLFlush (OCLBuf *oclbp)
{
    if (oclbp->clen) {
#ifdef USE_SYSLOG
	syslog (lognums[oclbp->type], "%.*s", oclbp->clen, oclbp->buf);
#else
	char dbuf[20];
	logTime (dbuf);
	fprintf (stderr, "%s "
# ifdef LOG_NAME
	    LOG_NAME  "[%d] %s: %.*s\n", dbuf, 
# else
	    "%s[%d] %s: %.*s\n", dbuf, prog, 
# endif
	    (int)getpid(), lognams[oclbp->type], oclbp->clen, oclbp->buf);
	fflush (stderr);
#endif
	oclbp->clen = 0;
    }
    if (oclbp->buf) {
	if (oclbp->buf != oclbp->lmbuf)
	    free (oclbp->buf);
	oclbp->buf = 0;
	oclbp->blen = 0;
    }
}

static void
OutChL (void *bp, char c)
{
    OCLBuf *oclbp = (OCLBuf *)bp;
    char *nbuf;
    int nlen;

    if (c == '\n')
	OutChLFlush (oclbp);
    else {
	if (oclbp->clen >= oclbp->blen) {
	    if (oclbp->buf == oclbp->lmbuf)
		OutChLFlush (oclbp);
	    nlen = oclbp->blen * 3 / 2 + 100;
	    nbuf = realloc (oclbp->buf, nlen);
	    if (nbuf) {
		oclbp->buf = nbuf;
		oclbp->blen = nlen;
	    } else {
		LogOutOfMem ("Logger");
		OutChLFlush (oclbp);
		oclbp->buf = oclbp->lmbuf;
		oclbp->blen = sizeof(oclbp->lmbuf);
	    }
	}
	oclbp->buf[oclbp->clen++] = c;
    }
}

static void
Logger (int type, const char *fmt, va_list args)
{
    static OCLBuf oclb;

    if (oclb.type != type) {
	OutChLFlush (&oclb);
	oclb.type = type;
    }
    DoPr(OutChL, &oclb, fmt, args);
}

#ifdef LOG_DEBUG_MASK
STATIC int debugLevel;

STATIC void
Debug (const char *fmt, ...)
{
    va_list args;

    if (debugLevel & LOG_DEBUG_MASK)
    {
	va_start(args, fmt);
	Logger (DM_DEBUG, fmt, args);
	va_end(args);
    }
}
#endif

#ifndef LOG_NO_INFO
STATIC void 
LogInfo(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    Logger (DM_INFO, fmt, args);
    va_end(args);
}
#endif

#ifndef LOG_NO_ERROR
STATIC void
LogError (const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    Logger (DM_ERR, fmt, args);
    va_end(args);
}
#endif

#ifdef LOG_PANIC_EXIT
STATIC void
LogPanic (const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    Logger (DM_PANIC, fmt, args);
    va_end(args);
    exit (LOG_PANIC_EXIT);
}
#endif

#endif /* NO_LOGGER */
