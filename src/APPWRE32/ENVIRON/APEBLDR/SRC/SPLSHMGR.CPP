#include "almsys.h"

#include <stdlib.h>
#include "splshmgr.h"
#include "progsys.h"
#include "windowsx.h"
#include "intdebug.h"
#include "oiputils.h"
#include "mdiclass.h"
#include "fnameobj.h"
#include "utility.h"

#define IDC_PICTURE     1000
#define IDC_DELETE                      1008
#define IDC_LOADFILE            1010
#define IDC_PASTEBMP            1012
#define IDC_PASTEDIB            1014
#define IDC_MINIMUMTIME 1020

//status bits
#define SPLASHMGR_MODIFIED              0x00000001

SPLASHMGR* SPLASHMGR::thisPtr=NULL;
WNDPROC SPLASHMGR::oldEditTimeProc=NULL;

static char NULL_SPLASH_TEXT[] = "No Splash Screen";

////////////////////////////////////////////////////////////////////
SPLASHMGR::SPLASHMGR()
	{
  splashPtr = new DIBITMAP();
  }

////////////////////////////////////////////////////////////////////
SPLASHMGR::~SPLASHMGR()
	{
	delete splashPtr;
  }
  
////////////////////////////////////////////////////////////////////
BOOL SPLASHMGR::ImportSplashScreen(
	DIBITMAP* newSplashPtr, 
	WORD delay)
	{
  BOOL rVal = FALSE;
  
  delayTime = delay;

  if(newSplashPtr)
	*splashPtr = *newSplashPtr;
    
  status = 0;
  
  I_ASSERT(!thisPtr);
	thisPtr = this;
    
  if(DialogBox(PROGSYSTEM::hInst, "SPLASHMGR", PROGSYSTEM::hWndFrame,
	(DLGPROC)SPLASHMGR::ImportSplashDlgProc))
    {
    *newSplashPtr = *splashPtr;
    rVal = TRUE;
    }
	
  thisPtr = NULL;
  
  return rVal;  
	} 
  

