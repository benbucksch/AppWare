/////////////////////////////////////////////////////////////////////////////
// File Name:  Browser.H
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date: 7/30/92
//
// Description.:  Header file for the Browser object.
// Object Datas
//	Data 0: the BrowserInfo structure
/////////////////////////////////////////////////////////////////////////////
#ifndef BROWSER_H
#define BROWSER_H
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_datb.h>
/*
void ODatbGetKeyObjectIDs(OBJECTID oiDatabase, MEMBLOCKID keyObjectIDs) ;
void ODatbGetFieldObjectIDs(OBJECTID oiDatabase, MEMBLOCKID fieldObjectIDs) ;
BOOL ODatbIsRecLock(OBJECTID oiDatabase) ;
LONG ODatbGetRecCount(OBJECTID oiDatabase) ;
LONG ODatbGetFoundCount(OBJECTID oiDatabase) ;
LONG ODatbGetFoundIndex(OBJECTID oiDatabase) ;
BOOL ODatbIsFileOpen(OBJECTID oiDatabase) ;
*/
#define OTYPE_BROWSER   MAKEID('B','r','w','s')
extern HANDLE hInstance ;
extern HBITMAP hBitmap ;
extern int _WinAllocFlag ;

extern "C" {
LONG FAR PASCAL AdministrationRoutine(OBJECTID oiBrowser, pAObjMessage theSystem) ;
OpWndItemD* FAR PASCAL BrowserCreate(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status, long special) ;
void FAR PASCAL WIDRMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL UpdateWindItemInfoMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL DisposeWindItemInfoMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL WindItemHitMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL WindMouseUpMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL DrawWindItemMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void FAR PASCAL TrackButtonMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;

void FAR PASCAL CancelFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;

BOOL FAR PASCAL EditObjectDialogProc (HWND hDlg, WORD message, WORD wParam, LONG lParam) ;
}

LONG EditObject( OBJECTID oiBrowser ) ;
LONG ValidateObjectIDs( OBJECTID oiBrowser, pAObjMessage theSystem ) ;
LONG BuildSignals(pAObjMessage theSystem) ;
LONG EvaluateObject( OBJECTID oiBrowser, pAObjMessage theSystem ) ;

#define AddRecordEvent MAKEID('B','r','a','d')
#define AppendFoundEvent MAKEID('B','r','a','f')
#define CloseFileEvent MAKEID('B','r','c','l')
#define SaveFoundEvent MAKEID('B','r','c','f')
#define CountRecordsEvent MAKEID('B','r','c','n')
#define DBFileNameEvent MAKEID('B','r','f','n')
#define DeleteRecordEvent MAKEID('B','r','d','l')
#define FindExactRecordsEvent MAKEID('B','r','f','e')
#define FindRecordsEvent MAKEID('B','r','f','r')
#define FindRecordIDEvent MAKEID('B','r','f','i')
#define GetFoundRecordEvent MAKEID('B','r','g','f')
#define GetRecordEvent MAKEID('B','r','g','r')
#define LoadFileEvent MAKEID('B','r','l','d')
#define MacroSearchEvent MAKEID('B','r','m','s')
#define NewFileEvent MAKEID('B','r','n','d')
#define OpenFileEvent MAKEID('B','r','o','f')
#define PowerSearchEvent MAKEID('B','r','p','s')
#define ReadFoundRecordEvent MAKEID('B','r','r','f')
#define ReadRecordEvent MAKEID('B','r','r','r')
#define RecordIndexEvent MAKEID('B','r','r','i')
#define RemoveFoundEvent MAKEID('B','r','r','m')
#define RetrieveFoundEvent MAKEID('B','r','s','f')
#define SortFoundEvent MAKEID('B','r','s','r')
#define StartXEvent MAKEID('B','r','s','x')
#define StopXEvent MAKEID('B','r','t','x')
#define UnlockRecordEvent MAKEID('B','r','u','l')
#define UpdateRecordEvent MAKEID('B','r','u','r')
//#define OBUTN_EVT_TRACKBUTTON MAKEID('B','r','t','b')

#define INFO_INDEX 0
#define OBJECTNAMESIZE 32

// Object Definition Dialog Box Constants
#define IDHELP				998
#define IDC_DATABASE		101
#define IDC_FIELD			102
#define IDS_FIELD			103
#define IDD_OBJECT			200

#define BUFFERSIZE  40
typedef struct _BrowserInfo {
	int	version ; // Current version = 0
	OBJECTID oiDatabase ; // the database object associated with this object
	OBJECTID oiField ; // the field object used to index the database
	BOOL file ; // TRUE if searching the file, FALSE if searching the found list
	BOOL cancel ; // supports the cancel function
	} BrowserInfo ;

void VerifyDatabaseAndField(BrowserInfo far *lpBrowserInfo) ;

typedef struct _BrowserButton {
	RECT rect ; // bounding rectangle of the button
	char far* title ; // name of the button
	} BrowserButton ;

class BrowserVI : public OpLiteWndItem
	{
	public :
	BrowserVI(OBJECTID oiItem, OBJECTID oiParent, RECT* rcItem, long status) ;
	~BrowserVI() {} ;
	long ALMAPI AdjustNewRect(RECT* newRect, LONG adjustFlag) ;
	void ALMAPI Destroy(void) {delete this; } ;
	void ALMAPI Draw(HDC hdc, RECT* invalidRect) ;
	void ALMAPI MouseDown(POINT* where, int keysDown, int button) ;
	void ALMAPI MouseMove(POINT* where, int keysDown) ;
	void ALMAPI MouseUp(POINT* where, int keysDown, int button) ;

	private :
	OBJECTID m_oiDatabase ;
	RECT m_rcDepressed ;
	long m_index ; // stores the current index
	int m_button ; // last button pressed
	BOOL m_depressed ; // TRUE if the button is still depressed
	char m_buffer[BUFFERSIZE] ; // buffer used to store the text string
	BOOL m_beingTracked ;
	OBJECTID m_oiLastFieldIndex ; // used with Next and Previous

	void DepressButton() ;
	void ReleaseButton() ;
	void SetScope() ;
	void WriteText() ;
	void CallDatabaseMethods() ;
	void UpdateText(int flow) ;
	void doPreviousFound(ODatbMethodParams far *Params) ;
	void doPreviousFile(ODatbMethodParams far *Params) ;
	void doNextFound(ODatbMethodParams far *Params) ;
	void doNextFile(ODatbMethodParams far *Params) ;
	void doFind(ODatbMethodParams far *Params) ;
	void doAdd(ODatbMethodParams far *Params) ;
	void doUpdate(ODatbMethodParams far *Params) ;
	void doDelete(ODatbMethodParams far *Params) ;
	void doDeleteFound(ODatbMethodParams far *Params) ;
	void AllRefresh() ;
	void FoundRefresh() ;
	int GetSortableActiveField(OBJECTID& oiField) ;
	} ;

#endif // BROWSER_H

