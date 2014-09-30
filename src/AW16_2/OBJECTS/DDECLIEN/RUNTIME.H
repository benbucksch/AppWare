/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE: 				Runtime.H
//							
// AUTHOR:  		Shaili Jain
//
// DESCRIPTION:	The following header file defines runtime constants
//	 						and ALM DATAs layout
//
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/


#define DATAVERSION        				 	1
#define DLLVERSION         				 	1


// IDS for function calls
#define IDSTOPEXECUTE               1
#define IDSTARTCONV									2
#define IDSTOPCONV									3
#define IDSTOPALLCONV								4
#define IDSTARTTRANS								5
#define IDSTOPTRANS									6

// TRANSACTION STATUS
#define  DDETRANS_BLOCKED			 			0x0001
#define  DDETRANS_ENABLED						0x0010
#define  DDETRANS_STOPPED						0x0100
#define  DDETRANS_CANENABLE					0x1000
#define  DDETRANS_COMPLETED					0x0110  // enabled | stopped

#define  DDEC_SUSPENDED							0x01
#define  DDEC_ENABLED               0x10

// flows for ddec functions
#define DDEC_SUCCESS								1
#define DDEC_FAILURE                2
#define DDEC_ERROR									0

#define DDEC_SERVERSTARTED 					1
#define DDEC_SERVERNOTSTARTED 			2
#define DDEC_SERVERALREADYSTARTED 	3


#define DDEC_CONVERSTARTED 					1
#define DDEC_CONVERNOTSTARTED 			2
#define DDEC_CONVEREXISTS						3

///////////////////////////////////////////////////////////////////
// The following structure is mapped on to the DATA0 of the object
///////////////////////////////////////////////////////////////////
struct DDECDATA{
	long version;             // data version
	char ServerName[MAXNAME]; // server Name
	char Service[MAXNAME];    // DDE service name
	char Topic[MAXNAME];      // DDE Topic Name
	char Format[MAXNAME];     // DDE Format Name 
	char Item[MAXNAME];       // DDE Item Name
	TYPEID   ObjectType;      // SERIUS OBJECT TYPE
	OBJECTID objectID;        // SERIUS OBJECT to receive transaction data
	int  		 linkType;        // AUTOLINK, NOTIFY, MANUAL
	BOOL 		 bDisplayErr;     // flag to diaplsy DDE error
	char     typeName[MAXNAME]; // serius object's type name
	char     objectName[MAXNAME]; // serius object's name
	};

///////////////////////////////////////////////////////////////////
// the following structure is mapped on to DATA1 of the object
///////////////////////////////////////////////////////////////////
struct DDECOPTIONS{
	char        Country [MAXNAME];
	char        Language[MAXNAME];
	char        CodePage[MAXNAME];
	BOOL        bDefContext;
	BOOL        bAck;      // Client sends ack with every transaction
	CONVCONTEXT CCFilter;  // Conversation Context
  long        timeout;   // in msecs
	long        advDelay;  // in msecs
	};

typedef DDECDATA 		*LPDDECDATA;
typedef DDECOPTIONS *LPDDECOPTIONS;

#ifdef MDRTCODE
	LPLONG GetRunTimePtr(OBJECTID theObject, BOOL msg );
#else
	extern LPLONG GetRunTimePtr(OBJECTID theObject, BOOL msg );
#endif



