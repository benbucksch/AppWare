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
// FILE: 				OBJD.H
//							The following header file declares variables that are
//	 						used during the OBJD
//
// AUTHOR:  		Shaili Jain
//
// DESCRIPTION:	Contains routines for handling of DDE interaction with  
//	 						DDEML.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

/***************************************************************************
All the defines and decalaration to be used during OBJD
***************************************************************************/

#define MAX_NAME 						100    		// max size for edit controls with app/topic/item names.

#define OEMCODE	            GetKBCodePage()

extern "C" BOOL CALLBACK OptionsDlgProc (
  HWND          hwnd,
  WORD msg,
  WORD wParam,
	LONG          lParam);

#pragma  argsused   									// suppress args used warnings
extern int _WinAllocFlag;
char szWild[] = "*";    							// used to indicate wild names ("" is also cool)
static char szT[MAX_NAME];     			 	// temp buf for munging names.


typedef char LPNAME[MAXNAME];

// the following structure are used for conv context during OBJD
struct COUNTRYLIST {
	char  Country[MAXNAME];
	unsigned int  id;
	};



// A list of countries and their telephone country code
COUNTRYLIST countryList[] = {
	 {"Australia", 		61},
	 {"Austria", 			43},
   {"Belgium",			32}, 
	 {"Brazil", 			55},
	 {"Canada",			 	 2},
	 {"Denmark", 			45},
	 {"Finland", 		 358},
	 {"Germany", 			49},
	 {"Ireland", 		 353},
	 {"Italy",  			39},
	 {"Netherlands", 599},
   {"New Zealand", 64},
	 {"Norway",      47},
	 {"Portugal",    351},
	 {"Spain",       34},
	 {"Sweden",      46},
	 {"Switzerland", 41},
	 {"Taiwan",     	886},
	 {"United Kingdom", 	44},
	 {"United States",  	1},
	 {"Other Country", 	0}
	 };

// A list of languages and their ids (defined pg. 380, Prog. Ref.Vol.1)
struct LANGUAGELIST {
	char Language[MAXNAME];
	DWORD id;

	};

