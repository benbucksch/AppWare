//======================================================================
// exelistb.cpp -- EXELISTBASE is a base class for the runtime event
//                and response lists
//
// jon 12-Jun-92 created
// jon 13-Oct-92 ooops. binary searches were using an upperbound of
//               numNodes not numNodes-1.
//======================================================================

	#include "almsys.h"

  #include "exelistb.h"

  #define NELEMS(ary) (sizeof(ary) / sizeof(ary[0]))

//----------------------------------------------------------------------
// EXELISTBASE constructor
//
// Initalize the list.
//----------------------------------------------------------------------
EXELISTBASE::EXELISTBASE(
  LONG theNodeSize)
  {
  blockID.id = NULLSERUID; // no block yet..
  dataPtr = 0;             // ..so there's no data
  nodeSize = theNodeSize;  // how big is a node on this list
  numNodes = 0;            // count of nodes in the list
  maxNodes = 0;            // max nodes before we need to grow the block
  growthFactor = 100;      // #nodes to reserve space for when we grow
  isSorted = FALSE;        // nodes are not in sorted order
  isSortable = FALSE;      // DEFAULT: nodes may not be sorted, override
                           // as necessary in descendant's constructor
  }

//----------------------------------------------------------------------
// ~EXELISTBASE destructor
//
// Free the block!
//----------------------------------------------------------------------
EXELISTBASE::~EXELISTBASE()
  {
  // lose the data block
  if (blockID.id != NULLSERUID)
    {
    delete (LPEXESERIUSBLOCK) GetObject(blockID);
    }
  }

//----------------------------------------------------------------------
// EXELISTBASE::GetNewNodeAddr
//
// Reserves space for a new node at the end of the list and returns a
// pointer to it.  Returns 0 on error.
//----------------------------------------------------------------------
LPVOID EXELISTBASE::GetNewNodeAddr()
  {
  LPBYTE  pNode;

  // Grow the block if required
  if (numNodes == maxNodes)
    {
    maxNodes = maxNodes + growthFactor;
    if (blockID.id == NULLSERUID)
      {
      blockID = GetUID(new EXESERIUSBLOCK(nodeSize * maxNodes));
      }
    else
      {
      ((LPEXESERIUSBLOCK) GetObject(blockID))
        -> ResizeBlock(nodeSize * (maxNodes - numNodes));
      }
    FreshenDataPtr();
    }

  pNode = dataPtr + (int)(numNodes * nodeSize);
  numNodes++;
  return pNode;
  }

//----------------------------------------------------------------------
// EXELISTBASE::AddNode
//
// Adds a node to the list.  Returns FALSE on error.
//----------------------------------------------------------------------
BOOL EXELISTBASE::AddNode(LPVOID theNode)
  {
  LPVOID  putItHere;

  // get the address of a new list node
  putItHere = GetNewNodeAddr();
  if (!putItHere)
    return FALSE;

 // put the new node away
  isSorted = FALSE;
  CopyMemory(putItHere, theNode, (int)nodeSize);
  return TRUE;
  }

