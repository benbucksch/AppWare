#ifndef __CFGDESC_H__
#define __CFGDESC_H__

#include "classdef.h"
#include "compuid.h"
#include "serrec.h"
#include "seruid.h"
#include "dbstring.h"

#define	CFG_DIRTY		0X0001
#define	CFG_NEW	    0X0002

class ACFGDESC:public SERIUSRECORD
	{
private:
	RUNTIMEUID		objectsBlk;
	RUNTIMEUID		functionsBlk;
	RUNTIMEUID		eventsBlk;
	DBSTRING	funcGroupName;

public:

	ACFGDESC(
		void);

	ACFGDESC(
		void *	objPtr):
			funcGroupName(objPtr)
		{
		}

	SERUID CreateObjectDesc(
		TSTRING		name,
		COMPUID		objectUID);

	SERUID CreateFunctionDesc(
		TSTRING		name,
		COMPUID		functionUID);

	SERUID CreateEventDesc(
		TSTRING		name,
		COMPUID		eventUID);

	void DeleteObjectDesc(
		SERUID	 objectUID);

	void DeleteFunctionDesc(
		SERUID	 objectUID);

	void DeleteEventDesc(
		SERUID	 objectUID);

	LPOBJDESC NextObjectDesc(
		LPOBJDESC	currentObject);

	LPAFUNCDESC NextFunctionDesc(
		LPAFUNCDESC	currentFunction);

	LPEVENTDESC NextEventDesc(
		LPEVENTDESC	currentEvent);

	LPOBJDESC pNextObjectDesc(
		LPOBJDESC	currentObject);

	LPAFUNCDESC pNextFunctionDesc(
		LPAFUNCDESC	currentFunction);

	LPEVENTDESC pNextEventDesc(
		LPEVENTDESC	currentEvent);

	BOOL SetFuncGroupName(
		LPCSTR  newName)
		{
		funcGroupName = newName;
		return TRUE;
		}

	LPCSTR GetFuncGroupName(
		void)
		{
		return funcGroupName;
		}

	LPSTR pGetFuncGroupName(
		void)
		{
		return funcGroupName.pString;
		}

	void ConvertToPointers(
		void);

	};

#endif
