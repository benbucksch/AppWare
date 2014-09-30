/*--------------------------------------------------------------------------*\
 | BTREE.CPP: B Tree.                                                       |
 |	A B-Tree index manager.                                             |
 | Serius Corporation, September 4, 1992.				mw  |
\*--------------------------------------------------------------------------*/
#include <string.h>
#include "safemem.h"
#include "cmpfunc.h"
#include "btree.h"


void BTree::Adopt		/* ---- Adopt ----------------------------- */
( int		fileHdl,		// file handle
  void		*recPtr,		// pointer to record pointers
  int		recCount,		// number of record pointers
  long		newParent		// new parent pointer
)
/* Assigns NEWPARENT to kid nodes. */
{ IdxNode	*kidNode;		// kid node

  kidNode = (IdxNode *) MemAlloc(sizeof(IdxNode));
  while (recCount--)
  { ReadRec(fileHdl, *(long *) recPtr, kidNode, NODESIZE);
    kidNode->parent = newParent;
    WriteRec(fileHdl, *(long *) recPtr, kidNode, NODESIZE);
    (char *) recPtr = (char *) recPtr + ENTLEN;
  } // while
  MemFree(kidNode);
} // Adopt

char *BTree::ChildPtr		/* ---- Child Pointer --------------------- */
( int		fileHdl,		// file handle
  long		left,			// left node pointer
  long		parent,			// parent node pointer
  IdxNode	*parentNode		// parent node
)
/* Returns ptr to the key in PARENT node, whose left kid pointer = LEFT. */
{ long		*recPtr;		// record pointer

  ReadRec(fileHdl, parent, parentNode, NODESIZE);
  recPtr = (long *) (parentNode->keySpace - sizeof(long));	// key0
  while (*recPtr != left)
    (char *) recPtr = (char *) recPtr + ENTLEN;
  return (char *) recPtr + sizeof(long);
} // ChildPtr

void BTree::Implode		/* ---- Implode --------------------------- */
( int		fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  IdxHdr	&idxHdr,		// index header
  IdxNode	*leftNode,		// left node
  IdxNode	*rightNode		// right node
)
/* Combines LEFT and RIGHT sibling nodes. */
{ long		left;			// left node pointer
  long		right;			// right node pointer
  long		parent;			// parent node pointer
  IdxNode	parentNode;		// parent node
  char		*parentKey;		// parent key whose left kid = left
  char		*ptr;			// temporary pointer

  left = rightNode->left;
  right = leftNode->right;
  parent = leftNode->parent;
  parentKey = ChildPtr(fileHdl, left, parent, &parentNode);

  /* ---- slides the parent key to the end of left node ---- */
  ptr = leftNode->keySpace + leftNode->keyCount * ENTLEN;
  _fmemcpy(ptr, parentKey, KEYLEN);

  /* ---- slides the keys from right node to left node ---- */
  ptr += KEYLEN;
  memmove(ptr, rightNode->keySpace - sizeof(long),
	  rightNode->keyCount * ENTLEN + sizeof(long));

  if (leftNode->nonLeaf)
    /* ---- points lower nodes to their new parent ---- */
    Adopt(fileHdl, &rightNode->key0, rightNode->keyCount + 1, left);

  /* --- update control values in left sibling node --- */
  leftNode->keyCount += rightNode->keyCount + 1;
  if (idxHdr.rightLeaf == leftNode->right)
    idxHdr.rightLeaf = rightNode->left;
  leftNode->right = rightNode->right;

  /* ---- points the deleted node's right brother to this left brother ---- */
  if (leftNode->right)
  { ReadRec(fileHdl, leftNode->right, rightNode, NODESIZE);
    rightNode->left = left;
    WriteRec(fileHdl, leftNode->right, rightNode, NODESIZE);
  } // if

  /* ---- removes key from the parent node ---- */
  parentNode.keyCount--;
  if (parentNode.keyCount == 0)
  { if (parentNode.parent == 0)
      leftNode->parent = 0;
  }
  else
    memmove(parentKey, parentKey + ENTLEN, (int)
      (parentNode.keySpace + parentNode.keyCount * ENTLEN - parentKey));

  WriteRec(fileHdl, left, leftNode, NODESIZE);
  FreeRec(fileHdl, fileHdr, right);
  WriteRec(fileHdl, parent, &parentNode, NODESIZE);
} // Implode

BOOL BTree::NodeScan           /* ---- Node Scan ------------------------- */
( char          *key,                   // key
  int           keyLen,                 // key length
  IdxNode       *node,                  // index node
  char          **addr,                 // address of the found key in node
  int           (*cmp)(char *, char *)	// the compare function
)
/* Return TRUE or FALSE, if KEY is found or not found according to the
   compare function.  Respectively.
   *ADDR >= KEY, where *ADDR - (len(KEY) + sizeof(long)) < KEY, and
   *ADDR + (len(KEY) + sizeof(long)) >= KEY.
*/
{ int           pos;                    // position of the key

  pos = NodeSearch(key, keyLen, keyLen + sizeof(long), node->keySpace, cmp, 0,
		   node->keyCount - 1);
  *addr = node->keySpace + pos * (keyLen + sizeof(long));
  return pos == node->keyCount || (*cmp)(key, *addr)? FALSE : TRUE;
} // NodeScan

