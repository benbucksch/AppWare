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
// FILE: 			 	ddemlsv.h
//							
// AUTHOR:  		Shaili Jain
//
// DESCRIPTION:	The following header file defines C++ class
//	 						to handle DDEML interaction 
//
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <windows.h>
#include <a_alm.h>
#include <ddeml.h>
#include <intdebug.h>
#include <o_pict.h>

#include "resource.h"

// define used to display debugging message
#define DEBUG							 				1
#undef DEBUG


#define DEFTIMEOUT         				1000
#define DDEC_CURRENTDATAVERSION 	1
#define DDEC_CURRENTDLLVERSION  	1
#define MAXFORMATS  			 				4 // TEXT, ALMOBJ, BITMAP, METAFILE

// event used to poll the HOT/WARM signal Queue
#define ddec_PollSigQ  						MAKEEVENTID('d','c','p','Q')

// Type id DDE Client ALM
#define OTYPE_DDEC  							MAKETYPEID('D','d','e','c')

// events defined to facilitate metafile to object
// and object to metafile
#define metafileToObject  				MAKEEVENTID('m','t','o','O')

#define SDDE_NOMATCHINGFORMAT 		-1
#define MAX_NAME 									100    // max size for edit controls with app/topic/item names.


#define FORMATLEN									MAX_NAME

#define CFALMOBJ      						"ALMOBJ"
#define CFTEXT        						"Text"
#define CFBITMAP      						"Bitmap"
#define CFMETAFILE  							"Metafile"

#define TEXT_FORMAT	        			CF_TEXT
#define BITMAP_FORMAT      				CF_BITMAP 
#define METAFILE_FORMAT  					CF_METAFILEPICT
#define ALMOBJ_FORMAT			  			4


#define CHILDCBWNDEXTRA	    			4
#define UM_GETNEXTCHILDX    			(WM_USER + 200)
#define UM_GETNEXTCHILDY    			(WM_USER + 201)
#define UM_DISCONNECTED     			(WM_USER + 202)

// message defined to facilitate setting of servicelist from DDEML callback
#define UM_SETSERVICELIST   			(WM_USER + 100)

#define DDECLIENTFLAGS						CBF_FAIL_ADVISES|CBF_FAIL_CONNECTIONS|CBF_FAIL_EXECUTES|CBF_FAIL_REQUESTS
#define NODBUG      							1

// LINK TYPES
#define AUTO_LINK           			IDAUTOLINK     // hot link
#define NOTIFY_LINK         			IDNOTIFYLINK   // warm link
#define MANUAL_LINK								IDMANUALLINK   // Cold link

// id for transaction of REQUEST type
#define ID_REQUESTTRANS     			-1

#define MIN_SYNCTIMEOUT						1000

#define MAXFILETITLELEN  					100
#define MAXNAME					 					100

//////////////////////////////////////////////////////////////////
// DDECLIENT Datas definition
/////////////////////////////////////////////////////////////////
#define MAXDDECLIENTDATAS 				2      	// 1. DDECDATA
																	 				// 2. DDECLIENTOPTIONS
#define data0_DDECDATA      			0
#define data1_DDECOPTIONS   			1

#define MyAlloc(cb)      					(LPSTR)new char[cb] 		
#define MyFree(p)       					delete p         				


// ERROR defines for import to ALM object datas
#define ALM_IMPORTSUCCESS   			1	
#define ALM_ERRTEXTIMPORT   			2
#define ALM_ERRBMPIMPORT 					3
#define ALM_ERRMETAIMPORT  				4
#define ALM_ERRALMIMPORT					5

#define USER_DEFNAME 							"Undefined"

// index of the user defined string in all lbox (at OBJD)
#define INDEX_USERDEFSTR  	0
 
// predefined format list item
/* CF_TEXT, CF_BITMAP, CF_METAFILE, CF_ALMOBJ */
#define SERFORMATS       		4

