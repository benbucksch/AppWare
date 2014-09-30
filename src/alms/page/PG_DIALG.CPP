///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Page
//
// Filename:  "pg_class.cpp"
//
// Description:
//  Page class definition
//
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <intutl.h>

#include "a_alm.h"
#include "a_almutl.h"
#include "o_wnd.h"
#include "o_wndlay.h"
#include "o_print.h"
#include "pg_class.h"
#include "pg_dialg.h"
#include "pg_glob.h"
#include "pg_macro.h"

static HWND   hPLayout;		// global handle for layout
static HWND   hWndObj;	 // global handle for the window object
//
//  Page Layout Dialog Box
//
BOOL CALLBACK PAGE::PageDialog ( HWND hDlg, WORD message, WORD wParam, LONG lParam )
{
	WORD	cmdID;
	WORD	cmdNot;

	PAGE *page;

	switch ( message )
	{
	case WM_INITDIALOG:
		page = (PAGE*)AObjGetRuntimeInfo((AOBJECTID)lParam);
		SetWindowLong(hDlg, DWL_USER, (long)page);
		page->SetDialog(hDlg);
		page->hPen1 = CreatePen(PS_SOLID, 1, 0x00808080L);
		page->hPen2 = CreatePen(PS_SOLID, 2, 0x00808080L);

		page->InitDlgControl();
		AUtlSetTitleFont(OTYPE_PAGE, NULL, GetDlgItem(hDlg, IDD_OBJECT));
		AUtlCenterDialog(hDlg, 0);

		return TRUE;

	case WM_MOVE:
		page = (PAGE*)GetWindowLong(hDlg, DWL_USER);
		GetWindowRect(page->hLayout, &page->layoutRect);
		return 0;

	case WM_COMMAND: 
		cmdID = GET_WM_COMMAND_ID(wParam, lParam);
		cmdNot = GET_WM_COMMAND_CMD(wParam, lParam);
				
		page = (PAGE*)GetWindowLong(hDlg, DWL_USER);
		switch ( cmdID)
		{
		case PAGEOK:
			SetFocus(GetDlgItem(hDlg, PAGEOK));
			page->UpdateObject();
			page->DeleteListData();
			EndDialog(hDlg, 0);
			return TRUE;

		case PAGECANCEL:
			SetFocus(GetDlgItem(hDlg, PAGECANCEL));
			page->DeleteListData();
			EndDialog(hDlg, 0);
			return TRUE;

		case PAGEHELP:
//			WinHelp(hDlg, HELPFILE, HELP_CONTEXT, HELPID_OBJD_Page);
			return TRUE;

		case PAGEADD:
			page->Add();
			return TRUE;

		case PAGEGRID:
			page->snap = IsDlgButtonChecked(hDlg, PAGEGRID);
			return TRUE;

		case PAGEREMOVE:
			page->Remove();
			page->UpdateRemoveButton();
			page->UpdateEditButton();
			return TRUE;

		case PAGEEDIT:
			page->Edit();
			return TRUE;

		case PAGEAVAIL:
			if (cmdNot == LBN_SELCHANGE)
				page->UpdateAddButton();

			if (cmdNot == LBN_DBLCLK)
				page->Add();
			return TRUE;

		case PAGESELECT:
			if (cmdNot == LBN_SELCHANGE)
			{
				page->MakeWindowTop();
				page->UpdateRemoveButton();
				page->UpdateEditButton();
			}

			if (cmdNot == LBN_DBLCLK)
				page->Edit();
			return TRUE;
		}
		break;

	case WM_PAINT:
		page = (PAGE*)GetWindowLong(hDlg, DWL_USER);
		page->DrawObjectName();
   	return FALSE;
	}
	return FALSE;
}


