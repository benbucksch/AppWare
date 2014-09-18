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
// FILE:"evt_conn.cpp"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <stdio.h>																														           
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "commObj.h"
//
//
//
void ReadFromComm (OBJECTID, COMMUNICATION*) ;
void IdleSystem(int msec)
{
	DWORD start ;

	start =GetTickCount() ;
	while (GetTickCount()<start+msec)
	{
	}
}

void ALMAPI Conn_StartUpEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	long     theGetKey = -1 ;
	OBJECTID oiConn ;
  COMMUNICATION *comm ;

	I_UNUSED (theEvent) ;
	I_UNUSED (theSystem) ;

	while ((oiConn=AObjGetNextObject(OTYPE_CONN, &theGetKey))!=NULL)
    {
		comm = new COMMUNICATION (oiConn, INBUFSIZE, OUTBUFSIZE) ;
		if (comm)
			{
			comm->SetActiveSearchTable () ;
			AObjSetRuntimeInfo (oiConn, (long) comm) ;
			AEvtPostStandard(oiConn, connIdle, 0, AEVT_ATTAIL) ;
			}
		}
}
void ALMAPI Conn_ShutDownEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	long     theGetKey = -1 ;
	OBJECTID oiConn ;
	COMMUNICATION *comm ;

	I_UNUSED (theEvent) ;
  I_UNUSED (theSystem) ;
	while ((oiConn=AObjGetNextObject(OTYPE_CONN, &theGetKey))!=NULL)
		{
		comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
		if (comm)
			{
			comm->CommClose() ;
			delete comm ;
			}
		}
	AEvtPostStandard (oiConn, connIdle, 0, AEVT_KILLDUPLICATES|AEVT_KILLSELF) ;
	return ; 
}

void ALMAPI Conn_IdleEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID       oiConn = theEvent->targetObject;
	COMMUNICATION *comm ;

  I_UNUSED (theEvent) ;
	I_UNUSED (theSystem) ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if (comm->ReadPortToMem()>0)
		{
    comm->DoIdle() ;
    }
	AEvtPostStandard (oiConn, connIdle, 0, AEVT_KILLDUPLICATES) ;
}
//
//
//
void ALMAPI Conn_AsynchListenEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID       oiConn = (OBJECTID)theEvent->targetObject ;
	long           ticks = theEvent->details ;
	DWORD          startTime = theEvent->modifiers ;
	BOOL           post = ticks<0?TRUE:((GetTickCount()<startTime+ticks)?TRUE:FALSE) ;
	COMMUNICATION *comm = (COMMUNICATION*) AObjGetRuntimeInfo (oiConn) ;

	I_UNUSED (theSystem) ;
	if (comm->ListenACall ())
		post = FALSE ;
	if (post)
	  {
		theEvent->when = AAppGetTime() ;
		theEvent->how = AEVT_KILLDUPLICATES|AEVT_ATTAIL ;
		AEvtPost (theEvent) ;
		}
} 
//
//
//
void ALMAPI Conn_SendScheduleTextEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn = (OBJECTID)theEvent->targetObject ;
	OBJECTID oiText = (OBJECTID)theEvent->modifiers ;
	WORD		 textLen ;
	LPSTR	   lpText	;
	COMMUNICATION* comm ;

	I_UNUSED (theSystem) ;
	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if (comm&&comm->LinkOpen())
		{
		textLen =OTextGetLength (oiText) ;
		lpText = (LPSTR) new char(++textLen) ;
  	OTextGetToBuffer (oiText, lpText, textLen) ;
		comm->PutString((BYTE*)lpText) ;
		delete[] lpText ;
		}
}
/*void ALMAPI Conn_CommOpenEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn = theEvent->targetObject ;
	COMMUNICATION *comm ;
	int            err=COMM_ERROR ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if (comm)
		{
		if (!comm->connbusy)
    	{
			err = comm->InitComm() ;
			comm->Dial() ;
			}
		}
	if (err==COMM_OK)
  	{
		AFuncReportResult(theSystem, 1, NULL) ;
		AEvtPostSignalAtMark (oiConn, 1) ;
    }
	else
		AFuncReportResult(theSystem, 2, NULL) ;
}
//
//
//
void ALMAPI Conn_CommCloseEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn = theEvent->targetObject ;
	COMMUNICATION *comm ;
  long           detail = theEvent->details ;
  int            err ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if(comm)
		{
		err= comm->ModemHangUp() ;
		err=comm->CommClose() ;
		if (detail==changeConfigure)
			{
			delete comm ;
			comm = new COMMUNICATION (oiConn, INBUFSIZE, OUTBUFSIZE) ;
			AObjSetRuntimeInfo (oiConn, (long) comm) ;
			}
		}
	if (err==COMM_OK)
  	{
		AFuncReportResult(theSystem, 1, NULL) ;
		AEvtPostSignalAtMark(oiConn, 0) ;
    }
	else
		AFuncReportResult(theSystem, 2, NULL) ;
}	*/
