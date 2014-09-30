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
	short int BaudRate ;
  short int DataBits ;
  short int StopBits ;
  short int Parity ;
	short int handshake ;
  short int Ports ;
  BOOL     	ChkParity ;
	BOOL    	CrDetect ;
} ;
//
//Modem Settings
//
struct MODEM{
	BOOL      AnsOrDial ;
  short int RingTimes ;
	short int PhoneNumberLen ;
  short int RedialTimes ;
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
	OBJECTID FoundString ;
	OBJECTID FoundIndex ;
  OBJECTID InteText ;
} ;
//
//For signal Information
//
struct SIGNALINFO{
	WORD 			signalIndex ;
	WORD      signalLen ;
	BYTE      CompareBits ;
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
		void InitDataBlock0 (OBJECTID theObject) ;
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
		OBJECTID theObject ;
		BOOL     connbusy ;

		CONN (OBJECTID) ;
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
		MEMBLOCKID  hSignalInfo ;
		MEMBLOCKID  hSignalBlock ;

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
		EDIT_SIGNAL(OBJECTID) ;
	 ~EDIT_SIGNAL() ;
		friend BOOL ALMCALLBACK ConnectionDialog (HWND, WORD, WORD, LONG) ;
} ;

/*struct LINE{
	UINT	line_break ;
	UINT  clear_to_send ;
	UINT  data_set_ready ;
	UINT  carrier_detect;
	UINT	data_terminal_ready;
  UINT	ready_to_send ;
	UINT	ring_indicator ;
  UINT  delta_carrier ;
  UINT  delta_ring ;
  UINT  delta_cts ;
  UINT  delta_dsr ;
  UINT  got_break ;
  UINT	parity ;
  UINT	framing ;
  UINT	overflow ;
	UINT	overrun ;
} ; */
//
//COMMUNICATION class for the communication's running time 
//
class COMMUNICATION{
	public:
		OBJECTID commObj ;
		BOOL     connbusy ;
		BOOL     listenFlag ;
		DATA0*   data0 ;
		int      inbuf;
		int      outbuf ;
		int      idCommDev ;
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

		COMMUNICATION(OBJECTID, int, int) ;
	 ~COMMUNICATION(void) ;
		void SetActiveSearchTable (void) ;
		int  InitComm(void) ;
		int  CommOpen (void) ;
		int  CommClose (void) ;
		int  ChangeCommState (void) ; //void ChangeCommState (void) ;
		int  SetBaudRate (int) ;
		int  SetParity (int) ;
		int  SetStopBits (int) ;
		int  SetHandShake(int) ;
		int  GetParity (void) ;
		int  GetStopBits (void) ;
		int  GetHandShake(void) ;
		int  SetXonXoff (int) ;
		int  LineBreak (int) ;
		int  RxCount (void) ;
		int  TxCount (void) ;
		int  FlushQueue (int) ;
		int  GetBuffer( BYTE*, int ) ;
		int  GetString (BYTE*, int) ;
		int  GetChar( void ) ;
		int  WaitChar (long) ;
		int  PutBuffer( const BYTE*, int ) ;
		int  PutChar( BYTE ) ;
		int  PutString( const BYTE * ) ;
		int  GetPauseStatus (void) ;
    void SetAnsOrDialParam (BOOL) ;
		int  SetEvent( UINT ) ;
		BOOL GetPhoneNumber (char*) ;
    int  Dial (void) ;
		int  ModemInit (void) ;
		int  ModemDial(char*) ;
		int  ModemRedial(void) ;
		int  ModemHangUp (void) ;
		int  ModemAutoAnswer (BOOL) ;
		int  ResetModem(void) ;
    void DoCheckModemResult(void) ;
		int  SetResultCode (int) ;
		int  GetResultCode(void) ;
		BOOL ListenACall(void) ;
		void IdleSystem(int) ;
		void ConvertString (char*) ;
		BOOL LinkOpen (void) {return (idCommDev>=0?TRUE:FALSE) ;} ;
		void SetFoundString (LPSTR) ;
    void SetFoundIndex (WORD) ;
		int  ReadPortToMem(void) ;
		void DoIdle (void) ;
		void SetInteText() ;
		void DoSearch() ;
		void DoListen() ;
		void DoTerminal() ;

    friend BOOL ALMCALLBACK AskPhoneDialog(HWND, WORD, WORD, LONG) ;
} ;
long ALMCALLBACK EditProc (HWND, WORD, WPARAM, LPARAM) ;
