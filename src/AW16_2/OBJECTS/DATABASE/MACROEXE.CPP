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
 | FILE		: MACROEXE.CPP                                            |
 | DESCRIPTION	: Database Macro Code Executor.                           |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	08-Oct-1992  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#define STRICT
#include <stdlib.h>

#include "macroexe.h"
#include "btree.h"
#include "cmpfunc.h"
#include "memblock.h"
#include "safemem.h"


void MacroExe::Distinct		/* ---- Distinct ------------------------ */
( long		**dest,			// destination list
  long		*destCount		// destination list count
)
/* Removes duplicates items in DEST list. */
{ long		i;			// counter
  long		j;			// counter

  i = 0;
  while (i < *destCount)
  { j = i + 1;
    while (j < *destCount)
      if (*(long *) hmemptr(*dest, i, sizeof(**dest)) ==
	  *(long *) hmemptr(*dest, j, sizeof(**dest)))
      { (*destCount)--;
	hmemcpy(hmemptr(*dest, j, sizeof(**dest)),
		hmemptr(*dest, j + 1, sizeof(**dest)),
		(*destCount - j) * sizeof(long));
      } // if
      else
       j++;
    i++;
  } // while
  *dest = (long *) MBRealloc(*dest, (*destCount / FOUNDBLOCK + 1) *
                                     FOUNDBLOCK * sizeof(long));
} // Distinct

void MacroExe::DistinctSort	/* ---- Distinct Sort ------------------- */
( long		**dest,			// destination list
  long		*destCount		// destination list count
)
/* Removes duplicate items and sort items in DEST list. */
{ long		i;			// counter

  Sort(dest, destCount);
  i = 1;
  while (i < *destCount)
    if (*(long *) hmemptr(*dest, i - 1, sizeof(**dest)) ==
	*(long *) hmemptr(*dest, i, sizeof(**dest)))
    { (*destCount)--;
      hmemcpy(hmemptr(*dest, i, sizeof(**dest)),
	      hmemptr(*dest, i + 1, sizeof(**dest)),
              (*destCount - i) * sizeof(long));
    } // if
    else
      i++;
} // DistinctSort

void MacroExe::Sort		/* ---- Sort ---------------------------- */
( long		**dest,			// destination list
  long		*destCount		// destination list count
)
/* Sorts items in DEST list. */
{
  qsort((void *) *dest, (unsigned) *destCount, sizeof(long), LongCmp);
} // Sort


void MacroExe::Exclude		/* ---- Exclude ------------------------- */
( long		**dest,			// destination list
  long		*destCount,		// destination list count
  long		*src,			// source list
  long		srcCount		// source list count
)
/* DEST list = DEST list exclude SRC list. */
{ long		s;			// counter
  long		d;			// counter

  for (s = 0; s < srcCount; s++)
  { d = 0;
    while (d < *destCount)
      if (*(long *) hmemptr(*dest, d, sizeof(**dest)) ==
          *(long *) hmemptr(src, s, sizeof(*src)))
      { (*destCount)--;
	hmemcpy(hmemptr(*dest, d, sizeof(**dest)),
		hmemptr(*dest, d + 1, sizeof(**dest)),
		(*destCount - d) * sizeof(long));
      } // if
      else
	d++;
  } // for
  *dest = (long *) MBRealloc(*dest, (*destCount / FOUNDBLOCK + 1) *
                                     FOUNDBLOCK * sizeof(long));
} // Exclude

void MacroExe::Find		/* ---- Find ---------------------------- */
( long		**dest,			// destination list
  long		*destCount,		// destination list count
  long		*src,			// source list
  long		srcCount		// source list count
)
/* DEST list = SRC list. */
{
  *dest = (long *) MBRealloc(*dest, (srcCount / FOUNDBLOCK + 1) *
                                     FOUNDBLOCK * sizeof(long));
  hmemcpy(*dest, src, srcCount * sizeof(long));
  *destCount = srcCount;
} // Find

