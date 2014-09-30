/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#include "features.h"
#include <a_alm.h>
#include <o_nwerr.h>
#include "errors.h"
#include "authwin.h"
#include <memory.h>

/*
 * =========================================================================
 *                         Local Macro Definitions
 * =========================================================================
 */

#define  LOADERROR      "Cannot Load String"
#define  APP_NAME       "NetWare Authentication ALM"

/*
 * =========================================================================
 *                          Local Global Variables
 * =========================================================================
 */
extern HINSTANCE hInstance;

/*
 ===========================================================================
 ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisplayIDMessage() )

 NAME        : DisplayIDMessage(unsigned msgno,int iconType)

 DESCRIPTION : Displays a message in Windows by the message number

 SYNOPSIS    :

 PARAMETERS  : -> input          <- output
               -> msgno
               -> iconType

 RETURN      : If an icon is specified, this function will call MessageBox.
               If not, this function calls ReportNWError.
               The return value will be the return value of the function
               called.

 MODIFICATIONS: March 10, 1994

 END_MANUAL_ENTRY
 ===========================================================================
*/
int DisplayIDMessage (unsigned msgno,int iconType)
{
	int rc = TRUE;

	struct locals
	{
		char  message[1024];
	} *l;

	l = new struct locals;
	if (l == 0)
	{
		rc = FALSE;
		goto Terminate;
	}
	FillMemory(l,0,sizeof(struct locals));

	if (msgno != 0)
	{
		if (LoadString(hInstance,msgno,l->message,sizeof(l->message) - 1) == 0)
		{
			MessageBox(GetActiveWindow(),LOADERROR,APP_NAME,
				(MB_OK | MB_SYSTEMMODAL | MB_ICONHAND));
			rc = FALSE;
			goto Terminate;
		}
	}
	else
	{
		*(l->message) = NULL;
	}

	if (iconType == NO_ICON)
	{

#if LOGIN IS_ENABLED || LOGOUT IS_ENABLED

		rc = MessageBox(GetActiveWindow(),l->message,APP_NAME,
			MB_OK|MB_TASKMODAL|MB_ICONEXCLAMATION);

#else

		rc = ReportNWError(l->message);

#endif

		goto Terminate;
	}
	else
	{
		rc = MessageBox(GetActiveWindow(),l->message,APP_NAME,iconType);
		goto Terminate;
	}

Terminate:
	if (l != 0)
	{
		delete l;
	}

	return(rc);
}  /* DisplayIDMessage */


/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DisplayTextMessage() )

 NAME        : DisplayTextMessage(char *message,int iconType)

 DESCRIPTION : Displays a message in Windows by the string

 SYNOPSIS    :

 PARAMETERS  : -> input          <- output
               -> message
               -> iconType

 RETURN      : If an icon is specified, this function will call MessageBox.
               If not, this function calls ReportNWError.
               The return value will be the return value of the function
               called.

 MODIFICATIONS: March 10, 1994

 END_MANUAL_ENTRY
 ===========================================================================
*/
int DisplayTextMessage (char *message,int iconType)
{
	if (iconType == NO_ICON)
	{

#if LOGIN IS_ENABLED || LOGOUT IS_ENABLED

		return(MessageBox(GetActiveWindow(),message,APP_NAME,
			MB_OK|MB_TASKMODAL|MB_ICONEXCLAMATION));

#else

		return(ReportNWError(message));

#endif

	}
	else
	{
		return(MessageBox(GetActiveWindow(),message,APP_NAME,iconType));
	}
}  /* DisplayTextMessage */


/*
 ============================================================================
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DSALMGetMessage() )

 NAME        : DSALMGetMessage(unsigned msgno)

 DESCRIPTION : Finds a message contained in the message string table

 SYNOPSIS    :

 PARAMETERS  : -> input          <- output
               -> msgno

 RETURN      : returns a pointer to the message

 MODIFICATIONS: March 10, 1994

 END_MANUAL_ENTRY
 ===========================================================================
*/
char *DSALMGetMessage (unsigned msgno, char *messageStr, int size)
{
	if (LoadString(hInstance,msgno,messageStr,size-1) > 0)
	{
		return(messageStr);
	}
	*messageStr = NULL;
	return(messageStr);
}  /* DSALMGetMessage */

