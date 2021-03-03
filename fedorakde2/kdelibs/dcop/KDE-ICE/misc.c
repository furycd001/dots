/* $XConsortium: misc.c /main/29 1996/11/13 14:43:55 lehors $ */
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
#include "KDE-ICE/Xtrans.h"
#include "KDE-ICE/globals.h"
#include <stdio.h>
#include <errno.h>


/*
 * scratch buffer
 */

char *
IceAllocScratch (iceConn, size)

IceConn		iceConn;
unsigned long	size;

{
    if (!iceConn->scratch || size > iceConn->scratch_size)
    {
	if (iceConn->scratch)
	    free (iceConn->scratch);

	iceConn->scratch = (char *) malloc ((unsigned) size);
	iceConn->scratch_size = size;
    }

    return (iceConn->scratch);
}



/*
 * Output/Input buffer functions
 */

void IceFlush (IceConn iceConn)
{
    (*_IceWriteHandler) (iceConn,
	(unsigned long) (iceConn->outbufptr - iceConn->outbuf),
	iceConn->outbuf);

    iceConn->outbufptr = iceConn->outbuf;
}


int
IceGetOutBufSize (iceConn)

IceConn iceConn;

{
    return (iceConn->outbufmax - iceConn->outbuf);
}


int
IceGetInBufSize (iceConn)

IceConn iceConn;

{
    return (iceConn->inbufmax - iceConn->inbuf);
}



/*
 * informational functions
 */

IceConnectStatus
IceConnectionStatus (iceConn)

IceConn iceConn;

{
    return (iceConn->connection_status);
}


char *
IceVendor (iceConn)

IceConn iceConn;

{
    char *string = (char *) malloc (strlen (iceConn->vendor) + 1);

    if (string)
	strcpy (string, iceConn->vendor);

    return (string);
}


char *
IceRelease (iceConn)

IceConn iceConn;

{
    char *string = (char *) malloc (strlen (iceConn->release) + 1);

    if (string)
	strcpy (string, iceConn->release);

    return (string);
}


int
IceProtocolVersion (iceConn)

IceConn iceConn;

{
    return (_IceVersions[iceConn->my_ice_version_index].major_version);
}


int
IceProtocolRevision (iceConn)

IceConn iceConn;

{
    return (_IceVersions[iceConn->my_ice_version_index].minor_version);
}


int
IceConnectionNumber (iceConn)

IceConn iceConn;

{
    return (_KDE_IceTransGetConnectionNumber (iceConn->trans_conn));
}


char *
IceConnectionString (iceConn)

IceConn iceConn;

{
    if (iceConn->connection_string)
    {
	char *string = (char *) malloc (
	    strlen (iceConn->connection_string) + 1);

	if (string)
	    strcpy (string, iceConn->connection_string);

	return (string);
    }
    else
	return (NULL);
}


unsigned long
IceLastSentSequenceNumber (iceConn)

IceConn iceConn;

{
    return (iceConn->send_sequence);
}


unsigned long
IceLastReceivedSequenceNumber (iceConn)

IceConn iceConn;

{
    return (iceConn->receive_sequence);
}


Bool
IceSwapping (iceConn)

IceConn iceConn;

{
    return (iceConn->swap);
}



/*
 * Read "n" bytes from a connection.
 *
 * Return Status 0 if we detected an EXPECTED closed connection.
 *
 */

Status
_IceRead (iceConn, nbytes, ptr)

register IceConn iceConn;
unsigned long	 nbytes;
register char	 *ptr;

