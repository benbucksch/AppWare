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
//* ZMODEMC.CPP
//*
//* Contains:
//*      protected:
//*         ZMODEMCLASS::ReadFrameHeader()
//*         ZMODEMCLASS::GetPad()
//*         ZMODEMCLASS::GetHeaderType()
//*         ZMODEMCLASS::ReadHexFrameHeaderData()
//*         ZMODEMCLASS::ReadBinaryHeaderData16()
//*         ZMODEMCLASS::ReadBinaryHeaderData32()
//*         ZMODEMCLASS::ReadHexByte()
//*         ZMODEMCLASS::GetCookedChar()
//*         ZMODEMCLASS::SendBinaryHeader()
//*         ZMODEMCLASS::SendHexHeader()
//*         ZMODEMCLASS::SendBinaryPacket()
//*         ZMODEMCLASS::EncodeCharacter()
//*         ZMODEMCLASS::SendEncodedCharacter()
//*         ZMODEMCLASS::ReadBinaryByte()
//*         ZMODEMCLASS::ReadCrc16()
//*         ZMODEMCLASS::ReadCrc32()
//*         ZMODEMCLASS::ReadBinaryPacket()
//*
//*
//* int ZMODEMCLASS::ReadFrameHeader( int immediate_return )
//*
int ZMODEMCLASS::ReadFrameHeader( int immediate_return )
{
  int  frame_type;
	int  getten_char ;
	int  garbage_count = 0 ;
	int  cancel_count = 0;
	
  for(;;) 
  	{
		getten_char = GetPad(garbage_count, cancel_count) ;
    if (getten_char!=ZPAD ) 
    	{
      header_type = FT_TIMEOUT;
      if (getten_char==FT_ERROR)
        return (getten_char);
    	}
    else
      header_type = GetHeaderType(garbage_count);
    switch( header_type ) 
    	{
      case 'B':
        frame_type = ReadHexFrameHeaderData();
        break;
      case 'A':																							 
        //check = 3;
        frame_type = ReadBinaryHeaderData16();
        break;
      case 'C':
				//check = 4 ;
        frame_type = ReadBinaryHeaderData32();
        break;

			case FT_TIMEOUT:
				frame_type = FT_TIMEOUT ;
				break;

			case ZCAN:
				continue;

      default:
        if (garbage_count >= 2048) 
        	frame_type = FT_GARBAGECOUNT;
				else
					{
    			cancel_count = 0 ;
				  frame_type = FT_SUCCESS;
					continue;
					}
        break;
		 	}
		break;
		}
	return(frame_type);
}
int ZMODEMCLASS::GetPad (int& garbage_count, int& cancel_count)
{
	int  timeout_timer = 0 ;
	BOOL loop = TRUE ;										  
	int  rVal ;
	int  c ;

	while(loop)
		{
		switch (c=ReadChar(1000L))
		  {
			case ZPAD:
			case ZPAD|0x80:
				loop = FALSE ;
				rVal = c;
				break;

			case FT_TIMEOUT:
				if (++timeout_timer>=10)
					{
					rVal = FT_TIMEOUT ;
					loop = FALSE ;
					}
				break;
			case CAN:
				timeout_timer = 0 ;
				if (cancel_count>=5)
					{
					loop = FALSE ;
					rVal = ZCAN ;
					}
				else
					{
					switch(c=ReadChar(120)) 
          	{
					  case CAN:
              if (++cancel_count>=5)
								{
								loop = FALSE ;
								rVal = ZCAN ;
								}
              break; 
						case ZCRCW:
							switch (ReadChar(100))
							  {
								case FT_TIMEOUT:
									loop = FALSE ;
									rVal = FT_ERROR ;
									break;

								default:
									if (++garbage_count>=2048)
										{
										loop = FALSE ;
										rVal = FT_GARBAGECOUNT ;
										}
									break;
							  }
							break;

						case FT_TIMEOUT:
						default :
							if (c!=FT_TIMEOUT)
								cancel_count= 0 ;
              break;
          	}
					}
				break;

			default:
				if (++garbage_count>=2048)
					{
					loop = FALSE ;
					rVal = FT_GARBAGECOUNT ;
					}
				break;
			}
		}
	return(rVal);
}
int ZMODEMCLASS::GetHeaderType(int& garbage_count)
{
  int  c;
	BOOL loop = TRUE ;

  while(loop) 
  	{
    switch (c=GetCookedChar())
			{
			case ZPAD:
				continue;

      case ZDLE:
				loop = FALSE ;
      	break;

		  case FT_TIMEOUT:
				return(FT_TIMEOUT);
				
			default:
				garbage_count ++ ;
				return(0);
			}
		}
  c = GetCookedChar();
  return( c );
}

