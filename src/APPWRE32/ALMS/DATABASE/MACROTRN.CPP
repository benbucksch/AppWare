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
 | FILE		: MACROTRN.CPP                                            |
 | DESCRIPTION	: Database Macro Translator.                              |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#define STRICT
#include <stdlib.h>
#include <ctype.h>

#include "macrotrn.h"
#include "dbkey.h"
#include "safemem.h"


void MacroTrn::ErrMsg           /* ---- Error Message ------------------- */
( const char	*msg			// message
)
/* Builds error messages. */
{ long		errMsgLen;		// error message length
  char		*errMsgPtr;		// error message pointer
  long		instLen;		// instruction length

  lstrcpy(errMsg, "Database\r\nMacroSearch: ");
  lstrcat(errMsg, msg);
  errMsgLen = lstrlen(errMsg);
  errMsgPtr = errMsg + errMsgLen;

  if (IsEndOfStr())
  { lstrcpy(errMsgPtr, " at the end of instruction.");
    return;
  } // if

  instLen = lstrlen(inst);
  if (instLen > MXSTRSIZE - errMsgLen - 8)
    instLen = MXSTRSIZE - errMsgLen - 8;

  lstrcpy(errMsgPtr, " at '");
  errMsgPtr += 5;
  while (instLen--)
    *errMsgPtr++ = *inst++;
  lstrcpy(errMsgPtr, "'.");
} // ErrMsg


BOOL MacroTrn::ConvertDate	/* ---- Convert Date -------------------- */
( char		*dateStr		// date string
)
/* Returns TRUE or FALSE, as the result of converting date in
   string format to date in long format.
*/
{
  *(long *) dateStr = ODateStringToDate(dateStr);
  return TRUE;
} // ConvertDate

BOOL MacroTrn::ConvertLong	/* ---- Convert Long -------------------- */
( char		*longStr		// long string
)
/* Returns TRUE or FALSE, as the result of converting long in
   string format to long format.
*/
{
  *(long *) longStr = atol(longStr);
  return TRUE;
} // ConvertLong

BOOL MacroTrn::ConvertLongDouble /* --- Convert Long Dobule ------------- */
( char		*longDblStr		// long double string
)
/* Returns TRUE or FALSE, as the result of converting long double in
   string format to long double format.
*/
{
///  *(long double *) longDblStr = _atold(longDblStr);
  *(long double *) longDblStr = atof(longDblStr);
  return TRUE;
} // ConvertLongDouble

BOOL MacroTrn::ConvertTime      /* ---- Convert Time -------------------- */
( char		*timeStr		// time value
)
/* Returns TRUE or FALSE, as the result of converting time in
   string format to time in long format.
*/
{
  *(long *) timeStr = OTimeStringToTime(timeStr);
  return TRUE;
} // ConvertTime


BOOL MacroTrn::SubStr		/* ---- SubString ----------------------- */
( const char	*t,			// target string
  const char	*s			// source string
)
/* Returns TRUE if target string is subset of source string, else
   returns FALSE.
*/
{
  while (*t != '\0' && toupper(*t) == toupper(*s))
  { t++;
    s++;
  } // while
  return *t == '\0'? TRUE : FALSE;
} // SubStr


BOOL MacroTrn::AssignFieldID    /* ---- Assign Field Identifier --------- */
( Macro		*code			// macro code
)
/* Returns the index of the field. */
{ Field		*fieldPtr;		// field pointer
  char          *instPtr;		// instruction pointer

  WORD		i;                      // counter

  for (fieldPtr = field, i = 0; i < fieldCount; i++, fieldPtr++)
    if (SubStr(fieldPtr->name, inst))
    { instPtr = inst + lstrlen(fieldPtr->name);
      if (str.IsWhiteSpace(*instPtr) || *instPtr == '(')
      { code->field = i + 1;
	if (!AssignKeyAttr(code, *fieldPtr))
	{ ErrMsg("Non-searchable field identifier");
	  return FALSE;
	} // if
	inst = instPtr;
	return TRUE;
      } // if
    } // if
  ErrMsg("Unknown field identifier");
  return FALSE;
} // AssignFieldID

