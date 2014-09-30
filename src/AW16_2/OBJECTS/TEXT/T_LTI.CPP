
////////////////////////////////////////////////////////
///  This file contains the code for lite text items
////////////////////////////////////////////////////////


#include "t_includ.h"


LiteTextItemBase::LiteTextItemBase(
	OBJECTID 								oiItem,
	OBJECTID 								oiParent,
	RECT* 									rcItem,
	long 										status):OpLiteWndItem(oiItem, oiParent, rcItem, status)
	{
	TEXT*										txt;

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "LiteTextItemBase::LiteTextItemBase", ;);

  txt->CopyAttributes(&m_attributes);
  CopyRect(&m_rcLastDrawn, &m_rcItem);
	m_miLastDrawnString = AMemAllocate(1);

	OWndSetItemActivatable(this, FALSE);
	OWndSetItemEnabled(this, FALSE);

  UnlockTextStyle(Object());
	}

////////////////

LiteTextItemTransparent::LiteTextItemTransparent(
	OBJECTID 								oiItem,
	OBJECTID 								oiParent,
	RECT* 									rcItem,
	long 										status):LiteTextItemBase(oiItem, oiParent, rcItem, status)
	{
	OWndSetItemTransparent(this, TRUE);
	}

/////////////////

LiteTextItemFramed::LiteTextItemFramed(
	OBJECTID 								oiItem,
	OBJECTID 								oiParent,
	RECT* 									rcItem,
	long 										status):LiteTextItemBase(oiItem, oiParent, rcItem, status)
	{
	OWndSetItemTransparent(this, FALSE);
	}

/////////////////

LiteTextItemBase::~LiteTextItemBase()
	{
	AMemFree(m_miLastDrawnString);
	}

////////////////

void ALMAPI LiteTextItemBase::Destroy()
	{
  delete this;
	}

////////////////


void ALMAPI LiteTextItemBase::DataChanged(
	LONG										details,
	LONG										modifiers,
	LONG										variation)
	{
	LPSTR										string, lastDrawn;

	I_UNUSED(details);I_UNUSED(modifiers);I_UNUSED(variation);

	string = LockTextString(Object());
	lastDrawn = (LPSTR)AMemLock(m_miLastDrawnString);

	if (lstrcmp(string, lastDrawn) )
  	{
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, FALSE);				//Invalidate the whole
		if (m_rcLastDrawn.top != m_rcLastDrawn.bottom)									//If they are equal, the string must have been empty or the text is off the screen
			OWndInvalidateLogicalRect(m_oiParent, &m_rcLastDrawn, TRUE);	//Erase what was there
		}

	AMemUnlock(m_miLastDrawnString);
  UnlockTextString(Object());
  }

/////////////////////////////////