void PAGE::InitDlgControl ( void )
{
	char 	name[OBJECTNAMESIZE+1];
	RECT	lorect;
  POINT	pt;

	data0 = LockPageData0(objectId);

  snap = data0->snap;
	CheckDlgButton(hDialog, PAGEGRID, data0->snap);

	AObjGetName(objectId, name, OBJECTNAMESIZE);
	SetWindowText(hDialog, name);

	hRemoveButton   = GetDlgItem(hDialog, PAGEREMOVE);
	hAddButton      = GetDlgItem(hDialog, PAGEADD);
	hEditButton     = GetDlgItem(hDialog, PAGEEDIT);
  hPageLayout			= GetDlgItem(hDialog, PAGELOUT);  

	SetFocus(hAddButton);

	GetClientRect(hDialog, &dialogRect);

	// key layout to the position of the label "Page Layout"
	GetWindowRect(hPageLayout, &lorect);
	pt.x = lorect.left;
  pt.y = lorect.bottom;
	ScreenToClient(hDialog, &pt);
  dialogRect.top = pt.y + 1;
  dialogRect.left = pt.x;

	grid_height = dialogRect.bottom - dialogRect.top;

	// make sure grid increments are even numbers
	// otherwise snap to grid will not work
	grid_inc_y = grid_height/14;
	grid_inc_x = grid_inc_y*(float(aspecty)/float(aspectx));

	grid_inc_y = (grid_inc_y % 2) ? grid_inc_y-1 : grid_inc_y;
	grid_inc_x = (grid_inc_x % 2) ? grid_inc_x-1 : grid_inc_x;

	grid_height = grid_inc_y * 14;
	grid_width  = (grid_inc_x * 8.5)+4;

	pixinch_x = grid_inc_x;
	pixinch_y = grid_inc_y;

	CreateLayoutClass();
	CreateLayoutWindow();
	FontSetup();

	WindowList();

	CreateObjectClass();
	SelectedList();

	UpdateRemoveButton();
	UpdateEditButton();
	UpdateAddButton();

	UnlockPageData0(objectId);
}


void PAGE::WindowList ( void )
{
	char*           name;
	LRESULT         index;
	AOBJECTID        objid;
	LONG            getkey;

	name = (char*)new char[maxEdit+1];
	if (name == NULL)
		return;

//	SendDlgItemMessage(hDialog, PAGEAVAIL, LB_RESETCONTENT, 0, 0L);
	ListBox_ResetContent(GetDlgItem(hDialog, PAGEAVAIL));

	getkey = -1;
	while ( (objid = AObjGetNextObject(OTYPE_WINDOW, &getkey)) != 0)
	{
		AObjGetName(objid, name, maxEdit);
//		index = SendDlgItemMessage(hDialog, PAGEAVAIL, LB_ADDSTRING, 0, LONG(name));
		index = ListBox_AddString(GetDlgItem(hDialog, PAGEAVAIL), (LPARAM)(int *)(name));

//		SendDlgItemMessage(hDialog, PAGEAVAIL, LB_SETITEMDATA, (WPARAM)index, (long)objid);
		ListBox_SetItemData(GetDlgItem(hDialog, PAGEAVAIL), (WPARAM)index, (LPARAM)objid);
	}
	delete [] name;
}


void PAGE::SelectedList ( void )
{
	WORD    i;
	LRESULT index;
	LONG    count;
	DATA1*  layout;

	data0 = LockPageData0(objectId);

	count = data0->count;

	for (i = 0; i < count; i++)
	{

		data1 = LockPageData1(objectId);

		layout = new DATA1(data1+i);

    layout->Refresh(hLayout);
//		index = SendDlgItemMessage(hDialog, PAGESELECT, LB_ADDSTRING, 0, LONG(layout->name) );
		index = ListBox_AddString(GetDlgItem(hDialog, PAGESELECT), (LPARAM)(int *)(layout->name));

		CreateObjectWindow(layout);
//		SendDlgItemMessage(hDialog, PAGESELECT, LB_SETITEMDATA, (WPARAM)index, (LPARAM)hObjectWnd);
		ListBox_SetItemData(GetDlgItem(hDialog, PAGESELECT), (WPARAM)index, (LPARAM)hObjectWnd);

  	UnlockPageData1(objectId);

	}

	UnlockPageData0(objectId);

}


