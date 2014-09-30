/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

/*
 * This file contains utility functions
 */

/*
 * ========================================================================
 *	Include files
 * ========================================================================
 */
#include "features.h"
#ifdef _Windows
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <setjmp.h>

#include <a_alm.h>		/* declares AppWare Bus API prototypes			*/
#include <o_text.h> 	/* kdeclares OTYPE_TEXT and text object ALMs	*/
#include <o_list.h>
#include <o_table.h>

#define NWWIN
#include <constant.h>
#include <nwcalls.h>
#include <nwnet.h>
#include <nwintcon.h>
#include <dbcsmac.h>
#include <nwundoc.h>
#include <nwlocale.h>
#include "utils.h"
#include "resource.h"
#include "alm_auth.h"
#include "parse.h"
#include "inerrors.h"

#ifdef __cplusplus

extern "C" {

#endif

#include <dstools.h>

#ifdef __cplusplus

}

#endif

#define OT_TREE_NAME				0x7802	/* Object type of a tree name	*/

WORD	WinOutput;
extern void ScriptError (WORD error, PSPACE *p, ...);

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( Gettoken() )

 NAME         : *Gettoken (BYTE *list, BYTE *token)

 DESCRIPTION  : This function will get the next character string out of
 		the list that is terminated with a \r\n.  The list is expected to
		contain several of these tokens.

 PARAMETERS   :   -> input          <-output 
                ->  list = list to parse
                <-  token = character string found

 RETURN       :  BYTE *
 		This will return the new head of the list.

 END_MANUAL_ENTRY
 ===========================================================================
*/
BYTE *Gettoken (BYTE *list, BYTE *token)
{
	int cval;
	BYTE *p;

	/*
	 * Initialize to a null token and the beginning of the list.
	 */
	*token = '\0';
	p = list;

	/*
	 * Strip off all leading whitespace.
	 */
	while (*p != NULL)
	{
		cval = NWCharVal((char *)p);
		if (isspace(cval))
		{
			p = NWNEXTCHAR(p);
		}
		else
		{
			break;
		}
	}
	/*
	 * While we are not done and have not found a \r\n keep putting the
	 * characters into the token variable.  Since the \r and \n are NEVER
	 * double-byte characters I can do single byte copying.
	 */
	while (*p != NULL && (*p != '\r' && *(p+1) != '\n'))
	{
		*token++ = *p++;
	}
	*token = NULL;
	/*
	 * Strip off any trailing \r \n found.
	 */
	while (*p == '\r' || *p == '\n')
	{
		p++;
	}
	return(p);
}

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DSConnStatus() )

 NAME         : DSConnStatus(NWCONN_HANDLE connID, WORD *connStatus)

 DESCRIPTION  : This function will get the connection info and translate
 		information about the connection into the status word passed in.

		The possible bits that could be set are:
		
		CONNECTION_AUTHENTICATED	= we are DS authenticated
		CONNECTION_LOGGED_IN			= we are logged into this server
		CONNECTION_NDS					= the server is an NDS server
     CONNECTION_PRIMARY			= this is our primary connection
	
 PARAMETERS   :   -> input          <-output 
                ->  connID = connection to server
                <-  connStatus = status of connection specified

 RETURN       :  WORD
		0 = everything is fine
		Failures of NWDSGetConnectionInfo

 END_MANUAL_ENTRY