int BTree::NodeSearch           /* ---- Node Search ----------------------- */
( char          *key,                   // key
  int           keyLen,                 // key length
  int           entLen,                 // key length + pointer length
  char          *node,                  // node of keys
  int           (*cmp)(char *, char *),	// the compare function
  int           low,                    // low
  int           high                    // high
)
/* Returns the position of KEY in NODE[0..HIGH].
   Pos = n, where KEY > NODE[n-1], KEY <= NODE[n], and KEY <= NODE[n+1].
   Pos = HIGH + 1, where KEY > NODE[HIGH].
*/
{ int           mid;                    // middle

  if (low > high)
    return low;
  mid = (low + high) >> 1;
  return (*cmp)(key, node + mid * entLen) > 0?
	   NodeSearch(key, keyLen, entLen, node, cmp, mid + 1, high) :
	   NodeSearch(key, keyLen, entLen, node, cmp, low, mid - 1);
} // NodeSearch

void BTree::Redistribute	/* ---- Redistribute ---------------------- */
( int		fileHdl,		// file handle
  IdxNode	*leftNode,		// left node
  IdxNode	*rightNode		// right node
)
/* Redistributes keys between LEFT and RIGHT sibling nodes. */
{ int		newLeftKeyCount;	// new left key count
  int		newRightKeyCount;	// new right key count
  long		parent;			// parent pointer
  IdxNode	*parentNode;		// parent node;
  char		*parentKey;		// the parent key whose left kid = left
  char		*src;			// source
  char		*dest;			// destination
  int		len;			// length to move

  newLeftKeyCount = (leftNode->keyCount + rightNode->keyCount) >> 1;
  if (newLeftKeyCount == leftNode->keyCount)	// no redistribution
    return;
  newRightKeyCount = leftNode->keyCount + rightNode->keyCount - newLeftKeyCount;
  parent = leftNode->parent;
  parentNode = (IdxNode *) MemAlloc(sizeof(IdxNode));
  parentKey = ChildPtr(fileHdl, rightNode->left, parent, parentNode);
  if (leftNode->keyCount < rightNode->keyCount)
  { /* ---- redistributes right to left ---- */

    /* ---- slides the parent key to left node ---- */
    dest = leftNode->keySpace + leftNode->keyCount * ENTLEN;
    memmove(dest, parentKey, KEYLEN);
    dest += KEYLEN;

    /* ---- slides the keys from right node to left node ---- */
    src = rightNode->keySpace - sizeof(long);
    len = (rightNode->keyCount - newRightKeyCount - 1) * ENTLEN + sizeof(long);
    memmove(dest, src, len);
    if (leftNode->nonLeaf)
      Adopt(fileHdl, dest, rightNode->keyCount - newRightKeyCount,
	    rightNode->left);

    /* ---- slides the key from right node to parent node ---- */
    src += len;
    memmove(parentKey, src, KEYLEN);

    /* ---- slides the remaining keys within the right node ---- */
    src += KEYLEN;
    dest = rightNode->keySpace - sizeof(long);
    len = newRightKeyCount * ENTLEN + sizeof(long);
    memmove(dest, src, len);
  } // if
  else
  { /* ---- redistributes left to right ---- */

    /* ---- slides the keys within right node to make room ---- */
    dest = rightNode->keySpace + (newRightKeyCount - rightNode->keyCount) *
	  ENTLEN - sizeof(long);
    memmove(dest, rightNode->keySpace - sizeof(long), rightNode->keyCount *
	    ENTLEN + sizeof(long));

    /* ---- slides the parent key to left node ---- */
    dest -= KEYLEN;
    memmove(dest, parentKey, KEYLEN);

    /* ---- slides the key from left node to parent node ---- */
    src = leftNode->keySpace + newLeftKeyCount * ENTLEN;
    memmove(parentKey, src, KEYLEN);

    /* ---- slides the keys from left node to right node ---- */
    src += KEYLEN;
    memmove(rightNode->keySpace - sizeof(long), src, (leftNode->keyCount -
	    newLeftKeyCount - 1) * ENTLEN + sizeof(long));
    if (rightNode->nonLeaf)
      Adopt(fileHdl, rightNode->keySpace - sizeof(long), leftNode->keyCount -
	    newLeftKeyCount, leftNode->right);
  } // else
  leftNode->keyCount = newLeftKeyCount;
  rightNode->keyCount = newRightKeyCount;
  WriteRec(fileHdl, parent, parentNode, NODESIZE);
  MemFree(parentNode);
} // Redistribute