void PAGE::UpdateObject ( void )
{
	WORD            i;
	LRESULT         count;
	HWND            hwindow;
	DATA1*          layout;

	data0 = LockPageData0(objectId);

	// Data 0 Snap
  data0->snap = snap;

	// Data 0 count
//	count = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETCOUNT, 0, 0L);
	count = ListBox_GetCount(GetDlgItem(hDialog, PAGESELECT));

	data0->count = int(count);

	AObjResizeData(objectId, 1L, sizeof(DATA1)*(count+1));
	data1 = LockPageData1(objectId);

	for (i = 0; i < count; i++)
	{
//		hwindow = (HWND)SendDlgItemMessage(hDialog, PAGESELECT, LB_GETITEMDATA, i, 0L);
		hwindow = (HWND)ListBox_GetItemData(GetDlgItem(hDialog, PAGESELECT), (LPARAM)i);

		layout = (DATA1*)GetWindowLong(hwindow, 0);
		data1[i] = *layout;
	}
	data1[i].objid = NULL;			// set the null terminator for the window object list

	UnlockPageData0(objectId);
	UnlockPageData1(objectId);

}


void PAGE::UpdateAddButton ( void )
{
	LRESULT	cnt;

//	cnt = SendDlgItemMessage(hDialog, PAGEAVAIL, LB_GETSELCOUNT, 0, 0L);
	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, PAGEAVAIL));

	if (cnt)
		EnableWindow(hAddButton, TRUE);
	else
		EnableWindow(hAddButton, FALSE);
}


void PAGE::UpdateRemoveButton ( void )
{
	LRESULT	cnt;

//	cnt = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELCOUNT, 0, 0L);
	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, PAGESELECT));

	if (cnt)
		EnableWindow(hRemoveButton, TRUE);
	else
		EnableWindow(hRemoveButton, FALSE);

}


void PAGE::UpdateEditButton ( void )
{
	LRESULT	cnt;

//	cnt = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELCOUNT, 0, 0L);
	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, PAGESELECT));

	if (cnt == 1)
		EnableWindow(hEditButton, TRUE);
	else
		EnableWindow(hEditButton, FALSE);
}


void PAGE::MakeWindowTop ( void )
{
	LRESULT   cnt;
  LPARAM		item;

//	cnt = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELCOUNT, 0, 0L);
	cnt = ListBox_GetSelCount(GetDlgItem(hDialog, PAGESELECT));

	if (cnt == 1)
	{
//		SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELITEMS, 1, LPARAM(&item));
		ListBox_GetSelItems(GetDlgItem(hDialog, PAGESELECT), (WPARAM)1, LPARAM(&item));

//		hObjectWnd = (HWND)SendDlgItemMessage(hDialog, PAGESELECT, LB_GETITEMDATA, WPARAM(item), 0L);
		hObjectWnd = (HWND)ListBox_GetItemData(GetDlgItem(hDialog, PAGESELECT), WPARAM(item));

		BringWindowToTop(hObjectWnd);
	}
}


void PAGE::Add ( void )
{
	UINT			i;
	LRESULT   count;
	LRESULT		index;
	WORD*    	intArray;
	AOBJECTID  objid;
	DATA1*   	layout;

//	count = SendDlgItemMessage(hDialog, PAGEAVAIL, LB_GETSELCOUNT, 0, 0L);
	count = ListBox_GetSelCount(GetDlgItem(hDialog, PAGEAVAIL));

	if (count == 0)
		return;

	intArray = (WORD*) new WORD[UINT(count+1)];
	if (intArray == NULL)
		return;

//	SendDlgItemMessage(hDialog, PAGEAVAIL, LB_GETSELITEMS, WPARAM(count), LONG(intArray));
	ListBox_GetSelItems(GetDlgItem(hDialog, PAGEAVAIL), WPARAM(count), LPARAM(intArray));

	for (i = 0; i < count; i++)
	{
//		objid = (AOBJECTID)SendDlgItemMessage(hDialog, PAGEAVAIL, LB_GETITEMDATA, intArray[i], 0L);
		objid = (AOBJECTID)ListBox_GetItemData(GetDlgItem(hDialog, PAGEAVAIL), intArray[i]);

		layout = (DATA1*)new DATA1(this, objid);
		CreateObjectWindow(layout);

//		index = SendDlgItemMessage(hDialog, PAGESELECT, LB_ADDSTRING, 0, long(layout->name));
		index = ListBox_AddString(GetDlgItem(hDialog, PAGESELECT), (LPARAM)(int *)(layout->name));

		// Note: the data should be set to hObjectWnd
//		SendDlgItemMessage(hDialog, PAGESELECT, LB_SETITEMDATA, WPARAM(index), LPARAM(hObjectWnd));
		ListBox_SetItemData(GetDlgItem(hDialog, PAGESELECT), WPARAM(index), LPARAM(hObjectWnd));
	}
	delete [] intArray;
}


