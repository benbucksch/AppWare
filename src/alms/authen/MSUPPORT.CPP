/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#include "features.h"
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <dbcsmac.h>		/* NWNEXTCHAR */
#include <nwlocale.h>		/* Localization */
//#include <nwtools.h>		/* NWVersionOption */

//#include <nwdsattr.h>	/* Email_Address_T ... */
#include <nwdserr.h>		/* ERR_BAD_SYNTAX */
#include <nwdsdefs.h>	/* SYN_... */

#include <a_alm.h>

#ifdef __cplusplus

extern "C" {

#endif

#include <nwbindry.h>
#include <dstools.h>
#include <nwdsname.h>	/* NWDSWhoAmI */
#include <nwdsdsa.h>		/* NWDSWhoAmI */
#include <nwndscon.h>		/* NWIsDSAuthenticated */
#include <nwdsbuft.h>
#include <nwdpath.h>
#include <nwdsmisc.h>

#ifdef __cplusplus

}
#endif

//#include "globals.h"
#include "mx.h"
#include "parse.h"	/* Needed for parseflag definition */
#include "resource.h"
#include "utils.h"
#include "msupport.h"
#include "cmds.h"

extern	void ScriptError(WORD error, PSPACE *p,...);

#define SEPERATOR 0x01
#define CENTURY	1900

