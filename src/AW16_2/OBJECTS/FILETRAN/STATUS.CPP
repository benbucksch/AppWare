#define STRICT
#define ASTRICT

#include <windows.h>
#include <commDlg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "com_dlg.h"
#include "filetrns.h"
#include "filedlg.h"
BOOL ALMAPI ShowStatusBarProc (HWND hDlg, WORD message,
																	 WORD wParam, LONG lParam) ;

STATUSCLASS::STATUSCLASS (void)
{
		transferStop = FALSE ;
    filename = new char[256] ;
}
STATUSCLASS::~STATUSCLASS()
{
		delete[] filename ;
}
void STATUSCLASS::ShowStatusWindow (int id)
{
		HWND hwnd = GetActiveWindow() ;
		MSG msg ;

		if (id==1)
			hStatus = CreateDialogParam (hInstance, "asciistatus", hwnd, (DLGPROC)ShowStatusBarProc, (long)this) ;
 		else
			hStatus = CreateDialogParam (hInstance, "Status", hwnd, (DLGPROC)ShowStatusBarProc, (long)this) ;
		//ShowWindow(hStatus, SW_SHOW) ;
		while(PeekMessage(&msg, hStatus, NULL, NULL, PM_REMOVE))
    	{
			IsDialogMessage(hStatus, &msg) ;
			}
}
BOOL STATUSCLASS::InitStatusWindow(int windowID, BOOL barstatus, char* protocolname, char* name, long filelength)
{
	int rVal = TRUE ;

  bBar = barstatus ;
	blocksize = 0 ;
	bBar = bBar ;
	strcpy(filename, name) ;
	filesize = filelength ;
	if (windowID==2)                        //Binary file
		{
		strcpy (protocol, protocolname) ;
		blockno = 0 ;
		error = 0 ;
		}
	ShowStatusWindow(windowID) ;
	if (transferStop) rVal = FALSE ;
  return rVal ;
}
void STATUSCLASS::UpdateStatusWindow (void)
{
	SendMessage (hStatus, WM_USER, 0, 0) ;
}
void STATUSCLASS::ShowContent (HWND hDlg, BOOL updateFilename)
{
		int id ;
		char showBlockSize[25] ;

		ltoa (blocksize, showBlockSize, 10) ;
	 	SetDlgItemText (hDlg, IDD_BLOCKSIZE, showBlockSize) ;
		//SetDlgItemInt  (hDlg, IDD_BLOCKSIZE, (UINT)blocksize, FALSE) ;
		SetDlgItemInt  (hDlg, IDD_BLOCKNO, (UINT)blockno, FALSE) ;
		SetDlgItemInt  (hDlg, IDD_ERROR, (UINT)error, FALSE) ;

		if (updateFilename||writeName)
			SetDlgItemText (hDlg, IDD_FILENAME, filename) ;

		if (bBar)
				ShowStatusBar (hDlg) ;
		else
				for(id=IDD_BAR; id<IDD_BAR+3; id++)
						ShowWindow (GetDlgItem(hDlg, id), SW_HIDE) ;

}
void STATUSCLASS::ShowStatusBar (HWND hDlg)
{
	float  fraction ;
	HWND   hBar ;
	HDC    hdc ;
	RECT   rect ;
	HPEN   hPen ;
  HBRUSH hBrush ;

  if (filesize!=0)
			fraction = (float)blocksize /(float)filesize ;
	else
			fraction = 1.0 ;

	hBar = GetDlgItem(hDlg, IDD_BAR) ;
	InvalidateRect(hBar, NULL, TRUE) ;
	UpdateWindow (hBar) ;

	hdc = GetDC(hBar) ;
	GetClientRect(hBar, &rect) ;
	hPen = (HPEN)SelectObject( hdc, GetStockObject(NULL_PEN)) ;
	hBrush = (HBRUSH)SelectObject( hdc, GetStockObject(BLACK_BRUSH)) ;

	Rectangle(hdc, rect.left, rect.top, rect.right*fraction+1, rect.bottom+1) ;

	SelectObject(hdc, hBrush) ;
	SelectObject(hdc, hPen) ;
	ReleaseDC(hBar, hdc) ;
}

BOOL ALMAPI ShowStatusBarProc (
	HWND hDlg,
	WORD message,
	WORD wParam,
	LONG lParam)
{      
	STATUSCLASS far * lpStatus ;
	char *name ;

	switch (message)
		{
		case WM_INITDIALOG:				  
			lpStatus = (STATUSCLASS far *) lParam ;
			SetWindowLong (hDlg, DWL_USER, lParam) ;
			name = (char *)lstrrchr ((LPSTR)lpStatus->filename, '\\') ;
			if (name!=NULL)
				{
     		lpStatus->filename=name+1 ;
        }
			SetDlgItemText (hDlg, IDD_FILENAME, lpStatus->filename) ;
			SetDlgItemText (hDlg, IDD_PROCOTOL, lpStatus->protocol) ;
			lpStatus->ShowContent (hDlg, TRUE) ;
			return TRUE ;
				
		case WM_COMMAND :
			switch (wParam)
				{
				case IDCANCEL:
					lpStatus = (STATUSCLASS far *) GetWindowLong (hDlg, DWL_USER) ;
					lpStatus->transferStop= TRUE ;
          EndDialog(hDlg, TRUE) ;
					return TRUE ;
				}
				return TRUE ;

		case WM_USER:
				lpStatus = (STATUSCLASS far *) GetWindowLong (hDlg, DWL_USER) ;
        lpStatus->ShowContent (hDlg, FALSE) ;
        return TRUE ;
	 }
	 return FALSE ;
}

