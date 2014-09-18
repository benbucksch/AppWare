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
// FILE:"zmodems.cpp"
//
//      protected:
//         ZMODEMCLASS::OpenLink()
//         ZMODEMCLASS::OpenNextFile()
//         ZMODEMCLASS::CloseLink()
//         ZMODEMCLASS::ReadBlock()
//         ZMODEMCLASS::BackChannelActivity()
//         ZMODEMCLASS::SendDataFrame()
//         ZMODEMCLASS::StartFileTransfer()
//         ZMODEMCLASS::SendFileData()
//         ZMODEMCLASS::_SendFile()
//
// AUTHOR:
//
///////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT	
#define UNIX_FIRST_DAY 2440588L

#include <sys/stat.h>
#include <string.h>
#include <mem.h>
#include <io.h>
#include <dir.h>
#include <stdlib.h>
#include <time.h>
#include "filetrns.h"
#include "zmmacro.h"
#include "crc.h"
#include "zmodem.h"

long ToJulian(int y, int m, int d )
{
  return( (long)(d - 32076)
      + 1461L * (y + 4800L + (m - 14) / 12) / 4
      + 367 * (m - 2 - (m - 14) / 12 * 12) / 12
      - 3 * ((y + 4900L + (m - 14) / 12) / 100) / 4
      + 1 );
}
long CreateUnixTime( 
	int year,
  int month,
  int day,
  int hour,
  int minute,
  int second)
{
  long result = ToJulian( year, month, day );
  result -= UNIX_FIRST_DAY;
  if( result >= 0L ) {
    result *= 3600L * 24;
    result += 3600L * hour;
    result += 60L * minute;
    result += second;
  }
  else
    result = 0L;
  return( result );
}

///////////////////////////////////////////////////////////////////////////
//* int ZMODEMCLASS::OpenLink( void )
//*
//////////////////////////////////////////////////////////////////////////
int ZMODEMCLASS::OpenLink( void )
{
  int  frame_type;
	int  rVal = TRUE ;
	BOOL loop = TRUE ;

	lErrorCount = 0 ;
  if (SendHexHeader(ZRQINIT, (unsigned char *)"\0\0\0\0" )==FT_SUCCESS )
   	{
  	while(loop) 
  		{
			if (CheckAbort()) break ;
		
    	frame_type = ReadFrameHeader( FALSE );
    	switch( frame_type ) 
    		{
      	case ZRINIT :
        	rx_buffer_size = (header[ 0 ] & 0xff) + ((header[ 1 ] << 8) & 0xff00);
        	rx_flags = header[ 3 ];
					loop = FALSE ;
        	break ;

				case ZCHALLENGE:
					SendHexHeader (ZACK, header) ;
					continue ;
					
				case ZCOMMAND:
					SendHexHeader (ZRQINIT, (unsigned char*)"\0\0\0\0") ;
					continue;

				case ZCAN :
				case FT_TIMEOUT:
					lErrorCount++ ;
					UpdateStateWindow (lErrorCount, 0, 0, NULL) ;
					if (lErrorCount<ZMODEM_MAX_ERRORS)
							break;	
					loop = FALSE ;
					rVal = FALSE ;
					break;

				default:
					SendHexHeader (ZNAK, header) ;
					continue;
				}
			}
		}
	else
		rVal = FALSE ;
    
  return (rVal);
 }

//*
//* int ZMODEMCLASS::OpenNextFile( void )
//*
int ZMODEMCLASS::OpenNextFile( void )
{
  struct stat buf;
  struct tm *t ;
	long       unix_time;
	char       drive[10] ;
	char       dir[50] ;
	char       name[256] ;
	char       ext[10] ;
	OFSTRUCT   of ;
	int        rVal = FALSE ;

	if (filecount)
		{
	 	hFile = OpenFile(filename, &of, OF_READ) ;
  	if (hFile!=HFILE_ERROR)
			{
			fnsplit (filename, drive, dir, name, ext) ;
			strcat (name,ext) ;
		
			stat (filename, &buf) ;
			filesize = buf.st_size;
  		t = localtime(&buf.st_mtime);
			unix_time = CreateUnixTime( t->tm_year + 1900,
      	                       	 	t->tm_mon + 1,
        	                      	t->tm_mday,
          	                    	t->tm_hour,
            	                  	t->tm_min,
														      t->tm_sec );
  		block_size = wsprintf (lpBuffer, "%s%c%ld %lo 0", name, 0x00, filesize, unix_time) ;
			if (Status&&lpStatus)
				 lpStatus->UpdateFileSize (filesize) ;
			UpdateStateWindow (0,0,0,name) ;
			filename +=strlen(filename)+1	;
			filecount-- ;
			rVal = TRUE ;
			}
		}
	return(rVal);
}


