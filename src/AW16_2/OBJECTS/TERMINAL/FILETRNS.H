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
// FILE:"filetrns.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include <windows.h>
#include <stddef.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_comm.h>
#include "com_dlg.h"
#include "macro.h"

extern HINSTANCE hInstance ;
//
//Structure for file transfer data
//
struct FTNSDATA0
{
    int       version ;
		OBJECTID  oiConn ;
		BOOL      Status ;       //Bar Status
		int 		  fileType ;
		int       flowCtrl ;
		DWORD     charDelay ;
		DWORD     lineDelay ;
		BOOL      wrap ;
		int       wrap_line ;
		int       lineEnd ;
		int       option ;
		int       maxTimeout ;
		int       maxTrytimes ;
    int       crc_mode ;
    BOOL      autodownload ;
} ;
struct PROTOCOLFRAME
{
  LPSTR lpszName ;
  int   nFrameID ;
  int   nFirstID ;
  int   nLastID ;  
} ;
//
//Class for the file transfer dialog box 
//
class FILETRANSFER{
	private:
  	FTNSDATA0 *data0 ;
    void InitDialog (HWND) ;
		void SaveDialog (HWND) ;
		void DoConfigure (HWND) ;
		void InitConfigure(HWND) ;
		//void SetProtocol(HWND, int) ;
		void ShowProtocolParam (HWND, int) ;
		void ShowAsciiParam (HWND) ;
		void ShowBinaryParam (HWND, int) ;
    void ShowZmodemParam (HWND) ;
		void ChangeProtocol(HWND) ;
		void SaveFileTransferCfg(HWND) ;
    void SaveAsciiParam(HWND) ;          
		void SaveXYKParam(HWND, int) ;
		void SaveZmodemParam(HWND) ;
    void VarifyProtocol(HWND) ;
	public:
		OBJECTID  oiFtns ;
		FILETRANSFER(OBJECTID) ;
	 ~FILETRANSFER(void) ;
		OBJECTID FltrGetConnObject (void){return (data0->oiConn);} ;
		void DoSignalIDs (pAObjMessage) ;
		void DoValidateObjectIDs(pAObjMessage) ;
		void DoCheckConnObj  (OBJECTID) ;
    BOOL IsAutoDownlaod (void) { return(data0->autodownload);} ;

		friend BOOL ALMCALLBACK FileTransferDlg (HWND, WORD, WORD, LONG) ;
		friend BOOL ALMCALLBACK FileTransferCfg (HWND, WORD, WORD, LONG) ;
    friend class FTCLASS ;
    friend class TEXTFTCLASS ;
    friend class XMODEMCLASS ;
    friend class YMODEMCLASS ;
    friend class KERMITCLASS ;
} ;
//
//Class for the status bar
//
class STATUSCLASS
{
	public:

		HWND hStatus ;
		char *filename ;
		long filesize ;
		long blocksize ;
		long blockno ;
		long error ;
		char protocol[30] ;
    BOOL writeName ;
		BOOL bBar ;
    BOOL transferStop ;

		STATUSCLASS(void) ;
		~STATUSCLASS (void) ;
		void UpdateStatusWindow (void) ;
		BOOL InitStatusWindow (int, BOOL, char*, char*, long) ;
		void ShowStatusWindow (int) ;
		void ShowContent (HWND, BOOL) ;
		void ShowStatusBar (HWND) ;
    void UpdateFileSize (long newsize) {filesize=newsize;} ;
   
} ;
//
//Base class for the file transfer
//
class FTCLASS
{
  public:
		OBJECTID     oiFT ;
		OBJECTID     oiConn ;
    int          fltr_state ;
		BOOL         sendOrReceive ;
    long         maxTimeout ;
  	int          maxTrytimes ;
    int          lErrorCount ;
		HFILE        hFile ;
		long         filesize ;
		BOOL         Status ;
    char         receiveChar ;
		int          fileType ;
		int          flowCtrl ;
    int          handshake ;
		int          nTransferResult ;
		HANDLE       hBuffer ;
		LPSTR        lpBuffer ;
		HANDLE       hReadBuf ;
		LPSTR        lpReadBuf ;
		STATUSCLASS *lpStatus ;
    BOOL         transferCancelled ;