#ifdef CFINIT
	unsigned int 										FID_CFALMOBJ;      // format id for ALMOBJd
#else
	extern 	 unsigned int 				 	FID_CFALMOBJ;
#endif

// TRANSACTION FLAGS:
#define XACT_AUTONACK       	 		XTYP_ADVSTART|XTYPF_FACK;
#define XACT_NOTIFYNACK     			XTYP_ADVSTART|XTYPF_NODATA|XTYP_FACK;
#define XACT_NOTIFY		      			XTYP_ADVSTART|XTYPF_NODATA

//////////////////////////////////////////////////////////////
// Signal Defines
//////////////////////////////////////////////////////////////
#define MAX_SIGNALS								7
#define SIGDDEC_CONVERSSTARTED 		1
#define SIGDDEC_CONVERSSTOPPED   	2
#define SIGDDEC_AUTOTRANSSTARTED 	3
#define SIGDDEC_AUTOTRANSSTOPPED 	4
#define SIGDDEC_MANTRANSCOMPLETED	5
#define SIGDDEC_ADVDATACHANGED 		6
#define SIGDDEC_REQDATACHANGED   	7

#define INVALIDID						 			0

typedef struct
	{
	char 				serverName[MAXNAME];
	char 				serviceName[MAXNAME];
	char 				instServiceName[MAXNAME];
	} SERVICELIST;

typedef SERVICELIST *LPSERVICELIST;

typedef struct
  {
	ATOM   			typeID;
	char   			name[FORMATLEN];
  } FORMATINFO;

typedef struct
  {
	TYPEID 			typeID;
	char   			name[FORMATLEN];
	} TYPEINFO;

// conversation Info for each DDE Object
typedef struct {
		BOOL 			fList;
		char 			ServerName[MAXFILETITLELEN];
		HSZ       hszService;
		HSZ       hszTopic;
	  char 			Service[MAXNAME];
		char 			Topic[MAXNAME];
		HCONV 		hConv;
		BYTE      status;
		CONVINFO 	ci; 			// most recent status info.
		} MYCONVINFO;       // parameters to AddConv() in reverse order.


// transaction processing structure - this structure is associated with
// infoctrl control windows.  A handle to this structure is placed into
// the first window word of the control.
typedef struct {    // used to passinfo to/from TransactionDlgProc and
		int       transID;
    DWORD    	ret;      // TextEntryDlgProc.
    DWORD 	 	Result;
    DWORD 	 	ulTimeout;
		WORD 		 	wType;
		BOOL      bAck;
		HDDEDATA 	hDdeData;
		WORD 		 	wFmt;
		HSZ 		 	hszItem;       // handle to item string
    WORD 		 	fsOptions;
		int      	linkType;
		long      advDelay;
		long      lastUpdatedTime; // used as a marker for next update
		OBJECTID  receiverOID;  // object that receives the data
		int       status;  // BLOCKED, ENABLED, STOPPED
		char 			Item[MAXNAME]; // item string name
		char      Format[MAXNAME];

		} XACT;


typedef struct {
    HDDEDATA 	hData;
    HSZ 			hszItem;
    WORD 			wFmt;
		} OWNED;


////////////////////////////////////////////////////////////////////////
// DDECQ Management defines -- Used for signal Queue
///////////////////////////////////////////////////////////////////////

// Max number of signals allowed in the circular DDECQ
#define MAXQITEMS   						64

// Element in the DDEC Signal Queue
struct QITEM {
	HSZ  hsz;
	int  wFmt;
	long sigID;
	MEMBLOCKID dataID;
	BOOL bNewSig;
	QITEM *nextItem;
	}  ;

