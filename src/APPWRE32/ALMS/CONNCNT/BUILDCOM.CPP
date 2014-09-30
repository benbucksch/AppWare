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
// FILE:"buildcom.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "com_dlg.h"
#include "commObj.h"

//Modem Result Code
char * cstResultCode[]={"CONNECT 110","CONNECT 300", "CONNECT 600", "CONNECT 1200",
												"CONNECT 2400", "CONNECT 4800", "CONNECT 9600", "CONNECT 19200", 
												"OK", "NO CARRIER", "RING", "BUSY", "ERROR","VOICE", "NO ANSWER"} ;

extern LPSTR StrCmp(LPSTR, LPSTR, int, int*) ;
COMMUNICATION::COMMUNICATION(AOBJECTID oiObj, int inBufSize, int outBufSize)
{
	commObj     = oiObj ;
	data0	      = (DATA0*)AObjLockData(commObj, CONNDATA);
	inbuf       = inBufSize ;
	outbuf      = outBufSize ;
	portOpened  = FALSE ;
	hCommDev    = INVALID_HANDLE_VALUE ;
	nCommError  = 0x00;
	listenFlag  = FALSE ;
	redialTimes = data0->modem.Redial?data0->modem.RedialTimes:-1 ;
	hCommMem    = GlobalAlloc (GPTR, (DWORD)(inBufSize-200)) ;
	lpCommMem   = (LPSTR) GlobalLock (hCommMem) ;
	*lpCommMem  = '\0' ;
	hResult     = GlobalAlloc (GPTR, (DWORD)(inBufSize-200)) ;
	resultStr   = (LPSTR) GlobalLock (hResult) ;
	*resultStr  = '\0' ;
	hSearch = GlobalAlloc (GPTR, (DWORD)(inBufSize-200)) ;
	searchStr = (LPSTR)GlobalLock (hSearch) ;
  *searchStr = '\0' ;
}
COMMUNICATION::~COMMUNICATION (void)
{
	if (data0) AObjUnlockData (commObj, CONNDATA) ;
	if (lpCommMem)
		{
		GlobalUnlock (hCommMem) ;
		GlobalFree (hCommMem) ;
		}
	if (hResult)
		GlobalFree (hResult) ;
	if (hSearch)
		GlobalFree (hSearch) ;
	if (hIndex)
		GlobalFree (hIndex) ;
	if (hActiveStr)
		GlobalFree (hActiveStr) ;
	if (hCommDev>=0)
		{
		CommClose() ;
		hCommDev = INVALID_HANDLE_VALUE ;
		}
  portOpened = FALSE ;
}
void COMMUNICATION::SetActiveSearchTable (void)
{
	SIGNALINFO *signalInfo ;
	LPSTR       signalBlock ;
	int         i ;
	int        *index ;
  LPSTR       activeStr ;
	DWORD  			blockSize ;

	signalInfo = (SIGNALINFO*)AObjLockData (commObj,SGNLINFO) ;
	if (signalInfo)
		{
		signalBlock =(LPSTR) AObjLockData (commObj, SGNLBLOCK) ;
		blockSize = (DWORD)AObjGetDataSize (commObj, SGNLBLOCK)+1 ;
		if (signalBlock)
    	{
			hIndex = GlobalAlloc (GPTR, (DWORD)data0->signal.signalCount*sizeof(int)) ;
			index = (int*)GlobalLock (hIndex) ;
			hActiveStr = GlobalAlloc (GPTR, blockSize) ; 
			activeStr =(LPSTR) GlobalLock(hActiveStr) ;
			if (index&&activeStr)
  			{
 				for (i=0; i<data0->signal.signalCount; i++, signalInfo++)
					{
					if (signalInfo->active)
						{
            
						int bits = signalInfo->CompareBits?
											-(signalInfo->signalIndex+1):
											 (signalInfo->signalIndex+1);
						*index = bits ;
						index++ ;
						lstrcpy(activeStr, signalBlock) ;
            activeStr +=_fstrlen(activeStr)+1 ;
						}
					signalBlock +=_fstrlen(signalBlock)+1 ;
					}
				}
			if(hIndex)
				GlobalUnlock(hIndex) ;
			if(hActiveStr)
     		GlobalUnlock(hActiveStr) ;
    	AObjUnlockData (commObj, SGNLBLOCK) ;
			}
    AObjUnlockData (commObj, SGNLINFO) ;
    }
}
//
//Need to be modified
//
BOOL COMMUNICATION::InitComm(void) 
{
	BOOL error = FALSE ;
	
	hCommDev = CommOpen() ;
  if (hCommDev!=INVALID_PORT_HANDLE)
		{
    if (!UpdateCommState ())
			{
			CommClose() ;
			hCommDev = INVALID_PORT_HANDLE ;
			portOpened = FALSE ;
			}
		}
	return (error) ;
}
HANDLE COMMUNICATION::CommOpen(void) 
{
	HANDLE handle ;

	//handle = OpenComm((LPCSTR)cstPorts[data0->port.Ports], inbuf, outbuf) ;
	handle = CreateFile (
										cstPorts[data0->port.Ports], 
										GENERIC_READ|GENERIC_WRITE, 
										NULL,
										0, 
										OPEN_EXISTING, 
										FILE_ATTRIBUTE_NORMAL, 
										NULL) ;
	if (handle==INVALID_PORT_HANDLE)
		{
		GetLastError() ;
		}
	else
		{
		portOpened = TRUE ;
		SetupComm (handle, inbuf, outbuf) ; 			//??? 
		SetCommMask(handle, COMM_EVENTMASK) ;
		}	
	return handle ;
}
BOOL COMMUNICATION::UpdateCommState() 
{
	PORT port=data0->port ;
  BOOL error = FALSE ;

	if (GetCommState(hCommDev, &commDCB))
		{
		commDCB.DCBlength        = sizeof(DCB) ;
	 	commDCB.BaudRate         = (DWORD)atoi(cstBaudrate[port.BaudRate]) ;
		commDCB.fBinary          = TRUE ;
		commDCB.fParity          = port.ChkParity ;
		commDCB.XonLim           = (WORD)(inbuf/4);//* distance from queue empty to Xon
		commDCB.XoffLim          = (WORD)COMM_RXROOM;
		commDCB.ByteSize         = (BYTE)atoi(cstDatabits[port.DataBits]) ;
		commDCB.Parity           = port.Parity ;
		commDCB.StopBits         = port.StopBits ;
		commDCB.XonChar          = 0x11 ;
		commDCB.XoffChar         = 0x13	;
		commDCB.ErrorChar        = 0	 ; //proboly need to be set
//  commDCB.EofChar          =
//	commDCB.EvtChar          =
		SetHandShake (port.handshake) ;
		error = SetCommState (hCommDev,&commDCB) ;
		}
  return error ;
}
int COMMUNICATION::CommClose (void)
{
	int rVal = COMM_ERROR ;

	if (hCommDev!=INVALID_PORT_HANDLE)
   	{
		PurgeComm(hCommDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR) ;//??? PORT
		if (CloseHandle (hCommDev))
			{
			hCommDev = INVALID_PORT_HANDLE ;
			portOpened = FALSE ;
			rVal = COMM_OK ;
			}
		}
	return rVal ;
}
//
//Need to get last error 
//
BOOL COMMUNICATION::SetBaudRate (DWORD BaudRate)
{
	commDCB.BaudRate = BaudRate ;
	return (SetCommState (hCommDev, &commDCB));
}
BOOL COMMUNICATION::SetByteSize (BYTE ByteSize) 
{
	commDCB.ByteSize = ByteSize ;
	return (SetCommState (hCommDev, &commDCB)) ;
}
BOOL COMMUNICATION::SetStopBits (BYTE StopBits)
{
 	commDCB.StopBits = StopBits ;
	return (SetCommState (hCommDev, &commDCB)) ;
}
BOOL COMMUNICATION::SetParity (BYTE Parity)
{	
	commDCB.Parity = Parity ;
	return (SetCommState (hCommDev, &commDCB)) ;
}
BOOL COMMUNICATION::SetHandShake(int handshake) 
{
	int result ;
  int rVal ;

	switch(handshake)
  	{
 		case NONE:
			commDCB.fOutxCtsFlow      = 0 ;
			commDCB.fOutxDsrFlow      = 0 ;
			commDCB.fDtrControl			  = 0 ;
			commDCB.fDsrSensitivity   = 0 ;
			commDCB.fTXContinueOnXoff = 0 ;
			commDCB.fOutX             = 0 ;
			commDCB.fInX              = 0 ;
			commDCB.fErrorChar        = 0 ;
			commDCB.fNull             = 0 ;
			commDCB.fRtsControl       = 0 ;
	 		commDCB.fAbortOnError     = 0 ;

			/*commDCB.fDtrflow = 0 ;         //incomming flow control
 			commDCB.fRtsflow = 0 ;
			commDCB.fInX     = 0 ;
			commDCB.fOutxCtsFlow = 0 ;     //outcomming flow control
			commDCB.fOutxDsrFlow = 0 ;
			commDCB.fOutX        = 0 ;*/
			break;

		case HARDWARE:
			commDCB.fOutxCtsFlow      = 0 ;
			commDCB.fOutxDsrFlow      = 0 ;
			commDCB.fDtrControl			  = 0 ;
			commDCB.fDsrSensitivity   = 0 ;
			commDCB.fTXContinueOnXoff = 0 ;
			commDCB.fOutX             = 0 ;
	 		commDCB.fInX              = 0 ;
			commDCB.fErrorChar        = 0 ;
			commDCB.fNull             = 0 ;
			commDCB.fRtsControl       = 0 ;
			commDCB.fAbortOnError     = 0 ;
			/*commDCB.fDtrflow = 0 ;         //incomming flow control
			commDCB.fRtsflow = 1 ;
			commDCB.fInX     = 0 ;
			commDCB.fOutxCtsFlow = 1 ;     //outcomming flow control
			commDCB.fOutxDsrFlow = 0 ;
			commDCB.fOutX        = 0 ;*/
      break;

		case XONXOFF:
			commDCB.fOutxCtsFlow      = 0 ;
			commDCB.fOutxDsrFlow      = 0 ;
			commDCB.fDtrControl			  = 0 ;
			commDCB.fDsrSensitivity   = 0 ;
			commDCB.fTXContinueOnXoff = 0 ;
			commDCB.fOutX             = 1 ;
			commDCB.fInX              = 1 ;
			commDCB.fErrorChar        = 0 ;
			commDCB.fNull             = 0 ;
			commDCB.fRtsControl       = 0 ;
			commDCB.fAbortOnError     = 0 ;
			/*commDCB.fDtrflow = 0 ;         //incomming flow control
			commDCB.fRtsflow = 0 ;
			commDCB.fInX     = 1 ;
			commDCB.fOutxCtsFlow = 0 ;     //outcomming flow control
			commDCB.fOutxDsrFlow = 0 ;
			commDCB.fOutX        = 1 ;*/
      break;
  	}

  //*
  //* Set the handshake method.
  //*
	return (SetCommState(hCommDev, &commDCB)) ;
}
int COMMUNICATION::GetHandShake(void) //need modify
{
/*	DCB dcb ;
	int handshake = -1 ;
	//PORT port = data0->port;
	//return (port.handshake) ;
	if (GetCommState (hCommDev, &dcb)==0)
  	{
		if (dcb.fInX==1&&dcb.fOutX==1) handshake=1 ;
		if (dcb.fRtsflow==1||	dcb.fOutxCtsFlow ==1) handshake =2 ;   //need modify
    }
	return (handshake) ;*/
}
BOOL COMMUNICATION::SetXonXoff(int xonxoff) //DCB* commDCB)
{
	commDCB.fInX  = (xonxoff!=0) ;
	commDCB.fOutX = (xonxoff!=0) ;
	//*
  //* Set the handshake method.
  //*
	return (SetCommState(hCommDev, &commDCB)) ;
}
BOOL COMMUNICATION::LineBreak (int state)
{
  BOOL rVal;
        
  if( state == COMM_BREAK_ON )
    rVal = SetCommBreak( hCommDev );
  else
    rVal = ClearCommBreak( hCommDev );

  return (rVal) ; 
}
DWORD COMMUNICATION::RxCount (void)
{
	COMSTAT comStat ;
 	DWORD   commError ;

 	if (ClearCommError (hCommDev, &commError, &comStat))
		return (comStat.cbInQue) ;
}
DWORD COMMUNICATION::TxCount (void)
{
	COMSTAT comStat ;
	DWORD   commError ;

	if (ClearCommError (hCommDev, &commError, &comStat))
		return (comStat.cbOutQue) ;
}
BOOL COMMUNICATION::FlushQueue (int fnQueue)
{
 	return (PurgeComm(hCommDev, fnQueue?PURGE_TXCLEAR:PURGE_RXCLEAR)) ;
}
DWORD COMMUNICATION::GetBuffer( BYTE *buffer, DWORD bytesToRead)
{
  DWORD   bytesRead =0 ;

  ReadFile(hCommDev, buffer, bytesToRead, &bytesRead, NULL) ;
	return bytesRead;
}
DWORD COMMUNICATION::GetString (BYTE* string, DWORD bytesToRead)
{
	DWORD bytesRead =0 ;

	bytesRead = GetBuffer(string, bytesToRead-1) ;
	if (bytesRead>0)
		string[bytesRead] = '\0' ;
	return (bytesRead) ;
}
DWORD COMMUNICATION::GetBufferTimeout(BYTE *buffer, DWORD bytesToRead, DWORD mesc) //WaitChar     
{
	COMMTIMEOUTS commTimeout ;
	DWORD        bytesRead = 0 ;

	GetCommTimeouts (hCommDev, &commTimeout) ;
	commTimeout.ReadTotalTimeoutConstant = mesc ;
	SetCommTimeouts (hCommDev, &commTimeout) ;
	bytesRead = GetBuffer (buffer, bytesToRead) ;
	ClearCommTimeouts () ;																				                       

  return (bytesRead) ;
}