void MacroExe::Merge		/* ---- Merge --------------------------- */
( long		**dest,			// destination list
  long		*destCount,		// destination list count
  long		*src,			// source list
  long		srcCount		// source list count
)
/* DEST list = DEST list merge SRC list. */
{ long		s;			// counter
  long		d;			// counter
  long		count;			// counter

  count = *destCount;
  for (s = 0; s < srcCount; s++)
  { /* adds the item of SRC list to DEST list */
    for (d = 0; d < count && *(long *) hmemptr(*dest, d, sizeof(**dest)) !=
			     *(long *) hmemptr(src, s, sizeof(*src)); d++);
    if (d == count)
    { if (*destCount % FOUNDBLOCK == 0)
	*dest = (long *) MBRealloc(*dest, (*destCount + FOUNDBLOCK) * sizeof(long));
      *(long *) hmemptr(*dest, *destCount, sizeof(**dest)) =
      *(long *) hmemptr(src, s, sizeof(*src));
      (*destCount)++;
    } // if
  } // for
} // Merge

void MacroExe::Require		/* ---- Require ------------------------- */
( long		**dest,			// destination list
  long		*destCount,		// destination list count
  long		*src,			// source list
  long		srcCount		// source list count
)
/* DEST = DEST require SRC. */
{ long		s;			// counter
  long		d;			// counter

  d = 0;
  while (d < *destCount)
  { for (s = 0; s < srcCount && *(long *) hmemptr(*dest, d, sizeof(**dest)) !=
				*(long *) hmemptr(src, s, sizeof(*src)); s++);
    if (s == srcCount)
    { (*destCount)--;
      hmemcpy(hmemptr(*dest, d, sizeof(*dest)),
	      hmemptr(*dest, d + 1, sizeof(*dest)),
	      (*destCount - d) * sizeof(long));
    } // if
    else
      d++;
  } // while
  *dest = (long *) MBRealloc(*dest, (*destCount / FOUNDBLOCK + 1) * FOUNDBLOCK *
				sizeof(long));
} // Require

void MacroExe::Execute		/* ---- Execute ------------------------- */
( OBJECTID	datbObj,		// datb object
  DatbCore	*datbCore,		// datb core info
  WORD		fieldCount,		// number of fields
  Field		*field,			// field
  WORD		macroCodeCount,		// number of macro code
  Macro		*macroCode		// an arrary macro codes
)
/* Executes macro codes. */
{ Macro		huge *macroCodePtr;	// macro code pointer
  long		tempListCount;		// number of items in temporary list
  long		*tempList;		// temporary list

  WORD		i;			// counter

  for (macroCodePtr = macroCode,
       i = 0; i < macroCodeCount; i++,
       macroCodePtr++)
  { if (!datbCore->foundCount && (macroCodePtr->oprtr == EXCLUDE ||
	                          macroCodePtr->oprtr == EXCLUDEX ||
	                          macroCodePtr->oprtr == REQUIRE ||
	                          macroCodePtr->oprtr == REQUIREX))
      continue;				// do nothing

    if (macroCodePtr->oprtr == EXCLUDEX ||
        macroCodePtr->oprtr == FINDX ||
	macroCodePtr->oprtr == MERGEX ||
	macroCodePtr->oprtr == REQUIREX)
      SearchExact(datbObj, datbCore, fieldCount, field, *macroCodePtr,
        &tempListCount, &tempList);
    else
      SearchPart(datbObj, datbCore, fieldCount, field, *macroCodePtr,
	&tempListCount, &tempList);

    switch (macroCodePtr->oprtr)
    { case EXCLUDE: case EXCLUDEX:
	Exclude(&datbCore->found, &datbCore->foundCount,
	  tempList, tempListCount);
	break;
      case FIND: case FINDX:
	Find(&datbCore->found, &datbCore->foundCount,
	  tempList, tempListCount);
	break;
      case MERGE: case MERGEX:
	Merge(&datbCore->found, &datbCore->foundCount,
	  tempList, tempListCount);
	break;
      case REQUIRE: case REQUIREX:
	Require(&datbCore->found, &datbCore->foundCount,
	  tempList, tempListCount);
	break;
    } // switch
    if (tempList)
      MBFree(tempList);
  } // for
} // Execute

