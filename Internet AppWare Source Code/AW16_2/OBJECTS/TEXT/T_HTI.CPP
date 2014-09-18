

////////////////////////////////////////////////////////
///  This file contains the code for hwnd'd text items
////////////////////////////////////////////////////////

#include "t_includ.h"

#define I_TYPEAHEAD_LENGTH	20

LRESULT far pascal _export AppwareTextObjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



////////////////////////////////////////////////////////////////

HwndTextItem::HwndTextItem(
	OBJECTID		oiItem,
	OBJECTID		oiParent,
	RECT*				rcItem,
	long				status) :	OpHwndWndItem(oiItem, oiParent, rcItem, status)
	{
	TEXT*						txt;

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::HwndTextItem", ;);

	txt->CopyAttributes(&m_attributes);
	m_selection = txt->SelectsAll() ? MAKELONG(0,-1) : 0L;

	OWndSetItemActivatable(this, TRUE);
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
	OWndSetItemUsesDefaultKey(this, (txt->IsEditable() && txt->UsesEnter()) );
	OWndSetItemUsesTab(this, (txt->IsEditable() && txt->UsesTab()) );
	OWndSetItemUsesCursorKeys(this, TRUE);

	UnlockTextStyle(Object());
	}

/////////////////////

void ALMAPI HwndTextItem::Destroy()
	{
  delete this;
	}

/////////////////////////////////

void ALMAPI HwndTextItem::DataChanged(
	long										details,
	long 										modifiers,
	long 										variation)
	{
	LPSTR										string;
	MEMBLOCKID							miCurrentString;
	LPSTR										szCurrentString;
	int											len, iFirstVisibleLine;

	I_UNUSED(details);	I_UNUSED(modifiers);	I_UNUSED(variation);

	string = LockTextString(Object());

	if (m_hwndItem && m_hwndItem != (HWND)modifiers) // No need to change the one that triggered the change!!
		{
		len = GetWindowTextLength(m_hwndItem)+1;
		miCurrentString = AMemAllocate(len);
		szCurrentString = (LPSTR)AMemLock(miCurrentString);
		GetWindowText(m_hwndItem, szCurrentString, len);

		if (lstrcmp(string, szCurrentString) )
			{	
			iFirstVisibleLine = (int)SendMessage(m_hwndItem, EM_GETFIRSTVISIBLELINE, 0, 0L);
			if (iFirstVisibleLine)
				SendMessage(m_hwndItem, WM_SETREDRAW, FALSE, 0L);

			if (OWndGetActiveItem() == this)
				{
				m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L);
				SetWindowText(m_hwndItem, string);
				SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection);
				}
			else
				SetWindowText(m_hwndItem, string);

			if (iFirstVisibleLine)
				{
				iFirstVisibleLine -= (int)SendMessage(m_hwndItem, EM_GETFIRSTVISIBLELINE, 0, 0L);
				SendMessage(m_hwndItem, WM_SETREDRAW, TRUE, 0L);
				if (IsWindowVisible(m_hwndItem))	 //cause MicroSucks will show the window in EM_LINESCROLL
					SendMessage(m_hwndItem, EM_LINESCROLL, 0, MAKELONG(iFirstVisibleLine, 0));
				}
			}

		AMemUnlock(miCurrentString);
		AMemFree(miCurrentString);
		}

  UnlockTextString(Object());
	}

/////////////////////////////////

void ALMAPI HwndTextItem::ParentOpened()
	{
	TEXT*							txt;
  LPSTR							string;
	HDC								hdc;

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::ParentOpened", return);


	m_hwndItem = OWndCreateWindowEx(
									this,
									0,
									g_textObjectClassName,
									"",
									txt->GetCreateWindowStyle(),
									GetHInstanceToUse(),
									this);

	UnlockTextStyle(Object());

	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::ParentOpened", ;);

  string = LockTextString(Object());
	I_ASSERT_STRING(string, "HwndTextItem::ParentOpened", ;);

	SetWindowText(m_hwndItem, string);

	UnlockTextString(Object());

	
	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);
	SetTabStops();
	SetEditLimit();
	}

//////////////////////