int COMMUNICATION::GetChar( void )
{
	BYTE   c ;
 	DWORD  n ;

	ReadFile (hCommDev, &c, 1, &n, NULL) ;
	if (n<=0)
	 	c = (BYTE)COMM_ERROR ; 
	return ((int)c) ;
}
int COMMUNICATION::GetCharTimeout(DWORD mesc) //WaitChar     
{
	COMMTIMEOUTS commTimeout ;
	char         c ;

	GetCommTimeouts (hCommDev, &commTimeout) ;
	commTimeout.ReadTotalTimeoutConstant = mesc ;
	SetCommTimeouts (hCommDev, &commTimeout) ;
	c = (char)GetChar () ;
	ClearCommTimeouts () ;

  return (c) ;
}
BOOL COMMUNICATION::ClearCommTimeouts(void)
{
	COMMTIMEOUTS commTimeout ;

	GetCommTimeouts (hCommDev, &commTimeout) ;
	commTimeout.ReadIntervalTimeout = 0 ;
	commTimeout.ReadTotalTimeoutMultiplier = 0 ;
	commTimeout.ReadTotalTimeoutConstant = 0 ;
	commTimeout.WriteTotalTimeoutMultiplier = 0 ;
	commTimeout.WriteTotalTimeoutConstant = 0 ;

	return (SetCommTimeouts (hCommDev, &commTimeout)) ;
}
DWORD COMMUNICATION::PutBuffer( const BYTE* buf, DWORD bytesToWrite )
{
	COMSTAT que ;
	DWORD   dwError ;
	DWORD   bytesWrite = 0;
	DWORD   n ;

  //*
  //* Get the error status and amout of room in the tx queue.
  //*
	ClearCommError(hCommDev, &dwError, &que );
  bytesWrite = inbuf - que.cbOutQue;
	if( bytesToWrite < bytesWrite )
    bytesWrite = bytesToWrite;

  //*
  //* Write the buffer out the port.
  //*
	if (WriteFile (hCommDev, buf, bytesWrite, &n, NULL))
		{
		bytesWrite = n ;
		}
 
  return bytesWrite ;
}
DWORD COMMUNICATION::PutString(const BYTE *string)
{
	return (PutBuffer(string, lstrlen( (char *)string )));
}
DWORD COMMUNICATION::PutBufferTimeout (BYTE *buffer, DWORD bytesToWrite, DWORD mesc)
{
	COMMTIMEOUTS commTimeout ;
	DWORD        bytesWrite ;

	GetCommTimeouts (hCommDev, &commTimeout) ;						        		
	commTimeout.WriteTotalTimeoutConstant = mesc ;
	SetCommTimeouts (hCommDev, &commTimeout) ;
	bytesWrite = PutBuffer (buffer, bytesToWrite) ;
	ClearCommTimeouts () ;
	return (bytesWrite) ;
}
int COMMUNICATION::PutChar( BYTE b )
{
	int rVal = COMM_ERROR ;
	if( PutBuffer(&b, 1) == 1 )
    rVal = COMM_OK ;
	return rVal ;
}
int COMMUNICATION::PutCharTimeout (BYTE c, DWORD mesc)
{
	COMMTIMEOUTS commTimeout ;
	int          rVal ;

	GetCommTimeouts (hCommDev, &commTimeout) ;						        		
	commTimeout.WriteTotalTimeoutConstant = mesc ;
	SetCommTimeouts (hCommDev, &commTimeout) ;
	rVal = PutChar (c) ;
	ClearCommTimeouts () ;
	return (rVal) ;
}
int COMMUNICATION::SetEvent(DWORD events)
{
		SetCommMask(hCommDev, events) ;
		currentEvents = events ;
		return COMM_OK ;
}
/*int COMMUNICATION::GetPauseStatus(void)
{
	COMSTAT comStat;
        
  //*
  //* Get the port info, since we also get comm error information
  //* store it away also.
  //*
	nCommError |= GetCommError( hCommDev, &comStat );
	return(nCommError);
}*/
//////////////////////////////////////////////////////////////////////////
//Modem Setting
//////////////////////////////////////////////////////////////////////////
/*int COMMUNICATION::WaitForResponse (void)
{
	char buffer[81] ;
	GetBufferTimeout (buffer, 81, 2000) ;
}	*/
int COMMUNICATION::ModemInit (void)
{
	char *filling ;
	int  rVal = COMM_ERROR ;

	if (portOpened)
		{
    filling = new char[40] ;
		wsprintf (filling, "ATQ0V1E1X4S0=%d", 0) ;
		if (listenFlag)
			wsprintf (filling, "ATQ0M0V1E1X4S0=%d", data0->modem.AnsOrDial?0:data0->modem.RingTimes) ;
		strcat (filling, "\r") ;
		//PutBufferTimeout ((BYTES*)filling, strlen(filling), 600) ;
		//rVal = WaitForResponse () ;
		PutString ((BYTE*)filling) ;
		IdleSystem (600) ;
		rVal = COMM_OK ;
    delete[] filling ;
		}
	return rVal ;
}

