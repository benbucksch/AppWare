#include "almsys.h"
#include "rtreclst.h"
#include "rtrec.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PurgeList
//
// CLASS.......:  RUNTIMERECLIST
//
// DESCRIPTION.:  Delete all the records in the list.
//
/////////////////////////////////////////////////////////////////////////////
void RUNTIMERECLIST::PurgeList(
  void)
  {
  LPRUNTIMEREC  pRec;

  while (0 != (pRec = (LPRUNTIMEREC)GetNextMember(0)))
    delete pRec;

  }

