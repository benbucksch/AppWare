//======================================================================
// exeprocl.cpp -- runtime proc list (DLL entry points)
//
// jon  9-Jul-92 created
// jon 28-Sep-92 code for dtor.
//======================================================================

#include "almsys.h"

#include "sereclib.h"
#include "exeprocl.h"

  EXEPROCLIST::~EXEPROCLIST()
    {
    LPEXEPROC  exeProc;
    long       i;

    for (i = 0 ; 0 != (exeProc = (LPEXEPROC)this->IdxGetNode(i)) ; i++)
      {
      delete exeProc;
      }

    }

// end of exeprocl.cpp