BOOL BTree::TreeScan		/* ---- Tree Scan --------------------------*/
( int		fileHdl,		// file handle
  long		*nodePtr,		// node pointer
  char		*key,			// key
  int		keyLen,			// key length
  char		**keyAddr,		// the key in the node
  int		(*cmp)(char *, char *)	// the compare function
)
/* Returns TRUE or FALSE, if KEY is found or not found according to the
   compare function.  Respectively.
*/
{ BOOL		isFound;		// is found?
  BOOL		isNonLeaf;		// is non-leaf?

  isFound = FALSE;
  isNonLeaf = TRUE;
  while (isNonLeaf)
  { if (NodeScan(key, keyLen, &IDXNODE, keyAddr, cmp))
      isFound = TRUE;
    if (IDXNODE.nonLeaf)
    { *nodePtr = *(long *) (*keyAddr - sizeof(long));
      ReadRec(fileHdl, *nodePtr, &IDXNODE, NODESIZE);
    } // if
    else
      isNonLeaf = FALSE;
  } // while
  if (IDXNODE.keySpace + IDXNODE.keyCount * (keyLen + sizeof(long)) == *keyAddr)
    if (IDXNODE.right)
    { *nodePtr = IDXNODE.right;
      ReadRec(fileHdl, *nodePtr, &IDXNODE, NODESIZE);
      *keyAddr = IDXNODE.keySpace - sizeof(long);
      CRECPTR = *nodePtr;
      CKEYNO = 0;
      return isFound;
    } // if
  CRECPTR = *nodePtr;
  CKEYNO = (int) (*keyAddr - IDXNODE.keySpace) / ENTLEN + 1;
  return CKEYNO > IDXNODE.keyCount || (*cmp)(key, *keyAddr)?
	   FALSE : TRUE;
} // TreeScan

void BTree::TreeScan		/* ---- Tree Scan for Insertion ----------- */
( int		fileHdl,		// file handle
  long		*nodePtr,		// node pointer
  char		*key,			// key
  int		keyLen,			// key length
  int		keyType,		// key type
  char		**keyAddr		// key address in the node
)
/* Scans CURRNODE and CURRKNO for insertion. */
{ BOOL		isNonLeaf;		// is non-leaf?

  do
  { NodeScan(key, keyLen, &IDXNODE, keyAddr, GetCmpExactLTFunc(keyType));
    isNonLeaf = IDXNODE.nonLeaf;
    if (isNonLeaf)
    { *nodePtr = *(long *) (*keyAddr - sizeof(long));
      ReadRec(fileHdl, *nodePtr, &IDXNODE, NODESIZE);
    } // if
  } while(isNonLeaf);
  CRECPTR = *nodePtr;
  CKEYNO = (int) ((*keyAddr - IDXNODE.keySpace) / ENTLEN + 1);
} // TreeScan

void BTree::BTreeInit		/* ---- BTree Initialization -------------- */
( IdxHdr		idxHdr		// index header
)
/* Inits BTree class. */
{
  KEYTYPE = idxHdr.keyType;
  KEYLEN = idxHdr.keyLen;
  ENTLEN = KEYLEN + sizeof(long);
  MXKEYNO = (NODESIZE - 20) / ENTLEN;
  CRECPTR = CKEYNO = 0;
} // BTreeInit

void BTree::CurrKeyVal		/* ---- Current Key Value ----------------- */
( int		fileHdl,		// file handle
  char		*key			// key
)
/* Copies current key value to KEY. */
{ long		parent;			// parent record pointer
  IdxNode	*parentNode;		// parent node
  char		*keyAddr;		// the key in the parent node
  long		kid;			// kid record pointer

  if (CRECPTR == 0)
    return;
  if (CKEYNO > IDXNODE.keyCount)
  { if (IDXNODE.right == 0)
      return;
    CRECPTR = IDXNODE.right;
    CKEYNO = 0;
    ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
  } // if
  if (CKEYNO == 0)
  { kid = CRECPTR;
    if ((parent = IDXNODE.parent) == 0)
      return;
    parentNode = (IdxNode *) MemAlloc(sizeof(IdxNode));
    while ((keyAddr = ChildPtr(fileHdl, kid, parent, parentNode)) ==
	    parentNode->keySpace)
    { kid = parent;
      if ((parent = parentNode->parent) == 0)
      { MemFree(parentNode);
	return;
      } // if
    } // while
    memcpy(key, keyAddr - ENTLEN, KEYLEN);
    MemFree(parentNode);
    return;
  } // if
  memcpy(key, IDXNODE.keySpace + (CKEYNO - 1) * ENTLEN, KEYLEN);
} // CurrKeyVal

