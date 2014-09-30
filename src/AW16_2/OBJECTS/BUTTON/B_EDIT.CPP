#include "b_include.h"
#include "b_dialog.h"
#include "stdio.h"


#define WM_SETACCELKEY		(WM_USER+50)  //wParam = accelKey
#define WM_GETACCELKEY		(WM_USER+51)  //returns accelKey
#define ACCEL_KEY_PROP "ak"

WNDPROC	g_pfnDefEdit = NULL;
LRESULT far pascal _export  SubClassAccelKey (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT far pascal _export ButtonEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT far pascal _export BitmapEditDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline BOOL IsShiftKeyDown() { return (GetKeyState(VK_SHIFT) & 0x8000);}		//Test the hi-order bit
inline BOOL IsAltKeyDown() { return (GetKeyState(VK_MENU) & 0x8000);}				//Test the hi-order bit
inline BOOL IsCtrlKeyDown() { return (GetKeyState(VK_CONTROL) & 0x8000);}		//Test the hi-order bit

///////////////////////////////////////////

EDIT_BUTTON::EDIT_BUTTON(
	OBJECTID								oi):BUTTON(this)
	{
	LPBUTTON								but;

  m_oiSelf = oi;

	but = LockButtonObject(m_oiSelf);
	I_ASSERT_POINTER(but, return);

	m_style = but->m_style;

	lstrcpy(m_szTitle, but->GetTitle()); //Copy whats in the source
	MakeSureButtonHasTitle();						//If the title hasn't been edited, get the object name
	SetTitleHasBeenEdited(TRUE);				// So next time we edit the button we don't change the title


	_fmemcpy(&m_attributes, &but->m_attributes, sizeof(ATTRIBUTES) );
	m_accelVKey = but->m_accelVKey;
	m_accelMKey = but->m_accelMKey;

	UnlockButtonObject(m_oiSelf);

	m_miNormal = AMemAllocate(0);
	SetNormalBitmapExists(
		(BOOL)CopyDatasToMemBlocks(m_miNormal, m_oiSelf, I_DATA_BMP_NORMAL)
		);

	m_miFocused = AMemAllocate(0);
	SetFocusedBitmapExists(
		(BOOL)CopyDatasToMemBlocks(m_miFocused, m_oiSelf, I_DATA_BMP_FOCUSED)
		);

	m_miPressed = AMemAllocate(0);
	SetPressedBitmapExists(
		(BOOL)CopyDatasToMemBlocks(m_miPressed, m_oiSelf, I_DATA_BMP_PRESSED)
		);

	m_miGrayed = AMemAllocate(0);
	SetGrayedBitmapExists(
		(BOOL)CopyDatasToMemBlocks(m_miGrayed, m_oiSelf, I_DATA_BMP_GRAYED)
		);

	m_currentState = I_NORMAL;

	m_hPalette = 0;
	CreateDefaultBitmaps();
	}

/////////////////

EDIT_BUTTON::~EDIT_BUTTON()
	{
	if (m_hPalette)
		DeleteObject(m_hPalette);

	AMemFree(m_miNormal);
	AMemFree(m_miFocused);
	AMemFree(m_miPressed);
	AMemFree(m_miGrayed);
	}

//////////////////////////
BOOL EDIT_BUTTON::Store()
	{	 
	LPBUTTON								but;

	but = LockButtonObject(m_oiSelf);
	I_ASSERT_POINTER(but, return FALSE);

	but->m_style = m_style;
	_fmemcpy(&but->m_attributes, &m_attributes, sizeof(ATTRIBUTES) );

	but->m_accelVKey = m_accelVKey;
	but->m_accelMKey = m_accelMKey;

	but->SetTitle(m_szTitle);

	UnlockButtonObject(m_oiSelf);

	if (DoesNormalBitmapExist())
		CopyMemBlocksToDatas(m_oiSelf, I_DATA_BMP_NORMAL, m_miNormal);
	else
		AObjResizeData(m_oiSelf, I_DATA_BMP_NORMAL, 0);

	if (DoesFocusedBitmapExist())
		CopyMemBlocksToDatas(m_oiSelf, I_DATA_BMP_FOCUSED, m_miFocused);
	else
		AObjResizeData(m_oiSelf, I_DATA_BMP_FOCUSED, 0);

	if (DoesPressedBitmapExist())
		CopyMemBlocksToDatas(m_oiSelf, I_DATA_BMP_PRESSED, m_miPressed);
	else
		AObjResizeData(m_oiSelf, I_DATA_BMP_PRESSED, 0);

	if (DoesGrayedBitmapExist())
		CopyMemBlocksToDatas(m_oiSelf, I_DATA_BMP_GRAYED, m_miGrayed);
	else
		AObjResizeData(m_oiSelf, I_DATA_BMP_GRAYED, 0);
	
	return TRUE;
	}

/////////////////////////////

void EDIT_BUTTON::InitButtonDialog(
	HWND 										hwnd)
	{
	char										szObjectName[80];

	AObjGetName(m_oiSelf, szObjectName, 80);
  SetWindowText(hwnd, szObjectName);

	switch(GetButtonType())
		{
		case OBUTN_PUSH:			 SendDlgItemMessage(hwnd, RB_PUSHBUTTON, BM_SETCHECK, 1, 0L);		break;
		case OBUTN_CHECK:			 SendDlgItemMessage(hwnd, RB_CHECKBOX, BM_SETCHECK, 1, 0L);			break;
		case OBUTN_RADIO:			 SendDlgItemMessage(hwnd, RB_RADIOBUTTON, BM_SETCHECK, 1, 0L);	break;
		case OBUTN_INVISIBLE:	 SendDlgItemMessage(hwnd, RB_INVISIBLE, BM_SETCHECK, 1, 0L);		break;
		case OBUTN_BITMAP:		 SendDlgItemMessage(hwnd, RB_BITMAP, BM_SETCHECK, 1, 0L);				break;
		}

	if (IsButtonCancel())					SendDlgItemMessage(hwnd, RB_CANCEL, BM_SETCHECK, 1, 0L);
	else if (IsButtonDefault())		SendDlgItemMessage(hwnd, RB_DEFAULT, BM_SETCHECK, 1, 0L);
	else													SendDlgItemMessage(hwnd, RB_NORMAL, BM_SETCHECK, 1, 0L);

	if (IsButtonEnabled())		SendDlgItemMessage(hwnd, CB_ENABLED, BM_SETCHECK, 1, 0L);
	if (IsButtonSelected())		SendDlgItemMessage(hwnd, CB_SELECTED, BM_SETCHECK, 1, 0L);
	if (DoesButtonInvert())		SendDlgItemMessage(hwnd, CB_INVERTS, BM_SETCHECK, 1, 0L);

	SetDlgItemText(hwnd, ED_TITLE, m_szTitle);

	SendMessage( GetDlgItem(hwnd, ED_TITLE), EM_LIMITTEXT, I_TITLE_LEN, 0L);

	//subclass ED_ACCEL
	g_pfnDefEdit = (WNDPROC)GetWindowLong(GetDlgItem(hwnd, ED_ACCEL), GWL_WNDPROC);
	SetWindowLong(GetDlgItem(hwnd, ED_ACCEL), GWL_WNDPROC, (LONG)SubClassAccelKey);

	SendMessage( GetDlgItem(hwnd, ED_ACCEL), WM_SETACCELKEY, (m_accelVKey & (m_accelMKey<<8)), 0);

	ShowProperControls(hwnd);
	}

///////////////////////

void EDIT_BUTTON::InitBitmapDialog(
	HWND 										hwnd)
	{
	switch(GetCurrentState())
		{
		case I_NORMAL:	CheckRadioButton(hwnd, RB_NORMAL, RB_GRAYED, RB_NORMAL); break;
		case I_FOCUSED: CheckRadioButton(hwnd, RB_NORMAL, RB_GRAYED, RB_FOCUSED); break;
		case I_PRESSED: CheckRadioButton(hwnd, RB_NORMAL, RB_GRAYED, RB_PRESSED); break;
		case I_GRAYED: 	CheckRadioButton(hwnd, RB_NORMAL, RB_GRAYED, RB_GRAYED); break;
		}

	switch (GetBitmapStyle())
		{
		case I_ORIGPROP:CheckRadioButton(hwnd, RB_CROP, RB_STRETCH, RB_ORGPROP); break;
		case I_STRETCH:	CheckRadioButton(hwnd, RB_CROP, RB_STRETCH, RB_STRETCH); break;
		case I_CROP:		CheckRadioButton(hwnd, RB_CROP, RB_STRETCH, RB_CROP); break;
		}

	SetBitmapMemUsage(hwnd);
	}


/////////////////////////

void EDIT_BUTTON::RadioButtonClicked(
	HWND										hwnd,
	LONG 										setMask,
	LONG										groupMask)  //The | of all masks for the group
	{
	m_style &= ~groupMask;			//Clear the masks of the groups
	m_style |= setMask;				//Set the one bit of the clicked button

	if (setMask == I_BF_PUSHBUTTON)
		InvalidateRect( GetDlgItem(hwnd, ED_TITLE), NULL, TRUE);

	if (setMask == I_BF_BITMAP  ||  setMask == I_BF_INVISIBLE)
			InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);

	ShowProperControls(hwnd);
	}

