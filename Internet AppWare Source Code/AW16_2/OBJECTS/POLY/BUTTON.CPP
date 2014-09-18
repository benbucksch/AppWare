#include "include.h"

/////////

LPVOID BUTTON::operator new (
	size_t									, 
	size_t 									sizeComplete)
	{
	return malloc(sizeComplete);
	}

/////////

BUTTON::BUTTON(
	LPBUTTON								pButtonSource)
	{
	memcpy(this, pButtonSource, pButtonSource->GetSize() );
	m_hrgn = NULL; //we dont copy the handle
	}

/////////

static void FixRect(
	LPRECT 									pRect)
	{
	SetRect(pRect, 	min(pRect->left, pRect->right), min(pRect->top, pRect->bottom),
									max(pRect->left, pRect->right), max(pRect->top, pRect->bottom) );
	}

//////

BUTTON::BUTTON(
	LONG 										uidSignal, 
	TOOL_TYPE								toolType, 
	LPPOINT 								pPoints, 
	USHORT 									usNumPoints)
	{

	lstrcpy(m_szButtonName, "Untitled");
	m_bSet = FALSE;
	m_hrgn = NULL;
	m_uidSignal = uidSignal;
	m_buttonType = I_PUSH;

	m_usNumRegions = 1;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 	now, we must initialize the region info.  The memory is already alloctated for this //
	// 	The new operator called just returns the memory given to the constructor            //
	//////////////////////////////////////////////////////////////////////////////////////////
	switch(toolType)
		{
		case I_TOOL_RECT:
			FixRect((LPRECT)pPoints);
			new (m_aRegions) REGION(I_RECT, I_COMBINE_INITIAL, pPoints, 2);
			break;

		case I_TOOL_ELLIPSE:
			FixRect((LPRECT)pPoints);
			new (m_aRegions) REGION(I_ELLIPSE, I_COMBINE_INITIAL, pPoints, 2);
			break;

		case I_TOOL_POLYGON:
		case I_TOOL_FREEHAND:
			new (m_aRegions) REGION(I_POLYGON, I_COMBINE_INITIAL, pPoints, usNumPoints);
			break;

		case I_TOOL_ARROW:
		default:
			I_FAILURE();			
			break;
		}

	}

/////////

size_t BUTTON::GetSize()
	{
	USHORT									us;
	LPREGION								pRegion;
	size_t									regionSize, rVal = sizeof(BUTTON);

	pRegion = (LPREGION)m_aRegions;
	for(us=0; us<m_usNumRegions; us++)
		{
		regionSize = pRegion->GetSize();
		rVal += regionSize;
		pRegion = (LPREGION)( (LPSTR)pRegion + regionSize);
		}

	return rVal;
	}

////////////

BOOL BUTTON::GetName(
	LPSTR 									szBuffer, 
	USHORT 									usBufferLen)
	{
	I_VERIFY_POINTER(szBuffer, return FALSE);

	usBufferLen = max(usBufferLen, I_BUTTON_NAME_LENGTH);

	lstrcpyn(szBuffer, m_szButtonName, usBufferLen);
	szBuffer[usBufferLen-1] = 0;

	return TRUE;
	}

/////////

BOOL BUTTON::SetName(
	LPCSTR 									szNewName)
	{
	I_VERIFY_POINTER(szNewName, return FALSE);

	lstrcpyn(m_szButtonName, szNewName, I_BUTTON_NAME_LENGTH);
	m_szButtonName[I_BUTTON_NAME_LENGTH-1] = 0;

	return TRUE;
	}

/////////

BOOL BUTTON::IsThisTheName(
	LPCSTR 									szCompareName)
	{
	I_VERIFY_POINTER(szCompareName, return FALSE);

	return lstrcmp(m_szButtonName, szCompareName) == 0 ? TRUE:FALSE;
	}

/////////

BOOL BUTTON::PointInButton(
	LPPOINT 								pPoint)
	{
	I_VERIFY(m_hrgn, return FALSE);

	return PtInRegion(m_hrgn, pPoint->x, pPoint->y);
	}

/////////