void ALMAPI HwndTextItem::ParentClosing()
	{

	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::ParentClosing", return);

	ReleaseTabStops();

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);

	if (m_hEditDS)
		{
		GlobalFree(m_hEditDS);
		m_hEditDS=0;
		}

	m_hwndItem = 0;
  }

////////////////////////////

BOOL ALMAPI HwndTextItem::GetSelection(
	LPLONG 									start,
	LPLONG 									count)
	{
	LPSTR										szText;
	UINT										uStart, uCount;
	UINT										uSelectionStart, uSelectionEnd;

	if (m_hwndItem  &&  OWndGetActiveItem() == this)
		m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L);

	uSelectionStart = LOWORD(m_selection);
	uSelectionEnd = HIWORD(m_selection);

	uStart = uSelectionStart;										 //start will always be less than end. see Technical Articals: Window Manager (search for EM_GETSEL) on developer CD
	uCount = uSelectionEnd - uSelectionStart;

	szText = LockTextString(Object());
	I_ASSERT_STRING(szText, "HwndTextItem::GetSelection", return FALSE);

	ActualPosAndCountToParameter(szText, &uStart, &uCount);

	UnlockTextString(Object());

	*start = (LONG)uStart;
	*count = (LONG)uCount;

  return TRUE;
	}

////////////////////////

BOOL ALMAPI HwndTextItem::SetSelection(
	long										start,
	long 										count)
	{
	UINT										uStart, uCount;
	LPSTR										szText;

	if (count < 0)
		{
		count = -count;
		if (count > start)
			count = start;
		start -= count;
		}

	uStart = (UINT)start;
	uCount = (UINT)count;

	szText = LockTextString(Object());
	I_ASSERT_STRING(szText, "HwndTextItem::SetSelection", return FALSE);

	ParameterPosAndCountToActual(szText, &uStart, &uCount);

	UnlockTextString(Object());

	m_selection = MAKELONG (uStart, uStart+uCount);

	if (IsWindow(m_hwndItem))
		SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection);

  return TRUE;
	}

////////////////////////////

long ALMAPI HwndTextItem::Activate(
	int											causedBy,
	BOOL 										mustKeep)
	{
	LPTEXT									txt;
	LPTEXTRUNTIME						tr;

  I_UNUSED(mustKeep);

	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Activate", return OWND_ACTIVATION_PASS);

	tr = GetTextRuntime();
	I_ASSERT_TR(tr, "", return OWND_ACTIVATION_KEPT);
	tr->SetLastActive(this);
  
  txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::Activate", return OWND_ACTIVATION_KEPT);

	if (causedBy != OWND_ACTIVATED_BY_MOUSE)
  	{
		if (txt->SelectsAll() )
			SendMessage(m_hwndItem, EM_SETSEL, 0, MAKELONG (0, -1) );
		else
			SendMessage(m_hwndItem, EM_SETSEL, 0, m_selection);
		}

	if (GetFocus() != m_hwndItem)
		SetFocus(m_hwndItem);

	m_whenChangeLastProcessed = 0;


	UnlockTextStyle(Object());

	return OWND_ACTIVATION_KEPT;
	}

/////////////////////////////////

void ALMAPI HwndTextItem::Deactivated()
	{

	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Deactivated", return);

	//Get the position of the cursor
	m_selection = SendMessage(m_hwndItem, EM_GETSEL, 0, 0L);

	if (AAppGetTime() < m_whenChangeLastProcessed)
		PostObjectChanged(Object(), MAKELONG( (int)m_hwndItem, 0), DONTPOSTEDITED, NODELAY);
	}

/////////////////////////////////

LRESULT ALMAPI HwndTextItem::ParentNotification(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
  LPARAM									rVal;

	I_UNUSED(hwnd);

	switch(message)
		{
    case WM_COMMAND:
			switch(HIWORD(lParam))
				{
				case EN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_ACTIVATED);
					break;

				case EN_KILLFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_DEACTIVATED);
					break;

				case EN_MAXTEXT:
					AEvtPostSignalAtTail(Object(), I_SIG_LENGTHERROR);
					break;

				case EN_CHANGE:
					UpdateIfModified();
					break;
				}
      rVal = 0L;
			break;

		case WM_CTLCOLOR:
			SetTextColor((HDC)wParam, m_attributes.AttrGetForeground());
			SetBkColor((HDC)wParam, m_attributes.AttrGetBackground());
			rVal = (LRESULT) m_attributes.AttrGetBrushHandle();
			break;

    default:
			rVal = 0L;
			break;
		}

  return rVal;
  }

