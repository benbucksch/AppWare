#ifndef __CLIPFUNC_H
#define __CLIPFUNC_H

#include "classdef.h"
#include "serrec.h"

class CLIPFUNC:public SERIUSRECORD
	{
	private:
		long status;
		POINT offset;
		ATYPEID typeID;

	public:
		CLIPFUNC(
			FUNCINST * funcPtr,
			LPCLIPSJT subjectPtr);

		void UpdateEnvFunc(
			SUBJECT * envSubjectPtr,
			POINT pasteOffset);

	};

#endif
