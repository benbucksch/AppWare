////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//	"flag_hdr.h"
//
// General header file for the Flag object
//
////////////////////////////////////////////////////////////////////////////

#ifndef		__FLAG_HDR_H	// prevent multiple includes
#define		__FLAG_HDR_H

#ifndef		A_ALM_H
#include	<a_alm.h>	// include <oip.h> if necessary
#endif

// constants ///////////////////////////////////////////////////////////////

// version constants
#define	VERSION_OBJECT						0
#define	VERSION_DLL								0L

// help constants
#define	SERIUS_HELPFILE						"serhelp.hlp"
#define	HELPID_OBJD_Flag					10001

// data constants
#define	DATA_FLAGDATA							0L
#define	DATA_SETTEXT							1L
#define	DATA_CLEARTEXT						2L
#define	NUM_FLAGDATA							3L

// signal constants
#define	SIGNAL_SET								0L
#define	SIGNAL_CLEARED						1L
#define	SIGNAL_NUMSIGNALS					2L

#define	SIGNAME_SET								"Set"
#define	SIGNAME_CLEARED						"Cleared"

// flag object default strings
#define	DEFAULT_SETTEXT						"TRUE"
#define	DEFAULT_CLEARTEXT					"FALSE"

// Global container class for the scroll bar object ////////////////////////

class GLOBAL
	{
	private:

	public:
		HINSTANCE	hInstance;
		int				cbByteOffset;
	};

// function prototypes /////////////////////////////////////////////////////

extern "C"
	{
	// functions included in "flag_ent.cpp"
	void CALLBACK Flag_ClearFlag(pAEvtInfo theEvent, pAObjMessage theSystem);
	void CALLBACK Flag_IsFlagSet(pAEvtInfo theEvent, pAObjMessage theSystem);
	void CALLBACK Flag_SetFlag(pAEvtInfo theEvent, pAObjMessage theSystem);
	void CALLBACK Flag_ToggleFlag(pAEvtInfo theEvent, pAObjMessage theSystem);
	void CALLBACK Flag_objectToText(pAEvtInfo theEvent, pAObjMessage theSystem);
	void CALLBACK Flag_textToObject(pAEvtInfo theEvent, pAObjMessage theSystem);

	// functions included in "flag_mai.cpp"
///	int CALLBACK /*!!PORT!! was _far_ _pascal_*/ 	LibMain/*!!PORT!! Replace with DllEntryPoint */(HINSTANCE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine);
///  int CALLBACK /*!!PORT!! was _far_ _pascal_*/ 	WEP/*!!PORT!! Replace with DllEntryPoint */(int nParameter);
	long CALLBACK GetSerVersion(ATYPEID theType);
	LONG CALLBACK Flag_Admn(AOBJECTID theEvent, pAObjMessage theSystem);
	LONG CALLBACK Flag_ObjD(AOBJECTID theObject, pAObjMessage theSystem);
	}

// functions in "flag_mai.cpp"
void Flag_doAssign(AOBJECTID dstObject, AOBJECTID srcObject);
LONG Flag_doVersionCheck(AOBJECTID theObject, long& version, long& status, char* error);
void Flag_editObject(AOBJECTID	theObject);
LONG Flag_getParameters(AOBJECTID flagObj, AObjInfo* infoRecord);
void Flag_buildSketchSignals(AMEMBLOCKID theBlock);
BOOL CALLBACK Flag_ObjDDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

///inline UINT	min(UINT val1, UINT val2)	{return (val1<val2) ? val1 : val2;}

////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __FLAG_HDR_H