BOOL BTree::DeleteKey		/* ---- Delete Key ------------------------ */
( int		fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  IdxHdr	&idxHdr,		// index header
  char		*key,			// key
  long		recPtr			// record pointer
)
/* Deletes KEY associated with RECPTR. */
{ IdxNode	tempNode;		// temporary node
  char		*keyAddr;		// key address
  long		parent;			// parent
  long		kid;			// kid

  int           comb;
  long          p;
  long          adr;
  long          y;
  long          z;
  char          *a;

  if (idxHdr.root == 0)
    return FALSE;			// nothing to delete
  BTreeInit(idxHdr);

  p = idxHdr.root;
  ReadRec(fileHdl, p, &IDXNODE, NODESIZE);
  TreeScan(fileHdl, &p, key, idxHdr.keyLen, idxHdr.keyType, &a);
  do
  { if (!(adr = PrevRecPtr(fileHdl)))
      return FALSE;
  } while (adr != recPtr);
  p = CRECPTR;
  if (CKEYNO)
    a = IDXNODE.keySpace + (CKEYNO - 1) * ENTLEN;
  else
  { kid = CRECPTR;
    parent = IDXNODE.parent;
    if (parent == 0)
      return FALSE;
    while ((keyAddr = ChildPtr(fileHdl, kid, parent, &tempNode)) ==
	   tempNode.keySpace)
    { kid = parent;
      if ((parent = tempNode.parent) == 0)
	return FALSE;
    } // while

    /* moves left-most key from the leaf node to where the deleted key is */
    _fmemcpy(keyAddr - ENTLEN, IDXNODE.keySpace, KEYLEN);
    WriteRec(fileHdl, parent, &tempNode, NODESIZE);

    IDXNODE.key0 = *(long *) (IDXNODE.keySpace + KEYLEN);
    a = IDXNODE.keySpace;
  } // else

  /* ---- deletes the key from the leaf node ---- */
  IDXNODE.keyCount--;
  memmove(a, a + ENTLEN, (int) (IDXNODE.keySpace + IDXNODE.keyCount * ENTLEN - a));

  while (IDXNODE.keyCount <= (MXKEYNO >> 1) && p != idxHdr.root)
  { comb = FALSE;
    z = IDXNODE.parent;

    if (IDXNODE.right)
    { y = IDXNODE.right;
      ReadRec(fileHdl, y, &tempNode, NODESIZE);
      if (tempNode.keyCount + IDXNODE.keyCount < MXKEYNO &&
	  tempNode.parent == z)
      { comb = TRUE;
	Implode(fileHdl, fileHdr, idxHdr, &IDXNODE, &tempNode);
      } // if
    } // if
    if (comb == FALSE && IDXNODE.left)
    { ReadRec(fileHdl, IDXNODE.left, &tempNode, NODESIZE);
      if (tempNode.parent == z)
	if (tempNode.keyCount + IDXNODE.keyCount < MXKEYNO)
	{ comb = TRUE;
	  Implode(fileHdl, fileHdr, idxHdr, &tempNode, &IDXNODE);
	} // if
	else
	{ Redistribute(fileHdl, &tempNode, &IDXNODE);
	  WriteRec(fileHdl, IDXNODE.left, &tempNode, NODESIZE);
	  WriteRec(fileHdl, p, &IDXNODE, NODESIZE);
	  return TRUE;
	} // else
    } // if
    if (comb == FALSE)
    { ReadRec(fileHdl, IDXNODE.right, &tempNode, NODESIZE);
      Redistribute(fileHdl, &IDXNODE, &tempNode);
      WriteRec(fileHdl, IDXNODE.right, &tempNode, NODESIZE);
      WriteRec(fileHdl, p, &IDXNODE, NODESIZE);
      return TRUE;
    } // if

    p = z;
    ReadRec(fileHdl, p, &IDXNODE, NODESIZE);
  } // while

  if (IDXNODE.keyCount == 0)
  { idxHdr.root = IDXNODE.key0;
    FreeRec(fileHdl, fileHdr, p);
    if (idxHdr.root == 0)
      idxHdr.rightLeaf = idxHdr.leftLeaf = 0;
    return TRUE;
  } // if
  WriteRec(fileHdl, p, &IDXNODE, NODESIZE);
  return TRUE;
} // DeleteKey

BOOL BTree::LocateKey		/* ---- Locate Key --CHECK THIS ROUTINE!!! - */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*key,			// key
  long		recPtr,			// record pointer
  int		(*cmp)(char *, char *)	// the compare function
)
/* Returns TRUE or FALSE, as the result of locating record with KEY associated
   with RECPTR.
*/
{ long		nodePtr;		// node pointer
  char		*keyAddr;		// the key in the node
  long		currRecPtr;		// current record pointer
  char		tempKey[256];

  BTreeInit(*idxHdr);
  if (idxHdr->root == 0)
    return 0;
  nodePtr = idxHdr->root;
  ReadRec(fileHdl, nodePtr, &IDXNODE, NODESIZE);
  if (TreeScan(fileHdl, &nodePtr, key, idxHdr->keyLen, &keyAddr, cmp) == FALSE)
    return FALSE;
  if (recPtr == CurrRecPtr(fileHdl))
    return TRUE;
  while ((currRecPtr = NextRecPtr(fileHdl)) != 0)
  { if (recPtr == currRecPtr)
      return TRUE;
    CurrKeyVal(fileHdl, tempKey);
    if (cmp(key, tempKey))
      return FALSE;
  } // while
  return FALSE;
} // LocateKey