/////////////////////////

void EDIT_BUTTON::CheckBoxHit(
	HWND 										hwnd,
	int                     item,
	LONG                    bitMask)
	{
	if (SendDlgItemMessage(hwnd, item, BM_GETCHECK, 0, 0L))
		m_style |= bitMask;
	else
		m_style &= ~bitMask;

	ShowProperControls(hwnd);

	if(I_BF_ENABLED & bitMask) 
			InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
	}

//////////////////////////////

void EDIT_BUTTON::ShowProperControls(
	HWND 										hwnd)
	{
	int											cmdShow;

	cmdShow = GetButtonType()==OBUTN_PUSH ? SW_SHOW:SW_HIDE;
	ShowWindow( GetDlgItem(hwnd, RB_NORMAL), cmdShow);
	ShowWindow( GetDlgItem(hwnd, RB_DEFAULT), cmdShow);
	ShowWindow( GetDlgItem(hwnd, RB_CANCEL), cmdShow);

	ShowWindow( GetDlgItem(hwnd, CB_SELECTED), (GetButtonType()==OBUTN_RADIO || GetButtonType()==OBUTN_CHECK)? SW_SHOW:SW_HIDE);

	ShowWindow( GetDlgItem(hwnd, CB_INVERTS), GetButtonType()==OBUTN_INVISIBLE ? SW_SHOW:SW_HIDE);

	ShowWindow( GetDlgItem(hwnd, PB_LOAD), GetButtonType()==OBUTN_BITMAP ? SW_SHOW:SW_HIDE);

	cmdShow = (GetButtonType()==OBUTN_BITMAP || GetButtonType()==OBUTN_INVISIBLE) ? SW_HIDE:SW_SHOW;
	ShowWindow( GetDlgItem(hwnd, PB_FONTS), cmdShow);
	ShowWindow( GetDlgItem(hwnd, ED_TITLE), cmdShow);
	ShowWindow( GetDlgItem(hwnd, ST_TITLE), cmdShow);

	cmdShow = (GetButtonType()==OBUTN_BITMAP || GetButtonType()==OBUTN_INVISIBLE) ? SW_SHOW:SW_HIDE;
	ShowWindow( GetDlgItem(hwnd, ST_SAMPLE), cmdShow);
	ShowWindow( GetDlgItem(hwnd, OD_SAMPLE), cmdShow);
	ShowWindow( GetDlgItem(hwnd, ST_ACCEL), cmdShow);
	ShowWindow( GetDlgItem(hwnd, ED_ACCEL), cmdShow);

	EnableWindow( GetDlgItem(hwnd, OD_SAMPLE), IsButtonEnabled() );
	}

