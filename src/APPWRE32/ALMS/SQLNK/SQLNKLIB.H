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
 | FILE         : SQLNKLIB.H                                              |
 | DESCRIPTION  : SequeLink Library Include File.                         |
 | COPYRIGHT    : Novell, Inc.                                            |
 | EDIT LOG     :                                                         |
 |   v1.00.00     mw    04-Mar-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef SQLNKLIB_H
#define SQLNKLIB_H

#define STRICT
#include <windows.h>

#define _WINDOWS
#define __DLL
#define _USEDLL
#include "sqlnk.h"			// version 2.4


#define SQLNKLIB204	204		// sqlnk version 2.4 
		
class SqlnkLib                    /* ---- SequeLink Library ------------- */
{ BOOL          isHandle;               // is handle? 
  HINSTANCE     handle;                 // handle 
  int           version;                // version 

public:
  SqlnkLib(void);
  /* Constructor. */

  ~SqlnkLib(void);
  /* Destructor. */

  BOOL FreeLib(void);
  /* Frees the library. */

  BOOL IsLoadLib(void);
  /* Returns whether the library is currently loaded. */

  BOOL LoadLib(void);
  /* Returns TRUE or FALSE, as the result of loading the library, and
     the indirect-function addresses.
  */

  int LibVersion(void);
  /* Returns the library version. */
};

#endif // SQLNKLIB_H 


extern SqlnkLib	sqlnkLib;		// sqlnk class 

extern short (CALLBACK *sql_dispatch)(LPVOID);
/* Verb control block structures */
