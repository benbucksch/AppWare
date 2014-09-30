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
// FILE:"func_conn.cpp"
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

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "commObj.h"

#define errorFormat1 "There is an error in the text specifying \
the connection Configuration at character position %d."
#define errorFormat2"An error occurred while attempting to set \
the connection configuration"

extern BOOL ALMCALLBACK CntObjProc(HWND, WORD, WORD, LONG) ;
extern long ChangeHexToDec (char *, char *) ;
extern void TextToHex (LPSTR, LPSTR, LPSTR, LONG) ;
//
//
//
char * GetCurrentString (
	LPSTR lpString, 
	int index)
{
	char *p ;
	int   i ;

	p = (char*)lpString ;
	for(i=0; i<index; i++)
		p += lstrlen(p)+1 ;

  return p ;
}
long GetActiveStringBlockSize(
	LPSTR lpActStr, 
	long* lpIndex)
{
	long size = 0 ;

	while(*lpIndex++)
		{
    size+=lstrlen(lpActStr++)+1 ;
		}
  return (size) ;
}		     
void SetupConfigureText (
	OBJECTID oiConn,
	LPSTR lpConfigure)
{
	DATA0* data0 ;
	char  version[10] ;
	char* baudrate ;
	char* databits ;
	char* parity   ;
	char* stopBits ;
	char* port ;
	char* handshake ;
	char  ChkParity[5] ;
	char  CrDetect[5] ;
	char* phonenumber ;
	char* dialtone ;
  char  retry[5] ;
	int   retrytimes ;
  int   ringtimes ;
	int   seconds ;

	data0 = (DATA0*)AObjLockData (oiConn, CONNDATA) ;
	if (data0)
		{
		itoa(data0->version, version, 16) ;
		MoveMemory (&version[2], &version[1], 3) ;
		version[1] = '.' ;
		baudrate = cstBaudrate[data0->port.BaudRate] ;
		databits = cstDatabits[data0->port.DataBits] ;
		parity   = cstParity[data0->port.Parity] ;
		stopBits = cstStopbits[data0->port.StopBits] ;
    port     = cstPorts[data0->port.Ports] ;
		handshake = csthandshake [data0->port.handshake] ;
		lstrcpy (ChkParity, data0->port.ChkParity?"TRUE":"FALSE") ;
		lstrcpy (CrDetect, data0->port.CrDetect?"TRUE":"FALSE") ;
		dialtone = cstDialM[data0->modem.DialMethod] ;
		lstrcpy (retry, data0->modem.Redial?"TRUE":"FALSE") ;
		retrytimes = data0->modem.RedialTimes ;
		ringtimes  = data0->modem.RingTimes ;
		seconds    = data0->modem.seconds ;
		if (data0->checkModem)
			{
      phonenumber =(LPSTR)AObjLockData (oiConn, PHONEN) ; 
			sprintf (lpConfigure,
							format1,
							version,
							baudrate,
							databits,
							parity,
							stopBits,
							port,
							handshake,
							ChkParity,
							CrDetect,
              (phonenumber)?phonenumber:"", 
							dialtone,
							retry,
							retrytimes,
							seconds,
							ringtimes) ;
			if (phonenumber)
				AObjUnlockData (oiConn, PHONEN) ;
			}
		else
    	{
			sprintf (lpConfigure,
							format2,
							data0->version,
							baudrate,
							databits,
							parity,
							stopBits,
							port,
							handshake,
							ChkParity,
							CrDetect
							) ;
      }
		AObjUnlockData (oiConn, CONNDATA) ;
    }
}
/////////////////////////////////////////////////////////////////////////////
// Function: GetDataString
//
/////////////////////////////////////////////////////////////////////////////
int GetDataString(							//Return >   Sucessful; The result is saved in the return value
	LPSTR lpConfigure, 						//Return =0  Sucessful; No result is saved in the return value
	char* text, 									//Return <0  Failed.
	char *result, 
	char* cstResult[], 
	int size)
{
	LPSTR lpstr ;
	char *p ;
  char  errorMsg[256] ;
	int   ok = 0 ;
	WORD  i ;
  int   charP=0 ;

  p = result ;
  *result ='\0' ; 
	lpstr = _fstrstr(lpConfigure, (LPSTR)text) ;
	if (lpstr)
		{
		charP = (int)(lpstr-lpConfigure) ;
    lpstr+=lstrlen(text) ;
		if (*(lpstr++)!=' ')
			{
      charP++ ;
			sprintf (errorMsg, errorFormat1, (lpstr-lpConfigure)) ;
			MessageBox (GetActiveWindow(), errorMsg, "Error", MB_OK) ;
			ok = -1 ;
			}
		else
			{
			while(*lpstr!='\0')
				{
        charP++ ;
				if (*lpstr==' ') //&&lstrlen(result)>0)
					{
					*result='\0' ;
					break ;
          }
				if (*lpstr!=' ')
					*result++=*lpstr ;
        lpstr++ ;
        }
			}
		if (*lpstr=='\0')
			*result='\0' ;
		result = p ;
		if (ok!=-1)
			{
			for (i=0; i<size; i++)
				{
      	ok = -1 ;
				if (!lstrcmp(strupr(cstResult[i]), result))
					{
        	ok = i ;
					break;
					}
				}
			if (ok==-1)
    		{
				sprintf (errorMsg, errorFormat1, (charP-lstrlen(result))) ;
				MessageBox (GetActiveWindow(), errorMsg, "Error", MB_OK) ;
      	}
			}
    }
  return (ok) ;
}
BOOL SaveConfigureText (
	OBJECTID oiConn, 
	LPSTR lpConfigure)
{
	BOOL   rVal = FALSE ;
	int    result ;
	DATA0 *data0 ;
	char   str[256] ;

	_fstrupr(lpConfigure) ;
	data0= (DATA0*)AObjLockData (oiConn, CONNDATA) ;
	if (data0)
		{
		result = GetDataString (lpConfigure, "BAUDRATE", str, cstBaudrate, 8) ; 
		if (result>0) 
			data0->port.BaudRate = result;
		result = GetDataString (lpConfigure, "DATABITS", str, cstDatabits, 4) ;
    if (result>0)
			data0->port.DataBits = result ;
		result = GetDataString (lpConfigure, "PARITY", str, cstParity, 5) ;
		if (result>0)
			data0->port.Parity = result ;
		result = GetDataString (lpConfigure, "STOPBITS", str, cstStopbits, 3) ;
    if (result>0)
			data0->port.StopBits = result ;
    result = GetDataString (lpConfigure, "PORT", str, cstPorts, 4) ;
		if (result>0)
			data0->port.Ports = result ;
		result = GetDataString (lpConfigure, "HANDSHAKE", str, csthandshake, 3) ;
		if (result==0)
			data0->port.handshake = result ;
		result = GetDataString (lpConfigure, "PARITY CHECK", str, NULL, 0) ;
		if (result==0)
			data0->port.ChkParity = (lstrcmp(str, "TRUE")==0)?TRUE:FALSE ;
		result = GetDataString (lpConfigure, "CARRIER DETECTED", str, NULL, 0) ;
		if (result==0)
			data0->port.CrDetect = (lstrcmp(str, "TRUE")==0)?TRUE:FALSE ;
		result = GetDataString (lpConfigure, "MODEM CONNECTION", str, cstModemConnctn, 2 ) ;
		if (result>0)
			{
      data0->checkModem = TRUE ;
			result = GetDataString (lpConfigure, "DIAL", str, cstDialM, 2 ) ;
			if (result>0) 
				data0->modem.DialMethod = result ;
			result = GetDataString (lpConfigure, "RETRY", str, NULL,0) ;
      if (result==0)
				data0->modem.Redial = (lstrcmp(str, "TRUE")==0)?TRUE:FALSE ;
			result = GetDataString (lpConfigure, "RETRYTIMES", str, NULL, 0) ;
			if (result==0)
				data0->modem.RedialTimes = atoi(str) ;
			result = GetDataString (lpConfigure, "RETRYINTERVAL", str, NULL, 0) ;
			if (result==0)									    
				data0->modem.seconds = atoi(str) ;
			result = GetDataString (lpConfigure, "WAITRINGS", str, NULL, 0) ;
			if (result==0)
				data0->modem.RingTimes =atoi(str) ;
			result = GetDataString (lpConfigure, "PHONENUMBER", str, NULL, 0) ;
			if (result==0)
				{
				if (lstrlen(str)!=0)
					{
					if (*str=='"'&& *(str+lstrlen(str)-1)=='"')
							{
							*(str+(lstrlen(str)-1)) = '\0' ;
							if (lstrlen(str+1)>0)
								AObjSetData (oiConn, PHONEN, str+1, lstrlen(str)+1);
							else
								{
								AObjSetData (oiConn, PHONEN, "", 1) ;
								MessageBox (GetActiveWindow(), "No phone number set", "Warning", MB_OK) ;
								}
							}
					else if(*str!='"'&& *(str+lstrlen(str)-1)!='"')
					  {
					  AObjSetData (oiConn, PHONEN, str, lstrlen(str)+1);
					  }
					else
						MessageBox (GetActiveWindow(), errorFormat2, "Error", MB_OK) ;
					}
				else
					{
					AObjSetData (oiConn, PHONEN, "", 1) ;
					MessageBox (GetActiveWindow(), "No phone number set", "Warning", MB_OK) ;
					}
				}
			}
    AObjUnlockData (oiConn, CONNDATA) ;
		rVal = TRUE ;
		}
	return(rVal);
	
}
BOOL DoActivateSearch (OBJECTID oiConn, int first, int last, BOOL activate)
{
  COMMUNICATION *comm ;
	SIGNALINFO *signalInfo ;
	LPSTR       signalBlock ;
	int*        index ;
  LPSTR       activeStr ;  
	BOOL        rVal = FALSE ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if (!comm)
		return rVal;
	signalInfo  = (SIGNALINFO*) AObjLockData (oiConn, SGNLINFO) ;
  signalBlock = (LPSTR) AObjLockData (oiConn, SGNLBLOCK) ;
	if (signalInfo)
  {
	if (signalBlock)
   	{
		int signalCount = comm->data0->signal.signalCount ;
		if (first<1||first>signalCount)
			return(FALSE);
		if (last<first)	
			last=first ;
		if (comm->hIndex)
    	{
			index =(int*) GlobalLock (comm->hIndex) ;
      activeStr = (LPSTR) GlobalLock (comm->hActiveStr) ;
			if (index&&activeStr)
      	{
				WORD i ;
    		WORD j = (WORD)first ;
				for (i=0; i<signalCount; i++ )
					{
					if(activate)
          	{
						if (signalInfo[i].active||(i+1==j))
            	{
							*index++ = signalInfo[i].CompareBits?
														 (signalInfo[i].signalIndex+1) :
														-(signalInfo[i].signalIndex+1) ;
							lstrcpy(activeStr, signalBlock) ;
							activeStr +=_fstrlen(activeStr)+1 ;
              }
						}
					else
						{
						if (signalInfo[i].active&&(i+1!=j))
            	{
							*index++ = signalInfo[i].CompareBits?
														 (signalInfo[i].signalIndex+1) :
														-(signalInfo[i].signalIndex+1) ;
							lstrcpy(activeStr, signalBlock) ;
							activeStr +=_fstrlen(activeStr)+1 ;
              }
            }
					if (i+1==j&&j<last+1)
						j++ ;
					signalBlock +=_fstrlen(signalBlock)+1 ;
					}
				*index = 0 ;
				rVal = TRUE ;
        }
			if(index) GlobalUnlock (comm->hIndex) ;
			if(activeStr) GlobalUnlock (comm->hActiveStr) ;
			}
		AObjUnlockData (oiConn, SGNLBLOCK) ;
		}
		AObjUnlockData (oiConn, SGNLINFO) ;
	}
	return(rVal);
	
}
void SetSearchString (OBJECTID oiConn, LPSTR lpString, long signal, int compareBits)
{
	SIGNALINFO  *signalInfo ;
	LPSTR        signalBlock ;
	LPSTR        tmpBlock ;
  WORD         blocksize ;
	WORD         signalNums ;
  WORD         addlen ;
	WORD         len = 0;
	long         i ;


	signalNums = AObjGetDataSize(oiConn, SGNLINFO)/sizeof (SIGNALINFO) ;
	if (signal==0) signal = 1;
	if (signal>signalNums) signal = signalNums ;
	blocksize = AObjGetDataSize (oiConn, SGNLBLOCK) ;

	signalInfo  = (SIGNALINFO*)AObjLockData (oiConn, SGNLINFO) ;
	signalBlock = (LPSTR)AObjLockData (oiConn, SGNLBLOCK) ;

	if (signalInfo)
		{
		if (signalBlock)
			{
			for (i=0; i<signal-1; i++, signalInfo++)
      	{
				len +=signalInfo->signalLen ;
				}
			signalInfo->CompareBits = compareBits ;
			addlen = _fstrlen(lpString)-_fstrlen (signalBlock+len) ;
			tmpBlock = new char[blocksize+addlen] ;
      *tmpBlock='\0' ;
			CopyMemory(tmpBlock, signalBlock, len) ;
			lstrcpy((char*)tmpBlock+len, lpString) ;
			CopyMemory(
				tmpBlock+len+_fstrlen(lpString)+1,
				signalBlock+len+_fstrlen(signalBlock+len)+1,
				blocksize-len-_fstrlen(signalBlock+len)) ;
			AObjUnlockData (oiConn, SGNLBLOCK) ;
			AObjSetData (oiConn, SGNLBLOCK, tmpBlock, blocksize+addlen) ;
      delete[]tmpBlock ;
      }
		AObjUnlockData (oiConn, SGNLINFO) ;
		COMMUNICATION *comm ;
		comm =(COMMUNICATION*) AObjGetRuntimeInfo (oiConn) ;
		comm->SetActiveSearchTable () ;
		} 																						               
}
void SynchronousListen (OBJECTID oiConn, DWORD ticks, DWORD startTime) 
{
	DWORD endTime ; 
	BOOL  loop = TRUE ;
	COMMUNICATION *comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;

	if (ticks==0)
		 endTime = -1 ;
	else if (ticks<-1)
	   endTime = startTime+ticks ;

	do
	  {
	  if(comm->ListenACall ()) break ;
		if (GetTickCount()>endTime)
			loop = FALSE ;
	  }while (loop) ;
}
void AsynchronousListen (OBJECTID oiConn, long ticks, DWORD startTime)
{
	AEvtInfo event ;

	event.sourceObject = oiConn ;
	event.targetObject = oiConn ;
	event.what         = asynchronousListen ;
	event.details      = ticks ;
	event.modifiers    = startTime ;
	event.how          = AEVT_ATTAIL ;
	event.when         = AAppGetTime() ;
	AEvtPost (&event) ;
}
void CancelListen (OBJECTID oiConn)
{
	AEvtInfo event ;

	COMMUNICATION *comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	comm->ResetModem () ;
	event.sourceObject = oiConn ;
	event.targetObject = oiConn ;
	event.what         = asynchronousListen ;
	event.how          = AEVT_KILLDUPLICATES|AEVT_ATTAIL|AEVT_KILLSELF ;
	event.when         = AAppGetTime() ;
	AEvtPost (&event) ;
}
int Conn_CommOpen (OBJECTID oiConn, BOOL listenFlag )
{
	COMMUNICATION *comm ;
	int            err=COMM_ERROR ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if (comm)
		{
		if (!comm->connbusy)
    	{
			comm->listenFlag = listenFlag ;
			comm->SetAnsOrDialParam (listenFlag) ;
			err = comm->InitComm() ;
			if (err==COMM_OK)
				{
				if(!comm->Dial())
					err=COMM_ERROR ;
				}
			}
		}
	return(err);
}
//
//
//
int Conn_CommClose (OBJECTID oiConn, long detail)
{
	COMMUNICATION *comm ;
  int            err ;

	comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
	if(comm)
		{
		err= comm->ModemHangUp() ;
		err=comm->CommClose() ;
		if (detail==(long)changeConfigure)
			{
			delete comm ;
			comm = new COMMUNICATION (oiConn, INBUFSIZE, OUTBUFSIZE) ;
			AObjSetRuntimeInfo (oiConn, (long) comm) ;
			}
		}
	return(err);
}