long BTree::LocateKey		/* ---- Locate Key ------------------------ */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*key,			// key
  int		(*cmp)(char *, char *)	// the compare function
)
/* Returns the record pointer of KEY, if the key is found according to the
   compare function.  Otherwise, returns 0.
*/
{ long		nodePtr;                // node pointer
  char          *keyAddr;		// the key in the node

  BTreeInit(*idxHdr);
  if (idxHdr->root == 0)
    return 0;
  nodePtr = idxHdr->root;
  ReadRec(fileHdl, nodePtr, &IDXNODE, NODESIZE);
  if (TreeScan(fileHdl, &nodePtr, key, KEYLEN, &keyAddr, cmp) == FALSE)
    return 0;
  return CurrRecPtr(fileHdl);
} // LocateKey

void BTree::InsertKey		/* ---- Insert Key ------------------------ */
( int		fileHdl,		// file handle
  FileHdr	&fileHdr,		// file header
  IdxHdr	&idxHdr,		// index header
  char		*key,			// key
  long		recPtr			// record pointer
)
/* Inserts KEY associated with RECPTR. */
{ char		k[ODATB_MXKEYSIZE];	// the promoted key
  char          *keyAddr;               // the key in the node
  BOOL          isNonLeaf;              // is non-leaf?
  long          t, p, sv;
  long          *b;
  IdxNode       tempNode, tempNode1;    // temporary nodes

  BTreeInit(idxHdr);
  memmove(k, key, KEYLEN);
  p = 0;
  sv = 0;
  isNonLeaf = FALSE;

  /* -------- Find insertion point -------- */
  t = idxHdr.root;
  if (t)
  { ReadRec(fileHdl, t, &IDXNODE, NODESIZE);
    TreeScan(fileHdl, &t, k, idxHdr.keyLen, idxHdr.keyType, &keyAddr);
  } // if

  /* -------- Insert key into leaf node -------- */
  while (t)
  { isNonLeaf = TRUE;
    /* slides the keys to make room for insertion */
    memmove(keyAddr + ENTLEN, keyAddr, (int) (IDXNODE.keySpace + MXKEYNO * ENTLEN -
	    keyAddr));

    /* inserts key associated with record pointer */
    memmove(keyAddr, k, KEYLEN);
    *(long *) (keyAddr + KEYLEN) = recPtr;

    IDXNODE.keyCount++;
    if (IDXNODE.keyCount <= MXKEYNO)
    { WriteRec(fileHdl, t, &IDXNODE, NODESIZE);
      return;
    } // if

    /* -------- redistributes keys between sibling nodes -------- */
    if (IDXNODE.left)
    { ReadRec(fileHdl, IDXNODE.left, &tempNode, NODESIZE);
      if (tempNode.keyCount < MXKEYNO && tempNode.parent == IDXNODE.parent)
      { Redistribute(fileHdl, &tempNode, &IDXNODE);
	WriteRec(fileHdl, IDXNODE.left, &tempNode, NODESIZE);
	WriteRec(fileHdl, t, &IDXNODE, NODESIZE);
	return;
      } // if
    } // if
    if (IDXNODE.right)
    { ReadRec(fileHdl, IDXNODE.right, &tempNode, NODESIZE);
      if (tempNode.keyCount < MXKEYNO && tempNode.parent == IDXNODE.parent)
      { Redistribute(fileHdl, &IDXNODE, &tempNode);
	WriteRec(fileHdl, IDXNODE.right, &tempNode, NODESIZE);
	WriteRec(fileHdl, t, &IDXNODE, NODESIZE);
	return;
      } // if
    } // if

    /* -------- splits node -------- */
    p = AllocRec(fileHdl, fileHdr, NODESIZE, RF_NODE);
    memset(&tempNode, '\0', NODESIZE);

    IDXNODE.keyCount = (MXKEYNO + 1) >> 1;
    keyAddr = IDXNODE.keySpace + (IDXNODE.keyCount + 1) * ENTLEN;

    memmove(k, keyAddr - ENTLEN, KEYLEN);                  // the promoted key
    tempNode.key0 = *(long *) (keyAddr - sizeof(long)); // the record pointer
    tempNode.nonLeaf = IDXNODE.nonLeaf;
    tempNode.keyCount = MXKEYNO - IDXNODE.keyCount;
    tempNode.left = t;
    tempNode.right = IDXNODE.right;
    memmove(tempNode.keySpace, keyAddr, tempNode.keyCount * ENTLEN);

    IDXNODE.right = p;
    if (idxHdr.rightLeaf == t)
      idxHdr.rightLeaf = p;

    if (t == CRECPTR && CKEYNO > IDXNODE.keyCount)
    { CRECPTR = p;
      CKEYNO -= IDXNODE.keyCount + 1;
    } // if

    recPtr = p; // new node's disk location
    sv = t; // current node's disk location
    t = IDXNODE.parent; // parent node
    if (t)
      tempNode.parent = t; // if the current node is not the root, assign the new node to the same parent
    else
    { p = AllocRec(fileHdl, fileHdr, NODESIZE, RF_NODE); // if the current node was the root, create a new root node
      IDXNODE.parent = p;
      tempNode.parent = p;
    } // else
    WriteRec(fileHdl, recPtr, &tempNode, NODESIZE); // save the new node to disk

    if (tempNode.right)  // if the new node has a right sibling
    { ReadRec(fileHdl, tempNode.right, &tempNode1, NODESIZE); // read the right sibling of the new node
      tempNode1.left = recPtr; // set the left sibling address pointer to the new node
      WriteRec(fileHdl, tempNode.right, &tempNode1, NODESIZE); // save the right sibling to disk
    } // if
    if (tempNode.nonLeaf) // if the new node is not a leaf, update the parent pointers in its children
      Adopt(fileHdl, &tempNode.key0, tempNode.keyCount + 1, recPtr);
    WriteRec(fileHdl, sv, &IDXNODE, NODESIZE); // save the current node to disk
    if (t)      // if a parent exists, and a new node was created, set up parameters to insert a new key into the parent node
    { ReadRec(fileHdl, t, &IDXNODE, NODESIZE);
      keyAddr = IDXNODE.keySpace;
      b = &IDXNODE.key0;
      while (*b != tempNode.left)       // find the insertion point
      { keyAddr += ENTLEN;
	b = (long *) (keyAddr - sizeof(long));
      } // while
    } // if
  } // while

  /* -------- new root -------- */
  if (!p)
    p = AllocRec(fileHdl, fileHdr, NODESIZE, RF_NODE);
  tempNode.nonLeaf = isNonLeaf;
  tempNode.keyCount = 1;
  tempNode.parent = tempNode.left = tempNode.right = 0;
  tempNode.key0 = sv;
  memmove(tempNode.keySpace, k, KEYLEN);
  *(long *) (tempNode.keySpace + KEYLEN) = recPtr;
  WriteRec(fileHdl, p, &tempNode, NODESIZE);

  idxHdr.root = p;                      // new index root
  if (isNonLeaf == FALSE)
  { idxHdr.leftLeaf = idxHdr.rightLeaf = p;
    CRECPTR = p;
    CKEYNO = 1;
  } // if
  return;
} // InsertKey

