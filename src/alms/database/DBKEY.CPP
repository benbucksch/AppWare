/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

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
 | FILE		: DBKEY.CPP                                               |
 | DESCRIPTION	: Database Key.                                           |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw    01-Sep-1993     Initial Edit                      |
\*------------------------------------------------------------------------*/
#include "datb.h"
#include "dbkey.h"
#include "memblock.h"
#include "safemem.h"
#include "o_date.h"
#include "o_time.h"
#include "o_number.h"
#include "o_text.h"


void *DBKey::GetKey		/* ---- Get Key ------------------------- */
( AOBJECTID	datbObj,		// datb object
  AOBJECTID	obj,			// object
  short		keyType,		// key type
  short		keySize			// key size
)
/* Returns pointer to the key or the converted key. */
{
  switch ((long) AObjGetType(obj))
  { case (long)OTYPE_DATE:
      if (keyType == ODATB_KEYDATE)
      { *(long *) key.key = ODateGetValue(obj);
	return key.key;
      } // if
      break;
    case (long)OTYPE_TIME:
      if (keyType == ODATB_KEYTIME)
      { *(long *) key.key = OTimeGetValue(obj);
	return key.key;
      } // if
      break;
    case (long)OTYPE_NUMBER:
      switch (keyType)
      { case ODATB_KEYLONG:
	  *(long *) key.key = ONmbrGetInteger(obj);
	  return key.key;
	case (long)ODATB_KEYLONGDOUBLE:
	  ONmbrGetReal(obj, (long double*) key.key);
	  return key.key;
	default:
	  break;
      } // switch
      break;
    case (long)OTYPE_TEXT:
      if (keyType == ODATB_KEYTEXT)
      { OTextGetToBuffer(obj, key.key, keySize);
	return key.key;
      } // if
      break;
    default:
      break;
  } // switch

  /* ---- converted key ---- */
  key.type = keyType;
  if (AEvtCallMethod(datbObj, obj, 0, ODATB_EVT_SENDKEY, (long) &key))
    key.key[keySize - 1] = 0;		// truncates over-sized key
  else
    memset(key.key, 0, keySize);
  return key.key;
} // GetKey

short DBKey::GetKeyType		/* ---- Get Key Type -------------------- */
( AOBJECTID	datbObj,		// datb object
  AOBJECTID	obj,			// object
  ATYPEID	type			// type
)
/* Returns the key type. */
{
  if (!type)
    type = AObjGetType(obj);
  switch ((long) type)
  { case (long)OTYPE_TEXT:
      return ODATB_KEYTEXT;
    case (long)OTYPE_NUMBER:
      if (ONmbrGetType(obj) == ONMBR_INTEGER)
	return ODATB_KEYLONG;
      else
	return ODATB_KEYLONGDOUBLE;
    case (long)OTYPE_DATE:
      return ODATB_KEYDATE;
    case (long)OTYPE_TIME:
      return ODATB_KEYTIME;
    default:
      break;
  } // switch

  key.type = ODATB_KEYQUERY;
  AEvtCallMethod(datbObj, obj, 0, ODATB_EVT_SENDKEY, (long) &key);
  switch (key.type)
  { case ODATB_KEYTEXT:
    case ODATB_KEYLONG:
    case ODATB_KEYLONGDOUBLE:
    case ODATB_KEYDATE:
    case ODATB_KEYTIME:
      return key.type;
    case ODATB_KEYLONG | ODATB_KEYLONGDOUBLE:
      return ODATB_KEYLONGDOUBLE;
    default:
      return ODATB_KEYQUERY;
  } // switch
} // GetKeyType

short DBKey::GetKeySize		/* ---- Get Key Size -------------------- */
( short		keyType			// key type
)
/* Returns the key size. */
{
  switch (keyType)
  { case ODATB_KEYTEXT:
      return DEF_TEXTKEYSIZE;
    case ODATB_KEYLONG:
      return sizeof(long);
    case ODATB_KEYLONGDOUBLE:
      return sizeof(long double);
    case ODATB_KEYDATE:
      return sizeof(long);
    case ODATB_KEYTIME:
      return sizeof(long);
    default:
      return 0;
  } // switch
} // GetKeySize

