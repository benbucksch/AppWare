/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

#include "features.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>		/* free */
#include <string.h>		/* strdup */
#include <ctype.h>		/* isdigit, atoi */

/* Novell header files */
#include <dbcsmac.h>		/* NWNEXTCHAR */
#include <nwlocale.h>	/* Localization */
#include "resource.h"
#include "parse.h"
#include "cmdline.h"

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SaveCmdLineArgs() )

 MODULE       : cmdline.c

 GLOBALS USED : 


 SYNTAX       : SaveCmdLineArg(char *arg, int argNumber)

 PARAMETERS   :   -> input          <-output 
              -> arg - - - - - The argument that we need to save in an
											array.
 					-> argNumber - - The number in the array to save this 
											argument into

 RETURN       : None


 DESCRIPTION  : (see Algorithm)



 ALGORITHM    : If too many args (Array is full)
 						return Error
					  Else if the index in the array is already full
					  	free the argument string in that index
					  Duplicate the argument passed and put in array.


 END_MANUAL_ENTRY
****************************************************************************/
WORD SaveCmdLineArg(char *arg, int argNumber, PSPACE *p)
{
	if (argNumber >= MAX_SCRIPT_ARGS)
	{
		return(SCRIPT_TOO_MANY_ARGS);
	}
	else if (p->cmdLineArgs[argNumber] != NULL)
	{
		free(p->cmdLineArgs[argNumber]);
	}
	if ((p->cmdLineArgs[argNumber] = strdup(arg)) == NULL)
	{
		return(MEMORY_ERROR);
	}
	return(0);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ExpandCmdLineArgs() )

 MODULE       : cmdline.c

 GLOBALS USED : 


 SYNTAX       : ExpandCmdLineArgs(char *dest, char *src)

 PARAMETERS   :   -> input          <-output 
              -> dest- -  A destination buffer, a buffer to copy the 
										converted line into.
              -> src - -  A source buffer, contains the original line read
										from the login script.


 RETURN       : None


 DESCRIPTION  : Goes through the src string and converts %digits to their
 						Ascii string equivalent.  
					  Once we have the Ascii equivalent we replace the %digit with
					  	the command line parameter (This was saved in an array by
						calling the SaveCmdLineArgs() function).

 ALGORITHM    : While *string != NULL
 						If we find %digit in string
							replace with cmdLine parameter
						Else
							continue

 END_MANUAL_ENTRY
****************************************************************************/
void ExpandCmdLineArgs (char *dest, char *src, PSPACE *p)
{
	int 	i;
	char 	*ptr;

	while (*src != 0)
	{
		if (*src == '%')
		{
			ptr = NWNEXTCHAR(src);
			if (NWISDIGIT(*ptr) && (i=NWATOI(ptr)) < MAX_SCRIPT_ARGS)
			{
				i += p->shiftArgs;
				if (i >= 0 && i < MAX_SCRIPT_ARGS && p->cmdLineArgs[i] != NULL)
				{
					lstrcpy(dest, p->cmdLineArgs[i]);
					dest += lstrlen(dest);
				}
				src = ptr;
				do
				{
					src = NWNEXTCHAR(src);
				} while (NWISDIGIT(*src));
			}
			else
			{
				CopyMemory(dest, src, NWNUMBEROFBYTES(src));
				dest += NWNUMBEROFBYTES(dest);
				src = NWNEXTCHAR(src);
			}
		}
		else
		{
			CopyMemory(dest, src, NWNUMBEROFBYTES(src));
			dest += NWNUMBEROFBYTES(dest);
			src = NWNEXTCHAR(src);
		}
	}
	*dest = 0;
}
