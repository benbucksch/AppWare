/*--------------------------------------------------------------------------*\
 | FILE		: DBKEY.CPP                                                 |
 | DESCRIPTION	: Database Key.                                             |
 | COPYRIGHT	: 1993 Novell, Inc.                                         |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw    01-Sep-1993     Initial Edit                        |
\*--------------------------------------------------------------------------*/
#include "datb.h"
#include "dbkey.h"
#include "memblock.h"
#include "safemem.h"
#include "o_date.h"
#include "o_time.h"
#include "o_number.h"
#include "o_text.h"


void *DBKey::GetKey		/* ---- Get Key --------------------------- */
( OBJECTID		datbObj,	// datb object
  OBJECTID		obj,		// object
  int			keyType,	// key type
  int			keySize		// key size
)
/* Returns pointer to the key or the converted key. */
{
  switch ((long) AObjGetType(obj))
  { case OTYPE_DATE:
      if (keyType == ODATB_KEYDATE)
      { *(long *) key.key = ODateGetValue(obj);
	return key.key;
      } // if
      break;
    case OTYPE_TIME:
      if (keyType == ODATB_KEYTIME)
      { *(long *) key.key = OTimeGetValue(obj);
	return key.key;
      } // if
      break;
    case OTYPE_NUMBER:
      switch (keyType)
      { case ODATB_KEYLONG:
	  *(long *) key.key = ONmbrGetInteger(obj);
	  return key.key;
	case ODATB_KEYLONGDOUBLE:
	  ONmbrGetReal(obj, (long double*) key.key);
	  return key.key;
	default:
	  break;
      } // switch
      break;
    case OTYPE_TEXT:
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
    hmemset(key.key, 0, keySize);
  return key.key;
} // GetKey

int DBKey::GetKeyType		/* ---- Get Key Type ---------------------- */
( OBJECTID		datbObj,	// datb object
  OBJECTID		obj,		// object
  TYPEID		type		// type
)
/* Returns the key type. */
{
  if (!type)
    type = AObjGetType(obj);
  switch ((long) type)
  { case OTYPE_TEXT:
      return ODATB_KEYTEXT;
    case OTYPE_NUMBER:
      return ONmbrGetType(obj) == ONMBR_INTEGER?
	       ODATB_KEYLONG : ODATB_KEYLONGDOUBLE;
    case OTYPE_DATE:
      return ODATB_KEYDATE;
    case OTYPE_TIME:
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

int DBKey::GetKeySize		/* ---- Get Key Size ---------------------- */
( int			keyType		// key type
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

int DBKey::GetSearchKeyType	/* ---- Get Search Key Type --------------- */
( OBJECTID		datbObj,	// datb object
  OBJECTID		obj		// object
)
/* Returns the search key type. */
{
  switch ((long) AObjGetType(obj))
  { case OTYPE_DATE:
      return ODATB_KEYDATE;
    case OTYPE_TIME:
      return ODATB_KEYTIME;
    case OTYPE_NUMBER:
      return ODATB_KEYLONGDOUBLE;	// beware of type conversion!!!
    case OTYPE_TEXT:
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

BOOL DBKey::KeyValFromStr	/* ---- Key Value From String ------------- */
( void			*keyVal,	// key value
  int			keyType,	// key type
  int			keySize,	// key size
  char			*str		// string
)
/* Converts string to key value. */
{ MemBlock		mb;		// memblock class
  OBJECTID		obj;		// object
  int			strSize;	// string size

  switch (keyType)
  { case ODATB_KEYTEXT:
      strSize = lstrlen(str) + 1;
      if (strSize > keySize)
      { hmemcpy(keyVal, str, keySize - 1);
	*(char *) hmemptr(keyVal, keySize - 1, sizeof(char)) = 0;
      } // if
      else
	hmemcpy((char *) keyVal, str, strSize);
      return TRUE;
    case ODATB_KEYLONG:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetType(obj, ONMBR_INTEGER);
      ONmbrConvertFromString(obj, str, FALSE);
      *(long *) keyVal = ONmbrGetInteger(obj);
      return TRUE;
    case ODATB_KEYLONGDOUBLE :
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetType(obj, ONMBR_REAL);
      ONmbrConvertFromString(obj, str, FALSE);
      ONmbrGetReal(obj, (long double*) keyVal);
      return TRUE;
    case ODATB_KEYTIME:
      obj = mb.Create(OTYPE_TIME);
      OTimeConvertFromString(obj, str);
      *(long *) keyVal = OTimeGetValue(obj);
      return TRUE;
    case ODATB_KEYDATE:
      obj = mb.Create(OTYPE_DATE);
      ODateConvertFromString(obj, str);
      *(long *) keyVal = ODateGetValue(obj);
      return TRUE;
    default:
      return FALSE;
  } // switch
} // KeyValFromStr

BOOL DBKey::KeyValToStr		/* ---- Key Value To String --------------- */
( void			*keyVal,	// key value
  int			keyType,	// key type
  int			keySize,	// key size
  char			*str		// string
)
/* Converts key value to string. */
{ MemBlock		mb;		// memblock class
  OBJECTID		obj;		// object

  switch (keyType)
  { case ODATB_KEYTEXT:
      hmemcpy(str, keyVal, keySize);
      return TRUE;
    case ODATB_KEYLONG:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetInteger(obj, *(long *) keyVal, TRUE);
      ONmbrConvertToString(obj, str, MXSTRSIZE);
      return TRUE;
    case ODATB_KEYLONGDOUBLE:
      obj = mb.Create(OTYPE_NUMBER);
      ONmbrSetReal(obj, *(long double *) keyVal, TRUE);
      ONmbrConvertToString(obj, str, MXSTRSIZE);
      return TRUE;
    case ODATB_KEYDATE:
      obj = mb.Create(OTYPE_DATE);
      ODateSetValue(obj, *(long *) keyVal);
      ODateConvertToString(obj, str, MXSTRSIZE);
      return TRUE;
    case ODATB_KEYTIME:
      obj = mb.Create(OTYPE_TIME);
      OTimeSetValue(obj, *(long *) keyVal);
      OTimeConvertToString(obj, str, MXSTRSIZE);
      return TRUE;
    default:
      return FALSE;
  } // switch
} // KeyValToStr
