#include "include.h"

POLY_VI::POLY_VI(
	AOBJECTID								oiItem, 
	AOBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	m_usTrackingButtonIndex = I_UNDEFINED_INDEX;
	m_tracking = I_TRACKING_NOT;

	OWndSetItemActivatable(this, FALSE);
	OWndSetItemEnabled(this, TRUE );

	SetTransparent();

	}

//////////////

void ALMAPI POLY_VI::Destroy()
	{
	delete this;
	}

//////////////

void ALMAPI POLY_VI::DataChanged(
	long 										details, 
	long 										modifiers, 
	long 										)
	{
	LPPOLY									poly;
	POINT										ptOffset;

	SetTransparent();
	
	ptOffset.x = m_rcItem.left;
	ptOffset.y = m_rcItem.top;
	OWndLPtoDP(m_oiParent, &ptOffset, 1);

	if (m_hwndParent)
		{
		poly = LockPoly(Object());
		I_VERIFY_POINTER(poly, return);

		poly->InvalidateButton((USHORT)details, m_hwndParent, TRUE, &ptOffset);

		if (modifiers != I_UNDEFINED_INDEX)
			poly->InvalidateButton((USHORT)modifiers, m_hwndParent, TRUE, &ptOffset);

		UnlockPoly(Object());
		}

	}

//////

void ALMAPI POLY_VI::ParentOpened()
	{
	LPPOLY									poly;

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);
	poly->CreateBitmapHandles();
	poly->CreateButtons();
	UnlockPoly(Object());
	}

//////////////

void ALMAPI POLY_VI::ParentClosing()
	{
	LPPOLY									poly;

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);
	poly->DestroyBitmapHandles();
	poly->DestroyButtons();
	UnlockPoly(Object());
	}

//////////////

