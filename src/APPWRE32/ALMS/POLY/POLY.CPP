#include "include.h"

/////////////

BOOL POLY::IgnoreCount_CreateBitmapHandles()
	{
	HPALETTE								hPalette;
	HDC											hdcScreen;

	I_ASSERT(!m_hBmpNormal && !m_hBmpHighlight);

	hdcScreen = GetDC(NULL);
	hPalette = SelectPalette(hdcScreen, (HPALETTE)GetStockObject(DEFAULT_PALETTE), FALSE);
	SelectPalette(hdcScreen, hPalette, FALSE);
	ReleaseDC(NULL, hdcScreen);

	m_hBmpNormal = CreateBitmapHandle(hPalette, m_oiSelf, I_DATA_NORMAL);
	m_hBmpHighlight = CreateBitmapHandle(hPalette, m_oiSelf, I_DATA_HIGHLIGHT);

	return (m_hBmpNormal && m_hBmpHighlight);
	}

///////////

BOOL POLY::IgnoreCount_DestroyBitmapHandles()
	{
	if (m_hBmpNormal)
		DeleteObject(m_hBmpNormal);
	m_hBmpNormal = 0;

	if (m_hBmpHighlight)
		DeleteObject(m_hBmpHighlight);
	m_hBmpHighlight = 0;

	return TRUE;
	}

///////////

BOOL POLY::IgnoreCount_CreateButtons()
	{
	USHORT									us;
	LPBUTTON								pButtonNext;
	BOOL										rVal=FALSE;

	if (m_usNumButtons)
		{
		I_ASSERT(!m_ptrButtons);

		m_miButtonPointers = AMemAllocate(m_usNumButtons * sizeof(LPVOID));
		m_ptrButtons = (LPBUTTON  *)AMemLock(m_miButtonPointers);
		I_VERIFY_POINTER (m_ptrButtons, return FALSE);

		pButtonNext = (LPBUTTON)m_aButtons;

		for (us=0; us < m_usNumButtons; us++)
			{
			m_ptrButtons[us] = pButtonNext;
			pButtonNext = m_ptrButtons[us]->CreateRegionHandle();
			}

		rVal = TRUE;
		}

	return rVal;
	}

///////////

BOOL POLY::IgnoreCount_DestroyButtons()
	{
	BOOL										rVal = FALSE;
	USHORT									us;

	if (m_usNumButtons)
		{
		if (m_ptrButtons)
			{
			for (us=0; us < m_usNumButtons; us++)
				m_ptrButtons[us]->DestroyRegionHandle();
			}

		m_ptrButtons = NULL;
		AMemUnlock(m_miButtonPointers);
		AMemFree(m_miButtonPointers);

		rVal = TRUE;
		}
	
	return rVal;
	}

////////

LPVOID POLY::operator new(
	size_t 									, 
	size_t 									sizeReal,
	AOBJECTID								oiPoly)
	{
	AObjSetDataCount(oiPoly, I_NUM_DATAS);
	AObjResizeData(oiPoly, I_DATA_POLY, sizeReal);
	
	return AObjLockData(oiPoly, I_DATA_POLY);  //will be unlocked in the constructor
	}

//////////

POLY::POLY(
	AOBJECTID								oiPoly,
	LPBUTTON  * 					ppButtons, 
	USHORT 									usNumButtons, 
	unsigned long 					uidNext, 
	BOOL 										bZeroCounters)
	{
	USHORT									us;
	LPBUTTON								pButtonDest;

	m_version = I_VERSION;
	m_oiSelf = oiPoly;

	m_hBmpNormal = m_hBmpHighlight = NULL;

	if (bZeroCounters)
		{
		m_usBitmapHandleUseCount = 0;
		m_usButtonUseCount = 0;
		}

	m_usLastButtonClicked = I_UNDEFINED_INDEX;

	m_uidNext = uidNext;

	m_miButtonPointers = 0L;
	m_ptrButtons  = NULL; //pointers to futher on down where the buttons are

	//now, copy in the buttons provided
	m_usNumButtons = usNumButtons;
	for (us=0, pButtonDest=(LPBUTTON)m_aButtons; us<m_usNumButtons; us++)
		{
		pButtonDest = new (pButtonDest) BUTTON(ppButtons[us]);
		pButtonDest = (LPBUTTON) ( (LPSTR)pButtonDest + pButtonDest->GetSize() );
		}

	AObjUnlockData(oiPoly, I_DATA_POLY);  //was locked in the new operator
	}