//////////////////////////////

void EDIT_BUTTON::Draw(
	LPDRAWITEMSTRUCT 				di)
	{
	HBRUSH									hBrush;
	MEMBLOCKID							miBitmap;
	LPBMP_DATA							bd;

	hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(di->hDC, &di->rcItem, hBrush);
	DeleteObject(hBrush);

	if (GetButtonType() == OBUTN_BITMAP)
  	{
		if (di->itemState & ODS_DISABLED)
			miBitmap = m_miGrayed;
		else if (di->itemState & ODS_SELECTED)
			miBitmap = m_miPressed;
		else if (di->itemState & ODS_FOCUS)
			miBitmap = m_miFocused;
		else
			miBitmap = m_miNormal;

		bd = (LPBMP_DATA)AMemLock(miBitmap);
		bd->Draw(di->hDC, &di->rcItem, I_CROP);
		AMemUnlock(miBitmap);
		}
	else if (GetButtonType() == OBUTN_INVISIBLE)
		{
		DrawText(di->hDC, "Invisible Button", -1, &di->rcItem, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
    hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
		FrameRect(di->hDC, &di->rcItem, hBrush);
		DeleteObject(hBrush);

		if (di->itemState & ODS_SELECTED && DoesButtonInvert() )
    	InvertRect(di->hDC, &di->rcItem);
    }
	}

/////////////////////////////

void EDIT_BUTTON::SetDialogButtonTitles(
	HWND										hwnd,
	BOOL										allowAccels)
	{
	if (allowAccels)
  	{
		SetDlgItemText(hwnd, PB_FONTS, "&Attributes...");
		SetDlgItemText(hwnd, PB_LOAD, "&Bitmap...");
		SetDlgItemText(hwnd, PB_HELP, "&Help...");
		SetDlgItemText(hwnd, ST_TITLE, "&Title:");
		SetDlgItemText(hwnd, GB_TYPE, "T&ype");
		SetDlgItemText(hwnd, RB_PUSHBUTTON, "&Push Button");
		SetDlgItemText(hwnd, RB_CHECKBOX, "Chec&k Box");
		SetDlgItemText(hwnd, RB_RADIOBUTTON, "&Radio Button");
		SetDlgItemText(hwnd, RB_INVISIBLE, "In&visible Button");
		SetDlgItemText(hwnd, RB_BITMAP, "Bit&map Button");
		SetDlgItemText(hwnd, GB_STYLE, "Styl&e");
		SetDlgItemText(hwnd, CB_ENABLED, "&Enabled");
		SetDlgItemText(hwnd, CB_SELECTED, "&Selected");
		SetDlgItemText(hwnd, CB_INVERTS, "&Inverts");
		SetDlgItemText(hwnd, RB_NORMAL, "&Normal");
		SetDlgItemText(hwnd, RB_DEFAULT, "&Default");
		SetDlgItemText(hwnd, RB_CANCEL, "&Cancel");
		}
	else
		{
		SetDlgItemText(hwnd, PB_FONTS, "Attributes...");
		SetDlgItemText(hwnd, PB_LOAD, "Bitmap...");
		SetDlgItemText(hwnd, PB_HELP, "Help...");
		SetDlgItemText(hwnd, ST_TITLE, "Title:");
		SetDlgItemText(hwnd, GB_TYPE, "Type");
		SetDlgItemText(hwnd, RB_PUSHBUTTON, "Push Button");
		SetDlgItemText(hwnd, RB_CHECKBOX, "Check Box");
		SetDlgItemText(hwnd, RB_RADIOBUTTON, "Radio Button");
		SetDlgItemText(hwnd, RB_INVISIBLE, "Invisible Button");
		SetDlgItemText(hwnd, RB_BITMAP, "Bitmap Button");
		SetDlgItemText(hwnd, GB_STYLE, "Style");
		SetDlgItemText(hwnd, CB_ENABLED, "Enabled");
		SetDlgItemText(hwnd, CB_SELECTED, "Selected");
		SetDlgItemText(hwnd, CB_INVERTS, "Inverts");
		SetDlgItemText(hwnd, RB_NORMAL, "Normal");
		SetDlgItemText(hwnd, RB_DEFAULT, "Default");
		SetDlgItemText(hwnd, RB_CANCEL, "Cancel");
    }

  }

////////////////

void EDIT_BUTTON::EditFont(
	HWND 										hwndOwner)
	{
	if (*m_szTitle)
		m_attributes.AttrEditFont(hwndOwner, m_szTitle, "Edit Atributes", GetButtonType()!=OBUTN_PUSH);
	else
		m_attributes.AttrEditFont(hwndOwner, "AaBbCc12345", "Edit Atributes", GetButtonType()!=OBUTN_PUSH);
	}

/////////////////

BOOL EDIT_BUTTON::SetCurrentState(
	BITMAP_STATE 						state)
	{
	BOOL										rVal=FALSE;
	
	if (state != GetCurrentState())
		{
		m_currentState = state;
		rVal = TRUE;
		}
	return rVal;
	}


BITMAP_STATE EDIT_BUTTON::GetCurrentState(void)
	{
	return m_currentState;
	}

///////////////

void EDIT_BUTTON::CreateDefaultBitmaps()
	{
	LPBMP_DATA							bd;

	if (!DoesNormalBitmapExist())
		CreateDefaultNormalBitmap(m_miNormal);
	else
		{
		if (m_hPalette)
			{
			DeleteObject(m_hPalette);
			m_hPalette = 0;
			}

		bd = (LPBMP_DATA)AMemLock(m_miNormal);
		if (bd)
			m_hPalette = bd->CreatePaletteForBitmap();
		AMemUnlock(m_miNormal);
		}

	if (!DoesFocusedBitmapExist())
		CreateDefaultFocusedBitmap(m_hPalette, m_miNormal, m_miFocused);

	if (!DoesPressedBitmapExist())
		CreateDefaultPressedBitmap(m_hPalette, m_miNormal, m_miPressed);

	if (!DoesGrayedBitmapExist())
		CreateDefaultGrayedBitmap(m_hPalette, m_miNormal, m_miGrayed);
	}


//////////////////////////////////////////////////

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	,
	LPARAM	)
	{
	UINT		nRes;

	switch (nMsg)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			break;

		default:
			nRes = 0;
			break;
		}

	return nRes;
	}


