#ifndef __CLIPOBJ_H
#define __CLIPOBJ_H

#include "classdef.h"
#include "objinst.h"
#include "seruid.h"

class CLIPOBJ:public OBJINST
	{

	public:
		CLIPOBJ(
			OBJINST * objPtr,
			BOOL groupOnClipBoard);

		LPOBJINST UpdateEnvObj(
			SUBJECT * envSubjectPtr,
			SERUID typeUID);

		~CLIPOBJ()
			{}

	};

#endif
