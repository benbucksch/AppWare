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
void ODatbGetKeyObjectIDs(AOBJECTID oiDatabase, AMEMBLOCKID keyObjectIDs) ;
void ODatbGetFieldObjectIDs(AOBJECTID oiDatabase, AMEMBLOCKID fieldObjectIDs) ;
BOOL ODatbIsRecLock(AOBJECTID oiDatabase) ;
LONG ODatbGetRecCount(AOBJECTID oiDatabase) ;
LONG ODatbGetFoundCount(AOBJECTID oiDatabase) ;
LONG ODatbGetFoundIndex(AOBJECTID oiDatabase) ;
BOOL ODatbIsFileOpen(AOBJECTID oiDatabase) ;
*/
#define OTYPE_BROWSER   AMAKEID('B','r','w','s')
extern HINSTANCE hInstance ;
extern HBITMAP hBitmap ;
extern int _WinAllocFlag ;

extern "C" {
LONG CALLBACK AdministrationRoutine(AOBJECTID oiBrowser, pAObjMessage theSystem) ;
OpWndItemD* CALLBACK BrowserCreate(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status, long special) ;
void CALLBACK WIDRMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK UpdateWindItemInfoMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK DisposeWindItemInfoMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK WindItemHitMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK WindMouseUpMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK DrawWindItemMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;
void CALLBACK TrackButtonMeth(pAEvtInfo theEvent, pAObjMessage theSystem) ;

void CALLBACK CancelFunc(pAEvtInfo theEvent, pAObjMessage theSystem) ;

BOOL CALLBACK EditObjectDialogProc (HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam) ;
}

LONG EditObject( AOBJECTID oiBrowser ) ;
LONG ValidateObjectIDs( AOBJECTID oiBrowser, pAObjMessage theSystem ) ;
LONG BuildSignals(pAObjMessage theSystem) ;
LONG EvaluateObject( AOBJECTID oiBrowser, pAObjMessage theSystem ) ;

#define AddRecordEvent AMAKEID('B','r','a','d')
#define AppendFoundEvent AMAKEID('B','r','a','f')
#define CloseFileEvent AMAKEID('B','r','c','l')
#define SaveFoundEvent AMAKEID('B','r','c','f')
#define CountRecordsEvent AMAKEID('B','r','c','n')
#define DBFileNameEvent AMAKEID('B','r','f','n')
#define DeleteRecordEvent AMAKEID('B','r','d','l')
#define FindExactRecordsEvent AMAKEID('B','r','f','e')
#define FindRecordsEvent AMAKEID('B','r','f','r')
#define FindRecordIDEvent AMAKEID('B','r','f','i')
#define GetFoundRecordEvent AMAKEID('B','r','g','f')
#define GetRecordEvent AMAKEID('B','r','g','r')
#define LoadFileEvent AMAKEID('B','r','l','d')
#define MacroSearchEvent AMAKEID('B','r','m','s')
#define NewFileEvent AMAKEID('B','r','n','d')
#define OpenFileEvent AMAKEID('B','r','o','f')
#define PowerSearchEvent AMAKEID('B','r','p','s')
#define ReadFoundRecordEvent AMAKEID('B','r','r','f')
#define ReadRecordEvent AMAKEID('B','r','r','r')
#define RecordIndexEvent AMAKEID('B','r','r','i')
#define RemoveFoundEvent AMAKEID('B','r','r','m')
#define RetrieveFoundEvent AMAKEID('B','r','s','f')
#define SortFoundEvent AMAKEID('B','r','s','r')
#define StartXEvent AMAKEID('B','r','s','x')
#define StopXEvent AMAKEID('B','r','t','x')
#define UnlockRecordEvent AMAKEID('B','r','u','l')
#define UpdateRecordEvent AMAKEID('B','r','u','r')
//#define OBUTN_EVT_TRACKBUTTON AMAKEID('B','r','t','b')

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
	AOBJECTID oiDatabase ; // the database object associated with this object
	AOBJECTID oiField ; // the field object used to index the database
	BOOL file ; // TRUE if searching the file, FALSE if searching the found list
	BOOL cancel ; // supports the cancel function
	} BrowserInfo ;

void VerifyDatabaseAndField(BrowserInfo  *lpBrowserInfo) ;

typedef struct _BrowserButton {
	RECT rect ; // bounding rectangle of the button
	char * title ; // name of the button
	} BrowserButton ;

class BrowserVI : public OpLiteWndItem
	{
	public :
	BrowserVI(AOBJECTID oiItem, AOBJECTID oiParent, RECT* rcItem, long status) ;
	~BrowserVI() {} ;
	long ALMAPI AdjustNewRect(RECT* newRect, LONG adjustFlag) ;
	void ALMAPI Destroy(void) {delete this; } ;
	void ALMAPI Draw(HDC hdc, RECT* invalidRect) ;
	void ALMAPI MouseDown(POINT* where, int keysDown, int button) ;
	void ALMAPI MouseMove(POINT* where, int keysDown) ;
	void ALMAPI MouseUp(POINT* where, int keysDown, int button) ;

	private :
	AOBJECTID m_oiDatabase ;
	RECT m_rcDepressed ;
	long m_index ; // stores the current index
	int m_button ; // last button pressed
	BOOL m_depressed ; // TRUE if the button is still depressed
	char m_buffer[BUFFERSIZE] ; // buffer used to store the text string
	BOOL m_beingTracked ;
	AOBJECTID m_oiLastFieldIndex ; // used with Next and Previous

	void DepressButton() ;
	void ReleaseButton() ;
	void SetScope() ;
	void WriteText() ;
	void CallDatabaseMethods() ;
	void UpdateText(int flow) ;
	void doPreviousFound(ODatbMethodParams  *Params) ;
	void doPreviousFile(ODatbMethodParams  *Params) ;
	void doNextFound(ODatbMethodParams  *Params) ;
	void doNextFile(ODatbMethodParams  *Params) ;
	void doFind(ODatbMethodParams  *Params) ;
	void doAdd(ODatbMethodParams  *Params) ;
	void doUpdate(ODatbMethodParams  *Params) ;
	void doDelete(ODatbMethodParams  *Params) ;
	void doDeleteFound(ODatbMethodParams  *Params) ;
	void AllRefresh() ;
	void FoundRefresh() ;
	int GetSortableActiveField(AOBJECTID& oiField) ;
	} ;

#endif // BROWSER_H

