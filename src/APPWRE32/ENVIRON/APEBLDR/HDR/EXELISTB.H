//======================================================================
// exelistb.h -- EXELISTBASE is the base class for runtime lists, eg,
//
//                 EXEEVENTLIST
//                 EXERESPONSELIST
//                 EXELIBLIST
//                 EXEENTRYPOINTLIST
//
// A runtime list has homogeneous elements.  It is implemented
// as an array stuffed into a SERIUSBLOCK that is grown in increments
// of a hundred elements.
//
// A runtime list may be sortable.  If it is sortable, it is either
// sorted or unsorted.  Test with the IsSortable() and IsSorted()
// methods.  The FindNode method will use binary search if the
// list is sorted, else li search.
//
// All runtime lists are stored in a RECORDBLOCK that is pointed at
// by the SERUID in the exeListsBlock offset of the EXEHEAD record.
//
// jon 12-Jun-92 created
//======================================================================

#ifndef __EXELISTB_H
#define __EXELISTB_H

#include "exserblk.h"
#include "exrecblk.h"
#include "recblk.h"
#include "rtreclib.h"

class EXELISTBASE
  {

protected:

  RUNTIMEUID   blockID;      // the serius block containing the list
  LPBYTE       dataPtr;      // data address of that node list
  LONG         nodeSize;     // how big is a node on this list
  LONG         numNodes;     // count of nodes currently in the list
  LONG         maxNodes;     // # of nodes before we need to grow the block
  LONG         growthFactor; // #nodes to reserve space for when we grow
  BOOL         isSortable;   // Can the nodes on this list be sorted?
  BOOL         isSorted;     // Are the nodes in ascending sorted order?

  LPVOID FindNode(LONG  theKey);
  LPVOID FindNode(LPCSTR theKey);

 // returns the key value (a long) for the specified node, by index
  virtual LONG GetNodeKey(
    LONG theNodeIdx)
    = 0;

 // returns the key value (a long) for the specified node, by pointer
  virtual LONG GetNodeKey(
    LPVOID theNodePtr)
    = 0;

 // returns the key value (a string) for the specified node, by index
  virtual LPCSTR GetNodeStringKey(
    LONG theNodeIdx)
    {
    return 0;
		}

 // returns the key value (a string) for the specified node, by pointer
  virtual LPCSTR GetNodeStringKey(
    LPVOID /*-- theNodePtr --*/)
    {
    return 0;
    }

 // returns the memory address for the specified node
  LPVOID GetNodeAddr(
    LONG nodeIdx)
    {
    return dataPtr + ((int)(nodeIdx * nodeSize));
    }

private:

  void  *operator new(
    size_t /*-- theSize --*/)
    {
    return 0;
    }

  EXELISTBASE();  // disallow default constructor

  LPBYTE FreshenDataPtr()
    {
    return dataPtr =
      (LPBYTE)(((LPEXESERIUSBLOCK)GetObject(blockID))->GetData());
    }

public:

 // for exelists that are constructed in the exeHead ctor
  void  * operator new(
    size_t      theSize,
    RUNTIMEUID  exeListBlockUID)
    {
    return ((LPEXERECORDBLOCK)GetObject(exeListBlockUID))
             ->AddRecord(theSize);
    }

  void  * operator new(
    size_t    theSize,
    void  *objPtr)
    {
    return objPtr;
    }

  void operator delete(
    void  *objPtr,
    size_t    size)
    {
	  ((LPRECORDBLOCK)GetObject(((RECORDPREFIX *)objPtr - 1)->recBlockUID))
      ->DeleteRecord(objPtr, size);
    }

  EXELISTBASE(
    LONG theNodeSize);

  EXELISTBASE(
    LPVOID objPtr)
    {
    FreshenDataPtr();
    }

  ~EXELISTBASE();

  BOOL AddNode(
    LPVOID theNode);

  BOOL PackAndSortNodes();

  LONG GetNumNodes()
    {
    return numNodes;
    }

  LPVOID IdxGetNode(
    LONG theIdx)
    {
    return theIdx >= 0 && theIdx < numNodes
             ? dataPtr + ((int)(theIdx * nodeSize))
             : 0;
    }

  LPVOID GetNewNodeAddr();

  };

#endif // __EXELISTB_H
