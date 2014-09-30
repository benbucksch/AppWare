/*//////////////////////////////////////////////////////////////
//
//   		Copyright 1993, Novell, Inc.  All rights reserved
//
//      THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//      PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//      ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//      A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//      THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//      ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//      WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//      REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//      COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//      WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//      EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//      THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR: Shaili Jain
//
// DESCRIPTION:  March 21, 1994
//
// CHANGES:
//
// 
//////////////////////////////////////////////////////////////*/

#include <windows.h>
#include "resource.h"

class OBJDCLASS;

#define OTYPE_SIGS                  MAKETYPEID('c','a','S','e')

#define MAXNAME                     100
#define SIGS_DATAVERSION         		2     // Version 1 -> Old Signal IDs
																					// Version 2 -> New Signal IDs ('0' based)
#define SIGS_DLLVERSION           	1			

#define MAXSIGNALDATAS             	2     // dat#1.         SIGSDATA

#define data0_SIGSDATA              0
#define data1_DEFVALS             	1

// SIGNAL defines
#define SIGS_NUMOFSIGS              2

#define ISIG_OLDDEFAULT          		1
#define ISIG_DEFAULT            		0				// 1
#define SIGSZ_DEFAULT        				"None"

#define ISIG_OLDANY              		2
#define ISIG_ANY                		1       // 2
#define SIGSZ_ANY             			"Any"


// strings for upsf scopes
#define SIGNALSDATASCOPE						"sigsData"
#define SIGNALSCOPE									"signal"

#define SIGS_SAVESIGS               0x0001
#define SIGS_SAVEOBJD               0x0002
#define SIGS_SAVEOPTIONS            0x0004
#define SIGS_FLAGS                  0x0007

#define SIGS_NEWITEM                1
#define SIGS_NOTINIT                0   
#define SIGS_SIGNALNOTFOUND         -1



#define INVALIDOID                                                                      0

#define OBJECTINSTDATA              MAXSIGSERVERDATAS  // slot to save c++ object instance

#define SZSIGS_SIGSALLIST           "SIGSals"



//Flags for alha, non-alpha, updated signals etc June 4, 1994
#define		SIG_ISALPHA					0x01
#define		SIG_ISINT						0x00
#define		SIG_ISUPDATED				0x10


class  SIGS;


/* ================================================================ 
Struct to hold used defined SIGSal info.  It is used during
OBJD
================================================================ */

struct SIGNALINFO {
	char      	name[MAXNAME];
	long      	sigID;
	BYTE        bisAlphaSignal;
	};

// Update signal info --> used to match signal IDs
struct UPDATESIGNAL {
	char oldName[MAXNAME];
	char updName[MAXNAME];
	};

typedef UPDATESIGNAL *LPUPDATESIGNAL;

// first data of SIG Server Object
struct SIGSDATA {
  long        version;          // version number
	OBJECTID    sigsOID;
	DWORD      	idInst;
	int         nUserSignals;     // number of user SIGSals
	BOOL     		isAlphaSignal;                          // if any signal is a string
	SIGNALINFO  signals[];        // array of SIGSals
	};



struct DEFOBJDDATA {
	SIGNALINFO 	defSignal;
	long       	maxSignalID;     // SKJ added Mar. 7 to track the SIGSals Used
	BYTE     		extraBytes[8];   // Extra 8 bytes
	};

typedef SIGSDATA       *LPSIGSDATA ;
typedef DEFOBJDDATA    *LPDEFOBJDDATA;
/****** Structrues ******/


/////////////////////////////////////////////////////////
// SIGS class defintition
////////////////////////////////////////////////////////

class SIGS {
	private:
	int         m_nUserSignals;               // user defined SIGSals off this Object
	SIGNALINFO  *m_userSignals;               // user defined SIGSals
	long        GetSignal(LPSTR szExec);

	// this class implements the interface to the object Edit dlg box
	friend      OBJDCLASS; 
	int         IsNewSignal(SIGNALINFO defSig);
	long      	m_maxSignalID;      // added to keep track of used SIGSals
	BOOL   			SignalExists(LPSTR SIGName);
	BOOL  			AdjustUserSignalIDs();
	MEMBLOCKID  m_upListID;
  /*********************************PUBLIC FIELDS******************************************/

  public:
	DWORD    		m_idInst;
	// default attributes
  // used as def. parameters in OBJD dlg box
	OBJECTID    m_sigsOID;                // SIGS object
  
	BOOL        m_bDisplayErr;
	BOOL     		m_isAlphaSignal;   // if there are any alhs signals
	int         m_bSave;
	// stored in data1_OPTIONS
	void        Createhsz();
  void        Destroyhsz();

	MEMBLOCKID  m_XMemID;               // block to hold the array of XCTIONINFO structures
	SIGS(OBJECTID objID);             // constructor
	~SIGS();                          // destructor

	int         SaveSignalsToData();          // save the SIGSsls to datas
	int         BuildSignalsFromData();
	BOOL                            CheckSignals();               // check for errors in SIGSals
	BOOL        RemoveSignal(int delIndex);

	void       	Save(BOOL val)                  {m_bSave = val; }
	int        	SetSave(int bVal)       {m_bSave |= bVal; return m_bSave;}
	int        	ResetSave(int bVal) {m_bSave &= ~bVal;return m_bSave; }
	int       	PostSignal(long theSignal);
	LONG       	GetSignal(OBJECTID expObj);
	BOOL       	SetIsAlphaSignal();


	friend 			void SaveOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **oldSig);
	friend     	void ResetOldSigs(OBJDCLASS *objd, int *nOldSigs, SIGNALINFO **oldSig);
	};


///////////////////////////////////////////////////////////

// GLOBALS
#ifdef SIGSMODULE
	HINSTANCE 							hInst;       // instance of the dll
#else
	extern HINSTANCE        hInst;  // instance of the dll
#endif

// FUNCTIONS
BOOL 					InitApplication(HANDLE);
BOOL          InitInstance(HANDLE, int);
int FAR       DoDialog(LPCSTR lpTemplateName, FARPROC lpDlgProc, DWORD param);

void        	Hszize(void);
void        	UnHszize(void);