////////////////////////////

static void PclDrawRect(
	HDC											hdc,
	LPRECT									rect)
	{
	PRECT_STRUCT						prect;

	prect.prStyle = 0;

  // top line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->top;
	prect.prSize.x = rect->right - rect->left;
  prect.prSize.y = 1;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // left line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->top;
	prect.prSize.x = 1;
	prect.prSize.y = rect->bottom - rect->top;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // right line
	prect.prPosition.x = rect->right;
	prect.prPosition.y = rect->top;
	prect.prSize.x = 1;
	prect.prSize.y = rect->bottom - rect->top;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

  // bottom line
	prect.prPosition.x = rect->left;
	prect.prPosition.y = rect->bottom;
	prect.prSize.x = rect->right - rect->left;
  prect.prSize.y = 1;
	Escape(hdc, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
	}

////////////////////////////

void ALMAPI HwndTextItem::Print(
	HDC 										hdc, 
	RECT* 									printRect, 
	int 										pclPrinter)
	{
	ATTRIBUTES							attPrinter(&m_attributes);
	LPSTR										string;
  TEXT*										txt;
	COLORREF								fore, back, oldFore, oldBack;
	int											oldMode;
	HFONT										hFontOld;
	HBRUSH									hBrush, hBrushOld;
	unsigned int						drawFlag;
	POINT										ptDeflate;
	BOOL										bIsColor;

	bIsColor = GetDeviceCaps(hdc, NUMCOLORS) > 2;

	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black
	back = bIsColor ? attPrinter.AttrGetBackground() : RGB(255,255,255);//white

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::Print", return);

	string = LockTextString(Object());
	I_ASSERT_STRING(string, "HwndTextItem::Print", UnlockTextStyle(Object());return);

	hBrush = CreateSolidBrush(back);
	hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
	attPrinter.AttrCreateFont(hdc);
	hFontOld = attPrinter.AttrSelectFont(hdc);
	oldFore	= SetTextColor(hdc, fore);
	oldBack = SetBkColor(hdc, back);
	oldMode = SetBkMode(hdc, TRANSPARENT);
	
	if (txt->HasFrame())
		{
		if (pclPrinter) 
			PclDrawRect(hdc, printRect);
		else
			Rectangle(hdc, printRect->left, printRect->top, printRect->right, printRect->bottom);
		ptDeflate.x = MulDiv(GetDeviceCaps(hdc, LOGPIXELSX), 1, 16); //Shrink by 1/16 of an inch
		ptDeflate.y = MulDiv(GetDeviceCaps(hdc, LOGPIXELSY), 1, 16); //Shrink by 1/16 of an inch
		InflateRect(printRect, -ptDeflate.x, -ptDeflate.y);
		}
	else
		{//No frame
		FillRect(hdc, printRect, hBrush);
		}

	drawFlag = txt->GetDrawTextStyle();

  //Draw the text
	DrawText(hdc, string, -1, printRect, drawFlag);

  //Restore the hdc
	SetBkMode(hdc, oldMode);
	SetBkColor(hdc, oldBack);
	SetTextColor(hdc, oldFore);
	SelectObject(hdc, hFontOld);
	attPrinter.AttrDeleteFont();
	SelectObject(hdc, hBrushOld);
	DeleteObject(hBrush);

	UnlockTextStyle(Object());
  UnlockTextString(Object());
	}

/////////////////////////////////

void ALMAPI HwndTextItem::Cut()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Cut", return);

	SendMessage(m_hwndItem, WM_CUT, 0, 0L);
	}

/////////////////////////////////

void ALMAPI HwndTextItem::Copy()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Copy", return);

	SendMessage(m_hwndItem, WM_COPY, 0, 0L);
	}

