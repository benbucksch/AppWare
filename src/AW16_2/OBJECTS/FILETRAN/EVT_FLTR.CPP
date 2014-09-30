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
// FILE:"evt_fltr.cpp"
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

#include <commDlg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "filetrns.h"
#include "crc.h"
#include "zmodem.h"
//need a shutdown event to rlease the file transfer//
int GetOpenCommID (OBJECTID) ;
HFILE GetOpenFileHandle (OBJECTID) ;

extern "C"
{
	 	void ALMAPI Fltr_StartUpEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_PostPollTimerEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_PostDataTimerEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_KillIdleEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_ActiveIdleEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_SendFileEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_Ascii_SendFileEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI FlTr_Ascii_CancelFileEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI Fltr_Ascii_ReceiveFileEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI FlTr_SendBinaryFileEvent (pAEvtInfo, pAObjMessage) ;
		void ALMAPI FlTr_ReceiveBinaryFileEvent (pAEvtInfo, pAObjMessage) ;
}
extern int DoFileOpenDlg(char *) ;
extern int ChooseDirectoryDlg (char *) ;
void PostEvent (
	OBJECTID oiObj,
	EVENTID what,
	long details,
	long modifiers,
	long when,
	long how)
{
		AEvtInfo event ;

		event.sourceObject = oiObj ;
		event.targetObject = oiObj ;
		event.what         = what ;											
		event.details      = details ;
		event.modifiers    = modifiers ;
		event.when         = when ;
		event.how          = how ;

		AEvtPost (&event) ;
}
//
//
//
BOOL ReceiveFileName(LPSTR lpFileName)
{
		OPENFILENAME    ofn;

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetActiveWindow() ;
		ofn.lpstrFilter = NULL;
		ofn.lpstrFilter = "Text file (*.txt)\0*.txt\0";
		ofn.lpstrCustomFilter =NULL;
		ofn.nFilterIndex = 1;
  	lpFileName[0] = 0 ;
    ofn.lpstrFile = (LPSTR)lpFileName;
		ofn.nMaxFile = 128;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = "Receive Text File";
		ofn.lpstrFileTitle = NULL;
		ofn.lpstrDefExt = NULL;
		ofn.Flags = OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_HIDEREADONLY;

		return (GetSaveFileName((LPOPENFILENAME)&ofn));
}
void SetTimerEvent (OBJECTID oiObj, const EVENTID what, long uTimeout)
{
	AEvtInfo timerEvent ;
	timerEvent.sourceObject = oiObj ; 
  timerEvent.targetObject = oiObj ; 
  timerEvent.what         = what ; 
  timerEvent.details      = 0L ; 
  timerEvent.modifiers    = uTimeout; 
  timerEvent.when         = AAppGetTime( ) + uTimeout; 
  timerEvent.how          = AEVT_KILLDUPLICATES|AEVT_SELF|AEVT_DIRECT; 
  timerEvent.variation    = 0L; 
	AEvtPost (&timerEvent) ;  
}
void KillTimerEvent (OBJECTID oiObj, const EVENTID what)
{
	AEvtInfo timerEvent ;
	timerEvent.sourceObject = oiObj ; 
  timerEvent.targetObject = oiObj ; 
  timerEvent.what         = what ; 
  timerEvent.details      = 0L; 
  timerEvent.modifiers    = 0L; 
  timerEvent.when         = AAppGetTime( ) ; 
  timerEvent.how          = AEVT_KILLDUPLICATES|AEVT_KILLSELF; 
  timerEvent.variation    = 0L; 
	AEvtPost (&timerEvent) ;
}
void RestartTimerEvent (OBJECTID oiObj, long uTimeout)
{
	KillTimerEvent (oiObj, setDataTimer) ;
	SetTimerEvent (oiObj, setDataTimer, uTimeout) ;
}
void ActiveIdleEvent (OBJECTID oiFT)
{
	OBJECTID      oiObj ;
	LONG          theGetKey = -1 ;
  FILETRANSFER  fileTransfer (oiFT) ;

	while ((oiObj = AObjGetNextObject (OTYPE_CONN, &theGetKey))!=NULL)
		{
		if (fileTransfer.FltrGetConnObject()==oiObj)
    	{
			AEvtPostStandard (oiObj, connIdle, 0, AEVT_ATMARK) ;
			break ;
      }
		}
}
void KillIdleEvent (OBJECTID oiFT)
{
  OBJECTID  oiObj ;
	LONG      theGetKey = -1 ;
  FILETRANSFER fileTransfer (oiFT) ;

 	while ((oiObj = AObjGetNextObject (OTYPE_CONN, &theGetKey))!=NULL)
		{
		if (fileTransfer.FltrGetConnObject ()==oiObj)
    	{
			AEvtPostStandard (oiObj, connIdle, 0, AEVT_KILLDUPLICATES|AEVT_KILLSELF) ;
      break ;
      }
		}
}
void ALMAPI Fltr_PostPollTimerEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	long     uTimeout = theEvent->modifiers ;
	//long     idTimer  = theEvent->details ;
	OBJECTID oiFT = theEvent->targetObject ;
	FTCLASS* run_ft = (FTCLASS*) AObjGetRuntimeInfo(oiFT) ;
	int      rVal ;

	I_UNUSED (theSystem) ;
	theEvent->when = AAppGetTime()+ uTimeout ;
	rVal = run_ft->ProcessBufferData ()  ;
 	if (rVal==FT_SUCCESS)
 		AEvtPost (theEvent) ;
}
void ALMAPI Fltr_PostDataTimerEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	long     uTimeout = theEvent->modifiers ;
	OBJECTID oiFT = theEvent->targetObject ;
	FTCLASS* run_ft = (FTCLASS*) AObjGetRuntimeInfo(oiFT) ;
	int      rVal ;

	I_UNUSED (theSystem) ;
	theEvent->when = AAppGetTime()+ uTimeout ;
  rVal = run_ft->HandleBufferData () ;
 	if (rVal ==FT_SUCCESS)
 		AEvtPost (theEvent) ;
}
void ALMAPI Fltr_StartUpEvent (
	pAEvtInfo theEvent, 
	pAObjMessage theSystem) 
{
	OBJECTID oiFT ;
	LONG       theGetKey = -1 ;

	I_UNUSED (theEvent) ;
	I_UNUSED (theSystem) ;

	while ((oiFT = AObjGetNextObject(OTYPE_FILETRANSFER, &theGetKey))!=NULL)
		{
		FILETRANSFER fltr(oiFT) ;
		if (fltr.IsAutoDownlaod ())
			AEvtPostSignalAtTail (oiFT, 1) ;
		AEvtPostSignalAtTail (oiFT, 3) ;
		AEvtPostSignalAtTail (oiFT, 5) ;
		}
}
void ALMAPI Fltr_KillIdleEvent (  //Change this event to a general function
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID	oiFT = theEvent->targetObject;
  OBJECTID  oiObj ;
	LONG      theGetKey = -1 ;
  FILETRANSFER fileTransfer (oiFT) ;

	I_UNUSED (theSystem) ;
	I_UNUSED (theEvent) ;

 	while ((oiObj = AObjGetNextObject (OTYPE_CONN, &theGetKey))!=NULL)
		{
		if (fileTransfer.FltrGetConnObject ()==oiObj)
    	{
			AEvtPostStandard (oiObj, connIdle, 0, AEVT_KILLDUPLICATES|AEVT_KILLSELF) ;
      break ;
      }
		}	
}
void ALMAPI Fltr_ActiveIdleEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID      oiFT = theEvent->targetObject ;
	OBJECTID      oiObj ;
	LONG          theGetKey = -1 ;
  FILETRANSFER  fileTransfer (oiFT) ;

	I_UNUSED (theEvent) ;
	I_UNUSED (theSystem) ;

	while ((oiObj = AObjGetNextObject (OTYPE_CONN, &theGetKey))!=NULL)
		{
		if (fileTransfer.FltrGetConnObject()==oiObj)
    	{
			AEvtPostStandard (oiObj, connIdle, 0, AEVT_ATMARK) ;
			break ;
      }
		}
}
//////////////////////////////////////////////////////////////////////////
//Function:    Fltr_SendFileEvent
//Description: Send file Event. Start to send a file
//////////////////////////////////////////////////////////////////////////
void ALMAPI Fltr_SendFileEvent (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID oiFT = (OBJECTID) theEvent->targetObject ;
	BYTE  	 fileType = theEvent->details ;
	HGLOBAL  hFileName = (HGLOBAL)theEvent->modifiers ;

	I_UNUSED (theSystem) ;
	if (fileType==ASCII)
		PostEvent (oiFT, sendTextFile, (long)startEvent, (long)hFileName, AAppGetTime(), AEVT_ATTAIL) ;
	else
		PostEvent (oiFT, sendBinaryFile, (long)startEvent, (long)hFileName, AAppGetTime(), AEVT_ATTAIL) ;
}
//////////////////////////////////////////////////////////////////////////
//Function:    Fltr_Ascii_SendFileEvent
//Description: Send a ascii file
//////////////////////////////////////////////////////////////////////////
void ALMAPI Fltr_Ascii_SendFileEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID     oiFT = theEvent->targetObject ;
	TEXTFTCLASS* ascii ; //FTCLASS*  run_ft ;

  I_UNUSED (theSystem) ;
	if (theEvent->details==(long)startEvent)
		{
		HGLOBAL hFileName = (HGLOBAL) theEvent->modifiers ;
		char *fileName = (char*)GlobalLock (hFileName) ;
		ascii = new TEXTFTCLASS (oiFT, fileName, SENDFILE) ;
		GlobalUnlock (hFileName) ;
		GlobalFree (hFileName) ;
		if (ascii)
		  {
			if (ascii->IsPortOpened())
     		{
				KillIdleEvent (oiFT) ;
				ascii->StartProtocol (oiFT, SENDFILE) ;
			  }
			}
		}
}
void ALMAPI Fltr_Ascii_ReceiveFileEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	TEXTFTCLASS*   ascii ;
	OBJECTID       oiFT = theEvent->targetObject ;

	I_UNUSED (theSystem) ;
	if (theEvent->details!=(long)startEvent)
  	ascii=(TEXTFTCLASS*) AObjGetRuntimeInfo(oiFT) ;
	if (theEvent->details==(long)startEvent)
    {
    char fileName[256] ;
		ReceiveFileName(fileName) ;
		if (strlen(fileName))
		  {
			ascii = new TEXTFTCLASS (oiFT, fileName, FALSE) ;
			if (ascii)
			  {
			 	if (ascii->IsPortOpened())
       		{
					KillIdleEvent (oiFT) ;
					ascii->StartProtocol (oiFT, RECEIVEFILE) ;
		 			}
				}
			}
		}
}
void ALMAPI FlTr_Ascii_CancelFileEvent (               //If run O.K. delete this function
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID     oiFT = (OBJECTID) theEvent->targetObject ;
	TEXTFTCLASS* run_ft = (TEXTFTCLASS*)AObjGetRuntimeInfo(oiFT) ;

	I_UNUSED (theSystem) ;
	run_ft->CancelFile (theEvent->modifiers) ;
	AEvtPostStandard (oiFT, activeIdle, 0, AEVT_ATMARK) ;
}
//
//
//
void ALMAPI FlTr_SendBinaryFileEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	FTCLASS*  run_ft ;
	OBJECTID  oiFT = theEvent->targetObject ;
	int       BinaryType = XMODEM ;
	HGLOBAL   hFileName = (HGLOBAL)theEvent->modifiers ;
	char*     fileName = (char*)GlobalLock (hFileName) ;
	FTNSDATA0* data0 = (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;

	I_UNUSED (theSystem) ;
	if (data0)
		{
		BinaryType = data0->fileType ; //flowCtrl ;
		AObjUnlockData (oiFT, FTNSDATA) ;
		}
	if (strlen(fileName))
		{
		KillIdleEvent (oiFT) ;
		switch (BinaryType)
   		{
			case XMODEM:																					                  
				run_ft = new XMODEMCLASS (oiFT, fileName, TRUE) ;
				run_ft->StartProtocol (oiFT, SENDFILE) ;
				break ;

			case YMODEM:
				run_ft = new YMODEMCLASS (oiFT, fileName, TRUE) ;
				run_ft->StartProtocol (oiFT, SENDFILE) ;
       	break ;
			case KERMIT:
				run_ft = new KERMITCLASS (oiFT, fileName, TRUE) ;
				run_ft->StartProtocol (oiFT, SENDFILE) ;
				break ;
			case ZMODEM:
				run_ft = new ZMODEMCLASS (oiFT, fileName, TRUE) ;
				run_ft->StartProtocol (oiFT, SENDFILE) ;
				break;
			}
		}
  delete[] fileName ;
}
void ALMAPI FlTr_ReceiveBinaryFileEvent (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	FTCLASS*  run_ft ;
	OBJECTID  oiFT = theEvent->targetObject ;
	int       BinaryType ;
	char*     fileName ;
	FTNSDATA0* data0 = (FTNSDATA0*)AObjLockData (oiFT, FTNSDATA) ;
  int       rVal ;

	I_UNUSED (theSystem) ;
	if (data0)
		{
		BinaryType = data0->fileType ; //flowCtrl ;
		AObjUnlockData (oiFT, FTNSDATA) ;
		}

	fileName = new char[256] ;
	if (BinaryType==XMODEM)
		rVal=ReceiveFileName(fileName) ;
	else
		rVal=ChooseDirectoryDlg (fileName) ;
	if (rVal&&strlen(fileName))
		{
		KillIdleEvent (oiFT) ;
		switch (BinaryType)
    	{
			case XMODEM:
				run_ft = new XMODEMCLASS (oiFT, fileName, FALSE) ;
				run_ft->StartProtocol (oiFT, RECEIVEFILE) ;
				break ;

			case YMODEM:
				run_ft = new YMODEMCLASS (oiFT, fileName, FALSE) ;
				run_ft->StartProtocol (oiFT, RECEIVEFILE) ;
        break ;

			case KERMIT:
        {
				KERMITCLASS * kermit = new KERMITCLASS (oiFT, fileName, FALSE) ;
				kermit->StartProtocol (oiFT, RECEIVEFILE) ;
				break ;
        }

			case ZMODEM:
				run_ft = new ZMODEMCLASS (oiFT, fileName, FALSE) ;
				run_ft->StartProtocol (oiFT, RECEIVEFILE) ;
				break;
			}
    }
}
