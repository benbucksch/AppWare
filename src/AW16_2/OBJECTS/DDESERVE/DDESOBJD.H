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

/********************************************************
	 OBJD.H
	 The following header file declares variables that are
	 used during the OBJD
*********************************************************/

/***************************************************************************
All the defines and decalaration to be used during OBJD
***************************************************************************/

// events defined to facilitate metafile to object
// and object to metafile
#define MAX_NAME 						100    // max size for edit controls with app/topic/item names.
#define DDES_TOPICNOTFOUND 	-1
#define OEMCODE	            GetKBCodePage()



extern "C" BOOL FAR PASCAL _export OptionsDlgProc (
  HWND          hwnd,
  WORD msg,
  WORD wParam,
	LONG          lParam);

#pragma  argsused   // suppress args used warnings
extern int _WinAllocFlag;
char szWild[] = "*";    // used to indicate wild names ("" is also cool)
// static char szT[MAX_NAME];     // temp buf for munging names.

class DDES;

typedef char LPNAME[MAXNAME];

// the following structure are used for conv context during OBJD
struct COUNTRYLIST {
	char  Country[MAXNAME];
	unsigned int  id;

	};



// A list of countries and their telephone country code
COUNTRYLIST countryList[] = {
	 {"Australia", 61},
	 {"Austria", 43},
	 {"Belgium",32},
	 {"Brazil", 55},
	 {"Canada ",2},
	 {"Denmark", 45},
	 {"Finland", 358},
	 {"Germany", 49},
	 {"Ireland", 353},
	 {"Italy",  39},
	 {"Netherlands",599},
	 {"New Zealand", 64},
	 {"Norway",      47},
	 {"Portugal",    351},
	 {"Spain",        34},
	 {"Sweden",       46},
	 {"Switzerland", 41},
	 {"Taiwan",     886},
	 {"United Kingdom", 44},
	 {"United States",  1},
	 {"Other Country", 0}
	 };

// A list of languages and their ids (defined pg. 380, Prog. Ref.Vol.1)
struct LANGUAGELIST {
	char Language[MAXNAME];
	DWORD id;

	};

LANGUAGELIST langList[]=
	{
	{"Danish", atol("DAN{")},
	{"German", atol("DEU{")},
	{"U.K. English",  atol("ENG{")},
	{"U.S. English",  atol("ENU{")},
	{"Modern Spanish",  atol("ESN{")},
	{"Castilian Spanish",  atol("ESP{")},
	{"Finnish",  atol("FIN")},
	{"French",  atol("FRA")},
	{"Canadian French",  atol("FRC")},
	{"Icelandic", atol("ISL")},
	{"Italian",  atol("ITA")},
	{"Dutch",  atol("NLD")},
	{"Norwegian",  atol("NOR")},
	{"Portuguese",  atol("PTG")},
	{"Swedish",  atol("SVE")}
	};


struct KBCPAGE {
	char name[MAXNAME];
	int  id;
	};

KBCPAGE kbCode[]= {
	{"ANSI", CP_WINANSI},
	{"OEM",  OEMCODE}
  };

// a set oif bits that indicate whether we can add the topic,service, item names to main list
#define DDES_ADDEDSERVICE   	0x0001			// 0000000000000001
#define DDES_ADDEDTOPIC      	0x0002  		// 0000000000000010
#define DDES_ADDEDITEM        0x0004    	// 0000000000000100
#define DDES_ADDEDFORMAT      0x0008      // 0000000000001000
#define DDES_ADDEDOBJECT      0x0010      // 0000000000010000
#define DDES_ADDEDALL					0x001F      // 0000000000011111

// a set of bits that indicate whether we can add the Signal name and oid to signal List
#define DDES_ADDEDSIGNAL   	 	0x0001			// 0000000000000001
#define DDES_ADDEDSIGOBJECT   0x0002  		// 0000000000000010
#define DDES_CANADDSIGNAL			0x0003      // 0000000000000011


// Class definition to be used during OBJD time.
class OBJDCLASS {

	private:
	int 	SetMainCtrls(int index);
	int 	SetMainList(HWND hWnd, LPSTR itemStr, OBJECTID objID);
	int 	SetSignalList(LPSTR itemStr, OBJECTID objID);
	// Build the object type list that support the chosen format;
	int  		BuildFormatListForSignals();


	protected:

	public:

	DDES 	*ddes; // an instance of class that provides all the dde
										 // dde functionalities
	HWND 	hDlg;
	BOOL 	bOK;
	int 	topicCount;
	int 	itemCount;
	int 	formatCount;
	int 	objTypeCount;
	int 	objectCount;
 	int 	bCanAdd;
	SIGNALINFO	defSignal;

	 
  // List of services, topics, and items 
	
	OBJDCLASS(HWND hWnd, OBJECTID objID);
	~OBJDCLASS(void);

	LPSTR		Service()	 			{ return ddes->ddesService.Service; }
	int 		SetAddFlag(int bVal) 	{ bCanAdd |= bVal; return bCanAdd;}
	int 		ResetAddFlag(int bVal) { bCanAdd &= ~bVal;return bCanAdd; }

