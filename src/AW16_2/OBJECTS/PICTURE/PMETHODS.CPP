/*//////////////////////////////////////////////////////////////
//
//          Copyright 1993, Novell, Inc.  All rights reserved
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
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
// to force strict compile
#define STRICT
#define ASTRICT

#include "a_alm.h"
#include "intdebug.h"
#include "picture.h"


// dfault sizes for picture
#define I_DEFAULT_HEIGHT   100
#define I_DEFAULT_WIDTH    100
#define I_MIN_WIDTH        50
#define I_MIN_HEIGHT       50

#define I_DOT_SIZE         16 //1.6mm
#define I_DOT_SIZEH        8


/////////////////////////////////////////////////////////////////////
extern BOOL EqualSizeRect(LPRECT srcRect, LPRECT rect);
extern BOOL PASCAL MakePaletteEntries(LPBITMAPINFOHEADER lpInfo, MEMBLOCKID mID,
	MEMBLOCKID palID);

extern HANDLE hBORDER ; // handle to border brush bitmap
extern PICTOBJECT *GetPictObject(OBJECTID oid);
extern HANDLE CreateAndDisplay(
	PICTOBJECT *pObject,
	HWND hPaintWnd, RECT ClientRect);
extern HPALETTE CreateDIBPaletteHandle(
	BITMAPINFOHEADER *lpInfo,
	MEMBLOCKID memID
	);
extern void DestroyPalette(HPALETTE hpal);





////////////////////////////////////////////////////////////////////
void FramePictRect(HDC hItemDC, LPRECT mapRect, HBRUSH hBrush)
  {
  MoveTo(hItemDC, mapRect->left, mapRect->top);
  LineTo(hItemDC, mapRect->left, mapRect->bottom);
  LineTo(hItemDC, mapRect->right, mapRect->bottom);
  LineTo(hItemDC, mapRect->right, mapRect->top);
  I_UNUSED(hBrush);
  }
/////////////////////////////////////////////////////////////////////

PICTURE_VINST::PICTURE_VINST(OBJECTID oid,
                        OBJECTID parentID,
                        RECT*    rcItem,
                        long     statusFlag)
  :OpLiteWndItem(oid, parentID, rcItem, statusFlag)
  {

  hPaintWnd = NULL;
  hPict = NULL;
  isActivated = FALSE;
  // PICT_OBJECT instance is tied to each ALM type instance
  // (not to each visual instance)
  pictObject = (PICTOBJECT*)GetPictObject(oid); // get PICT_OBJECT instance

  // for startup set this flag to be true, so that we can create
  // a new handle for the picture at OWND_EVT_DRAWWINDITEM time, 3/23/93
  pictObject->SetbImageSave(TRUE);
  if (pictObject->IsSelectable()) { 
		OWndSetItemActivatable(this, TRUE);
    OWndSetItemUsesTab(this, FALSE);
    }
  else {
    OWndSetItemActivatable(this, FALSE);
    }
  OWndSetItemTransparent(this, TRUE);
  OWndSetItemEnabled(this, TRUE);
  }

/*********************************************************
*
*
*********************************************************/
PICTURE_VINST::~PICTURE_VINST()
	{

	}
/*********************************************************
*
*
*********************************************************/
void ALMAPI PICTURE_VINST::ParentClosing
  (
  )

  {
  pictObject->DeletePHandle(hPict);
  hPict = NULL;
  }

/*********************************************************
*
*
*********************************************************/
void ALMAPI PICTURE_VINST::ParentOpened
	(
	)

	{
	// June 20, 1995
	if (OWndQueryCanSetPalette(this))
		{
    LPLOGPALETTE npPal;
		MEMBLOCKID  logPalID = AMemAllocate(0);
		if (MakePaletteEntries(&(pictObject->bmpInfoHdr), pictObject->memID, logPalID))
			{
			npPal = (LPLOGPALETTE)AMemLock(logPalID);
			OWndSetPalette(this, npPal->palPalEntry, npPal->palNumEntries);
			AMemUnlock(logPalID);
			}

		hPict = 0;
		AMemFree(logPalID);
		}

  }

/*********************************************************
*
*
*********************************************************/
void ALMAPI PICTURE_VINST::Destroy
  (
  )

  {
  I_ASSERT(hPict==NULL);

  if(hPict)
    pictObject->DeletePHandle(hPict);
  hPict = NULL;

  delete this;
  }


