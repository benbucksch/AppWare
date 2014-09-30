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

#include <stdio.h>
#include <string.h>
#include <mem.h>
#include "filetrns.h"

extern void ActiveIdleEvent (AOBJECTID oiFT) ;

XMODEMCLASS::XMODEMCLASS (AOBJECTID oiFlTr, char * fileName, BOOL sORr):FTCLASS (oiFlTr, fileName, sORr)
{
	FILETRANSFER fltr(oiFlTr) ;

  filename = new char[256] ;
	lstrcpy(filename, fileName) ;
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
	lErrorCount  = 0 ;       
	bSeq         = 1 ;
	fCRC         = TRUE ;
	maxTimeout   = fltr.data0->maxTimeout*60 ;
  maxTrytimes  = fltr.data0->maxTrytimes ;
  option       = fltr.data0->option ;
  maxblocksize = fltr.data0->option==0?128:1024 ;

  AObjSetRuntimeInfo (oiFlTr, (long) this) ;
}
XMODEMCLASS::~XMODEMCLASS(void)
{
	delete[] filename ;
	delete this ;
}
void XMODEMCLASS::StartProtocol (AOBJECTID oiFT, int sendOrReceive)
{
	int rVal ;
	if (sendOrReceive==SENDFILE)
 	  {
		fltr_state = XMODEM_INIT ;
		rVal = Initiation (TRUE) ;
		}
	else
	  {
	  fltr_state = WAIT_SOH ;
		rVal = Initiation (FALSE) ;
		ReceiveBinaryFile () ;
		SendInitChar () ;
	  }
	if (rVal==FT_SUCCESS)
	  {
		SetTimerEvent (oiFT, setPollTimer, 15) ;
		SetTimerEvent (oiFT, setDataTimer, maxTimeout) ;   //sendOrReceive?3600:180) ;
		}
	else
		AEvtPostSignalAtMark(oiFT, 6) ;
}
int XMODEMCLASS::Initiation (BOOL bBar)
{
	int rVal = FT_SUCCESS ;
  
	if (Status)
		{
			if (!lpStatus->InitStatusWindow (2, bBar, "XMODEM", filename, filesize)) 
				rVal = FT_LOCALCANCEL ;
		}
  CommSetXONXOFF (oiConn, 0) ;
	return rVal ;
}
int XMODEMCLASS::HandleBufferData ()
{
	int rVal = FT_SUCCESS ;
	if (sendOrReceive==SENDFILE)
	  {
		lErrorCount ++ ;
		if (lErrorCount>maxTrytimes)
	  	{
	  	CancelSendBinaryFile () ;
			TimeOut() ;
			rVal = FT_TIMEOUT;
	  	}
		}
	else
	  {
	  if (startstatus)
	    {
	    lErrorCount++ ;
			if (lErrorCount<maxTrytimes/2)
			  {
			  SendInitChar () ;
			  }
			else if (lErrorCount==maxTrytimes/2)
			  {
				CommPutChar(oiConn, NAK) ;
				startstatus = FALSE ;
				fCRC = FALSE ;
				}
	    }
		else
		  {
		  lErrorCount++ ;
			CommPutChar(oiConn, NAK) ;
			if (lErrorCount>=maxTrytimes)
			  {
			  CancelSendBinaryFile () ;
				TimeOut() ;
				rVal = FT_TIMEOUT;  
			  }
		  }
	  }
	if (rVal==FT_TIMEOUT)
	  {
	  KillTimerEvent (oiFT, setPollTimer) ;
		KillTimerEvent (oiFT, setDataTimer) ;
	  }
	return(rVal) ;
}
int XMODEMCLASS::ProcessBufferData (void)
{
	int rVal = FT_SUCCESS ;
	switch (fltr_state)
  	{
		case XMODEM_INIT:
			rVal = WaitFirstChar () ;
			break;
		case WAIT_ACK:
			rVal = WaitACK() ;
			break;
		case WAIT_FINAL_ACK :
			rVal = WaitACK() ;
			//AEvtPostSignalAtMark (oiFT, 1) ;
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
			CheckDataBlock () ;
			break;
  	}
	if (rVal !=FT_SUCCESS)
	  {
		CloseBinaryFile () ;
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
int XMODEMCLASS::WaitFirstChar (void) 
{
	int result ;
	int rVal = FT_SUCCESS;

	result = GetFirstChar () ;
	switch (result)
		{
		case NAK:
		case INIT_C:
		case INIT_G:
			fltr_state = WAIT_ACK ;
			if(WriteDataBlock()==FT_ERROR)
				rVal = FT_ERROR ; 
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
int XMODEMCLASS::GetFirstChar(void)
{
	int numchars ;
	unsigned char c;

	numchars = CommRxCount (oiConn) ;
	if (numchars>0)
		{
		while (CommGetBuffer(oiConn, &c, 1))
		  {
			switch (c)
				{
    		case CAN:
      	return FT_REMOTECANCEL ;

    		case NAK:
					fCRC = FALSE ;
					lErrorCount = 0 ;
        	maxblocksize = 128 ;
      		return (NAK);

				case 'C':
				case 'c':
					fCRC = TRUE ;
					lErrorCount = 0 ;
					return(INIT_C);

				case 'G':
				case 'g':
					if (option==3)
						{
						maxblocksize = 1024 ;
						lErrorCount = 0 ;
						return (INIT_G) ;
						}
      		break ;

    		default:
      		break;
				}
		  }
	  }	
	if (Status)
    {
		lpStatus->error = lErrorCount ;
		lpStatus->UpdateStatusWindow () ;
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
    }  	 
	return (FT_SUCCESS) ;
}
int XMODEMCLASS::WriteDataBlock(void)
{
	int bytes ;

	if (!ReadFile (hFile, &lpBuffer[DATA_OFFSET], maxblocksize, (DWORD*)&bytes, NULL))        
 		return FT_ERROR ;
	if (bytes==0)
	  {
		fltr_state = WAIT_FINAL_ACK ;
		EndSendBinaryFile() ;
		return (FT_SUCCESS) ;
		}
	if (bytes<maxblocksize)
		lpBuffer[DATA_OFFSET+bytes] = 0x1a ;
	lpBuffer[SOH_OFFSET] = maxblocksize==128?SOH:STX ;
	lpBuffer[SEQ_OFFSET] = bSeq ;
	lpBuffer[CSEQ_OFFSET] = (255&bSeq)^255 ;
	bSeq++ ;

	if (fCRC)
		{
		nTransSize = maxblocksize+5; 
    int vCRC = CalcCRC16(&lpBuffer[DATA_OFFSET], maxblocksize) ;
		lpBuffer[maxblocksize+3] = HIBYTE (vCRC) ;
		lpBuffer[maxblocksize+4] =LOBYTE(vCRC) ;
		}
	else
		{
    nTransSize = maxblocksize+4 ;
		lpBuffer[maxblocksize+3] = CheckSum (&lpBuffer[DATA_OFFSET]) ;
    }

	CommPutBuffer (oiConn, (BYTE*)lpBuffer, nTransSize) ;
	if (Status)
		{
		lpStatus->blockno++ ;
		lpStatus->blocksize +=maxblocksize ; 
		lpStatus->error = lErrorCount ;
		lpStatus->UpdateStatusWindow () ;
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
    }

  return (FT_SUCCESS) ;
}
int XMODEMCLASS::WaitACK(void)
{
	int numChars ;
	int rVal ;
	unsigned char c = 0;
	
	if (option!=3)
	  {
		numChars = CommRxCount (oiConn) ;
		if (numChars)
	  	{
			while (CommGetBuffer(oiConn, &c, 1))
		  	{
				if (c==ACK||c==NAK||c==CAN)
					{
					rVal = c ;
    			lErrorCount=0 ;
					break ;
    			}
				}
			}
		}
	else
		rVal = ACK ;
	switch (rVal)
	  {
		case ACK:
			if (fltr_state==WAIT_ACK)
			  {
				rVal = WriteDataBlock () ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			else if (fltr_state==WAIT_FINAL_ACK)
			  rVal = FT_FILEEND ;
			break; 

		case NAK:
			ResendDataBlock () ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			rVal = FT_SUCCESS ;
			break;

		case CAN:
			rVal = FT_REMOTECANCEL ;
			RemoteCancel () ;
			break;

		default:
			rVal = FT_SUCCESS ;
			break;
	  }
	return (rVal) ;
}
void XMODEMCLASS::CancelSendBinaryFile (void)
{
	int i ;
	char c = CAN ;

	for (i=0; i<5; i++)            //write 5 times CAN
   	{
    CommPutChar(oiConn, c) ;
    }
  return ;
}
void XMODEMCLASS::EndSendBinaryFile (void)
{
		char c = EOT ;
    CommPutChar(oiConn, c) ;
		//if (Status) DestroyWindow (lpStatus->hStatus) ;
		CloseBinaryFile () ;
		return ;
}
int XMODEMCLASS::ResendDataBlock (void)
{
  CommPutBuffer (oiConn, (BYTE*)lpBuffer, nTransSize) ;
  return FT_SUCCESS ;
}
int XMODEMCLASS::TimeOut (void)              
{
	return(CloseBinaryFile());
}
int XMODEMCLASS::LocalCancel (void)          //need send a CAN to remote computer!
{
		CancelSendBinaryFile () ;
		MessageBox (GetActiveWindow(), "File Transfer is canceled by the local computer!", "Warning",
								MB_ICONINFORMATION|MB_OK) ;
		CloseBinaryFile() ;

    return (FT_SUCCESS) ;
}
int XMODEMCLASS::RemoteCancel (void) //CancelBinaryFileTransfer (void)
{
		MessageBox (GetActiveWindow(), "File Transfer is canceled by the remote computer!", "Warning",
								MB_ICONINFORMATION|MB_OK) ;
		CloseBinaryFile() ;

    return (FT_SUCCESS) ;
}
int XMODEMCLASS::CloseBinaryFile (void)
{
	if (Status) DestroyWindow (lpStatus->hStatus) ;
	if (hFile!=INVALID_HANDLE_VALUE)
		CloseHandle (hFile) ;
  CommSetHandShake (oiConn, handshake) ;
	return FT_SUCCESS ;
}
int XMODEMCLASS::ReceiveBinaryFile (void)
{
	rvDataCount = 0 ;
	waittimes = 0 ;
	fCRC = TRUE ;
	bSeq = bLastSeq =1 ;
	startstatus = TRUE ;
	return 1 ;
}
//////////////////////////////////////////////////////////////////////////
//Receive File Funcitions
//
//////////////////////////////////////////////////////////////////////////
int XMODEMCLASS::SendInitChar(void)
{
	startstatus = TRUE ;
	if (fCRC)
  	{
		if (option<3)
       CommPutChar (oiConn, 'C') ;
		else
       CommPutChar (oiConn, 'G') ;

		 }
	else
	  {
	  	CommPutChar (oiConn, NAK) ;
	  }
	fltr_state = WAIT_SOH ;
	return FT_SUCCESS ;
}
int XMODEMCLASS::WaitSOH(void)
{
	int status, rVal ;

	rVal = status =GetSOHChar() ;
	switch(status)
    {
		case FT_LOCALCANCEL:
			LocalCancel() ;
			break ;
		case SOH :
    case STX:
			if (startstatus)
				startstatus = FALSE ;
			rVal = FT_SUCCESS ;
			fltr_state = WAIT_SEQ ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			break ;
		case CAN :
			rVal = FT_REMOTECANCEL ;
			RemoteCancel() ; 
      break ;
		case EOT :
			rVal = FT_FILEEND ;
			EndTransferBinaryFile() ;
			break ;
		default:
			rVal = FT_SUCCESS ;
			break;
		}

	return rVal ;
}
int XMODEMCLASS::GetSOHChar (void)
{
	int rVal ;
	int c ;

	c = CommGetChar (oiConn) ;
	switch (c)
		{
		case SOH:
			maxblocksize = 128 ;
			lErrorCount = 0 ;
			return SOH ;
		case STX:
			maxblocksize = 1024 ;
			lErrorCount = 0 ;
		  return STX ;

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
int XMODEMCLASS::WaitSEQChar (void)
{
	BYTE curSeq  ;
  BYTE cSeq ;
	int  rVal = FT_SUCCESS ;

	curSeq = CommGetChar (oiConn) ;
  cSeq   = CommGetChar (oiConn) ;
	if (curSeq!=((255&cSeq)^255))
		{
		fltr_state = WAIT_SOH ;
    CommPutChar (oiConn, NAK) ;
		}
	else
	  {
		if (bSeq!=curSeq)
			{
			if (bLastSeq==curSeq)
				{
				fltr_state = WAIT_SOH ;
      	SendACK() ;
				}
			else
				{
				CancelSendBinaryFile () ;
      	rVal = FT_LOCALCANCEL ;
      	}
			}
		else
  	{
		fltr_state = WAIT_DATA ;
  	}
	}
	RestartTimerEvent (oiFT, maxTimeout) ;
	return(rVal);
	
}
void XMODEMCLASS::ReceiveDataBlock(void)
{
	int numChars =0 ;
	char* szData ;

	szData = new char[maxblocksize+2] ;
	numChars = CommGetBuffer (oiConn, (BYTE*)szData, maxblocksize-rvDataCount) ;
	CopyMemory(&lpBuffer[rvDataCount], szData, numChars) ;
	rvDataCount+=numChars ;
			                       
	if (rvDataCount==maxblocksize)
		fltr_state = WAIT_CHECK ; 

  delete[] szData ;  	
	RestartTimerEvent (oiFT, maxTimeout) ;
}
int XMODEMCLASS::CheckDataBlock (void)
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
		fltr_state = WAIT_SOH ;
		rVal = WriteToFile () ;
		}
	else
		{
		if (option<3)
    	{
			fltr_state =WAIT_SOH ;
			rvDataCount = 0 ;
			setmem(lpBuffer, maxblocksize, ' ') ;
      CommPutChar (oiConn, NAK) ;
			}
		else    //XMODEM1KG	protocol, if has error, exit.
			rVal = FT_LOCALCANCEL ;
		}
	RestartTimerEvent (oiFT, maxTimeout) ;
  return (rVal) ;
}
int XMODEMCLASS::SendACK(void)
{
	if (option<3)
    CommPutChar (oiConn, ACK) ;

  return (FT_SUCCESS) ;
}
int XMODEMCLASS::WriteToFile (void)
{
	int rVal ;
	if (!WriteFile(hFile, lpBuffer, maxblocksize, NULL, NULL))
		{
		rvDataCount = 0 ;
		setmem(lpBuffer, maxblocksize, ' ') ;
   	CommPutChar (oiConn, CAN) ;
		rVal = FT_LOCALCANCEL ;
		}
	else
		{
		if (Status)
			{
			lpStatus->blocksize +=maxblocksize ; 
			lpStatus->blockno ++ ;
			lpStatus->error = lErrorCount ;
			lpStatus->UpdateStatusWindow () ;
			if (lpStatus->transferStop)
				rVal = FT_LOCALCANCEL ;
			}
		if (rVal!=FT_LOCALCANCEL)
     	{
			rvDataCount = 0 ;
			setmem(lpBuffer, maxblocksize, ' ') ;
      SendACK() ;
			bLastSeq = bSeq ;
			bSeq++ ;
			rVal = FT_SUCCESS ;
			}	
		}
	return(rVal);
}
int XMODEMCLASS::EndTransferBinaryFile(void)
{
	if(Status)
		DestroyWindow(lpStatus->hStatus) ;
	if (handshake==XONXOFF)
    CommSetHandShake (oiConn, handshake) ;
	CommPutChar (oiConn, ACK) ;
	if (hFile!=INVALID_HANDLE_VALUE)
		CloseHandle (hFile) ;
	return (FT_SUCCESS) ;
}
//
//
//
BYTE XMODEMCLASS::CheckSum( LPCSTR lpData )
{
   //
   // automatic variables
   //
   int   i;
	 BYTE  bCheckSum = 0;

   //
   // loop through all 128 characters
   //
   for ( i = 0; i < 128; i++ )
      bCheckSum += lpData[ i ];
   //
   // return the checksum value
   //
   return( bCheckSum );
}
static int CrcTable[256] = {
   0, 4129, 8258, 12387, 16516, 20645, 24774, 28903,
   -32504,-28375,-24246,-20117,-15988,-11859,-7730,-3601,
   4657, 528, 12915, 8786, 21173, 17044, 29431, 25302,
   -27847,-31976,-19589,-23718,-11331,-15460,-3073,-7202,
   9314, 13379, 1056, 5121, 25830, 29895, 17572, 21637,
   -23190,-19125,-31448,-27383,-6674,-2609,-14932,-10867,
   13907, 9842, 5649, 1584, 30423, 26358, 22165, 18100,
   -18597,-22662,-26855,-30920,-2081,-6146,-10339,-14404,
   18628, 22757, 26758, 30887, 2112, 6241, 10242, 14371,
   -13876,-9747,-5746,-1617,-30392,-26263,-22262,-18133,
   23285, 19156, 31415, 27286, 6769, 2640, 14899, 10770,
   -9219,-13348,-1089,-5218,-25735,-29864,-17605,-21734,
   27814, 31879, 19684, 23749, 11298, 15363, 3168, 7233,
   -4690,-625,-12820,-8755,-21206,-17141,-29336,-25271,
   32407, 28342, 24277, 20212, 15891, 11826, 7761, 3696,
   -97,-4162,-8227,-12292,-16613,-20678,-24743,-28808,
   -28280,-32343,-20022,-24085,-12020,-16083,-3762,-7825,
   4224, 161, 12482, 8419, 20484, 16421, 28742, 24679,
   -31815,-27752,-23557,-19494,-15555,-11492,-7297,-3234,
   689, 4752, 8947, 13010, 16949, 21012, 25207, 29270,
   -18966,-23093,-27224,-31351,-2706,-6833,-10964,-15091,
   13538, 9411, 5280, 1153, 29798, 25671, 21540, 17413,
   -22565,-18438,-30823,-26696,-6305,-2178,-14563,-10436,
   9939, 14066, 1681, 5808, 26199, 30326, 17941, 22068,
   -9908,-13971,-1778,-5841,-26168,-30231,-18038,-22101,
   22596, 18533, 30726, 26663, 6336, 2273, 14466, 10403,
   -13443,-9380,-5313,-1250,-29703,-25640,-21573,-17510,
   19061, 23124, 27191, 31254, 2801, 6864, 10931, 14994,
   -722,-4849,-8852,-12979,-16982,-21109,-25112,-29239,
   31782, 27655, 23652, 19525, 15522, 11395, 7392, 3265,
   -4321,-194,-12451,-8324,-20581,-16454,-28711,-24584,
   28183, 32310, 20053, 24180, 11923, 16050, 3793, 7920
};

//
// CalcCRC16
//
unsigned int XMODEMCLASS::CalcCRC16( LPCSTR lpData, long size )
{
	//
  // automatic variables
  //
  unsigned int   i, crc;
  unsigned char  crch1;

  //
  // initialize the crc to 0
  //
  crc = 0;

  //
  // loop through all 128 characters
  //
	for ( i = 0; i < size ; i++)
	 {                               //128; i++ ) {
   crch1 = crc >> 8;
   crch1 = crch1  ^ lpData[i];
	 crc = crc << 8;
	 crc = crc ^ CrcTable[ crch1 ];
   }

  //
  // return the crc
  //
  return( crc );
} 