void COMMUNICATION::SetAnsOrDialParam (BOOL listenFlag)
{
	data0->modem.AnsOrDial = !listenFlag ;
}
BOOL COMMUNICATION::Dial (void)
{
	char phoneNumber[256] ;
	BOOL ok = FALSE ;

	checkResultCode = FALSE ;
	if (data0->checkModem)
		{
		if (portOpened)
    	{
			ModemInit () ;
			if (data0->modem.AnsOrDial)
			  {
				if (GetPhoneNumber(phoneNumber)) 
					{
					checkResultCode = TRUE ;
					ModemDial(phoneNumber) ;
					}
				}
			ok = TRUE ;
			}
		}
	else
		ok = TRUE ;
	return (ok) ;
}
BOOL COMMUNICATION::GetPhoneNumber(char *phoneNumber)
{
	char* lpPhone ;
	BOOL  ok=FALSE ;

	lpPhone = (LPSTR) AObjLockData (commObj, PHONEN) ;
	if (lpPhone)
		{
		lstrcpy (phoneNumber, lpPhone) ;
		AObjUnlockData (commObj, PHONEN) ;
		ok = TRUE ;
		}
	else
	  {
		ok = DialogBoxParam(hInstance, "askPhone", GetActiveWindow(), (DLGPROC)AskPhoneDialog, (LPARAM)phoneNumber) ; 
		if (ok)
		  {
		  AObjSetData (commObj, PHONEN, phoneNumber, lstrlen(phoneNumber)) ;
		  }
		}
	return(ok) ;
}
int COMMUNICATION::ModemAutoAnswer (BOOL listen)
{
	char *filling ;
	int   retval = COMM_ERROR ;

	if (portOpened&&!data0->modem.AnsOrDial)
		{
    filling = new char[40] ;
		listenFlag = listen ;
    wsprintf (filling, "%s%d\r", "ATS0=", listen?data0->modem.RingTimes:0) ;
		//strcat (filling, "\r") ;
		PutString((BYTE*)filling);
		retval = COMM_OK ;
    delete[]filling ;
		}
	return (retval) ;
}
int COMMUNICATION::ResetModem(void)
{
	char *filling ;
	int   retval = COMM_ERROR  ;

	if (portOpened)
		{
    filling = new char[40] ;
		lstrcpy (filling, "ATZ\r") ;
		PutString((BYTE*)filling) ;
		retval = COMM_OK ;
    delete[]filling ;
		}
	return (retval) ;
}
void COMMUNICATION::DoCheckModemResult (void)
{
	int errorCode ;
	if (checkResultCode)
	  {
	  errorCode = GetResultCode ();
		if (errorCode>0)
		  {
			if (data0->modem.Redial)
				{
				IdleSystem ((data0->modem.seconds-2.5)*1000) ;
				if(errorCode>NOCARRIER)
			  	{
			  	ModemRedial() ;
        	if (--redialTimes<0)
						checkResultCode = FALSE ;
			  	}
				}
			if (errorCode>NOCARRIER&&redialTimes<0)
		  	{
		  	if (hCommDev=INVALID_PORT_HANDLE)
		    	{
					EscapeCommFunction (hCommDev, CLRDTR|CLRRTS) ;
		 			CloseHandle(hCommDev) ;
					hCommDev = INVALID_PORT_HANDLE ;
					} 
		  	}
			if (errorCode<NOCARRIER)
				checkResultCode = FALSE ;
			}
	  }
}
int COMMUNICATION::SetResultCode (int code)
{
	char *filling ;
  int   retval = COMM_ERROR ;

	if (portOpened)
		{
    filling = new char[40] ;
		wsprintf (filling, "%s%d\r", "ATV",code) ;
		PutString((BYTE*)filling) ;
		retval = COMM_OK ;
    delete[]filling ;
		}
	return (retval) ;
}
int COMMUNICATION::ModemHangUp(void)
{
	char *szTemp ;
	int   rVal=COMM_ERROR  ;

	if (portOpened)
	  {
		if(data0->checkModem)
			{
   	 	szTemp = new char[256] ;
			PutString((BYTE*)"+++") ;
			IdleSystem (2000) ;
			lstrcpy (szTemp, "ATH\r") ;
			PutString ((BYTE*)szTemp) ;
			IdleSystem (1800) ;
    	delete[]szTemp ;
			}
			rVal = COMM_OK ;
		}
	return rVal ;
}
int COMMUNICATION::ModemDial(char *phoneNumber)
{
	int   retval = COMM_ERROR;
	char *filling;

	//*
  //* Do some parameter checking.
	//*
 	if ( phoneNumber == NULL )
  	return( retval);
	if (portOpened)
		{
    filling = new char[40] ;
		if(data0->modem.DialMethod==TONE )
    	lstrcpy( filling, "ATDT" );
  	else
   		lstrcpy( filling, "ATPT" );
		if ( *filling )
			{
			strcat( filling, phoneNumber );
      strcat (filling, "\r") ;
			PutString((BYTE*)filling);
			IdleSystem (2500) ;
     	retval = COMM_OK ;
			}
    delete[]filling ;
		}
	return (retval) ;
}
int COMMUNICATION::ModemRedial( void )
{
	char *lpPhone ;
	int ok ;
	lpPhone = (char*) AObjLockData (commObj, PHONEN) ;
	ModemInit() ;
	ok = ModemDial(lpPhone) ;
	AObjUnlockData (commObj, PHONEN) ;

	return( ok);
}
void COMMUNICATION::ConvertString (char* szString)
{
	int nIn=0 ;
	int nOut= 0 ;

	while (szString[nIn])
		{
		if (szString[nIn]=='^')
			{
			if (szString[nIn+1]=='M')
				{
				szString[nOut]='\r' ;
        nIn++ ;
        }
			}
		nIn++ ;
    nOut++ ;
		}
	szString[nOut]='\0' ;
} 
void COMMUNICATION::IdleSystem(int msec)
{
	DWORD start ;

	start =GetTickCount() ;
	while (GetTickCount()<start+msec)
	{
	}
}
BOOL COMMUNICATION::ListenACall ()
{
	int  resultCode ;
	int  baudrate ;
	BOOL rVal = FALSE ;

	resultCode=GetResultCode () ;
	if (listenFlag)
	  {
		if (resultCode==10)
		  {
			listenFlag = FALSE ;
			IdleSystem (300) ;				//delay 0.3sec to get the result
			}
		}
	else
	  {
		if (resultCode<8)
		  {
			baudrate = (UINT)atoi(cstBaudrate[resultCode]) ;
			SetBaudRate(baudrate) ;
			AEvtPostSignalAtTail (commObj, 1) ;
			rVal = TRUE ;
			}
		if (resultCode==-1)
			rVal = TRUE ;
    }
	return(rVal);
	
}
int COMMUNICATION::GetResultCode (void)
{
	LPSTR cutstr1, cutstr2 ;
	char  resultCode [30] ;
	int   result ;
	int   index ;
	int   size = sizeof (cstResultCode)/sizeof(cstResultCode[0]) ;

	_fstrcat (resultStr, lpCommMem) ;

  cutstr1 = resultStr ;
	for (index=0; index<size; index++)
	  {
		lstrcpy (resultCode, cstResultCode[index]) ;
		strcat (resultCode, "\r") ;
		cutstr2 = StrCmp (resultStr, resultCode, TRUE, &result) ;
		if (result==1)
		  {
			cutstr1 += lstrlen (cutstr1) ;
			break	;
			}
		else if (result==-1)
			{
			if (cutstr1 >cutstr2)
     		cutstr1 = cutstr2 ;
			}
		else
   		cutstr1 +=lstrlen(cutstr1) ;
		}
  if (*cutstr1)
		MoveMemory (resultStr, cutstr1, lstrlen(cutstr1)) ;
	else
		*resultStr='\0' ;
	return(index<size?index:-1);
}
void COMMUNICATION::SetFoundString (LPSTR foundStr)
{
	AOBJECTID foundStrObj = data0->signal.FoundString ;
	OTextSetValueFromBuffer (foundStrObj, foundStr) ;
	AEvtPostStandard (foundStrObj, AEVENT_OBJECTCHANGED, 0, 0) ;
}
void COMMUNICATION::SetFoundIndex (int index) 
{
	AOBJECTID foundIndexObj = data0->signal.FoundIndex ;
  if (foundIndexObj)
  	ONmbrSetInteger (foundIndexObj, index, FALSE) ;
}
int COMMUNICATION::ReadPortToMem(void)
{
	int numChars = 0 ;
	 
	if(portOpened)
		{
		if ((numChars = RxCount()) >0)
			{
			numChars = GetBuffer((BYTE*)lpCommMem, numChars) ;
			lpCommMem[numChars] = '\0' ;
			//AEvtPostSignalAtMark (commObj, 2) ;
			}
		}
	return numChars ;
}
void COMMUNICATION::DoIdle (void)
{
	DoTerminal() ;
	//DoListen() ;
	DoCheckModemResult () ;
	DoSearch() ;
	SetInteText() ;
}
void COMMUNICATION::SetInteText(void)
{
	BOOL     SetInte = data0->signal.checkBox&INTERCEPT ;
	AOBJECTID inteObj = data0->signal.InteText ;

	AEvtPostSignalAtMark (commObj, 2) ;               //Send Intercept Signal 
	if (SetInte)
  	{
		if (OTextSetValueFromBuffer (inteObj, lpCommMem))
   		{
    	AEvtPostStandard(inteObj, AEVENT_OBJECTCHANGED, 0, AEVT_KILLDUPLICATES);
			}
    }
}