void MacroExe::SearchExact	/* ---- Search Exact -------------------- */
( OBJECTID	datbObj,		// datb object
  DatbCore	*datbCore,		// datb core info
  WORD		fieldCount,		// number of fields
  Field		*field,			// an array of fields
  Macro		code,			// macro code
  long		*listCount,		// number of items in found-list
  long		**list			// found-list
)
/* Searchs record-IDs based on CODE, and puts them into LIST. */
{ Field		huge *fieldPtr;		// field pointer
  WORD		idxHdrPos;		// index header position

  OBJECTID	tempObject;		// temporary object
  char		*tempKey;		// temporary key
  char		currKey[ODATB_MXKEYSIZE];// current key

  BTree		file;			// BTree class
  IdxHdr	idxHdr;			// index header
  char		*key;			// key
  long		recID;			// record ID
  long		recPtr;			// record pointer
  DBKey		dbKey;			// dbkey class
  BOOL		isMatch;		// is match?
  int		(*cmp)(char *, char *);	// the compare function

  WORD		i;			// counter

  (void) fieldCount;

  *listCount = 0;
  *list = 0;
  if (code.oprtr == NOCOMMAND)
    return;

  if (code.isIndex)
  { /* ---- searchs record-IDs based on the index-field tree ---- */
    idxHdrPos = 0;
    for (fieldPtr = field, i = 0; i < code.field; i++, fieldPtr++)
      if (fieldPtr->keyType)
        idxHdrPos++;

    if (!datbCore->accessMode)			// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxHdrPos);
    if (code.isRange)
    { cmp = GetCmpExactGEFunc(idxHdr.keyType);
      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code.keyVal1,
			     GetCmpPartLEFunc(idxHdr.keyType));
      if (recID)
      { /* verifies key value 2 */
	file.CurrKeyVal(datbCore->fileHdl, currKey);
	if ((*cmp)(code.keyVal2, currKey))
	  recID = 0;
	key = code.keyVal2;
      } // if
    } // if
    else
    { cmp = GetCmpExactFunc(idxHdr.keyType);
      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code.keyVal1, cmp);
      key = code.keyVal1;
    } // else
    while (recID)
    { if (*listCount % FOUNDBLOCK == 0)
	*list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) * sizeof(long));
      *(long *) hmemptr(*list, *listCount, sizeof(**list)) = recID;
      (*listCount)++;

      recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, key, cmp);
    } // while
    if (!datbCore->accessMode)			// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
    return;
  } // if

  /* ---- search record-IDs based on record-ID tree ---- */
  if (!datbCore->accessMode)			// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  recPtr = file.FirstKey(datbCore->fileHdl, &idxHdr);
  while (recPtr)
  { isMatch = TRUE;
    if ((tempObject = ReadObject(datbCore->fileHdl, recPtr, code.field - 1)) != 0)
    { tempKey = (char *) dbKey.GetKey(datbObj, tempObject,
                                      code.keyType, code.keySize);
      AObjDestroy(tempObject);
      if (code.isRange)
      { if (CmpExact(code.keyVal1, tempKey, code.keyType) > 0 ||
	    CmpExact(code.keyVal2, tempKey, code.keyType) < 0)
	  isMatch = FALSE;
      } // if
      else
	if (CmpExact(code.keyVal1, tempKey, code.keyType))
	  isMatch = FALSE;
      if (isMatch)
      { recID = 0;
	file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
	if (*listCount % FOUNDBLOCK == 0)
	  *list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) *
					sizeof(long));
	*(long *) hmemptr(*list, *listCount, sizeof(**list)) = recID;
	(*listCount)++;
      } // if
    } // if
    recPtr = file.NextRecPtr(datbCore->fileHdl);
  } // while
  if (!datbCore->accessMode)			// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
} // SearchExact

