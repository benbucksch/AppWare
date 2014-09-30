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
// FILE:"terminal.h"
//
// AUTHOR: Sibai Li
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
#include <o_wnd.h>
#include <o_text.h>
#include <o_comm.h>
#include <intdebug.h>
#include "macro.h"

extern HINSTANCE hInstance ;
//
//Terminal Data Block Structure 
//
struct TERMDATA0{
		int      version ;
		OBJECTID oiConn ;
		BOOL     cacheText ;
		OBJECTID cacheTextObj ;
		int      Emulation ;
		BYTE     Mode ;
    int      curShape ;
    BYTE     CRorLF ;
		int      columns ;
    int      bufferSize ;
		BYTE     scrollBar ;
		BOOL     activate ;
		LOGFONT	 logFont ;
 		int      translate ;
} ;
//
// Terminal Class For Design time
//
class TERMINAL{

	private:
		TERMDATA0* data0 ;
		void InitTerminalDialog (HWND) ;
		void SetConnectionObj (HWND) ;
		void SetCacheTextObj (HWND) ;
		void CacheText (HWND) ;
		void TerminalConfigure (HWND) ;
		void SaveTerminal (HWND) ;
		void InitTerminalConfiguration(HWND) ;
		void SaveTerminalConfiguration (HWND) ;
		void EditTerminalFont (HWND) ;
	public:
		OBJECTID   oiTerm ;

		TERMINAL(OBJECTID) ;
	 ~TERMINAL(void) ;
   	OBJECTID GetConnObject (void) {return (data0->oiConn); } ;
		BOOL     GetCacheText(void){return(data0->cacheText);} ;
		OBJECTID GetCacheTextObj(void) {return (data0->cacheTextObj);} ;
		int      GetEmulationType (void){return (data0->Emulation);} ;
		BYTE     GetModeType(void) {return(data0->Mode);};
		int      GetCurShape (void) {return (data0->curShape);};
		BYTE     GetCRorLF (void) {return (data0->CRorLF);} ;
		int      GetColumns (void) {return (data0->columns);} ;
		int      GetBufferSize (void) {return (data0->bufferSize);} ;
		BYTE     GetScrollBar (void) {return (data0->scrollBar); } ;
		LOGFONT  GetLogFont (void) {return (data0->logFont); } ;
    BOOL     GetActivate (void) {return (data0->activate); } ;
		long     CheckVersion (pAObjMessage) ;
		void     ExportObject (void) ;
		void     ImportObject (void) ;
		void     DoValidateObjectIDs (pAObjMessage) ;
		void     DoCheckConnObj (OBJECTID) ;
		friend BOOL ALMCALLBACK TerminalDialog (HWND, WORD, WORD, LONG) ;
		friend BOOL ALMCALLBACK TerminalConfigureDialog (HWND, WORD, WORD, LONG) ;
} ;
//
//RUNTMNL Class For Run time
//
class RUNTMNL //: public TermAttr
{
	public:
		OBJECTID   oiConn ; //add Feb 16
		HWND       hwnd ;           
		HFONT      hFont ;
		WORD   	   dwTextColor;
		WORD       dwBkGndColor;
		HBRUSH     hBkBrush;		
		int        curShape ;

		int        charWidth ;
		int        charHeight ;
		int        cxClient ;
		int        cyClient ;
		int        vMaxCols ;
		int        vMaxRows ;
		int        nCurrCol ;
		int        nCurrRow ;
		int        caretCurrCol ;
		int        caretCurrRow ;

		FARPROC    DefProc ; //Add
		OBJECTID	 oiTerm ;
		BOOL       capture ;
		HFILE      hCaptureFile ;
		HWND       hParent ;
		HGLOBAL    hBuffer ;
		HGLOBAL     hAttr ;
		LPSTR      termBuffer ;
		LPSTR      attrBuffer ;

		//int        emulation ;
		BOOL       bLineWrap ;
		BOOL       bEcho ;
		WORD       currAttr ;
		WORD       bMaxCol ;
		WORD       bMaxRow ;
		WORD       bMaxSize ;
		WORD       bCurrCol ;
    WORD       bCurrRow ;
		WORD       firstVisibleCol ;
		int        firstVisibleRow ;
		WORD       saveCol ;
		WORD       saveRow ;
		int        nVscrollMax ;
		int        nVscrollPos ;
		int        nHscrollMax ;
		int        nHscrollPos ;
		RECT       rect ;
		TERMDATA0* data0 ;

		RUNTMNL (OBJECTID) ;
	 ~RUNTMNL () ;

	 	void GetSize (HWND, LPARAM) ;
		void SetupTmnlCursor (HWND) ;
		void DestroyTmnlCursor(HWND hwnd) ;
		//HDC  SetupDC (HWND) ;
		void SetScrollBar (HWND) ;
		void DoVscroll (WPARAM, LPARAM) ;
		void DoHscroll (WPARAM, LPARAM) ;
		BOOL ChangingTerminal (long, long) ;
		void RepaintTerminal (void) ;
		BOOL ChangeConfigure (void) ;
    BOOL OpenCaptureFile(HFILE) ;
    BOOL CloseCaptureFile(void) ;
		BOOL ClearTerminal (void) ;
		BOOL ResetTerminal (void) ;
		BOOL ReceiveText (OBJECTID) ;
		//void Activate (HWND) ;
    void SetupFocus (HWND) ;
		BOOL CheckPort (void) ;
		//void KeyDown (int) ;
		void ReturnKey (void) ;
		void TabKey (void) ;

