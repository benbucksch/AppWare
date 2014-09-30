//======================================================================
// exelibl.h -- runtime library (DLL) list
//
// jon 12-Jun-92 created
//======================================================================

#ifndef __EXELIBL_H
#define __EXELIBL_H

#include "classdef.h"
#include "exelib.h"
#include "exelistb.h"
#include "exehead.h"

class EXELIBLIST:public EXELISTBASE
  {

private:

  LONG GetNodeKey(
    LONG /*-- theNodeIdx --*/)
    {
    MessageBox(NULL, "A lib list doesn't have numeric keys...", "ERROR from exeLibList::GetNodeKey(LONG)", MB_OK);
    return 0;
    }

  LONG GetNodeKey(
    LPVOID /*-- theNodePtr --*/)
    {
    MessageBox(NULL, "A lib list doesn't have numeric keys...", "ERROR from exeLibList::GetNodeKey(LPVOID)", MB_OK);
    return 0;
    }

  LPCSTR GetNodeStringKey(
    LONG theNodeIdx)
    {
    return (theNodeIdx < 0 || theNodeIdx >= numNodes)
      ? NULL
      : (LPCSTR)((LPEXELIB)dataPtr + (int)theNodeIdx)->name;
    }

  LPCSTR GetNodeStringKey(
    LPVOID theNodePtr)
    {
    return ((LPEXELIB)theNodePtr)->name;
    }

public:

  EXELIBLIST()
    : EXELISTBASE(sizeof(EXELIB))
    {
    isSortable = FALSE;
    }

  EXELIBLIST(
    void  * objPtr)
    : EXELISTBASE(objPtr)
    {
    }

  ~EXELIBLIST();

  //--------------------------------------------------------------------
  //    EXELIBLIST::BindUIDToExeProc (static)
  //
  //  Creates an association between the kernel object identified by
  //  theUID and the procName in the DLL libName.
  //--------------------------------------------------------------------
  static BOOL BindUIDToExeProc(
    SERUID theUID,
    LONG   trash,     // *** TRASH! *** was "userParm"
    LPCSTR  libName,
    LPCSTR  procName)
    {
    //
    // 1. get the global file object
    // 2. get the headExeRec
    // 3. get the list of runtime library descriptors
    // 4. get (or allocate a new) node for the specified library
    // 5. get that library's list of procs
    // 6. get (or allocate a new) node for the specified proc
    // 7. let proc know this UID wants the address of the proc at runtime
    //
    return ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
              ->GetExeLibList()
              ->GetNodeFor(libName)
              ->GetProcList()
              ->GetNodeFor(procName)
              ->AddBeadFor(theUID);
    }

  //--------------------------------------------------------------------
  //    EXELIBLIST::BindEventToExeProc (static)
  //
  //  Creates an association between theEvent/theType and
  //  the procName in the DLL libName.
  //--------------------------------------------------------------------
  static BOOL BindEventToExeProc(
    AEVENTID theEvent,
    ATYPEID  theType,
    LPCSTR   libName,
    LPCSTR   procName)
    {
    //
    // 1. get the global file object
    // 2. get the headExeRec
    // 3. get the list of runtime library descriptors
    // 4. get (or allocate a new) node for the specified library
    // 5. get that library's list of procs
    // 6. get (or allocate a new) node for the specified proc
    // 7. let proc know this event/type pair wants the address of
    //    the proc at runtime
    //
    return ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
              ->GetExeLibList()
              ->GetNodeFor(libName)
              ->GetProcList()
              ->GetNodeFor(procName)
              ->AddBeadFor(theType, theEvent);
    }

  LPEXEPROCLIST GetProcList(
    LONG theLibIdx)
    {
    return (theLibIdx < 0 || theLibIdx >= numNodes)
      ? 0
      : ((LPEXELIB)dataPtr + (int)theLibIdx)->GetProcList();
    }

  LPEXELIB FindNode(
    LPCSTR theLibName)
    {
    return (LPEXELIB)EXELISTBASE::FindNode(theLibName);
    }

  LPEXELIB GetNodeFor(
    LPCSTR theLibName)
    {
    LPEXELIB  myNode;

    myNode = this->FindNode(theLibName);
    if (!myNode)
      {
      myNode = (LPEXELIB)(this->GetNewNodeAddr());
      myNode->Init(theLibName);
      }

    return myNode;
    }

  };

#endif  // __EXELIBL_H
