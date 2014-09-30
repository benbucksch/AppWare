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
 | FILE		: BTREE.H                                                 |
 | DESCRIPTION	: A B-Tree Index Manager Include File.                    |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit.                        |
 |		  mw	22-May-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#ifndef BTREE_H
#define BTREE_H

#include "vrfile.h"
#include "index.h"


class BTree: public VRFile	/* ---- B Tree ---------------------------- */
{ int		KEYTYPE;		// key type
  int		KEYLEN;			// key length
  int		ENTLEN;			// entire length (KEYLEN+sizeof(long))
  int		MXKEYNO;		// maximum key number
  IdxNode	IDXNODE;		// Index node
  long		CRECPTR;		// current record pointer
  int		CKEYNO;			// current key number

  void	Adopt(HANDLE, void *, int, long);
  /* Assigns NEWPARENT to kid nodes. */

  char	*ChildPtr(HANDLE, long, long, IdxNode *);
  /* Returns ptr to the key in PARENT node, whose left kid pointer = LEFT. */


  void	Implode(HANDLE, FileHdr &, IdxHdr &, IdxNode *, IdxNode *);
  /* Combines LEFT and RIGHT sibling nodes. */

  BOOL	NodeScan(char *, int, IdxNode *, char **, int (*)(char *, char *));
  /* Returns TRUE or FALSE, if KEY is found or not found according to the
     compare function.  Respectively.
     *ADDR >= KEY, where *ADDR - (len(KEY) + sizeof(long)) < KEY, and
     *ADDR + (len(KEY) + sizeof(long)) >= KEY.
  */

  int	NodeSearch(char *, int, int, char *, int (*)(char *, char *),
		   int, int);
  /* Returns the position of KEY in NODE[0..HIGH].
     Pos = n, where KEY > NODE[n - 1], KEY <= NODE[n], and KEY <= NODE[n + 1].
     Pos = HIGH + 1, where KEY > NODE[HIGH].
  */

  void	Redistribute(HANDLE, IdxNode *, IdxNode *);
  /* Redistributes keys between LEFT and RIGHT sibling nodes. */

  BOOL	TreeScan(HANDLE, long *, char *, int, char **,
		 int (*)(char *, char *));
  /* Returns TRUE or FALSE, if KEY is found or not found accordint to the
     compare function.  Respectively.
  */

  void	TreeScan(HANDLE, long *, char *, int, int, char **);
  /* Scans CURRNODE and CURRKNO for insertion. */

public:
  void	BTreeInit(IdxHdr);

  void	CurrKeyVal(HANDLE, char *);
  /* Copies the current key value to KEY. */

  long	CurrRecPtr(HANDLE);
  /* Returns the current record pointer. */

  int	DeleteKey(HANDLE, FileHdr &, IdxHdr &, char *, long);
  /* Deletes KEY associated with RECPTR. */

  long	FirstKey(HANDLE, IdxHdr *);
  /* Returns record pointer of the first key. */

  void	InsertKey(HANDLE, FileHdr &, IdxHdr &, char *, long);
  /* Inserts KEY associated with RECPTR. */

  long	LastKey(HANDLE, IdxHdr *);
  /* Returns record pointer of the last key, or 0. */

  BOOL	LocateKey(HANDLE, IdxHdr *, char *, long,
		  int (*cmp)(char *, char *));
  /* Returns TRUE or FALSE, as the result of locating record with KEY
     associated with RECPTR.
  */

  long	LocateKey(HANDLE, IdxHdr *, char *, int (*)(char *, char *));
  /* Returns the record pointer of KEY, if KEY is found according to the
     compare function.  Otherwise, returns 0. */

  long	LocateNext(HANDLE, IdxHdr *, char *, long);
  /* Returns record pointer of next sequential position of KEYVAL associated
     with RECPTR, or returns 0.
  */

  long	LocateNextKey(HANDLE, IdxHdr *, char *, int (*)(char *, char *));
  /* Returns the next sequential record pointer, if the keys are match
     according to the compare function.  Otherwise, returns 0.
  */

  long	LocatePrev(HANDLE, IdxHdr *, char *, long);
  /* Returns record pointer of previous sequential position of KEYVAL associated
     with RECPTR, or returns 0.
  */

  long	LocateRecIdx(HANDLE, IdxHdr *, long);
  /* Returns record pointer of the record associated with RECIDX, or 0. */

  long	LocateRecID(HANDLE, IdxHdr *, long);
  /* Returns record index of the record associated with RECID, or 0. */

  long	LocateRecID0(HANDLE, IdxHdr *, long);
  /* Returns record index of the record associated with RECORDID, or 0.
     Based on index header 0.
  */

  long	NextKey(HANDLE);
  /* Returns record pointer of the next sequential record, or 0. */

  long	NextRecPtr(HANDLE);
  /* Returns record pointer of the next sequential record, or returns 0. */

  long	PrevRecPtr(HANDLE);
  /* Returns record pointer of the previous sequential record, or returns 0. */

  long  LocateFrontRelN(HANDLE, IdxHdr *, long);
  /* Returns record pointer of the record, relative N position to the first
     record.
  */

  long  LocateRearRelN(HANDLE, IdxHdr *, long);
  /* Returns record pointer of the record, relative N position to the rear
     record.
  */

  long	LocateRelN(HANDLE, IdxHdr *, char *, long, long);
  /* Returns record pointer of the record, relative N position to the current
     key KEYVAL associated with RECPTR, or returns 0.
  */
};

#endif /* BTREE */
