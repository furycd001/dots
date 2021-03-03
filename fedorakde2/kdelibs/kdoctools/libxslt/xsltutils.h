/*
 * xsltutils.h: interfaces for the utilities module of the XSLT engine
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_XSLTUTILS_H__
#define __XML_XSLTUTILS_H__

#include <libxml/xpath.h>
#include <libxml/xmlerror.h>
#include "xsltInternals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 */
#define TODO 								\
    xsltGenericError(xsltGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

/**
 * STRANGE:
 *
 * macro to flag that a problem was detected internally
 */
#define STRANGE 							\
    xsltGenericError(xsltGenericErrorContext,				\
	    "Internal error at %s:%d\n",				\
            __FILE__, __LINE__);

/**
 * IS_XSLT_ELEM:
 *
 * Checks that the element pertains to XSLt namespace
 */
#define IS_XSLT_ELEM(n)							\
    (((n) != NULL) && ((n)->ns != NULL) &&				\
     (xmlStrEqual((n)->ns->href, XSLT_NAMESPACE)))

/**
 * IS_XSLT_NAME:
 *
 * Checks the value of an element in XSLT namespace
 */
#define IS_XSLT_NAME(n, val)						\
    (xmlStrEqual((n)->name, (const xmlChar *) (val)))

/*
 * Our own version of namespaced atributes lookup
 */
xmlChar *	 xsltGetNsProp			(xmlNodePtr node,
						 const xmlChar *name,
						 const xmlChar *nameSpace);

/*
 * XSLT specific error and debug reporting functions
 */
extern xmlGenericErrorFunc xsltGenericError;
extern void *xsltGenericErrorContext;
extern xmlGenericErrorFunc xsltGenericDebug;
extern void *xsltGenericDebugContext;

void		xsltMessage			(xsltTransformContextPtr ctxt,
						 xmlNodePtr node,
						 xmlNodePtr inst);
void		xsltSetGenericErrorFunc		(void *ctx,
						 xmlGenericErrorFunc handler);
void		xsltSetGenericDebugFunc		(void *ctx,
						 xmlGenericErrorFunc handler);

/*
 * Sorting
 */

void		xsltDocumentSortFunction	(xmlNodeSetPtr list);
void		xsltDoSortFunction		(xsltTransformContextPtr ctxt,
						 xmlNodePtr *sorts,
						 int nbsorts);

/*
 * QNames handling
 */

const xmlChar * xsltGetQNameURI			(xmlNodePtr node,
						 xmlChar **name);

/*
 * Output, reuse libxml I/O buffers
 */
int		xsltSaveResultTo		(xmlOutputBufferPtr buf,
						 xmlDocPtr result,
						 xsltStylesheetPtr style);
int		xsltSaveResultToFilename	(const char *URI,
						 xmlDocPtr result,
						 xsltStylesheetPtr style,
						 int compression);
int		xsltSaveResultToFile		(FILE *file,
						 xmlDocPtr result,
						 xsltStylesheetPtr style);
int		xsltSaveResultToFd		(int fd,
						 xmlDocPtr result,
						 xsltStylesheetPtr style);

/*
 * profiling
 */
void		xsltSaveProfiling		(xsltTransformContextPtr ctxt,
						 FILE *output);

long		xsltTimestamp			(void);

#ifdef __cplusplus
}
#endif

#endif /* __XML_XSLTUTILS_H__ */