BOOL EDIT_BUTTON::LoadBitmap(void)
	{
	OPENFILENAME						ofn;
	char 		 								szFullFileName[256];
	LPBMP_DATA							bmpData;
	HFILE										hFile;
	long										lSize;
  int											type;
	MEMBLOCKID							mi;
	BOOL										rVal = FALSE;

	szFullFileName[0] = 0;
	_fmemset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize			 = sizeof(OPENFILENAME);
	ofn.hwndOwner				 = GetActiveWindow();
 	ofn.lpstrFilter			 = "Bitmaps\0*.bmp\0All Files\0*.*\0";
	ofn.nFilterIndex		 = 1;
	ofn.lpstrFile				 = szFullFileName;
	ofn.nMaxFile				 = sizeof(szFullFileName);
	ofn.lpstrInitialDir	 = g_szBitmapPathName;
	ofn.lpstrTitle			 = "Load Bitmap";	
	ofn.lpstrDefExt			 = "bmp";
  ofn.Flags						 = OFN_HIDEREADONLY |
													OFN_FILEMUSTEXIST |
													OFN_NOCHANGEDIR |
													OFN_PATHMUSTEXIST |
													OFN_ENABLEHOOK;
	ofn.lpfnHook         = OpenFileHook;

	if (GetOpenFileName(&ofn))
		{
		hFile = _lopen(szFullFileName, READ);
		if(hFile != -1)
	  	{
			lSize = _llseek(hFile, 0L, SEEK_END);
			_llseek(hFile, 0L, SEEK_SET);	// reset the file pointer

	    //make sure its really a bitmap
			_lread(hFile, (char far*)&type, sizeof(int));
			if (type == 0x4d42) //"BM"
	    	{
				_llseek(hFile, 0L, SEEK_SET);	// reset the file pointer

				mi = GetCurrentBitmapMemblock();
				AMemSetSize(mi, lSize + sizeof(BYTE));
				bmpData = (LPBMP_DATA)AMemLock(mi);

				_hread(hFile, (char huge*)bmpData, lSize);
				AMemUnlock(mi);
				BitmapLoaded();
				rVal = TRUE;
				}

			_lclose(hFile);
			}
		}

	return rVal;
	}

