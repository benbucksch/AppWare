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
 | FILE		: MACROTRN.H                                              |
 | DESCRIPTION	: Database Macro Translator Include File.                 |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	08-Oct-1992  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "datb_fnc.h"
#include "macro.h"
#include "strutl.h"
#include "memblock.h"


#define MACROCODEBLOCK	8		// macro code block

class MacroTrn			/* ---- Macro Translator ---------------- */
{ AOBJECTID	obj;			// datb object
  WORD		fieldCount;		// number of fields
  Field		*field;			// an array of fields
  char		*inst;			// macro instruction
  char		*errMsg;		// error message			
  WORD		codeCount;		// number of macro code
  Macro		*code;			// an array of macro code
  Macro		*codePtr;		// code pointer

  StrUtl	str;			// strutl class
  MemBlock	key;			// memblock class

  void ErrMsg(const char *);
  /* Builds error message. */


  BOOL ConvertDate(char *);
  /* Returns TRUE or FALSE, as the result of converting date in
     string format to date in long format.
  */

  BOOL ConvertLong(char *);
  /* Returns TRUE or FALSE, as the result of converting long in
     string format to long format.
  */

  BOOL ConvertLongDouble(char *);
  /* Returns TRUE or FALSE, as the result of converting long double in
     string format to long double format.
  */

  BOOL ConvertTime(char *);
  /* Returns TRUE or FALSE, as the result of converting time in
     string format to time in long format.
  */


  BOOL SubStr(const char *, const char *);
  /* Returns TRUE if target string is subset of source string, else
     returns FALSE.
  */


  BOOL AssignFieldID(Macro *);
  /* Returns TRUE or FALSE as the result of assigning field id. */

  BOOL AssignKeyAttr(Macro *, Field);
  /* Returns TRUE or FALSE as the result of assigning the key attributes to
     the macro code.
  */

  BOOL Code(void);
  /* Code	: Code b* Statement | b* '\0' */

  int Command(void);
  /* Command	: (EXCLUDE | EXCLUDEX | FIND | FINDX | MERGE | MERGEX |
		   REQUIRE | REQUIREX) b
  */

  BOOL IsEndOfStr(void);
  /* IsEndOfStr	: b* eos */

  BOOL KeyVal1(Macro *);
  /* KeyVal1	: .* Range | .* RightParen */

  BOOL KeyVal2(Macro *);
  /* KeyVal2	: .* RightParen */

  BOOL LeftParen(void);
  /* LeftParen	: '(' */

  BOOL Range(void);
  /* Range	: '..' */

  BOOL RightParen(void);
  /* RightParen	: ')' */

  BOOL Statement(void);
  /* Statement	: Command b* Identifier b* LeftParen KeyVal1 RightParen
		| Command b* Identifier b* LeftParen KeyVal1 Range KeyVal2
		                                             RightParen 
  */

public:
  BOOL Translate(AOBJECTID, char *, char *, WORD *, Macro **);
  /* Translates instructions script into macro codes. */
};