		FTCLASS(OBJECTID, char *, BOOL) ;

		virtual ~FTCLASS() ;
		virtual int Initiation(BOOL) ;
		virtual int ReadCharsToBuffer (void){return 0;} ;
		virtual int SendFile (void) ;
		virtual int ReceiveFile (long) ;
		virtual int SendBinaryFile (void) ;
    virtual void StartProtocol (OBJECTID, int){} ;
    virtual int  HandleBufferData (void){return 1;} ;
    virtual int  ProcessBufferData (void){return 1 ;} ;
    int ReadCommToBuffer (void) ;
		BOOL IsPortOpened (void) ;
		int  WriteChar (char) ;
		int  ReadPort (void) ;
		int  CancelFile (void) ;
    void CancelFileTransfer (void) { transferCancelled= TRUE;} ;
		void Delay (DWORD) ;
		 
} ;
//
//Detrive class for the Ascii file transfer
//
class TEXTFTCLASS: public FTCLASS
{
	public:
  char* filename ;
	long  charDelay ;
	long  lineDelay ;
	BOOL  wrapline ;
	int   maxwrapline ;
	int   lineEnd ;
  int   numChars ;
	int   charcounts ;
	long  totalChars ;
	//BOOL  enablepause ;

	TEXTFTCLASS(OBJECTID, char *, BOOL) ;
 ~TEXTFTCLASS() ;
	int  Initiation (BOOL) ;
  void StartProtocol (OBJECTID, int) ;
  int  ProcessBufferData (void) ;
  void CheckResult (int) ;
	int  CheckXoff(void) ;
	int  AddLineEnd(void) ;
	int  SendFile(void) ;
	int  ReceiveFile(long) ;
	int  WriteBytesToFile(void) ;
	int  CancelFile (long) ;
} ;
//
//Detrive class for the XMODEM Protocol
//
class XMODEMCLASS :public FTCLASS
{
	public:
	char* filename ;
	long  filesize ;
	int   option ;
	int   maxblocksize ;
	int   nTransSize ;
	BYTE  bSeq ;
	BYTE  bLastSeq ;
	BOOL  fCRC ;
	int   waittimes ;
	BOOL  startstatus ;
	LPSTR rvData ;
	int   rvDataCount ;

	XMODEMCLASS(OBJECTID, char *, BOOL) ;
 ~XMODEMCLASS() ;
  virtual void StartProtocol (OBJECTID, int) ;     
          int  ProcessBufferData (void) ;  
          int  HandleBufferData (void) ;  
	virtual int  Initiation(BOOL) ;
	virtual int  WaitFirstChar (void) ; //SendBinaryFile (void) ;
					int  GetFirstChar(void) ;
					int  WaitACK (void) ;
					int  WriteDataBlock (void) ;
					void CancelSendBinaryFile (void) ;
	virtual void EndSendBinaryFile (void) ;
					int  ResendDataBlock (void) ;
					int  CloseBinaryFile (void) ;
					int  TimeOut (void) ;
					int  LocalCancel (void) ;
					int  RemoteCancel (void) ;
	virtual int  ReceiveBinaryFile(void) ;
					int  SendInitChar (void) ;
	virtual int  WaitSOH () ;
	virtual int  GetSOHChar () ;
					int  WaitSEQChar (void) ;
	virtual void ReceiveDataBlock (void) ;
	virtual int  CheckDataBlock (void) ;
          int  WriteToFile (void) ;
					int  SendACK (void) ;
	virtual int  EndTransferBinaryFile (void) ;
	virtual BOOL EndOrContinueSendBinaryFile (void){return TRUE ;} ;