		void Paste(void) ;
		BOOL ScrollBufferChars (void) ;
		void ScrollScreenChars (void) ;
		void WriteToCaptureFile (LPSTR) ;
		void WriteToCaptureText (LPSTR) ;
		void MoveTo (int, int) ;
		void SaveCursorPos (int, int) ;
		void RestoreCursorPos (void) ;
		void EraseScreen (int,int) ;
		void EraseFromCursorToEnd (int, int) ;
		void EraseFromStartToCursor (int, int) ;
		void ClearToEndOfLine (int, int) ;
		void ClearToStartOfLine (int, int) ;
		void ClearLine (int,int) ;
		void InsertLine (int, int) ;
		void DeleteLine (int, int) ;
		void DeleteChar (int, int) ;
		void ScrollUp (void) ;
		void ScrollDown (void) ;
		void SetCurrAttr (WORD fg, WORD bg) ;
		void DisplayChar (char) ;
		//BOOL canActivate () {return (data0->activate) ;} ;
    void PutString (char*) ;
		virtual void far PutChar (char) ;
		virtual int far RedrawTerminal (void) ;
		virtual void KeyDown (int){} ;
		friend long ALMCALLBACK TmlProc (HWND, WORD, WORD, LONG) ;
} ;
//
//Ansi Type Terminal class
//
class AnsiTerm : public RUNTMNL
{
	public:
		int        nTermState ;
		int        nNumChars ;
		char       nParams[MAX_PARAMS] ;
		int        nCurrParam ;

		AnsiTerm(OBJECTID) ;
		~AnsiTerm() ;
		int far RedrawTerminal (void) ;
    void far PutChar (char) ;
		BOOL ProcessChars (LPSTR, int) ;
		void CharReceiveState (char) ;
		void EscapeRcvdState (char) ;
		void ParamReceiveState (char) ;
		void KeyDown (int) ;
	
} ;
//
//VT52 Type Terminal class
//
class VT52Term: public RUNTMNL
{
	public:
		int        nTermState ;
		int        nNumChars ;
		char       nParams[MAX_PARAMS] ;
		int        nCurrParam ;

		VT52Term(OBJECTID) ;
		~VT52Term(void) ;
    int far RedrawTerminal (void) ;
		BOOL ProcessChars (LPSTR, int) ;
		void CharReceiveState (char) ;
		void EscapeRcvdState (char) ;
		void ParamReceiveState (char) ;
		void FinalState (char) ;
		void KeyDown (int) ;
		void ArrowKey (int) ;
} ;
//
//TTY type Terminal class
//
class TTYTerm: public RUNTMNL
{
	public:
		int		nNumChars ;

		TTYTerm (OBJECTID) ;
		~TTYTerm (void) ;
		int far RedrawTerminal (void) ;
		BOOL ProcessChars (LPSTR, int) ;
		void KeyDown (int) ;
} ;
//
//TMNL_VI class for window layout and running time
//
class TMNL_VI:public OpHwndWndItem
{
	public:
		OBJECTID   oiTmnl ;
		RUNTMNL*   run_tmnl ;

		TMNL_VI (OBJECTID oiObj, OBJECTID oiParent, RECT* rcItem, long statusFlag);
	 ~TMNL_VI (void);
		void ALMAPI ParentOpened (void) ;
		void ALMAPI ParentClosing (void) ;
		long ALMAPI Activate(int, BOOL);
		void ALMAPI DataChanged(long details,long modifiers,long variation);
		void ALMAPI Destroy (void) ;
    void ALMAPI Paste(void) ;
		void SetupClass (void) ;
		BOOL CreateTerminal (void) ;
		BOOL DestroyTerminal (void) ;
};

void SetEmulationType (HWND, int) ;
void SetTerminalMode (HWND, BYTE) ;
void SetCursorStatus (HWND, int, BOOL) ;
void SetCRorLF (HWND, BYTE) ;
void SetColumns (HWND, int) ;
void SetBufferSize (HWND, int) ;
void SetScrollBar (HWND, BYTE) ;
void SetTranslation (HWND, int) ;
int  SaveEmulationType (HWND) ;
BYTE SaveMode (HWND) ;
BOOL SaveCursorShape (HWND) ;
BOOL SaveCursorBlink (HWND) ;
BYTE SaveCRorLF (HWND) ;
BOOL SaveColumns (HWND) ;
int  SaveBufferSize (HWND) ;
BYTE SaveScrollBar (HWND) ;
int  SaveTranslation (HWND) ;
void SaveEmulationType (HWND, OBJECTID) ;
