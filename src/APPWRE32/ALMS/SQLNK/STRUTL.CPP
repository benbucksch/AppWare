/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: STRUTL.CPP                                              |
 | DESCRIPTION	: String Utilities.                                       |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "strutl.h"
#include "safemem.h"


void StrUtl::CRCount		/* ---- Carriage Return Count ----------- */
( const char	*s,			// string
  long		*crCount,		// number of carriage-returns
  long		*charCount		// number of characters
)
/* Counts the number of carriage-returns, and the number of characters in
   the string.
*/
{ const char	*p;			// pointer

  p = s;
  *crCount = 0;
  while (*s != '\0')
  { if (*s == '\r')
      (*crCount)++;
    s++;
  } // while
  *charCount = s - p;
} // CRCount

char *StrUtl::FirstInvalidVarChar /* -- First Invalid Variable character  */
( const char	*s			// string
)
/* Returns a pointer to the first invalid variable character, or
   returns 0.
*/
{
  while (*s != '\0')
    if (IsVarChar(*s))
      s++;
    else
      return (char *) s;
  return 0;
} // FirstInvalidVarChar
	
char *StrUtl::FirstNewLine	/* ---- First New Line ------------------ */
( const char	*s			// string
)
/* Returns a pointer to the first new-line, or returns 0. */
{
  while (*s != '\0')
    if (*s == '\r' && *(s + 1) == '\n')
      return (char *) s;
    else
      s++;
  return 0;
} // FirstNewLine


BOOL StrUtl::IsVarChar		/* ---- Is Variable Character? ---------- */
( unsigned char	c               	// character
)
/* Returns TRUE if the char is a variable character, else return FALSE. */
{
  return !(
    c == ' '  || c == '\t' || c == '\r' || c == '\n' ||	// white spaces
    c == ':'  || c == ','  || c == '('  || c == ')'  ||	// rev characters
    c == '+'  || c == '-'  || c == '*'  || c == '/'  ||	// operators
    c == '%'  || c == '='  || c == '<'  || c == '>');    
} // IsVarChar

BOOL StrUtl::IsWhiteSpace       /* ---- Is White Space? ----------------- */
( unsigned char	c               	// character
)
/* Returns TRUE if the character is white-space, else return FALSE. */
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
} // IsWhiteSpace


long StrUtl::CRCount		/* ---- Carriage Return Count ----------- */
( const char	*s			// string
)
/* Returns the number of carriage-returns in the string. */
{ long		i;			// counter

  i = 0;
  while (*s != '\0')
  { if (*s == '\r')
      i++;
    s++;
  } // while
  return i;
} // CRCount

long StrUtl::CRToNewLine	/* ---- Carriage Return To New Line ----- */
( char		*s			// string
)
/* Returns the number of carriage-return replaced in the string. */
{ char		*p;			// pointer
  long		crCount;		// number of carriage-returns
  long		charCount;		// number of characters
  long		nlCount;		// number of new-line

  CRCount(s, &crCount, &charCount);
  if (!crCount)
    return 0;				// nothing to repalace

  nlCount = crCount;
  s += charCount;			// points to end of string
  p = s + crCount;
  *p-- = *s--;				// copies 0 terminated string
  while (crCount)
    if (*s == '\n')
    { *p-- = *s--;			// copies '\n'
      *p-- = '\r';			// inserts '\r'
       
      crCount--;
    } // if
    else
      *p-- = *s--;			// copies character
  return nlCount;
} // CRToNewLine

void StrUtl::NewLineToCR	/* ---- New Line To Carriage Return ----- */
( char		*s			// string
)
/* Replaces new-line with carriage-return in the string. */
{ char		*p;			// pointer

  p = s = FirstNewLine(s);
  if (p == 0)
    return;				// nothing to replace

  while (*p != '\0')
  { if (*p == '\r' && *(p + 1) == '\n')	// is new-line
      p++;				// trims '\r'
    *s++ = *p++;			// copies character
  } // while  		      
  *s = '\0';				// trims trailing characters
} // NewLineToCR


void StrUtl::LTrim		/* ---- Left Trim ----------------------- */
( char		*s			// string
)
/* Trims leading white spaces in the string. */
{ char		*p;			// pointer

  if (!IsWhiteSpace(*s))
    return;				// nothing to trim

  p = s;
  /* trims at lease 1 white space */
  while (IsWhiteSpace(*++s))
    /* Do Nothing */;
  lstrcpy(p, s);
} // LTrim

void StrUtl::LRTrim		/* ---- Left and Right Trim ------------- */
( char		*s			// string
)
/* Trims leading, and trailing white spaces in the string. */
{
  RTrim(s);
  LTrim(s);
} // Trim

void StrUtl::RTrim		/* ---- Right Trim ---------------------- */
( char		*s			// string
)
/* Trims trailing white spaces in the string. */
{ char		*p;			// pointer

  p = s + lstrlen(s);
  if (p == s)
    return;				// nothing to trim
  p--;

  if (!IsWhiteSpace(*p))
    return;				// nothing to trim
  p--;

  /* trims at least 1 white space */
  while (p >= s && IsWhiteSpace(*p))
    p--;
  *++p = '\0';
} // Rtrim

void StrUtl::TrimVar		/* ---- Trim Variable ------------------- */
( char		*s			// string
)
/* Trims invalid characters in the string. */
{ char		*p;			// pointer

  p = s = FirstInvalidVarChar(s);
  if (p == 0)
    return;				// nothing to trim

  while (*p != '\0')
    if (IsVarChar(*p))			// is variable character		    
      *s++ = *p++;			// copies character						     
    else
      p++;				// trims character
  *s = '\0';				// trims trailing characters
} // TrimVar