void MacroExe::SearchPart	/* ---- Search Partial ------------------ */
( OBJECTID	datbObj,		// datb object
  DatbCore	*datbCore,		// datb core info
  WORD		fieldCount,		// number of fields
  Field		*field,			// an array of fields
  Macro		code,			// macro code
  long		*listCount,		// number of items in found-list
  long		**list			// found-list
)
/* Searchs record ids based on the code, and puts them into the list. */
{ Field		huge *fieldPtr;		// field pointer
  WORD		idxHdrPos;		// index header position

  OBJECTID	tempObject;		// temporary object
  char		*tempKey;		// temporary key
  char		currKey[ODATB_MXKEYSIZE];// current key

  BTree		file;			// BTree class
  IdxHdr	idxHdr;			// index header
  char		*key;			// key
  long		recID;			// record ID
  long		recPtr;			// record pointer
  DBKey		dbKey;			// dbkey class
  BOOL		isMatch;		// is match?
  int		(*cmp)(char *, char *);	// the compare function

  WORD		i;			// counter

  (void) fieldCount;

  *listCount = 0;
  *list = 0;
  if (code.oprtr == NOCOMMAND)
    return;

  if (code.isIndex)
  { /* ---- searchs record-IDs based on the index-field tree ---- */
    idxHdrPos = 0;
    for (fieldPtr = field, i = 0; i < code.field; i++, fieldPtr++)
      if (fieldPtr->keyType)
        idxHdrPos++;

    if (!datbCore->accessMode)			// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxHdrPos);
    if (code.isRange)
    { cmp = GetCmpPartGEFunc(idxHdr.keyType);
      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code.keyVal1,
			     GetCmpPartLEFunc(idxHdr.keyType));
      if (recID)
      { /* verifies key value 2 */
	file.CurrKeyVal(datbCore->fileHdl, currKey);
	if ((*cmp)(code.keyVal2, currKey))
	  recID = 0;
	key = code.keyVal2;
      } // if
    } // if
    else
    { cmp = GetCmpPartFunc(idxHdr.keyType);
      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code.keyVal1, cmp);
      key = code.keyVal1;
    } // else
    while (recID)
    { if (*listCount % FOUNDBLOCK == 0)
	*list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) * sizeof(long));
      *(long *) hmemptr(*list, *listCount, sizeof(**list)) = recID;
      (*listCount)++;

      recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, key, cmp);
    } // while
    if (!datbCore->accessMode)			// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
    return;
  } // if

  /* ---- search record-IDs based on record-ID tree ---- */
  if (!datbCore->accessMode)			// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  recPtr = file.FirstKey(datbCore->fileHdl, &idxHdr);
  while (recPtr)
  { isMatch = TRUE;
    if ((tempObject = ReadObject(datbCore->fileHdl, recPtr, code.field - 1)) != 0)
    { tempKey = (char *) dbKey.GetKey(datbObj, tempObject,
                                      code.keyType, code.keySize);
      AObjDestroy(tempObject);
      if (code.isRange)
      { if (CmpPart(code.keyVal1, tempKey, code.keyType) > 0 ||
	    CmpPart(code.keyVal2, tempKey, code.keyType) < 0)
	  isMatch = FALSE;
      } // if
      else
	if (CmpPart(code.keyVal1, tempKey, code.keyType))
	  isMatch = FALSE;
      if (isMatch)
      { recID = 0;
	file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
	if (*listCount % FOUNDBLOCK == 0)
	  *list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) *
					sizeof(long));
	*(long *) hmemptr(*list, *listCount, sizeof(**list)) = recID;
	(*listCount)++;
      } // if
    } // if
    recPtr = file.NextRecPtr(datbCore->fileHdl);
  } // while
  if (!datbCore->accessMode)			// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);
} // SearchPart

