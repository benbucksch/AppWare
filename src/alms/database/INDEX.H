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
 | FILE		: INDEX.H                                                 |
 | DESCRIPTION	: A B-Tree Index Manager Include File.                    |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#ifndef INDEX_H
#define	INDEX_H

#include "dbkey.h"


#define NODESIZE	500	// 512 - size of record header

#define posofIdxHdr	sizeof(FileID) + sizeof(Lock) + sizeof(FileHdr)
				// position of IdxHdr

struct IdxHdr			/* ---- Index Header -------------------- */
{ long		field;			// field # in the record
  short int	keyType;		// key type
  short int	keyLen;			// key length (1..236)
  long		root;			// root pointer
  long		leftLeaf;		// left most leaf pointer
  long		rightLeaf;		// right most leaf pointer
};

struct IdxNode			/* ---- Index Node ---------------------- */
{ short int	nonLeaf;		// TRUE if non-leaf, FALSE if leaf
  short int	keyCount;		// number of keys in a node
  long		parent;			// parent pointer
  long		left;			// left sibling pointer
  long		right;			// right sibling pointer
  long		key0;			// left kid of key 1
  char		keySpace[NODESIZE - 20];// keys spaces, 20 is sizeof(long) * 5
  char		spill[ODATB_MXKEYSIZE + sizeof(long)];
					// temporary spilling spaces
};

#endif /* INDEX_H */