///////////	

LONG POLY::DoAssign( //static member funtions
	AOBJECTID								oiDest,
	AOBJECTID								oiSource)
	{
	LPPOLY									polyDest, polySource;
	LPBUTTON								pButtonDest, pButtonSource;
	LONG										lSourceSize;
	USHORT									us;

	I_VERIFY(oiDest != oiSource, return A_ERROR);

	//First, destroy handles, but leave the counters alone
	polyDest = LockPoly(oiDest);
	I_VERIFY_POINTER(polyDest, return A_ERROR);

	polyDest->IgnoreCount_DestroyBitmapHandles();
	polyDest->IgnoreCount_DestroyButtons();				
	
	UnlockPoly(oiDest);

	// Copy the bitmaps
	CopyDatas(oiDest, I_DATA_NORMAL, oiSource, I_DATA_NORMAL);
	CopyDatas(oiDest, I_DATA_HIGHLIGHT, oiSource, I_DATA_HIGHLIGHT);

	//Now, resize the dest so it has the same amount of room for buttons and points
	lSourceSize = GetPolyDataSize(oiSource);
	SetPolyDataSize(oiDest,	lSourceSize);


	//Now, copy the necessary data
	polyDest = LockPoly(oiDest);
	I_VERIFY_POINTER(polyDest, return A_ERROR);

	polySource = LockPoly(oiSource);
	I_VERIFY_POINTER(polySource, UnlockPoly(oiDest);return A_ERROR);

	polyDest->m_usLastButtonClicked		= polySource->m_usLastButtonClicked;
	polyDest->m_usNumButtons					= polySource->m_usNumButtons;

	//copy the buttons over
	pButtonSource = (LPBUTTON)polySource->m_aButtons;
	pButtonDest = (LPBUTTON)polyDest->m_aButtons;
	for(us=0; us < polyDest->m_usNumButtons; us++)
		{
		new (pButtonDest) BUTTON(pButtonSource);
		pButtonDest = (LPBUTTON)((LPSTR)pButtonDest + pButtonDest->GetSize());
		pButtonSource = (LPBUTTON)((LPSTR)pButtonSource + pButtonSource->GetSize());
		}

	UnlockPoly(oiSource);

	//Recreate the handles if the counters aren't 0
	if (polyDest->m_usBitmapHandleUseCount)
		polyDest->IgnoreCount_CreateBitmapHandles();	

	if (polyDest->m_usButtonUseCount)
		polyDest->IgnoreCount_CreateButtons();

	UnlockPoly(oiDest);		 

	PostObjectChanged(oiDest);
	return A_OK;
	}
										
////////////

LONG POLY::Edit( //static member funtions
	AOBJECTID								oiEdit)
	{
	BOOL										bEdited;
	LPPOLY									poly;

	poly = LockPoly(oiEdit);
	I_VERIFY_POINTER(poly, return A_ERROR);
	poly->IgnoreCount_DestroyBitmapHandles();
	poly->IgnoreCount_DestroyButtons();
	UnlockPoly(oiEdit);

	bEdited = DialogBoxParam(g_hInstance,	MAKEINTRESOURCE(POLYBUTTON_MAIN_SETUP_DIALOG),	GetActiveWindow(),	(DLGPROC)EDIT_POLY::PolybuttonEditDlg, (LONG)oiEdit);
	
	poly = LockPoly(oiEdit);
	I_VERIFY_POINTER(poly, return A_ERROR);
	if (poly->m_usBitmapHandleUseCount)
		poly->IgnoreCount_CreateBitmapHandles();	
	if (poly->m_usButtonUseCount)
		poly->IgnoreCount_CreateButtons();
	UnlockPoly(oiEdit);

	if (bEdited)
		{
		PostObjectChanged(oiEdit);
		return A_MODIFIED;
		}
	else
  	return A_OK;
	}

/////////

