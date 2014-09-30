#ifndef __cfglib_h__
#define __cfglib_h__

#include "classdef.h"
#include "compuid.h"

BOOL cfgGetDatabaseVersion(
	int 		hFile);

void cfgReadConfigs(
	void);

BOOL cfgCheckConfigFile(
	int 		hFile,
	LPSTR   fileName);

void cfgCleanUp(
	void);

BOOL cfgAddConfig(
	LPFILEOBJECT	fileObject);

LPFILEOBJECT cfgGetNextFileObject(
	LPFILEOBJECT	currentFileObject);

LPOBJDESC cfgGetNextObjDesc(
	LPOBJDESC	currObjDesc);

LPFUNCDESC cfgGetNextFuncDesc(
	LPFUNCDESC	currFuncDesc);

LPEVENTDESC cfgGetNextEventDesc(
	LPEVENTDESC	currMsgDesc);

void cfgInitObjectList(
	HWND	hComboBox,
	LPSTR	pszGeneralText	= NULL);

void cfgInitEventList(
	HWND	hComboBox);

TSTRING cfgGetEventName(
	COMPUID		eventID);

BOOL cfgCheckObjectUID(
	COMPUID	compUID);

BOOL cfgCheckFunctionUID(
	COMPUID	compUID);

BOOL cfgCheckEventUID(
	COMPUID	compUID);

void cfgWrite(
	int   hFile);

#ifdef AW_I_DEBUG
BOOL cfgWriteFuncBitmaps(
  LPFUNCDESC  pFuncDesc,
  LPCSTR      szDir);

void cfgWriteBitmaps(
  void);
#endif //#ifdef AW_I_DEBUG

#endif

