#ifndef __CLIPPARAM_H
#define __CLIPPARAM_H

#include "classdef.h"
#include "parmmask.h"
#include "serrec.h"

#define CP_FUNCINCLIP 	0x00000001L
#define CP_SOURCEINCLIP 0x00000002L

class CLIPPARAM:public SERIUSRECORD, public PARAMMASK
{
	private:
		SERUID ownerFuncUID;
		int ordinal;
		int linkOrdinal;
		SERUID linkFuncUID;

	public:
		long   clipStatus;

		CLIPPARAM(
			PARAM * paramPtr,
			BOOL deleteAll);

		SERUID GetOwnerFuncUID()
			{
			return ownerFuncUID;
			}

		void SetOwnerFuncUID(
			SERUID setUID)
			{
			ownerFuncUID = setUID;
			}

		BOOL TestFuncInClipFlag()
			{
			return (clipStatus & CP_FUNCINCLIP)?TRUE:FALSE;
			}

		BOOL TestSourceInClipFlag()
			{
			return (clipStatus & CP_SOURCEINCLIP)?TRUE:FALSE;
			}

		void UpdateEnvParam();

	};

#endif
