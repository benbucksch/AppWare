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
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/

#include <windows.h>
#include <ddeml.h>
#include "resource.h"


#define OTYPE_DDES                  ATYPE_MAKEID('D','d', 'e', 's')

#define MAXNAME                     100
#define DDES_DATAVERSION_1         1
#define DDES_DLLVERSION_1          2

#define DDES_DATAVERSION            2
#define DDES_DLLVERSION             2

#define MAXDDESERVERDATAS           5     // dat#1. 	DDESDATA
                                          // data#2. 	DDESTOPICS
                                          // data#3. 	DDESITEMS
																					// data#4. 	OBJD def Values
																					// data#5. 	Service Help Text
#define data0_DDESDATA              0
#define data1_DDESTOPICS            1
#define data2_DDESITEMS             2
#define data3_DEFVALS               3
#define data4_SYSHELP								4

// SIGNAL defines
// Should I add a signal on Feb. 17, 1995 for POKE
#define DDES_NUMOFSIGS              7
// this define is used to index into the topiclist for 'system' topic
#define INDEX_SYSTEMTOPIC         	0

#define SIGDDES_CONVERSSTARTED      1
#define SIGDDES_CONVERSSTOPPED      2
#define SIGDDES_AUTOTRANSSTARTED    3
#define SIGDDES_AUTOTRANSSTOPPED    4
#define SIGDDES_MANTRANSCOMPLETED   5
#define SIGDDES_ADVDATACHANGED      6
#define SIGDDES_REQDATACHANGED      7
// #define SIGDDES_DATARECEIVED        8  // POKE TRANSACTION

// indexes into signal list in object data (DATA0)
// #define		I_REQUESTCONVSIG						0
#define		I_ESTABCONVSIG						0
#define		I_STOPCONVSIG						 	1
#define		I_REQUESTTRANSSIG				 	0
#define		I_UPDATETRANSSIG					1
#define   I_NOSIGNAL								-1

#define   MAX_TOPICSIGNAL						2  		// for every topic there are 2 signals
																					//	1.	Conv. Established
																					//	2.	Conv. Terminated
#define	MAX_ITEMSIGNAL							2  		// for every item there are 2 signals
																					// 	1. 	Transaction on Item Requested
																					//  2.  Transaction Data Updated
#define	INVALIDOID									0

#define OBJECTINSTDATA              MAXDDESERVERDATAS  // slot to save c+= object instance


#define USER_DEFNAME                "UserDefined"
#define CFALMOBJ                    "ALMOBJ"
#define CFTEXT                      "TEXT"
#define CFBITMAP                    "BITMAP"
#define CFMETAFILE                  "METAFILE"

#define MAX_FORMATS                 4 // text, bitmap, metafile, almObj

#define MAX_EXEC                    40
#define CCHARS                      80
#define CLINES                      40

#define UM_CHGDATA                  (WM_USER + 435)

#define CSYSTEMITEMS                6
#define CTESTITEMS                  4
#define CTOPICS                     2

// Item under 'system' topic to list all the signals off a service
#define SZDDES_SIGNALLIST           "Signals"

#define DDES_TOPICNOTFOUND          -1
#define DDES_ITEMNOTFOUND           -2
#define DDES_XCTIONNOTFOUND         -3
#define DDES_REMOVEALLITEMS         -3
#define DDES_ITEMALREADYEXISTS      -4
#define DDES_NEWITEM                1
#define DDES_NOTINIT                0   // no dde initialisation
#define DDES_SIGNALNOTFOUND         -1

#define DDES_SAVESIGS               0x0001
#define DDES_SAVEOBJD               0x0002
#define DDES_SAVEOPTIONS            0x0004
#define DDES_FLAGS                  0x0007



#define 	DDES_SUCCESS						1
#define 	DDES_FAILURE						0



class  DDES;
// predefined format list item

typedef struct {
		ATOM  atom;
		char  name[MAXNAME];
		} FORMATINFO;


/* ================================================================
Struct to hold used defined signal info.  It is used during
OBJD
================================================================ */

struct SIGNALINFO {
  char      name[MAXNAME];
  long      sigID;
  AOBJECTID  rcvrOID;
  };



struct TOPICINFO {
  int nItems;         // num of Items under this topic
  char name[MAXNAME];
  };

// attributes that define a service
struct ITEMSINFO {
  char Topic[MAXNAME];
  char Item[MAXNAME];
  FORMATINFO format;
  AOBJECTID dataSrcOID;        // data source
 	long  updateTime;
  };

// first data of DDE Server Object
struct DDESDATA {
  long        version;          // version number
	AOBJECTID    ddesOID;
	DWORD 			idInst;
	char        Service[MAXNAME];

  int         nUserSignals;     // number of user signals
  int         nTopicSignals;    // number of topic/item signals
	int         nTopics;          // num of Topics

	// the following structure is mapped on to DATA1 of the object
	char        Country [MAXNAME];
	char        Language[MAXNAME];
	char        CodePage[MAXNAME];
	BOOL        bDefContext;
	CONVCONTEXT CCFilter;  // Conversation Context

