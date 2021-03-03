/*
 * xsltproc.c: user program for the XSL Transformation 1.0 engine
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#include "libxslt.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#ifdef LIBXML_DOCB_ENABLED
#include <libxml/DOCBparser.h>
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
#include <libxml/xinclude.h>
#endif
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>

#ifdef WIN32
#ifdef _MSC_VER
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define gettimeofday(p1,p2)
#endif /* _MS_VER */
#else /* WIN32 */
#include <sys/time.h>
#endif /* WIN32 */

extern int xmlLoadExtDtdDefaultValue;

static int debug = 0;
static int repeat = 0;
static int timing = 0;
static int novalid = 0;
static int noout = 0;
#ifdef LIBXML_DOCB_ENABLED
static int docbook = 0;
#endif
#ifdef LIBXML_HTML_ENABLED
static int html = 0;
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
static int xinclude = 0;
#endif
static int profile = 0;
static int nonet;
static xmlExternalEntityLoader defaultLoader = NULL;

static xmlParserInputPtr
xsltNoNetExternalEntityLoader(const char *URL, const char *ID,
                               xmlParserCtxtPtr ctxt) {
    if (URL != NULL) {
        if ((!xmlStrncasecmp((const xmlChar *) URL,
		            (const xmlChar *) "ftp://", 6)) ||
            (!xmlStrncasecmp((const xmlChar *) URL,
		            (const xmlChar *) "http://", 7))) {
	    fprintf(stderr, "Attempt to load network entity %s \n", URL);
	    if (nonet)
		return(NULL);
	}
    }
    if (defaultLoader != NULL) {
	return(defaultLoader(URL, ID, ctxt));
    }
    return(NULL);
}

static void usage(const char *name) {
    printf("Usage: %s [options] stylesheet file [file ...]\n", name);
    printf("   Options:\n");
    printf("      --version or -V: show the version of libxml and libxslt used\n");
    printf("      --verbose or -v: show logs of what's happening\n");
    printf("      --output file or -o file: save to a given file\n");
    printf("      --timing: display the time used\n");
    printf("      --repeat: run the transformation 20 times\n");
    printf("      --debug: dump the tree of the result instead\n");
    printf("      --novalid: skip the Dtd loading phase\n");
    printf("      --noout: do not dump the result\n");
    printf("      --maxdepth val : increase the maximum depth\n");
#ifdef LIBXML_HTML_ENABLED
    printf("      --html: the input document is(are) an HTML file(s)\n");
#endif
#ifdef LIBXML_DOCB_ENABLED
    printf("      --docbook: the input document is SGML docbook\n");
#endif
    printf("      --param name value : pass a (parameter,value) pair\n");
    printf("      --nonet refuse to fetch DTDs or entities over network\n");
    printf("      --warnnet warn against fetching over the network\n");
#ifdef LIBXML_CATALOG_ENABLED
    printf("      --catalogs : use the catalogs from $SGML_CATALOG_FILES\n");
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
    printf("      --xinclude : do XInclude processing on document intput\n");
#endif
    printf("      --profile or --norman : dump profiling informations \n");
}

int
main(int argc, char **argv)
{
    int i;
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc, res;
    struct timeval begin, end;
    const char *params[16 + 1];
    int nbparams = 0;
    const char *output = NULL;

    if (argc <= 1) {
        usage(argv[0]);
        return (1);
    }
    xmlInitMemory();
    LIBXML_TEST_VERSION defaultLoader = xmlGetExternalEntityLoader();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-"))
            break;

        if (argv[i][0] != '-')
            continue;
#ifdef LIBXML_DEBUG_ENABLED
        if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug"))) {
            debug++;
        } else
