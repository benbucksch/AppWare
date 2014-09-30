#include "almsys.h"

#include "exeflwmg.h"
#include "sereclib.h"
#include "exrecblk.h"
#include "exeflow.h"
#include "rectypes.h"
#include "exefunc.h"


EXEFLOWMGR::EXEFLOWMGR()
	{

	flows = GetUID(new EXERECORDBLOCK(sizeof(EXEFLOW), RT_EXEFLOW));

	}

EXEFLOWMGR::~EXEFLOWMGR()
	{
  ((LPEXERECORDBLOCK)GetObject(flows))->PurgeBlock();

	delete (LPEXERECORDBLOCK)GetObject(flows);
	}

LPEXEFLOW EXEFLOWMGR::GetNextFlow(
  LONG  theSignal,
  int   &theKey)
  {
	LPEXEFLOW  flowPtr;

  while(0 != (flowPtr = (LPEXEFLOW)GetObject(flows, theKey)))
		{
		//We need to check and make sure the signal or flow connection is valid.
		//An invalid connection shows up in the workbench by a '?' for the
		//signal or flow name
		if (flowPtr->TestFlowConnectedFlag() && flowPtr->GetSignal() == theSignal)
			return flowPtr;
		}

  return 0;
	}
