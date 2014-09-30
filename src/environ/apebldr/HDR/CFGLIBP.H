#ifndef __cfglib_h__
#define __cfglib_h__

#include "classdef.h"
#include "compuid.h"

#ifdef ADBG_DEBUG
void cfgDumpConfigInfo(
  HANDLE hFile);
#endif

void cfgReadConfigs(
	void);

BOOL cfgCheckConfigFile(
	HANDLE 		hFile,
	LPSTR   fileName);

LPOBJDESC cfgGetNextObjDesc(
	LPOBJDESC	currRec);

LPOBJDESC cfgGetObjectDesc(
	COMPUID	objectType);

LPAFUNCDESC cfgGetNextFuncDesc(
	DWORD 			objectID,
	LPAFUNCDESC	currRec);

LPAFUNCDESC cfgGetFunctionDesc(
	COMPUID	functionType);

LPSTR cfgGetFunctionCategoryName(
	COMPUID functionType);

LPEVENTDESC cfgGetNextEventDesc(
	LPEVENTDESC	currEventDesc);

LPFILEOBJECT cfgGetNextFileObject(
	LPFILEOBJECT	currentFileObject);


#endif

