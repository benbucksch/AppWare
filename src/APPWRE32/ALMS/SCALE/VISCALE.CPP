/*//////////////////////////////////////////////////////////////
//
//      Copyright 1993, Novell, Inc.  All rights reserved
//
//      THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//      PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//      ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//      A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//      THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//      ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//      WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//      REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//      COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//      WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//      EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//      THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define 	STRICT
#define  	ASTRICT

#include <math.h>
#include "a_alm.h"
#include "op_wnd.h"
#include "o_wnd.h"
#include "o_number.h"
#include "scale.h"
#include "intdebug.h"

#define round(x) floorl(x+0.5)
/**************************************************************************
* Function: vScaleCreate
* Description: A pointer to this function is passed back to the Window  Object
*              The Window Object calls this for every v-instance of the picture
*              item.  The constructor to VI_SCALE is called to create and init
*              a visual instance of scale's c++ class.  The methods in this
*              class provide the implementation for the Window Item Protocol
*
**************************************************************************/
OpWndItemD* ALMAPI vScaleCreate(
	AOBJECTID                oiItem,
	AOBJECTID                oiParent,
	RECT*                           rcItem,
	long                            status,
	long                            special
	)
	{
	I_UNUSED(special);
	return new VI_SCALE(oiItem, oiParent, rcItem, status);
	}


