#define _MAIN_MODULE_
#include "resizer.h"
#undef _MAIN_MODULE_

extern "C" BOOL WINAPI DllEntryPoint(
	HINSTANCE								hInstDLL,
	DWORD										dwReason,
	LPVOID									)
	{
  WNDCLASS    						wndClass;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_hInstance = (HINSTANCE)hInstDLL;

		  wndClass.style         = CS_GLOBALCLASS;
		  wndClass.lpfnWndProc   = ResizerProc;
		  wndClass.cbClsExtra    = 0L;
		  wndClass.cbWndExtra    = sizeof(void  *);
		  wndClass.hInstance     = g_hInstance;
		  wndClass.hIcon         = NULL;
		  wndClass.hCursor       = NULL;
		  wndClass.hbrBackground = 0L;
		  wndClass.lpszMenuName  = NULL;
		  wndClass.lpszClassName = g_szClassName;
		  RegisterClass(&wndClass);
			break;

		case DLL_PROCESS_DETACH:
			UnregisterClass(g_szClassName, g_hInstance);
			break;
		}
	

	return TRUE;
	}

///////////////////////////////////////////////////////////////
// ResizeThings                                              //
// Addes values the sides of the children of the dialog box. //
// hwndDlg					- The parent dialog box                  //
// szResizeList		- The RCDATA list of control ids           //
// iLeftChange			- The amount to add to the Left          //
// iRightChange		- The amount to add to the Right           //
// iTopChange			- The amount to add to the Top             //
// iBottomChange		- The amount to add to the Bottom        //
// Note: FreeResource is not getting called.  Doing so causes//
//       a gpi fault.  Don't know why.  Bounds checker does  //
//       not report any resource leaks, and on NT its not		 //
//       necessary to call FreeResource.										 //
///////////////////////////////////////////////////////////////
void  ResizeThings(
	HWND										hwndDlg,
	LPCSTR									szResizeList,
	int											iLeftChange,
	int											iRightChange,
	int											iTopChange,
	int											iBottomChange)
	{
	HINSTANCE								hInstance;
	HRSRC										hResource;
	HGLOBAL									hGlobal;
	USHORT									us;
	short *									psControls;
	HWND										hwndControl;
	RECT										rcControl;

	hInstance = (HINSTANCE)GetWindowLong(hwndDlg, GWL_HINSTANCE);
	if ( (hResource = FindResource(hInstance, szResizeList, RT_RCDATA)) != NULL)
		{
	  if ( (hGlobal = LoadResource(hInstance, hResource)) != NULL)
			{
		  if ( (psControls = (short *)LockResource(hGlobal)) != NULL)
				{
				for (us=0;psControls[us]; us++)
					{
					hwndControl = GetDlgItem(hwndDlg, (int)psControls[us]);
					if (hwndControl  &&  IsWindow(hwndControl))
						{
						GetWindowRect(hwndControl, &rcControl);
						MapWindowPoints(NULL, GetParent(hwndControl), (LPPOINT)&rcControl, 2);
						rcControl.left 		+= iLeftChange; 
						rcControl.right 	+= iRightChange;
						rcControl.top			+= iTopChange;
						rcControl.bottom	+= iBottomChange;
						MoveWindow(hwndControl, rcControl.left, rcControl.top, rcControl.right-rcControl.left, rcControl.bottom-rcControl.top, TRUE);
						}
					}
			  UnlockResource(hGlobal);
				}//if ( (psControls...
			}//if ( (hGlobal....
		}	//if ( (hResource...
						 
}

//////////////////////////////////////////////////////////////////
// FindResizerData                                              //
// Find the resizer control in the parent dialog box and        //
// returns the pointer to the data.  The pointer is stored in   //
// the extra bytes of the control.  If it cant find a resizer   //
// control, it returns NULL.  This should never happen because  //
// this function wouldn't get called unless there was a resizer //
// control in the parent dialog box.                            //
//////////////////////////////////////////////////////////////////
LPRESIZER FindResizerData(
	HWND										hWndDialog)
	{
	HWND										hwndChild;
	LPRESIZER								pResizer=NULL;
	char										className[10];

	hwndChild = GetWindow(hWndDialog, GW_CHILD);
	while(hwndChild && !pResizer)
		{
		GetClassName(hwndChild, className, sizeof(className) );
		if (lstrcmp(className, g_szClassName) == 0)
			pResizer = (LPRESIZER)GetWindowLong(hwndChild, 0L);

		hwndChild = GetWindow(hwndChild, GW_HWNDNEXT);
		}

	return pResizer;
	}


