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
 | FILE		: SLEX.H                                                  |
 | DESCRIPTION	: Simple Lexical Analyser Include File.                   |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	24-May-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#ifndef SLEX_H
#define SLEX_H

class Slex			/* ---- Simple Lexical Analyser --------- */
{ char		*textPtr;		// text pointer
  char		textChar;		// text character
  char		*tokenPtr;		// token pointer

  char *InitToken(void);
  /* Inits token. */

  char *MakeToken(void);
  /* Makes token. */

  void RollbackToken(char *);
  /* Rollbacks token. */

public:
  void InitSlex(char *str);	       
  /* Inits slex. */


  char *Blanks(void);
  /* Syntax	: '' | ' ' | '\n' | '\r' | '\t'
  */

  char *Colon(void);
  /* Syntax	: ':'
  */

  char *OpenBraces(void);
  /* Syntax	: '{'
  */

  char *CloseBraces(void);
  /* Syntax	: '}'
  */

  char *OpenBracket(void);
  /* Syntax	: '['
  */

  char *CloseBracket(void);
  /* Syntax	: ']'
  */

  char *Text(void);
  /* Syntax	: [^'.']*
  */


  char *GetToken(void);
  /* Returns the current token. */
};

#endif /* SLEX_H */