BOOL MacroTrn::AssignKeyAttr    /* ---- Assign Key Attributes ----------- */
( Macro		*code,			// macro code
  Field		field			// field
)
/* Assigns key attributes to macro code. */
{ DBKey		dbKey;			// dbkey class

  if (field.keyType)
  { /* ---- the field is indexed ---- */
    code->isIndex = TRUE;
    code->keyType = field.keyType;
    code->keySize = field.keySize;
    return TRUE;
  } // if

  /* ---- the field is not indexed ---- */
  code->isIndex = FALSE;
  code->keyType = dbKey.GetKeyType(obj, field.obj, field.type);
  switch (code->keyType)
  { case ODATB_KEYDATE:
    case ODATB_KEYTIME:
      code->keySize = sizeof(long);
      return TRUE;
    case ODATB_KEYLONG:			// converts to long double
    case ODATB_KEYLONG | ODATB_KEYLONGDOUBLE:
      code->keyType = ODATB_KEYLONGDOUBLE;
    case ODATB_KEYLONGDOUBLE:
      code->keySize = sizeof(long double);
      return TRUE;
    case ODATB_KEYTEXT:
      code->keyType = ODATB_KEYTEXT;
      code->keySize = ODATB_MXKEYSIZE;
      return TRUE;
    default:
      return FALSE;			// non search-able field
  } // switch
} // AssignKeyAttr

BOOL MacroTrn::Code             /* ---- Code ---------------------------- */
( void
)
/* Code		: Code Statement b* | b* î. */
{
  while (!IsEndOfStr())
  { /* ---- adds macro code ---- */
    if (codeCount % MACROCODEBLOCK == 0)
      code = (Macro *) MBRealloc(code, (codeCount + MACROCODEBLOCK) *
				       sizeof(Macro));
    codePtr = code + codeCount;

    if (!Statement())
      return FALSE;
    if (codePtr->oprtr == FIND || codePtr->oprtr == FINDX)
    { /* ---- ignores previous macro code ---- */
      *code  = *codePtr;
      codeCount = 1;
    } // if
    else
      codeCount++;
  } // while
  return TRUE;
} // Code

int MacroTrn::Command           /* ---- Command ------------------------- */
( void
)
/* Command	: (EXCLUDE | EXCLUDEX | FIND | FINDX | MERGE | MERGEX |
		   REQUIRE | REQUIREX) b
*/
{
  switch (*inst)
  { case 'E': case 'e':
      if (SubStr("EXCLUDE", inst))
      { if (str.IsWhiteSpace(*(inst + 7)))
	{ inst += 8;
	  return EXCLUDE;
	} // if

	if (SubStr("X", inst + 7))
	  if (str.IsWhiteSpace(*(inst + 8)))
	  { inst += 9;
	    return EXCLUDEX;
	  } // if
      } // if
      break;
    case 'F': case 'f':
      if (SubStr("FIND", inst))
      { if (str.IsWhiteSpace(*(inst + 4)))
	{ inst += 5;
	  return FIND;
	} // if

	if (SubStr("X", inst + 4))
	  if (str.IsWhiteSpace(*(inst + 5)))
	  { inst += 6;
	    return FINDX;
	  } // if
      } // if
      break;
    case 'M': case 'm':
      if (SubStr("MERGE", inst))
      { if (str.IsWhiteSpace(*(inst + 5)))
	{ inst += 6;
	  return MERGE;
	} // if

	if (SubStr("X", inst + 5))
	  if (str.IsWhiteSpace(*(inst + 6)))
	  { inst += 7;
	    return MERGEX;
	  } // if
      } // if
      break;
    case 'R': case 'r':
      if (SubStr("REQUIRE", inst))
      { if (str.IsWhiteSpace(*(inst + 7)))
	{ inst += 8;
	  return REQUIRE;
	} // if

	if (SubStr("X", inst + 7))
	  if (str.IsWhiteSpace(*(inst + 8)))
	  { inst += 9;
	    return REQUIREX;
	  } // if
      } // if	        
      break;
  } // switch
  ErrMsg("Invalid macro command");
  return NOCOMMAND;
} // Command

BOOL MacroTrn::IsEndOfStr       /* ---- Is End Of String? --------------- */
( void
)
/* IsEndOfStr	: space* EOS */
{
  while (str.IsWhiteSpace(*inst))
    inst++;				// ignores white spaces
  return *inst == '\0'? TRUE : FALSE;
} // IsEndOfStr

BOOL MacroTrn::KeyVal1		/* ---- Key Value 1 --------------------- */
( Macro		*code			// key value
)
/* KeyVal1	: .* Range | .* RightParen */
{ char		*instPtr;		// instruction pointer
  WORD		keySize;		// key size
  char		*keyPtr;		// key pointer			

  /* ---- copies key value up to max length of the key ---- */
  for (keySize = 1, instPtr = inst;
       *instPtr != '\0' && *instPtr != ')' &&
       !(*instPtr == '.' && *(instPtr + 1) == '.');
       keySize++, instPtr++)
    /* Do Nothing */;

  keyPtr = code->keyVal1 = (char *) key.Alloc(keySize +
                                              sizeof(long double));

  while (keySize > 1)
  { *keyPtr++ = *inst++;
    keySize--;
  } // while
  *keyPtr = '\0';

  switch (code->keyType)
  { case ODATB_KEYDATE:
      return ConvertDate(code->keyVal1);
    case ODATB_KEYTIME:
      return ConvertTime(code->keyVal1);
    case ODATB_KEYLONG:			
      return ConvertLong(code->keyVal1); 	 
    case ODATB_KEYLONGDOUBLE:
      return ConvertLongDouble(code->keyVal1);
    case ODATB_KEYTEXT:
      return TRUE;
  } // switch
  return FALSE;				// unexpected error
} // KeyVal1

