////////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:"commobj.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <stddef.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_text.h>
#include <o_number.h>
#include <o_time.h>
#include "o_comm.h"
#include "macro.h"
#include "conn.h"
//distance from queue full to Xoff

extern HINSTANCE hInstance ;

extern "C" {
	LONG ALMCALLBACK ConnectAdmn  (AOBJECTID, pAObjMessage) ;
	//Event function
	void ALMAPI Conn_StartUpEvent (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_ShutDownEvent (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_IdleEvent (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_SendScheduleTextEvent (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_AsynchListenEvent (pAEvtInfo, pAObjMessage) ;

	//Function
	void ALMAPI Conn_ActivateFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_DeactivateFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_SetSearchFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_GetConfigFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_SetConfigFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_BreakFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_ListenFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_CancelListenFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_ScheduleTextFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_UnscheduleTextFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_ConfigureFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_DialFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_HangUpFunc (pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_OpenCommFunc(pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_CloseCommFunc(pAEvtInfo,	pAObjMessage) ;
	void ALMAPI Conn_SendTextFunc (pAEvtInfo, pAObjMessage) ;
  void ALMAPI Conn_TextToHexFunc (pAEvtInfo, pAObjMessage) ;
  void ALMAPI Conn_HexToTextFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_ValueToCharFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_CharToValueFunc (pAEvtInfo, pAObjMessage) ;
	void ALMAPI Conn_CheckSumFunc (pAEvtInfo, pAObjMessage) ;
}
