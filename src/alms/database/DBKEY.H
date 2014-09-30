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
 | FILE		: DBKEY.H                                                 |
 | DESCRIPTION	: Database Key Include File.                              |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw    01-Sep-1993     Initial Edit                      |
\*------------------------------------------------------------------------*/
#ifndef DBKEY_H
#define DBKEY_H

#include "o_datb.h"


#define DEF_TEXTKEYSIZE		13

class DBKey			/* ---- Database Key -------------------- */
{ ODatbKey	key;			/* database key */

public:
  void *GetKey(AOBJECTID datbObj, AOBJECTID obj, short keyType,
	       short keySize);
  /* Returns pointer to the key or the converted key. */

  short GetKeyType(AOBJECTID datbObj, AOBJECTID obj, ATYPEID type = 0);
  /* Returns the key type. */

  short GetKeySize(short);
  /* Returns the key size. */

  short GetSearchKeyType(AOBJECTID datbObj, AOBJECTID obj);
  /* Returns the search key type. */

  BOOL KeyValFromStr(void *keyVal, short keyType, short keySize,
		     char *str, int strSize);
  /* Converts string to key value. */

  BOOL KeyValToStr(void *keyVal, short keyType, short keySize,
		   char *str, int strSize);
  /* Converts key value to string. */
};

#endif /* DBKEY_H */

