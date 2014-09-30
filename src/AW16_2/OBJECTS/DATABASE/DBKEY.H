/*--------------------------------------------------------------------------*\
 | FILE		: DBKEY.H                                                   |
 | DESCRIPTION	: Database Key Include File.                                |
 | COPYRIGHT	: 1993 Novell, Inc.                                         |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw    01-Sep-1993     Initial Edit                        |
\*--------------------------------------------------------------------------*/
#ifndef DBKEY_H
#define DBKEY_H

#include "o_datb.h"


#define DEF_TEXTKEYSIZE		13

class DBKey			/* ---- Database Key ---------------------- */
{ ODatbKey		key;		/* database key */

public:
  void *GetKey(OBJECTID datbObj, OBJECTID obj, int keyType, int keySize);
  /* Returns pointer to the key or the converted key. */

  int GetKeyType(OBJECTID datbObj, OBJECTID obj, TYPEID type = 0);
  /* Returns the key type. */

  int GetKeySize(int);
  /* Returns the key size. */

  int GetSearchKeyType(OBJECTID datbObj, OBJECTID obj);
  /* Returns the search key type. */

  BOOL KeyValFromStr(void *keyVal, int keyType, int keySize, char *str);
  /* Converts string to key value. */

  BOOL KeyValToStr(void *keyVal, int keyType, int keySize, char *str);
  /* Converts key value to string. */
};

#endif /* DBKEY_H */