short DBKey::GetSearchKeyType	/* ---- Get Search Key Type ------------- */
( AOBJECTID	datbObj,		// datb object
  AOBJECTID	obj			// object
)
/* Returns the search key type. */
{
  switch ((long) AObjGetType(obj))
  { case (long)OTYPE_DATE:
      return ODATB_KEYDATE;
    case (long)OTYPE_TIME:
      return ODATB_KEYTIME;
    case (long)OTYPE_NUMBER:
      return ODATB_KEYLONGDOUBLE;	// beware of type conversion!!!
    case (long)OTYPE_TEXT:
      return ODATB_KEYTEXT;
    default:
      break;
  } // switch

  key.type = ODATB_KEYQUERY;
  AEvtCallMethod(datbObj, obj, 0, ODATB_EVT_SENDKEY, (long) &key);
  switch (key.type)
  { case ODATB_KEYLONG:
    case ODATB_KEYLONG | ODATB_KEYLONGDOUBLE:
    case ODATB_KEYLONGDOUBLE:
      return ODATB_KEYLONGDOUBLE;	// beware of type conversion!!!
    default:
      return key.type;
  } // switch
} // GetSearchKeyType

BOOL DBKey::KeyValFromStr	/* ---- Key Value From String ----------- */
( void	   	*keyVal,		// key value
  short 	keyType,		// key type
  short 	keySize,		// key size
  char		*str,			// string
  int		strSize			// string size
)
/* Converts string to key value. */
{ MemBlock	mb;			// memblock class
  AOBJECTID	obj;			// object
  int		textSize;		// string size

  switch (keyType)
  { case ODATB_KEYTEXT:
      textSize = lstrlen(str) + 1;
      if (textSize > keySize)
      { memcpy(keyVal, str, keySize - 1);
	*((char*)keyVal + (keySize - 1)) = 0;
      } // if
      else
	memcpy((char *) keyVal, str, textSize);
      return TRUE;
    case ODATB_KEYLONG:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetType(obj, ONMBR_INTEGER);
      ONmbrConvertFromString(obj, str, FALSE);
      ONmbrConvertToString(obj, str, (short) strSize);
      *(long *) keyVal = ONmbrGetInteger(obj);
      return TRUE;
    case ODATB_KEYLONGDOUBLE :
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetType(obj, ONMBR_REAL);
      ONmbrConvertFromString(obj, str, FALSE);
      ONmbrConvertToString(obj, str, (short) strSize);
      ONmbrGetReal(obj, (long double*) keyVal);
      return TRUE;
    case ODATB_KEYDATE:
      obj = mb.Create(OTYPE_DATE);
      ODateConvertFromString(obj, str);
      ODateConvertToString(obj, str, (short) strSize);
      *(long *) keyVal = ODateGetValue(obj);
      return TRUE;
    case ODATB_KEYTIME:
      obj = mb.Create(OTYPE_TIME);
      OTimeConvertFromString(obj, str);
      OTimeConvertToString(obj, str, (short) strSize);
      *(long *) keyVal = OTimeGetValue(obj);
      return TRUE;
    default:
      return FALSE;
  } // switch
} // KeyValFromStr

BOOL DBKey::KeyValToStr		/* ---- Key Value To String ------------- */
( void	   	*keyVal,		// key value
  short		keyType,		// key type
  short		keySize,		// key size
  char		*str,			// string
  int		strSize			// string size
)
/* Converts key value to string. */
{ MemBlock	mb;			// memblock class
  AOBJECTID	obj;			// object

  switch (keyType)
  { case ODATB_KEYTEXT:
      CopyMemory(str, keyVal, keySize);
      return TRUE;
    case ODATB_KEYLONG:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetInteger(obj, *(long *) keyVal, TRUE);
      ONmbrConvertToString(obj, str, (short) strSize);
      return TRUE;
    case ODATB_KEYLONGDOUBLE:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetReal(obj, *(long double *) keyVal, TRUE);
      ONmbrConvertToString(obj, str, (short) strSize);
      return TRUE;
    case ODATB_KEYDATE:
      obj = mb.Create(OTYPE_DATE);
      ODateSetValue(obj, *(long *) keyVal);
      ODateConvertToString(obj, str, (short) strSize);
      return TRUE;
    case ODATB_KEYTIME:
      obj = mb.Create(OTYPE_TIME);
      OTimeSetValue(obj, *(long *) keyVal);
      OTimeConvertToString(obj, str, (short) strSize);
      return TRUE;
    default:
      return FALSE;
  } // switch
} // KeyValToStr