void BUTTON::FrameButton(
	HDC 										hdc, 
	LPPOINT 								pPtOffset)
	{
	LPREGION								pRegion;
	USHORT									us;
	HPEN										hPen;
	HBRUSH									hBrush;
	int											oldROP;

	I_VERIFY(m_hrgn, return);

	if (pPtOffset)
		OffsetButton(pPtOffset->x, pPtOffset->y);

	hPen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0,0,0)));
	oldROP = SetROP2(hdc, R2_NOTXORPEN);
	hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

	pRegion = (LPREGION)m_aRegions;
	for (us=0; us<m_usNumRegions; us++)
		{
		pRegion->FrameRegion(hdc);
		pRegion = (LPREGION) ((LPSTR)pRegion + pRegion->GetSize());
		}

	SelectObject(hdc, hBrush);
	SetROP2(hdc, oldROP);
	hPen = (HPEN)SelectObject(hdc, hPen);
	DeleteObject(hPen);

	if (pPtOffset)
		OffsetButton(-pPtOffset->x, -pPtOffset->y);

	}

/////////

void BUTTON::InvertButton(
	HDC 										hdc, 
	LPPOINT 								pPtOffset)
	{
	I_VERIFY(m_hrgn, return);

	if (pPtOffset)
		OffsetRgn(m_hrgn, pPtOffset->x, pPtOffset->y);

	InvertRgn(hdc, m_hrgn);

	if (pPtOffset)
		OffsetRgn(m_hrgn, -pPtOffset->x, -pPtOffset->y);
 }

//////////	 

void BUTTON::InvalidateButton(
	HWND 										hwnd, 
	BOOL 										bErase,
	LPPOINT 								pPtOffset) //defaults to NULL
	{
	I_VERIFY(m_hrgn, return);

	if (pPtOffset)
		OffsetRgn(m_hrgn, pPtOffset->x, pPtOffset->y);

	InvalidateRgn(hwnd, m_hrgn, bErase);

	if (pPtOffset)
		OffsetRgn(m_hrgn, -pPtOffset->x, -pPtOffset->y);
	}

/////////

void BUTTON::OffsetButton(
	int											dx, 
	int 										dy)
	{
	USHORT									us;
	LPREGION								pRegion;

	if (m_hrgn)
		OffsetRgn(m_hrgn, dx, dy);

	pRegion = (LPREGION)m_aRegions;
	for (us=0; us<m_usNumRegions; us++)
		{
		pRegion->OffsetRegion(dx, dy);
		pRegion = (LPREGION) ((LPSTR)pRegion + pRegion->GetSize());
		}
	}

/////////

USHORT BUTTON::NumberOfSignals()
	{
	switch(GetType())
		{
		case I_PUSH:
		case I_TRANSPARENT:
			return 1;
		case I_RADIO:
		case I_CHECK:
			return 2;
		default:
			return 0;
		}
	}

/////////

void BUTTON::GetSignals(
	pAObjSignalRecord 			pSignals)
	{
	switch(GetType())
		{
		case I_PUSH:
		case I_TRANSPARENT:
			pSignals->theID = m_uidSignal + I_SIG_CLICKED_OFFSET;
			GetName(pSignals->theName, 200);
			lstrcat(pSignals->theName, " Pushed");
			break;

		case I_RADIO:
		case I_CHECK:
			pSignals->theID = m_uidSignal + I_SIG_SET_OFFSET;
			GetName(pSignals->theName, 200);
			lstrcat(pSignals->theName, " Set");
			pSignals++;
			pSignals->theID = m_uidSignal + I_SIG_CLEARED_OFFSET;
			GetName(pSignals->theName, 200);
			lstrcat(pSignals->theName, " Cleared");
			break;
		}
	}

/////////	
void BUTTON::Clicked(
	OBJECTID								oiPoly, 
	HWND										hwndParent, 
	LPPOINT									pPtOffset) 
	{
	AEvtPostSignalAtTail(oiPoly, I_SIG_CLICKED);
	AEvtPostSignalAtTail(oiPoly, m_uidSignal + I_SIG_CLICKED_OFFSET);

	if (hwndParent)
		InvalidateButton(hwndParent, TRUE, pPtOffset);
	}

//////

BOOL BUTTON::Set(
	OBJECTID								oiPoly, 
	HWND										hwndParent, 
	LPPOINT									pPtOffset) 
	{
	BOOL										rVal = FALSE;

	if (!m_bSet)
		{
		m_bSet = TRUE;
		AEvtPostSignalAtTail(oiPoly, I_SIG_SET); 
		AEvtPostSignalAtTail(oiPoly, m_uidSignal + I_SIG_SET_OFFSET);
		rVal = TRUE;

		if (hwndParent)
			InvalidateButton(hwndParent, TRUE, pPtOffset);
		}
	return rVal;
	}

/////////	 

