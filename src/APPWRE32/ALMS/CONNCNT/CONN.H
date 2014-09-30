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
// FILE:"conn.h"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
//
//Port settings
//
struct PORT
{
  DWORD BaudRate ;
  BYTE  DataBits ;
  BYTE  StopBits ;
  BYTE  Parity ;
  int   handshake ;
  int   Ports ;
  BOOL  ChkParity ;
  BOOL  CrDetect ;
} ;
//
//Modem Settings
//
struct MODEM{
	BOOL      AnsOrDial ;
  int RingTimes ;
  int PhoneNumberLen ;
  int RedialTimes ;
  int       seconds ;
  BOOL			Redial ;
	int     	DialMethod ;
} ;
//
//Signal Settings
//
struct SIGNAL{
	BYTE		 viewMode ;
	BYTE		 checkBox ;
	int      signalCount ;
	AOBJECTID FoundString ;
	AOBJECTID FoundIndex ;
  AOBJECTID InteText ;
} ;
//
//For signal Information
//
struct SIGNALINFO{
	int		signalIndex ;
	int     signalLen ;
	int     CompareBits ;
	BOOL      active ;
} ;
//
//Data block structure
//
struct DATA0{
	int      version ;
	BOOL     checkModem ;
	PORT     port ;
	MODEM    modem ;
	SIGNAL   signal ;
} ;
//
//CONN class for the Connection object Dialog Box
//
class CONN
{
	private:
		void InitDataBlock0 (AOBJECTID theObject) ;
		void InitDlg (HWND hDlg) ;
		void SaveDlg (HWND hDlg) ;
		void ConfigureDialog (HWND hwnd)  ;
		void SetInterceptText (HWND) ;
		void InitCntDialog (HWND) ;
		void SelectModem (HWND) ;
		void ModemSettings (HWND) ;
		void SaveCntConfigure (HWND) ;
		void InitModemSettings (HWND) ;
		void ChangeModemSettings (HWND hDlg) ;
		void SaveModemParams (HWND) ;
		//void ExportConnDataBlock0 (void) ;
		//void ExportPhoneNumber (void) ;
		//void ExportSignalDataBlock(void) ;
		//void ImportConnDataBlock0 (void) ;
		//void ImportPhoneNumber (void) ;
		//void ImportSignalDataBlock(void) ;

	public:
		DATA0*   data0 ;
		AOBJECTID theObject ;
		BOOL     connbusy ;

		CONN (AOBJECTID) ;
	 ~CONN (void) ;
		void ExportObject();
		void ImportObject();
		long doCheckVersion (pAObjMessage) ;
		void doValidateObjectIDs(pAObjMessage) ;
		void doSignalIDs(pAObjMessage) ;
		void doCheckObjectIDs (void) ;
		BOOL IsModem(){return(data0->checkModem);} ;

		friend BOOL ALMCALLBACK ModemSettingsProc(HWND, WORD, WORD, LONG) ;
		friend BOOL ALMCALLBACK CntObjProc(HWND, WORD, WORD, LONG) ;
} ;
//
//EDIT_SIGNAL class for editing the signal dialog box
//
class EDIT_SIGNAL : public CONN
{
	private:
		BYTE        currentView ;
		char        marktable[2] ;
		SIGNAL     *signal ;
		AMEMBLOCKID  hSignalInfo ;
		AMEMBLOCKID  hSignalBlock ;

		void SetDisplayText (HWND, LPSTR) ;
		void ChangeViewMode (HWND, LONG) ;
		void SetComboxContent (HWND, int) ;
		void SetSignalCheckButton(HWND) ;
    int  FindAddPosition (long) ;
		int  UpdateSignalBlock (char*, int) ;
    void UpdateSignalInfo (HWND, int , long) ;
		BOOL InitSignalDialog(HWND) ;
		void SetSignalList (HWND) ;
		void AddStringToList(HWND) ;
		void ChangeStringInList(HWND) ;
		void RemoveStringsFromList (HWND) ;
		void SaveCompareBits (HWND, int) ;
		void ActivateSignal (HWND, int) ;
		void DisplayString (HWND) ;
		void ConfigureDialog (HWND) ;
		BOOL ChangeSignalCheckButton (HWND, int) ;
		void ChangeComboBoxContent (HWND, int) ;
		void SaveSignalString (void) ;