void LiteTextItemBase::DrawTheText(
	HDC											hdc,
	LPRECT									clipRect, 
	BOOL										bIndent)
	{
	LPSTR										string, lastDrawn;
  TEXT*										txt;
	COLORREF								fore;
	int											mode;
	HFONT										hFont;
	unsigned int						drawFlag;
	RECT										rcToDrawIn;

	I_UNUSED(clipRect);

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "LiteTextItemBase::Draw", return);

	string = LockTextString(Object());
	I_ASSERT_STRING(string, "LiteTextItemBase::Draw", UnlockTextStyle(Object());return);

	drawFlag = txt->GetDrawTextStyle();

	AMemSetSize(m_miLastDrawnString, lstrlen(string)+1);
	lastDrawn = (LPSTR) AMemLock(m_miLastDrawnString);
	lstrcpy(lastDrawn, string); //So if its the same, we won't invalidate again
	AMemUnlock(m_miLastDrawnString);

	m_attributes.AttrCreateFont(hdc);
	hFont = m_attributes.AttrSelectFont(hdc);
	fore	= SetTextColor(hdc, m_attributes.AttrGetForeground());
	mode	= SetBkMode(hdc, TRANSPARENT);

	//Set the rectangle to draw in
	CopyRect(&rcToDrawIn, &m_rcItem);
	if (bIndent)
		{  //Indent on mm, so the text isn't on the frame
		rcToDrawIn.left += 10;
		rcToDrawIn.right -= 10;
		}

  //Draw the text in that rectangle
	DrawText(hdc, string, -1, &rcToDrawIn, drawFlag);

  /// Calculate the rect we just drew
	SetRect(&m_rcLastDrawn, 0, 0, 0, 0);
	m_rcLastDrawn.right = rcToDrawIn.right - rcToDrawIn.left ;
	DrawText(hdc, string, -1, &m_rcLastDrawn, drawFlag|DT_CALCRECT);
	m_rcLastDrawn.top = rcToDrawIn.top;
	m_rcLastDrawn.left = rcToDrawIn.left;
	m_rcLastDrawn.bottom += rcToDrawIn.top;
	m_rcLastDrawn.right += rcToDrawIn.left;

  //Restore the hdc
	SetBkMode(hdc, mode);
	SetTextColor(hdc, fore);
	SelectObject(hdc, hFont);
	m_attributes.AttrDeleteFont();

	UnlockTextStyle(Object());
  UnlockTextString(Object());
  }

//////////////////

void ALMAPI LiteTextItemTransparent::Draw(
	HDC											hdc,
	LPRECT									clipRect)
	{
	DrawTheText(hdc, clipRect, FALSE);
	}

//////////////////
void ALMAPI LiteTextItemFramed::Draw(
	HDC											hdc,
	LPRECT									clipRect)
	{
	HPEN										hPen;
	HBRUSH									hBrush;

	m_attributes.AttrCreateBrush();	 							//Create the background brush									 
	hBrush = m_attributes.AttrSelectBrush(hdc);		//and select it. hBrush is now the old value

	hPen = (HPEN) GetStockObject(BLACK_PEN);		//Get the black pen. (the edit control always uses it)
	hPen = (HPEN) SelectObject(hdc, hPen);			//and select it. hPen is now the old value

	Rectangle(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

	SelectObject(hdc, hBrush); //Select in the old
	SelectObject(hdc, hPen);

	m_attributes.AttrDeleteBrush();	 //Delete the brush 

	DrawTheText(hdc, clipRect, TRUE);	 //Call the common code...it draws the text
	}
//////////////////

void ALMAPI LiteTextItemBase::Print(
	HDC 										hdc, 
	RECT* 									printRect, 
	int 										pclPrinter)
	{
	ATTRIBUTES							attPrinter(&m_attributes);
	LPSTR										string;
  TEXT*										txt;
	COLORREF								fore, oldFore;
	int											oldMode;
	HFONT										hFont;
	unsigned int						drawFlag;
	BOOL										bIsColor;

	I_UNUSED(pclPrinter);

	bIsColor = GetDeviceCaps(hdc, NUMCOLORS) > 2;

	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "LiteTextItemBase::Print", return);

	string = LockTextString(Object());
	I_ASSERT_STRING(string, "LiteTextItemBase::Print", UnlockTextStyle(Object());return);

	drawFlag = txt->GetDrawTextStyle();

	attPrinter.AttrCreateFont(hdc);
	hFont = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldMode = SetBkMode(hdc, TRANSPARENT);

  //Draw the text
	DrawText(hdc, string, -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFont);
	attPrinter.AttrDeleteFont();

	UnlockTextStyle(Object());
  UnlockTextString(Object());
	}


////////////////

void LiteTextItemBase::ChangeFont(
	LPATTRIBUTES 						newAttr,
	long										changeFlag)
	{
	//The lite text item creates and deletes the font and brush handles when drawing
	if (m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr) )
		OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE);
	}

////////////////
BOOL far pascal LiteTextItemChangeFont(
	OpWndItemD* 						lwi, 
	long 										attrPtr, 
	long 										changeFlag)
	{
	((LiteTextItemBase *)lwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
	}