{
    register unsigned long nleft;

    nleft = nbytes;
    while (nleft > 0)
    {
	int nread;

	if (iceConn->io_ok)
	    nread = _KDE_IceTransRead (iceConn->trans_conn, ptr, (int) nleft);
	else
	    return (1);

	if (nread <= 0)
	{
#ifdef WIN32
	    errno = WSAGetLastError();
#endif
            if( nread < 0 && errno == EINTR )
                continue;
	    if (iceConn->want_to_close)
	    {
		/*
		 * We sent a WantToClose message and now we detected that
		 * the other side closed the connection.
		 */

		_IceConnectionClosed (iceConn);	    /* invoke watch procs */
		_IceFreeConnection (iceConn);

		return (0);
	    }
	    else 
	    {
		/*
		 * Fatal IO error.  First notify each protocol's IceIOErrorProc
		 * callback, then invoke the application IO error handler.
		 */

		iceConn->io_ok = False;

		if (iceConn->connection_status == IceConnectPending)
		{
		    /*
		     * Don't invoke IO error handler if we are in the
		     * middle of a connection setup.
		     */

		    return (1);
		}

		if (iceConn->process_msg_info)
		{
		    int i;

		    for (i = iceConn->his_min_opcode;
			i <= iceConn->his_max_opcode; i++)
		    {
			_IceProcessMsgInfo *process;

			process = &iceConn->process_msg_info[
			    i - iceConn->his_min_opcode];

			if (process->in_use)
			{
			    IceIOErrorProc IOErrProc = process->accept_flag ?
			      process->protocol->accept_client->io_error_proc :
			      process->protocol->orig_client->io_error_proc;

			    if (IOErrProc)
				(*IOErrProc) (iceConn);
			}
		    }
		}

		(*_IceIOErrorHandler) (iceConn);
		return (1);
	    }
	}

	nleft -= nread;
	ptr   += nread;
    }

    return (1);
}



/*
 * If we read a message header with a bad major or minor opcode,
 * we need to advance to the end of the message.  This way, the next
 * message can be processed correctly.
 */

void
_IceReadSkip (iceConn, nbytes)

register IceConn	iceConn;
register unsigned long	nbytes;

{
    char temp[512];

    while (nbytes > 0)
    {
	unsigned long rbytes = nbytes > 512 ? 512 : nbytes;

	_IceRead (iceConn, rbytes, temp);
	nbytes -= rbytes;
    }
}



/*
 * Write "n" bytes to a connection.
 */

void
_IceWrite (iceConn, nbytes, ptr)

register IceConn iceConn;
unsigned long	 nbytes;
register char	 *ptr;

{
    register unsigned long nleft;

    nleft = nbytes;
    while (nleft > 0)
    {
	int nwritten;

	if (iceConn->io_ok)
	    nwritten = _KDE_IceTransWrite (iceConn->trans_conn, ptr, (int) nleft);
	else
	    return;


	if (nwritten <= 0)
	{
#ifdef WIN32
	    errno = WSAGetLastError();
#endif
            if( nwritten < 0 && errno == EINTR )
                continue;
	    /*
	     * Fatal IO error.  First notify each protocol's IceIOErrorProc
	     * callback, then invoke the application IO error handler.
	     */

	    iceConn->io_ok = False;

	    if (iceConn->connection_status == IceConnectPending)
	    {
		/*
		 * Don't invoke IO error handler if we are in the
		 * middle of a connection setup.
		 */

		return;
	    }

	    if (iceConn->process_msg_info)
	    {
		int i;

		for (i = iceConn->his_min_opcode;
		     i <= iceConn->his_max_opcode; i++)
		{
		    _IceProcessMsgInfo *process;

		    process = &iceConn->process_msg_info[
			i - iceConn->his_min_opcode];

		    if (process->in_use)
		    {
			IceIOErrorProc IOErrProc = process->accept_flag ?
			    process->protocol->accept_client->io_error_proc :
			    process->protocol->orig_client->io_error_proc;

			if (IOErrProc)
			    (*IOErrProc) (iceConn);
		    }
		}
	    }

	    (*_IceIOErrorHandler) (iceConn);
	    return;
	}

	nleft -= nwritten;
	ptr   += nwritten;
    }
}

#ifdef WORD64

IceWriteData16 (iceConn, nbytes, data)

IceConn 	iceConn;
unsigned long 	nbytes;
short  		*data;

{
    int numShorts = nbytes / 2;
    int index = 0;

    while (index < numShorts)
    {
	int spaceLeft, count, i;
	int shortsLeft = numShorts - index;

	spaceLeft = iceConn->outbufmax - iceConn->outbufptr - 1;

	if (spaceLeft < 2)
	{
	    IceFlush (iceConn);
	    spaceLeft = iceConn->outbufmax - iceConn->outbufptr - 1;
	}

	count = (shortsLeft < spaceLeft / 2) ? shortsLeft : spaceLeft / 2;

	for (i = 0; i < count; i++)
	    STORE_CARD16 (iceConn->outbufptr, data[index++]);
    }
}


IceWriteData32 (iceConn, nbytes, data)

IceConn 	iceConn;
unsigned long  	nbytes;
int	 	*data;

