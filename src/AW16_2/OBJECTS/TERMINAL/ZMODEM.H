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
// FILE:"zmodem.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:                                             
//
//////////////////////////////////////////////////////////////*/
#include <stdio.h>

#define ZMAXHLEN 4
#define ZMODEM_MAX_ERRORS 10

class ZMODEMCLASS : public FTCLASS
  {
  public:
    ZMODEMCLASS(OBJECTID, char *, BOOL) ;
   ~ZMODEMCLASS() ;
    virtual void StartProtocol (OBJECTID, int) ;     
    virtual int  Initiation(BOOL) ;  
           
  protected:

    BOOL SystemIdle (void) ;
    int  ReadChar (DWORD) ; 
    int  WritePort (char) ;
    int  WriteToBuffer (const char*, int) ;
		int  UpdateStateWindow (int, int, long, char*) ;
    void CloseStateWindow(void) ;
    BOOL CheckAbort (void) ;

    //*
    //* Receive file routines
    //*
    void _ReceiveFile( void );
    int  GetFileName( void );
    BOOL SendFileOffset( int frame_type );
    long ConvertHeaderToOffset( void );
    void ReadFileData( void );
    int  OpenReceiveFile(char*) ;

    //*
    //* Send file routines
    //*
    void _SendFile(void);
    int  OpenLink(void);
    int  OpenNextFile( void );
    int  CloseLink( void );
    int  ReadBlock(int&);
    int  BackChannelActivity(int&);
    void SendDataFrame(int& );
    int  StartFileTransfer( void );
    int  SendFileData( void );


    //*
    //* Common transfer routines
    //*
    int ReadFrameHeader( int immediate_return );
    int GetPad(int&, int&);
    int GetHeaderType(int&);
    int ReadHexFrameHeaderData( void );
    int ReadBinaryHeaderData16( void );
    int ReadBinaryHeaderData32( void );
    int ReadHexByte( void );
    int GetCookedChar( void );
    int SendBinaryHeader( char frame_type, char  *header_data );
    int SendHexHeader( char frame_type, unsigned char  *header_data );
    int SendBinaryPacket( int length, char terminator );
    int EncodeCharacter( char *buf, unsigned char c );
    int SendEncodedCharacter( char c );
    int ReadBinaryByte( void );
    int ReadCrc16( int terminator );
    int ReadCrc32( int terminator );
    int ReadBinaryPacket( void );
    int WriteFileBlock (int) ;
    int ReadFileBlock (int) ; 

    char*     filename ;
    int       filecount ;
    ChkCrc16* Crc16 ;
    ChkCrc32* Crc32 ;
    int rx_flags;
    int error_recovery;
    unsigned int rx_buffer_size;
    unsigned int rx_frame_output_count;
    unsigned char header[ 4 ];
    int header_type;
    int  crc_mode;
    long nbytes ;
    int  block_number ;
    int  block_size ;
    long file_time ;
  };
