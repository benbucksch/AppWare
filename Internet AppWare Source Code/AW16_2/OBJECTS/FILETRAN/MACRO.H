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
// FILE:"macro.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
//
//Basic Dialog box parameters defination
//
#ifdef CONST
char *cstProtocol[]={"Ascii", "XMODEM", "YMODEM", "KERMIT", "ZMODEM"} ;
char *cstXmodem [] = {"XMODEM/CRC", "XMODEM 1K", "XMODEM 1KG"} ;
char *cstYmodem [] = {"standard", "YMODEM G"} ;
char *cstLineEnd[] = {"CR","LF","CR&LF"} ;
#endif
#ifndef CONST
extern char *cstProtocol[] ;
extern char *cstXmodem[] ;
extern char *cstYmodem[] ;
extern char *cstLineEnd[] ;
#endif
// Define Current Version 
#define CURRENTVERSION   0
//
//
//
#define XONXOFF  1
#define XON  0x11      //XON char
#define XOFF 0x13      //XOFF char
#define LF   0x0A 		 //LF 
#define CR   0x0D      //CR
#define EOT  0x04      //EOT
//
#define XOFFHOLD 101
//
//
//
#define connIdle            MAKEEVENTID ('c','t','I','d')
//
//Define File Transfer Event IDs
//
#define cancelFileTransfer  MAKEEVENTID ('c','l','F','t')
#define pauseFileTransfer   MAKEEVENTID ('p','s','F','t')
#define resumeFileTransfer  MAKEEVENTID ('r','s','F','t')
#define pauseFile           MAKEEVENTID ('p','s','F','l')
#define killIdle            MAKEEVENTID ('k','l','I','d')
#define activeIdle          MAKEEVENTID ('a','t','I','d')
#define sendFile            MAKEEVENTID ('s','d','F','l') 
#define sendTextFile        MAKEEVENTID ('s','d','T','f')
#define receiveTextFile     MAKEEVENTID ('r','v','T','f')
#define waitXon             MAKEEVENTID ('w','t','O','n')
#define startEvent          MAKEEVENTID ('s','t','E','t')
#define sendXoff            MAKEEVENTID ('s','d','O','f')
#define readBytes           MAKEEVENTID ('r','d','B','t')
#define writeBytes          MAKEEVENTID ('w','t','B','t')
#define endTextFile         MAKEEVENTID ('e','d','T','f')
#define sendBinaryFile      MAKEEVENTID ('s','d','B','f')
#define startSendBinaryFile MAKEEVENTID ('s','t','S','b')
#define waitFirstChar       MAKEEVENTID ('w','t','F','c')
#define writeDataBlock      MAKEEVENTID ('w','t','D','b')
#define writeHeaderBlock    MAKEEVENTID ('w','t','H','b')
#define waitACK		          MAKEEVENTID ('w','t','A','c')
#define waitFinalACK        MAKEEVENTID ('w','t','f','A')
#define endSendBinaryFile   MAKEEVENTID ('e','d','B','f')
#define receiveBinaryFile   MAKEEVENTID ('r','v','B','f')
#define startReceiveBinaryFile MAKEEVENTID ('s','t','R','b')
#define initReceive         MAKEEVENTID ('i','t','R','v')
#define sendInitChar        MAKEEVENTID ('s','d','I','c')
#define waitSOH             MAKEEVENTID ('w','t','S','c')
#define checkSEQchar        MAKEEVENTID ('c','S','E','Q')
#define receiveDataBlock    MAKEEVENTID ('r','v','D','b')
#define checkDataBlock      MAKEEVENTID ('c','k','D','b')

