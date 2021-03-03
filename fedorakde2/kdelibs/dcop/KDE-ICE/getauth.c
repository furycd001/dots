/* $XConsortium: getauth.c,v 1.8 94/04/17 20:15:33 mor Exp $ */
/******************************************************************************


Copyright (c) 1993  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Author: Ralph Mor, X Consortium
******************************************************************************/

#include "KDE-ICE/ICElib.h"
#include "KDE-ICE/ICElibint.h"
#include "KDE-ICE/ICEutil.h"
#include "KDE-ICE/globals.h"

static Bool auth_valid (const char *auth_name, int num_auth_names, const char **auth_names, int *index_ret);

extern IceAuthDataEntry _IcePaAuthDataEntries[];


/*
 * The functions in this file are not a standard part of ICElib.
 *
 * The sample implementation uses an .ICEauthority to manipulate
 * authentication data.
 *
 * For the client that initiates a Protocol Setup, we look in the
 * .ICEauthority file to get the data.
 *
 * For the client accepting the Protocol Setup, we get the data
 * from an in-memory database of authentication data (set by the
 * application calling IceSetPaAuthData).  We have to get the data
 * from memory because getting it directly from the .ICEauthority
 * file is not secure - someone can just modify the contents of the
 * .ICEauthority file behind our back.
 */

void
_IceGetPoAuthData (protocolName, networkId, authName,
    authDataLenRet, authDataRet)

const char		*protocolName;
const char		*networkId;
const char		*authName;
unsigned short		*authDataLenRet;
char			**authDataRet;

{
    IceAuthFileEntry    *entry;

    entry = IceGetAuthFileEntry (protocolName, networkId, authName);

    if (entry)
    {
	*authDataLenRet = entry->auth_data_length;

	if ((*authDataRet = (char *) malloc (entry->auth_data_length)) != NULL)
	    memcpy (*authDataRet, entry->auth_data, entry->auth_data_length);
    }
    else
    {
	*authDataLenRet = 0;
	*authDataRet = NULL;
    }

    IceFreeAuthFileEntry (entry);
}



void
_IceGetPaAuthData (protocolName, networkId, authName,
    authDataLenRet, authDataRet)

const char		*protocolName;
const char		*networkId;
const char		*authName;
unsigned short	*authDataLenRet;
char		**authDataRet;

{
    IceAuthDataEntry	*entry;
    int			found = 0;
    int			i;

    for (i = 0; i < _IcePaAuthDataEntryCount && !found; i++)
    {
	entry = &_IcePaAuthDataEntries[i];

	found =
	    strcmp (protocolName, entry->protocol_name) == 0 &&
            strcmp (networkId, entry->network_id) == 0 &&
            strcmp (authName, entry->auth_name) == 0;
    }

    if (found)
    {
	*authDataLenRet = entry->auth_data_length;

	if ((*authDataRet = (char *) malloc (entry->auth_data_length)) != NULL)
	    memcpy (*authDataRet, entry->auth_data, entry->auth_data_length);
    }
    else
    {
	*authDataLenRet = 0;
	*authDataRet = NULL;
    }
}



void
_IceGetPoValidAuthIndices (protocol_name, network_id,
    num_auth_names, auth_names, num_indices_ret, indices_ret)

const char	*protocol_name;
const char	*network_id;
int		num_auth_names;
const char	**auth_names;
int		*num_indices_ret;
int		*indices_ret;		/* in/out arg */

{
    FILE    		*auth_file;
    char    		*filename;
    IceAuthFileEntry    *entry;
    int			index_ret, i;

    *num_indices_ret = 0;

    if (!(filename = IceAuthFileName ()))
	return;

    if (access (filename, R_OK) != 0)		/* checks REAL id */
	return;

    if (!(auth_file = fopen (filename, "rb")))
	return;

    for (;;)
    {
	if (!(entry = IceReadAuthFileEntry (auth_file)))
	    break;

	if (strcmp (protocol_name, entry->protocol_name) == 0 &&
	    strcmp (network_id, entry->network_id) == 0 &&
	    auth_valid (entry->auth_name, num_auth_names,
	    auth_names, &index_ret))
	{
	    /*
	     * Make sure we didn't store this index already.
	     */

	    for (i = 0; i < *num_indices_ret; i++)
		if (index_ret == indices_ret[i])
		    break;

	    if (i >= *num_indices_ret)
	    {
		indices_ret[*num_indices_ret] = index_ret;
		*num_indices_ret += 1;
	    }
	}

	IceFreeAuthFileEntry (entry);
    }

    fclose (auth_file);
}



void
_IceGetPaValidAuthIndices (protocol_name, network_id,
    num_auth_names, auth_names, num_indices_ret, indices_ret)

const char	*protocol_name;
const char	*network_id;
int		num_auth_names;
const char	**auth_names;
int		*num_indices_ret;
int		*indices_ret;		/* in/out arg */

{
    int			index_ret;
    int			i, j;
    IceAuthDataEntry	*entry;

    *num_indices_ret = 0;

    for (i = 0;	i < _IcePaAuthDataEntryCount; i++)
    {
	entry = &_IcePaAuthDataEntries[i];

	if (strcmp (protocol_name, entry->protocol_name) == 0 &&
            strcmp (network_id, entry->network_id) == 0 &&
	    auth_valid (entry->auth_name, num_auth_names,
	    auth_names, &index_ret))
	{
	    /*
	     * Make sure we didn't store this index already.
	     */

	    for (j = 0; j < *num_indices_ret; j++)
		if (index_ret == indices_ret[j])
		    break;

	    if (j >= *num_indices_ret)
	    {
		indices_ret[*num_indices_ret] = index_ret;
		*num_indices_ret += 1;
	    }
	}
    }
}



/*
 * local routines
 */

static Bool
auth_valid (const char *auth_name, int num_auth_names, const char **auth_names, int *index_ret)
{
    /*
     * Check if auth_name is in auth_names.  Return index.
     */

    int i;

    for (i = 0; i < num_auth_names; i++)
	if (strcmp (auth_name, auth_names[i]) == 0)
	{
	    break;
	}
   
    if (i < num_auth_names)
    {
	*index_ret = i;
	return (1);
    }
    else
	return (0);
}