void PAGE::Remove ( void )
{
	long            i;
	LRESULT         count;
	WORD*           intArray;
	DATA1*          layout;

//	count = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELCOUNT, 0, 0L);
	count = ListBox_GetSelCount(GetDlgItem(hDialog, PAGESELECT));

	if (count == 0)
		return;

	intArray = (WORD*) new WORD[UINT(count)];
	if (intArray == NULL)
		return;

//	SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELITEMS, WPARAM(count), LONG(intArray));
	ListBox_GetSelItems(GetDlgItem(hDialog, PAGESELECT), WPARAM(count), LONG(intArray));

	for (i = count-1; i >= 0; i--)
	{
//		hObjectWnd = (HWND)SendDlgItemMessage(hDialog, PAGESELECT, LB_GETITEMDATA, intArray[UINT(i)], 0L);
		hObjectWnd = (HWND)ListBox_GetItemData(GetDlgItem(hDialog, PAGESELECT), intArray[UINT(i)]);

		layout = (DATA1*)GetWindowLong(hObjectWnd, 0L);

//		SendDlgItemMessage(hDialog, PAGESELECT, LB_DELETESTRING, intArray[UINT(i)], 0L);
		ListBox_DeleteString(GetDlgItem(hDialog, PAGESELECT), intArray[UINT(i)]);

		DestroyWindow(hObjectWnd);
		if (layout != NULL)
			delete layout;
	}
	delete [] intArray;
}


void PAGE::Edit ( void )
{
	LRESULT count;
	LONG    ret;
	int    	intArray;
  HWND		hwindow;
  DATA1*	layout;

//	count = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELCOUNT, 0, 0L);
	count = ListBox_GetSelCount(GetDlgItem(hDialog, PAGESELECT));

	if (count != 1)
		return;

//	ret = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETSELITEMS, WPARAM(count), (LONG)&intArray);
	ret = ListBox_GetSelItems(GetDlgItem(hDialog, PAGESELECT), WPARAM(count), (LPARAM)&intArray);

	if (ret == LB_ERR)
		return;

//	hwindow = (HWND)SendDlgItemMessage(hDialog, PAGESELECT, LB_GETITEMDATA, intArray, 0L);
	hwindow = (HWND)ListBox_GetItemData(GetDlgItem(hDialog, PAGESELECT), intArray);

	layout = (DATA1*)GetWindowLong(hwindow, 0);

	if (layout->objid)
	{
		AObjEdit(layout->objid);

		layout->Refresh(hLayout);
		layout->Resize(hLayout, hwindow);
	}
}


void PAGE::FontSetup ( void )
{
	logfont.lfHeight         = 10;
	logfont.lfWidth          = 0;
	logfont.lfEscapement     = 0;
	logfont.lfOrientation    = 0;
	logfont.lfWeight         = 0;
	logfont.lfItalic         = 0;
	logfont.lfUnderline      = 0;
	logfont.lfStrikeOut      = 0;
	logfont.lfCharSet        = 0;
	logfont.lfOutPrecision   = 0;
	logfont.lfClipPrecision  = 0;
	logfont.lfQuality        = 0;
	logfont.lfPitchAndFamily = 0;

	hFont = CreateFontIndirect(&logfont);
}


void PAGE::CreateLayoutClass ( void )
{
	WNDCLASS        wndclass;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC)PAGE::LayoutWndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 4;
	wndclass.hInstance     = g_hInstance;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = "LayoutWindow";

	RegisterClass(&wndclass);
}