void MacroExe::Search		/* ---- Search -------------------------- */
( OBJECTID	datbObj,		// datb object			
  DatbCore	*datbCore,		// database info
  WORD		fieldCount,		// number of fields
  Field		*field,			// an array of fields
  long		codeCount,		// number of macro code
  Macro		*code,			// macro code
  long		*listCount,		// number of items in list
  long		**list			// list
)
/* Searchs record ids based on the code, and puts them into the list. */
{ Field		huge *fieldPtr;		// field pointer
  WORD		idxHdrPos;		// index header position

  OBJECTID	tempObject;		// temporary object
  char		*tempKey;		// temporary key
  char		currKey[ODATB_MXKEYSIZE];// current key

  BTree		file;			// BTree class
  int		idx;			// index counter
  IdxHdr	idxHdr;			// index header
  long		recID;			// record ID
  long		recPtr;			// record pointer
  DBKey		dbKey;			// DBKey class
  BOOL		isMatch;		// is match?
  long		tempListCount;		// number of items in temporary list
  long		*tempList;		// temporary list
  long		listIdx;		// list index
  int		i, j;			// counters
  int		(*cmp)(char *, char *);	// the compare function

  WORD		k;			// counter

  (void) fieldCount;

  *list = 0;
  *listCount = 0;
  for (i = 0; i < codeCount && !code[i].oprtr; i++);
  if (i == codeCount)			// no significant field
    return;
  j = i;				// the first significant field

  for (idx = j; idx < codeCount && (code[idx].oprtr && code[idx].isIndex) ==
                0; idx++);
  if (idx == codeCount)			// no significant index field
  { /* ---- searchs record-IDs based on record-ID tree ---- */
    if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);
    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
    recPtr = file.FirstKey(datbCore->fileHdl, &idxHdr);
    while (recPtr)
    { isMatch = TRUE;
      for (i = j; i < codeCount; i++)
	if (code[i].oprtr)
	{ if ((tempObject = ReadObject(datbCore->fileHdl, recPtr,
				       code[i].field - 1)) == 0)
	  { isMatch = FALSE;
	    break;
	  } // if
	  if ((tempKey = (char *) dbKey.GetKey(datbObj, tempObject,
				    code[i].keyType, code[i].keySize)) == 0)
	  { AObjDestroy(tempObject);
	    isMatch = FALSE;
	    break;
	  } // if
	  AObjDestroy(tempObject);

	  if (code[i].isRange)
	  { if (CmpPart(code[i].keyVal1, tempKey, code[i].keyType) > 0 ||
		CmpPart(code[i].keyVal2, tempKey, code[i].keyType) < 0)
	    { isMatch = FALSE;
	      break;
	    } // if
	  }
	  else
	    if (CmpPart(code[i].keyVal1, tempKey, code[i].keyType))
	    { isMatch = FALSE;
	      break;
	    } // if
	} // if
      if (isMatch)
      { recID = 0;
	file.CurrKeyVal(datbCore->fileHdl, (char *) &recID);
	if (*listCount % FOUNDBLOCK == 0)
	  *list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) *
					sizeof(long));
	*(long *)hmemptr(*list, *listCount, sizeof(**list)) = recID;
	(*listCount)++;
      } // if
      recPtr = file.NextRecPtr(datbCore->fileHdl);
    } // while
    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
    return;
  } // if

  /* ---- searchs record-IDs based on the index-field tree ---- */

  /* ---- the first significant index-field ---- */
  idxHdrPos = 0;
  for (fieldPtr = field, k = 0; k < code[idx].field; k++, fieldPtr++)
    if (fieldPtr->keyType)
      idxHdrPos++;

  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxHdrPos);
  if (code[idx].isRange)
  { cmp = GetCmpPartGEFunc(idxHdr.keyType);
    recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1,
			   GetCmpPartLEFunc(idxHdr.keyType));
    if (recID)
    { file.CurrKeyVal(datbCore->fileHdl, currKey);
      if ((*cmp)(code[idx].keyVal2, currKey))
	recID = 0;
    } // if
  } // if
  else
  { cmp = GetCmpPartFunc(idxHdr.keyType);
    recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1, cmp);
  } // else

  while (recID)
  { if (*listCount % FOUNDBLOCK == 0)
      *list = (long *) MBRealloc(*list, (*listCount + FOUNDBLOCK) * sizeof(long));
    *(long *) hmemptr(*list, *listCount, sizeof(**list)) = recID;
    (*listCount)++;

    if (code[idx].isRange)
      recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal2,
				 cmp);
    else
      recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1,
				 cmp);
  } // while
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  /* ---- other significant index-fields ---- */
  for (idx++; idx < codeCount && *listCount > 0; idx++)
    if (code[idx].oprtr && code[idx].isIndex)
    { idxHdrPos = 0;
      for (fieldPtr = field, i = 0; i < code[idx].field; i++, fieldPtr++)
        if (fieldPtr->keyType)
          idxHdrPos++;
  
      tempListCount = 0;
      tempList = 0;
      if (!datbCore->accessMode)		// the file is not lock
	file.LockFileHdr(datbCore->fileHdl);
      file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, idxHdrPos);
      if (code[idx].isRange)
      { cmp = GetCmpPartGEFunc(idxHdr.keyType);
	recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1,
		  GetCmpPartLEFunc(idxHdr.keyType));
	if (recID)
	{ file.CurrKeyVal(datbCore->fileHdl, currKey);
	  if ((*cmp)(code[idx].keyVal2, currKey))
	    recID = 0;
	} // if
      } // if
      else
      { cmp = GetCmpPartFunc(idxHdr.keyType);
	recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1, cmp);
      } // else
      while (recID)
      { if (tempListCount % FOUNDBLOCK == 0)
	  tempList = (long *) MBRealloc(tempList, (tempListCount + FOUNDBLOCK) *
				    sizeof(long));
	*(long *) hmemptr(tempList, tempListCount, sizeof(*tempList)) = recID;
	tempListCount++;

	if (code[idx].isRange)
	  recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal2,
				     cmp);
	else
	  recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, code[idx].keyVal1,
				     cmp);
      } // while
      if (!datbCore->accessMode)		// the file is not lock
	file.UnlockFileHdr(datbCore->fileHdl);
      Require(list, listCount, tempList, tempListCount);
      MBFree(tempList);
    } // if

  if (*listCount == 0)
    return;

  for (idx = j; idx < codeCount && (code[idx].oprtr && !code[idx].isIndex) ==
                0; idx++);
  if (idx == codeCount)		// no significant non-index field
    return;

  /* ---- compares non-index field ---- */
  if (!datbCore->accessMode)		// the file is not lock
    file.LockFileHdr(datbCore->fileHdl);
  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr);
  if (!datbCore->accessMode)		// the file is not lock
    file.UnlockFileHdr(datbCore->fileHdl);

  cmp = GetCmpPartFunc(idxHdr.keyType);
  listIdx = 0;
  while (*listCount > listIdx)
  { isMatch = TRUE;
    if (!datbCore->accessMode)		// the file is not lock
      file.LockFileHdr(datbCore->fileHdl);
    recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr,
      (char *) hmemptr(*list, listIdx, sizeof(**list)),
			    cmp);
    if (recPtr == 0)
      isMatch = FALSE;
    else
      for (i = j; i < codeCount; i++)
	if (code[i].oprtr && !code[i].isIndex)
	{ if ((tempObject = ReadObject(datbCore->fileHdl, recPtr,
				       code[i].field - 1)) == 0)
	  { isMatch = FALSE;
	    break;
	  } // if
	  if (tempObject == 0)
	  { isMatch = FALSE;
	    break;
	  } // if
	  if ((tempKey = (char *) dbKey.GetKey(datbObj, tempObject,
				    code[i].keyType, code[i].keySize)) == 0)
	  { AObjDestroy(tempObject);
	    isMatch = FALSE;
	    break;
	  } // if
	  AObjDestroy(tempObject);
	  if (code[i].isRange)
	  { if (CmpPart(code[i].keyVal1, tempKey, code[i].keyType) > 0 ||
		CmpPart(code[i].keyVal2, tempKey, code[i].keyType) < 0)
	    { isMatch = FALSE;
	      break;
	    } // if
	  } // if
	  else
	    if (CmpPart(code[i].keyVal1, tempKey, code[i].keyType))
	    { isMatch = FALSE;
	      break;
	    } // if
	} // if
    if (!datbCore->accessMode)		// the file is not lock
      file.UnlockFileHdr(datbCore->fileHdl);
    if (isMatch)
      listIdx++;
    else
    { (*listCount)--;
      hmemcpy((char *) hmemptr(*list, listIdx, sizeof(**list)),
	      (char *) hmemptr(*list, listIdx + 1, sizeof(**list)),
	      (*listCount - listIdx) * sizeof(long));
    } // else
  } // while
} // Search

