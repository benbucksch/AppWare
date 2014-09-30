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
 | FILE		: MEMBLOCK.CPP                                            |
 | DESCRIPTION	: Memory Blocks Utilities.                                |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include "memblock.h"


MBNode *MemBlock::NewMBNode	/* ---- New Memory Block Node ----------- */
( AMEMBLOCKID	mb			// memory block
)
/* Returns the newly created memory block node. */
{ MBNode	*mbNode;		// memory block node

  mbNode = (MBNode *) MemAlloc(sizeof(MBNode));
  mbNode->mb = mb;
  return mbNode;
} // NewMBNode

ObjNode *MemBlock::NewObjNode	/* ---- New Object Node ----------------- */
( AOBJECTID	obj,            	// object
  int		dataIdx         	// data index
)
/* Returns the newly created object node. */
{ ObjNode	*objNode;		// object node

  objNode = (ObjNode *) MemAlloc(sizeof(ObjNode));
  objNode->obj = obj;
  objNode->dataIdx = dataIdx;
  return objNode;
} // NewObjNode


MemBlock::~MemBlock		/* ---- Memory Block -------------------- */
( void
)
/* The destructor. */
{ void		*ptr;			// pointer
  MBNode	*mbNode;		// memory block node
  ObjNode	*objNode;		// object node
  HANDLE	gHdl;			// global memory handle

  /* ---- memory ---- */
  while ((ptr = memStack.Pop()) != 0)
    MemFree(ptr);

  while ((ptr = gmStack.Pop()) != 0)
    GMFree(ptr);

  /* ---- memory block ---- */
  while ((mbNode = (MBNode *) mbStack.Pop()) != 0)
  { AMemUnlock(mbNode->mb);
    MemFree(mbNode);
  } // while
  while ((mbNode = (MBNode *) mbIDStack.Pop()) != 0)
  { AMemFree(mbNode->mb);
    MemFree(mbNode);
  } // while

  /* ---- object datablock ---- */
  while ((objNode = (ObjNode *) objStack.Pop()) != 0)
  { AObjUnlockData(objNode->obj, objNode->dataIdx);
    MemFree(objNode);
  } // while
  while ((objNode = (ObjNode *) objIDStack.Pop()) != 0)
  { AObjDestroy(objNode->obj);
    MemFree(objNode);
  } // while

  /* ---- global memory ---- */
  while ((gHdl = (HANDLE) gStack.Pop()) != 0)
    GlobalUnlock(gHdl);
} // ~MemBlock


void *MemBlock::Alloc		/* ---- Allocate ------------------------ */
( size_t	byteCount		// number of bytes
)
/* Returns the pointer to the newly allocated temporary memory block of the
   specified size.
*/
{ void		*ptr;			// pointer

  ptr = MemAlloc(byteCount);
  memStack.Push(ptr);
  return ptr;
} // Alloc

void *MemBlock::GAlloc		/* ---- Global Memory Allocate ---------- */
( long		byteCount		// number of bytes
)
/* Returns the pointer to the newly allocate temporary memory block of the
   specified size.
*/
{ void		*ptr;			// pointer

  ptr = GMAlloc(byteCount);
  gmStack.Push(ptr);
  return ptr;
} // GAlloc

AMEMBLOCKID MemBlock::Allocate	/* ---- Allocate ------------------------ */
( long		byteCount		// number of bytes
)
/* Returns the newly created temporary memory block of the specified size.
*/
{ AMEMBLOCKID	mb;			// memory block

  while ((mb = AMemAllocate(byteCount)) == 0)
    MemError();
  mbIDStack.Push(NewMBNode(mb));
  return mb;
} // Allocate

AOBJECTID MemBlock::Create	/* ---- Create -------------------------- */
( ATYPEID	type			// type
)
/* Returns the newly created temporary object of the specified object type.
*/
{ AOBJECTID	obj;			// object

  if ((obj = AObjCreate(type)) != 0)
    objIDStack.Push(NewObjNode(obj));
  return obj;
} // Create


void *MemBlock::GetPointer	/* ---- Get Pointer --------------------- */
( AMEMBLOCKID	mb			// memory block
)
/* Returns the pointer to the data contained in the specified memory block.
*/
{ void		*ptr;			// pointer

  if ((ptr = AMemLock(mb)) != 0)
    mbStack.Push(NewMBNode(mb));
  return ptr;
} // GetPointer

void *MemBlock::GetPointer	/* ---- Get Pointer --------------------- */
( AOBJECTID	obj,			// object
  int		dataIdx			// data index
)
/* Returns the pointer to the specified object datablock. */
{ void		*ptr;			// pointer

  if ((ptr = AObjLockData(obj, dataIdx)) != 0)
    objStack.Push(NewObjNode(obj, dataIdx));
  return ptr;
} // GetPointer


void *MemBlock::GLock		/* ---- Global Memory Lock -------------- */
( HANDLE	gHdl			// global memory handle
)
/* Returns the pointer to the global memory block. */
{ void		*ptr;			// pointer

  if ((ptr = GlobalLock(gHdl)) != 0)
    gStack.Push((void *) gHdl);
  return ptr;
} // GLock