{
    int numLongs = nbytes / 4;
    int index = 0;

    while (index < numLongs)
    {
	int spaceLeft, count, i;
	int longsLeft = numLongs - index;

	spaceLeft = iceConn->outbufmax - iceConn->outbufptr - 1;

	if (spaceLeft < 4)
	{
	    IceFlush (iceConn);
	    spaceLeft = iceConn->outbufmax - iceConn->outbufptr - 1;
	}

	count = (longsLeft < spaceLeft / 4) ? longsLeft : spaceLeft / 4;

	for (i = 0; i < count; i++)
	    STORE_CARD32 (iceConn->outbufptr, data[index++]);
    }
}


IceReadData16 (iceConn, swap, nbytes, data)

IceConn 	iceConn;
Bool		swap;
unsigned long 	nbytes;
short  		*data;

{
    /* NOT IMPLEMENTED YET */
}


IceReadData32 (iceConn, swap, nbytes, data)

IceConn 	iceConn;
Bool		swap;
unsigned long  	nbytes;
int	 	*data;

{
    /* NOT IMPLEMENTED YET */
}

#endif  /* WORD64 */



void
_IceAddOpcodeMapping (iceConn, hisOpcode, myOpcode)

IceConn	iceConn;
int 	hisOpcode;
int 	myOpcode;

{
    if (hisOpcode <= 0 || hisOpcode > 255)
    {
	return;
    }
    else if (iceConn->process_msg_info == NULL)
    {
	iceConn->process_msg_info = (_IceProcessMsgInfo *) malloc (
	    sizeof (_IceProcessMsgInfo));
	iceConn->his_min_opcode = iceConn->his_max_opcode = hisOpcode;
    }
    else if (hisOpcode < iceConn->his_min_opcode)
    {
	_IceProcessMsgInfo *oldVec = iceConn->process_msg_info;
	int oldsize = iceConn->his_max_opcode - iceConn->his_min_opcode + 1;
	int newsize = iceConn->his_max_opcode - hisOpcode + 1;
	int i;

	iceConn->process_msg_info = (_IceProcessMsgInfo *) malloc (
	    newsize * sizeof (_IceProcessMsgInfo));

	memcpy (&iceConn->process_msg_info[
	    iceConn->his_min_opcode - hisOpcode], oldVec,
	    oldsize * sizeof (_IceProcessMsgInfo));

	free ((char *) oldVec);

	for (i = hisOpcode + 1; i < iceConn->his_min_opcode; i++)
	{
	    iceConn->process_msg_info[i -
		iceConn->his_min_opcode].in_use = False;

	    iceConn->process_msg_info[i -
		iceConn->his_min_opcode].protocol = NULL;
	}

	iceConn->his_min_opcode = hisOpcode;
    }
    else if (hisOpcode > iceConn->his_max_opcode)
    {
	_IceProcessMsgInfo *oldVec = iceConn->process_msg_info;
	int oldsize = iceConn->his_max_opcode - iceConn->his_min_opcode + 1;
	int newsize = hisOpcode - iceConn->his_min_opcode + 1;
	int i;

	iceConn->process_msg_info = (_IceProcessMsgInfo *) malloc (
	    newsize * sizeof (_IceProcessMsgInfo));

	memcpy (iceConn->process_msg_info, oldVec,
	    oldsize * sizeof (_IceProcessMsgInfo));

	free ((char *) oldVec);

	for (i = iceConn->his_max_opcode + 1; i < hisOpcode; i++)
	{
	    iceConn->process_msg_info[i -
		iceConn->his_min_opcode].in_use = False;

	    iceConn->process_msg_info[i -
		iceConn->his_min_opcode].protocol = NULL;
	}

	iceConn->his_max_opcode = hisOpcode;
    }

    iceConn->process_msg_info[hisOpcode -
	iceConn->his_min_opcode].in_use = True;

    iceConn->process_msg_info[hisOpcode -
	iceConn->his_min_opcode].my_opcode = myOpcode;

    iceConn->process_msg_info[hisOpcode -
	iceConn->his_min_opcode].protocol = &_IceProtocols[myOpcode - 1];
}



char *
_IceGetPeerName (iceConn)

IceConn iceConn;

{
    return ((char*)_KDE_IceTransGetPeerNetworkId (iceConn->trans_conn));
}
