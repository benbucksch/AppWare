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
#define SIGS_TOPICNOTFOUND 	-1
#define OEMCODE	            GetKBCodePage()



#pragma  argsused   // suppress args used warnings
extern int _WinAllocFlag;
char szWild[] = "*";    // used to indicate wild names ("" is also cool)
// static char szT[MAX_NAME];     // temp buf for munging names.

class SIGS;

typedef char LPNAME[MAX_NAME];

// a set oif bits that indicate whether we can add the topic,service, item names to main list
#define SIGS_ADDEDSERVICE   	0x0001			// 0000000000000001
#define SIGS_ADDEDTOPIC      	0x0002  		// 0000000000000010
#define SIGS_ADDEDITEM        0x0004    	// 0000000000000100
#define SIGS_ADDEDFORMAT      0x0008      // 0000000000001000
#define SIGS_ADDEDOBJECT      0x0010      // 0000000000010000
#define SIGS_ADDEDALL					0x001F      // 0000000000011111

// a set of bits that indicate whether we can add the Signal name and oid to signal List
#define SIGS_ADDEDSIGNAL   	 	0x0001			// 0000000000000001
#define SIGS_ADDEDSIGOBJECT   0x0002  		// 0000000000000010
#define SIGS_CANADDSIGNAL			0x0003      // 0000000000000011

#ifdef SIGOBJD
// Class definition to be used during OBJD time.
class OBJDCLASS {

	private:
	int 		SetMainCtrls(int index);
	int 		SetMainList(HWND hWnd, LPSTR itemStr, AOBJECTID objID);
	int 		SetSignalList(LPSTR itemStr);
	// Build the object type list that support the chosen format;
	int  		BuildFormatListForSignals();
	BOOL 		SetSignal(LPSTR name);
	BOOL		KeepUpdatedSignals(LPSTR newName, LPSTR oldName); // store the names of update signals

	protected:

	public:

	AOBJECTID	oid;
	SIGS 		*sigs; // an instance of class that provides all the signal
	HWND 		hDlg;
	BOOL 		bOK;
	int 		bCanAdd;
	SIGNALINFO	defSignal;

					OBJDCLASS(HWND hWnd, AOBJECTID objID);
					~OBJDCLASS(void);

	int 		SetAddFlag(int bVal) 	{ bCanAdd |= bVal; return bCanAdd;}
	int 		ResetAddFlag(int bVal) { bCanAdd &= ~bVal;return bCanAdd; }

	int 		ChangeName(LPSTR name, int ctrlID);
	BOOL 		GetFromMainList(LPSTR Topic,LPSTR	Item,LPSTR Format,AOBJECTID *ObjectID);
	// methods on Signal list

	int			CreateSignalsFromMainList(); // Create a list of signals based on items in MAIN list
                                       // called during IDOK handling
	void 		BuildSignalList();
	int  		AddSignalList(SIGNALINFO signalInfo, BOOL bUpdated);
	int 		DeleteSignalList(LPSTR pszSignal);
	int  		UpdateSignalList();

	BOOL 		GetFromSignalList(SIGNALINFO *curSignal);
	BOOL 		InitSigCtrls();
	BOOL 		SetCtrlsFromSignalList();
	// Process different attributes's dialog
	int 		ProcessSubDialog(int dlgType);

	};

LONG GetDlgItemLong(HWND hwnd, WORD id, BOOL *pfTranslated, BOOL fSigned);
VOID SetDlgItemLong(HWND hwnd, WORD id, LONG l, BOOL fSigned);

#else
class OBJDCLASS;
#endif