//////////////////////////////////////////////////////////////////////////
// SubclassedDialogProc                                                 //
// This is the dialog proc that gets called before the original         //
// dialog proc.  It only cares about the WM_SIZE message, which is      //
// where it resizes the controls in the dialog box.  It then calls the  //
// original dialog proc.  If for some reason it can't find the resizer  //
// data (which contains the original proc) it just plain fails.         //
//////////////////////////////////////////////////////////////////////////
LRESULT  CALLBACK  SubclassedDialogProc(
	HWND 										hWnd, 
	UINT 										msg, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPRESIZER								pResizer;
	MINMAXINFO  *						pMinMaxInfo;
	int											dx, dy;
	RECT										rcWindow;

	pResizer = FindResizerData(hWnd);
	if (!pResizer)
		return 0L;

	switch (msg)
		{
		case WM_INITDIALOG:
			GetWindowRect(hWnd, &rcWindow);
			pResizer->iMinParentWidth  = pResizer->iParentWidth  = rcWindow.right - rcWindow.left;
			pResizer->iMinParentHeight = pResizer->iParentHeight = rcWindow.bottom - rcWindow.top;
			break;

		case WM_GETMINMAXINFO:
			pMinMaxInfo = (MINMAXINFO  *)lParam;
			pMinMaxInfo->ptMinTrackSize.x = pResizer->iMinParentWidth; 
			pMinMaxInfo->ptMinTrackSize.y = pResizer->iMinParentHeight;
			break;

		case WM_SIZE:
			GetWindowRect(hWnd, &rcWindow);
			if (pResizer->iParentWidth != 0 &&  pResizer->iParentHeight != 0)
				{
				SendMessage(hWnd, WM_SETREDRAW, FALSE, 0L);
				dx = rcWindow.right - rcWindow.left - pResizer->iParentWidth;
				dy = rcWindow.bottom - rcWindow.top - pResizer->iParentHeight;
				if (dx > 0)
					{
					ResizeThings(hWnd, "AWResizeRight", 0, dx, 0, 0);
					ResizeThings(hWnd, "AWResizeLeft", dx, 0, 0, 0);
					}
				else if (dx < 0)
					{
					ResizeThings(hWnd, "AWResizeLeft", dx, 0, 0, 0);
					ResizeThings(hWnd, "AWResizeRight", 0, dx, 0, 0);
					}

				if (dy > 0)
					{
					ResizeThings(hWnd, "AWResizeBottom", 0, 0, 0, dy);
					ResizeThings(hWnd, "AWResizeTop", 0, 0, dy, 0);
					}
				else if (dy < 0)
					{
					ResizeThings(hWnd, "AWResizeTop", 0, 0, dy, 0);
					ResizeThings(hWnd, "AWResizeBottom", 0, 0, 0, dy);
					}
				SendMessage(hWnd, WM_SETREDRAW, TRUE, 0L);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				}
			pResizer->iParentWidth  = rcWindow.right - rcWindow.left;
			pResizer->iParentHeight = rcWindow.bottom - rcWindow.top;
			break;
		}

	return pResizer->pfnOldDialogProc(hWnd, msg, wParam, lParam);
	}


//////////////////////////////////////////////////////////////////////////////////
// ResizerProc                                                                  //
// This is the window proc for the resizer contol.  All in does is              //
// allocate memory for the resizer contol, and then subclass the parent         //
// dialog box.  When the control is destroyed, it un-subclasses the dialog box, //
// and frees the memory it allocated.                                           //
//////////////////////////////////////////////////////////////////////////////////
LRESULT  CALLBACK  ResizerProc(
	HWND 										hWnd, 
	UINT 										msg, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPRESIZER								pResizer;

	if (msg == WM_CREATE)
		{
		if ( (pResizer = new RESIZER()) != NULL)
			{
			SetWindowLong(hWnd, 0L, (long)pResizer); 
			pResizer->iParentWidth = 0;
			pResizer->iParentHeight = 0;
			pResizer->pfnOldDialogProc = (WNDPROC)GetWindowLong( GetParent(hWnd), DWL_DLGPROC);
			if (pResizer->pfnOldDialogProc)
				SetWindowLong(GetParent(hWnd), DWL_DLGPROC, (long)SubclassedDialogProc);
			}
		}
	else if (msg == WM_DESTROY)
		{
		if ( (pResizer = (LPRESIZER)GetWindowLong(hWnd, 0L)) != NULL)
			{
			if (pResizer->pfnOldDialogProc)
				SetWindowLong(GetParent(hWnd), DWL_DLGPROC, (long)pResizer->pfnOldDialogProc);
			delete pResizer;
			}
		}

	return DefWindowProc(hWnd, msg, wParam, lParam);
	}