void COMMUNICATION::DoSearch(void)
{
	LPSTR cutstr1, cutstr2 ;
  LPSTR activeStr ;
  int  *index ;             //Pointer signal index array
	int   result ;
  BOOL  compareBits ;
	WORD  searchStrLen = _fstrlen (searchStr) ;
	WORD  activeStrPos ;

	if (!hIndex||!hActiveStr)
		return ;
	_fstrcat (searchStr, lpCommMem) ;

  cutstr1 = searchStr ;
	index = (int *)GlobalLock(hIndex) ; 
  activeStr = (LPSTR) GlobalLock(hActiveStr) ;
	if (index&&activeStr)
  	{
		while (*index)
			{
			compareBits = *index>0?TRUE:FALSE ;
			cutstr2 = StrCmp (searchStr, activeStr, compareBits, &result) ;
			if (result==1)
				{
				AEvtPostSignalAtTail (commObj, (*index)+3) ;
				SetFoundString (activeStr) ;
				activeStrPos =(cutstr2-searchStr+1)-(searchStrLen-_fstrlen(activeStr)); //Found the index of the last character                   
				SetFoundIndex ((int)activeStrPos) ;																		  		//of active string (searching string) in the current data entry buffer
				//cutstr1+=lstrlen(cutstr1) ;
				cutstr1 = cutstr2+lstrlen(activeStr) ;
				}
			else if (result==-1)
				{
				if (cutstr1 >cutstr2)
					{
        	cutstr1 = cutstr2 ;
					} 
				}
			else
      	cutstr1 +=lstrlen(cutstr1) ;
			index++ ;
      activeStr+=_fstrlen(activeStr)+1 ;
			}
    if (*cutstr1)
			MoveMemory (searchStr, cutstr1, lstrlen(cutstr1)+1) ;
		else
			*searchStr='\0' ;
    }
		GlobalUnlock (hIndex) ;
    GlobalUnlock (hActiveStr) ;
}
void COMMUNICATION::DoListen(void)
{
	if (listenFlag)
		ListenACall() ;
}
void COMMUNICATION::DoTerminal(void)
{
	long theGetKey = -1 ;
  AOBJECTID oiTerm ;

	while ((oiTerm=AObjGetNextObject(OTYPE_TERMINAL, &theGetKey))!=NULL)
		{
		AEvtPostStandard(oiTerm, idleTerminal, (long)commObj, AEVT_ATTAIL) ;
		}
}
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
BOOL ALMCALLBACK AskPhoneDialog(HWND hDlg, WORD message,
													WORD wParam, LONG lParam)

{
	char * phoneNumber ;
	WORD	cmdID;

	switch (message)
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			return FALSE ;

		case WM_COMMAND:
			cmdID = GET_WM_COMMAND_ID(wParam, lParam);
			switch(cmdID)
				{
			 	case idAskPhone:
					if (GetWindowTextLength(GetDlgItem(hDlg, idAskPhone)))
						EnableWindow (GetDlgItem (hDlg, IDOK), TRUE);
					return FALSE ; 			

		       	case IDOK:
					phoneNumber = (char*)GetWindowLong(hDlg, DWL_USER) ;
					GetWindowText(GetDlgItem(hDlg, idAskPhone), phoneNumber, 256) ;
			 	 	EndDialog (hDlg, TRUE) ;
		 		 	return TRUE ;

				case IDCANCEL:
					EndDialog (hDlg, FALSE) ;
					return TRUE ;

				}
    }
    return FALSE ;
}




