#ifndef __OBJDESC_H__
#define __OBJDESC_H__

#include "classdef.h"
#include "compdesc.h"
#include "routines.h"
#include "sericon.h"
#include "response.h"

//      defines for the various routine indexes.

#define MAX_ROUTINES 					3

#define ADMIN_ROUTINE_INDEX   0
#define DEF_ROUTINE_INDEX     1
#define IMPEX_ROUTINE_INDEX   2

//Persistence Flags

#define OBJD_CANTBEPERSISTENT	0x00000001ul
#define OBJD_MUSTBEPERSISTENT	0x00000002ul
#define OBJD_HIDDENTYPE				0x00000004ul

struct OBJDESC:public COMPDESC
	{

	friend BOOL far pascal AEvtCheckForMethod (
		TYPEID  theType,
		EVENTID selector);

	friend BOOL far pascal AEvtCallMethod (
		OBJECTID sourceObject,
		OBJECTID targetObject,
		TYPEID   targetType,
		EVENTID  selector,
		LONG     methodParam);

	friend BOOL I_CALLBACK ObjDescDlgProc(
		HWND hDlg,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

private:
	short           fileIndex;
	ROUTINES        routines;
	RESPONSEARRAY   responses;
	long            objectFlags;
	DBSTRING        helpFile;
	SERIUSICON      icons;

public:

	OBJDESC(
		void):
			routines(MAX_ROUTINES),
			responses(),
			helpFile("")
		{
    objectFlags = 0;
		}

	OBJDESC(
		void far*       objPtr):
			COMPDESC(objPtr),
			routines(objPtr),
			responses(objPtr),
			helpFile(objPtr),
			icons(objPtr)
		{
		}

	OBJDESC(
		COMPUID id,
		TSTRING         name);

	LPSERIUSICON GetIcons(
		void)
    {
    return &icons;
    }

	LPSTR   GetFunction(
		short   index);

	LPSTR GetDLL(
		short index);

	LPSTR   pGetFunction(
		short   index);

	LPSTR pGetDLL(
		short index);

	LPSTR GetHelpFileName(
		void)
		{
		return helpFile;
		}

	BOOL SetHelpFileName(
		LPSTR  newName)
		{
		this->helpFile = newName;
		return TRUE;
		}

	LPSTR pGetHelpFileName(
		void)
		{
		return helpFile.pString;
		}

	BOOL SetFunction(
		short index,
		LPSTR newFunction);

	BOOL SetDLL(
		short   index,
		LPSTR newDLL);

	LPRESPONSE pGetResponse(
		COMPUID         eventID);

	LPRESPONSEARRAY GetResponseArray(
		void)
		{
		return &responses;
		}

	void ConvertToPointers(
		void);


  BOOL SetMustBePersistentFlag(
    void)
    {
    this->objectFlags &= (~OBJD_CANTBEPERSISTENT);
    this->objectFlags |= OBJD_MUSTBEPERSISTENT;
    return TRUE;
    }

  BOOL ClearMustBePersistentFlag(
    void)
    {
    this->objectFlags &= (~OBJD_MUSTBEPERSISTENT);
    return TRUE;
    }

  BOOL TestMustBePersistentFlag(
    void)
    {
    return (this->objectFlags & OBJD_MUSTBEPERSISTENT) ? TRUE : FALSE;
    }

  BOOL SetCantBePersistentFlag(
    void)
    {
    this->objectFlags &= (~OBJD_MUSTBEPERSISTENT);
    this->objectFlags |= OBJD_CANTBEPERSISTENT;
    return TRUE;
    }

  BOOL ClearCantBePersistentFlag(
    void)
    {
    this->objectFlags &= (~OBJD_CANTBEPERSISTENT);
    return TRUE;
    }

  BOOL TestCantBePersistentFlag(
    void)
    {
    return (this->objectFlags & OBJD_CANTBEPERSISTENT) ? TRUE : FALSE;
    }

  BOOL SetMayBePersistentFlag(
    void)
    {
    this->objectFlags &= (~(OBJD_MUSTBEPERSISTENT|OBJD_CANTBEPERSISTENT));
    return TRUE;
    }

  void SetHiddenFlag(
    void)
    {
    this->objectFlags |= (OBJD_HIDDENTYPE);
    }

  void ClearHiddenFlag(
    void)
    {
    this->objectFlags &= ~(OBJD_HIDDENTYPE);
    }

  BOOL TestHiddenFlag(
    void)
    {
    return (this->objectFlags & OBJD_HIDDENTYPE) ? TRUE : FALSE;
    }
	};

#endif
