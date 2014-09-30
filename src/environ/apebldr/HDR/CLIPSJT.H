#ifndef __CLIPSJT_H
#define __CLIPSJT_H

#include "classdef.h"
#include "serrec.h"
#include "dbstring.h"

class CLIPSJT:public SERIUSRECORD
	{

	public:

		RECT bounds;
		RECT logBounds;
		POINT projectOffset;
		POINT originOffset;
		RUNTIMEUID objs;
		RUNTIMEUID funcs;
		RUNTIMEUID comments;
		RUNTIMEUID params;
		RUNTIMEUID flows;
		RUNTIMEUID groups;
		long status;
		DBSTRING name;

		CLIPSJT(
			SUBJECT * subjectPtr);

		~CLIPSJT();

		SUBJECT * UpdateEnvSubject(
			RUNTIMEUID subjectBlock,
			POINT      pasteOffset);

	};

#endif
