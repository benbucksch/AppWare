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
// FILE:"Xmodem.cpp"
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

#include <stdlib.h>
#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <mem.h>
#include "filetrns.h"

char * CutFileName (char * filename) ;
extern void ActiveIdleEvent (OBJECTID oiFT) ;
YMODEMCLASS::YMODEMCLASS (OBJECTID oiFT, char *fileName, BOOL sORr) :XMODEMCLASS (oiFT, fileName, sORr)
{
	char * p ;
	FILETRANSFER fltr(oiFT) ;

  fileCount = 0 ;
	if (sORr)
	  {
		filename = new char[2048] ;
		p = filename ;
		while(strlen(fileName))
			{
			memcpy(p, fileName, strlen(fileName)+1) ;
			fileName = fileName+strlen(fileName)+1 ;
			p=p+strlen(p)+1 ;
			fileCount ++ ;
			}
		p = '\0' ;
		}
	else
	  {
		filename = new char[256] ;
		strcpy (filename, fileName) ;
		}
	lErrorCount =0 ;
	maxTimeout   = fltr.data0->maxTimeout*60 ;
  maxTrytimes  = fltr.data0->maxTrytimes ;
	bSeq = 0 ;
	fCRC = TRUE ;
	dataBlock0 = TRUE ;
	AObjSetRuntimeInfo (oiFT, (long)this) ;
}
YMODEMCLASS::~YMODEMCLASS(void)
{
	delete[] filename ;
  delete this ;
}
void YMODEMCLASS::StartProtocol (OBJECTID oiFT, int sendOrReceive)
{
	int rVal ;
	if (sendOrReceive==SENDFILE)
	  {
		fltr_state = YMODEM_INIT ;
		rVal = Initiation (TRUE) ;
		}
	else
	  {
	  fltr_state = SEND_INIT ;
		rVal = Initiation (FALSE) ;
		ReceiveBinaryFile () ;
		SendInitChar () ;
	  }
	if (rVal==FT_SUCCESS)
	  {
		SetTimerEvent (oiFT, setPollTimer, 15) ;
		SetTimerEvent (oiFT, setDataTimer, maxTimeout) ;
		}
	else
		AEvtPostSignalAtMark(oiFT, 6) ;
}
int YMODEMCLASS::ProcessBufferData (void)
{
	int rVal = FT_SUCCESS ;
	switch (fltr_state)
  	{
		case YMODEM_INIT:
			rVal = WaitFirstChar () ;
			break;
		case WAIT_ACK:
			rVal = WaitACK() ;
			break;
		case WAIT_FINAL_ACK :
			rVal = WaitFinalACK () ;
			break;
		case SEND_INIT:
			SendInitChar () ;
			break;
		case WAIT_SOH:
			rVal = WaitSOH () ;
			break;
		case WAIT_SEQ:
			rVal = WaitSEQChar() ;
			break;
		case WAIT_DATA:
			ReceiveDataBlock() ;
			break;
		case WAIT_CHECK:
			rVal = CheckDataBlock () ;
			break;
  	}
	if (rVal !=FT_SUCCESS)
	  {
    KillTimerEvent (oiFT, setPollTimer) ;
		KillTimerEvent (oiFT, setDataTimer) ;
		ActiveIdleEvent (oiFT) ;
		if (rVal==FT_FILEEND)
			AEvtPostSignalAtMark (oiFT, 0) ;
		else
			AEvtPostSignalAtMark (oiFT, 6) ;
	  }

	return(rVal);
}