	BYTE CheckSum( LPCSTR ) ;
	unsigned int CalcCRC16( LPCSTR, long ) ;

} ;
//
//Detrive class for the YMODEM Protocol
//
class YMODEMCLASS:public XMODEMCLASS
{
	public:

			char*  filename ;
			char*  directory ;
			int    fileCount ;
			long   length ;
			long   moddate ;
			BOOL   dataBlock0 ;

			YMODEMCLASS (OBJECTID, char *, BOOL) ;
			~YMODEMCLASS () ;
			int  Initiation(BOOL) ;
      void StartProtocol (OBJECTID, int) ;
      int  ProcessBufferData (void) ;
			int  WaitFirstChar(void) ; //SendBinaryFile (void) ;
			int  WriteHeaderBlock(void) ;
      int  WaitFinalACK (void) ;
			void EndSendBinaryFile(void) ;
			int  EndOrContinueSendBinaryFile(void) ;
			int  ReceiveBinaryFile (void) ;
			int  WaitSOH (void) ;
      int  GetSOHChar (void) ;
			int  EndTransferBinaryFile (void) ;
			void ReceiveDataBlock (void) ;
			int  CheckDataBlock0 (void) ;
			int  CheckDataBlock (void) ;
} ;
//
//Detrive class for the KERMIT Protocol
//
class KERMITCLASS:public FTCLASS
{
	public:
    int   receive_state ;
    int   packetlen ;
    int   rvDataCount ;
		char* sndpkt ;
		char* rcvpkt ;
		char* rdatap ;
		char* filename ;
		char* directory ;
		int   seq ;
    int   datalen ;
    int   blockno ;
		long  packno ;

    int  parity ;
		int  max_length;
    int  qbin;
		int  text_mode;
		char time_out;
    char pad_count;
    char pad_char;
    char eol;
    char quote_char;
    char his_qbin_char;
    char my_qbin_char;
    char repeat_flag;
    char his_repeat_char;
    char my_repeat_char;
    char his_capabilities;
    char strip_path_from_filename;
		int  subdirectory_flag;

		long start_time ;
		char type ;
		char nextType ;
		int  data_length ;
		int  block_size ;
		char *buffer ;
		char *resendBuf ;

		KERMITCLASS(OBJECTID, char *, BOOL) ;
		~KERMITCLASS () ;

    void StartProtocol (OBJECTID, int) ;     
    int  ProcessBufferData (void) ;  
    int  HandleBufferData (void) ; 
		int  Initiation (BOOL) ;
		int  SendPacket(char) ;
    int  InitPacket (char) ;
		int  InitSPacket( void) ; //char data[ 10 ] )
		int  InitFPacket(void) ;
    int  InitOtherPacket (void) ;
		int  InitDPacket( void ) ;
		int  ReadFileToBuffer( char * ) ;
		int  WaitACKPacket (void) ;
		int  ReceivePacket( void );
		int  ReadPort(char*, int, char, long) ;
		int  ResendPacket (void) ;
		void EndFileTransfer (void) ;
		int  FinalZPacket (void) ;
		int  ReceiveBinaryFile (void) ;
		int  GetInitPacket (void) ;
		void LoadInitParameters( void ) ;
    int  SendACKPacket (void) ;
		int  SendInitialAck( void ) ;
		int  SendACKorNAK (char) ;
		int  GetPacket (void) ;
		int  GetFileName (void) ;
		int  SaveDataBlock (void) ;
		int  CloseFile (void) ;
		int  SaveBuffer( void ) ;
		void PostEvent(OBJECTID, EVENTID, long, long) ;
		int  ChecksumCalculate( BYTE *buffer ) ;
		int  ReadFileByte( int *c ) ;
		int  WriteFileByte( int c ) ;
} ;

extern void SetTimerEvent (OBJECTID, const EVENTID, long) ;
extern void KillTimerEvent (OBJECTID, const EVENTID) ;
extern void RestartTimerEvent (OBJECTID, long) ;
