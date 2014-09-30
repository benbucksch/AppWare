#include "b_includ.h"


///////////////////////

BITMAP_VI::BITMAP_VI(
	AOBJECTID 								oiItem, 
	AOBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	LPBUTTON								but;

	m_hbmpNormal = m_hbmpFocused = m_hbmpPressed = m_hbmpGrayed = NULL;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	m_bTracking = FALSE;

	ChangeState(but->IsButtonEnabled() ? I_NORMAL : I_GRAYED);

	OWndSetItemActivatable(this, TRUE);
	OWndSetItemEnabled(this, but->IsButtonEnabled() );

	SetAccelKey();

	UnlockButtonObject(Object());

	}

///////////////////////

void ALMAPI BITMAP_VI::Destroy()
	{
	I_ASSERT(!m_hbmpNormal && !m_hbmpFocused && !m_hbmpPressed && !m_hbmpGrayed);
	delete this;
	}

///////////////////////////

void ALMAPI BITMAP_VI::DataChanged(
	long 										details, 
	long 										modifiers, 
	long 										variation)
	{
	I_UNUSED(details);I_UNUSED(modifiers);I_UNUSED(variation);

	InvalidateItem(TRUE);
	}
////////////////////

void ALMAPI BITMAP_VI::ParentOpened(void)
	{
	//CreateBitmapHandles();
	}
////////////////////

void ALMAPI BITMAP_VI::ParentClosing(void)
	{
	DestroyBitmapHandles();
	}
////////////////////

void ALMAPI BITMAP_VI::AccelKeyHit(void)
	{
	AEvtPostSignalAtTail(Object(), I_SIG_PUSHED);
	}

////////////////////

long ALMAPI BITMAP_VI::Activate(
	int 										causedBy, 
	BOOL 										mustKeep)
	{
	I_UNUSED(causedBy); I_UNUSED(mustKeep);

	ChangeState(I_FOCUSED);

	return OWND_ACTIVATION_KEPT;
	}

////////////////////
void ALMAPI BITMAP_VI::Deactivated()
	{
	LPBUTTON								but; 
	
	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	ChangeState(but->IsButtonEnabled() ? I_NORMAL : I_GRAYED);

	I_ASSERT(m_bTracking?FALSE:TRUE);
		
  UnlockButtonObject(Object());
	}

////////////////////

void ALMAPI BITMAP_VI::Enable(
	BOOL 										bEnable)
	{
	ChangeState(bEnable ? OWndGetActiveItem()==this ? I_FOCUSED : I_NORMAL : I_GRAYED);
	}

////////////////////

BOOL ALMAPI BITMAP_VI::QueryDesiredPalette(
	AMEMBLOCKID 							miPalette)
	{
	LPBMP_DATA							bmpData;

	bmpData = LockNormalBitmap(Object());
	I_VERIFY_POINTER(bmpData, return 0);

	bmpData->GetPaletteForBitmap(miPalette);
	
	UnlockNormalBitmap(Object());

	return TRUE;
	}

////////////////////

void ALMAPI BITMAP_VI::Move(
	LPRECT 									rcNew)
	{
	OpLiteWndItem::Move(rcNew);

	if (m_hwndParent)  //If the parent window is open
		CreateBitmapHandles();
	}

////////////////////