/*********************************************************
*
*
*********************************************************/
void ALMAPI PICTURE_VINST::Hide()
  {
  if (IsActivated() && pictObject->IsSelectable())
    DrawActFrame(FALSE);
  OpLiteWndItem::Hide();
  }

/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::Show()
  {
  if (IsActivated() && pictObject->IsSelectable())
    DrawActFrame(TRUE);
  OpLiteWndItem::Show();
  }

/*********************************************************
*
*
*********************************************************/
void PICTURE_VINST::DrawActFrame(
  BOOL bActFrame)
  {

  if (bActFrame)
    {
    HDC       hDC;
    RECT      ghostRect = m_rcItem; // m_rcItem
    POINT     inflatePt;

    // Change to DP, and Draw the Rect -- > SKJ
    OWndLPtoDP(m_oiParent, (LPPOINT)&ghostRect, 2);
    // increase by 2 pixels
    inflatePt.x = 2;
    inflatePt.y = 2;
    InflateRect(&ghostRect, inflatePt.x, inflatePt.y);
    hDC = GetDC(m_hwndParent);
    int sDC = SaveDC(hDC);
    int mode = SetMapMode(hDC,MM_TEXT);

    int oldROP = SetROP2(hDC, R2_NOTXORPEN);
    Rectangle(hDC, ghostRect.left,ghostRect.top, ghostRect.right, ghostRect.bottom);
    SetROP2(hDC, oldROP);

    // FrameRect(hDC, &ghostRect, hBrush);
    SetMapMode(hDC, mode);
    RestoreDC(hDC, sDC);
    ReleaseDC(m_hwndParent, hDC);
    }
  else {
    HDC     hDC;
    // HBRUSH   hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    RECT    ghostRect =  m_rcItem;
    hDC =   GetDC(m_hwndParent);
    POINT   inflatePt;
    // increase by 2 pixels
    inflatePt.x = 2;
    inflatePt.y = 2;
    OWndDPtoLP(m_oiParent, (LPPOINT)&inflatePt, 1);
    InflateRect(&ghostRect, inflatePt.x, inflatePt.y);

    // Change to DP, and Draw the Rect -- > SKJ
    OWndLPtoDP(m_oiParent, (LPPOINT)&ghostRect, 2);
    int sDC = SaveDC(hDC);
    int mode = SetMapMode(hDC,MM_TEXT);

    int oldROP = SetROP2(hDC,  R2_NOTXORPEN);
    Rectangle(hDC, ghostRect.left,ghostRect.top, ghostRect.right, ghostRect.bottom);
    SetROP2(hDC, oldROP);

    //FrameRect(hDC, &ghostRect, hBrush);
    SetMapMode(hDC, mode);
    RestoreDC(hDC, sDC);
    ReleaseDC(m_hwndParent, hDC);
    }

  }


/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::Print(HDC hItemDC, RECT* printRect, int pclPrinter)
  {
	// print Picture
	pictObject->PrintPicture(m_hwndParent, hItemDC, printRect, &hPict);
  I_UNUSED(pclPrinter);
  }

/*********************************************************
*
*
*********************************************************/

long ALMAPI PICTURE_VINST::Activate(int causedBy, BOOL mustKeep)
  {
  long rVal;
  //  case OWND_EVT_ACTIVATEWINDITEM:
  // SKJ -> generate an ACTIVATED signal only if the item is not
  // already activated Oct.6, 1993
  ADBG_ASSERT_E(!IsActivated(), return OWND_ACTIVATION_KEPT, "Picture is already active");
	if (pictObject->IsSelectable())
		{
		SetActivated(TRUE);
		DrawActFrame(TRUE);
		rVal = OWND_ACTIVATION_KEPT;
		AEvtPostSignalAtMark(m_oiObject, PICTACTIVATEDSIGNAL);

		}
  else
    rVal = OWND_ACTIVATION_PASS;

  if (mustKeep)
    rVal = OWND_ACTIVATION_KEPT;
  I_UNUSED(causedBy);
  return rVal;
  }

/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::Deactivated(void)
  {
  //case OWND_EVT_DEACTIVATEWINDITEM:

  if (IsActivated() && pictObject->IsSelectable())
    {
    DrawActFrame(FALSE);
    SetActivated(FALSE);
    AEvtPostSignalAtMark(m_oiObject, PICTDEACTIVATEDSIGNAL);
    }
  }

/*********************************************************
*
*
*********************************************************/