long ALMAPI POLY_VI::AdjustNewRect(
	LPRECT 									rcNew, 
	long 										adjustFlag)
	{
	LPPOLY									poly;
	POINT										ptImageSize;
	int											iTmp;
	long										rVal=0L;

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return 0);
	poly->GetImageSize(&ptImageSize);
	UnlockPoly(Object());

	if (ptImageSize.x  &&  ptImageSize.y)
		{
		OWndLStoDS(m_oiParent, (LPPOINT)rcNew, 2); //convert to pixels

		//Make sure the rect is not inside-out
		if (rcNew->bottom < rcNew->top)
			{
			iTmp = rcNew->bottom;
			rcNew->bottom = rcNew->top;
			rcNew->top = iTmp;
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

		if (rcNew->right < rcNew->left)
			{
			iTmp = rcNew->right;
			rcNew->right = rcNew->left;
			rcNew->left = iTmp;
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

		//Make sure it is the right height
		if (rcNew->bottom - rcNew->top != ptImageSize.y)
			{
			if (adjustFlag & OWND_ADJRECT_ANCHOR_B)	
				{
				rcNew->top = rcNew->bottom - ptImageSize.y;
				rVal |= OWND_ADJRECT_CHANGED_T;
				}
			else
				{
				rcNew->bottom = rcNew->top + ptImageSize.y;
				rVal |= OWND_ADJRECT_CHANGED_B;
				}
			}

		//Make sure it is the right width
		if (rcNew->right - rcNew->left != ptImageSize.x)
			{
			if (adjustFlag & OWND_ADJRECT_ANCHOR_R)	
				{
				rcNew->left = rcNew->right - ptImageSize.x;
				rVal |= OWND_ADJRECT_CHANGED_L;
				}
			else
				{
				rcNew->right = rcNew->left + ptImageSize.x;
				rVal |= OWND_ADJRECT_CHANGED_R;
				}
			}
		
		OWndDStoLS(m_oiParent, (LPPOINT)rcNew, 2); //back to logical points
		}
	else
		{//no image size -- no bitmap loaded
		rVal = OpLiteWndItem::AdjustNewRect(rcNew, adjustFlag);
		}

	return rVal;
	}

//////////////

void ALMAPI POLY_VI::Draw(
	HDC											hdc, 
	LPRECT 									rcInvalid)
	{
	LPPOLY									poly;
	RECT										rcDS;
	RECT										rcDP;
	HDC											hdcMem;
	HDC											hdcScreen;
	HBITMAP									hbmpOld;
	int											iSavedDC;
	USHORT									us;
	HRGN										hrgnButton;
	HRGN										hrgnRect;
	BOOL										bSet, bSelectClipWorked, bTrackingInThisOne, bHaveHighlightImage = FALSE;
	HBRUSH									hBrush;
	HPEN										hPen;

	I_UNUSED(rcInvalid);

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);

	if (poly->GetNormalBitmap())
		{
		CopyRect(&rcDS, &m_rcItem);
		CopyRect(&rcDP, &m_rcItem);

		OWndLPtoDP(m_oiParent, (LPPOINT)&rcDP, 2);
		OWndLStoDS(m_oiParent, (LPPOINT)&rcDS, 2);

		hrgnRect = CreateRectRgnIndirect(&rcDP);

		hdcScreen = GetDC(NULL);
		hdcMem = CreateCompatibleDC(hdcScreen);

		iSavedDC = SaveDC(hdc);  //save the dc
		SetMapMode(hdc, MM_TEXT); 

		//First, bitblt the normal image
		hbmpOld = (HBITMAP)SelectObject(hdcMem, poly->GetNormalBitmap());
		BitBlt(hdc, rcDS.left, rcDS.top, rcDS.right - rcDS.left, rcDS.bottom - rcDS.top,
						hdcMem, 0, 0, SRCCOPY);

		
		if (poly->GetHighlightBitmap())
			{// if we have a bitmap for the highlight image
	 		SelectObject(hdcMem, poly->GetHighlightBitmap());
			bHaveHighlightImage = TRUE;
			}

		//Now, bitblt in the highlights
		for(us=0; us<poly->GetNumberOfButtons(); us++)
			{
			poly->IsButtonSet(us, &bSet);
			bTrackingInThisOne = (us == m_usTrackingButtonIndex && m_tracking == I_TRACKING_INSIDE);
			if (bSet || (bTrackingInThisOne && poly->GetButtonType(us)==I_PUSH) )  
				{
				if ( (hrgnButton = poly->GetButtonRegionHandle(us)) != NULL)
					{

					if (bHaveHighlightImage)
						{//blt in the bitmap
						OffsetRgn(hrgnButton, rcDP.left, rcDP.top);
						bSelectClipWorked = SelectClipRgn(hdc, hrgnButton);
						OffsetRgn(hrgnButton, -rcDP.left, -rcDP.top);

						if (bSelectClipWorked  && IntersectClipRect(hdc, rcDS.left, rcDS.top, rcDS.right, rcDS.bottom) )
							BitBlt(hdc, rcDS.left, rcDS.top, rcDS.right - rcDS.left, rcDS.bottom - rcDS.top, hdcMem, 0, 0, SRCCOPY);
						else
							{
							SelectClipRgn(hdc, hrgnRect);
							OffsetRgn(hrgnButton, rcDS.left, rcDS.top);
							InvertRgn(hdc, hrgnButton);  //cause we couldn't bitblt into it
							OffsetRgn(hrgnButton, -rcDS.left, -rcDS.top);
							}
						}
					else
						{
						OffsetRgn(hrgnButton, rcDS.left, rcDS.top);
						InvertRgn(hdc, hrgnButton); //cause we don't have a highlight image
						OffsetRgn(hrgnButton, -rcDS.left, -rcDS.top);
						}

					}
				}// if (bSet || ....)

			if (bTrackingInThisOne && (poly->GetButtonType(us)==I_RADIO || poly->GetButtonType(us)==I_CHECK) )
				{
				SelectClipRgn(hdc, hrgnRect);
				poly->FrameButton(us, hdc, (LPPOINT)&rcDS);
				}

			}

		RestoreDC(hdc, iSavedDC);

		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hrgnRect);
		}//if (poly->GetNormalBitmap())
	else
		{
		if (OWndIsLayoutTime())
			{
			hBrush = (HBRUSH) SelectObject(hdc, GetStockObject(NULL_BRUSH)); 
			hPen = (HPEN) SelectObject(hdc, GetStockObject(BLACK_PEN));
			Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
			SelectObject(hdc, hBrush);
			SelectObject(hdc, hPen); 
			}
		}

	UnlockPoly(Object());
	}

//////////////
void ALMAPI POLY_VI::MouseDown(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPPOLY									poly;
	POINT										ptMouse, ptOffset;


	I_UNUSED(keysDown);

	if (button != MK_LBUTTON)
		return;

	ptMouse.x = where->x - m_rcItem.left;
	ptMouse.y = where->y - m_rcItem.top;
	OWndLStoDS(m_oiParent, &ptMouse, 1);

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);

	if ( (m_usTrackingButtonIndex = poly->PointInWhichButton(&ptMouse)) != I_UNDEFINED_INDEX)
		{
		OWndSetCaptureItem(this);
		SetTrackingMode(I_TRACKING_INSIDE);

		ptOffset.x = m_rcItem.left;
		ptOffset.y = m_rcItem.top;
		OWndLPtoDP(m_oiParent, &ptOffset, 1);
		poly->InvalidateButton(m_usTrackingButtonIndex, m_hwndParent, TRUE, &ptOffset);
		}

	UnlockPoly(Object());
	}