////////////////////////////////////////////////////////////////////
BOOL CALLBACK SPLASHMGR::ImportSplashDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	HDC hdc;
  HBRUSH brush;
  HBRUSH oldBrush;
  HPEN oldPen;
  HPEN pen;
  int oldMode;
  COLORREF oldColor;
  DRAWITEMSTRUCT* di;
  RECT rc;
  static HWND nextCbWnd = NULL;
  HBITMAP bmp;
  BOOL palChanged;
  LPBITMAPINFO pDibInfo;
  HANDLE dib;
  HPALETTE hPal;
  char buffer[16];
  
  switch(message)
	{
	  case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			I_ASSERT(thisPtr);
      I_ASSERT(!oldEditTimeProc);
		thisPtr->SetButtons(hDlg);
      nextCbWnd = SetClipboardViewer(hDlg);
      wsprintf(buffer, "%u", thisPtr->delayTime);
		SetWindowText(GetDlgItem(hDlg, IDC_MINIMUMTIME), buffer);

      //subclass the edit control
      oldEditTimeProc = (WNDPROC)SetWindowLong(GetDlgItem(hDlg, IDC_MINIMUMTIME),
	GWL_WNDPROC, (long)SPLASHMGR::EditTimeProc);    
      
		break;
      
    case WM_DESTROY:
	SetWindowLong(GetDlgItem(hDlg, IDC_MINIMUMTIME),
	GWL_WNDPROC, (long)oldEditTimeProc);
       
      oldEditTimeProc = NULL;

	ChangeClipboardChain(hDlg, nextCbWnd);
      break;
      
    case WM_COMMAND:
	switch(GET_WM_COMMAND_ID(wParam, lParam))
	{
	case IDOK:
		if(thisPtr->status & SPLASHMGR_MODIFIED)
		{
	    GetWindowText(GetDlgItem(hDlg, IDC_MINIMUMTIME), buffer,
		sizeof(buffer)-1);
	    thisPtr->delayTime = (WORD)atoi(buffer);
			EndDialog(hDlg, 1);
			break;
		}
	   
	  //fall through to CANCEL
	case IDCANCEL:
		EndDialog(hDlg, 0);
	  break;
	  
	case IDC_DELETE:
		if(thisPtr->splashPtr->Valid())
		{
	    delete thisPtr->splashPtr;
	    thisPtr->splashPtr = new DIBITMAP();
	    
	    InvalidateRect(GetDlgItem(hDlg, IDC_PICTURE), NULL, TRUE);
	    thisPtr->status |= SPLASHMGR_MODIFIED;
	    }
	  else
		I_ASSERT(FALSE);
	    
	  thisPtr->SetButtons(hDlg);
	  break;
	  
	case IDC_LOADFILE:
		if(thisPtr->LoadBitmapFile())
		{
	    InvalidateRect(GetDlgItem(hDlg, IDC_PICTURE), NULL, TRUE);
	    thisPtr->status |= SPLASHMGR_MODIFIED;
	    }
	  
	  thisPtr->SetButtons(hDlg);  
	  break;
	  
	case IDC_PASTEDIB:
	   if(OpenClipboard(hDlg))
		{
		  dib = GetClipboardData(CF_DIB);
		  if(dib)
			{
				if(thisPtr->splashPtr)
				delete thisPtr->splashPtr;
		  
		      pDibInfo = (LPBITMAPINFO)GlobalLock(dib);
		      thisPtr->splashPtr = new DIBITMAP(pDibInfo);
	      GlobalUnlock(dib);
		    InvalidateRect(GetDlgItem(hDlg, IDC_PICTURE), NULL, TRUE);
		    thisPtr->status |= SPLASHMGR_MODIFIED;
		    }
		  else
			I_ASSERT(FALSE);
	      
	    CloseClipboard();
	    }
	  else
		{
	    goto CANT_OPEN_CLIPBOARD;
	    }
       
		thisPtr->SetButtons(hDlg);
		break;
	  
	case IDC_PASTEBMP:
	  if(OpenClipboard(hDlg))
		{
		  bmp = (HBITMAP)GetClipboardData(CF_BITMAP);
		  if(bmp)
			{  
				if(thisPtr->splashPtr)
				delete thisPtr->splashPtr;
		  
		if(IsClipboardFormatAvailable(CF_PALETTE))
		hPal = (HPALETTE)GetClipboardData(CF_PALETTE);
	      else
		hPal = NULL;
		
		      thisPtr->splashPtr = new DIBITMAP(bmp, hPal);
		    InvalidateRect(GetDlgItem(hDlg, IDC_PICTURE), NULL, TRUE);
		    thisPtr->status |= SPLASHMGR_MODIFIED;
		    }
		  else
			I_ASSERT(FALSE);
	      
	    CloseClipboard();
	    }
	  else
		{
CANT_OPEN_CLIPBOARD:
	    MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(GetActiveWindow(), "AppWare cannot retrieve the bitmap"
		" from the clipboard because another application has the clipboard"
	      " locked.", "Clipboard Error", MB_ICONEXCLAMATION|MB_TASKMODAL|
	      MB_OK);
	    }
	  
	  thisPtr->SetButtons(hDlg);  
	  break;
	
	}
      break;
	  
    case WM_DRAWITEM:
	if((UINT)wParam == IDC_PICTURE)
	{
	di = (DRAWITEMSTRUCT*)lParam;
	
	//frame the picture
	
	if(!thisPtr->splashPtr->Valid())
		oldColor = GetSysColor(COLOR_GRAYTEXT);
	else
		oldColor = GetSysColor(COLOR_ACTIVEBORDER);
	
	pen = CreatePen(PS_SOLID, 1, oldColor);
	
	oldPen = SelectPen(di->hDC, pen);
	oldBrush = SelectBrush(di->hDC, GetStockObject(NULL_BRUSH));
	
	Rectangle(di->hDC, di->rcItem.left, di->rcItem.top,
		di->rcItem.right, di->rcItem.bottom);
	  
	SelectPen(di->hDC, oldPen);
	SelectBrush(di->hDC, oldBrush);
	
	if(!thisPtr->splashPtr->Valid())
		{
	  brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	  oldMode = SetBkMode(di->hDC, TRANSPARENT);
	  oldColor = SetBkColor(di->hDC, GetSysColor(COLOR_WINDOWTEXT));
	  
	  rc = di->rcItem;
	  
	  //shrink the rect so we don't draw on the border
	  InflateRect(&rc, -1, -1);
	  
	  FillRect(di->hDC, &rc, brush);
	  DrawText(di->hDC, NULL_SPLASH_TEXT, lstrlen(NULL_SPLASH_TEXT),
		&rc, DT_NOPREFIX|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	  
	  SetBkColor(di->hDC, oldColor);
	  SetBkMode(di->hDC, oldMode);
	  DeleteObject(brush); 
	  }
	else
		{
	  rc = di->rcItem;
	  InflateRect(&rc, -1, -1);
	  thisPtr->splashPtr->Draw(di->hDC, rc.left, rc.top,
		&rc);
	  }
	}
	break;
    
		case WM_PALETTECHANGED:
	if(!thisPtr->splashPtr->Valid())
	break;
    
			if((HWND) wParam == hDlg)
				break;

			//else fall though

		case WM_QUERYNEWPALETTE:
			if(!thisPtr->splashPtr->Valid())
	break;
			
			hdc = GetDC(GetDlgItem(hDlg, IDC_PICTURE));
      GetClientRect(GetDlgItem(hDlg, IDC_PICTURE), &rc);
      InflateRect(&rc, -1, -1);
			palChanged = thisPtr->splashPtr->DrawIfPaletteChanged(hdc, 0, 0, &rc);
			ReleaseDC(GetDlgItem(hDlg, IDC_PICTURE),  hdc);
			
			if(palChanged)
				return TRUE;

			break;
      
    case WM_DRAWCLIPBOARD:
	thisPtr->SetButtons(hDlg);
      if(nextCbWnd != NULL)
	SendMessage(nextCbWnd, message, wParam, lParam);
	break;
      
    case WM_CHANGECBCHAIN:
	if(nextCbWnd == (HWND)wParam)
	{
		nextCbWnd = (HWND)LOWORD(lParam);
	if(nextCbWnd == hDlg)
		nextCbWnd = NULL;
	}
	
      if(nextCbWnd != NULL)
			SendMessage(nextCbWnd, message, wParam, lParam);
	
	break;
		
	
	}
    
  return 0;
	}
	