#endif
        if ((!strcmp(argv[i], "-v")) ||
                (!strcmp(argv[i], "-verbose")) ||
                (!strcmp(argv[i], "--verbose"))) {
            xsltSetGenericDebugFunc(stderr, NULL);
        } else if ((!strcmp(argv[i], "-o")) ||
                   (!strcmp(argv[i], "-output")) ||
                   (!strcmp(argv[i], "--output"))) {
            i++;
            output = argv[i++];
        } else if ((!strcmp(argv[i], "-V")) ||
                   (!strcmp(argv[i], "-version")) ||
                   (!strcmp(argv[i], "--version"))) {
            printf("Using libxml %s and libxslt %s\n",
                   xmlParserVersion, xsltEngineVersion);
            printf
                ("xsltproc was compiled against libxml %d and libxslt %d\n",
                 LIBXML_VERSION, LIBXSLT_VERSION);
            printf("libxslt %d was compiled against libxml %d\n",
                   xsltLibxsltVersion, xsltLibxmlVersion);
        } else if ((!strcmp(argv[i], "-repeat"))
                   || (!strcmp(argv[i], "--repeat"))) {
            if (repeat == 0)
                repeat = 20;
            else
                repeat = 100;
        } else if ((!strcmp(argv[i], "-novalid")) ||
                   (!strcmp(argv[i], "--novalid"))) {
            novalid++;
        } else if ((!strcmp(argv[i], "-noout")) ||
                   (!strcmp(argv[i], "--noout"))) {
            noout++;
#ifdef LIBXML_DOCB_ENABLED
        } else if ((!strcmp(argv[i], "-docbook")) ||
                   (!strcmp(argv[i], "--docbook"))) {
            docbook++;
#endif
#ifdef LIBXML_HTML_ENABLED
        } else if ((!strcmp(argv[i], "-html")) ||
                   (!strcmp(argv[i], "--html"))) {
            html++;
#endif
        } else if ((!strcmp(argv[i], "-timing")) ||
                   (!strcmp(argv[i], "--timing"))) {
            timing++;
        } else if ((!strcmp(argv[i], "-profile")) ||
                   (!strcmp(argv[i], "--profile"))) {
            profile++;
        } else if ((!strcmp(argv[i], "-norman")) ||
                   (!strcmp(argv[i], "--norman"))) {
            profile++;
        } else if ((!strcmp(argv[i], "-warnnet")) ||
                   (!strcmp(argv[i], "--warnnet"))) {
            xmlSetExternalEntityLoader(xsltNoNetExternalEntityLoader);
        } else if ((!strcmp(argv[i], "-nonet")) ||
                   (!strcmp(argv[i], "--nonet"))) {
            xmlSetExternalEntityLoader(xsltNoNetExternalEntityLoader);
            nonet = 1;
#ifdef LIBXML_CATALOG_ENABLED
        } else if ((!strcmp(argv[i], "-catalogs")) ||
                   (!strcmp(argv[i], "--catalogs"))) {
            const char *catalogs;

            catalogs = getenv("SGML_CATALOG_FILES");
            if (catalogs == NULL) {
                fprintf(stderr, "Variable $SGML_CATALOG_FILES not set\n");
            } else {
                xmlLoadCatalogs(catalogs);
            }
#endif
#ifdef LIBXML_XINCLUDE_ENABLED
        } else if ((!strcmp(argv[i], "-xinclude")) ||
                   (!strcmp(argv[i], "--xinclude"))) {
            xinclude++;
            xsltSetXIncludeDefault(1);
#endif
        } else if ((!strcmp(argv[i], "-param")) ||
                   (!strcmp(argv[i], "--param"))) {
            i++;
            params[nbparams++] = argv[i++];
            params[nbparams++] = argv[i];
            if (nbparams >= 16) {
                fprintf(stderr, "too many params\n");
                return (1);
            }
        } else if ((!strcmp(argv[i], "-maxdepth")) ||
                   (!strcmp(argv[i], "--maxdepth"))) {
            int value;

            i++;
            if (sscanf(argv[i], "%d", &value) == 1) {
                if (value > 0)
                    xsltMaxDepth = value;
            }
        } else {
            fprintf(stderr, "Unknown option %s\n", argv[i]);
            usage(argv[0]);
            return (1);
        }
    }
    params[nbparams] = NULL;
    xmlSubstituteEntitiesDefault(1);
    if (novalid == 0)           /* TODO XML_DETECT_IDS | XML_COMPLETE_ATTRS */
        xmlLoadExtDtdDefaultValue = 6;
    else
        xmlLoadExtDtdDefaultValue = 0;
    for (i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "-maxdepth")) ||
            (!strcmp(argv[i], "--maxdepth"))) {
            i++;
            continue;
        } else if ((!strcmp(argv[i], "-o")) ||
                   (!strcmp(argv[i], "-output")) ||
                   (!strcmp(argv[i], "--output"))) {
            i++;
	    continue;
	}
        if ((!strcmp(argv[i], "-param")) || (!strcmp(argv[i], "--param"))) {
            i += 2;
            continue;
        }
        if ((argv[i][0] != '-') || (strcmp(argv[i], "-") == 0)) {
            if (timing)
                gettimeofday(&begin, NULL);
            cur = xsltParseStylesheetFile((const xmlChar *) argv[i]);
            if (timing) {
                long msec;

                gettimeofday(&end, NULL);
                msec = end.tv_sec - begin.tv_sec;
                msec *= 1000;
                msec += (end.tv_usec - begin.tv_usec) / 1000;
                fprintf(stderr, "Parsing stylesheet %s took %ld ms\n",
                        argv[i], msec);
            }
            if (cur != NULL) {
                if (cur->indent == 1)
                    xmlIndentTreeOutput = 1;
                else
                    xmlIndentTreeOutput = 0;
                i++;
            }
            break;

        }
    }
    /*
     * disable CDATA from being built in the document tree
     */
    xmlDefaultSAXHandlerInit();
    xmlDefaultSAXHandler.cdataBlock = NULL;

    if ((cur != NULL) && (cur->errors == 0)) {
        for (; i < argc; i++) {
            if (timing)
                gettimeofday(&begin, NULL);
#ifdef LIBXML_HTML_ENABLED
            if (html)
                doc = htmlParseFile(argv[i], NULL);
            else
#endif
#ifdef LIBXML_DOCB_ENABLED
            if (docbook)
                doc = docbParseFile(argv[i], NULL);
            else
#endif
                doc = xmlParseFile(argv[i]);
            if (doc == NULL) {
                fprintf(stderr, "unable to parse %s\n", argv[i]);
                continue;
            }
            if (timing) {
                long msec;

                gettimeofday(&end, NULL);
                msec = end.tv_sec - begin.tv_sec;
                msec *= 1000;
                msec += (end.tv_usec - begin.tv_usec) / 1000;
                fprintf(stderr, "Parsing document %s took %ld ms\n",
                        argv[i], msec);
            }
#ifdef LIBXML_XINCLUDE_ENABLED
            if (xinclude) {
                if (timing)
                    gettimeofday(&begin, NULL);
                xmlXIncludeProcess(doc);
                if (timing) {
                    long msec;

                    gettimeofday(&end, NULL);
                    msec = end.tv_sec - begin.tv_sec;
                    msec *= 1000;
                    msec += (end.tv_usec - begin.tv_usec) / 1000;
                    fprintf(stderr, "XInclude processing %s took %ld ms\n",
                            argv[i], msec);
                }
            }
#endif
            if (timing)
                gettimeofday(&begin, NULL);
            if (output == NULL) {
                if (repeat) {
                    int j;

                    for (j = 1; j < repeat; j++) {
                        res = xsltApplyStylesheet(cur, doc, params);
                        xmlFreeDoc(res);
                        xmlFreeDoc(doc);
#ifdef LIBXML_HTML_ENABLED
                        if (html)
                            doc = htmlParseFile(argv[i], NULL);
                        else
#endif
#ifdef LIBXML_DOCB_ENABLED
                        if (docbook)
                            doc = docbParseFile(argv[i], NULL);
                        else
#endif
                            doc = xmlParseFile(argv[i]);
                    }
                }
		if (profile) {
		    res = xsltProfileStylesheet(cur, doc, params, stderr);
		} else {
		    res = xsltApplyStylesheet(cur, doc, params);
		}
                if (timing) {
                    long msec;

                    gettimeofday(&end, NULL);
                    msec = end.tv_sec - begin.tv_sec;
                    msec *= 1000;
                    msec += (end.tv_usec - begin.tv_usec) / 1000;
                    if (repeat)
                        fprintf(stderr,
                                "Applying stylesheet %d times took %ld ms\n",
                                repeat, msec);
                    else
                        fprintf(stderr,
                                "Applying stylesheet took %ld ms\n", msec);
                }
                xmlFreeDoc(doc);
                if (res == NULL) {
                    fprintf(stderr, "no result for %s\n", argv[i]);
                    continue;
                }
                if (noout) {
                    xmlFreeDoc(res);
                    continue;
                }
#ifdef LIBXML_DEBUG_ENABLED
                if (debug)
                    xmlDebugDumpDocument(stdout, res);
                else {
#endif
                    if (cur->methodURI == NULL) {
                        if (timing)
                            gettimeofday(&begin, NULL);
                        xsltSaveResultToFile(stdout, res, cur);
                        if (timing) {
                            long msec;

                            gettimeofday(&end, NULL);
                            msec = end.tv_sec - begin.tv_sec;
                            msec *= 1000;
                            msec += (end.tv_usec - begin.tv_usec) / 1000;
                            fprintf(stderr, "Saving result took %ld ms\n",
                                    msec);
                        }
                    } else {
                        if (xmlStrEqual
                            (cur->method, (const xmlChar *) "xhtml")) {
                            fprintf(stderr, "non standard output xhtml\n");
                            if (timing)
                                gettimeofday(&begin, NULL);
                            xsltSaveResultToFile(stdout, res, cur);
                            if (timing) {
                                long msec;

                                gettimeofday(&end, NULL);
                                msec = end.tv_sec - begin.tv_sec;
                                msec *= 1000;
                                msec +=
                                    (end.tv_usec - begin.tv_usec) / 1000;
                                fprintf(stderr,
                                        "Saving result took %ld ms\n",
                                        msec);
                            }
                        } else {
                            fprintf(stderr,
                                    "Unsupported non standard output %s\n",
                                    cur->method);
                        }
                    }
#ifdef LIBXML_DEBUG_ENABLED
                }
#endif

                xmlFreeDoc(res);
            } else {
                xsltRunStylesheet(cur, doc, params, output, NULL, NULL);
                if (timing) {
                    long msec;

                    gettimeofday(&end, NULL);
                    msec = end.tv_sec - begin.tv_sec;
                    msec *= 1000;
                    msec += (end.tv_usec - begin.tv_usec) / 1000;
                    fprintf(stderr,
			"Running stylesheet and saving result took %ld ms\n",
                            msec);
                }
                xmlFreeDoc(doc);
            }
        }
        xsltFreeStylesheet(cur);
    }
    xsltUnregisterAllExtModules();
    xmlCleanupParser();
    xmlMemoryDump();
    return (0);
}