 public:
		EDIT_SIGNAL(AOBJECTID) ;
	 ~EDIT_SIGNAL() ;
		friend BOOL ALMCALLBACK ConnectionDialog (HWND, UINT, WPARAM, LPARAM);
} ;

//
//COMMUNICATION class for the communication's running time 
//
class COMMUNICATION
{
	public:
		AOBJECTID commObj ;
		DATA0*   data0 ;
		DCB      commDCB ;
		HANDLE   hCommDev ;
		BOOL     portOpened ;
		BOOL     listenFlag ;
		int      inbuf;
		int      outbuf ;
		int      nCommError ;
    int      redialTimes ;
		UINT     currentEvents ;
    BOOL     checkResultCode ;
    HANDLE   hResultCode ;
		HANDLE   hCommMem ;
		LPSTR    lpCommMem ;
    HANDLE   hResult ;
    LPSTR    resultStr ;
		HANDLE   hSearch ;
		LPSTR    searchStr ;
		HANDLE   hIndex ;
    HANDLE   hActiveStr ;

		COMMUNICATION(AOBJECTID, int, int) ;
	 ~COMMUNICATION(void) ;
		void   SetActiveSearchTable (void) ;
		int    InitComm(void) ;
		HANDLE CommOpen (void) ;
		int    CommClose (void) ;
		BOOL   UpdateCommState (void) ;  
		BOOL   SetBaudRate (DWORD) ;
		BOOL   SetByteSize (BYTE) ;
		BOOL   SetParity (BYTE) ;
		BOOL   SetStopBits (BYTE) ;
		BOOL   SetHandShake(int) ;
		DWORD  GetBaudRate (void){return (commDCB.BaudRate) ;} ;
		BYTE   GetByteSize (void){return (commDCB.ByteSize) ;} ;
		BYTE   GetParity (void){return (commDCB.Parity) ;};
		BYTE   GetStopBits (void){return (commDCB.StopBits) ;};
		int    GetHandShake(void) ;
		int    SetXonXoff (int) ;
		int    LineBreak (int) ;
		DWORD  RxCount (void) ;
		DWORD  TxCount (void) ;
		BOOL   FlushQueue (int) ;
		DWORD  GetBuffer( BYTE*, DWORD) ;
		DWORD  GetString (BYTE*, DWORD) ;
		DWORD  GetBufferTimeout (BYTE*, DWORD, DWORD) ;
		int    GetChar(void) ;
		int    GetCharTimeout(DWORD) ;
		DWORD  PutBuffer( const BYTE*, DWORD ) ;
		DWORD  PutBufferTimeout (BYTE*, DWORD, DWORD) ;
		int    PutChar( BYTE ) ;
		DWORD  PutString( const BYTE * ) ;
	 	int    PutCharTimeout (BYTE, DWORD) ;
		BOOL   ClearCommTimeouts(void) ;
    void   SetAnsOrDialParam (BOOL) ;
		int    SetEvent(DWORD) ;
		BOOL   GetPhoneNumber (char*) ;
    int    Dial (void) ;
		int    ModemInit (void) ;
		int    ModemDial(char*) ;
		int    ModemRedial(void) ;
		int    ModemHangUp (void) ;
		int    ModemAutoAnswer (BOOL) ;
		int    ResetModem(void) ;
    void   DoCheckModemResult(void) ;
		int    SetResultCode (int) ;
		int    GetResultCode(void) ;
		BOOL   ListenACall(void) ;
		void   IdleSystem(int) ;
		void   ConvertString (char*) ;
		void   SetFoundString (LPSTR) ;
    void   SetFoundIndex (int) ;
		int    ReadPortToMem(void) ;
		void   DoIdle (void) ;
		void   SetInteText() ;
		void   DoSearch() ;
		void   DoListen() ;
		void   DoTerminal() ;

    friend BOOL ALMCALLBACK AskPhoneDialog(HWND, WORD, WORD, LONG) ;
} ;
long ALMCALLBACK EditProc (HWND, WORD, WPARAM, LPARAM) ;