void ALMAPI   PICTURE_VINST::Draw(HDC hItemDC, RECT* invalidRect)
  {
  // case OWND_EVT_DRAWWINDITEM: // should be OWND_EVT_DRAWWINDITEM
  BOOL bBackGround = FALSE;
  BOOL isLayout = OWndIsLayoutTime();
  int mode = GetMapMode(hItemDC);
  int sDC = SaveDC(hItemDC);
  mapRect = m_rcItem;
  int debug = 1;
  if (debug)
      mode = SetMapMode(hItemDC,MM_TEXT);
  OWndLPtoDP(m_oiParent, (LPPOINT)&mapRect, 2);
  mapRect = m_rcItem;
  OWndLStoDS(m_oiParent, (LPPOINT)&mapRect, 2);

  if (isLayout ) // )  // if used during layout
    {
    // This will create a picture handle only if we already do not have one
		if (hPict == NULL)
			{
			MEMBLOCKID midPal = AMemAllocate(0);
			OWndGetPalette(m_oiParent, midPal);
			HPALETTE *lphPal = (HPALETTE *)AMemLock(midPal);
			hPict = pictObject->CreatePictH(hItemDC,&mapRect, *lphPal); // &m_rcItem);
      AMemUnlock(midPal);
			AMemFree(midPal);
			}
		pictObject->DrawPicture(m_hwndParent, hItemDC,hPict, &mapRect); // &m_rcItem);
    FrameRect(hItemDC, &mapRect, (HBRUSH)GetStockObject(BLACK_BRUSH)); // &m_rcItem
    }
  else {
    // runtime drawing
    // This should send a paint to all children of the Table Window
    if (OWndIsItemVisible(this) == 0) {
      // if the picture was active before, then remove the act. frame.
      if (IsActivated() && pictObject->IsSelectable())
        DrawActFrame(FALSE); 
      return;
      }

		if (!hPict) {
			MEMBLOCKID midPal = AMemAllocate(0);
			OWndGetPalette(m_oiParent, midPal);
			HPALETTE *lphPal = (HPALETTE *)AMemLock(midPal);
			hPict = pictObject->CreatePictH(hItemDC,&mapRect, *lphPal); // &m_rcItem);
			AMemUnlock(midPal);
			AMemFree(midPal);
			}

		// Do not get the client Rect, but use the
    // one provided by the Window Object
    // m_rcItem = *invalidRect; // Get the rect that is passed in.
    if (hPict) {
      pictObject->DrawPicture (m_hwndParent,hItemDC,hPict,&mapRect); //&m_rcItem);
      }
		else { 
      }
    if (pictObject->IsFramed())
      { 
      HBRUSH    hBrush = CreateSolidBrush(RGB(0,0,0)); 
      FrameRect(hItemDC, &mapRect, hBrush); // m_rcItem
      DeleteObject(hBrush);
      }
    if (IsActivated() && pictObject->IsSelectable())
      {
      DrawActFrame(TRUE);
      }
    }

  if (debug)
    SetMapMode(hItemDC,mode);

  RestoreDC(hItemDC, sDC);

  I_UNUSED(bBackGround);
  I_UNUSED(invalidRect);
  }
  

/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::DoubleClicked(POINT *pt, int kDown, int Button)
  {

  AEvtPostSignalAtMark(m_oiObject, PICTDBLCLICKEDSIGNAL);
  I_UNUSED(pt);
  I_UNUSED(kDown);
  I_UNUSED(Button);
  }

/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::MouseDown(POINT* where, int keysDown, int button)
  {

  AEvtPostSignalAtMark(m_oiObject, PICTCLICKEDSIGNAL);
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  }



/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::DataChanged(long details, long modifiers, long variation)
  {
     
  // if the picture has been resized than
  // delete the old picture handle and recreate it in
  // OWND_EVT_DRAWWINDITEM
  if (modifiers == RECREATEHANDLE)
    // (EqualSizeRect(&(m_rcItem), &(wi->rect)) == FALSE))
    {

		MEMBLOCKID  logPalID = AMemAllocate(0);
		if (MakePaletteEntries(&(pictObject->bmpInfoHdr), pictObject->memID, logPalID))
			{
			LPLOGPALETTE npPal;
			npPal = (LPLOGPALETTE)AMemLock(logPalID);
			OWndSetPalette(this, npPal->palPalEntry, npPal->palNumEntries);
			AMemUnlock(logPalID);
      }

		AMemFree(logPalID);

    // INitialize the visual picture handle
		pictObject->DeletePHandle(hPict);
    hPict = NULL;
    }
  OWndInvalidateLogicalRect(m_oiParent,&m_rcItem, TRUE);
  // InvalidateRect(m_hwndParent, &map, TRUE); //(m_rcItem)
  I_UNUSED(details);
  I_UNUSED(variation);

  }


