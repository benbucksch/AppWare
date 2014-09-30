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
 | FILE		: MEMBLOCK.H                                              |
 | DESCRIPTION	: Memory Blocks Utilities Include File.                   |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef MEMBLOCK_H
#define MEMBLOCK_H

#define ASTRICT
#include "a_alm.h"

#include "stack.h"


typedef struct mbnode		MBNode;
struct mbnode                   /* ---- Memory Block Node --------------- */
{ AMEMBLOCKID	mb;			// memory block
};

typedef struct objnode          ObjNode;
struct objnode                  /* ---- Object Node --------------------- */
{ AOBJECTID	obj;            	// object
  int		dataIdx;		// data index
};

class MemBlock			/* ---- Memory Block -------------------- */
{ Stack		memStack,		// memory Stack 
		mbStack,		// memory block Stack
		mbIDStack,		// memory block id Stack
		objStack,		// object datablock Stack
		objIDStack;		// object datablock id Stack

  MBNode *NewMBNode(AMEMBLOCKID mb);
  /* Returns a newly created memory block node. */

  ObjNode *NewObjNode(AOBJECTID obj, int dataIdx = 0);
  /* Returns a newly created object node. */

public:
  ~MemBlock(void);
  /* The destructor. */

  void *Alloc(long bytes);
  /* Returns a pointer to the newly allocated temporary memory block of the
     specified size.
  */
   
  AMEMBLOCKID Allocate(long bytes = 0);
  /* Returns a newly created temporary memory block of the specified size.
  */

  AOBJECTID Create(ATYPEID type);
  /* Returns a newly created temporary object of the specified object type.
  */

  void *GetPointer(AMEMBLOCKID mb);
  /* Returns a pointer to the data contained in the specified memory block.
  */

  void *GetPointer(AOBJECTID obj, int dataIdx);
  /* Returns a pointer to the specified object datablock. */
};

#endif /* MEMBLOCK_H */
