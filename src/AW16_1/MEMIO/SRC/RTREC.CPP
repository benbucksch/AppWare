#include "almsys.h"

#include "rtrec.h"


void RUNTIMEREC::UpdateUIDTable(
  LPRUNTIMERECLIST  pRecList)
  {
  LPRUNTIMEREC  pRec = 0;
  LPUIDTABLE    pUidTable;

  pUidTable = pTheSystem->GetFileObject()->GetUIDTable();

  while(0 != (pRec = (LPRUNTIMEREC)pRecList->GetNextMember(pRec)))
    {
    pUidTable->SetPointer(pRec->GetUID(), pRec);
    }

  }
