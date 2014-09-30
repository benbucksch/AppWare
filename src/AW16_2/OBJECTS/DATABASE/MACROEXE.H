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
 | FILE		: MACROEXE.H                                              |
 | DESCRIPTION	: Database Macro Code Executor Include File.              |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	08-Oct-1992  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "macro.h"
#include "datb_fnc.h"


class MacroExe			/* ---- Macro code Executor ------------- */
{ void	Distinct(long **, long *);
  /* Removes duplicates items in DEST list. */

  void	DistinctSort(long **, long *);
  /* Removes duplicates items in sorted DEST list. */

  void	Sort(long **, long *);
  /* Sorts items in DEST list. */


  void SearchExact		/* ---- Search Exact -------------------- */
  ( OBJECTID	datbObj,		// datb object
    DatbCore	*datbCore,		// datb core info
    WORD	fieldCount,		// number of fields
    Field	*field,			// an array of fields
    Macro	code,			// macro code
    long	*listCount,		// number of items in found-list
    long	**list			// found-list
  );
  /* Searchs record ids based on the code, and puts them into the list. */

  void SearchPart		/* ---- Search Partial ------------------ */
  ( OBJECTID	datbObj,		// datb object
    DatbCore	*datbCore,		// datb core info
    WORD	fieldCount,		// number of fields
    Field	*field,			// an array of fields
    Macro	code,			// macro code
    long	*listCount,		// number of items in found-list
    long	**list			// found-list
  );
  /* Searchs record ids based on the code, and puts them into the list. */
public:
  void	Exclude(long **, long *, long *, long);
  /* DEST list = DEST list excludes SRC list. */

  void	Execute(OBJECTID, DatbCore *, WORD, Macro *);
  /* Executes macro codes. */

  void	Find(long **, long *, long *, long);
  /* DEST list = SRC list. */

  void	Merge(long **, long *, long *, long);
  /* DEST list = DEST list merges SRC list. */

  void	Require(long **, long *, long *, long);
  /* DEST list = DEST list requires SRC list. */


  void Execute			/* ---- Execute ------------------------- */
  ( OBJECTID	datbObj,		// datb object
    DatbCore	*datbCore,		// datb core info
    WORD	fieldCount,		// number of fields
    Field	*field,			// field
    WORD	macroCodeCount,		// number of macro code
    Macro	*macroCode		// an arrary macro codes
  );
  /* Executes macro codes. */

  void Search			/* ---- Search -------------------------- */
  ( OBJECTID	datbObj,		// datb object			
    DatbCore	*datbCore,		// database info
    WORD	fieldCount,		// number of fields
    Field	*field,			// an array of fields
    long	codeCount,		// number of macro code
    Macro	*code,			// macro code
    long	*listCount,		// number of items in found-list
    long	**list			// list
  );
  /* Searchs record ids based on the code, and puts them into the list. */
};