/////////////////

BOOL EDIT_BUTTON::LoadBitmap(
	HWND 										hWnd)
	{
	HBITMAP									hBitmapClipboard;
	HPALETTE								hPalette;
  BOOL										rVal = FALSE;
	HANDLE									hDib;
	long										lHandleSize;
	BYTE huge *							ptrDest;
	BYTE huge *							ptrSource;

	if ( OpenClipboard(hWnd) )
  	{
		if (IsClipboardFormatAvailable(CF_DIB))
			{
			hDib = GetClipboardData(CF_DIB);
			lHandleSize = GlobalSize(hDib);
			if (AMemSetSize(GetCurrentBitmapMemblock(), lHandleSize + sizeof(BITMAPFILEHEADER) ))
				{
				ptrSource = (BYTE huge *)GlobalLock(hDib);
				I_VERIFY_POINTER(ptrSource, return FALSE);

				ptrDest = (BYTE huge *)AMemLock(GetCurrentBitmapMemblock());
				I_VERIFY_POINTER(ptrDest, GlobalUnlock(hDib);return FALSE);

				ptrDest += sizeof(BITMAPFILEHEADER);		 //cause hDib contains a bitminfoheader followed by the bits
				hmemcpy(ptrDest, ptrSource, lHandleSize);

				GlobalUnlock(hDib);
				AMemUnlock(GetCurrentBitmapMemblock());

				BitmapLoaded();
				rVal = TRUE;
				}
			}
		else if (IsClipboardFormatAvailable(CF_BITMAP))
			{
			hBitmapClipboard = (HBITMAP)GetClipboardData(CF_BITMAP);
			hPalette = (HPALETTE)GetClipboardData(CF_PALETTE);
			CopyBitmapHandleToMemblock(GetCurrentBitmapMemblock(), hBitmapClipboard, hPalette);
			BitmapLoaded();
			rVal = TRUE;
			}

		CloseClipboard();
		}

	return rVal;
	}

///////////////////////////////////////

void EDIT_BUTTON::BitmapLoaded(void) 
	{	 //Called when a bitmap is loaded.  Set m_whatExist
	switch(GetCurrentState())
		{
		case I_NORMAL: SetNormalBitmapExists(TRUE); break;
		case I_FOCUSED: SetFocusedBitmapExists(TRUE); break;
		case I_PRESSED: SetPressedBitmapExists(TRUE); break;
		case I_GRAYED: SetGrayedBitmapExists(TRUE); break;
		}

	CreateDefaultBitmaps();
	}

///////////////////////////////////////////

void EDIT_BUTTON::BitmapDeleted(void)
	{		//Called when a bitmap is deleted.  Set m_whatExists
	switch(GetCurrentState())
		{
		case I_NORMAL: SetNormalBitmapExists(FALSE); break;
		case I_FOCUSED: SetFocusedBitmapExists(FALSE); break;
		case I_PRESSED: SetPressedBitmapExists(FALSE); break;
		case I_GRAYED: SetGrayedBitmapExists(FALSE); break;
		}

	CreateDefaultBitmaps();
	}

///////////////////////////////////////