void DosGetDateTime(MYDATETIME *now)
{
	struct dostime_t timep;
	struct dosdate_t	datep;

 	now->mytimezone = 0;

	_dos_getdate(&datep);
	now->day = datep.day;
	now->month = datep.month;
	now->year = datep.year;
	now->weekday = datep.dayofweek;

	_dos_gettime(&timep);
	now->hours = timep.hour;
	now->minutes = timep.minute;
	now->seconds = timep.second;
	now->hundredths = timep.hsecond;
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( UpperToken() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : UpperToken()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: A pointer to a string.


 RETURN       :
 * On Exit: The string (or character) passed in is in uppercase 

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Uppercases a string (or character) until a space or the end of the 
 *			string is found.

 END_MANUAL_ENTRY
****************************************************************************/
	
void UpperToken(char *token)
{
	while (*token != 0 && *token != SEPERATOR)
	{
		*token = (char)NWTOUPPER(*token);
		token = NWNEXTCHAR(token);
	}
} /* end of UpperToken */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CopyToken() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : CopyToken()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: A pointer to a buffer to copy a string and the number of the
 *				token to be copied. (The number will references a string).

 RETURN       :
 * On Exit: The token is copied into the buffer passed.

 DESCRIPTION  : 

 ALGORITHM    : 
 * 		Copies a token into an array of strings.  
 *			The TChP() function (Token Character Pointer), returns a pointer
 *				to the string (in the current parse line) associated with the
 *				token number.  Token number 0 is the first character string in
 *				the current parse line, Token number 1 is the second string and
 *				so on.
 *			The TVal() function (Token Value), returns the length of a variable
 *				string, or the character if the token is a character.  This value
 *				is usually set by the parser, but can be changed by the program.
 *
 *			NOTE: this function should not be called when the token is a single
 *				character.  This will make TVal the character and give an invalid
 *				length to the CopyMemory() function.

 END_MANUAL_ENTRY
****************************************************************************/
	
void CopyToken(MXGLOBALS *mxg,char *destinationString, char *token, char tokenNumber)
{
	CopyMemory(destinationString, token, TVal(mxg,tokenNumber));	
	
	/*
	 * NULL terminate the string
	 */
	destinationString[TVal(mxg,tokenNumber)] = '\0';
} /* end of CopyToken */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SaveString() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : SaveString()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: A pointer to a string.

 RETURN       :
 * On Exit: The string passed in is saved into a buffer called tokenString.
 *				The index into the array of pointers is returned.
 *				Or a 0 is returned if an error occurs.

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Duplicates the string passed in and places the pointer to the 
 *				duplicated string in an array of pointers. 
 *
 *			NOTE: The array of pointers to strings is declared as:
 *					static char *tokenString[255] = {""};
 *			
 *			This algorithm goes through and finds the first empty place in
 *				the tokenString array of pointers (pointers to strings, that is), 
 *				duplicates the string (by implicitly allocating memory) and 
 *				returns the index in the array of pointers that the pointer to 
 *				the string was placed.
 *
 *			On error a 0 is returned and, by the way, tokenStrings[0] points to 
 *			an empty string.

 END_MANUAL_ENTRY
****************************************************************************/
	
int SaveString(char *str, PSPACE *p)
{
	int i;

	for (i = 1; i < sizeof(p->tokenString)/sizeof(p->tokenString[0]); i++)
	{
		if (p->tokenString[i] == p->emptystring)
		{
			if ((p->tokenString[i] = strdup(str)) == NULL)
			{
				ScriptError(MEMORY_ERROR,p);
				return(0);
			}
			return(i);
		}
	}
	ScriptError(SCRIPT_STRING_TABLE,p);
	return(0);
} /* end of SaveString */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( FreeString() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : FreeString()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: An index into a table (array) of pointers to strings.

 RETURN       : 
 * On Exit: The string at the index number in the array is freed.

 DESCRIPTION  : 

 ALGORITHM    : 
 * 		Frees the memory used by a string.  The string is found by indexing
 *				into the tokenString array of pointers.
 *			For example, if strID was 7.  Then tokenString[7] would contain a 
 *				pointer to a string.  This would be the string freed and 
 *				tokenString[7] would be NULLED out.
 *
 *			NOTE: The array of pointers to strings is declared as:
 *					static char *tokenStrings[255] = {""};

 END_MANUAL_ENTRY
****************************************************************************/
	
void FreeString (int strID, PSPACE *p)
{
	if (strID != 0)
	{
		free(p->tokenString[strID]);
		p->tokenString[strID] = p->emptystring;
	}
} /* end of FreeString */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SaveToken() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : SaveToken()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: The number of the token to be copied. 
 *					(The token number will references a string).

 RETURN       : 
 * On Exit: Character string referenced by the token number is saved into
 *					an array of pointers to strings.
 *				On error a 0 is returned.

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Save the character string associated with the token number into
 *			a temporary buffer.  And NULL terminate the string in the buffer.
 *			Then call SaveString which duplicates the string and places it into
 *			an array of strings.  Then sets the master value to the index 
 *			returned from save string.
 *
 *			Normally, we would have passed TChP(tokenNumber) to SaveString, except
 *				it needed to be NULL terminated.  
 *
 *			For an explanation of the TVal and TChP() functions, look at the 
 *				CopyToken function above.

 END_MANUAL_ENTRY
****************************************************************************/

void SaveToken (char tokenNumber, PSPACE *p)
{
	int	size;
	char	temp[MAX_STRING];

	size = TVal(&p->mxg,tokenNumber);						/* Get the size of the token */
	CopyMemory(temp, TChP(&p->mxg,tokenNumber), size);		/* Copy the token into temp buffer */
	temp[size] = 0;									/* Be sure to NULL terminate it */
	SetMVal(&p->mxg,SaveString(temp,p));						/* Set the master value */
} /* end of SaveToken */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SaveQuotedToken() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : SaveQuotedToken()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: NOTHING passed in.

 RETURN       : 
 * On Exit: Translated string is saved into the tokenString array.

 DESCRIPTION  : 

 ALGORITHM    : 
 * 	Assumes that token number 0 is the first quote of a quoted string.
 *		Translates the string to valid 'C' escape codes.
 * 	Saves the translated string in the tokenString array.
 * 	sets mval to the index into tokenString.
 * 	Skips the token just processed. (Skips over the quoted string).

 END_MANUAL_ENTRY
****************************************************************************/
	
void SaveQuotedToken(PSPACE *p)
{
	char	literal;
	char	*srcBase;
	char	*src;
	char	*dest;
	char	temp[MAX_STRING];

	srcBase = TChP(&p->mxg,0);
	src = srcBase + 1;
	dest = temp;
	while (dest - temp < sizeof(temp) - 1 && *src != 0 && *src != '"')
	{
		if (*src != '\\')
		{
			*dest++ = *src++;
		}
		else
		{
			switch (*(src + 1))
			{
				case 'r':
					literal = '\r';
					goto GotLiteral;

				case 'n':
					literal = '\n';
					goto GotLiteral;

				case '\\':
					literal = '\\';
					goto GotLiteral;

				case '7':
					literal = '\a';
					goto GotLiteral;

				case '"':
					literal = '"';
					/* FALLTHROUGH */

				GotLiteral:
					*dest++ = literal;
					src += 2;
					break;

				default:
					*dest++ = *src++;
			}
		}
	}
	*dest = 0;
	/* Skip to the next token */
	SkipVariableToken(&p->mxg,(unsigned char)(src - srcBase));
	SetMVal(&p->mxg,SaveString(temp,p));
} /* end of SaveQuotedToken */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( SaveQuotedString() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : SaveQuotedString()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: NOTHING passed in.

 RETURN       : 
 * On Exit: String is saved into parsedName (NULL terminated), without quotes. 

 DESCRIPTION  : 

 ALGORITHM    : 
 * 	Assumes that token number 0 is the first quote of a quoted string.
 * 	Saves the string into parsedName.
 * 	Skips the token just processed. (Skips over the quoted string).

 END_MANUAL_ENTRY
****************************************************************************/
	
void SaveQuotedString (PSPACE *p)
{
	char	*srcBase;
	char	*src;

	int	i;

	srcBase = TChP(&p->mxg,0);
	src = srcBase + 1;			/* skip the '"' sign */

	for (i = 0;
		i < MAX_STRING - 1 && *src != (char)NULL && *src != '"';
		src++, i++)
	{
	 	p->parsedName[i] = *src;
	}
	p->parsedName[i] = (char)NULL;

	/* Skip to the next token */
	SkipVariableToken(&p->mxg,(unsigned char)(src - srcBase));
} /* end of SaveQuotedString */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( Evaluate() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : Evaluate()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: A left operator, the relationship operator, a right operator and
 *				wether this is to be evaluated as a string or as values.

 RETURN       : 
 * On Exit: Returns the condition of relation.  (i.e. if line is 'if 4 == 5',
 *				returns TRUE (1) if 4 is equal to 5, or FALSE (0) if they are 
 *				not equal).

 DESCRIPTION  : 

 ALGORITHM    : 
 *			If by value is 0, simply compare the strings recieved.
 *			Otherwise, convert the strings to long intergers and do a comparison
 *			(subtraction is similar to a compare).
 *			Take the result of the comparison (subtraction) and apply the 
 *			appropriate relational operator to it.
 *			Return the result of the relation operation.
 *			Default return 0.

 END_MANUAL_ENTRY
****************************************************************************/
	
int Evaluate (char *leftOp, enum RELTYPE rel, char *rightOp, int byValue)
{
	long	result;

	result = byValue? strtol(leftOp, NULL, 10) - strtol(rightOp, NULL, 10):
							(long)strcmpi(leftOp, rightOp);
	switch (rel)
	{
		case REL_E:		return(result == 0);
		case REL_G:		return(result >  0);
		case REL_L:		return(result <  0);
		case REL_NE:	return(result != 0);
		case REL_GE:	return(result >= 0);
		case REL_LE:	return(result <= 0);
	}
	return(0);
} /* end of Evaluate */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( GroupMember() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : GroupMember()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: A pointer to a user entered groupName

 RETURN       : 
 * On Exit: returns a TRUE or FALSE depending on if the object is in the Bindery.

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Makes a call to the bindery to see if the group name passed in 
 *			exists in the bindery.  Returns a TRUE or FALSE depending on if 
 *			the object is in the bindery.  A return code of 0 from the 
 *			NWIsObjectInSet means the object was found, a Non-Zero return 
 *			code means object was NOT found.

 END_MANUAL_ENTRY
****************************************************************************/
	
WORD GroupMember (char *groupName,PSPACE *p)
{
	NWDSContextHandle	context;						/* Current context				*/

	NWCCODE	ccode = FALSE;
	char		buffer[MAX_DN_BYTES];

	Buf_T 	*attrNames;             // attribute names requested
	Buf_T		*objectInfo;            // reply buffer with names/values

	struct attrNameP	*firstAttr = NULL;
	struct attrValP  	*attrVal;

	context = ERR_CONTEXT_CREATION;
	if (p->NDSorBindery == BINDERY)
	{
		ccode = (NWIsObjectInSet(p->scriptconnID, groupName,
			OT_USER_GROUP,"GROUP_MEMBERS", p->fullName, OT_USER) == 0);
	}
	else
	{
		if ((context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			ScriptError(CANNOT_CREATE_CONTEXT,p);
			return(FALSE);
		}

	  	/*
		 * initialize data fields and structures
	    */
   	if ((ccode = NWDSAllocBuf(512, &attrNames)) != 0)
		{
			ScriptError(MEMORY_ERROR,p);
			ccode = FALSE;
			goto Error3;
		}

   	if ((ccode = NWDSAllocBuf(4096, &objectInfo)) != 0)
		{
			ScriptError(MEMORY_ERROR,p);
			ccode = FALSE;
			goto Error3;
		}

   	if ((ccode = NWDSInitBuf(context, DSV_READ, attrNames)) != 0)
		{
			ScriptError(MEMORY_ERROR,p);
			ccode = FALSE;
			goto Error3;
		}

   	if (DSAddAttrNameToList("Group Membership",0,&firstAttr,
			&firstAttr,FALSE,context,attrNames))
		{
			ScriptError(MEMORY_ERROR,p);
			ccode = FALSE;
			goto Error2;
		}

		/*
		 * get group name RDN
		 */
	 	if (NWDSCanonicalizeName(context, groupName, buffer))	
		{
			ScriptError(SCRIPT_CANONICALIZE,p);
			ccode = FALSE;
			goto Error1;
		}
		NWDSRemoveAllTypes(context,buffer,buffer);
		NWDSAbbreviateName(context,buffer,groupName);

		/*
		 * get my user name (returns rdn)
		 */
		NWDSWhoAmI(context,buffer);

   	DSLoadAttrValues(context,buffer,FALSE,attrNames,objectInfo,
			&firstAttr,FALSE);

		ccode = FALSE;
		/*
		 * starting value
		 */
		attrVal = firstAttr->first;
		while (attrVal)
		{
			/*
			 * this abbreviate name and remove all types is due to the fact that
			 * nwnet does not always return abbreviated/typeless names.
			 * For example: if current context is SOFT_DEV.CIS.NOVELL a group
			 * name may come back as "JAM.SOFT_DEV."
			 * KPW 09-22-93
			 */
			/*
			 * ############# this can be changed if desired after libs work
				##################
			*/
	 		NWDSCanonicalizeName(context, attrVal->val, buffer);
			NWDSAbbreviateName(context,buffer,buffer);
			NWDSRemoveAllTypes(context,buffer,buffer);

			if (!strcmpi(buffer, groupName))
			{
				ccode = TRUE;
				break;
			}
			attrVal = attrVal->next;
		}

Error1:
	   DSFreeAttrNameList(&firstAttr);
Error2:
   	NWDSFreeBuf(objectInfo);
Error3:
		if (context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
		{
			NWDSFreeContext(context);
		}
	   NWDSFreeBuf(attrNames);
	}
	return(ccode);
} /* end of GroupMember */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ConcatStringTokens() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : ConcatStringTokens()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: The identification number of two tokens to be concatenated.

 RETURN       : 
 * On Exit: Set the Master value to the index into the tokenStrings array.

 DESCRIPTION  : 

 ALGORITHM    : 
 * 	Assumes that token number 0 is the first quote of a quoted string.
 * 	Saves the translated string in the tokenString array.
 * 	sets mval to the index into tokenString.
 * 	Skips the token just processed. (Skips over the quoted string).

 END_MANUAL_ENTRY
****************************************************************************/
	
void ConcatStringTokens (int leftTokenID, int rightTokenID, PSPACE *p)
{
	char	temp[MAX_STRING];
	char	*left;
	char	*right;

	int	leftStrID;
	int	rightStrID;

	/*
	 * Get the value of the left token ID
	 */
	leftStrID = TVal(&p->mxg,leftTokenID);

	/*
	 * Get the value of the right token ID
	 */
	rightStrID = TVal(&p->mxg,rightTokenID);

	/*
	 * Look the string up in the tokenString array
	 * and set pointers to the string.
	 */
	left = p->tokenString[leftStrID];
	right = p->tokenString[rightStrID];

	/*
	 * If the left string is empty
	 */
	if (left == 0 || *left == 0)
	{
		/*
		 * Free the string and point to the right string
		 */
		FreeString(leftStrID,p);
		SetMVal(&p->mxg,rightStrID);	
	}
	/*
	 * If right string is empty
	 */
	else if (right == 0 || *right == 0)
	{
		/*
		 * Free the right string and point to the left string
		 */
		FreeString(rightStrID,p);
		SetMVal(&p->mxg,leftStrID);
	}
	/*
	 * If left and right string are not empty
	 */
	else
	{
		/*
		 * Copy to a temporary place, Concatenate the string and free
		 * up the two strings and save out the new string
		 */
		lstrcpy(temp, left);
		strcat(temp, right);
		FreeString(leftStrID,p);
		FreeString(rightStrID,p);
		SetMVal(&p->mxg,SaveString(temp,p));
	}
} /* end of ConcatStringTokens */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ArithOperation() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : ArithOperation()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: NOTHING passed in.

 RETURN       :
 * On Exit: Sets the master value to the index in the tokenString array

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Takes the current TVal(0) and TVal(1) and the global 'operation' 
 *				variable and preforms the appropriate athrimetic operation.
 *			Saves the string in the tokenString array and Sets the master value
 *				to the index in the array.
 *			The TVal() function (Token Value), returns the length of a variable
 *				string, or the character if the token is a character.  This value
 *				is usually set by the parser, but can be changed by the program.

 END_MANUAL_ENTRY
****************************************************************************/
	
void ArithOperation (PSPACE *p)
{
	long	result;
	int	leftStrID;
	int	rightStrID;

	char	*left;
	char	*right;
	char	temp[20];	/* must be big enough for a long converted to a string */

	/*
	 * If no operation to preform, return
	 */
	if (p->operation == 0)
	{
		return;
	}

	/*
	 * Get the first token (left) & second token (right) stringID
	 * and get their index into the tokenString array
	 */
	leftStrID = TVal(&p->mxg,0);
	rightStrID = TVal(&p->mxg,1);
	left = p->tokenString[leftStrID];
	right = p->tokenString[rightStrID];

	/*
	 * Now do the operation
	 */
	switch (p->operation)				
	{
	case '+':
		result = strtol(left, NULL, 10) + strtol(right, NULL, 10);
		break;

	case '-':
		result = strtol(left, NULL, 10) - strtol(right, NULL, 10);
		break;
	
	case '/':
		result = strtol(right, NULL, 10);
		if (result == 0)
		{
			ScriptError(DIVIDE_BY_ZERO,p);
		}
		else
		{
		 	result = strtol(left, NULL, 10) / result;
		}
		break;

	case '*':
		result = strtol(left, NULL, 10) * strtol(right, NULL, 10);
		break;
	
	case '%':
	 	result = strtol(right, NULL, 10);
	 	if (result == 0)
		{
			ScriptError(DIVIDE_BY_ZERO,p);
		}
		else
		{
			result = strtol(left, NULL, 10) % result;
		}
		break;
	}
	/*
	 * Free up the strings used, convert the result to an ascii string
	 * and save it away into the tokenString array
	 */
	FreeString(leftStrID,p);
	FreeString(rightStrID,p);
	ltoa(result, temp, 10);
	SetMVal(&p->mxg,SaveString(temp,p));
} /* end of ArithOperation */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ShiftOperation() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : ShiftOperation()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: NOTHING passed in.

 RETURN       : 
 * On Exit: Shortens the string by either copying over it or placing the 
 *				NULL character closer to the begining

 DESCRIPTION  : 

 ALGORITHM    : 
 *			Takes TVal(1) as the distance to shift 
 *			Takes TVal(0) as an index into the token string array, which 
 *				points to the string to shift.
 *			Makes sure we can do the shift, and then does the appropriate shift.
 *				This performs a Decimal shift.  For example, the number 3456 
 *			shifted left twice becomes 56.  The same number shifted right twice
 *			becomes 34.

 END_MANUAL_ENTRY
****************************************************************************/
	
void ShiftOperation (PSPACE *p)
{
	int	i;
	int	shiftDistance;

	char	*str;

	/*
	 * Get the positive number to shift
	 */
	shiftDistance = TVal(&p->mxg,1);

	/*
	 * Get the token string pointer for TVal(0)
	 */
	str = p->tokenString[TVal(&p->mxg,0)];

	/*
	 * Get the length of the string
	 */
	i = lstrlen(str);				

	/*
	 * If we want to shift more than the length of the string, Error
	 */
	if (shiftDistance > i)		
	{
		/*
		 * Set String to 0
		 */
		if (str != 0)
		{
			str[0] = 0;
		}
	}
	else if (p->operation == '<')					
	{
		/*
		 * Make the copy
		 */
		lstrcpy(str, str + shiftDistance);
	}
	else												
	{
		/*
		 * Move the end closer
		 */
		if (str != 0)
		{
			str[i - shiftDistance] = 0;
		}
	}
} /* end of ShiftOperation */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( ParseLine() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : ParseLine()

 PARAMETERS   :   -> input          <-output 
 *	On Entry: Parse flag, this is the flag to tell whether we are parsing
 *				the command line, login script, or map command.	 And the line 
 *				to parse.

 RETURN       :

 DESCRIPTION  : 

 ALGORITHM    : 
			Copy the parse flag and the parse line to 'pLine'.
			Save the line into lowerCommandLine.
			Set the parse address to 'pLine'.  Then parse the line.
			After we have parsed the line free up and NULL out the tokenString
			array.
			The SetParseAddress() function sets the parse up to know where the
			line is that it needs to parse.

 END_MANUAL_ENTRY
****************************************************************************/
	
int ParseLine (PARSEFLAG flag, char *line, PSPACE *p)
{
	char	pLine[MAX_STRING + 1];
	int 	ccode;
	int	i;

	pLine[0] = (char)flag;
	lstrcpy(pLine + 1, line);

	if ((p->lowerCommandLine = strdup(pLine)) == NULL)
	{
		ScriptError(MEMORY_ERROR,p);
		return(-1);
	}

	SetParseAddress(&p->mxg,pLine);

	ccode = Parse(&p->mxg,p);

	for (i = 1; i < sizeof(p->tokenString)/sizeof(p->tokenString[0]); i++)
	{
		if (p->tokenString[i] != p->emptystring)
		{
			free(p->tokenString[i]);
			p->tokenString[i] = p->emptystring;
		}
	}

	/*
	 * Free the work line and NULL the pointer
	 */
	free(p->lowerCommandLine);
	p->lowerCommandLine = NULL;

	return(ccode);
} /* end of ParseLine */


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( CheckNDSAttribute() )

 MODULE       : msupport.c

 GLOBALS USED : 


 SYNTAX       : CheckNDSAttribute()

 PARAMETERS   :   -> input          <-output 

 RETURN       : A 0 if successful


 DESCRIPTION  : Checks to see if an attribute exists and if it does reads
 						it from the database and puts it into the tokenString 
						array (see msupport.c)

 ALGORITHM    : Find the length of the full attribute name
 						(so that we can tell the parser how much to skip)
					  Then separate the attribute from the RDN.
					  Read the attribute from the database.
					  Do necessary conversions according to the syntax ID.
					  Tell the parser how much to skip
					  Save the attribute just read into the tokenString array

 END_MANUAL_ENTRY
****************************************************************************/
int CheckNDSAttribute(PSPACE *p)
{
	NWDSContextHandle	context;						/* Current context				*/

	char	user[MAX_DN_BYTES];
	char	completeName[MAX_STRING];
	char	fullBuffer[MAX_STRING];
	char	*attributeName;
	char	*ptr;
	char	*attrName = 0;
	char	*NoValue = {"N"};
	char	*YesValue = {"Y"};

	int	ccode = -1;
	int	size;
	int	i;

	unsigned short	isvolume;

	uint32 syntaxID;

	context = ERR_CONTEXT_CREATION;
	size = TVal(&p->mxg,0) > MAX_STRING ? MAX_STRING : TVal(&p->mxg,0);		
	CopyMemory(fullBuffer, TChP(&p->mxg,0), size);
	if ((context = NWDSCreateContext()) == (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		ScriptError(CANNOT_CREATE_CONTEXT,p);
		return(-1);
	}


	/*
	 * Null terminate buffer
	 */
	fullBuffer[size] = 0;
	attributeName = fullBuffer;
	
	/*
	 * DS does not like the _ character, so remove it
	 */
	while (*attributeName)
	{
		if (*attributeName == '_')
		{
			*attributeName = ' ';
		}
		attributeName++;
	}
	
	attributeName = fullBuffer;

	/*
	 * Find the size so that we can tell the parser to skip this token
	 */
	if (*attributeName == '"')
	{
	 	attributeName = NWNEXTCHAR(attributeName);
		/*
		 * find trailing quote
		 */
		if ((ptr = NWLSTRCHR(attributeName, '"')) != NULL)
		{
			*ptr = 0;
		}
		else
		{
			goto Error0;
		}
		/*
		 * add one for the NULL
		 */
		size = lstrlen(fullBuffer) + 1;
	}
	else 
	{	
		/*
		 * find end of string
		 */
		if ((ptr = NWLSTRCHR(attributeName, '"')) != NULL)
		{
			*ptr = 0;
			size = lstrlen(attributeName);
		}
		/*
		 * if ptr is NULL then the size is correct
		 */
	}

	/*
	 * Now only look for a DS name if we are logged into DS
	 */
	if (NWIsDSAuthenticated())
	{
		/*
		 * Now seperate the attribute from the RDN
		 */
		completeName[0] = 0;
		if (NWCharVal(attributeName) == '.')
		{
			strcat(completeName,".");
	 		attributeName = NWNEXTCHAR(attributeName);
		}

		if ((ptr = NWLSTRCHR(attributeName, '.')) != NULL)
		{
			*ptr = 0;
			strcat(completeName, ptr+1);
		}


		/*
		 * read the DS attribute value and save it according to the syntax
		 */
		NWDSWhoAmI(context,user);
		if ((ccode = ReadAttribute(user, attributeName, (void **)&attrName, &syntaxID))
			== 0)
		{
			ptr = attrName;
			switch(syntaxID)
			{
				case SYN_CE_STRING:
				case SYN_CI_STRING:
				case SYN_CLASS_NAME:
				case SYN_DIST_NAME:
				case SYN_NU_STRING:
				case SYN_PR_STRING:
				case SYN_TEL_NUMBER:
					break;

				case SYN_FAX_NUMBER:
					Fax_Number_T *p;
					p = (Fax_Number_T *)ptr;
					ptr = p->telephoneNumber;
					break;

				case SYN_BOOLEAN:
					if (*ptr)
					{
						ptr = YesValue;
					}
					else
					{
						ptr = NoValue;
					}
					break;

				case SYN_COUNTER:
				case SYN_INTEGER:
				case SYN_INTERVAL:
					long	*lp;
					lp = (long *)ptr;
					ltoa(*lp, fullBuffer,10);
					ptr = fullBuffer;
					break;

				case SYN_CI_LIST:
					ptr = ((CI_List_T *)ptr)->s;
					break;

				case SYN_EMAIL_ADDRESS:
					ptr = ((EMail_Address_T *)ptr)->address;
					break;

				case SYN_PO_ADDRESS:
					{
						char	**p1;

						p1 = (char **)ptr;
						*fullBuffer = 0;

						for (i = 0; i< 6; i++)
						{
							if (*p1[i])
							{
								wsprintf(&fullBuffer[lstrlen(fullBuffer)],
									"%s\n", p1[i]);
							}
						}
						ptr = fullBuffer;
						break;
					}

				case SYN_PATH:
					{
						char	server[64];
						char	volume[64];
						char	dir[128];
						WORD	connectionID = 0;
		
						/*
						 * build a buffer that contains the complete volume object
						 * name and path
						 */
						lstrcpy(completeName,((Path_T *)ptr)->volumeName);
						strcat(completeName,":");
						NWDSParsePath(context, completeName, server,
							&connectionID, volume, dir,&isvolume);
						wsprintf(completeName,"%s/%s:%s",server,volume,
							((Path_T *)ptr)->path);
						ptr = completeName;
						break;
					}

				default:
					ccode = ERR_BAD_SYNTAX;
					free(attrName);
					goto Error1;
			}
			SkipVariableToken(&p->mxg,size);
			SetMVal(&p->mxg,SaveString(ptr,p));
		}
	}
	if (attrName)
	{
		free(attrName);
	}

	/*
	 * DS Read failed or DS not  loaded
	 */
	if (ccode)
	{
		if ((ptr = GetEnvironmentString(NWLSTRUPR(attributeName))) != NULL)
		{
			/*
			 * we found a value
			 */
			SkipVariableToken(&p->mxg,size);
			SetMVal(&p->mxg,SaveString(ptr,p));
			ccode = 0;
		}
	}
						 
	/*
	 * If successful ccode will equal 0. So fall through
	 */
Error1:
	if (context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		NWDSFreeContext(context);
	}
	return(ccode);
Error0:
	/*
	 * could not find ending " or '
	 */
	if (context != (NWDSContextHandle)ERR_CONTEXT_CREATION)
	{
		NWDSFreeContext(context);
	}
	return(-1);

} /* end of CheckNDSAttribute */


/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( AvailableDrive() )

 MODULE       : mapcmd.c

 GLOBALS USED : 


 SYNTAX       : AvailableDrive()

 PARAMETERS   :   -> input          <-output 
					int relativeDriveNumber	A relative network drive number.
													(e.g. *5:=server\vol:  -The 5 is the
 													relative network drive number)

 RETURN       :Returns a drive or a 0 if there are not enough network drives.

 DESCRIPTION  :Goes through all drive (A-Z) looking for network drives.

 ALGORITHM    :1) find out what is the first and last network drives
					2) Loop through the drives. When it finds a network drive,
					it decrements the relative DriveNumber variable.  When the
					relativeDriveVariable is less then or equal to 1 map has found
					the Nth network drive that is available.

					For example, if 5 was the relative drive number, and F was
					the first network drive, G the second, H the third, I the
					fourth, J the fifth; this function would return the drive
					letter J.


 END_MANUAL_ENTRY
****************************************************************************/

int AvailableDrive(int relativeDriveNumber)
{
NWCONN_HANDLE		connectionID;
WORD	ccode,
		driveStatus;
int 	driveNumber = 1,
		lastDrive = 26;

	/* this code is only for vlms. We have to loop through and get the drive
		status for each drive.
	*/
	for (; driveNumber <= lastDrive; driveNumber++)	/* go through all drives */
	{
		ccode = NWGetDriveStatus(driveNumber, NW_FORMAT_NETWARE,&driveStatus,
			&connectionID,NULL,NULL,NULL);
		if (ccode ==0 && !(driveStatus & NW_LOCAL_DRIVE) &&
			  !(driveStatus & NW_PNW_DRIVE) )
		{
			if (relativeDriveNumber-- <= 1)				/* Decrement relativeDriveNumber, When it hits 1  */
				return(driveNumber + 'A' - 1);			/*   then return the drive */
		}
	}
	return(0);	/* Relative drive is greater then then available network drives */
} /* end of AvailableDrive */

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DayOfYear() )

 MODULE       : common.c

 GLOBALS USED : 


 SYNTAX       : DayOfYear(arg1, arg2)

 PARAMETERS   :   -> input          <-output 
              -> arg1 
              <- arg2 


 RETURN       : Returns the number of the days up to the specified day, month,
 						year

 DESCRIPTION  : Returns the number of days up to the specifed day, month and  
 						year passed in.

 ALGORITHM    : if leap year use table 2 (see below)
 					  else use table 1 (see below)
					  Add up the days in each month until we hit the current month
					  return the total

table 1		{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
table 2		{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}


 END_MANUAL_ENTRY
****************************************************************************/
int DayOfYear(int year, int month, int day)
{
int i, leap;
static int dayTable[2][13] =
{
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	for (i=1; i<month; i++)
		day += dayTable[leap][i];
	return (day);
}

/****************************************************************************
 лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
 BEGIN_MANUAL_ENTRY ( DaysUntilPasswordExpires() )

 MODULE       : common.c

 GLOBALS USED : 


 SYNTAX       : DaysUntilPasswordExpires(arg1, arg2)

 PARAMETERS   :   -> input          <-output 


 RETURN       : The number of days until the password expires.


 DESCRIPTION  : This function calls the bindery and figures how many days
 						are left before a users password expires.

 ALGORITHM    : Read the bindery
 						Figure how many days between now and the day the password
						expires.
					  If it is a negative number return 0
					  Else return the days left.

 END_MANUAL_ENTRY
****************************************************************************/
int  DaysUntilPasswordExpires(PSPACE *p)
{
long	ltime;
NWCCODE ccode;
char	*attrName = 0;
uint32	syntaxID;
BYTE	data[128], mseg;
struct tm *Today;
struct tm *expDay;
int	year = 0, leap, expiredDay, numberOfDaysLeft = 0;
Restrictions *restrictions;


	/* Get the expiration date */

	if(p->NDSorBindery == NDS)
	{
		if((ccode = ReadAttribute(p->scriptuserName,A_PASSWORD_EXPIRATION_TIME,
			(void **)&attrName, &syntaxID)) == (WORD)NULL)
		{

			NWCONN_HANDLE      myTempNWDSConnID;
			time_t    serverTimeUTCTemp,
			          workStationTime;
			char      *isTZSet;



			 /*
			 * tzset() will set global variable timezone according to the value of
			 * the environment variable tz.  Microsoft assumes, if tz is not set,
			 * tz=PST8PDT, which sets timezone to (8 * SECONDS_IN_AN_HOUR).  All
			 * calls to localtime() and mktime() take into account timezone's value.
			 * Windows library assumes tz=EST5EDT.  Need to check if tz is set.  If
			 * tz is not set, follow these steps: in SLC should be MST7MDT.
			 *
			 * 1. oldTimezone = timezone;
			 * 2. timezone = 0;
			 * 3. get serverUTCTime
			 * 4. get workstationTime
			 * 5. timezone = serverUTCTime - workstationTime;
			 * 6. make localtime() and mktime() calls, etc.
			 * 7. timezone = oldTimezone;
			 */

			tzset();                         /* reads the environment var TZ */
			isTZSet = GetEnvironmentString("TZ");
			if (isTZSet == NULL)
			{
				/* The connection number giving us NWDS information should also be
				* used to get the UTC time.
				* myTempNWDSConnID is a global that will be used to get the time.*/

				timezone = 0;	/* Get rid of effects of MSC global TZ=PST8PDT */
				if ((ccode = NWGetNearestDirectoryService(&myTempNWDSConnID)) != 0)
				    goto Error1;
         
			   /* Do the next two calls in row, then check for errors.*/
			   if ((ccode = NWGetFileServerUTCTime(myTempNWDSConnID, 
				         (uint32*) &serverTimeUTCTemp)) != 0)
	            goto Error1;

			   time(&workStationTime);
			   timezone = (serverTimeUTCTemp - workStationTime);
				/*
					In case of errors we don't have an alternative, though
					we may want to add a message in this case post 4.0
				*/
Error1:;

      	} /* end if TZ==NULL */


			expDay = localtime((time_t *) attrName);
			free(attrName);

			restrictions = (Restrictions *) data;

			restrictions->passwordExpiresYear = (BYTE)expDay->tm_year;
			restrictions->passwordExpiresMonth = (BYTE)(expDay->tm_mon+1);
			restrictions->passwordExpiresDay = (BYTE)expDay->tm_mday;
			
			goto CALC;
		}
		else if (ccode == (NWCCODE)ERR_NO_SUCH_VALUE ||
			ccode == (NWCCODE)ERR_NO_SUCH_ATTRIBUTE)
		{
			return(32767);
		}
	}
	else
	{

		if ((ccode = NWReadPropertyValue(p->scriptconnID,
				p->scriptuserName, OT_USER,
				"LOGIN_CONTROL", 1, data, &mseg, NULL)) != 0)
		{
			ScriptError(SCRIPT_PROPERTY_ERROR,p,"LOGIN_CONTROL",p->scriptuserName);
			return( 32767 );
		}
		restrictions = (Restrictions *)data;

CALC:;
		/* Get Today's date */
		time(&ltime);
		Today = gmtime(&ltime);
		year = (Today->tm_year + CENTURY);

		/* Get the number of the day of the year that the password expires */
		expiredDay = DayOfYear(restrictions->passwordExpiresYear,
										restrictions->passwordExpiresMonth,
										restrictions->passwordExpiresDay);

		/*
			if the expired day is 0 or below, assume there is no expiration date.
		*/
		if ( expiredDay <= 0 )
			return( 32767 );

		/*
			If the current year and the expired year are the same, simply subtract
			the the current day of the year from the expired day of the year.  If
			not equal, add the number of days in each year except the expired year,
			subtract the day of the year for the current year and add the number
			of the day of the year for the expired year to get the number of days
			left.
		*/
		if (Today->tm_year == (char)restrictions->passwordExpiresYear)
			numberOfDaysLeft = expiredDay - (Today->tm_yday +1);
		else
		{
			do
			{
				leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
				if (leap)
					numberOfDaysLeft += 366;
				else
					numberOfDaysLeft += 365;
				year++;
			}	while (year < (restrictions->passwordExpiresYear + CENTURY));

			/* 
				Add the day of the year in the expired year and subtract the
				day of the current year from the number of days left.
			*/
			numberOfDaysLeft += (expiredDay - (Today->tm_yday + 1));
		}

		/* If the Number of days is negative, return a zero. Otherwise, return the
			actual value.
		*/
		return ((numberOfDaysLeft > 0) ? numberOfDaysLeft: 0);
	}
	return(0);
}
	
int IsAccessServerPresent (void)
{
	return(0);
}