===========================================================================*/
WORD DSConnStatus (NWCONN_HANDLE connID,WORD *connStatus)
{
	NWCONN_TYPE		connType = 0;	/* Connection type returned			*/
	NWCCODE			ccode = 0;		/* Return codes							*/
	NWCONN_HANDLE	pConnID = 0;	/* Connection handle of primary		*/
	CONNECT_INFO	connInfo;		/* Connection info return buf			*/

	connInfo.connectFlags = 0;

	ccode = NWDSGetConnectionInfo(connID,NULL,&connType,NULL,NULL,
		NULL,NULL,NULL,NULL,NULL);

	if (connType & NWNDS_AUTHENTICATED)
	{
		connInfo.connectFlags |= CONNECTION_AUTHENTICATED;
	}

	if (connType & NWNDS_LICENSED)
	{
		connInfo.connectFlags |= CONNECTION_LOGGED_IN;
	}

	if (connType & NWNDS_CONNECTION)
	{
		connInfo.connectFlags |= CONNECTION_NDS;
	}

	if (NWGetPrimaryConnectionID(&pConnID) == 0)
	{
		if (connID == pConnID)
		{
       	connInfo.connectFlags |= CONNECTION_PRIMARY;
		}
	}
	*connStatus = connInfo.connectFlags;
	return(ccode);
}
	

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( WildMatch() )

 NAME         : WildMatch(BYTE *pattern, BYTE *string)

 DESCRIPTION  : This will take a pattern and a string and return whether
 		the wildcard pattern matches the string given.

 PARAMETERS   :   -> input          <-output 
                ->  pattern = wildcard pattern
					  ->	string = string to match

 RETURN       :  WORD
 		1 = match successful
		0 = no match

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD WildMatch (BYTE *pattern, BYTE *string)
{
	BYTE	p;
	BYTE	s;

	int	l;

	/*
	 * While we have not found an asterisk check carefully the pattern.
	 */
	while ((p = (BYTE)*pattern) != '*')
	{
		s = *string;
		if (p == '?')
		{
			if ((*pattern == (BYTE)SQUESTION) && ((s == 0) || (s == '.')))
			{
				/*
				 * special question skip period or end
				 */
				pattern = NWNEXTCHAR(pattern);
			}
			else
			{
				if (s == 0)
				{
					/*
					 * no char to match
					 */
					return(0);
				}
				/*
				 * succeed on ? match
				 */
				pattern = NWNEXTCHAR(pattern);
				string = NWNEXTCHAR(string);
			}
		}
		else if ((*pattern == (BYTE)SPERIOD) && (s == 0))
		{
			/*
			 * match special period to end-of-string
			 */
			pattern = NWNEXTCHAR(pattern);
		}
		else if (p != s)
		{
			/*
			 * failure on non-match
			 */
			return(0);
		}
		else if (p == 0)
		{
			/*
			 * success on match to nulls
			 */
			return(1);
		}
		else
		{
			/*
			 * chars match, but not end yet
			 */
			pattern = NWNEXTCHAR(pattern);
			string = NWNEXTCHAR(string);
		}
	}

	/*
	 * MUST MATCH AN ASTERISK WILDCARD
	 */
	if (*pattern == '*')
	{
		pattern = NWNEXTCHAR(pattern);
	}

	for (l = 0;
			string[l] != 0 && string[l] != '.';
			l+= NWNUMBEROFBYTES(&string[l]))
	{
		/*
		 * count max characters that may be skipped
		 */
	}
	p = *pattern;
	while (l >= 0)
	{
		s = string[l];
		if (((p == s) || (p == '?') || (p == '.')) && 
			WildMatch(pattern, &string[l]))
		{
			/*
			 * success
			 */
			return(1);
		}
		l -= NWNUMBEROFBYTES(NWPREVCHAR(string, &string[l]));
	}
	return(0);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( IsWild() )

 NAME         : IsWild(BYTE *s)

 DESCRIPTION  : This will search the string for a wildcard character and
 		return whether one was found or not.

 PARAMETERS   :   -> input          <-output 
                ->  s = string to search

 RETURN       :  WORD
 		1 = wildcard character in string
		0 = no wildcard characters

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD IsWild (BYTE *s)
{
	char ch;

	while ((ch = (char)*s) != 0)
	{
		s = NWNEXTCHAR(s);
		switch (ch)
		{
		case ASTERISK:
		case QUESTION:
		case SPERIOD:
		case SASTERISK:
		case SQUESTION:
			return(1);
		}
	}
	return(0);
}

#if LOGIN IS_DISABLED && LOGOUT IS_DISABLED

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AddToList() )

 NAME        : AddToList(AMEMBLOCKID *list, BYTE *row)

 DESCRIPTION : This a character string to a list

 PARAMETERS  :   -> input          <-output
 						-> list	= memory block of list
						->	row = row to add to list

 RETURN      : 0 - successful
					non-zero - failure

 END_MANUAL_ENTRY
 ============================================================================
