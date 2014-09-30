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
 | FILE		: SQLNKLIB.CPP                                            |
 | DESCRIPTION	: SequeLink SQL Library.                                  |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "sqlnklib.h"


SqlnkLib	sqlnkLib;		// sqlnklib class

short (CALLBACK *sql_dispatch)(LPVOID);


SqlnkLib::SqlnkLib		/* ---- SequeLink Library Constructor --- */
( void
)
/* Constructor. */
{
  isHandle = FALSE;
} // SqlnkLib

SqlnkLib::~SqlnkLib		/* ---- SequeLink Library Destructor ---- */
( void
)
/* Destructor. */
{
  FreeLib();
} // ~SqlnkLib


int SqlnkLib::LibVersion	/* ---- Library Version ----------------- */
( void
)
/* Returns the library version. */
{
  return version;
} // LibVersion

BOOL SqlnkLib::FreeLib		/* ---- Free Library -------------------- */
( void
)
/* Frees the library. */
{
  if (isHandle)
  { FreeLibrary(handle);
    isHandle = FALSE;
  } // if
  return FALSE;
} // FreeLib

BOOL SqlnkLib::IsLoadLib	/* ---- Is Load Library ----------------- */
( void
)
/* Returns whether the library is currently loaded. */
{
  return isHandle;
} // IsLoadLib

BOOL SqlnkLib::LoadLib		/* ---- Load Library -------------------- */
( void				     
)
/* Returns TRUE or FALSE, as the result of loading the library, and
   the indirect-function addresses.
*/
{
  if (isHandle)
    return TRUE;			// it's already loaded

  if ((handle = LoadLibrary("SQLNKW3.DLL")) < (HINSTANCE) 32)
    return FALSE;
  else
    version = SQLNKLIB204;
  isHandle = TRUE;

  if ((sql_dispatch = (short (CALLBACK *)(LPVOID))
	  GetProcAddress(handle, "SQLDispatch")) == 0)
    return FreeLib();	   
  return TRUE;
} // LoadLib
