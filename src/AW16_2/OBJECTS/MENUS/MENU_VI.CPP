////////////////////////////////////////////////////////////////
//
//					Copyright 1994, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					MENU_VI.CPP
//
// AUTHOR:				Russell Graves    28 Feb 94 - Initial entry
//
// DESCRIPTION:		Popup menu functions
//                
//                
//
// CHANGES:
//
////////////////////////////////////////////////////////////////
#define   STRICT
#define   ASTRICT
#define  	MENU_ARROWWIDTH 	16  // Logical units
#define  	MENU_ARROWHEIGHT 21
#define   MENU_TEXTBORDER 2
#define		MENU_POPUPBORDER 1
#define   MENU_ADJUST 1

#include  <a_alm.h>
#include  <intdebug.h>
#include  <windowsx.h>
#include  <string.h>
#include  "menu_vi.h"
#include  "menulib.h"
#include  "modinfo.h"
#include  "itemlib.h"

////////////////////////////////////////////////////////////////
extern "C"
OpWndItemD* ALMAPI MenuPopupCreate 
  (
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        miExtra
  )
  {
  I_UNUSED(miExtra);

  MenuPopupVI*  menu_vi;
  
  menu_vi = new MenuPopupVI(oiItem, oiParent, rcItem, status);
 
  return menu_vi;
  }

////////////////////////////////////////////////////////////////
//
//  Constructor
//
////////////////////////////////////////////////////////////////
MenuPopupVI::MenuPopupVI
  (
  OBJECTID  oiItem,
  OBJECTID  oiParent,
  RECT*     rcItem,
  long      status
  ):OpLiteWndItem(oiItem, oiParent, rcItem, status)
  {
  OWndSetItemEnabled(this, TRUE);
  OWndSetItemTransparent(this, TRUE);
  }   
////////////////////////////////////////////////////////////////
//
//  Destructor
//
////////////////////////////////////////////////////////////////
MenuPopupVI::~MenuPopupVI
  (
  void
  )
  {
  }   
 
////////////////////////////////////////////////////////////////
//
//  Destroy
//
////////////////////////////////////////////////////////////////
void  ALMAPI  MenuPopupVI::Destroy(void)
  {
  delete this;
  }

////////////////////////////////////////////////////////////////
//
//  Draw
//
////////////////////////////////////////////////////////////////
void  ALMAPI  MenuPopupVI::Draw(
  HDC hdc, 
  RECT* invalidRect)
  {
  I_UNUSED(invalidRect);

  PopupDraw(hdc);
  return;
  }

/*
////////////////////////////////////////////////////////////////
//
//  Mouse Up
//
////////////////////////////////////////////////////////////////
  void  ALMAPI  MenuPopupVI::MouseUp(
  POINT* where,
  int keysDown,
  int button)
  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);
  }   */

/*
////////////////////////////////////////////////////////////////
//
//  Mouse Move
//
////////////////////////////////////////////////////////////////
void  ALMAPI  MenuPopupVI::MouseMove(
  POINT* where,
  int keysDown)
  {
  }   */

////////////////////////////////////////////////////////////////
//
//  Data Changed
//
////////////////////////////////////////////////////////////////
void  ALMAPI  MenuPopupVI::DataChanged(
	long 	  details,
	long	  modifiers,
	long	  variation)
	{

	I_UNUSED(details);
	I_UNUSED(modifiers);
	I_UNUSED(variation);
  
  // Redraw Popup menu.
  OWndInvalidateLogicalRect(m_oiParent,&m_rcItem, FALSE);
  }  
   
////////////////////////////////////////////////////////////////
//
//  Mouse Down
//
////////////////////////////////////////////////////////////////
void  ALMAPI  MenuPopupVI::MouseDown(
  POINT*      ptWhere,
  int         keysDown, 
  int         button)
  {
  HMENU       hMenu = MenuGetMenuHandle(m_oiObject);
  POINT       pt    = *ptWhere;
  LPMENUINFO  pMenuInfo;
  
  I_UNUSED(keysDown);
  I_UNUSED(button);

  pMenuInfo = (LPMENUINFO)AObjLockData(m_oiObject, MENU_INFO_DATA);
  if (!(pMenuInfo->status & MENU_DISABLED))
    {
    pt.y = m_rcItem.bottom; // Line up popup menu at the bottom of rectangle

    OWndLPtoDP(m_oiParent, &pt, 1);     // change to LStoDS
//    OWndLStoDS(m_oiParent, &pt, 1);     
    ClientToScreen(m_hwndParent, &pt);
    TrackPopupMenu(hMenu, TPM_CENTERALIGN, pt.x,
                   pt.y, 0, m_hwndParent, NULL);
    OWndInvalidateLogicalRect(m_oiParent,&m_rcItem, FALSE);
    MenuReleaseMenuHandle(m_oiObject, hMenu);                                  
    }
  AObjUnlockData(m_oiObject, MENU_INFO_DATA);
  }

