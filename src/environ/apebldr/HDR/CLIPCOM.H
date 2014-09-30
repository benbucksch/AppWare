#ifndef __CLIPCOMMENT_H
#define __CLIPCOMMENT_H

#include "classdef.h"
#include "serrec.h"
#include "dbstring.h"

class CLIPCOMMENT:public SERIUSRECORD
	{
	private:
		long status;
		POINT offset;
		ATYPEID typeID;
		DBSTRING name;
		DBSTRING body;

	public:
		CLIPCOMMENT(
			COMMENT * commentPtr,
			LPCLIPSJT subjectPtr);

		void UpdateEnvComment(
			SUBJECT * envSubjectPtr,
			POINT pasteOffset);

	};

#endif