int YMODEMCLASS::Initiation (BOOL bBar)
{
	int rVal = FT_SUCCESS ;
	if (Status)
		{
		if (!lpStatus->InitStatusWindow(2, bBar, "YMODEM", filename, 128)) 
    	rVal = FT_LOCALCANCEL ;
		}
	if (handshake==XONXOFF)
		CommSetXONXOFF (oiConn, 0) ;
	return rVal ;
}
int YMODEMCLASS::WaitFirstChar (void) 
{
	int result ;
	int rVal ;

	result = GetFirstChar () ;
	switch (result)
		{
		case NAK:
		case INIT_C:
			fltr_state = WAIT_ACK ;
			rVal = WriteHeaderBlock() ;
			if (rVal==FT_FILEEND)
			  {
				rVal = FT_SUCCESS ;
				fltr_state = WAIT_FINAL_ACK ;
				}
			RestartTimerEvent (oiFT, maxTimeout) ;
			break ;

		case FT_LOCALCANCEL:
			rVal = result ;
			LocalCancel () ;
			break ;

		case FT_REMOTECANCEL:
			rVal = result ;
			RemoteCancel () ; 
			break ;
		}
	return rVal;
}
int YMODEMCLASS::WriteHeaderBlock (void)
{
	ftime     ft ;
	struct tm t ;
	char      drive[10] ;
	char      dir[50] ;
	char      name[256] ;
	char      ext[10] ;
  long      fileLength ;
	int i ;

	fltr_state = WAIT_ACK ;
	lpBuffer[0] = SOH ;
	lpBuffer[1] = 0 ;  
	lpBuffer[2] = 0xff ;
	for (i=3; i<133; i++) ;
		lpBuffer[i] = 0 ;
	if (fileCount>0)
		{
		fnsplit (filename, drive, dir, name, ext) ;
		strcat (name,ext) ;
		fileLength = _llseek (hFile, 0, 2) ;
		_llseek (hFile, 0, 0) ;

    getftime(hFile, &ft) ;
    t.tm_sec = ft.ft_tsec*2 ;   
    t.tm_min = ft.ft_min ;   
    t.tm_hour= ft.ft_hour ;  
		t.tm_mday= ft.ft_day ;
		t.tm_mon = ft.ft_month - 1 ;
    t.tm_year= ft.ft_year +80 ;  
		t.tm_isdst= FALSE ;

		putenv("TZ=GMT0GMT") ;
		tzset() ;
		sprintf (lpBuffer+3, "%s%c%ld %lo 0", filename, 0x00, fileLength, mktime(&t)) ;
		}
	else
  	lpBuffer[3] = '\0' ;

  nTransSize = 132 ;
	if (fCRC)
		{
		nTransSize = 133 ;
		int vCRC = CalcCRC16(&lpBuffer[DATA_OFFSET], 128) ;
		lpBuffer[CHK_OFFSET] = HIBYTE (vCRC) ;
		lpBuffer[CHK_OFFSET+1] =LOBYTE(vCRC) ;
		}
	else
		lpBuffer[CHK_OFFSET] = CheckSum (&lpBuffer[DATA_OFFSET]) ;

	CommPutBuffer (oiConn, (BYTE*)lpBuffer, nTransSize) ;

	if (Status)
		{
   	strcpy(lpStatus->filename, name) ;
		lpStatus->filesize = fileLength ;
		lpStatus->error = lErrorCount ;
		lpStatus->blocksize +=128 ; 
		lpStatus->UpdateStatusWindow () ;
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
   	}
	if (fileCount<=0)
		return (FT_FILEEND) ;
	else
		return (FT_SUCCESS) ;
}
int YMODEMCLASS::WaitFinalACK (void)
{
	int rVal ;
	rVal = WaitACK() ;
	if (rVal==FT_FILEEND)
	  {
		rVal = EndOrContinueSendBinaryFile() ;
	  }
	return(rVal);
}
void YMODEMCLASS::EndSendBinaryFile (void)
{
	char c = EOT ;

  CommPutChar (oiConn, c) ;
	if (Status)
		DestroyWindow (lpStatus->hStatus) ;
	if (hFile!=HFILE_ERROR)
		_lclose (hFile) ;
	hFile = HFILE_ERROR ;

	return ;
}
int YMODEMCLASS::EndOrContinueSendBinaryFile (void)
{
	OFSTRUCT of ;
	int rVal ;

	if (filename)
		filename +=strlen(filename)+1 ;
	if(--fileCount>0)
		{
		hFile = OpenFile (filename, &of, OF_READ) ;
		Initiation(TRUE) ;
		lErrorCount =0 ;
		bSeq = 0 ;
		fCRC = TRUE ;
		dataBlock0 = TRUE ;
		fltr_state = YMODEM_INIT ;
		return FT_SUCCESS ; 
		}
	if(fileCount==0)
		{
		if (handshake==XONXOFF)
      CommSetHandShake (oiConn, handshake) ;
		fltr_state = YMODEM_INIT ;
    return FT_SUCCESS ;
		}
	if(fileCount<0)
		{
		rVal = FT_FILEEND ;
		//AEvtPostSignalAtMark (oiFT, 1) ;
		//AEvtPostStandard (oiFT, activeIdle, 0, AEVT_ATTAIL) ;
    }
	return rVal ;
}
int YMODEMCLASS::ReceiveBinaryFile (void)
{
	if (Status)
		{
		strcpy(lpStatus->filename,"") ;
    lpStatus->UpdateStatusWindow() ;
    }

	directory = new char[256] ;
	strcpy (directory, filename) ;

	rvDataCount = 0 ;
	waittimes = 0 ;
	fCRC = TRUE ;
	bSeq = bLastSeq =0 ;
	startstatus = TRUE ;

	return FT_SUCCESS ;
}
int YMODEMCLASS::WaitSOH(void)
{
	int status, rVal ;

	rVal = status =GetSOHChar() ;
	switch(status)
    {
		case FT_LOCALCANCEL:
			LocalCancel() ;
			break ;
		case SOH :
			maxblocksize = 128 ;
			if (startstatus)
				startstatus = FALSE ;
			rVal = FT_SUCCESS ;
			fltr_state = WAIT_SEQ ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			break ;
		case STX :
			maxblocksize = 1024 ;
			if (startstatus)
				startstatus = FALSE ;
			rVal = FT_SUCCESS ;
			fltr_state = WAIT_SEQ ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			break;
		case CAN :
			rVal = FT_REMOTECANCEL ;
			RemoteCancel() ; 
      break ;
		case EOT :
			rVal = EndTransferBinaryFile() ;
			break ;
		default:
			rVal = FT_SUCCESS ;
			break;
		}

	return rVal ;
}
int YMODEMCLASS::GetSOHChar (void)
{
	int rVal ;
	int c ;

	c = CommGetChar (oiConn) ;
	switch (c)
		{
		case SOH:
		case STX:
			lErrorCount = 0 ;
			return c ;

		case CAN:
			return CAN ;

		case EOT:
			return EOT ;

		default:
			rVal =FT_SUCCESS ;
		}

	if (Status)
		{
		if (lpStatus->transferStop) return FT_LOCALCANCEL ;
    }

	return(rVal);
}
int YMODEMCLASS::EndTransferBinaryFile(void)
{
	struct tm *t ;
	struct ftime ft ;

  CommPutChar (oiConn, ACK) ;
	if (hFile!=HFILE_ERROR)
    {
		if (moddate)
			{
			putenv ("TZ=GMT0GMT") ;
			tzset() ;
			t = localtime(&moddate) ;
			ft.ft_tsec = t->tm_sec /2 ;
			ft.ft_min  = t->tm_min ;
			ft.ft_hour = t->tm_hour ;
			ft.ft_day  = t->tm_mday ;
			ft.ft_month= t->tm_mon +1 ;
			ft.ft_year = t->tm_year-80 ;
   		setftime(hFile, &ft) ;
			}
				
		_lclose (hFile) ;
		rvDataCount = 0 ;
		waittimes = 0 ;
		bSeq = bLastSeq =0 ;
		dataBlock0 = TRUE ;
		lErrorCount =0 ;
		if (Status)
			{
			lpStatus->error = 0 ;
			lpStatus->blocksize = 0 ;
			lpStatus->blockno = 0 ;
			strcpy(lpStatus->filename, "") ;
			lpStatus->UpdateStatusWindow() ;
			if (lpStatus->transferStop) return FT_LOCALCANCEL ;
    	}
		fltr_state = SEND_INIT ;
		}
	else
  	{
		if(Status)
			DestroyWindow(lpStatus->hStatus) ;
    }
	return FT_SUCCESS;
}
void YMODEMCLASS::ReceiveDataBlock(void)
{
	int numChars ;
	BYTE *szData ;

	szData = new BYTE [maxblocksize+2] ;
	numChars = CommGetBuffer (oiConn, szData, maxblocksize-rvDataCount) ;
	memcpy(&lpBuffer[rvDataCount], szData, numChars) ;
	rvDataCount+=numChars ;

	if (rvDataCount==maxblocksize)
	  {
		fltr_state = WAIT_CHECK ;
		lpBuffer[maxblocksize] = '\0' ;
		}
	delete []szData ;
}
int YMODEMCLASS::CheckDataBlock (void)
{
	BYTE hiCRC ,loCRC ;
	int  bCRC ;
	BYTE checksum ;
	BOOL checkOK=TRUE ;
  int  rVal = FT_SUCCESS ; 

	if (fCRC)
		{
		hiCRC = CommGetChar (oiConn) ;
    loCRC = CommGetChar (oiConn) ;
		bCRC = CalcCRC16 (lpBuffer, maxblocksize) ;
		if (hiCRC!=HIBYTE(bCRC)||loCRC!=LOBYTE(bCRC))
			checkOK = FALSE ;				
		}
	else
		{
    checksum = CommGetChar (oiConn) ;
		if(checksum!=CheckSum (lpBuffer))
   		checkOK = FALSE ;
		}

	if (checkOK)
		{
		if (dataBlock0)
			rVal = CheckDataBlock0() ;
    else
			{
			fltr_state = WAIT_SOH ;
			WriteToFile () ;
			}
		}
	else
	 {
	 rvDataCount = 0 ;
	 setmem(lpBuffer, maxblocksize, ' ') ;
   CommPutChar (oiConn, NAK) ;
		fltr_state = WAIT_SOH ;
	 }
  return (rVal) ;
}