void EDIT_BUTTON::SetBitmapMemUsage(
	HWND										hwnd)
	{	//Sets the static text indicating the mem usage for bitmap buttons
	long										lSize;
	char										szSize[30];

	lSize = DoesNormalBitmapExist() ?  AMemGetSize(m_miNormal) : 0L;
	wsprintf(szSize, "%ldK", lSize/1024L);
	SetDlgItemText(hwnd, ST_NORMAL, szSize);

	lSize = DoesFocusedBitmapExist() ?  AMemGetSize(m_miFocused) : 0L;
	wsprintf(szSize, "%ldK", lSize/1024L);
	SetDlgItemText(hwnd, ST_FOCUSED, szSize);

	lSize = DoesPressedBitmapExist() ?  AMemGetSize(m_miPressed) : 0L;
	wsprintf(szSize, "%ldK", lSize/1024L);
	SetDlgItemText(hwnd, ST_PRESSED, szSize);

	lSize = DoesGrayedBitmapExist() ?  AMemGetSize(m_miGrayed) : 0L;
	wsprintf(szSize, "%ldK", lSize/1024L);
	SetDlgItemText(hwnd, ST_GRAYED, szSize);
	}

///////////////////////////////////////

MEMBLOCKID EDIT_BUTTON::GetCurrentBitmapMemblock()
	{
	MEMBLOCKID							rVal = 0;

	switch(GetCurrentState())
		{
		case I_NORMAL: rVal = m_miNormal; break;
		case I_FOCUSED: rVal = m_miFocused; break;
		case I_PRESSED: rVal = m_miPressed; break;
		case I_GRAYED: rVal = m_miGrayed; break;
		}
	return rVal;
	}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

LONG EditButtonObject(
	OBJECTID            oi)
	{
	BOOL								bEdited;

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(BUTTON_SETUP_DIALOG), GetActiveWindow(), (DLGPROC)ButtonEditDlg, (LONG) oi);

	if (bEdited)
		return A_MODIFIED;
	else
  	return A_OK;
	}

//////////////////////////////////////////////////

LRESULT far pascal _export ButtonEditDlg(
	HWND                    hwnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam )
	{
	LPEDIT_BUTTON           eb;
	LPDRAWITEMSTRUCT				di;
	short										i;
	HDC											hdc;
	HPALETTE								hPalOld;
	LRESULT									rVal;

	switch(msg)
		{
		case WM_INITDIALOG:
			eb = new EDIT_BUTTON( (OBJECTID) lParam);
			SetWindowLong(hwnd, DWL_USER, (long)eb);
			eb->InitButtonDialog(hwnd);
			SetFocus( GetDlgItem(hwnd, ED_TITLE) );
			SendDlgItemMessage(hwnd, ED_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));
			AUtlSetTitleFont(OTYPE_BUTTON, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			delete eb;
			return FALSE;

    case WM_DRAWITEM:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			if (wParam == OD_SAMPLE)
				{
				di = (LPDRAWITEMSTRUCT)lParam;
				
				if (eb->GetPaletteHandle() )
					{
					hPalOld = SelectPalette(di->hDC, eb->GetPaletteHandle(), TRUE);
					RealizePalette(di->hDC);
					}

				eb->Draw(di);

				if (eb->GetPaletteHandle() )
					{
					SelectPalette(di->hDC, hPalOld, TRUE);
					RealizePalette(di->hDC);
					}

				return TRUE;
				}
      break;

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hwnd)
				break;
			//else fall through
		case WM_QUERYNEWPALETTE:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			if (eb->GetPaletteHandle())
				{
				hdc = GetDC(hwnd);
	  	  hPalOld = SelectPalette(hdc, eb->GetPaletteHandle(), FALSE);
	  	  rVal = RealizePalette(hdc);         
	    	SelectPalette(hdc, hPalOld, TRUE);
	  	  RealizePalette(hdc);
	  	  ReleaseDC(hwnd, hdc);
	 	  	if (rVal)                            
	    		InvalidateRect(hwnd, NULL, TRUE);
				}
      break;

		case WM_COMMAND:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			switch(wParam)
				{
				case RB_PUSHBUTTON:		eb->RadioButtonClicked(hwnd, I_BF_PUSHBUTTON, I_BF_TYPE_BITS); break;
				case RB_CHECKBOX:			eb->RadioButtonClicked(hwnd, I_BF_CHECKBOX, I_BF_TYPE_BITS); break;
				case RB_RADIOBUTTON:	eb->RadioButtonClicked(hwnd, I_BF_RADIOBUTTON, I_BF_TYPE_BITS); break;
				case RB_INVISIBLE:		eb->RadioButtonClicked(hwnd, I_BF_INVISIBLE, I_BF_TYPE_BITS); break;
				case RB_BITMAP:				eb->RadioButtonClicked(hwnd, I_BF_BITMAP, I_BF_TYPE_BITS); break;

				case RB_NORMAL:				eb->RadioButtonClicked(hwnd, I_BF_NORMAL, I_BF_PUSH_STYLE_BITS); break;
				case RB_DEFAULT:			eb->RadioButtonClicked(hwnd, I_BF_DEFAULT, I_BF_PUSH_STYLE_BITS); break;
				case RB_CANCEL:				eb->RadioButtonClicked(hwnd, I_BF_CANCEL, I_BF_PUSH_STYLE_BITS); break;

				case CB_ENABLED:			eb->CheckBoxHit(hwnd, wParam, I_BF_ENABLED); break;
				case CB_SELECTED:			eb->CheckBoxHit(hwnd, wParam, I_BF_SELECTED); break;
				case CB_INVERTS:			eb->CheckBoxHit(hwnd, wParam, I_BF_INVERTS); break;

				case ED_ACCEL:
					if (HIWORD(lParam) == EN_KILLFOCUS)
						{//WM_GETACCELKEY returns LOBYTE=vKey, HIBYTE = modifier
						i = (short)SendDlgItemMessage(hwnd, ED_ACCEL, WM_GETACCELKEY, 0, 0);
						eb->SetAccelKey(LOBYTE(i), HIBYTE(i) );
						eb->SetDialogButtonTitles(hwnd, TRUE);
						}
					else if (HIWORD(lParam) == EN_SETFOCUS)
						eb->SetDialogButtonTitles(hwnd, FALSE);

					break;

				case ED_TITLE:
					if (HIWORD(lParam) == EN_KILLFOCUS)
						eb->SetTitle(hwnd, ED_TITLE);
					break;

				case PB_LOAD:
					DialogBoxParam(g_hInstance, MAKEINTRESOURCE(BIT_BUTTON_DIALOG),
												hwnd,(DLGPROC)BitmapEditDlg, (LPARAM)eb);
					InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case PB_OK:
					SetFocus(GetDlgItem(hwnd, PB_OK)); //Will cause the title to get a killfocus
					EndDialog(hwnd, eb->Store());
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					eb->SetTitle(hwnd, ED_TITLE);
					eb->EditFont(hwnd);
					break;

				case PB_HELP:
					break;

				case OD_SAMPLE:
					MessageBeep(0);
        	break;

				}

			break;
		}

	return FALSE;
	}

