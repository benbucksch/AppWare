#ifndef __CLIPGRUP_H
#define __CLIPGRUP_H

#include "classdef.h"
#include "objgroup.h"

class CLIPGROUP:public SERIUSRECORD
	{
	private:
		int objCount;
		long groupStatus;
		long baseStatus;
		DBSTRING name;
		LPGROUPINFO groupInfo;
		int width;
		RUNTIMEUID objIndexes;
		POINT offset;

	public:
		CLIPGROUP(
			LPOBJGROUP groupPtr);

		LPOBJGROUP UpdateEnvGroup(
			SUBJECT * envSubjectPtr,
			POINT pasteOffset);

		~CLIPGROUP()
			{}

		void CreateObjList(
			LPOBJGROUP groupPtr);

	};

#endif