////////////////////////////////////////////////////////////////
//
//  Popup Draw
//
////////////////////////////////////////////////////////////////
void  MenuPopupVI::PopupDraw(
  HDC         hdc)
  {
  RECT		    rect;
	HDC		    	hdcMem;
	HBITMAP		  hBitMap;
	HBRUSH      newBrush, oldBrush;
	HPEN        newPen, oldPen;
	int         oldMode;
	COLORREF    newTextColor, oldTextColor;
	RECT        textRect;
	RECT        newBounds;
	int         hold;
  LPMENUINFO  pMenuInfo;
  LPITEMENTRY pItemEntry;
  LPSTR       pTitle;
  LPSTR       pText;
  char        pBuffer[MENU_MAX_TEXT];
  OBJECTID    itemObjID;
  WORD        wCurrentItem;
  int	        menu_ARROWWIDTH = LUxtoDUx(MENU_ARROWWIDTH);
  int         menu_TEXTBORDER = LUxtoDUx(MENU_TEXTBORDER);
	int         menu_POPUPBORDER = LUxtoDUx(MENU_POPUPBORDER);
  int         menu_ADJUST = LUxtoDUx(MENU_ADJUST);
  int         du1x = LUxtoDUx(1);
  int         du1y = LUytoDUy(1);
    
	//NOTE: the assumption made is that the size of the box will be big enough
	// to hold both the text and bitmap - at least big enough to hold the
	//arrow bitmap since these size restrictions should have been imposed.

  pMenuInfo = (LPMENUINFO)AObjLockData(m_oiObject, MENU_INFO_DATA);

	//normalize the bounds incase the rectangle is attatched to the window
	//and the size changes
  newBounds = m_rcItem;

	if(newBounds.left > newBounds.right)
		{
		hold = newBounds.left;
		newBounds.left = newBounds.right;
		newBounds.right = hold;
		}

	if(newBounds.top > newBounds.bottom)
		{
		hold = newBounds.top;
		newBounds.top = newBounds.bottom;
		newBounds.bottom = hold;
		}

  pItemEntry  = (LPITEMENTRY)AObjLockData(m_oiObject, MENU_ITEMLIST_DATA);
  pTitle = (LPSTR)AObjLockData(m_oiObject, MENU_TITLE_DATA);
  
  textRect  =  newBounds;

	if(pTitle != '\0')
		{
		//figure the length of the text + a single space
		textRect.right = textRect.left +
			LOWORD(GetTextExtent(hdc, pTitle, lstrlen(pTitle)) +
			LOWORD(GetTextExtent(hdc, " ", 1)));
		}
	else
		textRect.right = textRect.left;


	if(textRect.right > newBounds.right - menu_ARROWWIDTH - menu_POPUPBORDER -
		menu_ADJUST)
		textRect.right = newBounds.right - menu_ARROWWIDTH - menu_POPUPBORDER -
			menu_ADJUST;

	if(textRect.right < textRect.left)
		textRect.right = textRect.left;

	rect = newBounds;
	rect.left = textRect.right;

	//Set up hdc
	newBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	//Base outline and text color on whether or not the popup is enabled
	if(pMenuInfo->status & MENU_DISABLED)
		{
		newTextColor = GetSysColor(COLOR_GRAYTEXT);
		hBitMap = LoadBitmap(MODULEINFO::hInstance, "POPGREYDNARROW");
		}
	else
		{
		newTextColor = GetSysColor(COLOR_WINDOWFRAME);
		hBitMap = LoadBitmap(MODULEINFO::hInstance, "POPDNARROW");
		}
	newPen = CreatePen(PS_SOLID, 1, newTextColor);
	oldBrush = (HBRUSH)SelectObject(hdc, newBrush);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	oldTextColor = SetTextColor(hdc, newTextColor);
	oldMode = SetBkMode(hdc, TRANSPARENT);

	//add a border between the edge and the text
	rect.left+=menu_TEXTBORDER;

	//figure the right edge of the text rectangle
	rect.right -= (menu_ARROWWIDTH+menu_POPUPBORDER + menu_ADJUST);

	if(rect.right <  rect.left)
		{
		//not enough room so get rid of border
		rect.left-=menu_TEXTBORDER;
		rect.right = rect.left;
		}
    

	hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBitMap);

  // Draw rectangle around the 'Current Item' area
	Rectangle(hdc, textRect.right, newBounds.top,
    rect.right,
		newBounds.bottom);
    
  // Draw Title inside 'textRect'
	if(pTitle	&& (textRect.right != textRect.left))
  		DrawText(hdc, (char*)pTitle, -1, &textRect, DT_SINGLELINE | DT_LEFT|
			DT_VCENTER);
      
  // Draw border around arrow bitmap
	MoveTo(hdc, rect.right,rect.top);
  LineTo(hdc, (newBounds.right-du1x),rect.top);
  LineTo(hdc, (newBounds.right-du1x),(rect.bottom-du1y));
  LineTo(hdc, (rect.right-du1x),(rect.bottom-du1y));
   
  // Draw current item in Rectangle defined by 'rect'
  wCurrentItem = pMenuInfo->popIndex; 
  
  if (wCurrentItem == -1)
    wCurrentItem = 0;
    
  if (pMenuInfo->itemCount >= (wCurrentItem + 1))
    {
    itemObjID = pItemEntry[wCurrentItem].id;

    if ((pItemEntry[wCurrentItem].status & ITEMENTRY_TEXTONLY) == ITEMENTRY_TEXTONLY)
      {
      pText = (LPSTR)AMemLock((MEMBLOCKID)itemObjID);
      if (pText != 0)
        {
        lstrcpyn(pBuffer, pText, sizeof(pBuffer));  
        pBuffer[sizeof(pBuffer)-1] = '\0';
        AMemUnlock((MEMBLOCKID)pItemEntry[wCurrentItem].id);
        }
      else
        pBuffer[0] = '\0';
      }

	else if   ((pItemEntry[wCurrentItem].status & ITEMENTRY_FOREIGN) ==	ITEMENTRY_FOREIGN)
	  {
	  pTitle = (LPSTR)AMemLock((MEMBLOCKID)itemObjID);

	  //The memory block contains the owner object id so increment pTitle
	  //past the id to the title string.

	  if (pTitle != 0)
	  	pTitle += sizeof(OBJECTID);
	  strcpy(pBuffer, pTitle);
	  }

    else
	  ItemGetTitle(itemObjID, pBuffer, sizeof(pBuffer));

 	if(pBuffer[0] != '\0')
 	  DrawText(hdc, (char*)pBuffer, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    }

	//put the width of the arrow bitmap into hold.
	//make sure the bitmap will not be too wide.
	if(rect.right + menu_ARROWWIDTH >= newBounds.right)
		//subtract 2 since we want the bitmap to lie within the right and left
		//edges.
		hold = newBounds.right - rect.right - 2;
	else
		hold = menu_ARROWWIDTH;

	if(hold > 0)
		{
		StretchBlt(hdc, rect.right, 
			rect.top + menu_POPUPBORDER,
			hold + menu_POPUPBORDER,
			rect.bottom - rect.top - (2 * menu_POPUPBORDER),    
			hdcMem, 0, 0,
			MENU_ARROWWIDTH,
			MENU_ARROWHEIGHT,
			SRCCOPY);
		}
	DeleteDC(hdcMem);

	//Restore old DC
	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);
	SetBkMode(hdc, oldMode);
	SetTextColor(hdc, oldTextColor);
	DeleteObject(hBitMap);
	DeleteObject(newBrush);
	DeleteObject(newPen);

	
  AObjUnlockData(m_oiObject, MENU_INFO_DATA);
  if (pItemEntry)
    AObjUnlockData(m_oiObject, MENU_ITEMLIST_DATA);
  if (pTitle)
    AObjUnlockData(m_oiObject, MENU_TITLE_DATA);
  }
  
////////////////////////////////////////////////////////////////
//
//  Convert Logical units to Device units 
//        (in the x direction only)
//
////////////////////////////////////////////////////////////////
int  MenuPopupVI::LUxtoDUx(
  int x)
  {
  POINT pt;
  pt.x = x;
  pt.y = 0;
  OWndDStoLS(m_oiParent, &pt, 1);
  return pt.x;
  }

////////////////////////////////////////////////////////////////
//
//  Convert Logical units to Device units 
//        (in the y direction only)
//
////////////////////////////////////////////////////////////////
int  MenuPopupVI::LUytoDUy(
  int y)
  {
  POINT pt;
  pt.x = 0;
  pt.y = y;
  OWndDStoLS(m_oiParent, &pt, 1);
  return pt.y;
  }
  