/////////////////////////////////

void ALMAPI HwndTextItem::Paste()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Paste", return);

	SendMessage(m_hwndItem, WM_PASTE, 0, 0L);
	}

/////////////////////////////////

void ALMAPI HwndTextItem::Undo()
	{
	I_ASSERT_WINDOW(m_hwndItem, "HwndTextItem::Undo", return);

	SendMessage(m_hwndItem, EM_UNDO, 0, 0L);
	}

//////////////////////////////////////
// This function will check and see if the edit control
// has been modified.  If it has, it will post the necessary
// signals and return TRUE.  Otherwise, it will return FALSE
//////////////////////////////////////
BOOL HwndTextItem::UpdateIfModified()
	{
	LPSTR										string;
	int											stringLen, dataLen;
	BOOL										rVal = FALSE;

	if (SendMessage(m_hwndItem, EM_GETMODIFY, 0, 0L))
		{
		SendMessage(m_hwndItem, EM_SETMODIFY, 0, 0L);  //Clear the modified bit

		stringLen = GetWindowTextLength(m_hwndItem)+1;
		dataLen = (int)AObjGetDataSize(Object(), I_DATA_STRING);

		if (dataLen < stringLen)	//getting bigger
			ResizeTextString(Object(), stringLen + I_TYPEAHEAD_LENGTH);
		else if (dataLen > stringLen + I_TYPEAHEAD_LENGTH)	//getting smaller
			ResizeTextString(Object(), stringLen);

		string = LockTextString(Object());
		I_ASSERT_STRING(string, "HwndTextItem::UpdateIfModified", ;);
		GetWindowText(m_hwndItem, string, stringLen);
		UnlockTextString(Object());

		m_whenChangeLastProcessed = PostObjectChanged(Object(), MAKELONG( (int)m_hwndItem, 0), POSTEDITED, DELAY);

		rVal = TRUE;
		}

	return rVal;
	}

///////////////////////////////////////
//  This function will return the hinstance to use
//  when creating the window handle.  It will also
//  Initialize the editDS
///////////////////////////////////////
HINSTANCE HwndTextItem::GetHInstanceToUse()
	{
	TEXT*										txt;
	LPVOID									ptr;
	HINSTANCE								hInstanceToUse;

	txt = LockTextStyle(Object());

	if (txt->IsMultiline())
		{
		m_hEditDS = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE |
													GMEM_ZEROINIT, 1024);
		ptr = GlobalLock(m_hEditDS);
		LocalInit(HIWORD((LONG)ptr), 0, (WORD)(GlobalSize(m_hEditDS) - 16));
		UnlockSegment(HIWORD((LONG)ptr));
		hInstanceToUse =(HINSTANCE) HIWORD((LONG)ptr);
		}
	else
		{
		m_hEditDS=0;
		hInstanceToUse = g_hInstance;
		}

	UnlockTextStyle(Object());

  return hInstanceToUse;
	}

///////////////////////////

void HwndTextItem::SetTabStops()
	{
	HDC											hdc;
	int											width;
  LPTEXT									txt;

	hdc = GetDC(m_hwndItem);
	GetCharWidth(hdc, ' ', ' ', &width);
	ReleaseDC(m_hwndItem, hdc);

  txt = LockTextStyle(Object());
	width *= MulDiv((int)GetDialogBaseUnits(), txt->GetTabSize(), 8);

	UnlockTextStyle(Object());

	SendMessage(m_hwndItem, EM_SETTABSTOPS, 1, (LPARAM)&width);
	}

/////////////////////////

void HwndTextItem::ReleaseTabStops()
	{
	///This function is necessary because a 12-byte fixed object in the local heap
	///is not freed when the control is destroyed if tabs have been set unless they
	///are manually unset.  See developer cd  pss id Q66452
	SendMessage(m_hwndItem, EM_SETTABSTOPS, 0, (LONG)NULL );
  }

//////////////////////////////////////



void HwndTextItem::SetEditLimit()
	{
	LPTEXT									txt;

	txt = LockTextStyle(Object());
	SendMessage(m_hwndItem, EM_LIMITTEXT, txt->GetMaxNumChars(), 0L);
  UnlockTextStyle(Object());
	}