/*********************************************************
*
*
*********************************************************/

long ALMAPI PICTURE_VINST::AdjustNewRect(
  LPRECT newRect,
  LONG adjustFlag)
  {

  long                    rVal = 0L;
  int                     tmp;
  if (newRect->bottom < 0 )
    {
    newRect->bottom = newRect->top + I_DEFAULT_HEIGHT;
    rVal |= (OWND_ADJRECT_CHANGED_B | OWND_ADJRECT_DEFAULT_H);
    }
  else if (newRect->bottom < newRect->top)
    {
    tmp = newRect->bottom;
    newRect->bottom = newRect->top;
    newRect->top = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_TB;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      { //if we swapp the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
      adjustFlag |= OWND_ADJRECT_ANCHOR_T;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
      adjustFlag |= OWND_ADJRECT_ANCHOR_B;
      }
    }

  if (newRect->right < 0)
    {
    newRect->right = newRect->left + I_DEFAULT_WIDTH;
    rVal |= (OWND_ADJRECT_CHANGED_R | OWND_ADJRECT_DEFAULT_W);
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
    }

  //At this point, the rectangle is not flipped inside-out

  // Make sure it is a minimum height and width
  if (newRect->bottom - newRect->top < I_MIN_HEIGHT)
    {
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      newRect->top = newRect->bottom - I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      newRect->bottom = newRect->top + I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
    }

  if (newRect->right - newRect->left < I_MIN_WIDTH)
    { 
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      newRect->left = newRect->right - I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      newRect->right = newRect->left + I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
    }

  //I_METHOD_NOT_PROVIDED(wi->Type(), "AdjustNewRect");
  return rVal;
  }

/*********************************************************
*
*
*********************************************************/

void ALMAPI PICTURE_VINST::Move(LPRECT newRect)
  {
  if (EqualRect(newRect, &(m_rcItem)) == FALSE)
      {
      // INitialize the visual picture handle
      mapRect = m_rcItem;
      OWndLPtoDP(m_oiParent, (LPPOINT)&(mapRect), 2);
      InvalidateRect(m_hwndParent, &mapRect, TRUE); // &m_rcItem
      if (EqualSizeRect(newRect, &(m_rcItem)) == FALSE)
        {
        pictObject->DeletePHandle(hPict);
        hPict = NULL;
        }
      // Remove the activate frame, if there was one --> July 5, 1994
      if (IsActivated() && pictObject->IsSelectable())
        DrawActFrame(FALSE);
      

      m_rcItem = *newRect;
      mapRect = m_rcItem;
      // Convert from Logical to Device Points
      OWndLPtoDP(m_oiParent, (LPPOINT)&(mapRect), 2);


      // if activated then invalidates rect including Act. Rect
      if (IsActivated() && pictObject->IsSelectable())
        {
        RECT      ghostRect = mapRect; // m_rcItem
        POINT     inflatePt;

        // increase by 2 pixels
        inflatePt.x = 2;
        inflatePt.y = 2;
        InflateRect(&ghostRect, inflatePt.x, inflatePt.y);
        InvalidateRect(m_hwndParent, &ghostRect, TRUE); // &m_rcItem
        }
      else
        {
        InvalidateRect(m_hwndParent, &mapRect, TRUE); // &m_rcItem

        }



      }
  }

/***************************************************************************

***************************************************************************/
void ALMAPI PICTURE_VINST::Cut()
  {
  // Perform operation to cut into the clipboard
  // and remove image from the picture Object
  
  if (IsActivated()) {
    pictObject->EditCut(m_hwndParent, &mapRect); //(m_rcItem));
    }
  }

/***************************************************************************

***************************************************************************/
void ALMAPI PICTURE_VINST::Copy()
  {

  if (IsActivated()) {
    pictObject->EditCopy(m_hwndParent, &mapRect); // (m_rcItem));
    }
  }

/***************************************************************************

***************************************************************************/
void ALMAPI PICTURE_VINST::Paste()
  {
  if (IsActivated())
    {
    pictObject->EditPaste(m_hwndParent, &(m_rcItem));
    }
  }


/***************************************************************************

***************************************************************************/
/*
void ALMAPI PICTURE_VINST::Undo()
  {

  AObjReportError   (m_oiObject,
                      ,
                           A_ERROR,
                           "Undo not implemented for picture"
                           0);
  }

*/   




/* ==== Picture direct functions ========================================= */

/***************************************************************************

***************************************************************************/