*/
WORD AddToList (AMEMBLOCKID *list, BYTE *row)
{
	char	*lpList;

	/*
	 * If the list is empty and never had any data allocated to it, then
	 * go off and allocate a single byte block.  We will resize it
	 * appropriately below.
	 */
	if (*list == 0)
	{
		lpList = 0;
		if ((*list = AMemAllocate(1)) != 0)
		{
			lpList = (char *)AMemLock(*list);
		}
		if (lpList == 0)
		{
			return(-1);
		}
		*lpList = 0;
		AMemUnlock(*list);
	}

	/*
	 * Resize the memory block to be able to hold an additional row
	 * plus the \r\n separator and the NULL.
	 */
	AMemSetSize(*list, AMemGetSize(*list) + lstrlen((char *)row) + 3);
	lpList = (char *)AMemLock(*list);
	if (lpList == 0)
	{
		return(-1);
	}
	/*
	 * If the list already had something in it, then append a \r\n
	 * to separate this row from the previous one.  Then concatenate on
	 * this row of data.
	 */
	if (*lpList != 0)
	{
		strcat(lpList,"\r\n");
	}
	strcat(lpList,(char *)row);
	AMemUnlock(*list);
	return(0);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AddToTable() )

 NAME        : AddToTable(AOBJECTID oidTable, BYTE *column1)

 DESCRIPTION : This adds one character string to a table

 PARAMETERS  :   -> input          <-output
 						-> AOBJECTID of the table 
					  ->  column1 - char *

 RETURN      : 0 - successful
					non-zero - failure

 END_MANUAL_ENTRY
 ============================================================================
