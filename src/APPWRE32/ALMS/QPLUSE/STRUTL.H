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
 | FILE		: STRUTL.H                                                |
 | DESCRIPTION	: String Utilities Include File.                          |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#ifndef STRUTL_H
#define STRUTL_H

#define STRICT
#include <windows.h>


class StrUtl			/* ---- String Utility ------------------ */
{
  void CRCount(const char *s, long *crCount, long *charCount);
  /* Counts the number of carriage-returns, and the number of characters in
     the string.
  */

  char *FirstInvalidVarChar(const char *s);
  /* Returns a pointer to the first invalid variable character, or
     returns 0.
  */

  char *FirstNewLine(const char *s);
  /* Returns a pointer to the first new-line, or returns 0. */

protected:
  BOOL IsVarChar(unsigned char c);
  /* Returns TRUE if the char is a variable character, else return FALSE. */

  BOOL IsWhiteSpace(unsigned char c);
  /* Returns TRUE if the character is white-space, else return FALSE. */

public:
  long CRCount(const char *s);
  /* Returns the number of carriage-returns in the string. */

  long CRToNewLine(char *s);
  /* Returns the number of carriage-return replaced in the string. */

  void NewLineToCR(char *s);
  /* Replaces new-line with carriage-return in the string. */


  void LTrim(char *s);
  /* Trims leading white-spaces in the string. */

  void LRTrim(char *s);
  /* Trims leading, and trailing white-spaces in the string. */

  void RTrim(char *s);
  /* Trims trailing white-spaces in the string. */

  void TrimVar(char *s);
  /* Trims invalid variable characters in the string. */
};

#endif /* STRUTL_H */