VI_SCALE::VI_SCALE(
	AOBJECTID oid,
	AOBJECTID parentID,
	RECT*            rcItem,
	long             statusFlag)
	:OpLiteWndItem(oid, parentID, rcItem, statusFlag)
{
  hBackground = NULL ;
	KeyHit      = FALSE ;
	scale =(SCALE*) AObjGetRuntimeInfo(oid) ;
	scale->mousedown = FALSE; 
	scale->GetBitmapHandle() ;
	SetDefaultRectValue () ;
	min = GetValue (scale->data0->min) ;
	max = GetValue (scale->data0->max) ;
	val = GetValue (scale->data0->val) ;
	ValidateValues() ;
	scale->GetBitmapHandle() ;
	if (scale->indData.hIndicator)
	  {
		hMaskBitmap = CreateMaskBitmap (scale->indData.hIndicator) ;
		hInvertBitmap = CreateInvertBitmap (scale->indData.hIndicator, hMaskBitmap) ;
		}
	Width = scale->indData.rect.right ;
	Height = scale->indData.rect.bottom ;
	if (!OWndIsLayoutTime())
		{
		if (val)
			{
			POINT point = ValueToPosition (val) ;
			OffsetRect (&scale->indData.rect,point.x, point.y) ;
			}
		else
    	scale->indData.rect = minRect ; //scale->indData.minRect ;
    }

	OWndSetItemTransparent(this, TRUE);
	OWndSetItemEnabled (this, TRUE) ;
	if (scale->CanActivate())
		{
		OWndSetItemActivatable(this, TRUE);
		OWndSetItemUsesTab(this, TRUE);
		OWndSetItemUsesCursorKeys (this, TRUE) ;
    }
	else {
		OWndSetItemActivatable(this, FALSE);
		}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
VI_SCALE::~VI_SCALE (void)
{
	if (hBackground)
		DeleteObject (hBackground) ;

	if (hMaskBitmap)
		DeleteObject (hMaskBitmap) ;
	if (hInvertBitmap)
		DeleteObject (hInvertBitmap) ;
}
void ALMAPI VI_SCALE::Destroy(void)
{
	delete this ;
}

/*********************************************************
*
*
*********************************************************/
long ALMAPI VI_SCALE::Activate(int causedBy, BOOL mustKeep)
{
	I_UNUSED(causedBy);
	I_UNUSED(mustKeep);

	if (scale->CanActivate())
		{
 		HDC hdc = GetDC (m_hwndParent) ;
		int mode = GetMapMode(hdc);
		int sDC  = SaveDC(hdc);
		mode = SetMapMode(hdc,MM_TEXT);
		RECT layRect = m_rcItem ;
		OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;
		RECT  ghostRect = scale->indData.rect ;
		if (CorrectRect(&ghostRect, &layRect))
 			{
			HBRUSH hBrush = (HBRUSH)CreatePatternBrush((HBITMAP)hBORDER);
			FrameRect(hdc, &ghostRect, hBrush);
			DeleteObject(hBrush);
    	}
		SetMapMode(hdc, mode);
		RestoreDC(hdc, sDC);
		ReleaseDC (m_hwndParent, hdc) ;
		}
	return  OWND_ACTIVATION_KEPT;
}

/*********************************************************
*
*
*********************************************************/
void ALMAPI VI_SCALE::Deactivated(void)
{
	//RECT layRect=m_rcItem ;
	//RECT ghostRect = scale->indData.rect; ;

	if (scale->CanActivate())
		{
 		HDC hdc = GetDC (m_hwndParent) ;
		int mode = GetMapMode(hdc);
		int sDC  = SaveDC(hdc);
		mode = SetMapMode(hdc,MM_TEXT);
		RECT layRect = m_rcItem ;
		OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;
		RECT  ghostRect = scale->indData.rect ;
		if (CorrectRect(&ghostRect, &layRect))
 			{
	  	GetSysColor(COLOR_HIGHLIGHTTEXT);
			HBRUSH hBrush = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			FrameRect(hdc, &ghostRect, hBrush);
			DeleteObject(hBrush);
    	}
		SetMapMode(hdc, mode);
		RestoreDC(hdc, sDC);
		ReleaseDC (m_hwndParent, hdc) ;
		InvalidateRect(m_hwndParent, &ghostRect, FALSE);
		UpdateWindow(m_hwndParent);
		}
}
/***************************************************************************

***************************************************************************/
void ALMAPI VI_SCALE::KeyDown(
	int vKey,
	char ascii,
	long keyData)
{
  long lastVal;

  lastVal = val ;
	KeyHit = TRUE ;
	switch(vKey)
    {
		case VK_RIGHT:
         {
         val = lastVal+ADDONE ;
				 if (val>max)
				   KeyHit = FALSE ;
         ValidateValues () ;
         }
         break ;
    case VK_LEFT:
         {
         val = lastVal-ADDONE ;
				 if (val<min)
					 KeyHit = FALSE ;
         ValidateValues () ;
         }
				 break ;
    case VK_HOME:
         {
				 if (lastVal==min)
					KeyHit = FALSE ;
				 else
         	val = min ;
         ValidateValues () ;
         }
         break ;
    case VK_END:
         {
				 if (lastVal==max)
					KeyHit = FALSE ;
				 else
         	val = max ;
         ValidateValues () ;
         }
         break ;
    case VK_UP:
         {
         val = lastVal-ADDONE ;
         ValidateValues () ;
         }
         break ;
    case VK_DOWN:
         val = lastVal+ADDONE ;
         ValidateValues () ;
         break ;
    }
		SetValue (scale->GetObject(2), val) ;

		I_UNUSED(ascii);
		I_UNUSED(keyData);
}
void ALMAPI VI_SCALE::Draw(HDC hdc, RECT* invalidRect)
{
  VRect = m_rcItem ;
	OWndLStoDS(m_oiParent, (LPPOINT)&(VRect), 2) ;
	int mode = GetMapMode(hdc);
	int sDC  = SaveDC(hdc);
	mode = SetMapMode(hdc,MM_TEXT);

	if (OWndIsLayoutTime())
		{
		RepaintScale (hdc, TRUE) ;
    FrameRect (hdc, &VRect, (HBRUSH)GetStockObject (BLACK_BRUSH)) ;
		}
	else
		{
		RepaintScale (hdc, FALSE) ;
		DrawActiveItem(hdc) ;
		}

	SetMapMode(hdc, mode);
	RestoreDC(hdc, sDC);
	I_UNUSED(invalidRect);
}
void ALMAPI VI_SCALE::DataChanged(
	long details,
	long modifiers,
	long variation)
{
	BOOL change = FALSE ;
	double long lastmin = min ;
	double long lastmax = max ;
  double long lastval = val ;
	//MessageBox (NULL, "number object has changed","", MB_OK) ;
	if (lastmin!=(min=GetValue (scale->data0->min)))
		change = TRUE ;
	if (lastmax!=(max=GetValue (scale->data0->max)))
		change = TRUE ;
	if (lastval!=(val=GetValue (scale->data0->val)))
		change = TRUE ;
	if (KeyHit)
	 	{
		change = TRUE ;
		KeyHit = FALSE ;  
		}
	if (change)
  	{
		ValidateValues() ;
    RECT layRect = m_rcItem ;
		OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;
		RedrawIndicator(m_hwndParent,layRect) ;
		//OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
		AEvtPostSignalAtMark (scale->oiScale, 1) ;
		} 
	I_UNUSED(details);
	I_UNUSED(modifiers);
	I_UNUSED(variation);

}

void ALMAPI VI_SCALE::MouseDown(
	POINT  *where,
	int     keysDown,
	int     button)
{
	RECT layRect = m_rcItem ;
  POINT mouse = *where ;

	I_UNUSED(keysDown);
	I_UNUSED(button);

	if (scale->mousedown)
		return ;

	OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;
	OWndLStoDS(m_oiParent, &mouse, 1) ;
	scale->mouse_save.x = mouse.x-layRect.left ;
	scale->mouse_save.y = mouse.y-layRect.top ;
	if (scale->data0->canDrag)
		{
		if (CheckInBitmap (scale->indData.rect, scale->mouse_save, &scale->cursor_pos))
			{
			OWndSetCaptureItem(this);
			scale->mousedown = TRUE ;
			}
   	}			
}

void ALMAPI VI_SCALE::MouseMove(
	POINT *where,
	int    keysDown)
{
	RECT  layRect = m_rcItem ;
  POINT mouse = *where ;
	long  lastVal ;
  
	if (!OWndGetCaptureItem()) return ;
	if (!scale->mousedown) return ;
	if(!(keysDown&MK_LBUTTON)) return ;

	OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;
	OWndLStoDS(m_oiParent, &mouse, 1) ;
	if (scale->data0->canDrag)
		{
		POINT tPos ;
		int xchg, ychg ;

		xchg = mouse.x - layRect.left - scale->mouse_save.x ;
		ychg = mouse.y - layRect.top - scale->mouse_save.y ;
		if (xchg==0 && ychg==0) return ;

		tPos.x = scale->mouse_save.x + xchg - scale->cursor_pos.x -1 ;
		tPos.y = scale->mouse_save.y + ychg - scale->cursor_pos.y - 1 ;
		lastVal = val ;
		val = PositionToValue (tPos) ;
		ValidateValues () ;
		if (val!=lastVal)
			{
			RedrawIndicator(m_hwndParent, layRect) ;
			if (scale->data0->synchronous)
				SetValue (scale->data0->val, val) ;
			AEvtPostSignalAtMark (scale->oiScale, 2) ;
			AEvtPostSignalAtMark (scale->oiScale, 1) ;
			}
		//AEvtPostSignalAtMark (scale->oiScale, 1) ;
		}
}

void ALMAPI VI_SCALE::MouseUp(
	POINT  *where,
	int      keysDown,
	int              button
	)
{
	//RECT  layRect = m_rcItem ;
	//POINT mouse = *where ;

	if (!OWndGetCaptureItem()) return ;
	if (!scale->mousedown) return ;

	scale->mousedown = FALSE ;
	OWndReleaseCapture();
	SetValue (scale->data0->val, val) ;
	if (scale->data0->synchronous)
		AEvtPostSignalAtMark (scale->oiScale, 3) ;

	I_UNUSED(where);
	I_UNUSED(keysDown);
	I_UNUSED(button);
} 

/***************************************************************************

***************************************************************************/
long ALMAPI VI_SCALE::AdjustNewRect(
	LPRECT newRect,
	LONG     adjustFlag)
	{
	LONG    rVal = 0L;
  POINT   scaleSize ;
	int     tmp ;
		  
	if (scale->scaleData.hScale!=NULL||scale->indData.hIndicator!=NULL)
		{
		if (scale->scaleData.hScale&&scale->indData.hIndicator)
			{
			scaleSize.x = abs(max(sRect.right, max(minRect.right, maxRect.right))-
												min(sRect.left, min(minRect.left,maxRect.left))) ;
			scaleSize.y =	abs(max(sRect.bottom, max(minRect.bottom, maxRect.bottom))-
												min(sRect.top, min(minRect.top,maxRect.top))) ;
			}
		else if (scale->scaleData.hScale==NULL)
		  {
		  scaleSize.x = abs(max(minRect.right, maxRect.right)-
												min(minRect.left,maxRect.left)) ;
			scaleSize.y =	abs(max(minRect.bottom, maxRect.bottom)-
												min(minRect.top,maxRect.top)) ;
			}
		else if	(scale->indData.hIndicator==NULL)
		  {
			scaleSize.x = abs(sRect.right-sRect.left) ;
			scaleSize.y =	abs(sRect.bottom-sRect.top) ;
	    }

		OWndDStoLS(m_oiParent, &scaleSize, 1) ;
		if(adjustFlag&OWND_ADJRECT_DEFAULT_H)
			{
  		if(adjustFlag & OWND_ADJRECT_ANCHOR_B) 
    		{
     		newRect->top = newRect->bottom - scaleSize.y ;
     		rVal |= OWND_ADJRECT_CHANGED_T;
    		}
    	else 
    		{
      	newRect->bottom = newRect->top + scaleSize.y ;
      	rVal |= OWND_ADJRECT_CHANGED_B;
				}
    	}
   else	if (newRect->bottom < newRect->top) 
  		{
    	tmp = newRect->bottom;
    	newRect->bottom = newRect->top;
    	newRect->top = tmp;
    	rVal |= OWND_ADJRECT_SWAPPED_TB;
    	if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
    		{ //if we swap the sides, we need to swap the anchor points
      	adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
      	adjustFlag |= OWND_ADJRECT_ANCHOR_T;
    		}	
    	else 
    		{
      	adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
      	adjustFlag |= OWND_ADJRECT_ANCHOR_B;
    		}
 			}     //else if (newRect->bottom < newRect->top)...

		if(adjustFlag & OWND_ADJRECT_DEFAULT_W)
			{
  		if(adjustFlag & OWND_ADJRECT_ANCHOR_B) 
    		{
      	newRect->left = newRect->right - scaleSize.x ;
      	rVal |= OWND_ADJRECT_CHANGED_L;
     		}
    	else 
    		{
      	newRect->right = newRect->left + scaleSize.x ;
      	rVal |= OWND_ADJRECT_CHANGED_R;
    		}
			}
  	else if (newRect->right < newRect->left) 
  		{
    	tmp = newRect->right;
    	newRect->right = newRect->left;
    	newRect->left = tmp;
    	rVal |= OWND_ADJRECT_SWAPPED_LR;
    	if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
    		{ //if we swapp the sides, we need to swap the anchor points
      	adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
      	adjustFlag |= OWND_ADJRECT_ANCHOR_L;
    		}
    	else 
    		{
      	adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
      	adjustFlag |= OWND_ADJRECT_ANCHOR_R;
    		}
			}     // else if (newRect->right < newRect->left)...
			if (newRect->bottom - newRect->top != scaleSize.y)
				{
				if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
					{
					newRect->top = newRect->bottom - scaleSize.y ;
					rVal |= OWND_ADJRECT_CHANGED_T;
					}
				else
					{
					newRect->bottom = newRect->top + scaleSize.y ;
					rVal |= OWND_ADJRECT_CHANGED_B;
					}
				}

			if (newRect->right - newRect->left != scaleSize.x)
				{
				if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
					{
					newRect->left = newRect->right - scaleSize.x;
					rVal |= OWND_ADJRECT_CHANGED_L;
					}
				else
					{
					newRect->right = newRect->left + scaleSize.x;
					rVal |= OWND_ADJRECT_CHANGED_R;
					}
				}
		}
	else
		rVal = OpLiteWndItem::AdjustNewRect(newRect, adjustFlag);
  return rVal;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI VI_SCALE::Move(LPRECT newRect)
{
	if (EqualRect(newRect, &(m_rcItem)) == FALSE)
		{
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE); // m_rcItem
		m_rcItem = *newRect;
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE); // m_rcItem
		}
}
////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void VI_SCALE::DrawActiveItem(HDC hDC)
{
	HBRUSH 		hBrush;
  RECT      layRect ;

	if (OWndGetActiveItem()==this)
		{
		layRect = m_rcItem ;
		OWndLStoDS(m_oiParent, (LPPOINT)&(layRect), 2) ;

		RECT  ghostRect = scale->indData.rect ;
		if (CorrectRect(&ghostRect, &layRect))
  		{
			hBrush = (HBRUSH)CreatePatternBrush((HBITMAP)hBORDER);
			FrameRect(hDC, &ghostRect, hBrush);
			DeleteObject(hBrush);
			}
    }
}
void VI_SCALE::SetDefaultRectValue (void)
{
	POINT offSize ;

	sRect    = scale->scaleData.rect ;
	minRect  = scale->indData.minRect ;
	maxRect  = scale->indData.maxRect ;
	scalepos = scale->data0->scalepos ;
	minpos   = scale->data0->minpos ;
	maxpos   = scale->data0->maxpos ;
	 
	if (scale->scaleData.hScale&&scale->indData.hIndicator)
		{
		offSize.x = min(sRect.left, min(minRect.left,maxRect.left)) ;
		offSize.y =	min(sRect.top, min(minRect.top,maxRect.top)) ;
		}
	else if (scale->scaleData.hScale==NULL)
	  {
	  offSize.x = min(minRect.left,maxRect.left) ;
		offSize.y =	min(minRect.top,maxRect.top) ;
		}
	else if	(scale->indData.hIndicator==NULL)
	  {
		offSize.x = sRect.left ;
		offSize.y =	sRect.top ;
	  }
	if (scale->scaleData.hScale)																		 
		{
		OffsetRect (&sRect, -offSize.x, -offSize.y) ;
		scalepos.x = scalepos.x-offSize.x ;
		scalepos.y = scalepos.y-offSize.y ;
		}
	if (scale->indData.hIndicator)
		{
		OffsetRect (&minRect, -offSize.x, -offSize.y) ;
		OffsetRect (&maxRect, -offSize.x, -offSize.y) ;
		minpos.x = minpos.x-offSize.x ;
		minpos.y = minpos.y-offSize.y ;
		maxpos.x = maxpos.x-offSize.x ;
		maxpos.y = maxpos.y-offSize.y ;
		}
}
//////////////////////////////////////////////////////////////////////////
//Runtime Utility
//////////////////////////////////////////////////////////////////////////
void VI_SCALE::ValidateValues (void)
{
	double long temp ;

	if (max < min )
	{
		temp = max ;
		max  = min ;
		min  = temp ;
		//val     = minimum ;
		if (val <min)
			val = min ;
		else if (val > max)
			val = max ;
	}
	else if (val < min )
		val = min ;
	else if (val > max )
		val = max ;
}
//////////////////////////////////////////////////////////////////////////
// Function:    PositionToValue
// Description: Calculate the value of the mouse and change it to the value
//              of the number object
//////////////////////////////////////////////////////////////////////////
double long VI_SCALE::PositionToValue (POINT thePosition )
{
	double long tVal ;
	double long t ;
	double long x, y ;
	//POINT maxpos = scale->data0->maxpos ;
	//POINT minpos = scale->data0->minpos ;

	x = (double long)maxpos.x - (double long)minpos.x ;
	y = (double long)maxpos.y - (double long)minpos.y ;

  t = 0. ;
	if (abs(x) > abs(y) )
		{
		//if (x!=t)
    if (abs(x)>t)
			t = (max - min) * (thePosition.x - minpos.x) / x  ;
    }
	else
		{
		//if (y!= t)
    if (abs(y)>t)
			t = (max - min) * (thePosition.y - minpos.y) / y  ;
    }

	tVal = t + min ;

	return (round(tVal)) ;
	}