HANDLE GetDDEPict(PICTURE_VINST *pVInst, HWND hWnd, LPRECT lpRect)
  {
  HDC hItemDC  = GetDC(hWnd);
  int mode= GetMapMode(hItemDC);
  HDC hDCMem = CreateCompatibleDC(hItemDC);
  int xLen  = lpRect->right - lpRect->left;
  int yLen  = lpRect->bottom - lpRect->top;
  HANDLE hBitmap = CreateCompatibleBitmap(hItemDC,xLen, yLen);
  
  if (hBitmap)
    {
    SelectObject(hDCMem, hBitmap);
    StretchBlt(hDCMem, 0, 0, xLen, yLen, hItemDC, lpRect->left, lpRect->top,
                     xLen, yLen, SRCCOPY);
    }
  DeleteDC(hDCMem);
  ReleaseDC(hWnd, hItemDC);

	I_UNUSED(mode);
	I_UNUSED(pVInst);
  return hBitmap;
  }


/****************************************************************************
*
*   This api will provide the handle to picture in clipboard format
*   Mar. 9, 1994
*   This function will be generate a picture handl from the first VI instance
*   i.e. The enum proc is aborted after the first call
*
*****************************************************************************/

BOOL ALMAPI VIProc(OpWndItemD * vi, LONG lpHandle, LONG lpCount)
  {
  PICTURE_VINST *lpPictVI = (PICTURE_VINST *) vi;
  LPLONG  tmpCount = (LPLONG)lpCount;
  LPHANDLE lpTmp  = (LPHANDLE)lpHandle;
  if (lpPictVI)
    {
    RECT mapRect = lpPictVI->m_rcItem;
    HDC  hDC = GetDC(lpPictVI->m_hwndParent);
		OWndLStoDS(lpPictVI->m_oiParent, (LPPOINT)&mapRect, 2);

		MEMBLOCKID midPal = AMemAllocate(0);
		OWndGetPalette(lpPictVI->m_oiParent, midPal);
		HPALETTE *lphPal = (HPALETTE *)AMemLock(midPal);
		*lpTmp = lpPictVI->pictObject->CreatePictH(hDC,&mapRect, *lphPal); // &m_rcItem);
		AMemUnlock(midPal);
		AMemFree(midPal);


		ReleaseDC(lpPictVI->m_hwndParent, hDC);
		(*tmpCount) +=1;
    }
  return FALSE;
  }



/****************************************************************************
*
*   This api will provide the handle to picture in clipboard format
*   Dec. 2, 1993
*   This function will be called by the DDE server object to get the picture
*   handle.
*
*****************************************************************************/


extern "C" BOOL far PASCAL _export OPictGetHandle(OBJECTID itemID, LONG *lHandle )
  {
  PICTOBJECT *pictObject; // = new PICTOBJECT(itemID,NULL);
  HDC     hItemDC;        // = CreateDC("DISPLAY", NULL, NULL, NULL);
  BITMAP  bmap;
  RECT    mapRect;
  int     xLen;
  int     yLen;
  LONG    nCountVI = 0;
  BOOL    rVal = FALSE;
  *lHandle = 0;
  // Grab the first VI nd create Handle from it
  OWndCallEachVisualInstance(itemID, VIProc, (LONG)lHandle, (LONG)(LPLONG)&nCountVI);
  if (nCountVI)
		{
		return TRUE;
		}

	// Else if there are no vInstance of the picture, then create a handle in Device DC
	pictObject = new PICTOBJECT(itemID,NULL);
	hItemDC  = CreateDC("DIB", NULL, NULL, NULL); // "DISPLAY"
	xLen =  GetDeviceCaps(hItemDC, HORZRES);
	yLen =  GetDeviceCaps(hItemDC, VERTRES);
	mapRect.left = 0;
	mapRect.right =  xLen;
	mapRect.top = 0;
	mapRect.bottom  = yLen;

	MEMBLOCKID midPal = AMemAllocate(0);
	HPALETTE hPal = CreateDIBPaletteHandle(&(pictObject->bmpInfoHdr), pictObject->memID);
	*lHandle = (long)pictObject->CreatePictH(hItemDC,&mapRect, hPal); 
	if (*lHandle)
		{
		GetObject((HANDLE)*lHandle, sizeof(BITMAP), (LPSTR)&bmap);
		rVal = TRUE;
		}
	else
		rVal = FALSE;

	DestroyPalette(hPal);
	DeleteDC(hItemDC);

  I_UNUSED(midPal);
	return rVal;
	}