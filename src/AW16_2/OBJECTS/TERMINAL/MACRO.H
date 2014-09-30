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
//Terminal Object Const
char *cstEmulation[]={"TTY(Generic)", "VT-100(ANSI)", "VT-52"} ;
char *cstTranslations[]={"(None)","Denmark/Norway","Finland","France",
											"French Canada", "Germany","Italy","Spain","Sweden","Switzerland","United Kingdom"} ;
#endif
#ifndef CONST
extern char *cstEmulation[] ;
extern char *cstTranslations[] ;
#endif

#define CURRENTVERSION   0
//
//Connection Type ID
//
//#define OTYPE_CONN       MAKETYPEID ('C','o','n','n')
//
//Define terminal type ID
//#define OTYPE_TERMINAL MAKETYPEID('T','e','r','m')
//
//Terminal Event IDs
#define changeConfigure  	MAKEEVENTID ('c','g','C','g')
#define idleTerminal  	 	MAKEEVENTID ('i','d','T','l')
#define clearTerminal 	 	MAKEEVENTID ('c','l','T','l')
#define receiveText   	 	MAKEEVENTID ('r','v','T','t')
#define resetTerminal 		MAKEEVENTID ('r','s','T','l')
#define startCapture  		MAKEEVENTID ('s','t','C','p')
#define endCapture    		MAKEEVENTID ('e','d','C','p')

//data block
#define TMNLDATA   0
//
//Terminal Emulation type
//
#define VT100     1
#define VT52      2
#define TTY       0
//
//Terminal Preferences
//
#define LINEWRAP  0x01  //Set line wrap
#define LOCALECHO 0x02  //Set Local echo
//#define SOUND     0x04
#define BLOCK     1     //Set block cursor
#define UNDERLINE 0     //Set underline cursor
#define INBOUND   0x01  //Set inbound CR/LF
#define OUTBOUND  0x02  //Set outbount CR/LF
#define Columns80    1  //Set 80 columns
#define Columns132   0  //Set 132 columns
#define VSCROLL  0x01   //Set Vertical Scroll Bar
#define HSCROLL  0x02   //Set Horizontal Scroll Bar
//
//
//
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define BUFFER(x,y) *(termBuffer + y*bMaxCol+x)
#define ATTRBUF(x,y) *(attrBuffer +y*bMaxCol+x)

#define CHAR_RECEIVE_STATE  1
#define ESCAPE_RCVD_STATE   2
#define PARAM_RECEIVE_STATE 3
#define MAX_PARAMS          20

#define XON  0x11      //XON char
#define XOFF 0x13      //XOFF char
#define LF   0x0A 		 //LF 
#define CR   0x0D      //CR
#define EOT  0x04      //EOT
#define COMM_RXROOM 100  //Room before buffer is full and send XOFF char

#define OBJECTNAMESIZE  256