/////////////////////////////////////////////////////////////////
void SPLASHMGR::SetButtons(HWND hDlg)
	{
	EnableWindow(GetDlgItem(hDlg, IDC_DELETE), (splashPtr->Valid())?TRUE:FALSE);
  EnableWindow(GetDlgItem(hDlg, IDC_PASTEBMP), IsClipboardFormatAvailable(CF_BITMAP));
  EnableWindow(GetDlgItem(hDlg, IDC_PASTEDIB), IsClipboardFormatAvailable(CF_DIB));     
	EnableWindow(GetDlgItem(hDlg, IDC_MINIMUMTIME), (splashPtr->Valid())?TRUE:FALSE);
	}               

/////////////////////////////////////////////////////////////////
BOOL SPLASHMGR::LoadBitmapFile()
	{
  HANDLE fileHandle;
  SFilePath filePath;
	BOOL rVal = FALSE;
  OPENFILENAME ofn;
  
	static char filters[] =
	{
	"Windows Bitmap(*.bmp)\0*.bmp\0"
	};

	MDI::ModalizeVAB(TRUE);
  
  InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters, filePath,
	"Import Splash Screen", 
    OFN_READONLY|OFN_PATHMUSTEXIST|OFN_SHOWHELP|
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR,
    "BMP", FALSE); 
    
  SetupHook(&ofn, SPLASHMGR::CheckDIBFile);     
  
	if(GetOpenFileName(&ofn))
	{
		fileHandle = CreateFile(filePath, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			
		I_ASSERT(fileHandle != INVALID_HANDLE_VALUE);   
    
    if(splashPtr->Valid())
			delete splashPtr;
	
    //pass in zero since we openened a file and there should be no
    //padding on the end.
    splashPtr = new DIBITMAP(fileHandle, 0);
    rVal = TRUE;
    CloseHandle(fileHandle);
    }  
  
  return rVal;
  }

/////////////////////////////////////////////////////////////////////////
BOOL SPLASHMGR::CheckDIBFile(
	HANDLE fileHandle,
	LPCSTR fileName)
	{
	char buffer[256];
  BITMAPFILEHEADER bfh;
  BOOL rVal = TRUE;
  DWORD cb=0;
  
  //seek to start
  SetFilePointer(fileHandle, 0, 0, FILE_BEGIN);
  I_VERIFY(ReadFile(fileHandle, &bfh, sizeof(BITMAPFILEHEADER), &cb, NULL));
  
  //don't check the bfSize member against the file size since this member
  //is different for win32 and win16 platforms.
	if(cb != sizeof(BITMAPFILEHEADER) ||
    bfh.bfType != (('M' << 8) + 'B'))
    {
    rVal = FALSE;
    wsprintf(buffer, "%s is not a valid bitmap file.", fileName);
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBox(GetActiveWindow(), buffer, "Invalid File", 
			MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);
    }
  
	SetFilePointer(fileHandle, 0, 0, FILE_BEGIN); //reposition back to start
  
  return rVal;
	}

//////////////////////////////////////////////////////////////////////
//only accept numbers
LRESULT CALLBACK SPLASHMGR::EditTimeProc(
	HWND            hWnd,
	UINT                    message,
	WPARAM    wParam,
	LPARAM    lParam)
  {
  I_ASSERT(thisPtr);
  I_ASSERT(oldEditTimeProc);
  
  switch(message)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			//if its a number then break;
			if(wParam >= 0x30 && wParam <= 0x39)
				{
	thisPtr->status |= SPLASHMGR_MODIFIED;
				break;
				}
			//otherwise, see if it's some other acceptable key.
			//If its not, then just return.
			switch(wParam)
				{
				case VK_HOME:
				case VK_END:
				case VK_INSERT:
				case VK_DELETE:
				case VK_RIGHT:
				case VK_LEFT:
					//check to see if its an enhanced key and if so, break
					//It it's not enhanced, then its an unwanted key

					//bit 24 of lParam will be set if its an enhanced key
					if(!((1L<<24) & lParam))
						return 0;
					//else fall through

				case VK_MENU: //alt key
				case VK_ESCAPE:
				case VK_TAB:
				case VK_RETURN:
				case VK_BACK:
				case VK_SHIFT:
				case VK_CONTROL:
					break;

				default:
					return 0;
				}
	}
     
   
  return CallWindowProc((FARPROC)oldEditTimeProc, hWnd, message, wParam, lParam);
  }