void ALMAPI Conn_ActivateFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID    oiConn ;
	OBJECTID    oiFirst = NULL ;
	OBJECTID    oiLast =NULL ;
  int         first, last ;
	int         flow ;

	I_UNUSED (theEvent) ;
	oiConn  = AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiFirst = AFuncGetTypedParameter (2, OTYPE_NUMBER) ;
	oiLast  = AFuncGetTypedParameter (3, OTYPE_NUMBER) ;
	first   = (int) ONmbrGetInteger (oiFirst) ;	 
	last    = oiLast?(int)ONmbrGetInteger (oiLast):first ;

	if (DoActivateSearch (oiConn,first, last, TRUE))
		flow = 1 ;
	else
		flow = 2 ;		//Should be the second flow but MAC side had no time to modify code
	AFuncReportResult(theSystem, flow, NULL) ;
}
void ALMAPI Conn_DeactivateFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID    oiConn ;
	OBJECTID    oiFirst = NULL ;
	OBJECTID    oiLast =NULL ;
  int         first, last ;
	int         flow ;

	I_UNUSED (theEvent) ;
	oiConn  =AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiFirst =AFuncGetTypedParameter (2, OTYPE_NUMBER) ;
	oiLast  =AFuncGetTypedParameter (3, OTYPE_NUMBER) ;
	first =  (int) ONmbrGetInteger (oiFirst) ;
	last  =  oiLast?(int)ONmbrGetInteger (oiLast):first ;

	if (DoActivateSearch (oiConn,first, last, FALSE))
		flow = 1 ;
	else
		flow = 2 ;    //Should be the second flow
	AFuncReportResult(theSystem, flow, NULL) ;
}
void ALMAPI Conn_BreakFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID       oiConn ;
	OBJECTID       oiNumber ;
	COMMUNICATION *comm ;
  long           ticks ;

	if (theEvent->targetObject)
		{
		comm=(COMMUNICATION*)theEvent->targetObject ;
		comm->LineBreak(COMM_BREAK_OFF) ;
		AFuncReportResult(theSystem, 1, NULL) ;
		}
	else
   	{
		oiConn  =AFuncGetTypedParameter (1, OTYPE_CONN) ;
		oiNumber=AFuncGetTypedParameter (2, OTYPE_NUMBER) ;
		ticks = (int) ONmbrGetInteger(oiNumber) ;
		comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
		if (comm)
			{
			if (comm->LineBreak (COMM_BREAK_ON)!=COMM_BREAK_ERROR)
			  {
      	theEvent->targetObject = (OBJECTID)comm ;
				theEvent->when = AAppGetTime()+ticks ;
				AEvtPost (theEvent) ;
				}
      }
    }
 
}
//*
//*
//*
void ALMAPI Conn_ListenFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID        oiConn ;
	OBJECTID        oiNumber ;
	long            ticks ;
	BOOL            SynchOrAsynch = FALSE ; //if TRUE then synchronous listen, else asynchronous listen ;
	DWORD           startTime ;
	int             err ;

	I_UNUSED(theEvent) ;

	oiConn  =AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiNumber=AFuncGetTypedParameter (2, OTYPE_NUMBER) ;
	if (oiNumber)
	  {
		ticks = ONmbrGetInteger(oiNumber) ;
		if (ticks<=0)
			 SynchOrAsynch = TRUE ;
		}
	else
		ticks = -2 ;                          //ticks=-2 Asynchronous (no time out) 
	err = Conn_CommOpen (oiConn, TRUE) ;
	if (err!=COMM_ERROR)
	  {
		startTime = GetTickCount() ;
	  if (SynchOrAsynch)
			SynchronousListen (oiConn, ticks, startTime) ;
		else
			AsynchronousListen (oiConn, ticks, startTime) ;
		AFuncReportResult (theSystem, 1, NULL) ;
	  }
	else
		AFuncReportResult (theSystem, 2, NULL) ;   //Could have other flow
}
//*
//*
//*
void ALMAPI Conn_CancelListenFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn ;

	I_UNUSED(theEvent) ;
	I_UNUSED(theSystem) ;
   
	oiConn  =AFuncGetTypedParameter (1, OTYPE_CONN) ;
	CancelListen (oiConn) ;
	Conn_CommClose (oiConn, 0) ;
  AEvtPostSignalAtMark (oiConn, 4) ;
	AFuncReportResult (theSystem, 1, NULL) ;
}
//*
//*
//*
void ALMAPI Conn_ScheduleTextFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID   oiConn ;
	OBJECTID   oiTime ;
  OBJECTID   oiText ;
	OBJECTID   oiReference ;
	long       sec1, sec2 ;
	UINT       textLen ;
  long       LValue ;  
	struct time  currentTime ;

  I_UNUSED (theEvent) ;
	oiConn=AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiTime=AFuncGetTypedParameter (2, OTYPE_TIME) ;
	oiText=AFuncGetTypedParameter (3, OTYPE_TEXT) ;
	
	sec1  = OTimeGetValue (oiTime) ;
	gettime (&currentTime) ;
	sec2 = (long)currentTime.ti_sec +(long)currentTime.ti_min*60L+(long)currentTime.ti_hour*3600L ;

	textLen = OTextGetLength (oiText) ;
	oiReference = AObjCreate (OTYPE_NUMBER) ;
	LValue = sec1+(long)textLen + (long)oiConn ;
  ONmbrSetInteger (oiReference, LValue, FALSE) ;
	AFuncSetParameter(1, oiReference) ;

	if (sec2<sec1)
		{
		AEvtInfo event ;
    event.sourceObject = oiConn ;
		event.targetObject = oiConn ;
		event.what         = sendScheduleText ;
		event.details      = (long)LValue ;
		event.modifiers    = (long)oiText ; 
		event.when         = AAppGetTime() + (sec1-sec2)*60 ;
		event.how          = AEVT_ATTAIL ;
		AEvtPost (&event) ;
		}
	AFuncReportResult (theSystem, 1, NULL) ;
}
//*
//*
//*
void ALMAPI Conn_UnscheduleTextFunc (
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn ;
	OBJECTID oiReference ;
  long     reference ;
	int      flow = 1 ;

	I_UNUSED(theEvent) ;
	I_UNUSED(theSystem) ;

	oiConn     =AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiReference=AFuncGetTypedParameter (2, OTYPE_NUMBER) ;

	reference = (long)ONmbrGetInteger(oiReference) ;
	if (AEvtPostStandard (oiConn, sendScheduleText, (long)reference,
												AEVT_KILLDETAILS|
												AEVT_KILLMODIFIERS|
												AEVT_KILLWHAT|
												AEVT_KILLSELF))
			flow = 1 ;
	else
			flow = 1 ;

	AFuncReportResult (theSystem, flow, NULL) ;
}
//*
//*
//*																																												         
void ALMAPI Conn_ConfigureFunc(
	pAEvtInfo theEvent,
	pAObjMessage theSystem)
{
	OBJECTID oiConn ;
	BOOL     ok ;
  CONN    *conn ;
	HWND     hwnd = GetActiveWindow () ; 

	I_UNUSED (theEvent) ;

	oiConn=AFuncGetTypedParameter(1, OTYPE_CONN) ;
	conn = new CONN (oiConn) ;
	ok = DialogBoxParam(hInstance, "Connection" , hwnd, (DLGPROC)CntObjProc, (LPARAM)conn) ;
	if (!ok)
  	{
		AFuncReportResult (theSystem, 2, NULL) ;
    }
	else
		{
		COMMUNICATION * comm ;
		comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
		if (comm&&comm->LinkOpen())
    	{
			Conn_CommClose (oiConn, (long)changeConfigure) ;
			Conn_CommOpen (oiConn, FALSE) ;
			}
		AFuncReportResult (theSystem, 1, NULL) ;
    }
}
//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI Conn_OpenCommFunc(
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID oiConn ;
	int      err ;

	I_UNUSED(theEvent) ;

  oiConn=AFuncGetTypedParameter (1, OTYPE_CONN) ;
	err = Conn_CommOpen	(oiConn, FALSE) ;
	if (err==COMM_OK)
  	{
		AFuncReportResult(theSystem, 1, NULL) ;
		AEvtPostSignalAtMark (oiConn, 1) ;
    }
	else
		AFuncReportResult(theSystem, 2, NULL) ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI Conn_CloseCommFunc(
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID oiConn ;
	int      err ;

	I_UNUSED(theEvent) ;

  oiConn=AFuncGetTypedParameter (1, OTYPE_CONN) ;
	err = Conn_CommClose (oiConn, NULL) ;
	if (err==COMM_OK)
  	{
		AFuncReportResult(theSystem, 1, NULL) ;
		AEvtPostSignalAtMark(oiConn, 0) ;
    }
	else
		AFuncReportResult(theSystem, 2, NULL) ;
}
//
//
//
void ALMAPI Conn_SendTextFunc (
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID	oiConn ;
  OBJECTID  oiText ;
  BYTE*     pText ;
	WORD      textSize ;
  int       flow=2 ;

  I_UNUSED (theEvent) ;
	oiConn = AFuncGetTypedParameter (1, OTYPE_CONN) ;
  oiText = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	textSize = OTextGetLength (oiText)+1 ;
	if (textSize>1)
  	{
		pText = (BYTE*)new char[textSize] ;  
		OTextGetToBuffer (oiText, (LPSTR)pText, textSize) ;

		COMMUNICATION* comm ;
		comm = (COMMUNICATION*)AObjGetRuntimeInfo (oiConn) ;
		if (comm&&comm->LinkOpen())
			{
			comm->PutBuffer (pText, textSize) ;
      flow = 1 ;
			}
    if (pText) delete[]pText ;
	 }
	AFuncReportResult (theSystem, flow, NULL) ;
}
//
//
//
void ALMAPI Conn_TextToHexFunc (
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID    oiText ;
  OBJECTID    oiHex ;
	MEMBLOCKID  idTextBlock ;
  WORD        textLen ;
  LPSTR       lpText ;
	LPSTR       lpBuffer ;

  I_UNUSED(theEvent) ;
	oiText=AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  textLen = OTextGetLength(oiText)+1 ;
  idTextBlock = AMemAllocate (2*(textLen-1)+1) ;
  lpBuffer =(LPSTR) AMemLock (idTextBlock) ;
	if (lpBuffer)
		{
    lpText = (LPSTR)new char[textLen] ;
		OTextGetToBuffer (oiText, lpText, textLen) ;
		if (*lpText!=NULL)
		  {
   		TextToHex (lpBuffer, lpText, "%02X", textLen) ;
			oiHex = AObjCreate (OTYPE_TEXT) ;
			if (oiHex)
     		{
   			AObjAssign (oiText, oiHex) ;
				OTextSetValueFromBuffer (oiHex, lpBuffer) ;
     		}
			}
		AMemUnlock (idTextBlock) ;
		AMemFree (idTextBlock) ;
		AFuncSetParameter (1, oiHex) ;
		}
	AFuncReportResult (theSystem, 1, NULL) ;
}

