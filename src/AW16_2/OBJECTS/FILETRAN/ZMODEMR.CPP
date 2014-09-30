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
// FILE:"zmodem.cpp"
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
#include <mem.h>
#include "filetrns.h"
#include "zmmacro.h"
#include "crc.h"
#include "zmodem.h"
//*
//* ZMODEMR.CPP
//*
//* Contains:
//*      protected:
//*         ZMODEMCLASS::GetFileName()
//*         ZMODEMCLASS::SendFileOffset()
//*         ZMODEMCLASS::ConvertHeaderToOffset()
//*         ZMODEMCLASS::ReadFileData()
//*         ZMODEMCLASS::_ReceiveFile()
//*

//*
//* int ZMODEMCLASS::GetFileName( void )
//*
char* StripPathComponent( BYTE *name )
{
  char *p;

  p = strrchr( (char *)name, '\\' );
  if( p )
    return( p + 1 );
  p = strrchr( (char *)name, ':' );
  if( p )
    return( p + 1 );
  return( (char *)name );
}

int ZMODEMCLASS::GetFileName( void )
{
  int result;
  int frame_type;

  for( ; ; ) 
  	{
    result =  SendHexHeader( ZRINIT, (unsigned char *)"\0\0\0\x27" );
		if (result!=FT_SUCCESS)
			break;
		   
    frame_type = ReadFrameHeader( TRUE );
    switch( frame_type ) 
    	{
      case ZFIN:
        {
				for (int i=0; i<4; i++)
				  SendHexHeader(ZFIN, (unsigned char *)"\0\0\0\0" );
        ReadChar(2000);  /* Read the "OO" */
        ReadChar(500);
        return( FALSE );
        }

      case ZSINIT:
        result = ReadBinaryPacket();
        if( result >= FT_SUCCESS) 
        	{
          SendHexHeader( ZACK, (unsigned char *)"\0\0\0\0" );
          break;
        	}
       	if (++lErrorCount>=ZMODEM_MAX_ERRORS)
					return (FALSE);
				break;

      case ZFILE:
				nbytes = 0 ;
				block_size = 0 ;
				block_number = 0 ;
				lErrorCount = 0 ;
        result = ReadBinaryPacket();
        if( result >= FT_ERROR)
          return( TRUE );
        break;

      default :
       	if (++lErrorCount>=ZMODEM_MAX_ERRORS)
					return (FALSE) ;
        break;
    }
  }
  return( FALSE );
}

BOOL ZMODEMCLASS::SendFileOffset( int frame_type )
{
  unsigned char header[ 4 ];
	BOOL rVal =FALSE ;

  if (!CheckAbort())
		{
    header[ 3 ] = (unsigned char)((nbytes >> 24) & 0xff);
  	header[ 2 ] = (unsigned char)((nbytes >> 16) & 0xff);
  	header[ 1 ] = (unsigned char)((nbytes >>  8) & 0xff);
  	header[ 0 ] = (unsigned char)((nbytes      ) & 0xff);
  	SendHexHeader( (char)frame_type, header );
		rVal = TRUE ;
		}
	return(rVal);
	
}

//*
//*  long ZMODEMCLASS::ConvertHeaderToOffset( void )
//*
long ZMODEMCLASS::ConvertHeaderToOffset( void )
{
  return( ((unsigned long)header[ 3 ] << 24) +
          ((unsigned long)header[ 2 ] << 16) +
          ((unsigned long)header[ 1 ] << 8 ) +
          ((unsigned long)header[ 0 ]      ) );
}

void ZMODEMCLASS::ReadFileData( void )
{
  int result = 0; 
  int fwrite_return;
  int frame_type;
  long rx_pos;

  if (SendFileOffset( ZRPOS))
   	{
    for( ; ; ) 
    	{
    	if (CheckAbort() )
      	return;
    	frame_type = ReadFrameHeader( TRUE );
    	if(frame_type>=FT_ERROR)
    		{
        if( frame_type == ZDATA ) 
        	{
      		rx_pos = ConvertHeaderToOffset();
      		if( rx_pos != (long)nbytes ) 
      			{
        		SendFileOffset( ZRPOS );
        		continue;
      			}
		      _llseek (hFile, nbytes, SEEK_SET) ;
    		  do 
		      	{
        		if (CheckAbort())
          		return;
        		result = ReadBinaryPacket();
        		if( result >= FT_SUCCESS) 
        			{
							block_number++;     //can be local parameter
          	 	fwrite_return = WriteFileBlock( block_size );
		          if( block_size>=0&&fwrite_return==HFILE_ERROR ) 
							 	return;
         			nbytes += block_size;
							UpdateStateWindow (lErrorCount, block_number, nbytes, NULL) ;
          		switch( result ) 
          			{
            		case 'j':
            		case 'k':
              		SendFileOffset( ZACK );
              		break;
          			}
        			}
     			 	} while( result == 'i' || result == 'j' );
    			}
    		else if( frame_type == ZEOF ) 
    			{
					if(nbytes==ConvertHeaderToOffset()) 
						return ;
					}
    		}
    else 
    	result = FT_ERROR;
    if( result < FT_SUCCESS) 
    	{
			if (++lErrorCount>=ZMODEM_MAX_ERRORS)
				return;
      SendFileOffset( ZRPOS );
      }
  	}
	}
}
int ZMODEMCLASS::WriteFileBlock( int buffer_size )
{
	return(_lwrite(hFile, lpBuffer, buffer_size));
 }
int ZMODEMCLASS::OpenReceiveFile (char *fname)
{
	OFSTRUCT of ;	
	int rVal = TRUE ;

	hFile = OpenFile (fname, &of, OF_CREATE|OF_WRITE) ;
	if(hFile==HFILE_ERROR)
		rVal = FALSE ;

	return (rVal);
}
void ZMODEMCLASS::_ReceiveFile( void )
{
  char name[ 144 ];
	char *fname ;

  while(GetFileName()) 
  	{
    strcpy( name, (char *)lpBuffer );
    fname = StripPathComponent((BYTE *)name );
    if( sscanf( lpBuffer + strlen(lpBuffer) + 1,"%ld %lo",&filesize,&file_time ) < 2 )
            file_time = -1L;
    if(!OpenReceiveFile(fname))
			break;

		if (UpdateStateWindow(0,0,0,fname)!=FT_SUCCESS)
    	break ;

    ReadFileData();
		_lclose (hFile) ;
		}
	CloseStateWindow() ;
}

