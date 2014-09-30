////////////////////////////////////////////////////////
///  This file contains the code for lite number items
////////////////////////////////////////////////////////


#include "n_includ.h"


LiteNumberItem::LiteNumberItem(
	OBJECTID 								oiItem,
	OBJECTID 								oiParent,
	RECT* 									rcItem,
	long 										status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	LPNUMBER								num;

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "LiteNumberItem::LiteNumberItem", ;);

	num->CopyAttributes(&m_attributes);

	UnlockNumberObject(Object());

	OWndSetItemActivatable(this, FALSE);
	OWndSetItemEnabled(this, FALSE);
	OWndSetItemTransparent(this, TRUE);

	CopyRect(&m_rcLastDrawn, &m_rcItem);

	*m_szLastDrawn = 0;
	}

////////////////

LiteNumberItem::~LiteNumberItem()
	{
	}

////////////////

void ALMAPI LiteNumberItem::Destroy()
	{
  delete this;
	}

////////////////


void ALMAPI LiteNumberItem::DataChanged(
	LONG										details,
	LONG										modifiers,
	LONG										variation)
	{
  LPNUMBER								num;

	I_UNUSED(details);I_UNUSED(modifiers);I_UNUSED(variation);

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "LiteNumberItem::DataChanged", return);

	if (lstrcmp(num->GetStringPtr(), m_szLastDrawn) )
		ForceRedraw();

	UnlockNumberObject(Object());
  }

/////////////////////////////////

void ALMAPI LiteNumberItem::Draw(
	HDC											hdc,
	LPRECT									clipRect)
	{
	NUMBER*									num;
	COLORREF								fore;
	int											mode;
	HFONT										hFont;
	unsigned int						drawFlag;

	I_UNUSED(clipRect);

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "LiteNumberItem::Draw", return);

	drawFlag = num->GetDrawTextStyle();
	lstrcpy(m_szLastDrawn, num->GetStringPtr()); //So if its the same, we won't invalidate again

	m_attributes.AttrCreateFont(hdc);
	hFont = m_attributes.AttrSelectFont(hdc);
	fore	= SetTextColor(hdc, m_attributes.AttrGetForeground());
	mode	= SetBkMode(hdc, TRANSPARENT);

  //Draw the text
	DrawText(hdc, m_szLastDrawn, -1, &m_rcItem, drawFlag);

  /// Calculate the rect we just drew
	SetRect(&m_rcLastDrawn, 0, 0, 0, 0);
	m_rcLastDrawn.right = m_rcItem.right - m_rcItem.left ;
	DrawText(hdc, m_szLastDrawn, -1, &m_rcLastDrawn, drawFlag|DT_CALCRECT);
	m_rcLastDrawn.top = m_rcItem.top;
	m_rcLastDrawn.left = m_rcItem.left;
	m_rcLastDrawn.bottom += m_rcItem.top;
	m_rcLastDrawn.right += m_rcItem.left;

  //Restore the hdc
	SetBkMode(hdc, mode);
	SetTextColor(hdc, fore);
	SelectObject(hdc, hFont);
	m_attributes.AttrDeleteFont();

	UnlockNumberObject(Object());
	}

///////////////////////

void ALMAPI LiteNumberItem::Print(
	HDC 										hdc, 
	RECT* 									printRect, 
	int 										pclPrinter)
	{
	LPNUMBER								num;
	ATTRIBUTES							attPrinter(&m_attributes);
	COLORREF								fore, oldFore;
	int											oldMode;
	HFONT										hFont;
	unsigned int						drawFlag;
	BOOL										bIsColor;

	I_UNUSED(pclPrinter);

	bIsColor = GetDeviceCaps(hdc, NUMCOLORS) > 2;

	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black

	num = LockNumberObject(Object());
	I_ASSERT_NUM(num, "LiteNumberItem::Print", return);

	drawFlag = num->GetDrawTextStyle();

	attPrinter.AttrCreateFont(hdc);
	hFont = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldMode = SetBkMode(hdc, TRANSPARENT);

  //Draw the text
	DrawText(hdc, num->GetStringPtr(), -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFont);
	attPrinter.AttrDeleteFont();

	UnlockNumberObject(Object());
	}

////////////////

void LiteNumberItem::ChangeFont(
	LPATTRIBUTES 						newAttr,
	long										changeFlag)
	{
	//The lite text item creates and deletes the font and brush handles when drawing
	if (m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr) )
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
	}

////////////////

BOOL far pascal _export LiteNumberItemChangeFont(
	OpWndItemD* 						lwi, 
	long 										attrPtr, 
	long 										changeFlag)
	{
	((LiteNumberItem *)lwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
	}

//////////////////////////////

void LiteNumberItem::ForceRedraw()
	{
	OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, FALSE);				//Invalidate the whole
	if (m_rcLastDrawn.top != m_rcLastDrawn.bottom)									//If they are equal, the string must have been empty or the text is off the screen
		OWndInvalidateLogicalRect(m_oiParent, &m_rcLastDrawn, TRUE); 	//Erase what was there
	}

////////////////

BOOL far pascal _export LiteNumberItemForceRedraw(
	OpWndItemD* 						lwi, 
	long 										notused, 
	long 										notusedEither)
	{
	I_UNUSED(notused);I_UNUSED(notusedEither);

	((LiteNumberItem *)lwi)->ForceRedraw();
	return TRUE;
	}