LONG POLY::Read( //static member function
	AOBJECTID								oiPoly)
	{
	LPPOLY									poly;
	LONG										rVal = A_OK;

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return A_WARNING);

	if (poly->m_version < 100)
		{ //This is ECO's piece of shit...kill it, kill it all
		UnlockPoly(oiPoly);
		new(sizeof(POLY), oiPoly) POLY(oiPoly, NULL, 0, I_UID_INITIAL_VALUE, TRUE);  //re-initial everything
		return A_WARNING;
		}

	//zero out stuff
	poly->m_hBmpNormal = poly->m_hBmpHighlight = NULL;
	poly->m_usBitmapHandleUseCount = poly->m_usButtonUseCount = 0;
	poly->m_usLastButtonClicked = I_UNDEFINED_INDEX;
	poly->m_miButtonPointers = 0;
	poly->m_ptrButtons = NULL;

	UnlockPoly(oiPoly);

	return rVal;
	}

////////////////
/// Name:POLY::GetSize
/// Purpose: returns the amount of memory needed for the polybutton
size_t POLY::GetSize()
	{
	size_t									s;
	LPBUTTON								pButton;
	USHORT									us;

	s = sizeof(POLY);

	for (pButton = (LPBUTTON)m_aButtons, us=0; us < m_usNumButtons; us++)
		{
		s += pButton->GetSize();
		pButton = (LPBUTTON) ( (LPSTR)pButton + pButton->GetSize() );
		}

	return s;
	}

////

LONG POLY::BuildSignals(
	AMEMBLOCKID 							miSignals)
	{
	pAObjSignalRecord				pSignals;
	USHORT									usNumSignals, usSignalNum, us;
	LPBUTTON								pButton;

	usNumSignals = I_NUM_BASE_SIGNALS;

	pButton = (LPBUTTON)m_aButtons; 
	for(us=0; us<m_usNumButtons; us++)
		{
		usNumSignals += pButton->NumberOfSignals();
		pButton = (LPBUTTON) ( (LPSTR)pButton + pButton->GetSize() );
		}

	AMemSetSize(miSignals, usNumSignals * sizeof(AObjSignalRecord) );
	pSignals = (pAObjSignalRecord)AMemLock(miSignals);
	I_VERIFY_POINTER(pSignals, return A_ERROR);
	FillMemory(pSignals, 0,  usNumSignals * sizeof(AObjSignalRecord) );

	
	usSignalNum = 0;

	pSignals[usSignalNum].theID = I_SIG_CLICKED;
	lstrcpy(pSignals[usSignalNum++].theName, "Region Clicked");

	pSignals[usSignalNum].theID = I_SIG_SET;
	lstrcpy(pSignals[usSignalNum++].theName, "Region Set");

	pSignals[usSignalNum].theID = I_SIG_CLEARED;
	lstrcpy(pSignals[usSignalNum++].theName, "Region Cleared");

	pButton = (LPBUTTON)m_aButtons; 
	for(us=0; us<m_usNumButtons; us++)
		{
		pButton->GetSignals(&pSignals[usSignalNum]);
		usSignalNum += pButton->NumberOfSignals();
		pButton = (LPBUTTON) ( (LPSTR)pButton + pButton->GetSize() );
		}

	AMemUnlock(miSignals);
	return A_OK;
	}

/////////////

BOOL POLY::CreateBitmapHandles()
	{
	if (!m_usBitmapHandleUseCount)
		IgnoreCount_CreateBitmapHandles();	

	m_usBitmapHandleUseCount++;

	return TRUE;
	}

//////

BOOL POLY::DestroyBitmapHandles()
	{
	m_usBitmapHandleUseCount--;
	
	if (!m_usBitmapHandleUseCount)
		IgnoreCount_DestroyBitmapHandles();	

	return TRUE;
	}

/////////

BOOL POLY::CreateButtons()
	{
	if (!m_usButtonUseCount)
		IgnoreCount_CreateButtons();	

	m_usButtonUseCount++;

	return TRUE;
	}
	
//////

BOOL POLY::DestroyButtons()
	{
	m_usButtonUseCount--;
	
	if (!m_usButtonUseCount)
		IgnoreCount_DestroyButtons();	

	return TRUE;
	}

