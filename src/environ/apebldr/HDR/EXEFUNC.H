#ifndef __EXEFUNCINST_H
#define __EXEFUNCINST_H

#include "exeflwmg.h"
#include "seruid.h"
#include "intalm.h"

class EXEFUNCINST:public EXEFLOWMGR
	{

	private:
		long						status;
		RUNTIMEUID			inputParams;
		RUNTIMEUID			outputParams;
		METHODPROC			methodAddress;

	public:

		EXEFUNCINST(
			WORD newStatus);

		~EXEFUNCINST();

		AOBJECTID  GetParameter(
      LONG paramIndex,
      ATYPEID theTypeID = 0);

		METHODPROC  GetMethodAddr()
			{
			return methodAddress;
			}

		void SetMethodAddr(
      METHODPROC addr)
			{
			methodAddress = addr;
			}

		RUNTIMEUID GetInputParams()
			{
			return inputParams;
			}

		RUNTIMEUID GetOutputParams()
			{
			return outputParams;
			}

		BOOL SetParameter(
			LONG 			parameterIndex,
			AOBJECTID 	theObject);

    AOBJECTID GetOutputParameter(
      LONG      parameterIndex,
      LPTYPEID  lpTypeID);

	};

#endif