#define kermitSendPacket       MAKEEVENTID ('k','t','S','p')
#define kermitWaitACK          MAKEEVENTID ('k','w','t','A')
#define kermitResendPacket     MAKEEVENTID ('k','t','R','p')
#define kermitSendInitACK      MAKEEVENTID ('k','s','I','a')
#define kermitGetInitPacket    MAKEEVENTID ('k','g','I','p')
#define kermitGetFPacket       MAKEEVENTID ('k','g','F','p')
#define kermitGetPacket        MAKEEVENTID ('k','g','D','p')
#define kermitResendInitACK    MAKEEVENTID ('k','r','I','a')
#define kermitSendACK          MAKEEVENTID ('k','s','D','a')
#define kermitResendACK        MAKEEVENTID ('k','r','D','a')
#define kermitResendNAK        MAKEEVENTID ('k','r','D','n')
#define kermitSendFinalACK     MAKEEVENTID ('k','s','F','a')
#define kermitSendNAK          MAKEEVENTID ('k','s','d','N')
//
//FIle Transfer Data block
//
#define FTNSDATA    0
#define ASCIIFILE   1
#define PATHNAME    2

#define BUFFERSIZE      2048
#define BLOCKSIZE       128
#define BYTESLEEP       50
#define XMODEM_MAX_ERRORS 6
//
//File Transfer return value
//
#define FT_SUCCESS      0
#define FT_OK           1
#define FT_ERROR        -1
#define FT_ALLFILEEND   -800
#define FT_FILEEND      -801
#define FT_OPENERROR    -802
#define FT_CLOSEERROR   -803 
#define FT_TIMEOUT      -804
#define FT_LOCALCANCEL  -805
#define FT_REMOTECANCEL -806
#define FT_XOFF         -807
#define FT_XON          -808
#define FT_CONTINUE     -809
#define FT_CTS          -810
#define FT_DSR          -811
#define FT_RLSD         -812
#define FT_GARBAGECOUNT  -813

//
#define TEXTFILE    1
#define CHARDELAY   12
#define LINEDELAY   13
#define BINARYFILE  2

#define ASCII       0
#define XMODEM      1
#define YMODEM      2
#define KERMIT      3
#define ZMODEM      4

//XMODEM protocal
#define SOH      0x01
#define STX      0x02
#define EOT      0x04
#define ACK      0x06
#define NAK      0x15
#define CAN      0x18
#define INIT_C   800
#define INIT_G   801
#define SOH_OFFSET  0
#define SEQ_OFFSET  1
#define CSEQ_OFFSET 2
#define DATA_OFFSET 3
#define CHK_OFFSET  131

#define KERMIT_DATA_OFFSET            4
#define KERMIT_MY_MAX_SIZE            94  //* My maximum kermit buffer size.
#define KERMIT_MY_PACKET_TIMEOUT      10  //* Buffer receive timeout in seconds
#define KERMIT_MY_EOL                 '\r'//* The EOL character I want.
#define KERMIT_MY_PAD_COUNT           0   //* My incoming pad character count.
#define KERMIT_MY_PAD_CHAR            0   //* My incoming pad characer.
#define KERMIT_MY_QUOTE_CHAR          '#' //* My quote character.
#define KERMIT_MY_QBIN_CHAR           '&' //* My default QBIN quoting character
#define KERMIT_MY_QBIN_DEFAULT        'Y' //* My default QBIN receive state
#define KERMIT_MY_MARK_CHARACTER      1   //* My mark character, normally ^A
#define KERMIT_MY_REPEAT_CHARACTER    '~' //* My repeat char
#define KERMIT_MAX_ERRORS             10  //* Maximum number of errors in Kermit

#define tochar( x )   ( ( x ) + 32 )
#define unchar( x )   ( ( x ) - 32 )
#define ctl( x )      ( ( x ) ^ (unsigned char) 64 )

//////////////////////////////////////////////////////////////////////////
#define SENDFILE    1
#define RECEIVEFILE 2
#define POLLTIMER   1
#define DATATIMER   2
#define STOTIMER    3

#define setPollTimer  MAKEEVENTID ('s','t','P','t')
#define setDataTimer  MAKEEVENTID ('s','t','D','t')
#define YMODEM_INIT    0
#define XMODEM_INIT    1
#define SEND_INIT      2
#define WAIT_ACK       3
#define WAIT_FINAL_ACK 4
#define WAIT_SOH       5
#define WAIT_SEQ       6
#define WAIT_DATA      7
#define WAIT_CHECK     8

#define OBJECTNAMESIZE 32