long BTree::LocateNext		/* ---- Locate Next ----------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*keyVal,		// key value
  long		recPtr			// record pointer
)
/* Returns record pointer of the next sequential record of KEYVAL
   associated with RECPTR, or returns 0.
*/
{
  BTreeInit(*idxHdr);
  if (idxHdr->root == 0)			// an empty tree
    return 0;
  if (recPtr == 0)
    return FirstKey(fileHdl, idxHdr);
  if (LocateKey(fileHdl, idxHdr, keyVal, recPtr,
      GetCmpExactFunc(idxHdr->keyType)))
    return NextRecPtr(fileHdl);
  return CurrRecPtr(fileHdl);
} // LocateNext

long BTree::LocatePrev		/* ---- Locate Previous ------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*keyVal,		// key value
  long		recPtr			// record pointer
)
/* Returns record pointer of the previous sequential record of KEYVAL
   associated with RECPTR, or returns 0.
*/
{
  BTreeInit(*idxHdr);
  if (idxHdr->root == 0)		// an empty tree
    return 0;
  if (recPtr == 0)
    return LastKey(fileHdl, idxHdr);
  LocateKey(fileHdl, idxHdr, keyVal, recPtr, GetCmpExactFunc(idxHdr->keyType));
  return PrevRecPtr(fileHdl);
} // LocatePrev

