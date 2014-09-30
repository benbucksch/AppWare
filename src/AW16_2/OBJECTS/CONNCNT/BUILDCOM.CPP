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
COMMUNICATION::COMMUNICATION(OBJECTID oiObj, int inBufSize, int outBufSize)
{
	commObj = oiObj ;
	data0	 = (DATA0*)AObjLockData(commObj, CONNDATA);
	inbuf  = inBufSize ;
	outbuf = outBufSize ;
	connbusy = FALSE ;
	idCommDev = -1 ;
	nCommError = 0x00;
	listenFlag = FALSE ;
	redialTimes = data0->modem.Redial?data0->modem.RedialTimes:-1 ;
	hCommMem = GlobalAlloc (GHND, inBufSize-200) ;
	lpCommMem = (LPSTR) GlobalLock (hCommMem) ;
	*lpCommMem = '\0' ;
	hResult = GlobalAlloc (GHND, inBufSize-200) ;
	resultStr = (LPSTR) GlobalLock (hResult) ;
	*resultStr = '\0' ;
	hSearch = GlobalAlloc (GHND, inBufSize-200) ;
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
	if (idCommDev>=0)
		{
		CommClose() ;
		idCommDev = -1 ;
		}
  connbusy = FALSE ;
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
			hIndex = GlobalAlloc (GHND, (long)data0->signal.signalCount*sizeof(int)) ;
			index = (int*)GlobalLock (hIndex) ;
			hActiveStr = GlobalAlloc (GHND, blockSize) ; 
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
						_fstrcpy(activeStr, signalBlock) ;
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
int COMMUNICATION::InitComm(void) 
{
	int err ;
	idCommDev = CommOpen() ;
  if (idCommDev>=0)
		{
    err = ChangeCommState () ;
		if (err<0)
			{
      err = COMM_ERROR ;	
			idCommDev = -1 ;
			}
		else
    	{
			connbusy = TRUE ;
      err = COMM_OK ;
      }
		}
	else
  	{
		idCommDev = -1 ;
    err=COMM_ERROR ;
    }
	return (err) ;
}
int COMMUNICATION::CommOpen(void) 
{
	int handle ;

	handle = OpenComm((LPCSTR)cstPorts[data0->port.Ports], inbuf, outbuf) ;
	if (handle<0)
    {
		if (handle==IE_HARDWARE)
			handle = COMM_HARDWARE ;
		else if(handle==IE_MEMORY)
			handle = COMM_MEMORY ;
		else if(handle==IE_NOPEN)
			handle = COMM_NOPEN ;
		else if(handle==IE_OPEN)
			handle = COMM_OPEN ;
		else
			handle = COMM_ERROR ;
		}
 else
 	{
	connbusy = TRUE ;
	SetCommEventMask(handle, COMM_EVENTMASK) ;
  }
	return handle ;
}
int COMMUNICATION::ChangeCommState(void) 
{
	DCB  dcb ;
	PORT port=data0->port ;
  int  error ;
	GetCommState(idCommDev, &dcb) ;

	dcb.BaudRate = (UINT)atoi(cstBaudrate[port.BaudRate]) ;
	dcb.ByteSize = (BYTE)atoi(cstDatabits[port.DataBits]) ;
	dcb.StopBits = port.StopBits ;
	dcb.Parity   = port.Parity ;
	dcb.fOutX    = 0;                 //* output Xon/Xoff flow control off
	dcb.fInX     = 0;                 //* input Xon/Xoff flow control off
	dcb.fRtsflow = 0;                 //* hardware handshaking off
	dcb.fDtrflow = 0;
	dcb.RlsTimeout = 0;
	dcb.CtsTimeout = 0;
	dcb.DsrTimeout = 0;
	dcb.fBinary    = 1;
  dcb.fParity    = port.ChkParity ;
	dcb.fRtsDisable = 0;
	dcb.fDtrDisable = 0;
	dcb.fPeChar     = 0;
	dcb.fNull       = 0;
	dcb.fChEvt      = 0;
	dcb.PeChar      = 0;
	dcb.EofChar     = 0;
	dcb.EvtChar     = 0;
	dcb.XonChar     = 0x11;           //* specify the Xon character
	dcb.XoffChar    = 0x13;           //* specify the Xoff character
	dcb.XonLim      = (unsigned short)(inbuf/4);//* distance from queue empty to Xon
	dcb.XoffLim     = (unsigned short)COMM_RXROOM; //* distance from queue full to Xoff
  SetHandShake (port.handshake) ;
	error = SetCommState(&dcb) ;
  return error ;
}
int COMMUNICATION::CommClose (void)
{
	int error ;
	int rVal = COMM_ERROR ;

	if (idCommDev>=0)
   	{
		error = FlushComm(idCommDev, 0) ;
		error = FlushComm(idCommDev, 1) ;
		error = CloseComm(idCommDev);
		}
	else
		error = -1 ;
  idCommDev = -1 ;
	connbusy = FALSE ;
	if(error==0) rVal = COMM_OK ;
	return rVal ;
}
int COMMUNICATION::SetBaudRate (int BaudRate)
{
	DCB tmpdcb ;
	int result ;
  int rVal ;

	GetCommState (idCommDev, &tmpdcb) ;
	tmpdcb.BaudRate = BaudRate ;
	result = SetCommState (&tmpdcb) ;

	if( result != 0 )
    rVal = COMM_ERROR ;
  else
		rVal = COMM_OK ;

	return rVal ;
}
int COMMUNICATION::SetStopBits (int StopBits)
{
	DCB tmpdcb ;
	int result ;
  int rVal ;

	GetCommState (idCommDev, &tmpdcb) ;
	tmpdcb.StopBits = StopBits ;
	result = SetCommState (&tmpdcb) ;

	if( result != 0 )
    rVal = COMM_ERROR ;
  else
		rVal = COMM_OK ;

	return rVal ;
}
int COMMUNICATION::SetParity (int Parity)
{
	DCB tmpdcb ;
	int result ;
  int rVal ;

	GetCommState (idCommDev, &tmpdcb) ;
	tmpdcb.Parity = Parity ;
	result = SetCommState (&tmpdcb) ;

	if( result != 0 )
    rVal = COMM_ERROR ;
  else
		rVal = COMM_OK ;

	return rVal ;
}
int COMMUNICATION::SetHandShake(int handshake) //DCB* commDCB)
{
	DCB tmpdcb ;
	int result ;
  int rVal ;

	GetCommState (idCommDev, &tmpdcb) ;

	switch(handshake)
  	{
		case NONE:
			tmpdcb.fDtrflow = 0 ;         //incomming flow control
			tmpdcb.fRtsflow = 0 ;
			tmpdcb.fInX     = 0 ;
			tmpdcb.fOutxCtsFlow = 0 ;     //outcomming flow control
			tmpdcb.fOutxDsrFlow = 0 ;
			tmpdcb.fOutX        = 0 ;
			break;

		case HARDWARE:
			tmpdcb.fDtrflow = 0 ;         //incomming flow control
			tmpdcb.fRtsflow = 1 ;
			tmpdcb.fInX     = 0 ;
			tmpdcb.fOutxCtsFlow = 1 ;     //outcomming flow control
			tmpdcb.fOutxDsrFlow = 0 ;
			tmpdcb.fOutX        = 0 ;
      break;
		case XONXOFF:
			tmpdcb.fDtrflow = 0 ;         //incomming flow control
			tmpdcb.fRtsflow = 0 ;
			tmpdcb.fInX     = 1 ;
			tmpdcb.fOutxCtsFlow = 0 ;     //outcomming flow control
			tmpdcb.fOutxDsrFlow = 0 ;
			tmpdcb.fOutX        = 1 ;
      break;
  	}

  //*
  //* Set the handshake method.
  //*
	result = SetCommState(&tmpdcb) ;//commDCB );
	if( result != 0 )
    rVal = COMM_ERROR ;
  else
		rVal = COMM_OK ;

	return rVal ;
}
int COMMUNICATION::GetParity (void)
{
	DCB dcb ;
	int Parity = -1 ;

	if (GetCommState (idCommDev, &dcb)==0)
  	{
		Parity = dcb.Parity ;
    }
	return (Parity) ;
}
int COMMUNICATION::GetStopBits (void)
{
	DCB dcb ;
	int StopBits = -1 ;

	if (GetCommState (idCommDev, &dcb)==0)
  	{
		StopBits = dcb.StopBits ;
    }
	return (StopBits) ;
}
int COMMUNICATION::GetHandShake(void)
{
	DCB dcb ;
	int handshake = -1 ;
	//PORT port = data0->port;
	//return (port.handshake) ;
	if (GetCommState (idCommDev, &dcb)==0)
  	{
		if (dcb.fInX==1&&dcb.fOutX==1) handshake=1 ;
		if (dcb.fRtsflow==1||	dcb.fOutxCtsFlow ==1) handshake =2 ;   //need modify
    }
	return (handshake) ;
}
int COMMUNICATION::SetXonXoff(int xonxoff) //DCB* commDCB)
{
	DCB tmpdcb ;
	int result ;
  int rVal ;

  GetCommState (idCommDev, &tmpdcb) ;  
	tmpdcb.fInX  = xonxoff ;
	tmpdcb.fOutX = xonxoff ;
	//*
  //* Set the handshake method.
  //*
	result = SetCommState(&tmpdcb) ;//commDCB );
	if( result != 0 )
    rVal = COMM_ERROR ;
  else
		rVal = COMM_OK ;

  return rVal ;
}
int COMMUNICATION::LineBreak (int state)
{
  int error;
        
  //*
	//* If the state is less than zero then return the current state of break.
  //*
  //if ( state < 0 )
	//  return( breakon );

  //*
  //* The break on or off depending on the state.
  //*
	if( state == COMM_BREAK_ON )
    error = SetCommBreak( idCommDev );
  else
    error = ClearCommBreak( idCommDev );

   
  //*
  //* Return the error of the function call or the state of the line.
  //*
  if( error == 0 )
    return( (unsigned short)state ); //breakon = 
	else
		return( COMM_BREAK_ERROR );
}
int COMMUNICATION::RxCount (void)
{
	COMSTAT comStat ;

	nCommError |= GetCommError (idCommDev, &comStat) ;
	//if (nCommError&CE_BREAK)   MessageBox (NULL, "1", "error", MB_OK) ;
	//if (nCommError&CE_CTSTO)   MessageBox (NULL, "2", "error", MB_OK) ;
	//if (nCommError&CE_DSRTO)   MessageBox (NULL, "3", "error", MB_OK) ;
	//if (nCommError&CE_FRAME)   MessageBox (NULL, "4", "error", MB_OK) ;
	//if (nCommError&CE_MODE)    MessageBox (NULL, "5", "error", MB_OK) ;
	//if (nCommError&CE_OVERRUN) MessageBox (NULL, "6", "error", MB_OK) ;
	//if (nCommError&CE_RLSDTO)  MessageBox (NULL, "7", "error",MB_OK) ;
	//if (nCommError&CE_TXFULL)  MessageBox (NULL, "8", "error", MB_OK) ;
	//if (nCommError&CE_RXOVER)  MessageBox (NULL, "RX is overflower","error", MB_OK) ;
	//if (nCommError&CE_RXPARITY) MessageBox (NULL, " Parity error", "error", MB_OK) ;
	return (comStat.cbInQue) ;
}
int COMMUNICATION::TxCount (void)
{
	COMSTAT comStat ;

	nCommError|=GetCommError (idCommDev, &comStat) ;
	return (comStat.cbOutQue) ;
}
int COMMUNICATION::FlushQueue (int fnQueue)
{
	COMSTAT comStat ;
  int     rVal = COMM_OK ;

	if (FlushComm(idCommDev, fnQueue)<0)
  	{
		nCommError|=GetCommError (idCommDev, &comStat) ;
    rVal = COMM_ERROR ;
    }
		return rVal ;
}
int COMMUNICATION::GetBuffer( BYTE *buffer, int char_count )
{
  COMSTAT que;

  int result = ReadComm( idCommDev, (char far *)buffer, char_count );
	if( result <= 0 ) {
    result = -result;
		nCommError |= (unsigned short)GetCommError( idCommDev, &que );
	}
  return( result );
}
int COMMUNICATION::GetString (BYTE* string, int n)
{
	int result ;

	result = GetBuffer(string, n-1) ;
	if (result>0)
		string[result] = '\0' ;
	return (result) ;
}
int COMMUNICATION::GetChar( void )
{
	COMSTAT que ;
	BYTE c='\0';
  int  n ;

	n = ReadComm (idCommDev, &c, 1) ;
	if (n<=0)
		{
		n=-n ;
		nCommError |= GetCommError( idCommDev, &que) ; //NULL );
		}
	return (c) ;
}
int COMMUNICATION::WaitChar(long mesc)
{
	int  rVal = COMM_OK ;
	long timeout ;
	BYTE c ;

	timeout = GetCurrentTime() +mesc ;
	for (;;)
		{
			if (ReadComm (idCommDev, &c, 1)>0)
				break ;
			if (timeout<GetCurrentTime())
      	{
				rVal = COMM_TIME_OUT ;
        break ;
				}
			RxCount();
		}
	if (rVal==COMM_OK)
		return (int)c ;
	else
		return rVal ;
}
int COMMUNICATION::PutBuffer( const BYTE* buf, int n )
{
	COMSTAT que ;
  int amount_to_write ;

  //*
  //* Get the error status and amout of room in the tx queue.
  //*
	nCommError |= GetCommError( idCommDev, &que );
  amount_to_write = inbuf - que.cbOutQue;
	if( n < amount_to_write )
    amount_to_write = n;

  //*
  //* Write the buffer out the port.
  //*
	int status = WriteComm (idCommDev, (char far *)buf, amount_to_write);
	if (status<=0)
		{
		nCommError |= GetCommError( idCommDev, &que );
    status = -status;
  	}
  return( status );
}
int COMMUNICATION::PutChar( BYTE b )
{
	int rVal = COMM_ERROR ;
	if( PutBuffer(&b, 1) == 1 )
    rVal = COMM_OK ;
	return rVal ;
}
int COMMUNICATION::PutString( const BYTE *string )
{
	return( PutBuffer( string, strlen( (char *)string ) ) );
}

int COMMUNICATION::SetEvent(UINT events)
{
		SetCommEventMask(idCommDev, events) ;
		currentEvents = events ;
		return COMM_OK ;
}
int COMMUNICATION::GetPauseStatus(void)
{
	COMSTAT comStat;
        
  //*
  //* Get the port info, since we also get comm error information
  //* store it away also.
  //*
	nCommError |= GetCommError( idCommDev, &comStat );
	return(nCommError);
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
		if (LinkOpen())
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
		strcpy (phoneNumber, lpPhone) ;
		AObjUnlockData (commObj, PHONEN) ;
		ok = TRUE ;
		}
	else
	  {
		ok = DialogBoxParam(hInstance, "askPhone", GetActiveWindow(), (DLGPROC)AskPhoneDialog, (LPARAM)phoneNumber) ; 
		if (ok)
		  {
		  AObjSetData (commObj, PHONEN, phoneNumber, strlen(phoneNumber)) ;
		  }
		}
	return(ok) ;
	
}
//////////////////////////////////////////////////////////////////////////
//Modem Setting
//////////////////////////////////////////////////////////////////////////
int COMMUNICATION::ModemInit (void)
{
	char *filling ;
	int  retval = COMM_ERROR ;

	if (LinkOpen())
		{
    filling = new char[40] ;
		sprintf (filling, "ATQ0V1E1X4S0=%d", 0) ;
		if (listenFlag)
			sprintf (filling, "ATQ0M0V1E1X4S0=%d", data0->modem.AnsOrDial?0:data0->modem.RingTimes) ;
		strcat (filling, "\r") ;
		PutString ((BYTE*)filling) ;
		IdleSystem (600) ;
		retval = COMM_OK ;
    delete[] filling ;
		}
	return retval ;
}
int COMMUNICATION::ModemAutoAnswer (BOOL listen)
{
	char *filling ;
	int   retval = COMM_ERROR ;

	if (LinkOpen()&&!data0->modem.AnsOrDial)
		{
    filling = new char[40] ;
		listenFlag = listen ;
    sprintf (filling, "%s%d\r", "ATS0=", listen?data0->modem.RingTimes:0) ;
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

	if (LinkOpen ())
		{
    filling = new char[40] ;
		strcpy (filling, "ATZ\r") ;
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
		  	if (idCommDev>-1)
		    	{
					EscapeCommFunction (idCommDev, CLRDTR|CLRRTS) ;
		 			CloseComm(idCommDev) ;
					idCommDev = -1 ;
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

	if (LinkOpen ())
		{
    filling = new char[40] ;
		sprintf (filling, "%s%d\r", "ATV",code) ;
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

	if (LinkOpen())
	  {
		if(data0->checkModem)
			{
   	 	szTemp = new char[256] ;
			PutString((BYTE*)"+++") ;
			IdleSystem (2000) ;
			strcpy (szTemp, "ATH\r") ;
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
	if (LinkOpen())
		{
    filling = new char[40] ;
		if(data0->modem.DialMethod==TONE )
    	strcpy( filling, "ATDT" );
  	else
   		strcpy( filling, "ATPT" );
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
		strcpy (resultCode, cstResultCode[index]) ;
		strcat (resultCode, "\r") ;
		cutstr2 = StrCmp (resultStr, resultCode, TRUE, &result) ;
		if (result==1)
		  {
			cutstr1 += strlen (cutstr1) ;
			break	;
			}
		else if (result==-1)
			{
			if (cutstr1 >cutstr2)
     		cutstr1 = cutstr2 ;
			}
		else
   		cutstr1 +=strlen(cutstr1) ;
		}
  if (*cutstr1)
		_fmemmove (resultStr, cutstr1, strlen(cutstr1)) ;
	else
		*resultStr='\0' ;
	return(index<size?index:-1);
}
void COMMUNICATION::SetFoundString (LPSTR foundStr)
{
	OBJECTID foundStrObj = data0->signal.FoundString ;
	OTextSetValueFromBuffer (foundStrObj, foundStr) ;
	AEvtPostStandard (foundStrObj, AEVENT_OBJECTCHANGED, 0, 0) ;
}
void COMMUNICATION::SetFoundIndex (WORD index) 
{
	OBJECTID foundIndexObj = data0->signal.FoundIndex ;
  if (foundIndexObj)
  	ONmbrSetInteger (foundIndexObj, index, FALSE) ;
}
int COMMUNICATION::ReadPortToMem(void)
{
	int numChars = 0 ;
	 
	if(LinkOpen())
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
	OBJECTID inteObj = data0->signal.InteText ;

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
				SetFoundIndex (activeStrPos) ;																		  		//of active string (searching string) in the current data entry buffer
				//cutstr1+=strlen(cutstr1) ;
				cutstr1 = cutstr2+strlen(activeStr) ;
				}
			else if (result==-1)
				{
				if (cutstr1 >cutstr2)
					{
        	cutstr1 = cutstr2 ;
					} 
				}
			else
      	cutstr1 +=strlen(cutstr1) ;
			index++ ;
      activeStr+=_fstrlen(activeStr)+1 ;
			}
    if (*cutstr1)
			_fmemmove (searchStr, cutstr1, strlen(cutstr1)+1) ;
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
  OBJECTID oiTerm ;

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
	switch (message)
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, DWL_USER, lParam) ;
			return FALSE ;

		case WM_COMMAND:
			switch(wParam)
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