BOOL MacroTrn::KeyVal2          /* ---- Key Value 2 --------------------- */
( Macro		*code			// key value
)
/* KeyVal2      : .* RightParen */
{ char		*instPtr;		// instruction pointer
  WORD		keySize;		// key size
  char		*keyPtr;		// key pointer

  /* ---- copies key value up to max length of the key ---- */
  for (keySize = 1, instPtr = inst;
       *instPtr != '\0' && *instPtr != ')';
       keySize++, instPtr++)
    /* Do Nothing */;

  keyPtr = code->keyVal2 = (char *) key.Alloc(keySize +
                                              sizeof(long double));

  while (keySize > 1)
  { *keyPtr++ = *inst++;
    keySize--;
  } // while
  *keyPtr = '\0';

  switch (code->keyType)
  { case ODATB_KEYDATE:
      return ConvertDate(code->keyVal2);
    case ODATB_KEYTIME:
      return ConvertTime(code->keyVal2);
    case ODATB_KEYLONG:
      return ConvertLong(code->keyVal2); 
    case ODATB_KEYLONGDOUBLE:
      return ConvertLongDouble(code->keyVal2);
    case ODATB_KEYTEXT:
      return TRUE;
  } // switch
  return FALSE;				// unexpected error
} // KeyVal2

BOOL MacroTrn::LeftParen        /* ---- Left Parenthesis ---------------- */
( void
)
/* LeftParen    : '(' */
{
  if (*inst == '(')
  { inst++;
    return TRUE;
  } // if
  ErrMsg("Left parenthesis is needed");
  return FALSE;
} // LeftParen

BOOL MacroTrn::Range            /* ---- Range --------------------------- */
( void
)
/* Range        : '..' */
{
  if (*inst == '.' && *(inst + 1) == '.')
  { inst += 2;
    return TRUE;
  } // if
  return FALSE;
} // Range

BOOL MacroTrn::RightParen       /* ---- Right Paranthesis --------------- */
( void
)
/* RightParen   : ')' */
{
  if (*inst == ')')
  { inst++;
    return TRUE;
  } // if
  ErrMsg("Right parenthesis is needed");
  return FALSE;
} // RightParen

BOOL MacroTrn::Statement	/* ---- Statement ----------------------- */
( void
)
/* Statement	: Command b* Identifier b* LeftParen KeyVal1 RightParen
		| Command b* Identifier b* LeftParen KeyVal1 Range KeyVal2
		                                             RightParen 
*/
{
  if ((codePtr->oprtr = Command()) == NOCOMMAND)
    return FALSE;
  if (IsEndOfStr())
  { ErrMsg("Field identifier and key value(s) are expected");
    return FALSE;
  } // if
  if (!AssignFieldID(codePtr))
    return FALSE;
  if (IsEndOfStr())
  { ErrMsg("Key value(s) are expected");
    return FALSE;
  } // if
  if (!LeftParen())
    return FALSE;
  if (!KeyVal1(codePtr))
    return FALSE;
  codePtr->isRange = Range();
  if (codePtr->isRange && !KeyVal2(codePtr))
    return FALSE;
  if (!RightParen())	       
    return FALSE;
  return TRUE;
} // Statement


BOOL MacroTrn::Translate	/* ---- Translate ----------------------- */
( AOBJECTID	datbObj,		// datb object
  char		*macroInst,		// macro instruction
  char		*errorMsg,		// error message
  WORD		*macroCodeCount,	// number of macro code
  Macro		**macroCode		// macro code
)
/* Translates instructions script into macro codes. */
{ MemBlock	mb;			// memblock class
  BOOL		result;			// result

  obj = datbObj;
  if ((fieldCount = (WORD) (AObjGetDataSize(datbObj, DATBFIELDS) /
		            sizeof(Field))) != 0)
    field = (Field *) mb.GetPointer(datbObj, DATBFIELDS);
  inst = macroInst;
  errMsg = errorMsg;
  *errMsg = '\0';

  codeCount = 0;
  code = *macroCode;
  result = Code();
  *macroCodeCount = codeCount;
  *macroCode = code;
  return result;
} // Translate
