//======================================================================
// exelibl.cpp -- runtime library (DLL) list
//
// jon 12-Jun-92 created
//======================================================================

  #include "almsys.h"

  #include "exelibl.h"

EXELIBLIST::~EXELIBLIST()
  {
  LPEXELIB   exeLib;
  long       i;

  for (i = 0 ; 0 != (exeLib = (LPEXELIB)this->IdxGetNode(i)) ; i++)
    {
    delete exeLib;
    }
  }

// -- end exelibl.cpp
