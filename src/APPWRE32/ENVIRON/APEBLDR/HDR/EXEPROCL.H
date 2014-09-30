//======================================================================
// exeprocl.h -- runtime DLL proc list
//
// jon 12-Jun-92 created
//======================================================================

#ifndef __EXEPROCL_H
#define __EXEPROCL_H

#include "classdef.h"
#include "exeproc.h"
#include "exelistb.h"
#include "exehead.h"

class EXEPROCLIST : public EXELISTBASE
  {

public:

  void  * operator new(
	  size_t  size)
	  {

	  return ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
     ->GetExeProcListBlock()->AddRecord(size);
	  }

  void  * operator new(
	  size_t    size,
    void  *objPtr)
	  {
	  return objPtr;
	  }

  void operator delete(
    void  *objPtr,
	  size_t    size)
	  {
	  ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
     ->GetExeProcListBlock()->DeleteRecord(objPtr, size);
	  }

  EXEPROCLIST()

    : EXELISTBASE(sizeof(EXEPROC))

    {
    isSortable = FALSE;
    }

  EXEPROCLIST(
    void  *objPtr)

    : EXELISTBASE(objPtr)

    {
    isSortable = FALSE;
    }

  ~EXEPROCLIST();

  LPEXEPROC FindNode(
    LPCSTR theProcName)
    {
    return (LPEXEPROC)EXELISTBASE::FindNode(theProcName);
    }

  LPEXEPROC GetNodeFor(
    LPCSTR theProcName)
    {
    LPEXEPROC  myNode, primerNode;

    myNode = this->FindNode(theProcName);
    if (!myNode)
      {
      myNode = (LPEXEPROC)(this->GetNewNodeAddr());
      myNode->Init();
      myNode->SetName(theProcName);
      }

    return myNode;
    }

private:

  LONG GetNodeKey(
    LONG /*-- theNodeIdx --*/)
    {
    MessageBox(NULL, "A proc list doesn't have numeric keys...", "ERROR from exeProcList::GetNodeKey(LONG)", MB_OK|MB_TASKMODAL);
    return 0;
    }

  LONG GetNodeKey(
    LPVOID /*-- theNodePtr --*/)
    {
    MessageBox(NULL, "A proc list doesn't have numeric keys...", "ERROR from exeProcList::GetNodeKey(LPVOID)", MB_OK|MB_TASKMODAL);
    return 0;
    }

  LPCSTR GetNodeStringKey(
    LONG theNodeIdx)
    {
    return (theNodeIdx < 0 || theNodeIdx >= numNodes)
             ? NULL
             : (LPCSTR)((LPEXEPROC)dataPtr + (int)theNodeIdx)->name;
    }

  LPCSTR GetNodeStringKey(
    LPVOID theNodePtr)
    {
    return ((LPEXEPROC)theNodePtr)->name;
    }

  };

#endif  // __EXEPROCL_H