long BTree::LocateNextKey	/* ---- Locate Next Key ------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*keyVal,		// key value
  int		(*cmp)(char *, char *)	// the compare function
)
/* Returns the next sequential record pointer, if the keys are match
   according to the compare function.  Otherwise, returns 0.
*/
{ long		parent;			// parent node pointer
  IdxNode	parentNode;		// parent node
  char		*keyAddr;		// the key in the node
  long		kid;			// kid record pointer
  int		entLen;			// length (key + record pointer)

  if (CRECPTR == 0)
    return 0;
  entLen = idxHdr->keyLen + sizeof(long);

  if (CRECPTR == idxHdr->leftLeaf)
    if (CKEYNO == IDXNODE.keyCount)
      if (IDXNODE.right == 0)
	return 0;
      else
      { CRECPTR = IDXNODE.right;
	ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
	CKEYNO = -1;
      } // else
    else
    { CKEYNO++;
      if ((*cmp)(keyVal, IDXNODE.keySpace + (CKEYNO - 1) * entLen) == 0)
	return *(long *) (IDXNODE.keySpace + CKEYNO * entLen - sizeof(long));
      return 0;
    } // else

  if (CKEYNO == IDXNODE.keyCount)
  { if (IDXNODE.right == 0)
      return 0;
    CRECPTR = IDXNODE.right;
    CKEYNO = 0;
    ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
  } // if
  else
    CKEYNO++;
  if (CKEYNO == 0)
    if ((*cmp)(keyVal, IDXNODE.keySpace) == 0)
      return IDXNODE.key0;
    else
    { kid = CRECPTR;
      if ((parent = IDXNODE.parent) == 0)
	return 0;
      while ((keyAddr = ChildPtr(fileHdl, kid, parent, &parentNode)) ==
	     parentNode.keySpace)
      { kid = parent;
	if ((parent = parentNode.parent) == 0)
	  return 0;
      } // while
      if ((*cmp)(keyVal, keyAddr - entLen) == 0)
	return IDXNODE.key0;
      return 0;
    } // else
  if ((*cmp)(keyVal, IDXNODE.keySpace + (CKEYNO - 1) * (idxHdr->keyLen +
	     sizeof(long))) == 0)
    return *(long *) (IDXNODE.keySpace + CKEYNO *
		      (idxHdr->keyLen + sizeof(long)) - sizeof(long));
  return 0;
} // LocateNextKey

long BTree::LocateRecIdx	/* ---- Locate Record Index --------------- */
( int		fileHdl,                // file handle
  IdxHdr	*idxHdr,		// index header
  long		recIdx			// record index
)
/* Returns record pointer of the record associated with RECIDX, or 0. */
{
  if (recIdx <= 0 || idxHdr->leftLeaf == 0)
    return 0;
  BTreeInit(*idxHdr);
  CRECPTR = idxHdr->leftLeaf;
  ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
  if (recIdx <= IDXNODE.keyCount)
  { CKEYNO = (int) recIdx;
    return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
  } // if
  if (IDXNODE.right == 0)
    return 0;
  recIdx -= IDXNODE.keyCount;
  CRECPTR = IDXNODE.right;
  ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
  while (recIdx > IDXNODE.keyCount + 1)
  { if (IDXNODE.right == 0)
      return 0;
    recIdx -= IDXNODE.keyCount + 1;
    CRECPTR = IDXNODE.right;
    ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
  } // while
  CKEYNO = (int) (recIdx - 1);
  return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
} // LocateRecIdx

long BTree::LocateRecID         /* ---- Locate Record ID ------------------ */
( int           fileHdl,                // file handle
  IdxHdr        *idxHdr,                // index header
  long          recID                   // record ID
)
/* Returns record index of the record associated with RECORDID, or 0. */
{ long          recIdx;                 // record index
  int		entLen;			// length (key + record pointer)
  IdxNode	idxNode;		// index node
  int		curKeyNo;		// current key number
  int		(*cmp)(char *, char *);	// the compare function

  if (idxHdr->leftLeaf == 0)
    return 0;
  cmp = GetCmpExactFunc(ODATB_KEYLONG);
  recIdx = 0;
  entLen = idxHdr->keyLen + sizeof(long);
  ReadRec(fileHdl, idxHdr->leftLeaf, &idxNode, NODESIZE);
  curKeyNo = 0;
  do
  { if (curKeyNo == idxNode.keyCount)
    { if (idxNode.right == 0)
	return 0;
      ReadRec(fileHdl, idxNode.right, &idxNode, NODESIZE);
      curKeyNo = 0;
    } // if
    else
      curKeyNo++;
    recIdx++;
  } while ((*cmp)((char *) &recID, idxNode.keySpace + curKeyNo * entLen -
		  sizeof(long)));
  return recIdx;
} // LocateRecID

long BTree::LocateRecID0	/* ---- Locate Record ID based on tree 0 -- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  long		recID			// record ID
)
/* Returns record index of the record associated with RECORDID, or 0.
   Based on index header 0.
*/
{ long		recIdx;			// record index

  if (!LocateKey(fileHdl, idxHdr, (char *) &recID,
		 GetCmpExactFunc(idxHdr->keyType)))
    return 0;
  recIdx = 1;
  while (PrevRecPtr(fileHdl))
    recIdx++;
  return recIdx;
} // LocateRecID0

long BTree::NextKey		/* ---- Next Sequential Key --------------- */
( int		fileHdl			// file handle
)
/* Returns record pointer of the next sequential record, or 0. */
{
  if (CRECPTR == 0)
    return 0;
  if (CKEYNO == IDXNODE.keyCount)
  { if (IDXNODE.right == 0)
      return 0;
    ReadRec(fileHdl, IDXNODE.right, &IDXNODE, NODESIZE);
    CRECPTR = IDXNODE.right;
    CKEYNO = 0;
  } // if
  else
    CKEYNO++;
  return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
} // NextKey