  SIGNALINFO  signals[];        // array of signals

  };

// second Data block
struct DDESTOPICDATA {
  // a list of services provided by this DDES object
  // use this to construct runtime TopicList and Item List
  TOPICINFO   topicList[];
  };

struct DDESITEMDATA {
  ITEMSINFO itemList[];
  };

struct DEFOBJDDATA_V1{
  // def strings used to init the controls at OBJD time
  char        defTopic[MAXNAME];
  char        defItem[MAXNAME];
  FORMATINFO  defFormat;
  AOBJECTID    defDataSrcOID;
	long        defUpdateTime;

	};

struct DEFOBJDDATA {
  // def strings used to init the controls at OBJD time
  char        defTopic[MAXNAME];
	char        defItem[MAXNAME];
  FORMATINFO  defFormat;
  AOBJECTID    defDataSrcOID;
	long        defUpdateTime;

	long        maxSignalID;     // SKJ added Mar. 7 to track the Signals Used
	BYTE        extraBytes[8];   // Extra 8 bytes
	};

typedef DDESDATA       *LPDDESDATA ;
typedef DDESTOPICDATA  *LPDDESTOPICS ;
typedef DDESITEMDATA   *LPDDESITEMS;
typedef DEFOBJDDATA    *LPDEFOBJDDATA;
/****** Structrues ******/


#define CFORMATS  4

extern FORMATINFO aFormats[CFORMATS];

// this structure depends on the fact that EXPENTRY is defined as
//  calling convention.  This allows us to merely point to
// the last parameter of DdeCallback and cast the pointer to this
// structure to save stack space. (a common hack)

typedef struct _XFERINFO {
    DWORD lData2;
    DWORD lData1;
    HDDEDATA hData;
    HSZ hszItem;
    HSZ hszTopic;
    HCONV hConv;
    WORD wFmt;
    WORD wType;
} XFERINFO;

typedef XFERINFO *PXFERINFO;    


// every time a succesful advise/warm transaction is established, we add this stucture
// to a AMEMBLOCKID (transID) which is held in the DDES class instance.
typedef struct _XCTIONINFO {
    DWORD    lData;
    HDDEDATA hData;
		HCONV    hConv;
		HSZ			 hszTopic;							// store the topic string handle
		HSZ			 hszItem;               // store the item string handle
    BOOL     isValid;
    AOBJECTID srcOID;
    DDES    *ddes;                   // keep the ddes context around for zpeed purposes
                                     // Also during certain transactions (EXECUTE) the user data
                                     // of a conv. handle does not seem to have the 'ddes' ptr
                                     // WHO CHANGES THIS?
    } XCTIONINFO;





typedef struct {    // used to passinfo to/from TextDataEntryDlgProc and
    HDDEDATA hDdeData;
    WORD wFmt;
    HSZ hszItem;
    } XACT;

/////////////////////////////////////////////////////////
// DDES class defintition
////////////////////////////////////////////////////////



// info about each item
struct DDESITEMLIST {
    HSZ         hszItem;
    char        pszItem[MAXNAME];
    AOBJECTID    dataSrcOID;           // data source ALM object
    FORMATINFO  format;
    long         updateTime;
    };

// info about each topic
struct DDESTOPICLIST {
    HSZ         hszTopic;
    char        pszTopic[MAXNAME];
    AMEMBLOCKID  itemListID;         // mem block holding the item List
    //  DDESITEMLIST *pItemList;    // pointer to the mem
    int         nItems;             //  #of items for this topic
    };

// info about each DDE Server Object
struct DDESSERVICE {
  HSZ           hszService;
  char          Service[MAXNAME];
  int           nTopics;
  AMEMBLOCKID    topicListID;        // list of ddetopiclist
  };

class OBJDCLASS; // defined in DDESOBJD.h
class DDES {

  private:
  int         nUserSignals;               // user defined Signals off this Object
  int         nTopicSignals;              // signals based on Topic / items
  SIGNALINFO  *userSignals;               // user defined signals
  SIGNALINFO  *topicSignals;              // user defined signals

  int         nXctions;
  long        GetSignal(LPSTR szExec);

	// this class implements the interface to the object Edit dlg box
	friend      OBJDCLASS; 
  // Create a List of items under the 'SYSTEM' topic
  void        CreateSystemTopic();
  DDESSERVICE ddesService;

	int         IsNewSignal(SIGNALINFO defSig);
	BOOL 				fAppowned;
	WORD 				RenderDelay;
	DWORD 			count;
	long 				maxSignalID;      // added to keep track of used signals
	BOOL 				SignalExists(LPSTR sigName);
	BOOL 				AdjustTopicSignalIDs();
	BOOL 				AdjustUserSignalIDs();
  /*********************************PUBLIC FIELDS******************************************/

  public:
	DWORD				idInst;
	HCONV				hConv;

  // default attributes
  // used as def. parameters in OBJD dlg box
  char        defTopic[MAXNAME];
  char        defItem[MAXNAME];
  FORMATINFO  defFormat;
  AOBJECTID    defDataSrcOID;
  ATYPEID      defObjectType;
	long        defUpdateTime;
  AOBJECTID    ddesOID;                // ddes object
  