////////////////////////////////////////
// Callback for the bitmap edit dialog 
////////////////////////////////////////
LRESULT far pascal _export	BitmapEditDlg(
	HWND 										hwnd,
	UINT										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
	LPEDIT_BUTTON						eb;
	LPDRAWITEMSTRUCT				di;
	LPBMP_DATA							bd;
	MEMBLOCKID							mi;
	HDC											hdc;
	HPALETTE								hPalOld;
	LRESULT									rVal;

	switch(message)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hwnd, 0);
			eb = (LPEDIT_BUTTON)lParam;
			SetWindowLong(hwnd, DWL_USER, lParam);
			eb->InitBitmapDialog(hwnd);
			break;

		case WM_DRAWITEM:
			if (wParam == OD_SAMPLE)
				{
				eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
				mi = eb->GetCurrentBitmapMemblock();
				di = (LPDRAWITEMSTRUCT)lParam;

				bd = (LPBMP_DATA)AMemLock(mi);
				I_ASSERT_POINTER(bd, return FALSE);

				if (eb->GetPaletteHandle() )
					{
					hPalOld = SelectPalette(di->hDC, eb->GetPaletteHandle(), TRUE);
					RealizePalette(di->hDC);
					}

				bd->Draw(di->hDC, &di->rcItem, eb->GetBitmapStyle());

				if (eb->GetPaletteHandle() )
					{
					SelectPalette(di->hDC, hPalOld, TRUE);
					RealizePalette(di->hDC);
					}

				AMemUnlock(mi);
        return TRUE;
				}
      break;

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hwnd)
				break;
			//else fall through
		case WM_QUERYNEWPALETTE:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			if (eb->GetPaletteHandle())
				{
				hdc = GetDC(hwnd);
	  	  hPalOld = SelectPalette(hdc, eb->GetPaletteHandle(), FALSE);
	  	  rVal = RealizePalette(hdc);         
	    	SelectPalette(hdc, hPalOld, TRUE);
	  	  RealizePalette(hdc);
	  	  ReleaseDC(hwnd, hdc);
	 	  	if (rVal)                            
	    		InvalidateRect(hwnd, NULL, TRUE);
				}
      break;


		case WM_COMMAND:
			eb = (LPEDIT_BUTTON) GetWindowLong(hwnd, DWL_USER);
			switch(wParam)
				{
				case PB_OK: //"Done"
					EndDialog(hwnd, FALSE);
					return TRUE;

				case RB_NORMAL:
					if ( eb->SetCurrentState(I_NORMAL) )
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_FOCUSED:
					if ( eb->SetCurrentState(I_FOCUSED) )
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_PRESSED:
					if ( eb->SetCurrentState(I_PRESSED) )
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_GRAYED:
					if ( eb->SetCurrentState(I_GRAYED) )
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_STRETCH:
					if (eb->SetBitmapStyle(I_STRETCH))
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_ORGPROP:
					if (eb->SetBitmapStyle(I_ORIGPROP))
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case RB_CROP:
					if (eb->SetBitmapStyle(I_CROP))
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case PB_PASTE:
					if (eb->LoadBitmap(hwnd) )
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					break;

				case PB_LOAD:
					if (eb->LoadBitmap())
						InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					eb->SetBitmapMemUsage(hwnd);
					break;
				case PB_DELETE:
					eb->BitmapDeleted();
					InvalidateRect( GetDlgItem(hwnd, OD_SAMPLE), NULL, TRUE);
					eb->SetBitmapMemUsage(hwnd);
					break;

				}//WM_COMMAND
			break;
		}

  return FALSE;         
}