long BTree::CurrRecPtr		/* ---- Current Record Pointer ------------ */
( int		fileHdl			// file handle
)
/* Returns the current record pointer, or returns 0. */
{
  if (CRECPTR == 0)
    return 0;
  if (CKEYNO < 0)
  { if (IDXNODE.left == 0)
      return 0;
    CRECPTR = IDXNODE.left;
    ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
    CKEYNO = IDXNODE.keyCount;
  } // if
  else
    if (CKEYNO > IDXNODE.keyCount)
    { if (IDXNODE.right == 0)
	return 0;
      CRECPTR = IDXNODE.right;
      ReadRec(fileHdl, CRECPTR, &IDXNODE, NODESIZE);
      CKEYNO = 0;
    } // if
  return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
} // CurrRecPtr

long BTree::FirstKey		/* ---- First Key ------------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr			// index header
)
/* Returns record pointer of the first record, or returns 0. */
{
  if (idxHdr->leftLeaf == 0)
    return 0;
  BTreeInit(*idxHdr);
  ReadRec(fileHdl, idxHdr->leftLeaf, &IDXNODE, NODESIZE);
  CRECPTR = idxHdr->leftLeaf;
  CKEYNO = 1;
  return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
} // FirstKey

long BTree::LastKey		/* ---- Last Key -------------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr			// index header
)
/* Returns record pointer of the last record, or returns 0. */
{
  if (idxHdr->rightLeaf == 0)
    return 0;
  BTreeInit(*idxHdr);
  ReadRec(fileHdl, idxHdr->rightLeaf, &IDXNODE, NODESIZE);
  CRECPTR = idxHdr->rightLeaf;
  CKEYNO = IDXNODE.keyCount;
  return *(long *) (IDXNODE.keySpace + CKEYNO * ENTLEN - sizeof(long));
} // LastKey

long BTree::NextRecPtr		/* ---- Next Sequential Record Pointer ---- */
( int		fileHdl
)
/* Returns record pointer of the next sequential record, or returns 0. */
{
  CKEYNO++;
  return CurrRecPtr(fileHdl);
} // NextRecPtr

long BTree::PrevRecPtr		/* ---- Previous Sequential Record Pointer  */
( int		fileHdl			// file handle
)
/* Returns record pointer of the previous sequential record, or returns 0. */
{
  CKEYNO--;
  return CurrRecPtr(fileHdl);
} // PrevRecPtr

long BTree::LocateFrontRelN	/* ---- Locate Front Relative N ----------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  long		n			// relative position n
)
/* Returns record pointer of the record, relative N position to the first
   record.
*/
{
  if (idxHdr->root == 0)		// nothing to read or an empty tree
    return 0;

  if (n == 0)
    return FirstKey(fileHdl, idxHdr);

  if (n < 0)
    n = -n;				// absolute value

  FirstKey(fileHdl, idxHdr);
  while (--n && NextRecPtr(fileHdl))
    /* Do Nothing */;

  return NextRecPtr(fileHdl);
} // LocateFrontRelN

long BTree::LocateRearRelN	/* ---- Locate Rear Relative N ------------ */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  long		n			// relative position n
)
/* Returns record pointer of the record, relative N position to the rear
   record.
*/
{
  if (n == 0 || idxHdr->root == 0)	// nothing to read or an empty tree
    return 0;

  if (n < 0)
    n = -n;				// absolute value

  if (n == 1)
    return LastKey(fileHdl, idxHdr);

  LastKey(fileHdl, idxHdr);
  while (--n && PrevRecPtr(fileHdl))
    /* Do Nothing */;

  return PrevRecPtr(fileHdl);
} // LocateRearRelN

long BTree::LocateRelN		/* ---- Locate Next ----------------------- */
( int		fileHdl,		// file handle
  IdxHdr	*idxHdr,		// index header
  char		*keyVal,		// key value
  long		recPtr,			// record pointer
  long		n			// relative position n
)
/* Returns record pointer of the record, relative N position to the current
   key KEYVAL associated with RECPTR, or returns 0.
*/
{
  BTreeInit(*idxHdr);
  if (idxHdr->root == 0)		// an empty tree
    return 0;
  if (recPtr == 0)
    return n <= 0? FirstKey(fileHdl, idxHdr) : LastKey(fileHdl, idxHdr);

  if (LocateKey(fileHdl, idxHdr, keyVal, recPtr,
      GetCmpExactFunc(idxHdr->keyType)))
  { if (n == 0)
      return CurrRecPtr(fileHdl);
  } // if
  else
  { if (n == 0)
      return 0;
    if (--n == 0)
      return CurrRecPtr(fileHdl);
  } // else

  if (n > 0)
  { while (--n && NextRecPtr(fileHdl))
      /* Do Nothing */;
    return NextRecPtr(fileHdl);
  } // if

  while (++n && PrevRecPtr(fileHdl))
    /* Do Nothing */;
  return PrevRecPtr(fileHdl);
} // LocateRelN