long ALMAPI BITMAP_VI::AdjustNewRect(
	LPRECT 									rcNew, 
	long 										adjustFlag)
	{
	POINT										ptSize, ptBmpSize;
	long										rVal=0L;
	int											tmp;


	if (adjustFlag & OWND_ADJRECT_DEFAULT_H)
  	{
		GetBitmapSize(NULL, &ptBmpSize, &ptSize);
    if(adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      rcNew->top = rcNew->bottom - ptBmpSize.y;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      rcNew->bottom = rcNew->top + ptBmpSize.y;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
		}
	else
		{
		if (rcNew->bottom < rcNew->top)
			{
			tmp = rcNew->bottom;
			rcNew->bottom = rcNew->top;
			rcNew->top = tmp;
			rVal |= OWND_ADJRECT_SWAPPED_TB;
			if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
				{
				adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
				adjustFlag |= OWND_ADJRECT_ANCHOR_T;
				}
			else
				{
				adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
				adjustFlag |= OWND_ADJRECT_ANCHOR_B;
				}
			}
		
		GetBitmapSize(rcNew, &ptBmpSize, &ptSize);

		if (rcNew->bottom - rcNew->top != ptSize.y)
			{
			if (adjustFlag & OWND_ADJRECT_ANCHOR_T)
				{ //The top should be anchored
				rcNew->bottom = rcNew->top + ptSize.y;
				rVal |= OWND_ADJRECT_CHANGED_B;
				}
			else 
				{
				rcNew->top = rcNew->bottom - ptSize.y;
				rVal |= OWND_ADJRECT_CHANGED_T;
				}
			}
		}

	if (adjustFlag & OWND_ADJRECT_DEFAULT_W)
		{
		GetBitmapSize(NULL, &ptBmpSize, &ptSize);
    if(adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      rcNew->left = rcNew->right - ptBmpSize.x;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      rcNew->right = rcNew->left + ptBmpSize.x;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
		}
	else 
		{
		if (rcNew->right < rcNew->left)
			{
			tmp = rcNew->right;
			rcNew->right = rcNew->left;
			rcNew->left = tmp;
			rVal |= OWND_ADJRECT_SWAPPED_LR;
			if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
				{
				adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
				adjustFlag |= OWND_ADJRECT_ANCHOR_L;
				}
			else
				{
				adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
				adjustFlag |= OWND_ADJRECT_ANCHOR_R;
				}
			}

		GetBitmapSize(rcNew, &ptBmpSize, &ptSize);

		if (rcNew->right - rcNew->left != ptSize.x)
			{
			if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
				{//the right should be anchored
				rcNew->left = rcNew->right - ptSize.x;
				rVal |= OWND_ADJRECT_CHANGED_L;
				}
			else
				{
				rcNew->right = rcNew->left + ptSize.x;
				rVal |= OWND_ADJRECT_CHANGED_R;
				}
			}
		}

	OWndLStoDS(m_oiParent, (LPPOINT)rcNew, 2);
	OWndDStoLS(m_oiParent, (LPPOINT)rcNew, 2);

	return rVal;
	}


////////////////////

void ALMAPI BITMAP_VI::Draw(
	HDC 										hdc, 
	LPRECT 									rcInvalid)
	{
	HDC											hdcMem;
	HDC											hdcScreen;
	HBITMAP									hbmpOld;
	RECT										rect;

	I_UNUSED(rcInvalid);

	if (!m_hbmpNormal)
		CreateBitmapHandles();

	hdcScreen = GetDC(NULL);
	hdcMem = CreateCompatibleDC(hdcScreen);

	switch(m_state)
		{
		case I_FOCUSED:	hbmpOld = (HBITMAP)SelectObject(hdcMem, m_hbmpFocused); break;
		case I_PRESSED:	hbmpOld = (HBITMAP)SelectObject(hdcMem, m_hbmpPressed); break;
		case I_GRAYED:	hbmpOld = (HBITMAP)SelectObject(hdcMem, m_hbmpGrayed); break;
		default:				hbmpOld = (HBITMAP)SelectObject(hdcMem, m_hbmpNormal); break;
		}

	CopyRect(&rect, &m_rcItem);
	OWndLPtoDP(m_oiParent, (LPPOINT)&rect, 2);
	
	StretchBlt(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top,
					hdcMem, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);

	SelectObject(hdcMem, hbmpOld);

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
	}

////////////////////

void ALMAPI BITMAP_VI::KeyDown(
	int 										vKey, 
	char 										ascii, 
	long 										keyData)
	{
	LPBUTTON								but;

	I_UNUSED(ascii); I_UNUSED(keyData);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	if (!but->IsButtonCapturedByKey())
		{
		if (vKey == VK_SPACE  ||  vKey == VK_RETURN)
			{
			m_bTracking = TRUE;
			ChangeState(I_PRESSED);
			but->SetButtonIsCapturedByKey(TRUE);
			but->BeginTrackingButton(this);
			but->PostTouchedSignal();
			}
		else if (	isalnum(vKey) )
			OWndSimulateAccelKey(m_oiParent, OTYPE_BUTTON, ascii, OWND_ALT);
		}

	UnlockButtonObject(Object());
	}

////////////////////

void ALMAPI BITMAP_VI::KeyUp(
	int 										vKey, 
	char 										ascii, 
	long 										keyData)
	{
	LPBUTTON								but;

	I_UNUSED(ascii); I_UNUSED(keyData);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	if (but->IsButtonCapturedByKey())
		{
		if (vKey == VK_SPACE  ||  vKey == VK_RETURN)
			{
			m_bTracking = FALSE;
			ChangeState(I_FOCUSED);
			but->SetButtonIsCapturedByKey(FALSE);
			but->EndTrackingButton();
			but->PostPushedSignal();
			}
		}

	UnlockButtonObject(Object());
	}

////////////////////

void ALMAPI BITMAP_VI::MouseMove(
	POINT* 									where, 
	int 										keysDown)
	{
	I_UNUSED(keysDown);

	if (m_bTracking)
		ChangeState(PointInItem(where) ? I_PRESSED: I_FOCUSED);
	}

////////////////////

void ALMAPI BITMAP_VI::MouseDown(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(where);I_UNUSED(keysDown);

	if (button == MK_LBUTTON)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		OWndSetCaptureItem(this);
		m_bTracking = TRUE;
		m_bDoubleClicked = FALSE;

		ChangeState(I_PRESSED);
		but->PostTouchedSignal();
		but->BeginTrackingButton(this);
		but->SetButtonIsCapturedByKey(FALSE);

		UnlockButtonObject(Object());
		}
	}
////////////////////

void ALMAPI BITMAP_VI::DoubleClicked(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(where);I_UNUSED(keysDown);

	if (button == MK_LBUTTON)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		OWndSetCaptureItem(this);
		m_bTracking = TRUE;
		m_bDoubleClicked = TRUE;

		ChangeState(I_PRESSED);
		but->PostTouchedSignal();
		but->BeginTrackingButton(this);
		but->SetButtonIsCapturedByKey(FALSE);

		UnlockButtonObject(Object());
		}
	}
