/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE: dt_ldi.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Lite date item
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "dt_incld.h"

LiteDateItem::LiteDateItem(OBJECTID	oiItem,	OBJECTID oiParent, RECT* rcItem, long	status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
{
	LPDate date;

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "LiteDateItem::LiteDateItem", ;);

	date->CopyAttributes(&m_attributes);
	CopyRect(&m_rcLastDrawn, &m_rcItem);
	m_miLastDrawnString = AMemAllocate(1);

	OWndSetItemActivatable(this, FALSE);
	OWndSetItemEnabled(this, FALSE);
	OWndSetItemTransparent(this, TRUE);

	UnlockDateObject(Object());

}

LiteDateItem::~LiteDateItem()
{
	AMemFree(m_miLastDrawnString);
}

void ALMAPI LiteDateItem::Destroy()
{
  delete this;
}

void ALMAPI LiteDateItem::DataChanged(LONG details,	LONG modifiers,	LONG variation)
{
  LPDate	date;
	LPSTR   lastDrawn;


	I_UNUSED(details);
	I_UNUSED(modifiers);
	I_UNUSED(variation);

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "LiteDateItem::DataChanged", return);
	lastDrawn = (LPSTR)AMemLock(m_miLastDrawnString);

	if (lstrcmp(date->GetDateStringPtr(), lastDrawn))
		ForceRedraw();

	AMemUnlock(m_miLastDrawnString);
	UnlockDateObject(Object());
}

void ALMAPI LiteDateItem::ParentOpened()
{
	HDC	hdc;

	hdc = GetDC(m_hwndParent);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndParent, hdc);
}

void ALMAPI LiteDateItem::ParentClosing()
{
	m_attributes.AttrDeleteFont();
}


void ALMAPI LiteDateItem::Draw(HDC hdc,	LPRECT clipRect)
{
	LPSTR					lastDrawn;
	LPDate 		  	time;
	COLORREF		  fore;
	int			  		mode;
	HFONT					hFont;
	unsigned int	drawFlag;

	I_UNUSED(clipRect);

	time = LockDateObject(Object());
	I_ASSERT_DATE(time, "LiteDATEItem::Draw", return);

	drawFlag = time->GetDrawDateStyle();

	AMemSetSize(m_miLastDrawnString, lstrlen(time->GetDateStringPtr())+1);
	lastDrawn = (LPSTR) AMemLock(m_miLastDrawnString);
	lstrcpy(lastDrawn, time->GetDateStringPtr()); //So if its the same, we won't invalidate again
	AMemUnlock(m_miLastDrawnString);

	m_attributes.AttrCreateFont(hdc);
	hFont = m_attributes.AttrSelectFont(hdc);
	fore	= SetTextColor(hdc, m_attributes.AttrGetForeground());
	mode	= SetBkMode(hdc, TRANSPARENT);

  //Draw the text
	DrawText(hdc, time->GetDateStringPtr(), -1, &m_rcItem, drawFlag);

  /// Calculate the rect we just drew
	SetRect(&m_rcLastDrawn, 0, 0, 0, 0);
	m_rcLastDrawn.right = m_rcItem.right - m_rcItem.left ;
	DrawText(hdc, time->GetDateStringPtr(), -1, &m_rcLastDrawn, drawFlag|DT_CALCRECT);
	m_rcLastDrawn.top = m_rcItem.top;
	m_rcLastDrawn.left = m_rcItem.left;
	m_rcLastDrawn.bottom += m_rcItem.top;
	m_rcLastDrawn.right += m_rcItem.left;

  //Restore the hdc																				 2
	SetBkMode(hdc, mode);
	SetTextColor(hdc, fore);
	SelectObject(hdc, hFont);
	m_attributes.AttrDeleteFont();

	UnlockDateObject(Object());
}

void ALMAPI LiteDateItem::Print(HDC	hdc, RECT* printRect, int	pclPrinter)
{
	ATTRIBUTES	 	attPrinter(&m_attributes);
  LPDate			 	date;
	COLORREF		 	fore, oldFore;
	int					 	oldMode;
	HFONT				 	hFont;
	unsigned int 	drawFlag;
	BOOL				 	bIsColor;

	I_UNUSED(pclPrinter);

	bIsColor = GetDeviceCaps(hdc, NUMCOLORS) > 2;

	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black

	date = LockDateObject(Object());
	I_ASSERT_DATE(date, "HwndDateItem::Print", return);

	drawFlag = date->GetDrawDateStyle();

	attPrinter.AttrCreateFont(hdc);
	hFont = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldMode = SetBkMode(hdc, TRANSPARENT);

  //Draw the text
	DrawText(hdc, date->GetDateStringPtr(), -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFont);
	attPrinter.AttrDeleteFont();

	UnlockDateObject(Object());

}

void LiteDateItem::ChangeFont(LPATTRIBUTES newAttr,	long changeFlag)
{
	//The lite text item creates and deletes the font and brush handles when drawing
	if (m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr) )
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
}

BOOL CALLBACK LiteDateItemChangeFont(OpWndItemD* lwi, long attrPtr, long changeFlag)
{
	((LiteDateItem *)lwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
}


void LiteDateItem::ForceRedraw()
{
	OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, FALSE);				//Invalidate the whole
	if (m_rcLastDrawn.top != m_rcLastDrawn.bottom)									//If they are equal, the string must have been empty or the text is off the screen
		OWndInvalidateLogicalRect(m_oiParent, &m_rcLastDrawn, TRUE); 	//Erase what was there
}

BOOL CALLBACK LiteDateItemForceRedraw(OpWndItemD*	lwi, long	notused, long notusedEither)
{
	I_UNUSED(notused);I_UNUSED(notusedEither);

	((LiteDateItem *)lwi)->ForceRedraw();
	return TRUE;
}