/////////
USHORT POLY::PointInWhichButton(
	LPPOINT									ptOffset)
	{
	USHORT									us, rVal = I_UNDEFINED_INDEX;

	I_VERIFY_POINTER(m_ptrButtons, return rVal);

	for (us=0; us<m_usNumButtons && rVal == I_UNDEFINED_INDEX; us++)
		{
		if (m_ptrButtons[us]->PointInButton(ptOffset))
			rVal = us;
		}
	
	return rVal;
	}

//////////

BOOL POLY::InvalidateButton(
	USHORT 									usIndex, 
	HWND 										hwnd, 
	BOOL 										bErase,
	LPPOINT 								pPtOffset)
	{
	BOOL										rVal = FALSE;

	if (usIndex < m_usNumButtons)
		{	
		m_ptrButtons[usIndex]->InvalidateButton(hwnd, bErase, pPtOffset);
		rVal = TRUE;
		}

	return rVal;
	}

//////////

BOOL POLY::FrameButton(
	USHORT 									usIndex, 
	HDC 										hdc, 
	LPPOINT 								pPtOffset)
	{
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < m_usNumButtons)
		{	
		m_ptrButtons[usIndex]->FrameButton(hdc, pPtOffset);
		rVal = TRUE;
		}
	return rVal;
	}

//////////

BOOL POLY::GetLastClickedButton(
	USHORT  *						pusIndex, 
	LPSTR 									szName, 
	USHORT 									usBufferLen)
	{
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);

	if (m_usLastButtonClicked != I_UNDEFINED_INDEX)
		{
		if (pusIndex)
			*pusIndex = m_usLastButtonClicked;
		if (szName)
			m_ptrButtons[m_usLastButtonClicked]->GetName(szName, usBufferLen);
		rVal = TRUE;
		}

	return rVal;
	}

//////

BOOL POLY::GetButtonIndexByName(
	USHORT  *						pusIndex, 
	LPSTR 									szName)
	{
	USHORT									us;
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	I_VERIFY_POINTER(pusIndex, return FALSE);
	I_VERIFY_POINTER(szName, return FALSE);

	for(us=0; us<m_usNumButtons && !rVal; us++)
		{
		if (m_ptrButtons[us]->IsThisTheName(szName))
			{
			rVal = TRUE;
			*pusIndex = us;
			}
		}

	return rVal;
	}

//////

BOOL POLY::GetButtonNameByIndex(
	USHORT 									usIndex, 
	LPSTR 									szName, 
	USHORT 									usBufferLen)
	{
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < m_usNumButtons)
		{	
		m_ptrButtons[usIndex]->GetName(szName, usBufferLen);
		rVal = TRUE;
		}
	
	return rVal;
	}

//////////

BUTTON_TYPES POLY::GetButtonType(
	USHORT 									usIndex)
	{
	I_VERIFY_POINTER(m_ptrButtons, return I_UNKNOWN_TYPE);
	
	if (usIndex < m_usNumButtons)
		return m_ptrButtons[usIndex]->GetType();
	else
		return I_UNKNOWN_TYPE;
	}

//////////

HRGN POLY::GetButtonRegionHandle(
	USHORT 									usIndex)
	{
	I_VERIFY_POINTER(m_ptrButtons, return NULL);

	if (usIndex < m_usNumButtons)
		return m_ptrButtons[usIndex]->GetRegionHandle();
	else
		return NULL;
	}

//////////

BOOL POLY::ButtonClicked(
	USHORT 									usIndex,
	HWND										hwndParent,
	LPPOINT									pPtOffset) 
	{
	BOOL										bSet;
	BOOL										bObjectChanged=FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < GetNumberOfButtons())
		{
		m_usLastButtonClicked = usIndex;
		switch (m_ptrButtons[usIndex]->GetType())
			{
			case I_RADIO:
				if (SetButton(usIndex, hwndParent, pPtOffset))
					bObjectChanged = TRUE;
				break;
			case I_CHECK:
				IsButtonSet(usIndex, &bSet);
				if (bSet)
					ClearButton(usIndex, hwndParent, pPtOffset);
				else
					SetButton(usIndex, hwndParent, pPtOffset);
				bObjectChanged = TRUE;
				break;

			case I_PUSH:
			case I_TRANSPARENT:
				m_ptrButtons[usIndex]->Clicked(m_oiSelf, hwndParent, pPtOffset);
				break;
			}
		}
	
	return bObjectChanged;    
	}

///////////