////////////////////////////////////////////////////////////////
/// Function called by the wndProc  ////////////////////////////
////////////////////////////////////////////////////////////////

LRESULT HwndTextItem::WmSetCursor(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
	LPTEXT									txt;
	BOOL										isEditable;

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::WmSetCursor", return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam));

	isEditable = txt->IsEditable();
	UnlockTextStyle(Object());

	if (!isEditable)
		{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
		}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
  }


//////////

LRESULT HwndTextItem::WmEraseBkgnd(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
  {
	HDC											hdc;
	RECT										rect;

	I_UNUSED(message);I_UNUSED(lParam);
	
	hdc = (HDC)wParam;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, m_attributes.AttrGetBrushHandle() );

	return TRUE;
	}
//////////

LRESULT HwndTextItem::WmChar(
	HWND 										hwnd,
	UINT 										message,
	WPARAM 									wParam,
	LPARAM 									lParam)
	{
  LRESULT									rVal;
	LPTEXT									txt;

	txt = LockTextStyle(Object());
	I_ASSERT_TXT(txt, "HwndTextItem::WmChar", return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam));
  
	if (wParam == VK_RETURN)
		AEvtPostSignalAtTail(Object(), I_SIG_ENTERKEY);

	if (wParam == VK_RETURN && !txt->UsesEnter())
		rVal = 0L;
	else if (wParam == VK_TAB && !txt->UsesTab())
		rVal = 0L;
	else
		rVal = CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);

	UnlockTextStyle(Object());

	return rVal;
	}

//////////////////////////////////////////////////////////////////////
//  This is the callback function for the subclassed edit control
//  for the text object  
//////////////////////////////////////////////////////////////////////

LRESULT far pascal _export AppwareTextObjectProc(
	HWND										hwnd,
	UINT										message,
	WPARAM									wParam,
	LPARAM									lParam)
	{
	HwndTextItem*						hti;
	LPCREATESTRUCT					cr;

	switch(message)
		{
		case WM_CREATE:
			cr = (LPCREATESTRUCT)lParam;
			hti =(HwndTextItem*)cr->lpCreateParams;
			SetThisPointer(hwnd, hti);
			hti = GetThisPointer(hwnd);
			break;


#if 0  //This code is probably not necessary because ES_READONLY is now used...
		case WM_SETCURSOR:
			hti = GetThisPointer(hwnd);
			I_ASSERT_HTI(hti, "AppwareTextObjectProc", break);
      return hti->WmSetCursor(hwnd, message, wParam, lParam);
#endif

		case WM_ERASEBKGND:
			hti = GetThisPointer(hwnd);
			I_ASSERT_HTI(hti, "AppwareTextObjectProc", break);
			return hti->WmEraseBkgnd(hwnd, message, wParam, lParam);

		case WM_CHAR:
			hti = GetThisPointer(hwnd);
			I_ASSERT_HTI(hti, "AppwareTextObjectProc", break);
			return hti->WmChar(hwnd, message, wParam, lParam);
		}

	return CallWindowProc(g_defEditProc, hwnd, message, wParam, lParam);
	}

////////////////////////////////////////////////
////////////////////////////////////////////////
void HwndTextItem::ChangeFont(
	LPATTRIBUTES 						newAttr,
	long										changeFlag)
	{
	HDC											hdc;

	if (m_hwndItem)
		{ //If our window handle is created, the font and brush handles will be too.
		hdc = GetDC(m_hwndItem);
		m_attributes.AttrChangeFontIndirect(hdc, m_hwndItem, changeFlag, newAttr);
		ReleaseDC(m_hwndItem, hdc);
		}
	else
		m_attributes.AttrChangeFontIndirect(NULL, NULL, changeFlag, newAttr);
	}

////////////////////////////////////////////////
BOOL far pascal HwndTextItemChangeFont(
	OpWndItemD* 						hwi, 
	long 										attrPtr, 
	long 										changeFlag)
	{

	((HwndTextItem *)hwi)->ChangeFont((LPATTRIBUTES)attrPtr, changeFlag);
	return TRUE;
	}


