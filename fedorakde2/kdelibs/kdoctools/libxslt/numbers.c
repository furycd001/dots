/*
 * numbers.c: Implementation of the XSLT number functions
 *
 * Reference:
 *   http://www.w3.org/TR/1999/REC-xslt-19991116
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Bjorn Reese <breese@users.sourceforge.net>
 */

#include "libxslt.h"

#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#ifdef HAVE_NAN_H
#include <nan.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "xsltutils.h"
#include "pattern.h"
#include "numbersInternals.h"

#ifndef FALSE
# define FALSE (0 == 1)
# define TRUE (1 == 1)
#endif

#define SYMBOL_QUOTE             ((xmlChar)'\'')

typedef struct _xsltNumberFormatToken {
    xmlChar token;
    int width;
    xmlChar *filling;
} xsltNumberFormatToken, *xsltNumberFormatTokenPtr;

static char alpha_upper_list[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char alpha_lower_list[] = "abcdefghijklmnopqrstuvwxyz";
static xsltNumberFormatToken default_token;


/************************************************************************
 *									*
 *			Utility functions				*
 *									*
 ************************************************************************/

#define IS_SPECIAL(self,letter)			\
    (((letter) == (self)->zeroDigit[0])	    ||	\
     ((letter) == (self)->digit[0])	    ||	\
     ((letter) == (self)->decimalPoint[0])  ||	\
     ((letter) == (self)->grouping[0])	    ||	\
     ((letter) == (self)->patternSeparator[0]))

#define IS_DIGIT_ZERO(x) xsltIsDigitZero(x)
#define IS_DIGIT_ONE(x) xsltIsDigitZero((x)-1)

static int
xsltIsDigitZero(xmlChar ch)
{
    /*
     * Reference: ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
     */
    switch (ch) {
    case 0x0030: case 0x0660: case 0x06F0: case 0x0966:
    case 0x09E6: case 0x0A66: case 0x0AE6: case 0x0B66:
    case 0x0C66: case 0x0CE6: case 0x0D66: case 0x0E50:
    case 0x0E60: case 0x0F20: case 0x1040: case 0x17E0:
    case 0x1810: case 0xFF10:
	return TRUE;
    default:
	return FALSE;
    }
}

#ifndef isnan
#ifndef HAVE_ISNAN

/*
 * NaN (Not-A-Number)
 *
 * In C99 isnan() is defined as a macro, so its existence can be determined
 * with the preprocessor.
 *
 * ANSI/IEEE 754-1986 states that "Every NaN shall compare unordered
 * with everything, including itself." This means that "number == number"
 * will return true for all numbers, except NaN. Unfortunately, this
 * simple test does not work on all platforms supporting NaNs. Instead
 * we use an almost equivalent comparison.
 */
static int
isnan(volatile double number)
{
    return (!(number < 0.0 || number > 0.0) && (number != 0.0));
}
#endif /* !HAVE_ISNAN */
#endif /* !isnan */

#ifndef isinf
#ifndef HAVE_ISINF
/*
 * Infinity (positive and negative)
 *
 * C99 defines isinf() as a macro. See comment for isnan().
 */
static int
isinf(double number)
{
# ifdef HUGE_VAL
    return ((number == HUGE_VAL) ? 1 : ((number == -HUGE_VAL) ? -1 : 0));
# else
    return FALSE;
# endif
}
#endif /* !HAVE_ISINF */
#endif /* !isinf */

static void
xsltNumberFormatDecimal(xmlBufferPtr buffer,
			double number,
			xmlChar digit_zero,
			int width,
			int digitsPerGroup,
			xmlChar groupingCharacter)
{
    xmlChar temp_string[sizeof(double) * CHAR_BIT * sizeof(xmlChar) + 1];
    xmlChar *pointer;
    int i;

    /* Build buffer from back */
    pointer = &temp_string[sizeof(temp_string)];
    *(--pointer) = 0;
    for (i = 1; i < (int)sizeof(temp_string); i++) {
	*(--pointer) = digit_zero + (int)fmod(number, 10.0);
	number /= 10.0;
	if ((i >= width) && (fabs(number) < 1.0))
	    break; /* for */
	if ((groupingCharacter != 0) &&
	    (digitsPerGroup > 0) &&
	    ((i % digitsPerGroup) == 0)) {
	    *(--pointer) = groupingCharacter;
	}
    }
    xmlBufferCat(buffer, pointer);
}

static void
xsltNumberFormatAlpha(xmlBufferPtr buffer,
		      double number,
		      int is_upper)
{
    char temp_string[sizeof(double) * CHAR_BIT * sizeof(xmlChar) + 1];
    char *pointer;
    int i;
    char *alpha_list;
    double alpha_size = (double)(sizeof(alpha_upper_list) - 1);

    /* Build buffer from back */
    pointer = &temp_string[sizeof(temp_string)];
    *(--pointer) = 0;
    alpha_list = (is_upper) ? alpha_upper_list : alpha_lower_list;
    
    for (i = 1; i < (int)sizeof(temp_string); i++) {
	number--;
	*(--pointer) = alpha_list[((int)fmod(number, alpha_size))];
	number /= alpha_size;
	if (fabs(number) < 1.0)
	    break; /* for */
    }
    xmlBufferCCat(buffer, pointer);
}

static void
xsltNumberFormatRoman(xmlBufferPtr buffer,
		      double number,
		      int is_upper)
{
    /*
     * Based on an example by Jim Walsh
     */
    while (number >= 1000.0) {
	xmlBufferCCat(buffer, (is_upper) ? "M" : "m");
	number -= 1000.0;
    }
    if (number >= 900.0) {
	xmlBufferCCat(buffer, (is_upper) ? "CM" : "cm");
	number -= 900.0;
    }
    while (number >= 500.0) {
	xmlBufferCCat(buffer, (is_upper) ? "D" : "d");
	number -= 500.0;
    }
    if (number >= 400.0) {
	xmlBufferCCat(buffer, (is_upper) ? "CD" : "cd");
	number -= 400.0;
    }
    while (number >= 100.0) {
	xmlBufferCCat(buffer, (is_upper) ? "C" : "c");
	number -= 100.0;
    }
    if (number >= 90.0) {
	xmlBufferCCat(buffer, (is_upper) ? "XC" : "xc");
	number -= 90.0;
    }
    while (number >= 50.0) {
	xmlBufferCCat(buffer, (is_upper) ? "L" : "l");
	number -= 50.0;
    }
    if (number >= 40.0) {
	xmlBufferCCat(buffer, (is_upper) ? "XL" : "xl");
	number -= 40.0;
    }
    while (number >= 10.0) {
	xmlBufferCCat(buffer, (is_upper) ? "X" : "x");
	number -= 10.0;
    }
    if (number >= 9.0) {
	xmlBufferCCat(buffer, (is_upper) ? "IX" : "ix");
	number -= 9.0;
    }
    while (number >= 5.0) {
	xmlBufferCCat(buffer, (is_upper) ? "V" : "v");
	number -= 5.0;
    }
    if (number >= 4.0) {
	xmlBufferCCat(buffer, (is_upper) ? "IV" : "iv");
	number -= 4.0;
    }
    while (number >= 1.0) {
	xmlBufferCCat(buffer, (is_upper) ? "I" : "i");
	number--;
    }
}

static int
xsltNumberFormatTokenize(xmlChar *format,
			 xsltNumberFormatTokenPtr array,
			 int array_max)
{
    int index = 0;
    int cnt;
    int j;

    default_token.token = (xmlChar)'0';
    default_token.width = 1;
    default_token.filling = BAD_CAST(".");
    
    for (cnt = 0; cnt < array_max; cnt++) {
	if (format[index] == 0) {
	    break; /* for */
	} else if (IS_DIGIT_ONE(format[index]) ||
		 IS_DIGIT_ZERO(format[index])) {
	    array[cnt].width = 1;
	    while (IS_DIGIT_ZERO(format[index])) {
		array[cnt].width++;
		index++;
	    }
	    if (IS_DIGIT_ONE(format[index])) {
		array[cnt].token = format[index] - 1;
		index++;
	    }
	} else if (format[index] == (xmlChar)'A') {
	    array[cnt].token = format[index];
	    index++;
	} else if (format[index] == (xmlChar)'a') {
	    array[cnt].token = format[index];
	    index++;
	} else if (format[index] == (xmlChar)'I') {
	    array[cnt].token = format[index];
	    index++;
	} else if (format[index] == (xmlChar)'i') {
	    array[cnt].token = format[index];
	    index++;
	} else {
	    /* XSLT section 7.7
	     * "Any other format token indicates a numbering sequence
	     *  that starts with that token. If an implementation does
	     *  not support a numbering sequence that starts with that
	     *  token, it must use a format token of 1."
	     */
	    array[cnt].token = (xmlChar)'0';
	    array[cnt].width = 1;
	    index++;
	}
	/*
	 * Skip over remaining alphanumeric characters from the Nd
	 * (Number, decimal digit), Nl (Number, letter), No (Number,
	 * other), Lu (Letter, uppercase), Ll (Letter, lowercase), Lt
	 * (Letters, titlecase), Lm (Letters, modifiers), and Lo
	 * (Letters, other (uncased)) Unicode categories. This happens
	 * to correspond to the Letter and Digit classes from XML (and
	 * one wonders why XSLT doesn't refer to these instead).
	 */
	while (IS_LETTER(format[index]) || IS_DIGIT(format[index]))
	    index++;
	/*
	 * Insert non-alphanumeric separator.
	 */
	j = index;
	while (! (IS_LETTER(format[index]) || IS_DIGIT(format[index]))) {
	    if (format[index] == 0)
		break; /* while */
	    index++;
	}
	if (index > j)
	    array[cnt].filling = xmlStrndup(&format[j], index - j);
	else
	    array[cnt].filling = NULL;
    }
    return cnt;
}


static void
xsltNumberFormatInsertNumbers(xsltNumberDataPtr data,
			      double *numbers,
			      int numbers_max,
			      xsltNumberFormatToken (*array)[],
			      int array_max,
			      xmlBufferPtr buffer)
{
    int i = 0;
    int minmax;
    double number;
    xsltNumberFormatTokenPtr token;
    int is_last_default_token = 0;

    minmax = (array_max >= numbers_max) ? numbers_max : array_max;
    for (i = 0; i < numbers_max; i++) {
	/* Insert number */
	number = numbers[(numbers_max - 1) - i];
	if (i < array_max) {
	  token = &(*array)[i];
	} else if (array_max > 0) {
	  token = &(*array)[array_max - 1];
	} else {
	  token = &default_token;
	  is_last_default_token = (i >= numbers_max - 1);
	}
	
	switch (isinf(number)) {
	case -1:
	    xmlBufferCCat(buffer, "-Infinity");
	    break;
	case 1:
	    xmlBufferCCat(buffer, "Infinity");
	    break;
	default:
	    if (isnan(number)) {
		xmlBufferCCat(buffer, "NaN");
	    } else {

		switch (token->token) {
		case 'A':
		    xsltNumberFormatAlpha(buffer,
					  number,
					  TRUE);

		    break;
		case 'a':
		    xsltNumberFormatAlpha(buffer,
					  number,
					  FALSE);

		    break;
		case 'I':
		    xsltNumberFormatRoman(buffer,
					  number,
					  TRUE);

		    break;
		case 'i':
		    xsltNumberFormatRoman(buffer,
					  number,
					  FALSE);

		    break;
		default:
		    if (IS_DIGIT_ZERO(token->token)) {
			xsltNumberFormatDecimal(buffer,
						number,
						token->token,
						token->width,
						data->digitsPerGroup,
						data->groupingCharacter);
		    }
		    break;
		}
	    }

	}

	if ((token->filling != NULL) && !is_last_default_token)
	    xmlBufferCat(buffer, token->filling);
    }
}

static int
xsltNumberFormatGetAnyLevel(xsltTransformContextPtr context,
			    xmlNodePtr node,
			    xmlChar *count,
			    xmlChar *from,
			    double *array,
			    int max ATTRIBUTE_UNUSED,
			    xmlDocPtr doc,
			    xmlNodePtr elem)
{
    int amount = 0;
    int cnt = 0;
    int keep_going = TRUE;
    xmlNodePtr ancestor;
    xmlNodePtr preceding;
    xmlXPathParserContextPtr parser;
    xsltCompMatchPtr countPat;
    xsltCompMatchPtr fromPat;

    if (count != NULL)
	countPat = xsltCompilePattern(count, doc, elem);
    else
	countPat = NULL;
    if (from != NULL)
	fromPat = xsltCompilePattern(from, doc, elem);
    else
	fromPat = NULL;
    context->xpathCtxt->node = node;
    parser = xmlXPathNewParserContext(NULL, context->xpathCtxt);
    if (parser) {
	/* preceding */
	for (preceding = xmlXPathNextPreceding(parser, node);
	     preceding != NULL;
	     preceding = xmlXPathNextPreceding(parser, preceding)) {
	    if ((from != NULL) &&
		xsltTestCompMatchList(context, preceding, fromPat)) {
		keep_going = FALSE;
		break; /* for */
	    }
	    if (count == NULL) {
		if ((node->type == preceding->type) &&
		    /* FIXME: must use expanded-name instead of local name */
		    xmlStrEqual(node->name, preceding->name))
		    cnt++;
	    } else {
		if (xsltTestCompMatchList(context, preceding, countPat))
		    cnt++;
	    }
	}

	if (keep_going) {
	    /* ancestor-or-self */
	    for (ancestor = node;
		 ancestor != NULL;
		 ancestor = xmlXPathNextAncestor(parser, ancestor)) {
		if ((from != NULL) &&
		    xsltTestCompMatchList(context, ancestor, fromPat)) {
		    break; /* for */
		}
		if (count == NULL) {
		    if ((node->type == ancestor->type) &&
			/* FIXME */
			xmlStrEqual(node->name, ancestor->name))
			cnt++;
		} else {
		    if (xsltTestCompMatchList(context, ancestor, countPat))
			cnt++;
		}
	    }
	}
	array[amount++] = (double)cnt;
	xmlXPathFreeParserContext(parser);
    }
    xsltFreeCompMatchList(countPat);
    xsltFreeCompMatchList(fromPat);
    return(amount);
}

static int
xsltNumberFormatGetMultipleLevel(xsltTransformContextPtr context,
				 xmlNodePtr node,
				 xmlChar *count,
				 xmlChar *from,
				 double *array,
				 int max,
				 xmlDocPtr doc,
				 xmlNodePtr elem)
{
    int amount = 0;
    int cnt;
    xmlNodePtr ancestor;
    xmlNodePtr preceding;
    xmlXPathParserContextPtr parser;
    xsltCompMatchPtr countPat;
    xsltCompMatchPtr fromPat;

    if (count != NULL)
	countPat = xsltCompilePattern(count, doc, elem);
    else
	countPat = NULL;
    if (from != NULL)
	fromPat = xsltCompilePattern(from, doc, elem);
    else
	fromPat = NULL;
    context->xpathCtxt->node = node;
    parser = xmlXPathNewParserContext(NULL, context->xpathCtxt);
    if (parser) {
	/* ancestor-or-self::*[count] */
	for (ancestor = node;
	     (ancestor != NULL) && (ancestor->type != XML_DOCUMENT_NODE);
	     ancestor = xmlXPathNextAncestor(parser, ancestor)) {
	    
	    if ((from != NULL) &&
		xsltTestCompMatchList(context, ancestor, fromPat))
		break; /* for */
	    
	    if ((count == NULL) ||
		xsltTestCompMatchList(context, ancestor, countPat)) {
		/* count(preceding-sibling::*) */
		cnt = 0;
		for (preceding = ancestor;
		     preceding != NULL;
		     preceding = 
		        xmlXPathNextPrecedingSibling(parser, preceding)) {
		    if (count == NULL) {
			if ((preceding->type == ancestor->type) &&
			    /* FIXME */
			    xmlStrEqual(preceding->name, ancestor->name))
			    cnt++;
		    } else {
			if (xsltTestCompMatchList(context, preceding,
				                  countPat))
			    cnt++;
		    }
		}
		array[amount++] = (double)cnt;
		if (amount >= max)
		    break; /* for */
	    }
	}
	xmlXPathFreeParserContext(parser);
    }
    xsltFreeCompMatchList(countPat);
    xsltFreeCompMatchList(fromPat);
    return amount;
}

static int
xsltNumberFormatGetValue(xmlXPathContextPtr context,
			 xmlNodePtr node,
			 xmlChar *value,
			 double *number)
{
    int amount = 0;
    xmlBufferPtr pattern;
    xmlXPathObjectPtr obj;
    
    pattern = xmlBufferCreate();
    if (pattern != NULL) {
	xmlBufferCCat(pattern, "number(");
	xmlBufferCat(pattern, value);
	xmlBufferCCat(pattern, ")");
	context->node = node;
	obj = xmlXPathEvalExpression(xmlBufferContent(pattern),
				     context);
	if (obj != NULL) {
	    *number = obj->floatval;
	    amount++;
	    xmlXPathFreeObject(obj);
	}
	xmlBufferFree(pattern);
    }
    return amount;
}

/**
 * xsltNumberFormat:
 * @ctxt: the XSLT transformation context
 * @data: the formatting informations
 * @node: the data to format
 *
 * Convert one number.
 */
void
xsltNumberFormat(xsltTransformContextPtr ctxt,
		 xsltNumberDataPtr data,
		 xmlNodePtr node)
{
    xmlBufferPtr output = NULL;
    xmlNodePtr copy = NULL;
    int amount, i;
    int array_amount;
    double number;
    xsltNumberFormatToken array[1024];

    output = xmlBufferCreate();
    if (output == NULL)
	goto XSLT_NUMBER_FORMAT_END;

    array_amount = xsltNumberFormatTokenize(data->format,
					    array,
					    sizeof(array)/sizeof(array[0]));

    /*
     * Evaluate the XPath expression to find the value(s)
     */
    if (data->value) {
	amount = xsltNumberFormatGetValue(ctxt->xpathCtxt,
					  node,
					  data->value,
					  &number);
	if (amount == 1) {
	    xsltNumberFormatInsertNumbers(data,
					  &number,
					  1,
					  &array,
					  array_amount,
					  output);
	}
	
    } else if (data->level) {
	
	if (xmlStrEqual(data->level, (const xmlChar *) "single")) {
	    amount = xsltNumberFormatGetMultipleLevel(ctxt,
						      node,
						      data->count,
						      data->from,
						      &number,
						      1,
						      data->doc,
						      data->node);
	    if (amount == 1) {
		xsltNumberFormatInsertNumbers(data,
					      &number,
					      1,
					      &array,
					      array_amount,
					      output);
	    }
	} else if (xmlStrEqual(data->level, (const xmlChar *) "multiple")) {
	    double numarray[1024];
	    int max = sizeof(numarray)/sizeof(numarray[0]);
	    amount = xsltNumberFormatGetMultipleLevel(ctxt,
						      node,
						      data->count,
						      data->from,
						      numarray,
						      max,
						      data->doc,
						      data->node);
	    if (amount > 0) {
		xsltNumberFormatInsertNumbers(data,
					      numarray,
					      amount,
					      &array,
					      array_amount,
					      output);
	    }
	} else if (xmlStrEqual(data->level, (const xmlChar *) "any")) {
	    amount = xsltNumberFormatGetAnyLevel(ctxt,
						 node,
						 data->count,
						 data->from,
						 &number,
						 1,
						 data->doc,
						 data->node);
	    if (amount > 0) {
		xsltNumberFormatInsertNumbers(data,
					      &number,
					      1,
					      &array,
					      array_amount,
					      output);
	    }
	}
    }
    /* Insert number as text node */
    copy = xmlNewText(xmlBufferContent(output));
    if (copy != NULL) {
	xmlAddChild(ctxt->insert, copy);
    }
    for (i = 0;i < array_amount;i++) {
	if (array[i].filling != NULL)
	    xmlFree(array[i].filling);
    }
    
 XSLT_NUMBER_FORMAT_END:
    if (output != NULL)
	xmlBufferFree(output);
}

static int
xsltFormatNumberPreSuffix(xsltDecimalFormatPtr self, xmlChar **format, xsltFormatNumberInfoPtr info)
{
    int	count=0;	/* will hold total length of prefix/suffix */

    while (1) {
	/* prefix / suffix ends at end of string or at first 'special' character */
	if (**format == 0)
	    return count;
	/* if next character 'escaped' just count it */
	if (**format == SYMBOL_QUOTE) {
	    if (*++(*format) == 0)
		return -1;
	    if (*++(*format) != SYMBOL_QUOTE)
		return -1;
	}
	else if (IS_SPECIAL(self, **format))
	    return count;
	/* else treat percent/per-mille as special cases, depending on whether +ve or -ve */
	else if (!info->is_negative_pattern) {
	    /* for +ve prefix/suffix, allow only a single occurence of either */
	    if (**format == self->percent[0]) {
		if (info->is_multiplier_set)
		    return -1;
		info->multiplier = 100;
		info->is_multiplier_set = TRUE;
	    } else if (**format == self->permille[0]) {
		if (info->is_multiplier_set)
		    return -1;
		info->multiplier = 1000;
		info->is_multiplier_set = TRUE;
	    }
	} else {
	    /* for -ve prefix/suffix, allow only single occurence & insist it's previously defined */
	    if (**format == self->percent[0]) {
		if (info->is_multiplier_set)
		    return -1;
		if (info->multiplier != 100)
		    return -1;
		info->is_multiplier_set = TRUE;
	    } else if (**format == self->permille[0]) {
		if (info->is_multiplier_set)
		    return -1;
		if (info->multiplier != 1000)
		    return -1;
		info->is_multiplier_set = TRUE;
	    }
	}
	
	count++;
	(*format)++;
    }
}
	    
/**
 * xsltFormatNumberConversion:
 * @self: the decimal format
 * @format: the format requested
 * @number: the value to format
 * @result: the place to ouput the result
 *
 * format-number() uses the JDK 1.1 DecimalFormat class:
 *
 * http://java.sun.com/products/jdk/1.1/docs/api/java.text.DecimalFormat.html
 *
 * Structure:
 *
 *   pattern    := subpattern{;subpattern}
 *   subpattern := {prefix}integer{.fraction}{suffix}
 *   prefix     := '\\u0000'..'\\uFFFD' - specialCharacters
 *   suffix     := '\\u0000'..'\\uFFFD' - specialCharacters
 *   integer    := '#'* '0'* '0'
 *   fraction   := '0'* '#'*
 *
 *   Notation:
 *    X*       0 or more instances of X
 *    (X | Y)  either X or Y.
 *    X..Y     any character from X up to Y, inclusive.
 *    S - T    characters in S, except those in T
 *
 * Special Characters:
 *
 *   Symbol Meaning
 *   0      a digit
 *   #      a digit, zero shows as absent
 *   .      placeholder for decimal separator
 *   ,      placeholder for grouping separator.
 *   ;      separates formats.
 *   -      default negative prefix.
 *   %      multiply by 100 and show as percentage
 *   ?      multiply by 1000 and show as per mille
 *   X      any other characters can be used in the prefix or suffix
 *   '      used to quote special characters in a prefix or suffix.
 */
xmlXPathError
xsltFormatNumberConversion(xsltDecimalFormatPtr self,
			   xmlChar *format,
			   double number,
			   xmlChar **result)
{
    xmlXPathError status = XPATH_EXPRESSION_OK;
    xmlBufferPtr buffer;
    xmlChar *the_format, *prefix = NULL, *suffix = NULL;
    xmlChar *nprefix, *nsuffix = NULL;
    xmlChar pchar;
    int	    prefix_length, suffix_length = 0, nprefix_length, nsuffix_length;
    double  scale;
    int	    j;
    xsltFormatNumberInfo format_info;
    /* delayed_multiplier allows a 'trailing' percent or permille to be treated as suffix */
    int		delayed_multiplier = 0;
    /* flag to show no -ve format present for -ve number */
    char	default_sign = 0;
    /* flag to show error found, should use default format */
    char	found_error = 0;

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
	return XPATH_MEMORY_ERROR;
    }

    format_info.integer_digits = 0;
    format_info.frac_digits = 0;
    format_info.frac_hash = 0;
    format_info.group = -1;
    format_info.multiplier = 1;
    format_info.is_multiplier_set = FALSE;
    format_info.is_negative_pattern = FALSE;

    the_format = format;

    /* First we process the +ve pattern to get percent / permille, as well as main format */
    prefix = the_format;
    prefix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
    if (prefix_length < 0) {
	found_error = 1;
	goto OUTPUT_NUMBER;
    }

    /* Here we process the "number" part of the format.  It gets a little messy because of    */
    /* the percent/per-mille - if that appears at the end, it may be part of the suffix       */
    /* instead of part of the number, so the variable delayed_multiplier is used to handle it */
    while ((*the_format != 0) &&
	   (*the_format != self->decimalPoint[0]) &&
	   (*the_format != self->patternSeparator[0])) {
	
	if (delayed_multiplier != 0) {
	    format_info.multiplier = delayed_multiplier;
	    format_info.is_multiplier_set = TRUE;
	    delayed_multiplier = 0;
	}
	if (*the_format == self->digit[0]) {
	    if (format_info.integer_digits > 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    if (format_info.group >= 0)
		format_info.group++;
	} else if (*the_format == self->zeroDigit[0]) {
	    format_info.integer_digits++;
	    if (format_info.group >= 0)
		format_info.group++;
	} else if (*the_format == self->grouping[0]) {
	    /* Reset group count */
	    format_info.group = 0;
	} else if (*the_format == self->percent[0]) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 100;
	} else  if (*the_format == self->permille[0]) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 1000;
	} else
	    break; /* while */
	
	the_format++;
    }

    /* We have finished the integer part, now work on fraction */
    if (*the_format == self->decimalPoint[0])
	the_format++;		/* Skip over the decimal */
    
    while (*the_format != 0) {
	
	if (*the_format == self->zeroDigit[0]) {
	    if (format_info.frac_hash != 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    format_info.frac_digits++;
	} else if (*the_format == self->digit[0]) {
	    format_info.frac_hash++;
	} else if (*the_format == self->percent[0]) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 100;
	    the_format++;
	    continue; /* while */
	} else if (*the_format == self->permille[0]) {
	    if (format_info.is_multiplier_set) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    delayed_multiplier = 1000;
	    the_format++;
	    continue; /* while */
	} else if (*the_format != self->grouping[0]) {
	    break; /* while */
	}
	the_format++;
	if (delayed_multiplier != 0) {
	    format_info.multiplier = delayed_multiplier;
	    delayed_multiplier = 0;
	    format_info.is_multiplier_set = TRUE;
	}
    }

    /* If delayed_multiplier is set after processing the "number" part, should be in suffix */
    if (delayed_multiplier != 0) {
	the_format--;
	delayed_multiplier = 0;
    }

    suffix = the_format;
    suffix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
    if ( (suffix_length < 0) ||
	 ((*the_format != 0) && (*the_format != self->patternSeparator[0])) ) {
	found_error = 1;
	goto OUTPUT_NUMBER;
    }

    /* We have processed the +ve prefix, number part and +ve suffix. */
    /* If the number is -ve, we must substitute the -ve prefix / suffix */
    if (number < 0) {
	the_format = (xmlChar *)xmlStrchr(format, self->patternSeparator[0]);
	if (the_format == NULL) {	/* No -ve pattern present, so use default signing */
	    default_sign = 1;
	}
	else {
	    /* Flag changes interpretation of percent/permille in -ve pattern */
	    the_format++;	/* Skip over pattern separator */
	    format_info.is_negative_pattern = TRUE;
	    format_info.is_multiplier_set = FALSE;

	    /* First do the -ve prefix */
	    nprefix = the_format;
	    nprefix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
	    if (nprefix_length<0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }

	    /* Next skip over the -ve number info */
	    the_format += prefix_length;
	    while (*the_format != 0) {
		if ( (*the_format == (self)->percent[0]) ||
		     (*the_format == (self)->permille[0]) ) {
		    if (format_info.is_multiplier_set) {
			found_error = 1;
			goto OUTPUT_NUMBER;
		    }
		    format_info.is_multiplier_set = TRUE;
		    delayed_multiplier = 1;
		}
		else if (IS_SPECIAL(self, *the_format))
		    delayed_multiplier = 0;
		else
		    break; /* while */
		the_format++;
	    }
	    if (delayed_multiplier != 0) {
		format_info.is_multiplier_set = FALSE;
		the_format--;
	    }

	    /* Finally do the -ve suffix */
	    if (*the_format != 0) {
		nsuffix = the_format;
		nsuffix_length = xsltFormatNumberPreSuffix(self, &the_format, &format_info);
		if (nsuffix_length < 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
		}
	    }
	    else
		nsuffix_length = 0;
	    if (*the_format != 0) {
		found_error = 1;
		goto OUTPUT_NUMBER;
	    }
	    /* Here's another Java peculiarity:
	     * if -ve prefix/suffix == +ve ones, discard & use default
	     */
	    if ((nprefix_length != prefix_length) || (nsuffix_length != suffix_length) ||
		((nprefix_length > 0) && (xmlStrncmp(nprefix, prefix, prefix_length) !=0 )) ||
		((nsuffix_length > 0) && (xmlStrncmp(nsuffix, suffix, suffix_length) !=0 ))) {
	 	prefix = nprefix;
		prefix_length = nprefix_length;
		suffix = nsuffix;
		suffix_length = nsuffix_length;
	    } else {
		default_sign = 1;
	    }
	}
    }

