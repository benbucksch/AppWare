/*--------------------------------------------------------------------------*/
/* INDEX.H: Index Manager.                                                  */
/* A B-Tree index manager.                                                  */
/* Serius Corporation, September 4, 1992.                                   */
/*--------------------------------------------------------------------------*/
#ifndef INDEX_H
#define	INDEX_H

#include "dbkey.h"

#define NODESIZE	500	// 512 - size of record header

#define posofIdxHdr	sizeof(FileID) + sizeof(Lock) + sizeof(FileHdr)
struct IdxHdr			// Index Header
{ long	field;				// field # in the record
  int	keyType;			// key type
  int	keyLen;				// key length (1..236)
  long	root;				// root pointer
  long	leftLeaf;			// left most leaf pointer
  long	rightLeaf;			// right most leaf pointer
};

struct IdxNode			// Index Node
{ int	nonLeaf;			// TRUE if non-leaf, FALSE if leaf
  int	keyCount;			// number of keys in a node
  long	parent;				// parent pointer
  long	left;				// left sibling pointer
  long	right;				// right sibling pointer
  long	key0;				// left kid of key 1
  char	keySpace[NODESIZE - 20];	// keys spaces
  char	spill[ODATB_MXKEYSIZE + sizeof(long)];	// temporary spilling spaces
};

#endif /* INDEX_H */