int YMODEMCLASS::CheckDataBlock0(void)
{
	int   rVal=FT_SUCCESS ;
	char *path ;
	path = new char[256] ;
 	if (Status)
 		{
		lpStatus->writeName = TRUE ;
 		strcpy(lpStatus->filename, lpBuffer) ;
 		lpStatus->blocksize +=128 ;
 		lpStatus->blockno++ ;
 		lpStatus->UpdateStatusWindow() ;
    if (lpStatus->transferStop) return FT_LOCALCANCEL ;
		lpStatus->writeName = FALSE ;
 		}
	if (lpBuffer[0]=='\0')
	  {
		CommPutChar (oiConn, ACK) ;
		if(Status)
			DestroyWindow(lpStatus->hStatus) ;
		return(FT_FILEEND);
		}
		
	sprintf (path, "%s\\%s", directory, lpBuffer) ;
	if (!lpBuffer[strlen (lpBuffer)+1])
		{
		length = 0L;
     moddate = 0L ;
		}
	else
		{
		sscanf(&lpBuffer[strlen(lpBuffer)+1], "%ld %lo", &length, &moddate) ;
		}
	OFSTRUCT of ;
	if (hFile!=HFILE_ERROR)
		{
		_lclose (hFile) ;
     hFile = HFILE_ERROR ;
		}
	if (hFile==HFILE_ERROR)
		{
		char *filename = CutFileName (path) ;
		hFile = OpenFile (filename, &of, OF_CREATE) ;
		}
	if (hFile!=HFILE_ERROR)
	  {
		rvDataCount = 0 ;
		setmem(lpBuffer, maxblocksize, ' ') ;
 		CommPutChar (oiConn, ACK) ;
 		bLastSeq = bSeq ;
 		bSeq++ ;
 		dataBlock0 = FALSE ;
		fltr_state = WAIT_SOH ;
		}
	else
		rVal =FT_LOCALCANCEL ;

	return rVal ;	
}

char *lstrrchr (char* str, char ch)
	{
  char* strl = str + lstrlen (str) ;

  do
		{
   	if (ch == *strl)
    	return strl ;
		strl = AnsiPrev (str, strl) ;
    }
  while (strl > str) ;

  return NULL ;
	}

char *CutFileName (char * filename)
{
	char * newName ;
	char * tmp1, * tmp2 ;

  newName = filename ;
	tmp1 = lstrrchr (filename, '.') ;
	if (tmp1!=NULL && tmp1+4!=NULL)
		*(tmp1+4) = '\0' ;
	tmp2 = lstrrchr (filename, '\\') ;
	if (tmp2!=NULL)
		{
		if (tmp1-tmp2>8)
			{
			memmove (tmp2+7, tmp1, (WORD)(tmp1-tmp2-8 +strlen(tmp1)));
			}
    }

  return newName ;
}
