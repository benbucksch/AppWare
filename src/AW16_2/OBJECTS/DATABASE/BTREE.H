/*--------------------------------------------------------------------------*\
 * BTree.H	: B Tree.                                                   *
 *	A B-Tree index manager.                                             *
 * Serius Corporation, September 4, 1992.                                   *
\*--------------------------------------------------------------------------*/
#ifndef BTREE
#define BTREE

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

  void	Adopt(int, void *, int, long);
  /* Assigns NEWPARENT to kid nodes. */

  char	*ChildPtr(int, long, long, IdxNode *);
  /* Returns ptr to the key in PARENT node, whose left kid pointer = LEFT. */


  void	Implode(int, FileHdr &, IdxHdr &, IdxNode *, IdxNode *);
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

  void	Redistribute(int, IdxNode *, IdxNode *);
  /* Redistributes keys between LEFT and RIGHT sibling nodes. */

  BOOL	TreeScan(int, long *, char *, int, char **,
		 int (*)(char *, char *));
  /* Returns TRUE or FALSE, if KEY is found or not found accordint to the
     compare function.  Respectively.
  */

  void	TreeScan(int, long *, char *, int, int, char **);
  /* Scans CURRNODE and CURRKNO for insertion. */

public:
  void	BTreeInit(IdxHdr);

  void	CurrKeyVal(int, char *);
  /* Copies the current key value to KEY. */

  long	CurrRecPtr(int);
  /* Returns the current record pointer. */

  int	DeleteKey(int, FileHdr &, IdxHdr &, char *, long);
  /* Deletes KEY associated with RECPTR. */

  long	FirstKey(int, IdxHdr *);
  /* Returns record pointer of the first key. */

  void	InsertKey(int, FileHdr &, IdxHdr &, char *, long);
  /* Inserts KEY associated with RECPTR. */

  long	LastKey(int, IdxHdr *);
  /* Returns record pointer of the last key, or 0. */

  BOOL	LocateKey(int, IdxHdr *, char *, long, int (*cmp)(char *, char *));
  /* Returns TRUE or FALSE, as the result of locating record with KEY
     associated with RECPTR.
  */

  long	LocateKey(int, IdxHdr *, char *, int (*)(char *, char *));
  /* Returns the record pointer of KEY, if KEY is found according to the
     compare function.  Otherwise, returns 0. */

  long	LocateNext(int, IdxHdr *, char *, long);
  /* Returns record pointer of next sequential position of KEYVAL associated
     with RECPTR, or returns 0.
  */

  long	LocateNextKey(int, IdxHdr *, char *, int (*)(char *, char *));
  /* Returns the next sequential record pointer, if the keys are match
     according to the compare function.  Otherwise, returns 0.
  */

  long	LocatePrev(int, IdxHdr *, char *, long);
  /* Returns record pointer of previous sequential position of KEYVAL associated
     with RECPTR, or returns 0.
  */

  long	LocateRecIdx(int, IdxHdr *, long);
  /* Returns record pointer of the record associated with RECIDX, or 0. */

  long	LocateRecID(int, IdxHdr *, long);
  /* Returns record index of the record associated with RECID, or 0. */

  long	LocateRecID0(int, IdxHdr *, long);
  /* Returns record index of the record associated with RECORDID, or 0.
     Based on index header 0.
  */

  long	NextKey(int);
  /* Returns record pointer of the next sequential record, or 0. */

  long	NextRecPtr(int);
  /* Returns record pointer of the next sequential record, or returns 0. */

  long	PrevRecPtr(int);
  /* Returns record pointer of the previous sequential record, or returns 0. */

  long  LocateFrontRelN(int, IdxHdr *, long);
  /* Returns record pointer of the record, relative N position to the first
     record.
  */

  long  LocateRearRelN(int, IdxHdr *, long);
  /* Returns record pointer of the record, relative N position to the rear
     record.
  */

  long	LocateRelN(int, IdxHdr *, char *, long, long);
  /* Returns record pointer of the record, relative N position to the current
     key KEYVAL associated with RECPTR, or returns 0.
  */
};

#endif /* BTREE */
