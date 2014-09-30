////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag_api.cpp"
//
// Flag Object APIs for communicating with other objects. 
//
////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<string.h>
#include	<a_alm.h>
#include	"flag_hdr.h"
#include	"flag.h"
#include  "o_flag.h"


/**********************************************************************
 * OFlagGet: Return the boolean value for the flag object specified
 *			by the flag ID.                                                                   			
 **********************************************************************/

BOOL ALMAPI OFlagGet(
	AOBJECTID	flagID)
{
	FLAG	flag(flagID);

  return flag.IsSet() ? TRUE : FALSE;
}


/**********************************************************************
 * OFlagSet: Set or clears the boolean value of the flag obj,
 *			the value of the boolean is specified by theBool boolean.                                                                   			
 **********************************************************************/

void ALMAPI OFlagSet(
	AOBJECTID	flagID,
	BOOL			theBool)
{
	FLAG	flag(flagID);

	if(theBool)
  	flag.Set();
	else
  	flag.Clear();
}


/**********************************************************************
 * OFlagTextToBool: returns a boolean based on the setting of the text
 *			for the flag object.                                                                   			
 **********************************************************************/

BOOL ALMAPI OFlagTextToBool(
	AOBJECTID	flagID,
	LPSTR			text)

	{
	FLAG	flag(flagID);

	if(!lstrcmpi(text, flag.SetText()))
  	return TRUE;
	else
		return FALSE;
	}


/**********************************************************************
 * OFlagBoolToText: Based on a boolean it sets the text for the
 *			flag object specified by the flag ID.                                                                   			
 **********************************************************************/

UINT ALMAPI OFlagBoolToText(
	AOBJECTID		flagID,
	BOOL				theBool,
	AMEMBLOCKID	theBlock,
	LPSTR				szBuffer,
	UINT				uBufLen)

	{
	FLAG	flag(flagID);
  LPSTR	text = theBool ? flag.SetText() : flag.ClearText();
	UINT	len = (UINT)lstrlen(text);

	if (theBlock)
		{
		AMemSetSize(theBlock, len + 1);
		LPSTR	blockText = (LPSTR) AMemLock(theBlock);
		lstrcpy(blockText, text);
		}

	if (szBuffer && uBufLen>0)
		{
		len = min(len, uBufLen-1);
		lstrcpyn(szBuffer, text, len);
		szBuffer[len] = 0;
		}

  return len;
	}


/**********************************************************************
 * OFlagTextToObject: Set or clears the boolean value of the flag obj,
 *			the value of the boolean is specified by the text passed in.                                                                   			
 **********************************************************************/

void ALMAPI OFlagTextToObject(
	AOBJECTID	flagID,
	LPSTR			text)

	{
	FLAG	flag(flagID);

	if(!lstrcmpi(text, flag.SetText()))
  	flag.Set();
	else
  	flag.Clear();
	}


/**********************************************************************
 * OFlagTextToObject: Pass the text string for the flag object 
 *			specified by flagID.                                                                   			
 **********************************************************************/

UINT ALMAPI OFlagObjectToText(
	AOBJECTID		flagID,
	AMEMBLOCKID	theBlock,
	LPSTR				szBuffer,
	UINT				uBufLen)

	{
	FLAG	flag(flagID);
  LPSTR	text = flag.GetText();
	UINT	len = (UINT)lstrlen(text);

	if (theBlock)
		{
		AMemSetSize(theBlock, len + 1);
		LPSTR	blockText = (LPSTR) AMemLock(theBlock);
		lstrcpy(blockText, text);
		}

	if (szBuffer && uBufLen>0)
		{
		len = min(len, uBufLen-1);
		lstrcpyn(szBuffer, text, len);
		szBuffer[len] = 0;
		}

  return len;
	}

////////////////////////////////////////////////////////////////////////////
