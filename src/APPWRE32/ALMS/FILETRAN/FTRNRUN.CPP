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
// FILE:"ftrnrun.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <string.h>
#include "filetrns.h"

extern void ActiveIdleEvent (AOBJECTID oiFT) ;

FTCLASS::FTCLASS(
	AOBJECTID oiObj,
	char* /*pathName*/, BOOL sORr) 
{
	FILETRANSFER fltr(oiObj) ;

	sendOrReceive  = sORr ;
	oiFT           = oiObj ;
	oiConn         = fltr.data0->oiConn ;
	Status         = fltr.data0->Status ;
	fileType       = fltr.data0->fileType ;
	flowCtrl       = fltr.data0->flowCtrl ;
	hBuffer        = GlobalAlloc (GMEM_MOVEABLE, BUFFERSIZE) ;
	lpBuffer       = (LPSTR)GlobalLock (hBuffer) ;
	hReadBuf       = GlobalAlloc (GMEM_MOVEABLE, BUFFERSIZE) ;
	lpReadBuf      = (LPSTR)GlobalLock (hReadBuf) ;
	transferCancelled = FALSE ;

  handshake = CommGetHandShake(oiConn) ;
	if (Status)
		{
		lpStatus = new STATUSCLASS() ;
		}
}
FTCLASS::~FTCLASS (void)
{
	GlobalUnlock (hBuffer) ;
	GlobalFree (hBuffer) ;
	GlobalUnlock (hReadBuf) ;
	GlobalFree (hReadBuf) ;
	if(hFile!=INVALID_HANDLE_VALUE)
    {
		CloseHandle (hFile) ;
		hFile = INVALID_HANDLE_VALUE ;
		}
	if (lpStatus)
   	delete lpStatus ;
}
int FTCLASS::Initiation (BOOL)
{
	return FT_SUCCESS ;
}
int FTCLASS::CancelFile(void)
{
	if (hFile!=INVALID_HANDLE_VALUE)
    {
		CloseHandle (hFile) ;
    hFile = INVALID_HANDLE_VALUE ;
		return (FT_SUCCESS) ;
		}
	return (FT_CLOSEERROR) ;
}
int FTCLASS::SendFile (void)
{
	return FT_SUCCESS ;
}
int FTCLASS::ReceiveFile (long)
{
	return FT_SUCCESS ;
}
int FTCLASS::SendBinaryFile(void)
{
	return FT_SUCCESS ;
}
void FTCLASS::Delay ( DWORD ticks )
{
  DWORD time_out;

  time_out = GetTickCount() + ticks ;
	for( ; GetTickCount() < time_out; )
    ;
}
BOOL FTCLASS::IsPortOpened (void)
{
	return (CommIsPortOpen(oiConn)) ;
} 
int FTCLASS::ReadPort (void)
{
	int c ;
  c = CommGetChar(oiConn) ;
	return (c) ;
}
int FTCLASS::WriteChar (char c)
{
	CommPutChar(oiConn, c) ;
	return 1 ;
}
//
//
//
TEXTFTCLASS::TEXTFTCLASS(AOBJECTID oiFlTr, char *fileName, BOOL sORr):FTCLASS(oiFlTr, filename, sORr)
{
	FILETRANSFER fltr(oiFlTr) ;

	filename  = new char[256] ;
	lstrcpy (filename, fileName) ; 
	charDelay    = fltr.data0->charDelay ;
	lineDelay    = fltr.data0->lineDelay ;
	maxwrapline  = fltr.data0->wrap_line ;
	lineEnd      = fltr.data0->lineEnd ;
	numChars     = 0 ;
	hFile        = CreateFile (
												filename, 
												sORr?GENERIC_READ:GENERIC_WRITE, 
												0, 
												NULL,
												sORr?OPEN_EXISTING:CREATE_NEW,
												FILE_ATTRIBUTE_NORMAL,
												NULL );
	if (hFile!=INVALID_HANDLE_VALUE)
		{
		filesize = GetFileSize (hFile, NULL) ;
    }
	if (maxwrapline > 0 )
			wrapline=TRUE ;
	else
			wrapline=FALSE ;
  charcounts = 0 ;
	totalChars = 0 ;

	CommFlushQueue (oiConn, 0) ; 
	CommFlushQueue (oiConn, 1) ; 
	//enablepause = FALSE ;
	AObjSetRuntimeInfo (oiFT, (long) this) ;
}
TEXTFTCLASS::~TEXTFTCLASS ()
{
	delete[] filename ;
	delete this ;
}
void TEXTFTCLASS::StartProtocol (AOBJECTID oiFT, int sendOrReceive)
{
	int result ;
	int rVal ;
	if (sendOrReceive==SENDFILE)
	  {
		fltr_state = SENDFILE ;
		rVal = Initiation(TRUE) ;
		result = SendFile();
		}
	else
	  {
		fltr_state = RECEIVEFILE ;
	  rVal = Initiation(FALSE) ;
		result = ReceiveFile (0) ;
		}
	if (rVal==FT_SUCCESS)
	  {
		SetTimerEvent (oiFT, setPollTimer, 0) ;
		CheckResult (result) ;
		}
	else
		AEvtPostSignalAtMark (oiFT, 6) ;
	
}
int TEXTFTCLASS::Initiation (BOOL bBar)
{
	int rVal =FT_SUCCESS ;

	if (Status)
		{
		if (!lpStatus->InitStatusWindow(1, bBar, NULL, filename, filesize))
  		 rVal = FT_LOCALCANCEL ;
		}
	//	if (!bBar)
	//		comm->PutChar(XON) ;  //Should add to the start of receive function
	return rVal ;
}
int TEXTFTCLASS::ProcessBufferData (void)  
{
	int  result ;
	int  numchars ;
	BYTE c ;
	
	if (sendOrReceive==SENDFILE)
	  {
		numchars = CommRxCount (oiConn) ;
		if (numchars>0)
			{
			while (CommGetBuffer (oiConn, &c, 1))
			  {
			  if (fltr_state==SENDFILE&&c==XOFF)
			    {
					fltr_state = XOFFHOLD ;
					break;
					}
				else if (fltr_state==XOFFHOLD&&c==XON)
				  {
				  fltr_state = SENDFILE ;
				  }
			  }
			}
		if (fltr_state==SENDFILE)
		  {
			result = SendFile();
			if (result!=FT_SUCCESS)
				KillTimerEvent (oiFT, setPollTimer) ;
			CheckResult (result) ;
			}
		}
	else
	  {
	  result = ReceiveFile(0) ;
		CheckResult (result) ;
	  }
	return(result);
	
}
void TEXTFTCLASS::CheckResult (int result)
{
	long resultId ;
	switch (result)
	  {
	  case FT_FILEEND:
			resultId = (long)endTextFile ;
	  	break;
		default:
			resultId  = 0 ;
			break;
	  }
	if (result!=FT_SUCCESS)
	  {
	  CancelFile (resultId) ;
		ActiveIdleEvent (oiFT) ;
	  }
}
int TEXTFTCLASS::CheckXoff (void)  //don't need any more
{
  if (CommGetChar(oiConn) ==0x13)
  	{
		SetFilePointer (hFile, (long)totalChars, NULL, 0) ;
		return FT_XOFF ;
    }
	return FT_XON ;
}
int TEXTFTCLASS::AddLineEnd(void)
{
	int error ;
	if (lineEnd==0)
  	{
    error = CommPutChar(oiConn,CR) ;
    }
	else if (lineEnd==1)
  	{
    error = CommPutChar (oiConn, LF) ;
    }
	else
		{
		error = CommPutChar (oiConn, CR) ;
    error = CommPutChar (oiConn, LF) ;
		}
	return error ;		
}
//
//
//
int TEXTFTCLASS::SendFile(void)
{
	int error ;
	int c ; 
  int rVal = FT_SUCCESS ;

	if (IsPortOpened()&&hFile!=INVALID_HANDLE_VALUE)
		{
		numChars = ReadFile (hFile, &c, 1, NULL,NULL) ;
		if (numChars==0) 
			return FT_FILEEND ;
		if (numChars==HFILE_ERROR) 
			return FT_ERROR ;
    error = CommPutChar(oiConn, c) ;
		if (error!=FT_OK) return error ; 
		totalChars++ ;
		if(wrapline) charcounts ++ ;
		Delay(charDelay) ;
    if (c==LF)
			Delay(lineDelay) ;
		if (wrapline && charcounts>maxwrapline)
			{
			charcounts = 0 ;															
			//if ((error=CheckPauseStatus())!=FT_OK) return error ;
			if ((error = AddLineEnd ())!=FT_OK) return error ;
      Delay(lineDelay) ;
			}
    //enablepause = TRUE ;
		if (Status)
			{
			lpStatus->blocksize++ ;
			if (lpStatus->blocksize)
			lpStatus->UpdateStatusWindow() ;
			if (lpStatus->transferStop) return FT_LOCALCANCEL ;
			}
		}
	else
   	rVal = FT_ERROR ;
	return rVal ;
}
int TEXTFTCLASS::ReceiveFile(long /*time*/)         //should plus other check
{
  //int error ;

  //enablepause = TRUE ;			           
	if (Status&&lpStatus->transferStop) return FT_LOCALCANCEL ;//Check whether the user cancel the receive file.
	numChars = CommRxCount(oiConn) ;
	numChars = CommGetString (oiConn, (BYTE*)lpBuffer, numChars+1) ;
	/*if ((error=CheckPauseStatus())<0)
		{
		if (error==FT_XOFF||error==FT_FILEEND) return error ;
		return FT_ERROR ;
		}*/
	if (numChars>0)
	  {
		WriteBytesToFile ();
  	totalChars+=numChars ;
		if (Status)             //Update Status Window
			{
			lpStatus->blocksize=totalChars ;
			lpStatus->UpdateStatusWindow() ;
    	}
		}
	return FT_SUCCESS ;
}
int TEXTFTCLASS::WriteBytesToFile(void) //BOOL sendXon
{
	int error ;
	WriteFile(hFile, lpBuffer, numChars, (DWORD*)&error, NULL) ;
  return (error) ;
}
int TEXTFTCLASS::CancelFile(long id)
{
	long signalID ;
  int  rVal =FT_CLOSEERROR ;
	//enablepause = FALSE ;
	if (Status)
		{
		DestroyWindow(lpStatus->hStatus) ;
    }
	if (hFile!=INVALID_HANDLE_VALUE)
    {
		CloseHandle (hFile) ;
		hFile = INVALID_HANDLE_VALUE ;
		if (id==(long)endTextFile)
			signalID = 0 ;//Succesed ;
		else
    	signalID = 6; //Failed ;
		rVal = FT_SUCCESS ;
		}
	AEvtPostSignalAtTail (oiFT, signalID) ;
	return (rVal) ;
}