//////////////
void ALMAPI POLY_VI::MouseMove(
	POINT* 									where, 
	int 										keysDown)
	{
	LPPOLY									poly;
	POINT										ptMouse, ptOffset;
	BOOL										bErase;

	I_UNUSED(keysDown);

	if (GetTrackingMode() == I_TRACKING_NOT)
		return;

	ptMouse.x = where->x - m_rcItem.left;
	ptMouse.y = where->y - m_rcItem.top;
	OWndLStoDS(m_oiParent, &ptMouse, 1);

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);

	if (m_usTrackingButtonIndex != poly->PointInWhichButton(&ptMouse) )
		bErase = SetTrackingMode(I_TRACKING_OUTSIDE);
	else
		bErase = SetTrackingMode(I_TRACKING_INSIDE);

	if (bErase)
		{
		ptOffset.x = m_rcItem.left;
		ptOffset.y = m_rcItem.top;
		OWndLPtoDP(m_oiParent, &ptOffset, 1);
		poly->InvalidateButton(m_usTrackingButtonIndex, m_hwndParent, TRUE, &ptOffset);
		}
	
	UnlockPoly(Object());
	}

//////////////

void ALMAPI POLY_VI::MouseUp(
	POINT* 									where, 
	int 										keysDown, 
	int 										button)
	{
	LPPOLY									poly;
	POINT										ptMouse, ptOffset;

	I_UNUSED(keysDown);

	if (button != MK_LBUTTON  ||  m_tracking == I_TRACKING_NOT)
		return;

	SetTrackingMode(I_TRACKING_NOT);
	OWndReleaseCapture();

	ptMouse.x = where->x - m_rcItem.left;
	ptMouse.y = where->y - m_rcItem.top;
	OWndLStoDS(m_oiParent, &ptMouse, 1);

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);

	if (poly->PointInWhichButton(&ptMouse) == m_usTrackingButtonIndex)
		{
		ptOffset.x = m_rcItem.left;
		ptOffset.y = m_rcItem.top;
		OWndLPtoDP(m_oiParent, &ptOffset, 1);
		poly->ButtonClicked(m_usTrackingButtonIndex, m_hwndParent, &ptOffset);
		}
	else
		poly->InvalidateButton(m_usTrackingButtonIndex, m_hwndParent, TRUE, &ptOffset);

	UnlockPoly(Object());

	m_usTrackingButtonIndex = I_UNDEFINED_INDEX;
	}

//////////////

void ALMAPI POLY_VI::CancelMode()
	{
	LPPOLY									poly;
	POINT										ptOffset;

	if (GetTrackingMode() != I_TRACKING_NOT)
		{
		OWndReleaseCapture();

		poly = LockPoly(Object());
		I_VERIFY_POINTER(poly, return);

		ptOffset.x = m_rcItem.left;
		ptOffset.y = m_rcItem.top;
		OWndLPtoDP(m_oiParent, &ptOffset, 1);

		poly->InvalidateButton(m_usTrackingButtonIndex, m_hwndParent, TRUE, &ptOffset);
		UnlockPoly(Object());

		SetTrackingMode(I_TRACKING_NOT);
		m_usTrackingButtonIndex = I_UNDEFINED_INDEX;
		}

	}

////////

BOOL POLY_VI::SetTrackingMode(
	TRACKING_MODE 					track)
	{
	BOOL										rVal = FALSE;

	if (m_tracking != track)
		{
		m_tracking = track;
		rVal = TRUE;
		}
	return rVal;
	}

////////

void POLY_VI::SetTransparent()
	{
	LPPOLY									poly;
	POINT										ptImageSize;

	poly = LockPoly(Object());
	I_VERIFY_POINTER(poly, return);
	poly->GetImageSize(&ptImageSize);
	UnlockPoly(Object());

	if (  (m_rcItem.right - m_rcItem.left > ptImageSize.x)  ||
				(m_rcItem.bottom - m_rcItem.top > ptImageSize.y) )
		OWndSetItemTransparent(this, TRUE);
	else
		OWndSetItemTransparent(this, FALSE);
	}


extern "C" OpWndItemD* ALMAPI	CreateVisualInstance(
	AOBJECTID								oiItem,
	AOBJECTID								oiParent,
	RECT*										rcItem,
	long										status,
	long										special)
	{
  OpWndItemD*							wi;

	I_UNUSED(special);	

	wi = new POLY_VI(oiItem, oiParent, rcItem, status);

	return wi;
	}