//////////////////////////////////////////////////////////////////////////
// Function:    ValueToPosition 
// Description: According to the value of the number object to calculate
//              the position of the indicator in the client area.
//////////////////////////////////////////////////////////////////////////
POINT VI_SCALE::ValueToPosition (double long val)
{
	//POINT maxpos = scale->data0->maxpos ;
	//POINT minpos = scale->data0->minpos ;
	POINT tPos ;
	double long  x, y ;

	if (max-min == 0)
		{
		tPos = minpos ;
		}
	else
		{
		x = maxpos.x - minpos.x ;
		y = maxpos.y - minpos.y ;
		x = x * (val - min) / (max - min) ;
		y = y * (val - min) / (max - min) ;
		x = round (x) ;
		y = round (y) ;
		tPos.x = minpos.x + x ;
		tPos.y = minpos.y + y ;
		}

	return (tPos) ;
	}
void VI_SCALE::RepaintScale(
	HDC hdc,
	BOOL IsLayoutTime)
{
	RECT rect ;

	if (scale->scaleData.hScale)
		{
  	rect = sRect ; //scale->scaleData.rect ;
		if (CorrectRect (&rect, &VRect))
    	{
			PaintDIBitmap (hdc, scale->scaleData.hScale, &rect) ;
			}
    }
	if (scale->indData.hIndicator)
		{
		if (IsLayoutTime)
    	{
			rect = minRect ; //scale->indData.minRect ;
			if (CorrectRect(&rect, &VRect))
				{
				PaintDIBitmap (hdc, scale->indData.hIndicator, &rect) ;
				}
      }
		else
			{
			rect = scale->indData.rect ;
			if (CorrectRect(&rect, &VRect))
				{
				SaveBackground (hdc, rect) ;
				PaintIndicator (hdc, rect) ;
				}
			}
		}
}
void VI_SCALE::RedrawIndicator(
	HWND hwnd,
	RECT layRect)
{
	RECT rect ;
  RECT newRect ;
	HDC hdc ;

	if (scale->indData.hIndicator)
		{
		rect = scale->indData.rect ;
		if (CorrectRect(&rect, &layRect))
			{
			POINT point = ValueToPosition (val) ;
     // RECT unitRect ;
			SetRect (&newRect,
								point.x,
								point.y,
								point.x+(int)Width, //rect.right-rect.left,
								point.y+(int)Height); //rect.bottom-rect.top) ;

			scale->indData.rect = newRect ;
			if (CorrectRect(&newRect, &layRect))
				{
				hdc = GetDC (hwnd) ;
				int mode = GetMapMode(hdc);
				int sDC  = SaveDC(hdc);
				mode = SetMapMode(hdc,MM_TEXT);
				//PaintIntersect (hdc, unitRect, newRect, rect) ;
				PaintIntersect (hdc, newRect, rect) ;
				PaintIndicator (hdc, newRect) ;
        DrawActiveItem(hdc) ;
				SetMapMode(hdc, mode);
      	RestoreDC(hdc, sDC);
				ReleaseDC (hwnd, hdc) ;
				}
			}
		}
}
//void VI_SCALE::PaintIntersect (HDC hdc, RECT Union, RECT newRect, RECT rect)
void VI_SCALE::PaintIntersect (HDC hdc, RECT newRect, RECT rect) 
{
	POINT UnionSize ;
	RECT  Union ;

	UnionRect( &Union, &newRect, &rect );
  UnionSize.x = Union.right - Union.left;
  UnionSize.y = Union.bottom - Union.top;

  HDC WorkDC = CreateCompatibleDC( NULL );
  HBITMAP WorkBM = CreateCompatibleBitmap(hdc, UnionSize.x, UnionSize.y );
  SelectObject( WorkDC, WorkBM );
  BitBlt(WorkDC,
         0,
         0,
         UnionSize.x,
         UnionSize.y,
         hdc,
         Union.left,
         Union.top,
         SRCCOPY );

   HDC ScratchDC = CreateCompatibleDC( NULL );
   SelectObject( ScratchDC, hBackground );

   BitBlt(WorkDC,
          rect.left - Union.left,
          rect.top - Union.top,
          rect.right-rect.left,
          rect.bottom-rect.top,
          ScratchDC,
          0,
          0,
          SRCCOPY );

		BitBlt(ScratchDC,
           0,
           0,
           newRect.right - newRect.left, //rect.right - rect.left,
           newRect.bottom - newRect.top, //rect.bottom-rect.top,
           WorkDC,
           newRect.left - Union.left,
           newRect.top - Union.top,
           SRCCOPY );

    DeleteDC( ScratchDC );

    BitBlt( hdc,
            Union.left,
            Union.top,
            UnionSize.x,
            UnionSize.y,
            WorkDC,
            0,
            0,
            SRCCOPY );

    DeleteDC( WorkDC );
    DeleteObject( WorkBM );
}
void VI_SCALE::SaveBackground (HDC hdc, RECT Rect)
{
	int width =  scale->indData.rect.right - scale->indData.rect.left ;
	int height = scale->indData.rect.bottom - scale->indData.rect.top ;

	if (!hBackground)
		{
		HDC MemDC = CreateCompatibleDC( NULL );
		hBackground= CreateCompatibleBitmap(hdc,
																				width,
																				height) ;
	  HBITMAP oldBitmap = (HBITMAP)SelectObject( MemDC, (HBITMAP)hBackground);

  	BitBlt( MemDC,
    	      0,
      	    0,
        	  Rect.right-Rect.left,
          	Rect.bottom-Rect.top,
          	hdc,
          	Rect.left,
          	Rect.top,
						SRCCOPY );

		SelectObject (MemDC, oldBitmap) ;
		DeleteDC( MemDC );
		}
}
/*void VI_SCALE::RestoreBackground (HDC hdc, RECT Rect)
{
	HDC MemDC = CreateCompatibleDC( NULL );

  SelectObject( MemDC, hBackground );

  BitBlt( hdc,
          Rect.left,
          Rect.top,
          Rect.right-Rect.left,
          Rect.bottom-Rect.top,
					MemDC,
          0,
          0,
          SRCCOPY );

  DeleteDC( MemDC );
  DeleteObject( hBackground );
  hBackground = NULL;
}*/
void VI_SCALE::PaintIndicator (HDC hdc, RECT rect)
{
	if (scale->data0->stamp)
		DoTransparent (hdc, hMaskBitmap, hInvertBitmap, &rect) ;
 		//PaintTransparentBitmap (hdc, scale->indData.hIndicator, &rect) ;
	else
		PaintDIBitmap (hdc, scale->indData.hIndicator, &rect) ;
} 