BOOL BUTTON::Clear(
	OBJECTID 								oiPoly, 
	HWND										hwndParent, 
	LPPOINT									pPtOffset)
	{
	BOOL										rVal = FALSE;
	
	if (m_bSet)
		{
		m_bSet = FALSE;
		AEvtPostSignalAtTail(oiPoly, I_SIG_CLEARED); 
		AEvtPostSignalAtTail(oiPoly, m_uidSignal + I_SIG_CLEARED_OFFSET);
		rVal = TRUE;

		if (hwndParent)
			InvalidateButton(hwndParent, TRUE, pPtOffset);
		}
	return rVal;
	}

/////////	 

LPBUTTON BUTTON::CreateRegionHandle(void)
	{
	USHORT									us;
	LPREGION								pRegion;
	
	I_ASSERT(m_usNumRegions);

	pRegion = (LPREGION)m_aRegions;
	m_hrgn = 0;

	for(us=0; us<m_usNumRegions; us++)
		pRegion = pRegion->CombineRegion(&m_hrgn);

	return (LPBUTTON)pRegion;
	}

/////////

BOOL BUTTON::DestroyRegionHandle()
	{
	I_VERIFY(m_hrgn, return FALSE);

	DeleteObject(m_hrgn);
	m_hrgn = 0;
	
	return TRUE;
	}


BOOL BUTTON::Configure(
	USHORT									usIndex)
	{
	LPCONFIGBUTTON					pConfig;

	pConfig = new CONFIGBUTTON(this, usIndex);
	delete pConfig;
	return TRUE;
	}

///////

CONFIGBUTTON::CONFIGBUTTON(
	LPBUTTON								pButtonOrig,
	USHORT									usIndex)
	{
	m_pButtonOrig = pButtonOrig;
	m_usIndex = usIndex;
	SetType(m_pButtonOrig->GetType());
	SetName(m_pButtonOrig->GetName());
	DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(BUTTON_CONFIG_DIALOG),	GetActiveWindow(),	(DLGPROC)ButtonConfigureDlg, (LONG)this);
	}

//////

void CONFIGBUTTON::Store()
	{
	m_pButtonOrig->SetName(GetName());
	m_pButtonOrig->SetType(GetType());
	}

//////

LRESULT far pascal _export CONFIGBUTTON::ButtonConfigureDlg( //static member function
	HWND										hWnd, 
	UINT 										msg, 
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPCONFIGBUTTON					pConfig;
	char										szName[I_BUTTON_NAME_LENGTH];
	char										szTitle[80];

	switch (msg)
		{
		case WM_INITDIALOG:
			pConfig = (LPCONFIGBUTTON)lParam;
			SetWindowLong(hWnd, DWL_USER, (long)pConfig);
			SetDlgItemText(hWnd, ED_TITLE, pConfig->GetName());
			GetDlgItem(hWnd, RB_RADIO);
			SendDlgItemMessage(hWnd, RB_PUSH, BM_SETCHECK, pConfig->GetType() == I_PUSH ?1:0, 0L);
			SendDlgItemMessage(hWnd, RB_RADIO, BM_SETCHECK, pConfig->GetType() == I_RADIO ?1:0, 0L);
			SendDlgItemMessage(hWnd, RB_CHECK, BM_SETCHECK, pConfig->GetType() == I_CHECK ?1:0, 0L);
		 	SendDlgItemMessage(hWnd, RB_TRANSPARENT, BM_SETCHECK, pConfig->GetType() == I_TRANSPARENT ?1:0, 0L);
			wsprintf(szTitle, "Configure Region #%d", pConfig->m_usIndex+1);
			SetWindowText(hWnd, szTitle);
			break;

		case WM_COMMAND:
			pConfig = (LPCONFIGBUTTON)GetWindowLong(hWnd, DWL_USER);
			I_VERIFY_POINTER(pConfig, return 0L);
			switch(wParam)
				{
				case RB_PUSH:
					pConfig->SetType(I_PUSH);
					break;
				case RB_RADIO:
					pConfig->SetType(I_RADIO);
					break;
				case RB_CHECK:
					pConfig->SetType(I_CHECK);
					break;
				case RB_TRANSPARENT:
					pConfig->SetType(I_TRANSPARENT);
					break;

				case ED_TITLE:
					if (HIWORD(lParam) == EN_CHANGE)
						{
						GetDlgItemText(hWnd, ED_TITLE, szName, I_BUTTON_NAME_LENGTH);
						pConfig->SetName(szName);
						}
					break;

				case PB_OK:
					pConfig->Store();
					EndDialog(hWnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hWnd, FALSE);
					break;
				}
		}
	
  return FALSE;         
	}