//*
//* int ZMODEMCLASS::CloseLink( void )
//*
//* ARGUMENTS
//*   NONE
//*
int ZMODEMCLASS::CloseLink( void )
{
  int frame_type;
	int result;
	int rVal = FT_ERROR ;

  for( ; ; ) {
		if (CheckAbort()) break;

		result = SendHexHeader( ZFIN , (unsigned char *) "\0\0\0\0" );
    if (result!=FT_SUCCESS) 
    	break ;
    frame_type = ReadFrameHeader( FALSE );
    if (frame_type == ZFIN)
			{
			rVal = FT_SUCCESS ;
      break;
			}
    if (frame_type<=FT_ERROR)
      break;
    lErrorCount++;
    if (lErrorCount >= ZMODEM_MAX_ERRORS)
			break;
		rVal =FT_SUCCESS ;	
  }
  WritePort('O');
  WritePort('O');
	CloseStateWindow();
	return (rVal);
}
//*															  
//* int ZMODEMCLASS::ReadBlock( void )
//*
int ZMODEMCLASS::ReadBlock(int& error )
{
  int bytes_to_read;

  if( rx_buffer_size > 0 )
    bytes_to_read = rx_buffer_size - rx_frame_output_count;
  else
    bytes_to_read = ZMAXSPLEN;
  if( bytes_to_read > ZMAXSPLEN )
    bytes_to_read = ZMAXSPLEN;
 
	_llseek (hFile, nbytes, SEEK_SET) ;
  block_size = ReadFileBlock( bytes_to_read );
  if(block_size==HFILE_ERROR) 
  	{
    error = FT_ERROR;
    return( TRUE );
  	}
  nbytes += block_size;
  rx_frame_output_count += block_size;

  if( rx_buffer_size == 0 ) 
  	{
    if( (rx_flags & CANOVIO ) == 0 )
      return( TRUE );
  	}
  else 
  	{
    if( rx_frame_output_count >= rx_buffer_size )
      return( TRUE );
  	}
  if (nbytes >= filesize)
    return( TRUE );
  return( FALSE );
}