LANGUAGELIST langList[]=
	{
	{"Danish", atol("DAN")},
	{"German", atol("DEU")},
	{"U.K. English",  atol("ENG")},
	{"U.S. English",  atol("ENU")},
	{"Modern Spanish",  atol("ESN")},
	{"Castilian Spanish",  atol("ESP")},
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

// Class definition to be used during OBJD time.
class OBJDCLASS {

	protected:
	DDEC 		*dde; 								// an instance of class that provides all the dde
																// dde functionalities

	int 		serviceCount;         // count of services for a given server
	int 		topicCount;        		// count of Topics for a given service
	int 		itemCount;						// count of Items for a given Topic
	int 		formatCount;         	// count of Formats for a given Topic
	int 		objTypeCount;					// Count of Object types that match the format
	int 		objectCount;					// Count of objects that match the type

	// List of services, topics, and items
	LPNAME 	*topicList;    				// list of Topics for a given service
	LPNAME 	*itemList;     				// list of Items for a given Topic
	LPNAME 	*formatList;   				// list of Formats for a given Topic

	int  DoTransaction(LPSTR itemStr, AMEMBLOCKID memID); // Handle transcation during OBJD


  // Inline functions to set/get fields
	inline  BOOL SetDisplayFlag(BOOL val){dde->bDisplayErr = val; return val;}

	inline  BOOL GetServerName(LPSTR name) {
								 lstrcpy(name, dde->convInfo.ServerName);
								 if (lstrcmp(dde->convInfo.ServerName, "") == 0)
									 return FALSE;
								 else
								   return TRUE;
							 }

	inline  BOOL GetService(LPSTR name) {
								 lstrcpy(name, dde->convInfo.Service);
								 if (lstrcmp(dde->convInfo.Service, "") == 0)
									 return FALSE;
								 else
								   return TRUE;
								 }

	inline  BOOL SetServerName(HWND hWnd, LPSTR name) {
							 if (lstrcmp(dde->convInfo.ServerName, name) != 0)
                 {
								 lstrcpyn((LPSTR)dde->convInfo.ServerName,(LPSTR) name, MAX_NAME -1 );
								 dde->convInfo.ServerName[MAX_NAME - 1] = '\0'; // null pad the the string
								 // If there is no server name, then disable run button
								 if (lstrcmp(dde->convInfo.ServerName, "") == 0)
										{
										EnableWindow(GetDlgItem(hWnd, IDEXECUTE), FALSE);
										}
								 else {
										EnableWindow(GetDlgItem(hWnd, IDEXECUTE), TRUE);
                    }
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetService(LPSTR name) {
							 if (lstrcmp(dde->convInfo.Service, name) != 0)
                 {
								 lstrcpyn((LPSTR)dde->convInfo.Service,(LPSTR) name, MAX_NAME -1 );
								 dde->convInfo.Service[MAX_NAME - 1] = '\0';
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

 	inline  BOOL SetTopic(LPSTR name) {
							 if (lstrcmp(dde->convInfo.Topic, name) != 0)
                 {
								 lstrcpyn((LPSTR)dde->convInfo.Topic,(LPSTR) name, MAX_NAME - 1);
								 dde->convInfo.Topic[MAX_NAME - 1] = '\0';
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetItem(LPSTR name) {
							 if (lstrcmp(dde->defItem, name) != 0)
                 {
								 lstrcpyn((LPSTR)dde->defItem,(LPSTR) name, MAX_NAME);
								 dde->defItem[MAX_NAME - 1] = '\0';
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetFormat(LPSTR name) {
							 if (lstrcmp(dde->defFormat, name) != 0)
                 {
								 lstrcpyn((LPSTR)dde->defFormat,(LPSTR) name, MAX_NAME);
								 dde->defFormat[MAX_NAME - 1] = '\0';
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetObjType(ATYPEID typeID) {
							 if (dde->defObjectType != typeID)
                 {
								 dde->defObjectType = typeID;
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetObject(AOBJECTID objID) {
							 if (dde->defReceiverOID != objID)
                 {
								 dde->defReceiverOID = objID;
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	inline  BOOL SetLinkType(int linkType) {
							 if (dde->defLinkType != linkType)
                 {
								 dde->defLinkType = linkType;
								 return TRUE;
								 }
							 else
                 return FALSE;
								 }

	
	int  SetDDEServiceList(HWND hWnd, BOOL bSet);				// This method will the combo box with service list
																											// that is built during XTYP_REGISTER and INITDIALOG
																											// bSet indicates whether the list has been added to or
																											// deleted from
	int  SetDDETopicList(HWND hWnd, AMEMBLOCKID txtID); 	// This method will probe DDEML to construct a
																											// list of available Topics for any chosen service
																											// If no service is chosen, this will return an empty list

	int  SetDDEItemList(HWND hwnd, AMEMBLOCKID dataID ); // This method will probe DDEML to construct a
																											// list of available Items for any chosen Topic
																											// If no Topic is chosen, this will return an empty list
	int  SetDDEFormatList(HWND, AMEMBLOCKID);

	int  GetSystemInfo(HWND hwnd);


	public:

	OBJDCLASS(HWND hWnd, AOBJECTID objID);  // HWND == Dlg Window handle
																				 // objID == OID of DDE Client Object

	~OBJDCLASS(void);

	UINT 					RunDDEServer(HWND hwnd, LPSTR name);

	void 					InitControlsFromDatas(HWND hWnd); 							// Init all the Dlg controls

	int  					BuildObjTypeList(HWND hwnd, ATOM formatID) ; 	// Build the object type list that support the chosen format;

	int  					BuildObjectList(HWND hwnd, ATYPEID sTypeID) ;		// Build the object list that support the chosen type

	int  					ChangeName(HWND hWnd, LPSTR name, int ctrlID);

	void 					SaveDatas(HWND hWnd);      										// write back into object datas

	int  					DDECTest(HWND hwnd);

	inline LPLONG IsDDE() { return (LPLONG)dde;} 				// check to see if dde is initialised


	friend void 	SetOptions(OBJDCLASS *objd, int ctrlID, long val);				// friend fucntion to set the CONV OPTIONS fields

	friend void 	SetOptionsStr(OBJDCLASS *objd, int ctrlID, LPSTR name); 	// friend fucntion to set the CONV OPTIONS fields

	friend long 	GetOptions(OBJDCLASS *objd, int ctrlID);									// friend fucntion to set the CONV OPTIONS fields

	friend LPSTR 	GetOptionsStr(OBJDCLASS *objd, int ctrlID);							// friend fucntion to set the CONV OPTIONS fields

	friend void 	SaveOptions(HWND hWnd, OBJDCLASS *objd);									// friend function to save the edited conv. context

	friend int		ALMCALLBACK ClientDlgProc(	HWND          hwnd,
																				register WORD 	msg,
																				register WORD 	wParam,
																				LONG          	lParam);

	friend int CALLBACK OptionsDlgProc (
  																			HWND          hwnd,
																  			WORD msg,
																  			WORD wParam,
																				LONG          lParam);

	};

LONG GetDlgItemLong(HWND hwnd, WORD id, BOOL *pfTranslated, BOOL fSigned);
VOID SetDlgItemLong(HWND hwnd, WORD id, LONG l, BOOL fSigned);




