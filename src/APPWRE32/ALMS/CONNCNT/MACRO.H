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
//Connection Object Port settings
char * cstBaudrate[]={"110","300","600","1200","2400","4800","9600","19200"} ;
char * cstDatabits[]={"5", "6", "7", "8"} ;
char * cstStopbits[]={"1", "1.5", "2"} ;
char * cstParity[]={"(None)", "Odd", "Even", "Mark", "Space"} ;
char * csthandshake[]={"(None)", "Xon/Xoff", "Hardware"} ;
char * cstPorts[]={"COM1", "COM2", "COM3", "COM4"} ;
char * cstDialM[]={"Tone", "Pulse"} ;
char * cstModemConnctn[] = {"FALSE", "TRUE"} ;
#endif
#ifndef CONST
extern char *cstBaudrate[] ;
extern char *cstDatabits[] ;
extern char *cstStopbits[] ;
extern char *cstParity[] ;
extern char *csthandshake[] ;
extern char *cstPorts[] ;
extern char *cstDialM[] ;
extern char *cstModemT[] ;
extern char *cstResultCode[] ;
extern long cstBaudrate1[] ;
extern int  cstDatabits1[] ;
extern int  cstStopbits1[] ;
extern BYTE cstParity1[] ;
extern char * cstModemConnctn[] ;
#endif
//
//Define Activate and Deactivate symbal
//
#define ACTMARK  0xD7
#define DACTMARK 0x20

#define CURRENTVERSION   0x0100
//
//Define Connection object marco
//
//Connection Type ID
//
//#define OTYPE_CONN       ATYPE_MAKEID ('C','o','n','n')

//Event defination
#define commOpen           AEVENT_MAKEID ('c','m','O','p')
#define commClose          AEVENT_MAKEID ('c','m','C','l')
#define connIdle           AEVENT_MAKEID ('c','t','I','d')
#define sendScheduleText   AEVENT_MAKEID ('s','d','S','t')
#define changeConfigure    AEVENT_MAKEID ('c','g','C','g')
#define asynchronousListen AEVENT_MAKEID ('a','s','L','s')
//
//buffer size
//
#define INBUFSIZE  4096
#define OUTBUFSIZE 4096

#define MAXLEN  256
//
//Signal Dialog
//view mode 
#define SGNLASCII 0x01   //ascii char mode 
#define SGNLBYTE  0x02   //Byte mode   8-bits
#define SGNLWORD  0x04   //Word mode  16-bits
#define SGNLLONG  0x08   //long mode  32-bits
//Compare bit
#define EIGHT     0      //compare all eight bits
#define SEVENT    1      //compare lower aevent bits
//
#define FOUNDSTR        0x01   //set found text object
#define FOUNDPOS        0x02   //set found position
#define INTERCEPT       0x04   //set intercept text
//Flow Control
#define NONE     0
#define XONXOFF  1
#define HARDWARE 2

//Ports defination
#define PORT_1     1   //COM1
#define PORT_2     2   //COM2
#define PORT_3     3   //COM3
#define PORT_4     4   //COM4

//Dial Method
#define TONE       0   //tone dialing method
#define PULSE      1   //pulse dialing method

//Result Code
#define NOCARRIER 8
#define BUSY      9 

//Data block Index
#define NUMDATAS    5
#define CONNDATA    0  //Connection Object general data block
#define PHONEN      1  //phone number
#define SGNLINFO    2  //Signal Information
#define SGNLBLOCK   3  //String signal
#define RUNDATA     4  //run time data
//
//
//
#define XON  0x11      //XON char
#define XOFF 0x13      //XOFF char
#define LF   0x0A 		 //LF 
#define CR   0x0D      //CR
#define EOT  0x04      //EOT
#define COMM_RXROOM 100  //Room before buffer is full and send XOFF char
//
//Return error ID
//
#define COMM_OK          1
#define COMM_ERROR       0
#define COMM_HARDWARE    -901
#define COMM_MEMORY      -902
#define COMM_NOPEN       -903
#define COMM_OPEN        -904
#define COMM_BREAK_ON    -905
#define COMM_BREAK_OFF   -906
#define COMM_BREAK_ERROR -907
#define COMM_TIME_OUT    -907
#define COMM_EOF         -908
//
//
#define INVALID_PORT_HANDLE INVALID_HANDLE_VALUE
//
//
//
#define OBJECTNAMESIZE    32
#define COMM_EVENTMASK    EV_BREAK|EV_CTS|EV_DSR|EV_ERR|EV_RING|EV_RLSD|EV_RXCHAR|EV_RXFLAG|EV_TXEMPTY
//
#define idleTerminal  	 	AEVENT_MAKEID ('i','d','T','l')

#define format1 "Version %s Modem Connection TRUE Baudrate %s DataBits %s \
Parity %s StopBits %s Port %s Handshake %s Parity Check %s Carrier Detected %s \
Phonenumber %s Dial %s Retry %s RetryTimes %d RetryInterval %d WaitRings %d"
#define format2 "Version %d Modem Connection FALSE Baudrate %s DataBits %s \
Parity %s StopBits %s Port %s Handshake %s Parity Check %s Carrier Detected %s"
									