OUTPUT_NUMBER:
    if (found_error != 0) {
        xsltGenericError(xsltGenericErrorContext,
                "xsltFormatNumberConversion : error in format string, using default\n");
	default_sign = (number < 0.0) ? 1 : 0;
	prefix_length = suffix_length = 0;
	format_info.integer_digits = 1;
	format_info.frac_digits = 1;
	format_info.frac_hash = 4;
	format_info.group = -1;
	format_info.multiplier = 1;
    }

    /* Ready to output our number.  First see if "default sign" is required */
    if (default_sign != 0)
	xmlBufferAdd(buffer, self->minusSign, 1);

    /* Put the prefix into the buffer */
    for (j = 0; j < prefix_length; j++) {
	if ((pchar = *prefix++) == SYMBOL_QUOTE) {
	    pchar = *prefix++;
	    prefix++;
	}
	xmlBufferAdd(buffer, &pchar, 1);
    }

    /* Next do the integer part of the number */
    number = fabs(number) * (double)format_info.multiplier;
    scale = pow(10.0, (double)(format_info.frac_digits + format_info.frac_hash));
    number = floor((scale * number + 0.5)) / scale;
    xsltNumberFormatDecimal(buffer, floor(number), self->zeroDigit[0],
			    format_info.integer_digits,
			    format_info.group, (xmlChar)',');

    /* Next the fractional part, if required */
    if (format_info.frac_digits + format_info.frac_hash > 0) {
	number -= floor(number);
	if ((number != 0) || (format_info.frac_digits != 0)) {
	    xmlBufferAdd(buffer, self->decimalPoint, 1);
	    number = floor(scale * number + 0.5);
	    for (j = format_info.frac_hash; j > 0; j--) {
		if (fmod(number, 10.0) >= 1.0)
		    break; /* for */
		number /= 10.0;
	    }
	    xsltNumberFormatDecimal(buffer, floor(number), self->zeroDigit[0],
				format_info.frac_digits + j,
				0, (xmlChar)0);
	}
    }
    /* Put the suffix into the buffer */
    for (j = 0; j < suffix_length; j++) {
	if ((pchar = *suffix++) == SYMBOL_QUOTE) {
	    pchar = *suffix++;
	    suffix++;
	}
	xmlBufferAdd(buffer, &pchar, 1);
    }

    *result = xmlStrdup(xmlBufferContent(buffer));
    xmlBufferFree(buffer);
    return status;
}