void PAGE::CreateLayoutWindow ( )
{
	hObjectWnd = NULL;
	
	hLayout = CreateWindow ("LayoutWindow", // window class name
		"Window Layout",                    	// window caption
		WS_CHILD | WS_BORDER,               	// window style
		dialogRect.left,	                 		// initial x position
		dialogRect.top,    						  			// initial y position
		grid_width,                         	// initial x size
		grid_height,                        	// initial y size
		hDialog,                            	// parent window handle
		NULL,                               	// window menu handle
		g_hInstance,                        	// program instance handle
		this);                              	// creation parameters

	hPLayout = hLayout;
	ShowWindow(hLayout, SW_SHOW);
	UpdateWindow(hLayout);

	GetWindowRect(hLayout, &layoutRect);
}


LONG CALLBACK PAGE::LayoutWndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
{
	HDC             hdc;
	PAINTSTRUCT     ps;
  RECT            objRect;
	DATA1*          layout;
  PAGE*	 					page;

	switch (message)
	{
	case WM_CREATE:
		page = (PAGE*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowLong(hwnd, 0, (long)page);
		GetWindowRect(hwnd, &page->layoutRect);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		page = (PAGE *)GetWindowLong(hwnd, 0);
		page->DrawGrid(hdc);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_MOUSEMOVE:
		page = (PAGE *)GetWindowLong(hwnd, 0);
		if (GetCapture() == NULL)
			break;

		if ((wParam & MK_LBUTTON) && hWndObj != NULL)
			page->MouseMove(hWndObj, wParam, lParam);
		return 0;

	case WM_LBUTTONDOWN:
//		page = (PAGE *)GetWindowLong(hwnd, 0);
		hWndObj = NULL;
		break;

	case WM_LBUTTONUP:
		if (GetCapture())
		{
			page = (PAGE *)GetWindowLong(hwnd, 0);
			page->MouseMove(hWndObj, wParam, lParam);
			page->DrawWindowFrame(hWndObj);

			// save new location in class DATA1
			GetWindowRect(hWndObj, &objRect);
			layout = (DATA1*)GetWindowLong(hWndObj, 0);
			layout->xy.x = objRect.left;
			layout->xy.y = objRect.top;
			ScreenToClient(page->hLayout, &layout->xy);

			ReleaseCapture();
		}
		return 0;
	 }
	 return DefWindowProc (hwnd, message, wParam, lParam);
}


void PAGE::DrawGrid ( HDC hdc )
{
	long    i;
	long    len;
	char    s[30];

	SelectObject(hdc, hPen2);

	// major rectangle
	SelectObject(hdc, hPen1);

	// horz lines of grid
	for (i = 1; i < 15; i++)
	{
		if (i == 11)
			SelectObject(hdc, hPen2);

		MoveToEx(hdc, 0, (int)i*grid_inc_y, NULL);
		LineTo(hdc, grid_width, (int)i*grid_inc_y);

		if (i == 11)
			SelectObject(hdc, hPen1);
	}

	// vert lines of grid
	for (i = 1; i < 9; i++)
	{
		MoveToEx(hdc, (int)i*grid_inc_x, 0, NULL);
		LineTo(hdc, (int)i*grid_inc_x, grid_height);
	}

	for (i = 0; i < 9; i++)
	{
		wsprintf(s, "%d", i);
		TextOut(hdc, (int)i*grid_inc_x+2, 1, s, 1);
	}

	len = 1;
	for (i = 0; i < 14; i++)
	{
		wsprintf(s, "%d", i);
		if (i > 9)
			len = 2;
		TextOut(hdc, 2, ((int)i*grid_inc_y)+1, s, (int)len);
	}
}


void PAGE::CreateObjectClass ( void )
{
	WNDCLASS        wndclass;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc   = (WNDPROC)PAGE::ObjectWndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 8;							// page layout + window object
	wndclass.hInstance     = g_hInstance;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = "ObjectWindow";

	RegisterClass(&wndclass);
}

void PAGE::CreateObjectWindow ( DATA1*  layout)
{
	hObjectWnd = CreateWindow("ObjectWindow",   	// window class name
		NULL,                                   	// window caption
		WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS, 	// window style
		layout->xy.x,                           	// initial x position
		layout->xy.y,                           	// initial y position
		layout->width,                          	// initial x size
		layout->height,                         	// initial y size
		hLayout,                                	// parent window handle
		NULL,                                   	// window menu handle
		g_hInstance,                            	// program instance handle
		layout);                              	// creation parameters

	ShowWindow(hObjectWnd, SW_SHOW);
	UpdateWindow(hObjectWnd);
}


LONG CALLBACK PAGE::ObjectWndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
{
	HDC             hdc;
	PAINTSTRUCT     ps;
	DATA1*					layout;
	PAGE*	          page;

	switch (message)
	{

	case WM_CREATE:
		layout = ((DATA1*)((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowLong(hwnd, 0, (long)layout);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		layout = (DATA1*)GetWindowLong(hwnd, 0);
		page = (PAGE*)GetWindowLong(hPLayout, 0);
		SelectObject(hdc, page->hFont);
		TextOut(hdc, 1, 1, layout->name, lstrlen(layout->name));
		EndPaint(hwnd, &ps);
		return 0;

	case WM_LBUTTONDBLCLK:
		layout = (DATA1*)GetWindowLong(hwnd, 0);
		page = (PAGE*)GetWindowLong(hPLayout, 0);
		if (layout->objid)
    {
			AObjEdit(layout->objid);
			layout->Refresh(page->hLayout);
			layout->Resize(page->hLayout, hwnd);
    }
		break;

	case WM_LBUTTONDOWN:
	{	
		POINT			pt;

		page = (PAGE*)GetWindowLong(hPLayout, 0);
		BringWindowToTop(hwnd);
		UpdateWindow(hwnd);

		hWndObj = hwnd;
		SetCapture(page->hLayout);

//		ClientToScreen(hwnd, &A_MAKEPOINT(lParam) );
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		ClientToScreen(hwnd, &pt);

//		ScreenToClient(page->hLayout, &A_MAKEPOINT(lParam) );
		ScreenToClient(page->hLayout, &pt);

//		page->mousex_save = LOWORD(lParam);
		page->mousex_save = lParam;

//		page->mousey_save = HIWORD(lParam);
		page->mousey_save = HIWORD(wParam);

		GetWindowRect(hwnd, &page->objectRect);

		page->DrawWindowFrame(hwnd);
		break;
	}
	}
	return DefWindowProc (hwnd, message, wParam, lParam);
}


void PAGE::MouseMove ( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	int     x, y;
	int     w, h;
	int     xchg;
	int     ychg;
	int     inc;

//	xchg = LOWORD(lParam) - mousex_save;
	xchg = lParam - mousex_save;


//	ychg = HIWORD(lParam) - mousey_save;
	ychg = HIWORD(wParam) - mousey_save;

	if (xchg == 0 && ychg == 0)
		return;

	if (objectRect.right + xchg > layoutRect.left + grid_width - 1)
		xchg = layoutRect.left + grid_width - objectRect.right - 1;

	if (objectRect.bottom + ychg > layoutRect.top + grid_height - 1)
		ychg = layoutRect.top + grid_height - objectRect.bottom - 1;

	// compute new coordinates relative to upper left of dialog parent
	x = objectRect.left - layoutRect.left + xchg - 1;
	y = objectRect.top - layoutRect.top + ychg - 1;

	if (snap)
	{
		inc = grid_inc_x/2;
		x /= inc;
		x *= inc;

		inc = grid_inc_y/2;
		y /= inc;
		y *= inc;
	}

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	w = objectRect.right-objectRect.left;
	h = objectRect.bottom-objectRect.top;

	MoveWindow(hwnd, x, y, w, h, TRUE);
}


void PAGE::DrawWindowFrame ( HWND hWnd )
{
	HDC  hDC;
	RECT rc;
	HPEN hPen;

	GetWindowRect(hWnd, &rc);
	hDC = GetWindowDC(hWnd);

	SetROP2(hDC, R2_NOT);

	hPen = CreatePen(PS_INSIDEFRAME, 1 * GetSystemMetrics(SM_CXBORDER), RGB(0, 0, 0));
	SelectObject(hDC, hPen);

	SelectObject(hDC, GetStockObject(NULL_BRUSH));

	Rectangle(hDC, 1, 1, rc.right - rc.left - 1, rc.bottom - rc.top - 1);

	ReleaseDC(hWnd, hDC);
	DeleteObject(hPen);
}


void PAGE::DrawObjectName ( void )
{
	AUtlSetTitleFont(OTYPE_PAGE, NULL, GetDlgItem(hDialog, IDD_OBJECT) );
}


DATA1::DATA1 ( PAGE* page, AOBJECTID  id )
{
	int 					win_width;
  int 					win_height;
	BOOL 					fHorz;
	BOOL 					fVert;
	POINT  				ptScroll;
	RECT				  rect;

	objid = id;

	xy.x            = 20;
	xy.y            = 20;

	width           = 20;         
	height          = 20;                   

	if (objid != 0L)
	{
		char* buf;

		buf = new char[maxEdit+1];
		AObjGetName(objid, buf, maxEdit);
		name = new char[lstrlen(buf)+1];
		lstrcpy(name, buf);
		delete [] buf;

		// get pixel width of window and do conversion
    fHorz = OWndLayHasHScroll(objid);
    fVert = OWndLayHasVScroll(objid);
    ptScroll = OWndLayGetScrollSize(objid);

		// get window logical size(scroll area or physical size)
		OWndGetClientRect(objid, &rect);

		if(fHorz)
		  rect.right = rect.left + ptScroll.x;
		if(fVert)
		  rect.bottom = rect.top + ptScroll.y;

		win_width = rect.right - rect.left;
		win_height = rect.bottom - rect.top;

		pixinch_x = page->PixInchX();
	  pixinch_y = page->PixInchY();

		width = (win_width/25.4)*2;		// convert to the pagelayout grid size
		height = (win_height/25.4)*2;
		
	}
	else
		name = NULL;
}


DATA1::DATA1 ( DATA1* data1 )
{
	*this = *data1;

	if (objid != 0L)
	{
		char* buf;

		buf = new char[maxEdit+1];
		AObjGetName(objid, buf, maxEdit);
		name = new char[lstrlen(buf)+1];
		lstrcpy(name, buf);
		delete [] buf;
	}
	else
		name = NULL;
}


void DATA1::Refresh (HWND hLayout )
{
	int 					win_width;
  int 					win_height;
	RECT			    rect;
	PAGE*					page;
	BOOL 					fHorz;
	BOOL 					fVert;
	POINT  				ptScroll;

	// recompute offsets from left and top if
	// pixels per inch has changed


	page = (PAGE*)GetWindowLong(hLayout, 0);
	if ( pixinch_x != page->PixInchX() )
   	xy.x = xy.x * (float(page->PixInchX())/float(pixinch_x));

	if ( pixinch_y != page->PixInchY() )
		xy.y = xy.y * (float(page->PixInchY())/float(pixinch_y));

	// refresh window size
	// get pixel width of window and do conversion
  fHorz = OWndLayHasHScroll(objid);
  fVert = OWndLayHasVScroll(objid);
  ptScroll = OWndLayGetScrollSize(objid);

	// get window logical size(scroll area or physical size)
	OWndGetClientRect(objid, &rect);

	if(fHorz)
	  rect.right = rect.left + ptScroll.x;
	if(fVert)
	  rect.bottom = rect.top + ptScroll.y;

	win_width = rect.right - rect.left;
	win_height = rect.bottom - rect.top;

	pixinch_x = page->PixInchX();
  pixinch_y = page->PixInchY();

	width = (win_width/25.4)*2;		// convert to the pagelayout grid size
	height = (win_height/25.4)*2;
}


void DATA1::Resize ( HWND hLayout, HWND hwindow )
{
	RECT		rect;
  POINT		point;

	GetWindowRect(hwindow, &rect);
	point.x = rect.left;
  point.y = rect.top;

	ScreenToClient(hLayout, &point),
	MoveWindow(hwindow, point.x, point.y, width, height, TRUE);
}