//void MacroExe::ExcAndReq1	/* ---- Exclude and Require --------------- */
//( DatbCore	*datbCore,
//  OBJECTID	datbObj,		// database object
//  MacroCode	code,			// macro code
//  long		*foundCount,		// number of items in found-list
//  long		**found			// found-list
//)
///* Minus foundList based on exclude and require code. */
//{ BTree		file;			// BTree class
//  IdxHdr	idxHdr;			// index header
//  OBJECTID	currObject;		// current object
//  ODatbKey	currKey;		// current key
//  char		*key;			// key
//  long		recID;			// record ID
//  long		recPtr;			// record pointer
//  BOOL		isMatch;		// is match?
//  long		foundIdx;		// found-list index
//  long		listCount;		// number of items in list
//  long		*list;			// list
//  int		(*cmp)(char *, char *);	// the compare function
//
//  if (code.oprtr == NOCOMMAND || *foundCount == 0)
//    return;
//
//  if (code.isIndex)
//  { /* ---- searchs record-IDs based on the index-field tree ---- */
//    if (!datbCore->accessMode)			// the file is not lock
//      file.LockFileHdr(datbCore->fileHdl);
//    file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, datbCore->idxList[code.field - 1]);
//    if (code.isRange)
//    { cmp = GetCmpExactGEFunc(idxHdr.keyType);
//      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, code.keyVal1,
//			     GetCmpPartLEFunc(idxHdr.keyType));
//      if (recID)
//      { /* verifies key value 2 */
//	file.CurrKeyVal(datbCore->fileHdl, currKey.key);
//	if ((*cmp)(key, currKey.key))
//	  recID = 0;
//	key = code.keyVal2;
//      } // if
//    } // if
//    else
//    { if (code.oprtr == EXCLUDEX || code.oprtr == FINDX ||
//	  code.oprtr == MERGEX || code.oprtr == REQUIREX)
//	cmp = GetCmpExactFunc(idxHdr.keyType);
//      else
//	cmp = GetCmpPartFunc(idxHdr.keyType);
//      recID = file.LocateKey(datbCore->fileHdl, &idxHdr, key, cmp);
//      key = code.keyVal1;
//    } // else
//    while (recID)
//    { if (listCount % FOUNDBLOCK == 0)
//	list = (long *) MBRealloc(list, (listCount + FOUNDBLOCK) * sizeof(long));
//      list[listCount++] = recID;
//
//      recID = file.LocateNextKey(datbCore->fileHdl, &idxHdr, key, cmp);
//    } // while
//    switch (code.oprtr)
//    { case EXCLUDE	:
//      case EXCLUDEX	:
//	Exclude(found, foundCount, list, listCount);
//	break;
//      case REQUIRE	:
//      case REQUIREX	:
//	Require(found, foundCount, list, listCount);
//	break;
//      default		: break;
//    } // switch
//    if (list)
//      MemFree(list);
//    if (!datbCore->accessMode)			// the file is not lock
//      file.UnlockFileHdr(datbCore->fileHdl);
//    return;
//  } // if
//
//  /* ---- search record-IDs based on record-ID tree ---- */
//  if (!datbCore->accessMode)			// the file is not lock
//    file.LockFileHdr(datbCore->fileHdl);
//  file.ReadBTreeHdr(datbCore->fileHdl, &idxHdr, 0);
//  cmp = GetCmpExactFunc(idxHdr.keyType);
//  foundIdx = 0;
//  while (foundIdx < *foundCount)
//  { isMatch = TRUE;
//    if ((recPtr = file.LocateKey(datbCore->fileHdl, &idxHdr,
//				 (char *) (*found + foundIdx), cmp)) != 0)
//    { if ((currObject = ReadObject(datbCore->fileHdl, recPtr, code.field - 1)) != 0)
//      { currKey.type = code.keyType;
//	AEvtCallMethod(datbObj, currObject, 0, ODATB_EVT_GETKEY, (long) &currKey);
//	AObjDestroy(currObject);
//	if (code.keyType == ODATB_KEYTEXT)
//	  AnsiUpperBuff(currKey.key, code.keyLen);
//	if (code.isRange)
//	{ if (CmpExact(code.keyVal1, currKey.key, code.keyType) > 0 ||
//	      CmpExact(code.keyVal2, currKey.key, code.keyType) < 0)
//	    isMatch = FALSE;
//	} // if
//	else
//	  if (CmpExact(code.keyVal1, currKey.key, code.keyType))
//	    isMatch = FALSE;
//      } // if
//      else
//	isMatch = FALSE;
//    } // if
//    else
//      isMatch = FALSE;
//    switch (code.oprtr)
//    { case EXCLUDE:
//      case EXCLUDEX:
//	if (recPtr == 0 || isMatch)
//	{ (*foundCount)--;
//	  memmove((char *) (*found + foundIdx),
//		  (char *) (*found + foundIdx + 1),
//		  (*foundCount - foundIdx) * sizeof(long));
//	} // if
//	else
//	  foundIdx++;
//	break;
//      case REQUIRE:
//      case REQUIREX:
//	if (isMatch)
//	  foundIdx++;
//	else
//	{ (*foundCount)--;
//	  memmove((char *) (*found + foundIdx),
//		  (char *) (*found + foundIdx + 1),
//		  (*foundCount - foundIdx) * sizeof(long));
//	} // else
//	break;
//      default: break;
//    } // switch
//  } // while
//  if (!datbCore->accessMode)			// the file is not lock
//    file.UnlockFileHdr(datbCore->fileHdl);
//} // ExcAndReq
