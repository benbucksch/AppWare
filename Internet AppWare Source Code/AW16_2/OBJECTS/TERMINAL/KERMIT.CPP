#define GETMARK 1
#define GETPACKET 2
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
// FILE:"Kermit.cpp"
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

#define RECEIVE_INIT 2
#define SEND_PACKET 3
#define WAIT_PACKET 4
#define WAIT_MARK   5
#define WAIT_LEN    6
#define WAIT_BLOCK  7
#define WAIT_CHECK  8
#define GET_PACKET  9
#define SEND_ACKPACKET 10

#include <stdlib.h>
#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <mem.h>
#include "filetrns.h"

extern void ActiveIdleEvent (OBJECTID oiFT) ;
KERMITCLASS::KERMITCLASS (
	OBJECTID oiFT, 
	char *fileName, 
	BOOL sORr) :FTCLASS (oiFT, fileName, sORr)
{
	OFSTRUCT of ;
	FILETRANSFER fltr(oiFT) ;

	if (hFile!=HFILE_ERROR) 
		CloseHandle(hFile) ;
	hFile = HFILE_ERROR ;
	if (sORr)
		{
		hFile = CreateFile (fileName, &of, OF_READ);
		if (hFile!=HFILE_ERROR)
			{
			filesize =SetFilePointer (hFile,0,2) ;
			SetFilePointer (hFile, 0, 0) ;
			}
    }
	if (lpBuffer==NULL)
 		return ;
	buffer = new char[1000] ;
	rcvpkt = new char[1000] ;
	resendBuf = new char[1000] ;
	filename = new char[255] ;
	lstrcpy (filename, fileName) ;

	my_qbin_char     = 'N';
	repeat_flag      = 1;
	my_repeat_char   = KERMIT_MY_REPEAT_CHARACTER;
	max_length       = KERMIT_MY_MAX_SIZE;
  time_out         = KERMIT_MY_PACKET_TIMEOUT;
	pad_count        = KERMIT_MY_PAD_COUNT;
  pad_char         = KERMIT_MY_PAD_CHAR;
  eol              = KERMIT_MY_EOL;
  quote_char       = KERMIT_MY_QUOTE_CHAR;
  qbin             = 0;
  his_qbin_char    = 'N';
	his_capabilities = 0;
	//text_mode = FALSE ;
  parity = FALSE ; //need get from buildcomm.cpp
	blockno = 0 ;

	maxTimeout   = fltr.data0->maxTimeout*60 ;
  maxTrytimes  = fltr.data0->maxTrytimes ;
	AObjSetRuntimeInfo (oiFT, (long)this) ;
}
KERMITCLASS::~KERMITCLASS(void)
{
	if (buffer) delete[] buffer ;
	if (rcvpkt) delete[] rcvpkt ;
	if (resendBuf) delete[] resendBuf ;
	if (filename) delete[] filename ;
	if (buffer) delete[] buffer ;
	if (directory) delete[] directory ;
}
int KERMITCLASS::Initiation (BOOL bBar)
{
	int rVal = FT_SUCCESS ;
  
	lErrorCount = 0 ;
	if (Status)
		{
		if (!lpStatus->InitStatusWindow (2, bBar, "KERMIT", filename, filesize))
			rVal = FT_LOCALCANCEL ;
		}
	return rVal ;
}
void KERMITCLASS::StartProtocol (OBJECTID oiFT, int sendOrReceive)
{
	int rVal ;
	if (sendOrReceive==SENDFILE)
	  {
		fltr_state = WAIT_PACKET ;
		receive_state = WAIT_MARK ;
		rVal = Initiation(TRUE) ;
		SendPacket ('S') ;
		}
	else
	  {
	  rVal = Initiation(FALSE) ;
		ReceiveBinaryFile () ;
		SendACKorNAK ('N') ;					 
		fltr_state = GET_PACKET ;
		receive_state = WAIT_MARK ;
		type = 'S';
	  }
	if (rVal ==FT_SUCCESS)
	  {
		SetTimerEvent (oiFT, setPollTimer, 15) ;
		SetTimerEvent (oiFT, setDataTimer, maxTimeout) ;
		}
	else
		AEvtPostSignalAtMark (oiFT, 6) ;
}
int KERMITCLASS::HandleBufferData ()
{
	int rVal = FT_SUCCESS ; 

	lErrorCount++ ;
	if (sendOrReceive == SENDFILE)
	  {
		if (lErrorCount<maxTrytimes)
		  {
	 		if (receive_state==WAIT_MARK)
	 		  {
				SendPacket (type) ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			}
		else
		  {
			if(lErrorCount<maxTrytimes)
			  {
			  SendPacket('N') ;
				RestartTimerEvent (oiFT, maxTimeout) ;
			  }
			else
				rVal = FT_TIMEOUT;
		  }
	  }
	else
	  {
	  if (lErrorCount<maxTrytimes)
	    {
			SendPacket('N') ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			if (Status)
				{
				lpStatus->error = lErrorCount ;	
				lpStatus->UpdateStatusWindow () ;	
				if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;	
		}
			}
	  }

	return (rVal);
	
}
int KERMITCLASS::ProcessBufferData (void) 
{
	int rVal ;
	switch (fltr_state)
	  {
		case SEND_PACKET:
			rVal = SendPacket (nextType) ;
			break;
		case WAIT_PACKET:
			rVal = WaitACKPacket () ;
			break;
		case GET_PACKET:
			rVal = GetPacket () ;
			break;
		case SEND_ACKPACKET:
			rVal = SendACKPacket () ;
			break;
	  }
	
	if (rVal !=FT_SUCCESS)
	  {
		EndFileTransfer() ;
    KillTimerEvent (oiFT, setPollTimer) ;
		KillTimerEvent (oiFT, setDataTimer) ;
		if (rVal==FT_ALLFILEEND)
			AEvtPostSignalAtMark (oiFT, 0) ;
		else
			AEvtPostSignalAtMark (oiFT, 6) ;
	  }

	return (rVal);
	
}
//////////////////////////////////////////////////////////////////////////
//Kermit Protocol: Send a File
//////////////////////////////////////////////////////////////////////////
int KERMITCLASS::SendPacket(char packetType)
{
	int  i=0, j;
	int  status;
	long checksum ;

  InitPacket (packetType) ;
	//*
 	//* Load the SOH, length and packet number and packet type first.
  //*
  lpBuffer[ i++ ] = KERMIT_MY_MARK_CHARACTER;
  lpBuffer[ i++ ] = (char)tochar(data_length + 3);
	lpBuffer[ i++ ] = (char)tochar(blockno & 0x3f ) ; 
  packno = blockno & 0x3f ;
	lpBuffer[ i++ ] = type; //packetType;
  //*
  //* Load any data that is being sent.
	//*
	for( j = 0; j < data_length; j++ )
		lpBuffer[ i++ ] = buffer[j];
  lpBuffer[ i ] = '\0';
  //*
  //* Finish with the checksum value and an end marker.
  //*
	checksum =  ChecksumCalculate( (BYTE*)(lpBuffer + 1) );
  lpBuffer[ i++ ] = (char)tochar( checksum ) ; 
  lpBuffer[ i++ ] = eol;
  lpBuffer[ i   ] = '\0';
	block_size = data_length;
	for( i = 0; i < pad_count; i++ )
		{
    if (CommPutChar (oiConn, pad_char)<0)
			return (FT_ERROR) ; //need check ok or not
		}

  //*
  //* Send the packet.
  //*
	status = CommPutBuffer (oiConn, (BYTE*)lpBuffer, lstrlen ((char*)lpBuffer)) ;
  lstrcpy ( resendBuf, (char *) lpBuffer) ;
	if( status < 0 )
		return FT_ERROR ;
	fltr_state = sendOrReceive?WAIT_PACKET:GET_PACKET ;
	return FT_SUCCESS ;
} 
int KERMITCLASS::InitPacket (char packetType)
{
	//fltr_state = PACKET_INIT ;
	type = packetType ;
	switch(packetType)
		{
		case 'S':
			InitSPacket() ;
     	nextType = 'F' ;
			break ;

		case'F':
			InitFPacket() ;
     	nextType = 'D' ;
			break ;

		case'D':
			if(InitDPacket ()==FT_FILEEND)
     		{
        nextType = 'B' ;
				}
			else
				nextType = 'D' ;
				break ;
		case'Y':
		case'N':
			InitOtherPacket();
			break ;

		case'R':
			InitSPacket() ;
			type = 'Y' ;
			break ;

		case'B':
			InitOtherPacket() ;
     	nextType = 0 ;
			break ;

		case'A':
			break ;

		case 'E':
    	break ;
		}
  return (FT_SUCCESS) ;
}
int KERMITCLASS::InitSPacket( void) //char data[ 10 ] )
{
	buffer[ 0 ] = tochar( KERMIT_MY_MAX_SIZE );
  buffer[ 1 ] = tochar( KERMIT_MY_PACKET_TIMEOUT );
	buffer[ 2 ] = tochar( KERMIT_MY_PAD_COUNT );
  buffer[ 3 ] = ctl( KERMIT_MY_PAD_CHAR );
  buffer[ 4 ] = tochar( KERMIT_MY_EOL );
  buffer[ 5 ] = KERMIT_MY_QUOTE_CHAR;
  buffer[ 6 ] = my_qbin_char;
  buffer[ 7 ] = '1';
	buffer[ 8 ] = my_repeat_char;
  if( repeat_flag ) 
  	data_length = 9 ;
  else 
		data_length = 8 ;
	type = 'S' ;
	if (Status)
		{
		lpStatus->blockno++ ;
		lpStatus->error = lErrorCount ;
		lpStatus->UpdateStatusWindow () ;
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
		}
  return (FT_SUCCESS) ;
	
}
int KERMITCLASS::InitFPacket(void)
{
	char      drive[10] ;
	char      dir[256] ;
	char      name[256] ;
	char      ext[10] ;

	fnsplit (filename, drive, dir, name, ext) ;
	lstrcpy (filename, name) ;
	strcat (filename, ext) ;

	lstrcpy(buffer, filename) ;
	data_length = lstrlen(buffer) ;
	type = 'F';
	if (Status)
		{
		lpStatus->blockno++ ;
		lpStatus->error = lErrorCount ;
		lpStatus->UpdateStatusWindow () ;
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
		}
	
  return (FT_SUCCESS) ;
}
int KERMITCLASS::InitDPacket( void )
{
		data_length = ReadFileToBuffer(buffer);
		type = 'D' ;
		if( data_length == 0 )
		{
				type = 'Z' ;
				return (FT_FILEEND) ;
    }
		if (Status)
			{
			lpStatus->blockno++ ;
			lpStatus->blocksize +=datalen ;
			lpStatus->error = lErrorCount ;
			lpStatus->UpdateStatusWindow () ;
			if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
		}
    return( FT_SUCCESS );
}
int KERMITCLASS::InitOtherPacket ( void )
{
		buffer[0] = '\0' ;
		data_length = 0 ;
		return (FT_SUCCESS) ;
}
int KERMITCLASS::ReadFileToBuffer( char * buf ) //need check again!!!
{
  int i;
  int c ;
  int c7;
  int mask;
  int repeat_char = my_repeat_char ; //add Feb. 20
  int repeat_count;
  int repeat_position;
	int width;

  datalen = 0;
	if( text_mode )
    mask = 0x7f;
  else
    mask = 0xff;

  i = 0;
	while( i < ( max_length - 6 ) )
	{
    if( ReadFileByte( &c ) != 1 )
			c = EOF;
    else
			datalen++ ;

		if( repeat_flag )
		{
			if( c != repeat_char )
			{
        repeat_char = c;
        repeat_count = 0;
				repeat_position = i;
      }
			else if( ++repeat_count > 1 )
			{
				while( repeat_count < 94 )
				{
          if( ReadFileByte( &c ) != 1 )
						c = EOF;
          else
					 datalen++ ;
          repeat_count++;
          if( c != repeat_char )
            break;
        };
        width = (i - repeat_position) / 2;
        while( width-- > 0 )
          buf[ repeat_position + 2 + width ] = buf[ repeat_position + width ];
        buf[ repeat_position ] = my_repeat_char;
        buf[ repeat_position + 1 ] = (char) tochar( repeat_count );
        repeat_char = c;
        repeat_count = 0;
        width = ( i - repeat_position ) / 2;
        repeat_position = repeat_position + 2 + width;
        i = repeat_position;
      }
    }

    if( c == EOF )
      break;
    c7 = c & 0x7f;
    c &= mask;
		if( c & 0x80 && qbin )
		{
      buf[ i++ ] = my_qbin_char;
      c = c7;
    }
		if( c7 < 0x20 || c7 == 0x7f )
		{
      buf[ i++ ] = KERMIT_MY_QUOTE_CHAR;
      c = ctl( (char) c );
    }
    else if( c7 == KERMIT_MY_QUOTE_CHAR )
      buf[ i++ ] = KERMIT_MY_QUOTE_CHAR;
    else if( qbin && c7 == my_qbin_char )
      buf[ i++ ] = KERMIT_MY_QUOTE_CHAR;
    else if( repeat_flag && c == my_repeat_char )
      buf[ i++ ] = KERMIT_MY_QUOTE_CHAR;
    buf[ i++ ] = (char)c;
	}
  return (i) ; 
}
int KERMITCLASS::ResendPacket (void)
{
  int i ;

	for( i = 0; i < pad_count; i++ )
		{
    if (CommPutChar (oiConn, pad_char) <0)
			return (FT_ERROR) ;
		}
	SendPacket (type) ;
	RestartTimerEvent (oiFT, maxTimeout) ;
	return (FT_SUCCESS) ;
}
int KERMITCLASS::WaitACKPacket ()
{
	char return_type ;
  int rVal = FT_SUCCESS ;

	return_type = ReceivePacket();
	switch( return_type )
		{
		case 'Y':
			if ((blockno&0x3f) ==packno)
				{
				blockno++ ;
				if (type=='B')
					rVal = FT_ALLFILEEND ;
				fltr_state = SEND_PACKET ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			else
				rVal=FT_ERROR ;
			break ;
		case 'N' :
			ResendPacket () ;
      break ;
    case 'A' :
			rVal= FT_ERROR ;
      break ;
    case 'T' :
			rVal =FT_TIMEOUT ;
      break ;
		case 'E':
			rVal = FT_ERROR ;
			break ;
		case 'Q':
    	rVal =FT_ERROR ;
		default:
			break;
		}

  return (rVal) ;
}
int KERMITCLASS::ReceivePacket()
{
	BYTE c ;
	int i=0 ;
	int rVal = FT_SUCCESS ;

	switch(receive_state)
	  {
		case WAIT_MARK:
		  {
			c=CommGetChar(oiConn) ;
			if (c==KERMIT_MY_MARK_CHARACTER)
			  {
			 	receive_state = WAIT_LEN ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			}
			break;
		case WAIT_LEN:
		  {
			lpBuffer[0]=CommGetChar (oiConn) ;
			if (lpBuffer[0]!='\0')
			  {
				receive_state = WAIT_BLOCK ;
				packetlen = unchar (lpBuffer[0])+1 ;
				rvDataCount = 1 ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			}
			break;
		case WAIT_BLOCK:
		  {
			char *szData = new char[packetlen+1] ;
			int numchars = CommGetBuffer (oiConn, (BYTE*)szData, packetlen-rvDataCount) ;
			if(numchars)
			  {
				CopyMemory (&lpBuffer[rvDataCount], szData, numchars) ;
		  	rvDataCount+=numchars ;
				if (rvDataCount==packetlen)
			  	receive_state = WAIT_CHECK ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			delete[]szData ;
			}
			break;
		case WAIT_CHECK:
		  {
			block_size = unchar(lpBuffer[i++]) ;
			int rseq = unchar(lpBuffer[i++]) ;
	    int type = lpBuffer[i++] ;
	    packno = rseq ;
	    int sum = unchar(lpBuffer[packetlen-1]) ;
	    lpBuffer[packetlen-1] = '\0' ;
	    if (ChecksumCalculate((BYTE*)lpBuffer)!=(int)sum) 
		     return('Q');
			receive_state = WAIT_MARK ;
			RestartTimerEvent (oiFT, maxTimeout) ;
    	return(type);
			}
		}
	return(rVal);
	
}
int KERMITCLASS::ReadPort(
	char* b,
	int max,
	char eol,
	long msecs)
{
	int i=0 ;
	long timeout ;
	int rVal ;
	int c ;

  timeout =GetCurrentTime() +msecs ;
	*b='\0' ;
	for (i=0;i<max; i++)
		{
    c = CommWaitChar (oiConn, 1000) ;
		if (c>0) 
			{
			b[i]= (char)c ;
			if ((char) c==eol)
				{
				b[i] = '\0' ;
				rVal = i ;
        break ;
        }
			}
		if (timeout<GetCurrentTime())
			{
			MessageBox (NULL, "TimeOut", "ReadPort", MB_OK) ;
			rVal = FT_TIMEOUT ;
			break ;
			}
		if (Status&&lpStatus->transferStop) return (FT_LOCALCANCEL) ;
    }
  return (rVal) ;
}
void KERMITCLASS::EndFileTransfer (void)
{
	if (hFile!=HFILE_ERROR)
		{
		_close (hFile) ;
    hFile = HFILE_ERROR ;
		}
	if (Status) DestroyWindow (lpStatus->hStatus) ;
	ActiveIdleEvent (oiFT) ;
}
//////////////////////////////////////////////////////////////////////////
//Receive a file
//////////////////////////////////////////////////////////////////////////
void KERMITCLASS::LoadInitParameters( void )
{
  if( block_size > 0 ) max_length = unchar(lpBuffer[4]) ; 
  if( block_size > 1 ) time_out = (char)unchar(lpBuffer[5]) ; 
	if( block_size > 2 ) pad_count = (char)unchar(lpBuffer[6]) ;
  if( block_size > 3 ) pad_char = (char)ctl(lpBuffer[7]) ; 
	if( block_size > 4 ) eol = (char)unchar(lpBuffer[8]) ; 
  if( block_size > 5 ) quote_char = lpBuffer[9] ;  
	if( block_size > 6 ) his_qbin_char = lpBuffer [10] ; 
  //*
  //* This is where I figure out whether or not we are going to do run
  //* length encoding.  If repeat_flag is 0, I won't use rle no matter what.
  //* If repeat_flag is 1, I am sending and want to use it.  If repeat flag is
  //* -1, I am receiving and want to use it.
  //*
	if( block_size > 8 )
	{
    his_repeat_char = lpBuffer[12] ; //rdatap[8] ; //lpBuffer[ 12 ];
		if( repeat_flag == -1 )
		{
      my_repeat_char =  his_repeat_char;
      if( (his_repeat_char > 32 && his_repeat_char < 63) ||
          (his_repeat_char > 95 && his_repeat_char < 127) )
        repeat_flag = 1;
      else
        repeat_flag = 0;
    }
		else if( repeat_flag == 1 )
		{
      if( my_repeat_char != his_repeat_char )
        repeat_flag = 0;
    }
  }
	else
    repeat_flag = 0;
  //*
  //* Here is where I negotiate eighth bit prefixing.
  //*
  if( my_qbin_char == 'Y' ) {
    if( his_qbin_char != 'N' && his_qbin_char != 'Y' ) {
      qbin = 1;
      my_qbin_char =  his_qbin_char;
    }
  }
  if( my_qbin_char == '&' ) {
    if( his_qbin_char == '&' || his_qbin_char == 'Y' ) {
      qbin = 1;
    }
  }
}
//
//
//
int KERMITCLASS::ReceiveBinaryFile (void)
{
	if (Status)
		{
		lstrcpy(lpStatus->filename,"") ;
    lpStatus->UpdateStatusWindow() ;
    }
	buffer = new char[128] ;
	data_length = 0 ;
  block_size = 0 ;
	start_time = AAppGetTime() ; 
	directory = new char[256] ;
	lstrcpy (directory, filename) ;
	return (1) ;
}
int KERMITCLASS::GetPacket (void)
{
	char packetType ;
	int rVal = FT_SUCCESS ;

	packetType = ReceivePacket();
	switch ( packetType )
		{
		case 'S':
			if (type=='S')
				{
        nextType = 'F' ;
				LoadInitParameters() ;
				fltr_state = SEND_ACKPACKET ;
				RestartTimerEvent (oiFT, maxTimeout) ;
				}
			else if(type=='F')
				rVal = FT_SUCCESS ;
			else
				rVal = FT_ERROR ; 
			break;

		case 'F':
			if (type=='F')
				{
        nextType = 'D' ;
				rVal = GetFileName() ;
				fltr_state = SEND_ACKPACKET ;
				RestartTimerEvent (oiFT, maxTimeout) ;
        }
			else if (type=='D')
				rVal = FT_SUCCESS ;
			else
				rVal = FT_ERROR	;
			break;

		case 'D':
			nextType = 'D' ;
			rVal = SaveDataBlock () ;
			fltr_state = SEND_ACKPACKET ;
			break;
			
		case 'Z':
			nextType = 'B' ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			fltr_state = SEND_ACKPACKET ;
			break;
			
		case 'B':
			SendACKPacket () ;
			RestartTimerEvent (oiFT, maxTimeout) ;
			rVal = FT_ALLFILEEND ;
			break;
			
		case 'Q':
			SendACKorNAK ('N') ;
			break;

		case 'E':
			rVal = FT_ERROR ;
			break;

   	default:
			break;
		}
	 
	return(rVal);
}
int KERMITCLASS::SendACKPacket (void)
{
	int result ;
	if (type=='S')
		result = SendInitialAck() ;
	else
		result = SendACKorNAK('Y') ;

	if (result==FT_SUCCESS)
		{
		if (type=='B')
			EndFileTransfer() ;
		else
    	type = nextType ;
		fltr_state = GET_PACKET ;
    }
	else
		ResendPacket () ;

	return (FT_SUCCESS) ;
	
}
int KERMITCLASS::GetInitPacket (void)
{
	type = ReceivePacket();
	if (type =='S')
		LoadInitParameters() ;

  return (type) ;
}
int KERMITCLASS::SendInitialAck( void )
{

	//InitSPacket() ;
	//type ='Y' ;
	if( SendPacket('R')!=FT_SUCCESS ) //R: Send Init ACK ;
		{
		return FT_ERROR ;
    }
  blockno ++ ;
	return FT_SUCCESS ;
}
int KERMITCLASS::SendACKorNAK (char nChar)
{
	if ( SendPacket(nChar)!=FT_SUCCESS)
		return (FT_ERROR) ;

	if (nChar=='Y') blockno ++ ;
	return (FT_SUCCESS) ;
}
int KERMITCLASS::GetFileName (void)
{
	char     path[256] ;
	OFSTRUCT of ;
	int      rVal ;

	if (hFile!=HFILE_ERROR)
	{
			CloseHandle (hFile) ;
      hFile = HFILE_ERROR ;
  }
	lstrcpy( filename, lpBuffer+3) ; 
	lstrcpy(path, directory) ;
	strcat (path, "\\") ;
	strcat (path, filename) ;
	hFile = CreateFile (path, &of, OF_CREATE) ;
	lErrorCount = 0 ;
	if (Status)
		{
		lpStatus->writeName = TRUE ;
 		lstrcpy(lpStatus->filename, filename) ;
		lpStatus->blocksize =0 ;	
		lpStatus->error = lErrorCount ;	
		lpStatus->UpdateStatusWindow () ;	
		if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;	
		lpStatus->writeName = FALSE ;
		}	
	if (hFile!=HFILE_ERROR)
		rVal = FT_SUCCESS ;
	else
		rVal = FT_ERROR ;

	return (rVal);
	
}
int KERMITCLASS::SaveDataBlock (void)
{
	if( (blockno & 0x3f ) == packno )
		{
  	if (!SaveBuffer() )
			return( FT_ERROR );
		if (Status)
			{
			lpStatus->blockno++ ;
			lpStatus->blocksize +=datalen ;
			lpStatus->error = lErrorCount ;
			lpStatus->UpdateStatusWindow () ;
			if (lpStatus->transferStop) return (FT_LOCALCANCEL) ;
			}
		}
		else
				return (NAK) ;
		return (FT_SUCCESS) ;//(type) ;
}
int KERMITCLASS::CloseFile (void)
{
	int rVal = FT_ERROR ;

	if (hFile!=HFILE_ERROR)
		{
		CloseHandle(hFile) ;
		hFile = HFILE_ERROR ;
		rVal = FT_SUCCESS ;
    }
	return (rVal) ;
}
//
//
//
int KERMITCLASS::SaveBuffer( void )
{
  unsigned char c;
  int i;
  int repeat_count;
  int bit8;
  int result;

	i = 0;
  datalen = 0 ;
	while( i < block_size)
	{ 
		c = lpBuffer[ 4 + i++ ]; //rdatap[i++] ; 
    repeat_count = 1;
    if( repeat_flag ) {
			if( c == (unsigned char)his_repeat_char )
			{
				repeat_count = unchar(lpBuffer[ 4 + i++ ] ); //rdatap[i++]) ;
        c = lpBuffer[ 4 + i++ ]; //rdatap[i++] ;
      }
    }
    bit8 = 0;
    if( qbin ) {
			if( c == (unsigned char)his_qbin_char )
			{
        c = lpBuffer[ 4 + i++ ]; //rdatap[i++] ;        
        bit8 = 128;
      }
    }
		if( c == (unsigned char)quote_char )
		{
      c = lpBuffer[ 4 + i++ ]; //rdatap[i++] ; 
      if( ( c & 0x7f ) > 62 && ( c & 0x7f ) < 96 )
        c = (char) ctl( c );
    }
    c |= bit8;
    if( text_mode )
      c &= 0x7f;
    do {
      result = WriteFileByte( c );
      if( result != 1 ) {
        return( FT_ERROR );
			}
			datalen++ ;
    } while( --repeat_count > 0 );
  }							  
	return( TRUE );
}
//
//
//
void KERMITCLASS::PostEvent(OBJECTID oiObj, EVENTID what, long modifiers, long how)
{
	AEvtInfo event ;

	event.sourceObject = oiObj ;
	event.targetObject = oiObj ;
	event.what         = what ;
	event.details      = (long) this ;//details ;
	event.modifiers    = modifiers ;
	event.when         = AAppGetTime() ;
	event.how          = how ;

	AEvtPost (&event) ;
}
int KERMITCLASS::ChecksumCalculate( BYTE *buffer )
{
  unsigned int checksum;
  unsigned int kermit_checksum;

  checksum = 0;
	while( *buffer != '\0' )
    checksum += *buffer++;
  kermit_checksum = ( checksum & 0xc0 ) >> 6;
  kermit_checksum += checksum;
  kermit_checksum &= 0x3f;
	return( (int) kermit_checksum );
}
int KERMITCLASS::ReadFileByte( int *c )
{
  return( ReadFile(hFile, c, 1) );
}

int KERMITCLASS::WriteFileByte( int c )
{
	return( WriteFile(hFile, &c, 1) );
}