//----------------------------------------------------------------------
// EXELISTBASE::PackAndSortNodes
//
// Packs the list into a EXESERIUSBLOCK that is just the right size.
// If this is a sortable list, it is sorted before packing.  Returns
// TRUE if the array was packed successfully.  The sort is based on
// Robert Sedgewick "Algorithms", p 97-99.  See also Knuth, Vol. 2,
// Section 5.2.1, pp. 84-95.
//----------------------------------------------------------------------
BOOL EXELISTBASE::PackAndSortNodes()
  {
  LONG        idx;
  LPBYTE  *ptrs;
  LPBYTE      pNodeNow;
  LONG        upperRung;
  LONG        lowerRung;
  LONG        span;
  RUNTIMEUID  oldUID;

  // -- build up <ptrs>, the pointer array
  ptrs = (LPBYTE  *)new LPBYTE[(int)numNodes];
  ptrs[0] = dataPtr;
  for (idx = 1 ; idx < numNodes ; idx++)
    {
    ptrs[(int)idx] = ptrs[(int)(idx - 1)] + (int)nodeSize;
    }

  if (numNodes <= 0 || !isSortable)
    goto skipSort;

  //---------------------------------------------------------------------
  // set the first span for the shellsort using these rules:
  //
  //   [1] Let h(1) = 1, h(s+1) = 3 * h(s) + 1 [ie: 1,4,13,40,121...]
  //   [2] Set span = h(t) where h(t+2) >= numNodes
  //
  // (per Knuth, Vol 2, Sec 5.2.1, item (8), p 95)
  //---------------------------------------------------------------------
  span = 1;
  if (numNodes > 13)
    {
    while (span < numNodes)
      span = (3 * span) + 1;
    span = span / 9;
    }

  // -- shellsort it
  for ( ; span > 0 ; span /= 3)
    {
    for (upperRung = span ; upperRung < numNodes ; upperRung++)
      {
      pNodeNow = ptrs[(int)upperRung];

      lowerRung = upperRung - span;
      while (lowerRung >= 0
               &&
             GetNodeKey(ptrs[(int)lowerRung]) > GetNodeKey(pNodeNow))
        {
        ptrs[(int)(lowerRung + span)] = ptrs[(int)lowerRung];
        lowerRung -= span;
        }

      ptrs[(int)(lowerRung + span)] = pNodeNow;
      }
    }

  skipSort:

  // remember the UID of the old (possibly unsorted) block
  oldUID = blockID;

  // allocate a new block for the sorted elements ..
  blockID = GetUID(new EXESERIUSBLOCK(nodeSize * numNodes));

  pNodeNow = FreshenDataPtr();
  maxNodes = numNodes;

  // .. and fill it in
  for (idx = 0 ; idx < numNodes ; idx++)
    {
    CopyMemory(pNodeNow, ptrs[(int)idx], (int)nodeSize);
    pNodeNow += (int)nodeSize;
    }

  // don't need the pointers anymore
  delete[] ptrs;

  // lose the old unsorted block
  delete (LPEXESERIUSBLOCK) GetObject(oldUID);

  // clean up and go home
  if (isSortable)
    isSorted = TRUE;

  return TRUE;
  }

//----------------------------------------------------------------------
// EXELISTBASE::FindNode.1 (for LONG keys)
//
// Returns the address of the node associated with the specified
// (LONG) key.
//----------------------------------------------------------------------
LPVOID EXELISTBASE::FindNode(
  LONG theKey)
  {
  if (isSorted)  // binary search
    {
    LONG    pivot;
    LONG    lower = 0;
    LONG    upper = numNodes - 1;
    LONG    keyNow;

    while (lower <= upper)
      {
      pivot = (lower + upper) >> 1;

      keyNow = GetNodeKey(pivot);
      if (theKey < keyNow)
        upper = pivot - 1;
      else if (theKey > keyNow)
        lower = pivot + 1;
      else
        return GetNodeAddr(pivot);
      }
    }
  else           // li search
    {
    LONG idx;

    for (idx = 0 ; idx < numNodes ; idx++)
      if (theKey == GetNodeKey(idx))
        return GetNodeAddr(idx);
    }

  return 0;
  }

//----------------------------------------------------------------------
// EXELISTBASE::FindNode.2 (for string keys)
//
// Returns the address of the node associated with the specified
// (LPSTR) key.
//----------------------------------------------------------------------
LPVOID EXELISTBASE::FindNode(LPCSTR theKey)
  {
  if (isSorted)  // binary search
    {
    LONG    pivot;
    LONG    lower = 0;
    LONG    upper = numNodes - 1;
    LONG    keyComp;

    while (lower <= upper)
      {
      pivot = (lower + upper) >> 1;

      keyComp = lstrcmpi(theKey, GetNodeStringKey(pivot));
      if (keyComp < 0)         // theKey is less than this node's key
        upper = pivot - 1;
      else if (keyComp > 0)    // theKey is greater than this node's key
        lower = pivot + 1;
      else
        return GetNodeAddr(pivot);  // we found it!
      }
    }
  else           // li search
    {
    LONG idx;

    for (idx = 0 ; idx < numNodes ; idx++)
      if (lstrcmpi(theKey, GetNodeStringKey(idx)) == 0)
        return GetNodeAddr(idx);
    }

  return 0;
  }