class DDECQ {
  public:
	  QITEM *startItem;
		QITEM *curItem;
		QITEM *insertItem;
		int   numOfItems;
		DDECQ();
    ~DDECQ();
		static DDECQ *BuildQ();
		static DestroyQ(DDECQ *q);
		AddToQ(HSZ hsz2, int wFmt,long sigID,HDDEDATA hData);
		RemoveCurItem();
		QITEM *GetCurItem();
		QITEM *GetStartItem();
 		SetCurItem(QITEM *item);
		SetStartItem(QITEM *item);
		BOOL DispatchCurSignal();

	};


// transaction option flags - for fsOptions field and DefOptions global.

#define XOPT_NODATA             0x0001
#define XOPT_ACKREQ             0x0002
#define XOPT_DISABLEFIRST       0x0004
#define XOPT_ABANDONAFTERSTART  0x0008
#define XOPT_BLOCKRESULT        0x0010
#define XOPT_ASYNC              0x0020
#define XOPT_COMPLETED          0x8000      // used internally only.


#define MAX_OWNED   20

/*
 *  GLOBALS
 */

#ifdef MAINMODULE
 /*
 *   GLOBALS
 */

HINSTANCE 		hInst;		/* application instance handle		  */
FORMATINFO 		SFormats[MAXFORMATS];  // 4 formats July 1993
OWNED 				aOwned[MAX_OWNED];
char 					*szDriver;		/* name of printer driver		  */
WORD 					fmtLink;        // registered LINK clipboard fmt
CONVCONTEXT  	CCFilter;
WORD DefOptions;

#else
extern HINSTANCE hInst;		/* application instance handle		  */
extern WORD fmtLink;        // registered LINK clipboard fmt
extern WORD DefOptions;
extern char *szDriver;		/* name of printer driver		  */
extern FORMATINFO SFormats[];
extern OWNED aOwned[MAX_OWNED];
extern CONVCONTEXT CCFilter;

#endif
// MACROS

/*  externally declared functions
 */

// ddemlcl.c

BOOL FAR PASCAL InitializeFormats(VOID);
BOOL FAR PASCAL InitializeInstance(WORD);
BOOL FAR PASCAL GetInitializationData(HWND);
VOID FAR PASCAL Find(void);
BOOL FAR PASCAL IsWild (LPSTR);
VOID FAR PASCAL SelectFile (HWND);
VOID FAR PASCAL FindText ( int );
LPSTR Status2String(WORD status);
LPSTR State2String(WORD state);
LPSTR Error2String(WORD error);
LPSTR Type2String(WORD wType, WORD fsOptions);
LPSTR GetHSZName(HSZ hsz);
int FAR DoDialog(LPCSTR lpTemplateName, DLGPROC lpDlgProc,OBJECTID objID);

// dde.c
// Call back routine for the dde client
extern "C" HDDEDATA EXPENTRY _export DdeCallback(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1,
				HSZ hsz2, HDDEDATA hData, DWORD lData1, DWORD lData2);

int 		GetTextData(HDDEDATA hData, MEMBLOCKID txtID);
LPSTR 	GetFormatData(HDDEDATA hData);
int 		MyGetClipboardFormatName(WORD fmt, LPSTR lpstr, int cbMax);
LPSTR 	GetFormatName(WORD wFmt);

//////////////////////////////////////////////////////////////////////////////
//
// Class used to manage an DDEC object
//
//////////////////////////////////////////////////////////////////////////////
class DDEC{

	private:
		BOOL 			fEnableCBs;
		BOOL 			fEnableOneCB;
		BOOL 			fBlockNextCB;
		BOOL 			fTermNextCB;
		HDDEDATA 	hDataOwned;
		WORD 			DefOptions;
		BOOL 			fCase;		/* searches case sensitive		  */
		WORD 			cFonts;		/* number of fonts enumerated		  */
		WORD 			cOwned;

		XACT*			GetCurrentTrans(int trID);

	public:
		
