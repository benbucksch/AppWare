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
 | FILE		: SLEX.CPP                                                |
 | DESCRIPTION	: Simple Lexical Analyser.                                |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	24-May-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "slex.h"


char *Slex::InitToken		/* ---- Init Token ---------------------- */
( void
)
/* Inits token. */
{
  *textPtr = textChar;			// restores the character
  return tokenPtr = (char *) textPtr;
} // InitToken

char *Slex::MakeToken		/* ---- Make Token ---------------------- */
( void
)
/* Makes token. */
{
  textChar = *textPtr;			// stores the character
  *textPtr = '\0';
  return tokenPtr;
} // MakeToken

void Slex::RollbackToken	/* ---- Rollback Token ------------------ */
( char		*ptr			// pointer
)
/* Rollbacks token. */
{
  textPtr = ptr;
  tokenPtr = ptr;
} // RollbackToken


void Slex::InitSlex		/* ---- Init Slex ----------------------- */
( char		*str			// string
)
/* Inits slex. */
{
  textPtr = tokenPtr = str;
  textChar = *textPtr;
} // InitSlex


char *Slex::Blanks		/* ---- Blanks -------------------------- */
( void
)
/* Syntax	: '' | ' ' | '\n' | '\r' | '\t'
*/
{
  InitToken();

  while (*textPtr == ' '  || *textPtr == '\n' ||
         *textPtr == '\r' || *textPtr == '\t')
    textPtr++;
  return MakeToken();
} // Blanks

char *Slex::Colon		/* ---- Colon --------------------------- */
( void
)
/* Syntax	: ':'
*/
{
  InitToken();

  if (*textPtr == ':')
  { textPtr++;
    return MakeToken();
  } // if
  return 0;
} // Colon

char *Slex::OpenBraces		/* ---- Open Braces --------------------- */
( void
)
/* Syntax	: '{'
*/
{
  InitToken();

  if (*textPtr == '{')
  { textPtr++;
    return MakeToken();
  } // if
  return 0;
} // OpenBraces

char *Slex::CloseBraces		/* ---- Close Braces -------------------- */
( void
)
/* Syntax	: '}'
*/
{
  InitToken();

  if (*textPtr == '}')
  { textPtr++;
    return MakeToken();
  } // if
  return 0;
} // CloseBraces

char *Slex::OpenBracket		/* ---- Open Bracket -------------------- */
( void
)
/* Syntax	: '['
*/
{
  InitToken();

  if (*textPtr == '[')
  { textPtr++;
    return MakeToken();
  } // if
  return 0;
} // OpenBracket

char *Slex::CloseBracket	/* ---- Close Bracket ------------------- */
( void
)
/* Syntax	: ']'
*/
{
  InitToken();

  if (*textPtr == ']')
  { textPtr++;
    return MakeToken();
  } // if
  return 0;
} // CloseBracket

char *Slex::Text		/* ---- Text ---------------------------- */
( void
)
/* Syntax	: [^':']*
*/
{
  InitToken();

  while (1)				// infinite loop
    switch (*textPtr)
    { case '\0'	:
      case ':'	:
	return MakeToken();
      default	:
	textPtr++;
    } // switch
} // Text


char *Slex::GetToken		/* ---- Get Token ----------------------- */
( void
)
/* Returns the current token. */
{
  return tokenPtr;
} // GetToken