//*
//* int ZMODEMCLASS::ReadHexFrameHeaderData( void )
//*
int ZMODEMCLASS::ReadHexFrameHeaderData( void )
{
  int c;
  int frame_type;
  unsigned int crc16;
  int i;

  frame_type = ReadHexByte();
  if( frame_type < 0 )
    return( frame_type );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&frame_type, 1, 0L);
  for( i = 0 ; i < 4 ; i++ ) 
  	{
    c = ReadHexByte();
    if( c < 0 )
      return( c );
    header[ i ] = (unsigned char) c;
    crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16 );
  	}
  c = ReadHexByte();
  if( c < 0 )
    return( c );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16 );
  c = ReadHexByte();
  if( c < 0 )
    return( c );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16);
  if( crc16 != 0 ) 
    return( FT_ERROR);

	for (i=0; i<5; i++)
	  {
	  c=ReadChar(50) ;
		c&=0x7f ;
	  if (c==LF)
			return(frame_type);
		if (c!=CR&&c!=XON)
			return(FT_ERROR);
		}
   return( frame_type );
}


//*
//* int ZMODEMCLASS::ReadBinaryHeaderData16( void )
//*
int ZMODEMCLASS::ReadBinaryHeaderData16( void )
{
  int c;
  int frame_type;
  unsigned int crc16;
  int i;

  frame_type = ReadBinaryByte();
  if( frame_type < 0 )
    return( frame_type );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&frame_type, 1, 0L );
  for( i = 0 ; i < 4 ; i++ ) 
  	{
    c = ReadBinaryByte();
    if (c < 0)
      return( c );
    header[ i ] = (unsigned char) c;
    crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16 );
  	}
  c = ReadBinaryByte();
  if (c < 0)
    return( c );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16);
  c = ReadBinaryByte();
  if (c < 0)
    return( c );
  crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&c, 1, (unsigned long)crc16 );
  if (crc16 != 0)
    return( frame_type );
}
//*
//* int ZMODEMCLASS::ReadBinaryHeaderData32( void )
//*
int ZMODEMCLASS::ReadBinaryHeaderData32( void )
{
  int c;
  int frame_type;
  unsigned long crc32;
  int i;

  frame_type = ReadBinaryByte();
  if( frame_type < 0 )
    return( frame_type );
  crc32 = Crc32->CalcCRC32((unsigned char*)&frame_type, 1, 0xFFFFFFFFL);
  for( i = 0 ; i < 4 ; i++ ) 
  	{
    c = ReadBinaryByte();
    if( c < 0 )
      return( c );
    header[ i ] = (unsigned char) c;
    crc32 = Crc32->CalcCRC32((unsigned char*)&c, 1, (unsigned long)crc32);
  	}
  c = ReadBinaryByte();
  if( c < 0 )				
    return( c );
  crc32 = Crc32->CalcCRC32((unsigned char*)&c, 1, (unsigned long)crc32);
  c = ReadBinaryByte();
  if( c < 0 )
    return( c );
  crc32 = Crc32->CalcCRC32((unsigned char*)&c, 1, (unsigned long)crc32);
  c = ReadBinaryByte();
  if( c < 0 )
    return( c );
  crc32 = Crc32->CalcCRC32((unsigned char*)&c, 1, (unsigned long)crc32);
  c = ReadBinaryByte();
  if( c < 0 )
    return( c );
  crc32 = Crc32->CalcCRC32((unsigned char*)&c, 1, (unsigned long)crc32);
  if( crc32 != 0xDEBB20E3L ) 
  	return(FT_ERROR);
  return( frame_type );
}
//*
//* int ZMODEMCLASS::ReadHexByte( void )
//*
int ZMODEMCLASS::ReadHexByte( void )
{
  int c;
  int value;

  c = GetCookedChar();
  if( c >= '0' && c <= '9' )
    value = c - '0';
  else if( c >= 'a' && c <= 'f' )
    value = c -'a' + 10;
  else
    return(FT_ERROR);
  value <<= 4;
  c = GetCookedChar();
  if( c >= '0' && c <= '9' )
    return( value + c - '0' );
  else if ( c >= 'a' && c <= 'f' )
    return( value + c -'a' + 10 );
  return(FT_ERROR);
}
//*
//* int ZMODEMCLASS::GetCookedChar( void )
//*
int ZMODEMCLASS::GetCookedChar( void )
{
  int c;

  for( ; ; ) {
    if (CheckAbort())
    	return(FT_ERROR);
    c = ReadChar(2500);
    if( c < 0 )
      return( c );
    c &= 0x7f;
    if( c != XOFF && c != XON )
      return( c );
  }
}
//*
//* int ZMODEMCLASS::SendBinaryHeader( char frame_type, char *header_data )
//*
int ZMODEMCLASS::SendBinaryHeader( char frame_type, char *header_data )
{
  unsigned int crc16;
  unsigned long crc32;
  char buffer[24];
  int length = 4;
  int i;

  buffer[ 0 ] = ZPAD;
  buffer[ 1 ] = ZDLE;
  if( rx_flags & 0x20 ) 
  	{
    buffer[ 2 ] = ZBIN32;
		crc32 = Crc32->CalcCRC32( (unsigned char*)&frame_type, 1, 0xFFFFFFFFL );
    crc32 = Crc32->CalcCRC32( (BYTE *)header_data, length, crc32 );
    crc32 = ~crc32;
    }
  else 
  	{
    buffer[ 2 ] = ZBIN16;
		crc16 = (unsigned int)Crc16->CalcCRC16((unsigned char*)&frame_type, 1, 0L );
    crc16 = (unsigned int)Crc16->CalcCRC16((BYTE *)header_data, length,	crc16 );
    }
  i = 3;
  i += EncodeCharacter( buffer + i, frame_type );
  while( length-- > 0 )
    i += EncodeCharacter( buffer + i, *header_data++ );
  if( rx_flags & 0x20 ) {
    i += EncodeCharacter( buffer + i, (unsigned char)( crc32 & 0xff ) );
    i += EncodeCharacter( buffer + i, (unsigned char)( crc32 >>  8 ) );
    i += EncodeCharacter( buffer + i, (unsigned char)( crc32 >> 16 ) );
    i += EncodeCharacter( buffer + i, (unsigned char)( crc32 >> 24 ) );
  }
  else {
    i += EncodeCharacter( buffer + i, (unsigned char)( crc16 >> 8 ) );
    i += EncodeCharacter( buffer + i, (unsigned char)( crc16 & 0xff ) );
  }
  return (WriteToBuffer(buffer, i));
}
//*
//* int ZMODEMCLASS::SendHexHeader( char frame_type, unsigned char *header_data )
//*
int ZMODEMCLASS::SendHexHeader( char frame_type, unsigned char *header_data )
{
  unsigned int crc;
  char *buf;
  char buffer[22];
  int length = 4;

	crc = (unsigned int)Crc16->CalcCRC16 ((unsigned char*)&frame_type, 1, 0L) ;
	crc = (unsigned int)Crc16->CalcCRC16 ((unsigned char*)header_data, length, crc) ;
  
  buffer[ 0 ] = ZPAD;
  buffer[ 1 ] = ZPAD;
  buffer[ 2 ] = ZDLE;
  buffer[ 3 ] = ZHEX;
	wsprintf (buffer+4, "%02x", frame_type) ;
  buf = buffer + 6;

  while( length-- > 0 ) {
		wsprintf (buf, "%02x", *header_data++) ;
    buf += 2;
  }
	wsprintf (buf, "%04x\r\n", crc) ;
  buf += 6;
  if( frame_type != ZFIN && frame_type != ZACK ) 
  	{
		wsprintf (buf, "%c", XON) ;
    buf += 1;
  	}
  *buf = '\0';
  return( WriteToBuffer(buffer, (int)(buf - buffer)));
}
//*
//* int ZMODEMCLASS::SendBinaryPacket( int length, char terminator )
//*
int ZMODEMCLASS::SendBinaryPacket( int length, char terminator )
{
  unsigned int crc16;
  unsigned long crc32;
  int stat;
  int i;

  if( rx_flags & 0x20 ) 
  	{
		crc32 = Crc32->CalcCRC32( (BYTE*)lpBuffer, length, 0xFFFFFFFFL );
    crc32 = Crc32->CalcCRC32( (BYTE*)&terminator, 1,(unsigned long)crc32 );
    crc32 = ~crc32;
    }
  else 
  	{
		crc16 = (unsigned int)Crc16->CalcCRC16((BYTE*)lpBuffer, length, 0 );
    crc16 = (unsigned int)Crc16->CalcCRC16((BYTE*)&terminator,1, (unsigned long)crc16 );
    }

  for( i = 0 ; i < length ; i++ ) 
  	{
    stat = SendEncodedCharacter( lpBuffer[ i ] );
    if ( stat!=FT_SUCCESS ) 
    	return( stat );
    }
  stat = WritePort(ZDLE);
  if (stat!=FT_SUCCESS) 
  	return( stat );
  stat = SendEncodedCharacter( terminator );
  if (stat !=FT_SUCCESS) 
  	return( stat );
  if( rx_flags & 0x20 ) 
  	{
    for( i = 0 ; i < 4 ; i++ ) 
    	{
      stat = SendEncodedCharacter( (unsigned char)(crc32 & 0xff) );
      crc32 >>= 8;
      if (stat!=FT_SUCCESS) 
      	return( stat );
     	}
  	}
  else 
  	{
    for( i = 0 ; i < 2 ; i++ ) 
    	{
      stat = SendEncodedCharacter( (unsigned char) (( crc16 >> 8 ) & 0xff) );
      crc16 <<= 8;
      if (stat!=FT_SUCCESS) 
      	return( stat );
      }	
  	}
  if (terminator == ZCRCW )
    return( WritePort(XON));
  else                                      
    return(FT_SUCCESS);
}
//*
//* int ZMODEMCLASS::EncodeCharacter( char *buf, unsigned char c )
//*
int ZMODEMCLASS::EncodeCharacter( char *buf, unsigned char c )
{
  switch ( c ) 
  	{
    case ZDLE        :
    case CR          :
    case CR | 0x80   :
    case XON         :
    case XON | 0x80  :
    case XOFF        :
    case XOFF | 0x80 :
    case DLE         :
    case DLE | 0x80  :
      *buf++ = ZDLE;
      *buf = (unsigned char) ( c ^ 0x40 );
      return( 2 );
    default :
      *buf = c;
      return( 1 );
  	}
}
//*
//* int ZMODEMCLASS::SendEncodedCharacter( char c )
//*
int ZMODEMCLASS::SendEncodedCharacter( char c )
  {
  int stat;

	switch ( c & 0xff ) 
  	{
    case ZDLE        :
    case CR          :
    case CR | 0x80   :
    case XON         :
    case XON | 0x80  :
    case XOFF        :
    case XOFF | 0x80 :
    case DLE         :
    case DLE | 0x80  :

   	if((stat = WritePort(ZDLE)) !=FT_SUCCESS)
       return( stat );
   	return(WritePort((BYTE)(c ^ 0x40)));
   	default :
  		return(WritePort((BYTE)c));
		}
}
//*
//* int ZMODEMCLASS::ReadBinaryByte( void )
//*
int ZMODEMCLASS::ReadBinaryByte( void )
{
  int c;
	int cancel_count;

  cancel_count = 0;
  if (CheckAbort() )
    return (FT_ERROR);
  c = ReadChar(2500 );
  if( c < 0 )
    return( c );
  if( c == ZDLE ) 
  	{
    cancel_count++;
    for( ; ; ) {
      if (CheckAbort())
        return(FT_ERROR);
      c = ReadChar( 2500 );
      if( c < 0 )
        return( c );
      switch( c ) 
      	{
        case ZDLE :
          cancel_count++;
          break;
        case 'h' :
        case 'i' :
        case 'j' :
        case 'k' :
          return( c + 0x100 );
        case 'l' :
          return( 0x7f );
        case 'm' :
          return( 0xff );
        default :
          return( c ^ 0x40 );
      	}
      if( cancel_count >= 5 ) 
				return(FT_ERROR);
		}
	 }
  else
    return( c );
}
//*
//* int ZMODEMCLASS::ReadCrc16( int terminator )
//*
int ZMODEMCLASS::ReadCrc16( int terminator )
{
  int crc1;
  int crc2;
  unsigned int crc16;

  crc1 = ReadBinaryByte();
  if(crc1 < FT_SUCCESS)
    return( crc1 );
  crc2 = ReadBinaryByte();
  crc16 = (unsigned int)Crc16->CalcCRC16((BYTE*)lpBuffer, block_size,0);
  crc16 = (unsigned int)Crc16->CalcCRC16((BYTE*)&terminator, 1, (unsigned long)crc16);
  if( (unsigned)crc1 != ( ( crc16 >> 8 ) & 0xff ) ||
      (unsigned)crc2 != ( crc16 & 0xff ) )
    return(FT_ERROR);
  return(FT_SUCCESS);
}
//*
//* int ZMODEMCLASS::ReadCrc32( int terminator )
//*
int ZMODEMCLASS::ReadCrc32( int terminator )
{
  int crc1;
  int crc2;
  int crc3;
  int crc4;
  unsigned long crc32;

  crc32 = Crc32->CalcCRC32((BYTE*)lpBuffer, block_size, 0xFFFFFFFFL );
  crc32 = Crc32->CalcCRC32((BYTE*)&terminator, 1, (unsigned long)crc32 );
  crc1 = ReadBinaryByte();
  if( crc1 < FT_SUCCESS)
    return( crc1 );
  crc2 = ReadBinaryByte();
  if(crc2 < FT_SUCCESS)
    return( crc2 );
  crc3 = ReadBinaryByte();
  if(crc3 < FT_SUCCESS)
    return( crc3 );
  crc4 = ReadBinaryByte();
  crc32 = Crc32->CalcCRC32((unsigned char*)&crc1, 1, (unsigned long)crc32);
  crc32 = Crc32->CalcCRC32((unsigned char*)&crc2, 1, (unsigned long)crc32);
  crc32 = Crc32->CalcCRC32((unsigned char*)&crc3, 1, (unsigned long)crc32);
  crc32 = Crc32->CalcCRC32((unsigned char*)&crc4, 1, (unsigned long)crc32);
  if (crc32 != 0xDEBB20E3L)
    return(FT_SUCCESS);
  return(FT_SUCCESS);
}
//*
//* int ZMODEMCLASS::ReadBinaryPacket( void )
//*
int ZMODEMCLASS::ReadBinaryPacket( void )
{
  int c;
  int result;

  block_size = 0;
  for( ; ; ) 
  	{
    c = ReadBinaryByte();
    if( c < 0 )
      return( c );
    if( c < 0x100 ) 
    	{
      if( block_size >= 1024 )
        return( FT_ERROR) ; 
      lpBuffer[ block_size++ ] = (char) c;
    	}
    else 
    	{
      switch( c & 0xff ) 
      	{
        case 'h' :
        case 'i' :
        case 'j' :
        case 'k' :
          if( header_type == 'C' )
            result = ReadCrc32( c );
          else
            result = ReadCrc16( c );
          if( result < 0 )
            return( result );
          else
            return( c & 0xff );
        default :
          return(FT_ERROR);
      	}
    	}
  	}
}
int ZMODEMCLASS::ReadFileBlock( int buffer_size )
{
  return( ReadFile(hFile, lpBuffer, buffer_size)) ;
}