//*
//* int ZMODEMCLASS::BackChannelActivity( void )
//*
int ZMODEMCLASS::BackChannelActivity(int& garbage_count)
{
  int c;

  while( (c = ReadChar(70)) >= FT_SUCCESS) 
  	{
    if( c == CAN || c == ZPAD ) 
    	{
      SendBinaryPacket( 0, ZCRCE );
      return( 1 );
    	}
    else 
    	{
      if( c != XON && c != XOFF ) 
      	{
        if( garbage_count++ > 100 ) 
        	{
          SendBinaryPacket( 0, ZCRCE );
          return( 1 );
        	}
        }
    	}
  	}
  return( 0 );
}
//*
//* void ZMODEMCLASS::SendDataFrame( void )
//*
void ZMODEMCLASS::SendDataFrame(int& error)
{
  char header[ 4 ];
  int  i;
  int  last_subpacket_in_frame;
	int  garbage_count = 0;
	long current_bytes = nbytes ;
	
  if (BackChannelActivity(garbage_count))
    return;
  for( i = 0 ; i < 4 ; i++ )
    header[ i ] = (char)((nbytes >> (i * 8)) & 0xff);
  rx_frame_output_count = 0;
  if (SendBinaryHeader( ZDATA, header)!=FT_SUCCESS)
  	return ;
  for( ; ; ) 
  	{
		if (CheckAbort()) 
			{
			error = FT_ERROR ;
			break;
			}
		//if (!SystemIdle()) break;
		last_subpacket_in_frame = ReadBlock(error);
    if( error_recovery )
      last_subpacket_in_frame = 1;
    if (SendBinaryPacket(block_size, (char)(last_subpacket_in_frame ? ZCRCW : ZCRCG ))!=FT_SUCCESS)
			{
			error = FT_ERROR ;
    	break ;
			}
		if (BackChannelActivity(garbage_count)) 
			break ;
		current_bytes +=block_size ;
		block_number++;
    UpdateStateWindow (lErrorCount, block_number, current_bytes, NULL) ;
		if (last_subpacket_in_frame )
    	break ;
  }
}
//*
//* int ZMODEMCLASS::StartFileTransfer( void )
//*
int ZMODEMCLASS::StartFileTransfer( void )
{
  int frame_type;
	BOOL startover = TRUE ;

  error_recovery = 0;
	lErrorCount = 0 ;
  for( ; ; ) 
  	{
		if (CheckAbort()) break;
		if (startover)
			{
			if (SendBinaryHeader(ZFILE, "\0\0\0\0" )!=FT_SUCCESS)
				return(FT_ERROR);
			if (SendBinaryPacket(block_size, ZCRCW )!=FT_SUCCESS)
    		return (FT_ERROR);
			}
		frame_type = ReadFrameHeader( FALSE );
    switch( frame_type ) 
    	{
      case ZRPOS  :
        nbytes = ((unsigned long)header[ 3 ] << 24) +
                 ((unsigned long)header[ 2 ] << 16) +
                 ((unsigned long)header[ 1 ] << 8) +
                 ((unsigned long)header[ 0 ]);
				return(ZRPOS);
			  
      case ZSKIP:
        return(ZSKIP);
        
		 	case ZCAN:
			case ZABORT:
			case FT_TIMEOUT:
			case FT_ERROR:
			case ZFIN:
				lErrorCount++;
				UpdateStateWindow (lErrorCount, 0, 0, NULL) ;
        if( lErrorCount>= ZMODEM_MAX_ERRORS ) 
          	return (FT_ERROR);
				break;
							
	    default :
        if(frame_type == ZRINIT ) 
        	{
					int c ;
					while((c=ReadChar(5000L))>FT_ERROR) ;
					if (c==ZPAD)
						startover = FALSE ;
					}	
        break;
    }
  }
	return (FT_ERROR) ;
}
//*
//* void ZMODEMCLASS::SendFileData( void )
//*
int ZMODEMCLASS::SendFileData( void )
{
  int frame_type;
  int done;
	int rVal = FT_SUCCESS ;
  int i;
	int error ;

 for( ; ; )
    {
		if (CheckAbort()) 
			{
			rVal = FT_ERROR ;
			break;
			}
		SendDataFrame(error);
    if(error < FT_SUCCESS)
      return (error);
    done = 0;
    while( !done ) 
    	{
      frame_type = ReadFrameHeader(FALSE);
      switch(frame_type) 
      	{
        case ZRPOS:
					nbytes =  ((unsigned long)header[ 3 ] << 24) +
                    ((unsigned long)header[ 2 ] << 16) +
                    ((unsigned long)header[ 1 ] << 8) +
										((unsigned long)header[ 0 ]);
					_llseek (hFile, nbytes, 0) ;
					UpdateStateWindow (lErrorCount, block_number, nbytes, NULL) ;
          error_recovery = 1;
          done = 1;
					break;

        case ZACK:
					//UpdateStateWindow (lErrorCount, block_number, nbytes, NULL) ;
          lErrorCount = 0 ;
          error_recovery = 0;
          done = 1;
					break;

        case ZRINIT:
        case ZSKIP:
          rVal = FT_ERROR ;
          done = 1;
          break;

				default :
          if (frame_type!=FT_SUCCESS)
            {
						rVal = FT_ERROR ;
						done = 1 ;
            }
          break;
      	}
			}
		if (nbytes >= filesize)
      break;
  }
  for( i = 0 ; i < 4 ; i++ )
    header[ i ] = (char)((nbytes >> ( i * 8 ) ) & 0xff);
  SendBinaryHeader( ZEOF, header );
  ReadFrameHeader( FALSE );

  return (rVal);
}
//*
//* void ZMODEMCLASS::_SendFile( void )
//*
void ZMODEMCLASS::_SendFile( void )
{
  if(OpenLink()) 
  	{
    while(OpenNextFile()) 
    	{
			if (CheckAbort()) break ;
      if (StartFileTransfer()==ZRPOS)
				SendFileData();
			if (hFile!=HFILE_ERROR) 
				{
				_lclose (hFile) ;
				}
      }
  	}
  CloseLink();
}