BOOL POLY::SetButton(
	USHORT 									usIndex,
	HWND										hwndParent,
	LPPOINT									pPtOffset)
	{
	USHORT									us;
	USHORT									usOtherChangedButton = I_UNDEFINED_INDEX;
	BOOL										bSet, rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < m_usNumButtons)
		{
		IsButtonSet(usIndex, &bSet);
		if (!bSet)
			{
			m_ptrButtons[usIndex]->Set(m_oiSelf, hwndParent, pPtOffset);

			if (m_ptrButtons[usIndex]->GetType() == I_RADIO)
				{	//Clear the other radio button
				for (us=0; us<m_usNumButtons; us++)
					if (us != usIndex  &&  m_ptrButtons[us]->GetType() == I_RADIO)
						{
						IsButtonSet(us, &bSet);
						if (bSet)
							{
							m_ptrButtons[us]->Clear(m_oiSelf, hwndParent, pPtOffset);
							usOtherChangedButton = us;
							}
						}
	 			}

	 		PostObjectChanged(m_oiSelf, usIndex, usOtherChangedButton);
			m_usLastButtonClicked = usIndex;
			rVal = TRUE;
			}
		}
	
	return rVal;
	}

//////////

BOOL POLY::SetButton(
	LPSTR 									szName)
	{
	USHORT									us;
	BOOL										rVal = FALSE;

	if (GetButtonIndexByName(&us, szName) )
		rVal = SetButton(us);

	return rVal;
	}

//////////

BOOL POLY::ClearButton(
	USHORT 									usIndex,
	HWND										hwndParent,
	LPPOINT									pPtOffset)
	{
	BOOL										bSet, rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < m_usNumButtons)
		{	
		IsButtonSet(usIndex, &bSet);
		if (bSet)
			{
			m_ptrButtons[usIndex]->Clear(m_oiSelf, hwndParent, pPtOffset);
	 		PostObjectChanged(m_oiSelf, usIndex);
			m_usLastButtonClicked = usIndex;
			rVal = TRUE;
			}
		}
	return rVal;
	}

//////////

BOOL POLY::ClearButton(
	LPSTR 									szName)
	{
	USHORT									us;
	BOOL										rVal = FALSE;

	if (GetButtonIndexByName(&us, szName) )
		rVal = ClearButton(us);

	return rVal;
	}

//////////

BOOL POLY::IsButtonSet(
	USHORT 									usIndex, 
	BOOL  *							pbSet)
	{
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);

	if (usIndex < m_usNumButtons)
		{	
		*pbSet = m_ptrButtons[usIndex]->IsSet();
		rVal = TRUE;
		}
	return rVal;
	}

//////////

BOOL POLY::IsButtonSet(
	LPSTR 									szName, 
	BOOL  *							pbSet)
	{
	USHORT									us;
	BOOL										rVal = FALSE;

	if (GetButtonIndexByName(&us, szName) )
		rVal = IsButtonSet(us, pbSet);

	return rVal;
	}

//////////

BOOL POLY::SetButtonName(
	USHORT 									usIndex, 
	LPSTR 									szNewName)
	{
	BOOL										rVal = FALSE;

	I_VERIFY_POINTER(m_ptrButtons, return FALSE);
	
	if (usIndex < m_usNumButtons)
		{	
		m_ptrButtons[usIndex]->SetName(szNewName);
		rVal = TRUE;
		}
	return rVal;

	}

//////////

BOOL POLY::SetButtonName(
	LPSTR 									szOldName, 
	LPSTR 									szNewName)
	{
	USHORT									us;
	BOOL										rVal = FALSE;

	if (GetButtonIndexByName(&us, szOldName) )
		rVal = SetButtonName(us, szNewName);

	return rVal;
	}

//////////

void POLY::GetImageSize(
	LPPOINT 								ptSize)
	{
	LPBMP_DATA							bd;

	bd = (LPBMP_DATA)AObjLockData(m_oiSelf, I_DATA_NORMAL);
	if (bd)
		{
		ptSize->x = (int)bd->m_bmpIH.biWidth;
		ptSize->y = (int)bd->m_bmpIH.biHeight;
		AObjUnlockData(m_oiSelf, I_DATA_NORMAL);
		}
	else
		ptSize->x = ptSize->y = 0;

	}