/////////////////////////////////
// Accel key edit control stuff
/////////////////////////////////
void NameAccelKey(
	int											accelKey,		//LOBYTE=vKey, HIBYTE = modifier
	LPSTR										str)
	{
	LPSTR										cp = str;
	BYTE										vKey, mKey;

	vKey = LOBYTE(accelKey);
	mKey = HIBYTE(accelKey);

	*cp = 0;

	if (mKey & OWND_SHIFT)
		{
		strcpy(cp, "Shift-");
    cp += strlen(cp);
    }
	if (mKey & OWND_CTRL)
		{
		mKey &= ~OWND_CTRL;
		strcpy(cp, "Ctrl-");
    cp += strlen(cp);
    }
	if (mKey & OWND_ALT)
		{
		mKey &= ~OWND_ALT;
		strcpy(cp, "Alt-");
    cp += strlen(cp);
    }

	if (vKey >='0' && vKey <= 'Z')
		sprintf(cp, "%c", vKey);

	else if (vKey >= VK_F1 && vKey <= VK_F16)
		sprintf(cp, "F%d", vKey-VK_F1+1);
	}

/////////////////////////////////

int CreateAccelKey(
	int				vKey)
	{
	int			 	accelKey = 0;

	//LOBYTE=vKey, HIBYTE = modifier

	if ( (vKey >='A' && vKey <= 'Z') || (vKey >= '0' && vKey <= '9') )
		{
    accelKey = vKey;
		if (IsAltKeyDown())
			accelKey |= OWND_ALT<<8;
    else
			accelKey |= OWND_CTRL<<8;
		}

	else if (vKey >= VK_F1 && vKey <= VK_F16)
		{
		accelKey = vKey;

		if (IsShiftKeyDown())
			accelKey |= OWND_SHIFT<<8;
		if (IsCtrlKeyDown())
			accelKey |= OWND_CTRL<<8;
		else if (IsAltKeyDown())
			accelKey |= OWND_ALT<<8;
		}

	return accelKey;
  }

/////////////////////////////////

LRESULT far pascal _export  SubClassAccelKey (
	HWND                    hWnd,
	UINT                    msg,
	WPARAM                  wParam,
	LPARAM                  lParam)
	{
	int											accelKey;
  char										tmpStr[100];

	switch(msg)
		{
		case WM_CREATE:
			SetProp(hWnd, ACCEL_KEY_PROP, 0);
			break;

		case WM_DESTROY:
			RemoveProp(hWnd, ACCEL_KEY_PROP);
			break;

		case WM_CHAR:
		case WM_SYSCHAR:
			if (wParam == VK_SPACE)
				{
				SetProp(hWnd, ACCEL_KEY_PROP, 0);
				SetWindowText(hWnd, " ");
				}
			return 0;

    case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			accelKey = CreateAccelKey(wParam);
			if (accelKey)
      	{
				NameAccelKey(accelKey, tmpStr);
				SetProp(hWnd, ACCEL_KEY_PROP, (HANDLE)accelKey);
				SetWindowText(hWnd, tmpStr);
				}
			return 0;

		case WM_SETACCELKEY:
			SetProp(hWnd, ACCEL_KEY_PROP, (HANDLE)wParam);
			NameAccelKey(wParam, tmpStr);
			SetWindowText(hWnd, tmpStr);
      return 0;

		case WM_GETACCELKEY:
			return (LRESULT)GetProp(hWnd, ACCEL_KEY_PROP);

    }

	return CallWindowProc(g_pfnDefEdit, hWnd, msg, wParam, lParam);
  }




















