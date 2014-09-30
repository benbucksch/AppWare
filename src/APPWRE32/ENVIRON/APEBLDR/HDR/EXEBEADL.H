//======================================================================
// exebeadl.h -- pool of binding beads that need proc addresses filled
//               in from DLLs at app startup time.  EXEFUNCINSTs,
//               EXEOBJINST Admn routines, and responses all have beads.
//
// jon  8-Jul-92 created
// jon 19-Oct-92 reworked from EXEPROCUIDLIST to EXEBEADLIST
//======================================================================

#ifndef __EXEBEADL_H
#define __EXEBEADL_H

#include "exebead.h"
#include "exelistb.h"

class EXEBEADLIST:public EXELISTBASE
  {

public:

  EXEBEADLIST()
    : EXELISTBASE(sizeof(EXEBEAD))
    {
    isSortable = FALSE;
    }

  EXEBEADLIST(
    void  *  objPtr)
    : EXELISTBASE(objPtr)
    {
    isSortable = FALSE;
    }

  LPEXEBEAD IdxGetNode(
    LONG theIdx)
    {
    return (LPEXEBEAD)EXELISTBASE::IdxGetNode(theIdx);
    }

private:
  LONG GetNodeKey(
    LONG /*-- theNodeIdx --*/)
    {
    complainAboutNumericKey();
    return 0;
    }

  LONG GetNodeKey(
    LPVOID /*-- theNodePtr --*/)
    {
    complainAboutNumericKey();
    return 0;
    }

  void complainAboutNumericKey()
    {
    MessageBox(NULL,
      "A bead list doesn't have numeric keys...",
      "ERROR from EXEBEADLIST::GetNodeKey()",
      MB_OK|MB_TASKMODAL);
    }

  };

#endif  // __EXEBEADL_H