  BOOL        bDisplayErr;
	int         bSave;

	static 			WORD 				cServers;
	CONVCONTEXT CCFilter;
	// stored in data1_OPTIONS
	char     		Country[MAXNAME];
	char     		Language[MAXNAME];
	char     		CodePage[MAXNAME];
	BOOL     		bDefContext;  // bool to use def conv context info
	void        Createhsz();
  void        Destroyhsz();

  AMEMBLOCKID  XMemID;               // block to hold the array of XCTIONINFO structures
	AMEMBLOCKID  helpMemID;						// block to hold the help text for thhis service
  DDES(AOBJECTID objID);             // constructor
  ~DDES();                          // destructor

  inline      DDESSERVICE GetddesService() { return ddesService; }

  int         AddTopicSignal(LPSTR sigName, BOOL isTopic);  // a a topic signal to our list
	int         SaveSignalsToData();          // save the signsls to datas
  int         BuildSignalsFromData();
	BOOL				CheckSignals();               // check for errors in signals
	long 				PostSignal(HSZ  	hszTopic,HSZ		hszItem, int		iFmt,	int  	wType);
  BOOL        RemoveSignal(int delIndex);

  int         IsNewTopic(LPSTR Topic);
  int         IsSamehszTopic(HSZ hszTopic);
  int         IsSamehszItem(int index, HSZ hitem, WORD dFormat, AOBJECTID *oid);
  int         RemoveItem(int delTopicIndex, int delItemIndex);
 
  static      DDES *  SetContext(HCONV hConv, HSZ   hszService, HSZ   hszItem,  WORD  wformat, WORD wType);
  static      DDES*   GetContextFromhConv(HCONV hConv);
  HSZ         GethszTopic(int index);
  DDESTOPICLIST *GetTopicList();
  BOOL        UnGetTopicList();
  DDESITEMLIST *GetItemList(int topicIndex);
  HDDEDATA    GetXctionData(PXFERINFO pXferInfo,AOBJECTID oid, WORD iFmt);
  int         AddXction(PXFERINFO pXferInfo, AOBJECTID oid);
	int         RemoveAllXctions(HCONV hConv);

  // SKJ, Feb. 18,1995 To handle POKE function
	int 				SetALMData(  PXFERINFO  lpXact, AOBJECTID oid, BOOL isHandle , BOOL bDestroyH);

  HDDEDATA    HandleTransactions(PXFERINFO pxferInfo, WORD  iFmt);
  int         EnumOIDXction(AOBJECTID    oid,XCTIONINFO  *Xction, long *count);

	// Validate Contexts
	BOOL 				ValidateContext(PCONVCONTEXT pCC);
	BOOL        HandleExec(HDDEDATA hData);

	void 				Save(BOOL val) 			{bSave = val; }
	int 				SetSave(int bVal) 	{ bSave |= bVal; return bSave;}
	int 				ResetSave(int bVal) { bSave &= ~bVal;return bSave; }

	// Called from DDES_Copy function
	BOOL 				GetConvInfoFromOID(AMEMBLOCKID topicID, AMEMBLOCKID itemID, AOBJECTID oid);


	friend 			void SaveOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **oldSig);
	friend 			void ResetOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **oldSig);
	};

// system Services
typedef struct _SYSITEMLIST {
    HSZ hszItem;
    HDDEDATA (*npfnCallback)(PXFERINFO, DDES*, int);
    PSTR pszItem;
    } SYSITEMLIST;

///////////////////////////////////////////////////////////

// GLOBALS
// a count of the dde server objects instantiated
static long ddesCount = 0;
#ifdef DDESMODULE
	HINSTANCE  hInst;       // instance of the dll
#else
	extern HINSTANCE	hInst;  // instance of the dll
#endif
extern FORMATINFO aFormats[CFORMATS];
extern SYSITEMLIST SystemTopicItemList[CSYSTEMITEMS];

// FUNCTIONS
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
int  DoDialog(LPCSTR lpTemplateName, FARPROC lpDlgProc, DWORD param);


extern "C" HDDEDATA EXPENTRY __export DdeCallback(WORD wType, WORD wFmt, HCONV hConv, HSZ hszTopic,
        HSZ hszItem, HDDEDATA hData, DWORD lData1, DWORD lData2);

// These functions are called during handling of items undes 'SYSTEM' topic
HDDEDATA TopicListXfer(PXFERINFO pXferInfo, DDES *ddes, int index);
HDDEDATA SysItemListXfer(PXFERINFO pXferInfo, DDES *ddes, int index);
HDDEDATA SysFormatsXfer(PXFERINFO pXferInfo, DDES *ddes, int index);
HDDEDATA HelpXfer(PXFERINFO pXferInfo, DDES*, int index);
HDDEDATA AllItemsXfer(PXFERINFO pXferInfo, DDES*, int index);
HDDEDATA SignalsXfer(PXFERINFO pXferInfo, DDES*, int index);

void 		Hszize(void);
void 		UnHszize(void);
void 		Delay(DWORD delay, BOOL fModal);