void ALMAPI Conn_HexToTextFunc (
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID    oiHex ;
  OBJECTID    oiText ;
  WORD        hexTextLen ;
  LPSTR       lpHexText ;
	LPSTR       lpDecText ;
  int         flow=1;

	I_UNUSED(theEvent) ;

  oiHex=AFuncGetTypedParameter(1, OTYPE_TEXT) ;
	hexTextLen = (WORD)OTextGetLength (oiHex)+1 ;
  lpHexText = (LPSTR)new char[hexTextLen] ;
	lpDecText = (LPSTR) new char[hexTextLen+1/2] ;

	if (lpHexText&&lpDecText)
		{
		OTextGetToBuffer(oiHex, lpHexText, hexTextLen);
		ChangeHexToDec ((char*)lpDecText, (char*)lpHexText) ;
		oiText = AObjCreate (OTYPE_TEXT) ;
		if (oiText)
  		{
			OTextSetValueFromBuffer(oiText, lpDecText);
			AFuncSetParameter(1, oiText) ;
      flow = 1 ;
			}
    }
	AFuncReportResult(theSystem, flow, NULL) ;

	if (lpHexText)
		delete[]lpHexText;
	if(lpDecText)
		delete[]lpDecText ;
}
void ALMAPI Conn_SetSearchFunc (
	pAEvtInfo theEvent,
  pAObjMessage theSystem)
{
	OBJECTID    oiConn ;
	OBJECTID    oiString ;
	OBJECTID    oiSignal ;
	OBJECTID    oiCompareBits ;
  WORD        stringLen ;
  LPSTR       lpString ;
	long        signal ;
  int 				compareBits ;

	I_UNUSED(theEvent) ;

	oiConn   = AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiString = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	oiSignal = AFuncGetTypedParameter (3, OTYPE_NUMBER) ;
	oiCompareBits = AFuncGetTypedParameter (4, OTYPE_NUMBER) ;

	stringLen = (WORD)OTextGetLength (oiString)+1 ;
	lpString = (LPSTR) new char[stringLen] ;
	if (lpString)
  	{
		OTextGetToBuffer (oiString, lpString, stringLen) ;
		signal = ONmbrGetInteger(oiSignal);

		if (!oiCompareBits)
			compareBits = 0 ;
		else
			compareBits =(int) ONmbrGetInteger(oiCompareBits);

		SetSearchString (oiConn, lpString, signal, compareBits?0:1) ;
    }
	AFuncReportResult(theSystem, 1, NULL) ;
}
void ALMAPI Conn_GetConfigFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID    oiConn ;
	OBJECTID    oiConfigure ;
  LPSTR       lpConfigure ;

	I_UNUSED(theEvent) ;

	oiConn   = AFuncGetTypedParameter (1, OTYPE_CONN) ;
	lpConfigure = (LPSTR)new char[500] ;
	if (lpConfigure)
		{
		SetupConfigureText (oiConn, lpConfigure) ;
		oiConfigure = AObjCreate (OTYPE_TEXT) ;
		OTextSetValueFromBuffer (oiConfigure, lpConfigure) ;
		AFuncSetParameter(1, oiConfigure) ;
		delete[]lpConfigure ;
    }

	AFuncReportResult(theSystem, 1, NULL) ;
}
void ALMAPI Conn_SetConfigFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID    oiConn ;
	OBJECTID    oiConfigure ;
	LPSTR       lpConfigure ;
	WORD        len ;
  int         flow = 2;

	I_UNUSED(theEvent) ;

	oiConn      = AFuncGetTypedParameter (1, OTYPE_CONN) ;
	oiConfigure = AFuncGetTypedParameter (2, OTYPE_TEXT) ;
	len = OTextGetLength(oiConfigure) ;
	lpConfigure = (LPSTR) new char[++len] ;
	if (lpConfigure)
		{
		OTextGetToBuffer (oiConfigure, lpConfigure, len) ;
		if (*lpConfigure)
    	{
			if (SaveConfigureText (oiConn, lpConfigure))
        	flow = 1 ;
			}
		delete[]lpConfigure ;
    }

	AFuncReportResult(theSystem, flow, NULL) ;
}
void ALMAPI Conn_ValueToCharFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID    oiText ;
	OBJECTID    oiValue ;
	WORD        value ;
  BYTE        value1 ;

	I_UNUSED(theEvent) ;

	oiValue =AFuncGetTypedParameter(1, OTYPE_NUMBER) ;

	value = (WORD)ONmbrGetInteger(oiValue);
	value1 = (BYTE)value ;
	if (value1==value) 
		{
		char retChar[2] ;
		retChar[0] = (char)value ;
    retChar[1] = '\0' ;
		//sprintf(retChar, "%c", value) ;
		oiText = AObjCreate(OTYPE_TEXT) ;
		if (oiText)
    	{
			OTextSetValueFromBuffer(oiText, (LPSTR)retChar);
			AFuncSetParameter(1, oiText) ;
      }
		AFuncReportResult(theSystem, 1, NULL) ;
		}
	else
		AFuncReportResult (theSystem, 1, NULL) ;
}
void ALMAPI Conn_CharToValueFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID    oiText ;
	OBJECTID    oiIndex ;
  OBJECTID    oiValue ;
  WORD        textLen ;
  LPSTR       lpText ;
	WORD        index ;

	I_UNUSED(theEvent) ;

	oiText=AFuncGetTypedParameter(1, OTYPE_TEXT) ;
	oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;

	textLen = OTextGetLength (oiText) ;
  lpText = (LPSTR)new char[++textLen] ;
	index = (WORD) (oiIndex)?ONmbrGetInteger(oiIndex):1;
	if (!index) index=1;

	if (lpText)
  	{
		OTextGetToBuffer(oiText, lpText, textLen);
		if (index<textLen)
			{
			char c = *(lpText+index-1) ;
			oiValue = AObjCreate (OTYPE_NUMBER) ;
			if (oiValue)
  			{
				ONmbrSetInteger(oiValue, (int)c, FALSE);
				AFuncSetParameter(1, oiValue) ;
				}
      }
    delete []lpText;
		}

	AFuncReportResult(theSystem, 1, NULL) ;
}
void ALMAPI Conn_CheckSumFunc (pAEvtInfo theEvent, pAObjMessage theSystem)
{
	OBJECTID	oiText ;
	LPSTR     lpText ;
	WORD      lenText ;
	BYTE      zeroSumChar[2] ;
	BYTE      sumChar[2] ;
	long      sum=0 ;
	OBJECTID  oiResult ;
	int       i ;

	I_UNUSED(theEvent) ;

	oiText =AFuncGetTypedParameter(1, OTYPE_TEXT) ;
	lenText = OTextGetLength (oiText)+1 ;
  lpText = (LPSTR)new char[lenText] ;
	OTextGetToBuffer (oiText, lpText, lenText) ;

	for(i=0; i<lenText; i++)
		sum+=(BYTE)*(lpText+i) ;
	delete[]lpText ;

	zeroSumChar[0] = 256-(sum%256) ;
  zeroSumChar[1] = '\0' ;
	sumChar[0] = sum%256 ;
  sumChar[1] = '\0' ;
	oiResult = AObjCreate(OTYPE_TEXT) ;
	if (oiResult)
		{
		OTextSetValueFromBuffer (oiResult, (LPSTR)zeroSumChar) ;
		AFuncSetParameter(1, oiResult) ;
		}
	oiResult = AObjCreate(OTYPE_TEXT) ;
	if (oiResult)
		{
		OTextSetValueFromBuffer (oiResult, (LPSTR)sumChar) ;
		AFuncSetParameter(2, oiResult) ;
		}
	oiResult = AObjCreate(OTYPE_NUMBER) ;
	if (oiResult)
		{
		ONmbrSetInteger (oiResult, sum, FALSE) ;
		AFuncSetParameter(3, oiResult) ;
		}
	AFuncReportResult(theSystem, 1, NULL) ;
}