	BOOL 		EnableTopic(HWND hwnd, BOOL bEnable);
	BOOL 		EnableItems(HWND hwnd, BOOL bEnable);
	BOOL 		EnableFormat(HWND hwnd, BOOL bEnable);
	BOOL 		EnableObject(HWND hwnd, BOOL bEnable);

	inline  BOOL GetService() {
					
								 return GetWindowText(GetDlgItem(hDlg, IDSERVICE), ddes->ddesService.Service, MAXNAME);
								
								 }
	inline  BOOL GetTopic() {
								
								return GetWindowText(GetDlgItem(hDlg, IDSERVICE), ddes->defTopic, MAXNAME);
								 }

	inline 	BOOL GetItem() {
								return GetWindowText(GetDlgItem(hDlg, IDITEM),ddes->defItem, MAXNAME);

								 };
	inline 	BOOL GetFormat() {
								return GetWindowText(GetDlgItem(hDlg, IDFORMAT),ddes->defFormat.name, MAXNAME);
								 }

	inline  BOOL GetObjType() {
  							BOOL 	rVal = FALSE;
								DWORD dwIndex = SendDlgItemMessage(hDlg, IDOBJTYPE, LB_GETCURSEL, 0, 0);
								if (dwIndex != LB_ERR) {
									ddes->defObjectType= (TYPEID)SendDlgItemMessage(hDlg, IDOBJECT,
										LB_GETITEMDATA, (WPARAM) dwIndex,0L);
									rVal =TRUE;
									}
								return rVal;
								
								}

	inline  DWORD GetObject() {
								DWORD dwIndex = SendDlgItemMessage(hDlg, IDOBJECT, LB_GETCURSEL, 0, 0);
								if (dwIndex != LB_ERR) {
									ddes->defDataSrcOID = (OBJECTID)SendDlgItemMessage(hDlg, IDOBJECT,
											LB_GETITEMDATA, (WPARAM) dwIndex,0L);
										}
								return dwIndex;

								}

	inline 	int GetUpdateTime()
								{
                BOOL val;
								ddes->defUpdateTime = GetDlgItemInt(hDlg, IDTIME, &val, FALSE);
								return 1;
								}

	BOOL 		SetService(LPSTR name);
	BOOL 		SetTopic(LPSTR name);
	BOOL 		SetItem(LPSTR name);
	BOOL 		SetFormat(LPSTR name);
	BOOL 		SetObjType(TYPEID typeID);
	BOOL 		SetObject(OBJECTID objID);
	int 		SetUpdateTime(long Time);
	BOOL 		SetSignal(LPSTR name);
	int 		SetContext();
	int 		ChangeName(LPSTR name, int ctrlID);
	BOOL 		GetFromMainList(LPSTR Topic,LPSTR	Item,LPSTR Format,OBJECTID *ObjectID);


	inline 	LPLONG IsDDE() { return (LPLONG)ddes;} // check to see if dde is initialised
	// Build the object type list that support the chosen format;
	int  		BuildFormatList();
	// Build the object type list that support the chosen format;
	int  		BuildObjTypeList( ATOM formatID) ;
	// Build the object list that support the chosen type
	int  		BuildObjectList( TYPEID sTypeID) ;

	// methods to access ddes fields
	inline DDESSERVICE *ddesService()	{ return &ddes->ddesService; }

	// methods on the Main list of (topics, item and oid)
	BOOL 		BuildDefMainStr(LPSTR mainStr);
	void 		BuildMainList();
	BOOL 		SetCtrlsFromMainLst();
	int  		AddMainList();
	int 		AddMainList(LPSTR defTopic,LPSTR defItem,LPSTR defFormat,OBJECTID defSrcOID,long  	defUpdateTime);
	int  		DeleteMainList();
	int  		DeleteMainList(LPSTR defTopic,LPSTR defItem,LPSTR defFormat,OBJECTID defSrcOID);
	int  		UpdateMainList();
	BOOL 		CheckForSystem(int index); // check to see if any system topic:items have been selected

	// methods on Signal list

	int			CreateSignalsFromMainList(); // Create a list of signals based on items in MAIN list
  																		// called during IDOK handling
	void 		BuildSignalList();
	int  		AddSignalList(SIGNALINFO signalInfo);
	int 		DeleteSignalList(LPSTR pszSignal,OBJECTID defRcvrOID);
	int  		UpdateSignalList();
	BOOL 		GetFromSignalList(SIGNALINFO *curSignal);
	BOOL 		InitSigCtrls();
	BOOL 		SetCtrlsFromSignalList();
	// Process different attributes's dialog
	int 		ProcessSubDialog(int dlgType);

	// Save and Init Sys Help dlg box
	BOOL SaveSysHelp();
	void InitSysHelp();
	};

LONG GetDlgItemLong(HWND hwnd, WORD id, BOOL *pfTranslated, BOOL fSigned);
VOID SetDlgItemLong(HWND hwnd, WORD id, LONG l, BOOL fSigned);