////////////////////

void ALMAPI BITMAP_VI::MouseUp(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPBUTTON								but;

	I_UNUSED(where);I_UNUSED(keysDown);

	if (button == MK_LBUTTON  &&  m_bTracking)
		{
		but = LockButtonObject(Object());
		I_ASSERT_POINTER(but, return);

		but->EndTrackingButton();
		but->PostMouseUpSignal();
		if (PointInItem(where))
			{
			if (m_bDoubleClicked)
				but->PostDoubleClickedSignal();
			else
				but->PostPushedSignal();
			}

		UnlockButtonObject(Object());

		OWndReleaseCapture();
		m_bTracking = FALSE;
		ChangeState(I_FOCUSED);
		}
	}

////////////////////

void ALMAPI BITMAP_VI::CancelMode(void)
	{
	LPBUTTON								but;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);
	
	if (but->IsBeingTracked())
		{
		but->EndTrackingButton();
		but->PostMouseUpSignal();	
		}
	
	UnlockButtonObject(Object());

	OWndReleaseCapture();

	if (m_bTracking)
		{
		OWndReleaseCapture();
		m_bTracking = FALSE;
		ChangeState(I_NORMAL);
		}
	}

////////////////////		

void BITMAP_VI::GetBitmapSize(
	LPRECT 									rcRequested, 
	LPPOINT 								ptBmpSize,
	LPPOINT									ptSize)
	{
	LPBUTTON								but;
	LPBMP_DATA							bmpData;
	float										fl;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	if (but->DoesNormalBitmapExist())
		{
		bmpData = LockNormalBitmap(Object());
		I_ASSERT_POINTER(bmpData, UnlockButtonObject(Object());return);

		ptBmpSize->x = (int)bmpData->m_bmpIH.biWidth;
		ptBmpSize->y = (int)bmpData->m_bmpIH.biHeight;
		OWndDStoLS(m_oiParent, ptBmpSize, 1);
		UnlockNormalBitmap(Object());

		if (rcRequested)
			{
			switch(but->GetBitmapStyle())
				{
				case I_CROP:			
					ptSize->x = ptBmpSize->x;
					ptSize->y = ptBmpSize->y;
					break;

				case I_ORIGPROP:
					ptSize->x = rcRequested->right - rcRequested->left;
					ptSize->y = rcRequested->bottom - rcRequested->top;
					
					fl = max(1.0, (float)ptSize->x / (float)ptBmpSize->x + 0.5);
					ptSize->x = (int)fl;
					ptSize->x *= ptBmpSize->x;

					fl = max(1.0, (float)ptSize->y / (float)ptBmpSize->y + 0.5);
					ptSize->y = (int)fl;
					ptSize->y *= ptBmpSize->y;

					break;

				default:
					ptSize->x = rcRequested->right - rcRequested->left;
					ptSize->y = rcRequested->bottom - rcRequested->top;
				}//switch
			}//if (rcRequested)
		}//if	(but->DoesNormalBitmapExist())
	else
		{
		ptBmpSize->x = I_DEFAULT_BM_WIDTH;
		ptBmpSize->y = I_DEFAULT_BM_HEIGHT;
		OWndDStoLS(m_oiParent, ptBmpSize, 1);
		ptSize->x = ptBmpSize->x;
		ptSize->y = ptBmpSize->y;
		}

	UnlockButtonObject(Object());
	}