*/
WORD AddToTable (AOBJECTID oidTable, BYTE *column1)
{
	int  		ccode = 0;

	/*
	 * Add the above created row to the table
	 */
	ccode = tblAddRow(oidTable,(char *)column1,tblGetRowCount(oidTable));
	if (ccode == 0)
	{
		ccode = -1;
	}
	return(ccode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AddToTable() )

 NAME        : AddToTable(AOBJECTID oidTable, BYTE *column1, BYTE *column2)

 DESCRIPTION : This adds two character strings to a table

 PARAMETERS  :   -> input          <-output
 						-> AOBJECTID of the table 
					  ->  column1 - char *
						-> column2 - char *

 RETURN      : 0 - successful
					non-zero - failure

 END_MANUAL_ENTRY
 ============================================================================
*/
WORD AddToTable (AOBJECTID oidTable, BYTE *column1, BYTE *column2)
{
	char 		*temp;
	int  		ccode = 0;

	ccode = lstrlen((char *)column1) + lstrlen((char *)column2) + 2;
	temp = (char *)new char[ccode];
	if (temp == NULL)
	{
		return(-1);
	}

	/*
	 * Create the row as a tab delimited columns.
	 */
	wsprintf(temp, "%s\t%s", column1, column2);

	/*
	 * Add the above created row to the table
	 */
	ccode = tblAddRow(oidTable,temp,tblGetRowCount(oidTable));
	if (ccode == 0)
	{
		ccode = -1;
	}

	delete temp;
	return(ccode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AddToTable() )

 NAME        : AddToTable(AOBJECTID oidTable, BYTE *column1, BYTE *column2,
 							BYTE *column3)

 DESCRIPTION : This adds three character strings to a table

 PARAMETERS  :   -> input          <-output
 						-> AOBJECTID of the table 
					  ->  column1 - char *
						-> column2 - char *
						-> column3 - char *

 RETURN      : 0 - successful
					non-zero - failure

 END_MANUAL_ENTRY
 ============================================================================
*/
WORD AddToTable (AOBJECTID oidTable, BYTE *column1, BYTE *column2,
		BYTE *column3)
{
	char 		*temp;
	int  		ccode = 0;

	ccode = lstrlen((char *)column1) + lstrlen((char *)column2) + lstrlen((char *)column3) + 3;
	temp = (char *)new char[ccode];
	if (temp == NULL)
	{
		return(-1);
	}

	/*
	 * Create the row as a tab delimited columns.
	 */
	wsprintf(temp, "%s\t%s\t%s", column1, column2, column3);

	/*
	 * Add the above created row to the table
	 */
	ccode = tblAddRow(oidTable,temp,tblGetRowCount(oidTable));
	if (ccode == 0)
	{
		ccode = -1;
	}

	delete temp;
	return(ccode);
}
/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( MyOutputList() )

 NAME         : MyOutputList(AOBJECTID oiObject,AMEMBLOCKID *list)

 DESCRIPTION  : This function simply calls OTextSetValueFromMemblock and
     then frees the memory block and resets the AMEMBLOCKID to empty.  This
     way I am always freeing the memory that is finished being used.

 PARAMETERS   :   -> input          <-output
                  ->  oiText   = object pointer to receive values
	                <-  list     = pointer to memoryblock id

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
void MyOutputList (AOBJECTID oiObject, AMEMBLOCKID *list, WORD Paramnum)
{
  AObjConvertFromText(oiObject,*list);
  AFuncSetParameter(Paramnum,oiObject);
  AMemFree(*list);
  *list = 0;
}

#endif

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ReadAttribute() )

 NAME         : ReadAttribute()

 DESCRIPTION  : This reads the specified attribute of the the specified 
 						objectName, and returns the attribute values and the
						syntaxID.

 ALGORITHM    : Allocate and initialize needed buffers
 					  Read the DS database for the specified objectName
					  Go through all of the objects attributes until we find one
					  	that matches the attributeName passed in.
					  Compute the size of the attribute
					  Malloc a buffer of that size
					  Read the attribute into the buffer

 PARAMETERS   :   -> input          <-output 
			-> objectName - - The RDN or DN to read an attribute from
			-> attributeName- The name of the attribute to read
			<- attrBuffer - - A double pointer to a buffer that will
									 be allocated in this function.
			<- syntaxID - - - The syntax ID of the attribute found.

 RETURN       :  INT
		0 if successful, otherwise the ccode of the API call that 
 					failed.

 END_MANUAL_ENTRY
 ===========================================================================
*/
int ReadAttribute (char *objectName,char *attributeName,void **attrBuffer,
		uint32 *syntaxID)
{
	int					ccode;

	struct locals
	{
		int					j;

		char					attrName[MAX_SCHEMA_NAME_BYTES];

		long			 		iterationHandle;

		uint32				attrCount;
		uint32				attrValCount;
		uint32				attrValSize;

		Buf_T					*entryInfo;
		Buf_T					*attrNamesBuf;

		size_t				bufSize;

		NWDSContextHandle	context;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		ccode = MEMORY_ERROR;
		goto Error0;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->iterationHandle = -1;
	l->bufSize = DS_MAX_BUF_SIZE;

	/* initialize output buffer pointer */
	*attrBuffer = NULL;
	if ((l->context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		ccode = ERR_CONTEXT_CREATION;
		goto Error0;
	}

	ccode = NWDSAllocBuf(l->bufSize, &l->attrNamesBuf);
	if (ccode < 0)	goto Error0;

	ccode = NWDSAllocBuf(l->bufSize, &l->entryInfo);
	if (ccode < 0)	goto Error1;	

	ccode = NWDSInitBuf(l->context, DSV_READ, l->attrNamesBuf);
	if (ccode < 0)	goto Error2;

	ccode = NWDSPutAttrName(l->context,l->attrNamesBuf,attributeName);
	if (ccode < 0)	goto Error2;

	/* Now read the attribute */
	ccode = NWDSRead(l->context, objectName, DS_ATTRIBUTE_VALUES, FALSE, 
					l->attrNamesBuf, &l->iterationHandle,l->entryInfo);
	if (ccode < 0)	goto Error3;

	ccode = NWDSGetAttrCount( l->context, l->entryInfo, &l->attrCount );
	if (ccode < 0)	goto Error3;

	for (l->j = 0; l->j < (int)l->attrCount; l->j++) 
	{
		ccode = NWDSGetAttrName(l->context,l->entryInfo,l->attrName,
			&l->attrValCount,syntaxID);
		if (!(strcmpi(l->attrName, attributeName)))
		{
			ccode = NWDSComputeAttrValSize(l->context,l->entryInfo,*syntaxID,
				&l->attrValSize);
			if(ccode < 0)
				goto Error3;
			else
			{
				if((*attrBuffer = (void *)malloc((int)l->attrValSize)) == 
					(void *)NULL)
				{
					ccode = ERR_NOT_ENOUGH_MEMORY;
					goto Done;
				}
			}
			ccode = NWDSGetAttrVal(l->context,l->entryInfo,*syntaxID,*attrBuffer);
			if (ccode != 0)
			{
				free(*attrBuffer);
				*attrBuffer = 0;
			}
			goto Done;
		}
	}

Error3:
	ccode=ERR_NO_SUCH_ATTRIBUTE;

Done:
Error2:
	NWDSFreeBuf(l->entryInfo);
Error1:
	NWDSFreeBuf(l->attrNamesBuf);
	NWDSFreeContext(l->context);
Error0:
	if (l != 0)
	{
		delete l;
	}

	return(ccode);
}

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( GetDefaultProfile() )

 NAME         : GetDefaultProfile()

 DESCRIPTION  : 

 PARAMETERS   :   -> input          <-output 
                ->  
                <-  

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
void GetDefaultProfile(WORD connectionID, char *user, char *profile)
{
	BYTE propValue[128];
	WORD ccode;

	strset((char *)propValue, 0); 
	ccode = NWReadPropertyValue(connectionID, user, OT_USER, "DEFAULT_PROFILE",
					(BYTE)1,	propValue, (BYTE *)NULL, (BYTE *)NULL);
	MoveMemory(profile, propValue, 47);
	if ((ccode != 0) || (lstrlen(profile) == 0)) 
		FillMemory(profile, 0, 48);			/* invalid default profile or no default profile */
	
} /* end of GetDefaultProfile */

/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AttachtoServer() )

 NAME         : AttachtoServer()

 DESCRIPTION  : 

 PARAMETERS   :   -> input          <-output 
                ->  
                <-  

 RETURN       :  None

 END_MANUAL_ENTRY
 ===========================================================================
*/
WORD AttachtoServer (BYTE *ServerName, NWCONN_HANDLE *ConnectionID)
{
	BYTE 	TreeName[MAX_TREE_NAME_CHARS+1];		/* Tree of desired server		*/

	NWCCODE	ccode;

	WORD 		errcode;

	errcode = 0;
	/*
	 * Assume that the ServerName passed in is a server, so go through
	 * the gyrations for servers.
	 */
	NWLSTRUPR((char *)ServerName);
	ccode = NWAttachToFileServer((char *)ServerName,0,ConnectionID);
	if (ccode == CONNECTION_TABLE_FULL)
	{
		errcode = CONNECTION_FULL;
		goto Terminate;
	}

	/*
	 * If we failed to get a connection above, then we need to assume that
	 * the ServerName is really a tree name and go from there.
	 */
	if (ccode != 0 && ccode != ALREADY_ATTACHED)
	{
		lstrcpy((char *)TreeName,(char *)ServerName);
		ccode = getConnectionToTree(ConnectionID, (char *)ServerName, (char *)TreeName);
		if (ccode != SUCCESS)
		{
			errcode = ccode;
			goto Terminate;
		}
		ccode = NWAttachToFileServer((char *)ServerName,0,ConnectionID);
		if (ccode == CONNECTION_TABLE_FULL)
		{
			errcode = CONNECTION_FULL;
			goto Terminate;
		}
	}

Terminate:
	return(errcode);
} /* end of AttachtoServer */

/*===========================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( getConnectionToTree() )

 NAME         : getConnectionToTree (NWCONN_HANDLE *connectionID,
 							char *server,char *tree)

 DESCRIPTION  : This function will search for a server in the tree
 		specified.

		This function will first attach to a DS server.  This will either
		be the connection id passed in, the preferred DS server, a server
		in the preferred tree or finally the primary connection server.

		Once attached to a server the function will scan the DS objects
		for the identified tree.  If the tree is found, then the net address
		attribute is read for that tree and a connection is established 
		to that server.  Then the server name is retrieved and returned.

 PARAMETERS   :   -> input          <-output 
                ->  connectionID = optional connection to a DS Server
					  ->	tree = tree name to attach to
                <-  connectionID = set to server attached to
					  <-	server = name of the server
					  <-	tree = tree name where server is located

 RETURN       :	WORD
 		0 = everything is fine
		NO_SERVER_OR_TREE = no server found in tree

 MODIFICATIONS: February 18, 1994

 END_MANUAL_ENTRY
============================================================================*/
WORD getConnectionToTree (NWCONN_HANDLE *connectionID,
		char *server,char *tree)
{
	WORD errcode;

	struct locals
	{
		BYTE	srchName[MAX_DN_BYTES];		/* Wildcarded search tree name		*/
		BYTE	objName[MAX_DN_BYTES];		/* Scan object return name				*/
	 	BYTE	objNameWrk[MAX_DN_BYTES];	/* Temporary object buffer				*/
		BYTE	dataBuffer[MAX_DN_BYTES];	/* Property values						*/
	
		NWFLAGS	moreFlag;					/* More flags for nwreadproperty		*/
		NWFLAGS	hasProps;					/* Scan object flag properties		*/
		NWFLAGS	objFlags;					/* Scan object flags						*/
		NWFLAGS	objSec;						/* Scan object security flags			*/

		NWOBJ_ID		objID;					/* ID of last object found				*/

		NWOBJ_TYPE	srchType;				/* Search type to look for				*/
		NWOBJ_TYPE	objType;					/* Object type of found object		*/

		NWCONN_HANDLE	conn;					/* Connection to server in tree		*/

		WORD	i;									/* Looping index							*/

		NWCCODE	ccode;				 		/* Return codes							*/
	} *l;

	errcode = SUCCESS;
	l = new struct locals;
	if (l == 0)
	{
		errcode = MEMORY_ERROR;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	l->ccode = 0;

	/*
	 * The Tree must be upper case characters.  Also create a search
	 * name for tree listings -- place a wildcard at the end so that 
	 * partial matches will be accepted.
	 */
	NWLSTRUPR(tree);
	strcat(lstrcpy((char *)l->srchName,tree),"*");
	
	/*
	 * If we have not already got a connection, then go and find out the
	 * preferred DS server.
	 */
	if (*connectionID == 0)
	{
		l->ccode = NWGetPreferredDSServer(connectionID);
	}

	/*
	 *	At this point we have a problem, if the VLM doesn't find the
	 *	preferred TREE, or a server in the preferred TREE, or if the
	 *	user doesn't set the preferred TREE in the net.cfg, or if the
	 *	user specifies both a preferred server and a preferred tree, and
	 *	BIND.VLM loads before NDS.VLM, the VLM will not have an entry
	 *	for a preferred DS server.
	 */
	if (l->ccode == NO_PREFERRED_SPECIFIED)
	{
		/*
		 *	Let's try setting the specified tree as the preferred tree,
		 * that way, when we ask for the preferred DS server, we'll get
		 * an answer.
		 */
		l->ccode = NWSetPreferredDSTree(lstrlen(tree),(unsigned char *)tree);
		if (l->ccode == 0)
		{
			l->ccode = NWGetPreferredDSServer(connectionID);
		}
	}

	/* 
	 * if we can't get a preferred DS connection, just get the primary
	 *	connection ID
	 */
	if (l->ccode != 0)
	{
		NWGetPrimaryConnectionID(connectionID);
	}

	/*
	 * This validates the tree name by searching for the object and 
	 * if it is found, getting the address of a server in that tree.
	 */
	l->objID = -1;						/* Initialize to beginning of list */
	l->srchType = OT_TREE_NAME;
	while (1)
	{
		l->ccode = NWScanObject(*connectionID,(char *)l->srchName,l->srchType,
			&l->objID,(char *)l->objName,&l->objType,&l->hasProps,&l->objFlags,
			&l->objSec);
		
		/*
		 * An error occurred trying to get object
		 */
		if (l->ccode != 0)
		{
			/*
			 * Specified tree was not found
			 */
			errcode = NO_SERVER_OR_TREE;
			goto Terminate;
		}

		lstrcpyn((char *)l->objNameWrk,(char *)l->objName,MAX_TREE_NAME_CHARS);

		/*
		 * initially terminate tree name
		 */
		l->objNameWrk[MAX_TREE_NAME_CHARS] = 0;
		
		/*
		 * trim off trailing underscores
		 */
		for (l->i = MAX_TREE_NAME_CHARS - 1; l->objNameWrk[l->i] == '_'; l->i--)
		{
			l->objNameWrk[l->i] = 0;
		}

		/*
		 * tree name found
		 */
		if (strcmpi((char *)l->objNameWrk,(char *)tree) == 0)
		{
			l->ccode = NWReadPropertyValue(*connectionID,(char *)l->objName,OT_TREE_NAME,
						"NET_ADDRESS",1,l->dataBuffer,&l->moreFlag,NULL);
			if (l->ccode == 0)
			{
				/*
				 * We now have an address to a server in the tree. Connect to
				 * it and get its name. This will return the connection to
 				 * the server if already connected or make a new connection
				 */

				l->dataBuffer[10] = 0x04;	/* server slot */
				l->dataBuffer[11] = 0x51;
				l->ccode = NWDSGetConnectionSlot(1,1,12,l->dataBuffer,&l->conn);
				if (l->ccode == 0)
				{
					*connectionID = l->conn;
 					/*
					 * stuff back into the global server name
					 */
 					NWGetFileServerName(*connectionID,server);
					break;
				}
				/*
				 * cannot get a connection to the server. Try another server
				 * in the tree
				 */
			}
		}
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(errcode);
}

void FillPrimaryConnInfo (primaryConnectionStruct *p)
{
	struct locals
	{
		NWCONN_HANDLE		connID;

		WORD 					status;
		unsigned int		connNum;

		BYTE					ObjName[MAX_DN_BYTES];
		BYTE					LogTime[10];
		BYTE					tree[MAX_DN_BYTES];

		NWDSContextHandle	context;

		NWOBJ_TYPE			ObjType;

		NWOBJ_ID				ObjID;
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	if (NWGetPrimaryConnectionID(&l->connID) == 0)
	{
		p->connectionID = l->connID;
		DSConnStatus(l->connID,&l->status);

		p->serverLoginType = BINDERY;
		if ((l->status & CONNECTION_NDS) != 0)
		{
			p->serverLoginType = NDS;
		}
		p->serverVersion = NWIsDSServer(l->connID,(char *)l->tree) ? NDS : BINDERY;

		p->userName[0] = '\0';
		if (NWIsDSAuthenticated())
		{
			if ((l->context = NWDSCreateContext()) != (NWDSContextHandle)ERR_CONTEXT_CREATION)
			{
				NWDSWhoAmI(l->context,(char *)p->userName);
				NWDSRemoveAllTypes(l->context,(char *)p->userName,(char *)p->userName);
				NWDSFreeContext(l->context);
			}
		}
		else
		{
			if (NWGetConnectionNumber(l->connID,&l->connNum) == 0)
			{
				if (NWGetConnectionInformation(l->connID,l->connNum,(char *)l->ObjName,
					&l->ObjType,&l->ObjID,l->LogTime) == 0)
				{
					lstrcpy((char *)p->userName,(char *)l->ObjName);
				}
			}
		}
		NWGetFileServerName(l->connID,(char *)p->serverName);
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CleanChars() )

 MODULE       : common.c

 GLOBALS USED : 


 SYNTAX       : CleanChars(char *data, int length)

 PARAMETERS   :   -> input          <-output 
              -> data- - - The string to clean characters from
					-> length- - The length of the string

 RETURN       : The length of an unadulterated string.


 DESCRIPTION  : Clears out any characters below 0x20 (except carriage
 						return and newline) and anything above 0x80


 ALGORITHM    : If a characters below 0x20 (except carriage return and newline)
 						and anything above 0x80 is encountered	delete it.
					  else
					  	go on and look for control characters
					  Go until we hit the length (length is passed in)
					  Return the newlength of the string now cleaned.


 END_MANUAL_ENTRY
****************************************************************************/
int CleanChars(char *data, int length)
{
int newLength = length;

	while (length > 0)
	{
		length--;

		/* Check for data BELOW 20h (space) assuring it is not Carriage Return or New Line 
			and also check for data ABOVE 0x7F (127 =  or a DEL) - BR. 
			NOTE: This is for a display of a file therefore, the user chose the 
				way this is to be output to the screen.  So, this SHOULD NOT BE
				CHANGED FOR ENABLING... The user wants to strip out characters 0x7f and greater 
		*/

		if ( (*data < 0x20 && *data != '\r' && *data != '\n') ||
			 ((*data & 0x80) == 0x80) )
		{
			CopyMemory(data, data + 1, length);
			newLength--;
		}
		else
			data = NWNEXTCHAR(data);
	}
	return(newLength);
}

WORD FirstUnusedDrive (void)
{
	char	dummy[PATH_SIZE];							/* Dummy for status				*/
	WORD			dstatus;						/* Status of given drive		*/
	WORD drive;
	NWCONN_HANDLE dummyID;

	dstatus = 0;
	dummy[0] = 0;
	for (drive = 1; drive <= MAXDRIVES; drive++)
	{
		NWGetDriveStatus(drive,NW_FORMAT_NETWARE,&dstatus,&dummyID,
				dummy,dummy,dummy);
		if (dstatus == NW_UNMAPPED_DRIVE)
		{
			break;
		}
	}
	if (drive > MAXDRIVES)
	{
		return(-1);
	}
	return(drive);
}

WORD LastUnusedDrive (void)
{
	char	dummy[PATH_SIZE];							/* Dummy for status				*/
	WORD			dstatus;						/* Status of given drive		*/
	WORD drive;
	NWCONN_HANDLE dummyID;

	dstatus = 0;
	dummy[0] = 0;
	for (drive = MAXDRIVES; drive >= 1; drive--)
	{
		NWGetDriveStatus(drive,NW_FORMAT_NETWARE,&dstatus,&dummyID,
				dummy,dummy,dummy);
		if (dstatus == NW_UNMAPPED_DRIVE)
		{
			break;
		}
	}
	if (drive < 1)
	{
		return(-1);
	}
	return(drive);
}

WORD 	isNDSConnection (NWCONN_HANDLE connectionID)
{
	WORD	connType;

	DSConnStatus(connectionID,&connType);
	if (connType & CONNECTION_NDS)
		return TRUE;
	return FALSE;

} /* end of isDSConnection */

#if LOGOUT IS_DISABLED
/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( UpdateLoginTime() )

 MODULE       : cmds.c

 GLOBALS USED : NDSorBindery


 SYNTAX       : UpdateLoginTime(arg1, arg2)

 PARAMETERS   :   -> input          <-output 
              -> ConnID - - ConnectionID of the server containing the bindery
										property that is to be updated.
              -> name - - - Username associated with the connectionID

 RETURN       : None

 DESCRIPTION  : Get the file server date and time, and update the bindery
 						property associated with connID and name passed in.

 ALGORITHM    : Get the file server date and time.
 					  Write the date and time to MISC_LOGIN_INFO property.

 END_MANUAL_ENTRY
****************************************************************************/
void UpdateLoginTime(WORD connID, char *name, PSPACE *p)
{
WORD 	ccode;
BYTE 	date[128];

	if(p->NDSorBindery == BINDERY)
	{
		if((ccode = NWGetFileServerDateAndTime( connID, date )) == 0)
		{
			ccode = NWWritePropertyValue(connID, name, OT_USER, "MISC_LOGIN_INFO", 
								1, date, (BYTE)0);
			if (ccode != 0 && ccode != NO_SUCH_PROPERTY 
					&& ccode != NO_SUCH_OBJECT_OR_BAD_PASSWORD)
			{
				ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,UTILSCPP+1,ccode);
			}
		}
		else
		{
			ScriptError(SCRIPT_UNEXPECTED_INTERNAL_ERROR,p,UTILSCPP+2,ccode);
		}
	}
}
#endif

