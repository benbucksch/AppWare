#ifndef __EXEFLOWMGR_H
#define __EXEFLOWMGR_H

#include "classdef.h"
#include "exserrec.h"
#include "rtuiddf.h"

class  EXEFLOWMGR:public EXESERIUSRECORD
	{
	protected:
		RUNTIMEUID flows;

	public:

		EXEFLOWMGR();

    EXEFLOWMGR(
      void * objPtr)
      {
      }

		~EXEFLOWMGR();

		LPEXEFLOW GetNextFlow(LONG theSignal, int &theKey);

		RUNTIMEUID GetFlowList()
			{
			return flows;
			}

	};

#endif
