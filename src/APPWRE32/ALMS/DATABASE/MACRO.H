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
 | FILE		: MACRO.H                                                 |
 | DESCRIPTION	: Database Macro Code Include File.                       |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef MACRO_H
#define MACRO_H

#define STRICT
#include <windows.h>


/* ---- macro code operators ---- */
#define NOCOMMAND	0		// has to be 0
#define REQUIRE		1		// has to be 1
#define REQUIREX	2
#define MERGE		3
#define MERGEX		4
#define FIND		5
#define FINDX		6
#define EXCLUDE		7
#define EXCLUDEX	8

struct Macro			/* ---- Macro Code ---------------------- */
{ int		oprtr;			// operator (macro code operator)
  int		field;			// field identifier (1-origin) 
  BOOL		isIndex;		// is index? 
  short		keyType;		// key type 
  short		keySize;		// key size 
  BOOL		isRange;		// is range? 
  char		*keyVal1;		// key value 1 (oprtr != NOCOMMAND) 
  char		*keyVal2;		// key value 2 (isRange = TRUE) 
};

#endif // MACRO_H 
