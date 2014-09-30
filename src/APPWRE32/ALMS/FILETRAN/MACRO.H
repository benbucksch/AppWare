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
#define connIdle            AEVENT_MAKEID ('c','t','I','d')
//
//Define File Transfer Event IDs
//
#define cancelFileTransfer  AEVENT_MAKEID ('c','l','F','t')
#define pauseFileTransfer   AEVENT_MAKEID ('p','s','F','t')
#define resumeFileTransfer  AEVENT_MAKEID ('r','s','F','t')
#define pauseFile           AEVENT_MAKEID ('p','s','F','l')
#define killIdle            AEVENT_MAKEID ('k','l','I','d')
#define activeIdle          AEVENT_MAKEID ('a','t','I','d')
#define sendFile            AEVENT_MAKEID ('s','d','F','l') 
#define sendTextFile        AEVENT_MAKEID ('s','d','T','f')
#define receiveTextFile     AEVENT_MAKEID ('r','v','T','f')
#define waitXon             AEVENT_MAKEID ('w','t','O','n')
#define startEvent          AEVENT_MAKEID ('s','t','E','t')
#define sendXoff            AEVENT_MAKEID ('s','d','O','f')
#define readBytes           AEVENT_MAKEID ('r','d','B','t')
#define writeBytes          AEVENT_MAKEID ('w','t','B','t')
#define endTextFile         AEVENT_MAKEID ('e','d','T','f')
#define sendBinaryFile      AEVENT_MAKEID ('s','d','B','f')
#define startSendBinaryFile AEVENT_MAKEID ('s','t','S','b')
#define waitFirstChar       AEVENT_MAKEID ('w','t','F','c')
#define writeDataBlock      AEVENT_MAKEID ('w','t','D','b')
#define writeHeaderBlock    AEVENT_MAKEID ('w','t','H','b')
#define waitACK		          AEVENT_MAKEID ('w','t','A','c')
#define waitFinalACK        AEVENT_MAKEID ('w','t','f','A')
#define endSendBinaryFile   AEVENT_MAKEID ('e','d','B','f')
#define receiveBinaryFile   AEVENT_MAKEID ('r','v','B','f')
#define startReceiveBinaryFile AEVENT_MAKEID ('s','t','R','b')
#define initReceive         AEVENT_MAKEID ('i','t','R','v')
#define sendInitChar        AEVENT_MAKEID ('s','d','I','c')
#define waitSOH             AEVENT_MAKEID ('w','t','S','c')
#define checkSEQchar        AEVENT_MAKEID ('c','S','E','Q')
#define receiveDataBlock    AEVENT_MAKEID ('r','v','D','b')
#define checkDataBlock      AEVENT_MAKEID ('c','k','D','b')

#define kermitSendPacket       AEVENT_MAKEID ('k','t','S','p')
#define kermitWaitACK          AEVENT_MAKEID ('k','w','t','A')
#define kermitResendPacket     AEVENT_MAKEID ('k','t','R','p')
#define kermitSendInitACK      AEVENT_MAKEID ('k','s','I','a')
#define kermitGetInitPacket    AEVENT_MAKEID ('k','g','I','p')
#define kermitGetFPacket       AEVENT_MAKEID ('k','g','F','p')
#define kermitGetPacket        AEVENT_MAKEID ('k','g','D','p')
#define kermitResendInitACK    AEVENT_MAKEID ('k','r','I','a')
#define kermitSendACK          AEVENT_MAKEID ('k','s','D','a')
#define kermitResendACK        AEVENT_MAKEID ('k','r','D','a')
#define kermitResendNAK        AEVENT_MAKEID ('k','r','D','n')
#define kermitSendFinalACK     AEVENT_MAKEID ('k','s','F','a')
#define kermitSendNAK          AEVENT_MAKEID ('k','s','d','N')
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

#define setPollTimer  AEVENT_MAKEID ('s','t','P','t')
#define setDataTimer  AEVENT_MAKEID ('s','t','D','t')
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

#define MAKEDWORD(dwLow, dwHight)  (DWORD)((((dwHight) << 8) & 0xFF000000) | (dwLow))