		BOOL 			fAutoReconnect;
		// default parameters
		char 			defItem[MAXNAME];
		char		 	defFormat[MAXNAME];
		TYPEID 		defObjectType;
		char   		defTypeName[MAXNAME];
		OBJECTID 	defReceiverOID;
		char     	defObjectName[MAXNAME];
		int      	defLinkType;
		BOOL      bDefAck;  				// ACK requested for ADVISE transaction
		long     	timeout;
		long     	defAdvDelay; 			// default adv. delay
		long      lastUpdatedTime;
		// stored in data1_OPTIONS
		char     	Country[MAXNAME];
		char     	Language[MAXNAME];
		char     	CodePage[MAXNAME];
		BOOL     	bDefContext;  		// bool to use def conv context info
		BOOL     	bDisplayErr; 			// This will error messages when a DDE call fails
		CONVCONTEXT CCFilter;
		MYCONVINFO 	convInfo;

		// Runtime info
		DDECQ * 	sigQ;
		OBJECTID  ddeCID;
		DWORD     idInst;        		// DDEML instance of this Client Application
		int       numOfTransaction;
		XACT 			*pxact;          	// holds last transaction specifc info
		MEMBLOCKID xactListID;    	// holds a list of transactions
		XACT      lastUpdtdXact; 		// last transaction that was updated
		XACT      lastActvdXact; 		// copy of the last started transction
		XACT      lastAbrtdXact; 		// copy of the last abandoned trans
		HINSTANCE serverInst;   		// store the server instance handle during StartServer

		static 		MEMBLOCKID  serviceListID; // holds the list of currenlty available services
		static 		HWND hConvWnd;
    
		///////////////////// METHODS ////////////////////////////

		DDEC(OBJECTID objID, BOOL bOBJD);
		~DDEC();
		UINT 		RunServer(LPSTR name);

		// used during OBJD
		int   	BuildServiceList(HWND );

		static 	void  RemoveFromServiceList(HSZ serviceName, HSZ instServiceName);

		static 	void  AddToServiceList(HSZ serviceName, HSZ instServiceName);

		HCONV 	CreateConv(HSZ hszApp, HSZ hszTopic, BOOL fList, WORD *pError);

		BOOL  	Disconnect();

		// All these members are declared static because they are called from
		// the dde client's callback routine

		static 	HANDLE FAR PASCAL AddConv(HSZ hszApp, HSZ hszTopic, HCONV hConv, BOOL fList);

		static 	LPSTR GetConvListText(HCONVLIST hConvList);

		static 	LPSTR GetConvInfoText(HCONV hConv, CONVINFO *pci);

		static 	LPSTR GetConvTitleText(HCONV hConv, HSZ hszApp, HSZ hszTopic, BOOL fList);

		BOOL  	ProcessTransaction();

		LPSTR  	CompleteTransaction();

		int 		SetALMData(XACT *xact, HDDEDATA hData,BOOL isHandle, BOOL bDestroyHandle);

		friend 	DDEC* GetConvContext(HANDLE hConv);

		// Returns the XACT context of the current transaction
		XACT 		*DDEC::GetTransactionContext(int trID);

		XACT	 	*DDEC::EnumTransactions(LPBYTE hszItem, BOOL bHandle, ATOM wFmt, int *index);

		short 	FAR CDECL MPError(HWND,WORD,WORD, ...);

		// these methods have a 1 to 1 correspondence to the ddec user functions
		int 				StopServer();

		int 	 			StartConversation();

		int 	 			StopConversation();

		static int 	StopAllConversations();

		int 				StartTransaction(LPSTR itemStr, LPSTR formatID, LPSTR linkStr, long delay, BOOL bAck,OBJECTID oID);

		int 				StopTransaction(int transactionID);

		int         GetData(int transactionID);

		int         IsServiceAvailable(LPSTR service);

		int         ExecTransaction(LPSTR commStr, LPSTR dataStr);

		int         SendData(LPSTR itemStr, LPSTR dataStr);

		int 				DoTransaction( LPSTR lpItem, MEMBLOCKID txtID);

		BOOL 				Delay(XACT *);

		BOOL 				DispatchCurSignal();

		};