////////////////////	 

BOOL BITMAP_VI::ChangeState(
	BITMAP_STATE 						newState)
	{
	BOOL										rVal = FALSE;

	if (newState != m_state)
		{
		m_state = newState;
		rVal = TRUE;
		InvalidateItem(FALSE);
		}

	return rVal;
	}


void BITMAP_VI::CreateBitmapHandles()
	{
	LPBUTTON								but;
	RECT										rect;
	AMEMBLOCKID							miPalette;
	LOGPALETTE*							palette;
	HPALETTE								hPalette = NULL;

	DestroyBitmapHandles(); //Delete the old handles first

	miPalette = AMemAllocate(0);
	if ( OWndGetPalette(m_oiParent, miPalette) )
		{
		palette = (LOGPALETTE*)AMemLock(miPalette);
		I_VERIFY_POINTER(palette, return);
		hPalette = CreatePalette(palette);
		AMemUnlock(miPalette);
		}
	AMemFree(miPalette);

	CopyRect(&rect, &m_rcItem);
	OWndLStoDS(m_oiParent, (LPPOINT)&rect, 2);

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return);

	but->CreateBitmapsHandles(hPalette, &rect, &m_hbmpNormal, &m_hbmpFocused, &m_hbmpPressed, &m_hbmpGrayed);

	if (hPalette)
		DeleteObject(hPalette);

	UnlockButtonObject(Object());
	}

////////////////////

void BITMAP_VI::DestroyBitmapHandles()
	{
	if(m_hbmpNormal)
		DeleteObject(m_hbmpNormal);			 

	if(m_hbmpFocused)
		DeleteObject(m_hbmpFocused);		 

	if(m_hbmpPressed)	
		DeleteObject(m_hbmpPressed);		 

	if(m_hbmpGrayed)		
		DeleteObject(m_hbmpGrayed);			 

	m_hbmpNormal = m_hbmpFocused = m_hbmpPressed = m_hbmpGrayed = 0;
	}

////////////////////

BOOL ALMAPI BITMAP_VI::AppearanceChanged(
	BITMAP_VI *							vi,
	long 										fwWhatChanged,
	long										)
	{
	LPBUTTON								but;

	but = LockButtonObject(vi->Object());
	I_ASSERT_POINTER(but, return TRUE);

	if (fwWhatChanged & I_APPEARANCE_CHANGED_ENABLE)
		OWndSetItemEnabled(vi, but->IsButtonEnabled() );

	UnlockButtonObject(vi->Object());

	return TRUE;
	}

///////////

BOOL BITMAP_VI::SetAccelKey()
	{
	LPBUTTON								but;
	BOOL										rVal=FALSE;

	but = LockButtonObject(Object());
	I_ASSERT_POINTER(but, return FALSE);

	if (but->GetAccelVKey())
		rVal = OWndSetItemAccelKey(this, but->GetAccelVKey(), but->GetAccelMKey());

	UnlockButtonObject(Object());

	return rVal;
	}
