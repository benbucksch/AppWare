////////////////////////////////////////////////////////////////
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
// FILE:"tmnl_vi.cpp"
//
// AUTHOR: Sibai Li
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <mem.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"

long DispAttributes[] = { 0x00000000L,      /* Black       */
                          0x00800000L,      /* Blue        */
                          0x00008000L,      /* Green       */
                          0x00808000L,      /* Cyan        */
                          0x00000080L,      /* Red         */
                          0x00800080L,      /* Magenta     */
                          0x00008080L,      /* Brown       */
                          0x00c0c0c0L,      /* LtGrey      */
                          0x00808080L,      /* DkGrey      */
                          0x00ff0000L,      /* LtBlue      */
                          0x0000ff00L,      /* LtGreen     */
                          0x00ffff00L,      /* LtCyan      */
                          0x000000ffL,      /* LtRed       */
                          0x00ff00ffL,      /* LtMagenta   */
													0x0000ffffL,      /* Yellow      */
													0x00ffffffL };
/**************************************************************************
**************************************************************************/
TMNL_VI::TMNL_VI (
	OBJECTID oiObj,
	OBJECTID oiParent,
	RECT* rcItem,
	long statusFlag):OpHwndWndItem(oiObj, oiParent, rcItem, statusFlag)
{
	int Emulation ;
	
	SetupClass () ;
	oiTmnl = oiObj ;
  TERMINAL terminal(oiObj) ;
	Emulation = terminal.GetEmulationType() ;
	if (Emulation==0) run_tmnl = new TTYTerm (oiTmnl) ;
	if (Emulation==1)	run_tmnl = new AnsiTerm (oiTmnl) ;
	if (Emulation==2) run_tmnl = new VT52Term (oiTmnl) ;

	OWndSetItemEnabled (this, TRUE) ;
	if (terminal.GetActivate()) 
		{
		OWndSetItemActivatable(this, TRUE) ;
		OWndSetItemUsesTab (this, TRUE) ;
		}
	else
		OWndSetItemActivatable (this, FALSE) ;

  AObjSetRuntimeInfo (oiTmnl, (long)run_tmnl) ;
}
TMNL_VI::~TMNL_VI (void)
{
	if (run_tmnl)
		delete run_tmnl ;
}
void ALMAPI TMNL_VI::ParentOpened (void)
{
	if (OWndGetItemControlID(this))
		CreateTerminal() ;
}
void ALMAPI TMNL_VI::ParentClosing (void)
{
	if (m_hwndItem)
		DestroyTerminal() ;
}
long ALMAPI TMNL_VI::Activate (
	int causedBy,
	BOOL mustKeep)
{
	TERMINAL terminal(oiTmnl) ;

	I_UNUSED (causedBy) ;
	I_UNUSED (mustKeep) ;

	if (terminal.GetActivate()) //run_tmnl->canActivate())
		run_tmnl->SetupFocus(m_hwndItem) ;

  return OWND_ACTIVATION_KEPT;	
}
void ALMAPI TMNL_VI::DataChanged(long details,long modifiers,long variation)
{
	TERMINAL  terminal(oiTmnl) ;
	int       Emulation ;                                                  

	I_UNUSED (variation) ;

	Emulation = terminal.GetEmulationType() ;
	if (run_tmnl->ChangingTerminal (details, modifiers))
		{
		DestroyTerminal () ;
		delete run_tmnl ;
		Emulation = terminal.GetEmulationType() ;
		if (Emulation==0) run_tmnl = new TTYTerm (oiTmnl) ;
		if (Emulation==1)	run_tmnl = new AnsiTerm (oiTmnl) ;
		if (Emulation==2) run_tmnl = new VT52Term (oiTmnl) ;
		if (run_tmnl)
			CreateTerminal() ;
		AObjSetRuntimeInfo(oiTmnl, (long)run_tmnl) ;
		}
}
/*void ALMAPI TMNL_VI::Copy(void)
{
	run_tmnl->Copy() ;
}*/
void ALMAPI TMNL_VI::Paste(void)
{
	run_tmnl->Paste() ;	
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void ALMAPI TMNL_VI::Destroy (void)
{
	delete this ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void TMNL_VI::SetupClass (void)
{
	WNDCLASS   tmlclass ;

	tmlclass.style         = CS_DBLCLKS;
	tmlclass.lpfnWndProc   = (WNDPROC)TmlProc ;
	tmlclass.cbClsExtra    = 0 ;
	tmlclass.cbWndExtra    = 0 ;
	tmlclass.hInstance     = hInstance ;
	tmlclass.hIcon         = NULL ;
	tmlclass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	tmlclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	tmlclass.lpszMenuName  = NULL ;
	tmlclass.lpszClassName = "terminal" ;
	RegisterClass (&tmlclass) ;

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL TMNL_VI::CreateTerminal (void)
{
	HWND        hTmnl ;
	DWORD       style = WS_CHILD|WS_VISIBLE|WS_TABSTOP ;
  BOOL        rVal=FALSE ;
  TERMINAL terminal(oiTmnl) ;

	if (terminal.GetScrollBar()&VSCROLL) style |=WS_VSCROLL ;
	if (terminal.GetScrollBar()&HSCROLL) style |=WS_HSCROLL ;
	hTmnl = OWndCreateWindowEx(
  				this,
					0,
					"terminal",
					NULL,
					style,
					hInstance,
					OWndIsLayoutTime()?NULL:run_tmnl) ;

	if (hTmnl!=NULL)
  	{
		ShowWindow (hTmnl, TRUE) ;
		UpdateWindow (hTmnl) ;
   	m_hwndItem = hTmnl ;
   	rVal = TRUE ;
		}
	if (run_tmnl)
		run_tmnl->hwnd = hTmnl ;
	return (rVal) ;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL TMNL_VI::DestroyTerminal (void)
{
	return (DestroyWindow (m_hwndItem));
}

RUNTMNL::RUNTMNL (OBJECTID oiObj)
{
	LOGFONT   logFont ;	

	TERMINAL terminal (oiObj) ;
	oiConn     = terminal.GetConnObject() ; 
	oiTerm     = oiObj ;
	bLineWrap  = terminal.GetModeType()&LINEWRAP ; 
 	bEcho      = terminal.GetModeType()&LOCALECHO ; 
	curShape   = terminal.GetCurShape() ; 
  logFont    = terminal.GetLogFont () ;
	hFont      = CreateFontIndirect (&logFont) ; 
	bMaxCol    = vMaxCols = terminal.GetColumns()?132:80 ; 
	bMaxRow    = vMaxRows = terminal.GetBufferSize() ; 
	bMaxSize   = bMaxCol*bMaxRow ;
	hBuffer    = GlobalAlloc (GMEM_MOVEABLE, bMaxSize) ;
	termBuffer = (LPSTR)GlobalLock (hBuffer) ;
	hAttr      = GlobalAlloc (GMEM_MOVEABLE, bMaxSize) ;
	attrBuffer = (LPSTR)GlobalLock (hAttr) ;
	currAttr   = 0x0f ;
	_fmemset (termBuffer, ' ', bMaxSize) ;
	_fmemset (attrBuffer, 0x0f, bMaxSize) ;
	bCurrCol   = bCurrRow = 0 ;
	caretCurrCol = 0 ;
	caretCurrRow = curShape ? 0:1 ;
	firstVisibleCol = firstVisibleRow = 0 ;

	//Set Default Colors
	dwTextColor  = 7 ;
	dwBkGndColor = 0 ;
	hBkBrush     = CreateSolidBrush( DispAttributes[dwBkGndColor]) ;
	//Set Capture file Option
	capture = FALSE ;
	hCaptureFile = NULL ;
}

//
//
//
RUNTMNL::~RUNTMNL ()
{
	GlobalUnlock (hBuffer) ;
	GlobalFree (hBuffer) ;
	GlobalUnlock (hAttr) ;
	GlobalFree (hAttr) ;
	if (capture&&hCaptureFile)
		CloseCaptureFile() ;
}
void RUNTMNL::GetSize (HWND hTmnl, LPARAM lParam) //RunTerminal::GetSize (HWND hwnd, LPARAM lParam)
{
	HDC       	hdc ;
	TEXTMETRIC	tm ;
  HFONT       hTempFont ;

	hdc = GetDC (hTmnl) ;
	if ((hTempFont= (HFONT)SelectObject (hdc, hFont))==NULL)
		hTempFont= (HFONT)SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ; //should be select a font style

	GetTextMetrics (hdc, &tm) ;
	charWidth  = tm.tmAveCharWidth ;
	charHeight = tm.tmHeight + tm.tmExternalLeading ;

  DeleteObject(hTempFont) ; 
	ReleaseDC (hTmnl, hdc) ;

	cxClient = LOWORD (lParam) ;
	cyClient = HIWORD (lParam) ;
	vMaxCols = max (1, min(vMaxCols, cxClient/charWidth)) ;
	vMaxRows = max (1, min(vMaxRows, cyClient/charHeight)) ;
	nCurrCol = 0 ;
	nCurrRow = 0 ;

	if (hTmnl ==GetFocus ())
		SetCaretPos (caretCurrCol*charWidth, caretCurrRow*charHeight) ;
}
void RUNTMNL::SetupTmnlCursor (HWND hTmnl)
{
	if (curShape)
		CreateCaret (hTmnl, NULL, charWidth, charHeight) ;
	else
		CreateCaret (hTmnl, NULL, charWidth, NULL) ;

	SetCaretPos (caretCurrCol*charWidth, caretCurrRow*charHeight) ;
	ShowCaret (hTmnl) ;
}
void RUNTMNL::DestroyTmnlCursor(HWND hTmnl)
{
	HideCaret (hTmnl) ;
	DestroyCaret () ;
}
void RUNTMNL::SetScrollBar (HWND hTmnl)
{
	TERMINAL terminal (oiTerm) ;
  BYTE     scrollBar = terminal.GetScrollBar() ;
	nVscrollPos = 0 ;
	nHscrollPos = 0 ;
  if (scrollBar&VSCROLL)
		{
		nVscrollMax = max(vMaxRows, bMaxRow-vMaxRows+2) ;
		SetScrollRange (hTmnl, SB_VERT, 0, nVscrollMax, FALSE) ;
		SetScrollPos   (hTmnl, SB_VERT, nVscrollPos, TRUE) ;
		}
  /*if(scrollBar&HSCROLL)
    {
		nHscrollMax = max (0, bMaxCol-vMaxCols) ;
		SetScrollRange (hwnd, SB_HORZ, 0, nHscrollMax, FALSE) ;
		SetScrollPos   (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
    }*/
}
void RUNTMNL::RepaintTerminal (void)
{
	PAINTSTRUCT ps ;
	HDC         hdc ;
  int         y ; 
	int         start_row ;
	int         start_col, end_col ;
	int         attribute ;
	DWORD       fg, bg ;
  HFONT       hTempFont ;
		  
	hdc = BeginPaint (hwnd, &ps) ;
	if ((hTempFont= (HFONT)SelectObject (hdc, hFont))==NULL)
		hTempFont =	(HFONT)SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ; //should be select a font style
	start_row = min (max (0, -firstVisibleRow), vMaxRows) ;
	for (y=start_row; y<vMaxRows; y++)
		{
		start_col  = nHscrollPos ;
		end_col = start_col ;
		while ( start_col <= vMaxCols)
			{
      attribute = ATTRBUF((start_col+firstVisibleCol),(y+firstVisibleRow));
      fg = attribute & 0x0f;
			bg = (attribute >> 4) & 0x0f;
      SetTextColor( hdc, (unsigned long)DispAttributes[(int)fg] );
			SetBkColor( hdc, (unsigned long) DispAttributes[(int)bg] );
			while (ATTRBUF((end_col+firstVisibleCol),(y+firstVisibleRow))==attribute && end_col<=vMaxCols )
				{
     		end_col++ ;
				}
			TextOut( hdc, start_col*charWidth, y*charHeight,
						 &BUFFER((start_col+firstVisibleCol), (y+firstVisibleRow)), end_col - start_col );
			start_col = end_col;
			}
		}
	DeleteObject (hTempFont) ;
	EndPaint (hwnd, &ps) ;
}
void RUNTMNL::DoVscroll (WPARAM wParam, LPARAM lParam)
{
	int nVscrollInc ;

	switch (wParam)
 		{
		case SB_TOP:
    		nVscrollInc = -nVscrollPos ;
        break ;

    case SB_BOTTOM:
        nVscrollInc = nVscrollMax - nVscrollPos ;
        break ;

    case SB_LINEUP:
				nVscrollInc = -1 ;
        break ;

    case SB_LINEDOWN:
				nVscrollInc = 1 ;
        break ;

    case SB_PAGEUP:
				nVscrollInc = min (-1, -vMaxRows ) ; 
        break ;

    case SB_PAGEDOWN:
				nVscrollInc = max (1, vMaxRows ) ; 
        break ;

    case SB_THUMBTRACK:
        nVscrollInc = LOWORD (lParam) - nVscrollPos ;
        break ;

    default:
        nVscrollInc = 0 ;
		}
	nVscrollInc = max (-nVscrollPos,
								min (nVscrollInc, nVscrollMax - nVscrollPos)) ;
	if (nVscrollInc)
  	{
		nVscrollPos += nVscrollInc ;
		nCurrRow -=nVscrollInc ;
		ScrollWindow (hwnd, 0, -charHeight * nVscrollInc, NULL, NULL) ;
    SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
		firstVisibleRow +=nVscrollInc ;
		InvalidateRect (hwnd, NULL, TRUE) ;
		}
}
void RUNTMNL::DoHscroll (WPARAM wParam, LPARAM lParam)
{
	int nHscrollInc;

	switch (wParam)
		{
  	case SB_LINEUP:
        nHscrollInc = -1 ;
        break ;

    case SB_LINEDOWN:
        nHscrollInc = 1 ;
        break ;

    case SB_PAGEUP:
        nHscrollInc = -8 ;
        break ;

    case SB_PAGEDOWN:
        nHscrollInc = 8 ;
        break ;

    case SB_THUMBPOSITION:
        nHscrollInc = LOWORD (lParam) - nHscrollPos ;
        break ;

    default:
				nHscrollInc = 0 ;
		}
  if (nHscrollInc == max (-nHscrollPos,
                    min (nHscrollInc, nHscrollMax - nHscrollPos)))
  	{
		nHscrollPos += nHscrollInc ;
		firstVisibleCol +=nHscrollInc ;
		nCurrCol +=nHscrollInc ;
		ScrollWindow (hwnd, -charWidth * nHscrollInc, 0, NULL, NULL) ;
		SetScrollPos (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
		}
}
void RUNTMNL::Paste(void)
{
	HGLOBAL hGMem ;
	LPSTR   lpGMem ;
	WORD    len ;
  WORD    i ;

	OpenClipboard (hwnd) ;
	hGMem = GetClipboardData(CF_TEXT) ;
	if (hGMem)
		{
		lpGMem = (LPSTR)GlobalLock (hGMem) ;
		len = strlen (lpGMem) ;
		for(i=0; i<len; i++)
			PutChar(lpGMem[i]) ;
		GlobalUnlock (hGMem) ;
		}
  CloseClipboard () ;
}
BOOL RUNTMNL::CheckPort (void)
{
	BOOL rVal =FALSE ;
	 
	rVal = CommIsPortOpen(oiConn) ;
  return (rVal) ;
}
void RUNTMNL::ReturnKey (void)
{
	TERMINAL terminal(oiTerm) ;
	BYTE     CRorLF = terminal.GetCRorLF() ;

	PutChar (CR) ;
  if (CRorLF&OUTBOUND)
				PutChar (LF) ;
}
void RUNTMNL::TabKey (void)
{
	int i ;

	for (i=0; i<8; i++)
			PutChar (' ') ;
}
void far RUNTMNL::PutChar(char c) //maybe return BOOL value!!!
{
	if (CheckPort())
		{
    CommPutChar (oiConn, c) ;
		}
}
void RUNTMNL::PutString(char* str)
{
if (CheckPort())  
  {
	CommPutString (oiConn, (BYTE*)str) ;
  }
}

void RUNTMNL::SetupFocus (HWND hTmnl)
{
		if (hTmnl!=GetFocus())
    		SetFocus (hTmnl) ;
}
int far RUNTMNL::RedrawTerminal (void)
{
	if (CheckPort())
		{
		return (strlen (CommGetMem(oiConn))) ;
		}
    return (-1) ;
}
//
//
//
BOOL RUNTMNL::ChangingTerminal (long details, long modifiers)
{
	BOOL redraw = TRUE ;

	switch (details)
		{
		case changeConfigure:
			return TRUE ;

		case clearTerminal:
   		redraw = ClearTerminal () ;
			break ;

		case resetTerminal:
    	redraw = ClearTerminal () ;
			break ;

		case startCapture:
      redraw = OpenCaptureFile((HFILE)modifiers) ;
			break ;

		case endCapture:
			redraw = CloseCaptureFile() ;
		  break;

		case receiveText:
			redraw = ReceiveText ((OBJECTID)modifiers) ;
			break ;
		}

		if (redraw )
			RedrawTerminal () ;
		return FALSE ;
}
BOOL RUNTMNL::ClearTerminal (void)
{
	_fmemset (termBuffer, ' ', bMaxSize) ;
	_fmemset (attrBuffer, 0x0f, bMaxSize) ;

	InvalidateRect (hwnd, NULL, FALSE) ;
	UpdateWindow (hwnd) ;

	nCurrCol = nCurrRow = 0 ;
	MoveTo (nCurrCol, nCurrRow) ;
	nVscrollPos = nHscrollPos = 0 ;
	SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
	SetScrollPos (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
	firstVisibleRow = firstVisibleRow = 0 ;

	return (FALSE);
}
BOOL RUNTMNL::ResetTerminal (void)
{
	return(FALSE) ;
}
BOOL RUNTMNL::OpenCaptureFile(HFILE hFile)
{
	hCaptureFile = hFile ;  	
  capture = TRUE   ;  
	return (FALSE) ;
}
BOOL RUNTMNL::CloseCaptureFile(void)
{
  _lclose(hCaptureFile) ;
  hCaptureFile = NULL ;
	capture = FALSE ;
	return (FALSE);
}

BOOL RUNTMNL::ReceiveText (OBJECTID oiText)
{
	WORD  length ;
	char* lpText ;
  char* p ;

	length = OTextGetLength (oiText) ;
	p=lpText = new char[length] ;
	OTextGetToBuffer (oiText, (LPSTR)lpText, length+1) ;

	WORD i ;
	for (i=0; i<length; i++, p++)
		{
		DisplayChar (*p) ;
		}

	InvalidateRect (hwnd, NULL, FALSE) ;
	UpdateWindow (hwnd) ;
	delete[] lpText ; 
	return(FALSE);
	
}
///////////////////////////////////////////////////////////////////////////
//SrcollBufferChars: Check whether the buffer is overflow or not.
//
///////////////////////////////////////////////////////////////////////////
BOOL RUNTMNL::ScrollBufferChars (void)
{
	LPSTR lpScrollText ;
	int rVal ;

	if (bCurrRow>=bMaxRow-1)
		{ 
		lpScrollText = (LPSTR)new char [bMaxCol+1] ;
		_fmemcpy(lpScrollText, &BUFFER(0,0), bMaxCol) ;
		lpScrollText[bMaxCol] = '\0' ;
		_fmemmove(&BUFFER(0,0), &BUFFER(0,1), (bMaxRow-1)*bMaxCol) ; //bCurrRow*bMaxCol) ;
		_fmemset(&BUFFER(0, (bMaxRow-1)), ' ', bMaxCol) ;
		WriteToCaptureText (lpScrollText) ;
		if (capture)
			WriteToCaptureFile(lpScrollText) ;            
		delete[] lpScrollText ;
		rVal = TRUE ;
		}
	else
		rVal = FALSE ;

	return rVal ;
}
void RUNTMNL::ScrollScreenChars (void)
{
	nCurrRow = vMaxRows - 1;
	nVscrollPos += max (-nVscrollPos, min(1, nVscrollMax - nVscrollPos)) ;
	SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
	if (!ScrollBufferChars() )
		{
		ScrollWindow( hwnd, 0, -charHeight, NULL, NULL );
		firstVisibleRow++ ;
    }
	ValidateRect( hwnd, NULL );
	UpdateWindow( hwnd );
}
void RUNTMNL::WriteToCaptureText (LPSTR lpScrollText)
{
	TERMINAL terminal(oiTerm) ;
	BOOL     cacheText = terminal.GetCacheText() ;
  OBJECTID textObj = terminal.GetCacheTextObj() ;

	if (cacheText&&textObj)
		{
		OTextSetValueFromBuffer (textObj, lpScrollText) ;
    }
}
void RUNTMNL::WriteToCaptureFile (LPSTR lpScrollText)
{
	int len ;
	char* str ;
	if (hCaptureFile)
		{
		if (lpScrollText)
			{
			len = strlen(lpScrollText) ;
			str = new char[len+3] ;
			sprintf (str, "%s\x0D\x0A", lpScrollText) ;
			_lwrite(hCaptureFile, str, strlen(str)) ;
			delete[]str ;
			}
    }
}
void RUNTMNL::MoveTo (int x, int y)
{
	caretCurrCol = x ;
  caretCurrRow = y ;
	if (!curShape)
		caretCurrRow++ ;

	SetFocus (hwnd) ;
	SetCaretPos(caretCurrCol*charWidth, caretCurrRow*charHeight );
	ShowCaret( hwnd );
}
void RUNTMNL::SaveCursorPos (int x, int y)
{
		saveCol = x ;
		saveRow = y ;
}
void RUNTMNL::RestoreCursorPos (void)
{
		MoveTo (saveCol, saveRow) ;		
}
void RUNTMNL::EraseScreen (int x, int y)
{

	_fmemset(&BUFFER(0,firstVisibleRow), ' ', vMaxRows*bMaxCol) ;
	InvalidateRect( hwnd, NULL, TRUE );
	SendMessage (hwnd, WM_SETREDRAW, TRUE, 0L) ;
	UpdateWindow (hwnd) ;

	MoveTo (x, y) ;
}
void RUNTMNL::EraseFromCursorToEnd (int x, int y)
{
	int  i ;
  RECT rect ;

	for (i=y*bMaxCol+x-1; i<bMaxSize; i++)
		termBuffer[i]=' ' ;

	rect.left = x*charWidth ;
	rect.top  = y*charHeight ;
  rect.right = cxClient ;
	rect.bottom = rect.top +charHeight ;
	InvalidateRect ( hwnd, &rect, TRUE) ;
	rect.left = 0 ;
	rect.top  = rect.bottom ;
	rect.bottom = cyClient ;
	InvalidateRect ( hwnd, &rect,TRUE) ;
	UpdateWindow (hwnd) ;
}
void RUNTMNL::EraseFromStartToCursor(int x, int y)
{
	int  i ;
  RECT rect ;

	for (i=0; i<y*bMaxCol+x; i++)
		termBuffer[i] =' ' ;

	rect.left = 0 ;
	rect.top  = y*charHeight ;
	rect.right = (x+1)*charWidth ;
	rect.bottom = (y+1)*charHeight ;
	InvalidateRect (hwnd, &rect, TRUE) ;
	rect.bottom = rect.top ;
	rect.top = 0 ;
	rect.right = cxClient ;
	InvalidateRect (hwnd, &rect, TRUE) ;
  UpdateWindow (hwnd) ;
}
//
//
//
void RUNTMNL::ClearToEndOfLine(int x, int y)
{
	RECT rect ;
	int  i;

	for (i=x; i<bMaxCol-1; i++)
		BUFFER(i,y) = ' ' ;

	rect.left  = x*charWidth ;
	rect.top   = y*charHeight ;
  rect.right = cxClient ;
	rect.bottom= rect.top +charHeight ;
	InvalidateRect (hwnd, &rect, TRUE) ;
	UpdateWindow (hwnd) ;				
}
void RUNTMNL::ClearToStartOfLine (int x, int y)
{
	RECT rect ;
	int  i ;

	for (i=0; i<x; i++)
		BUFFER(i,y) ;
	rect.left = 0 ;
	rect.top  = y*charHeight ;
	rect.right = (x+1)*charWidth ;
	rect.bottom = (y+1)*charHeight ;
	InvalidateRect (hwnd, &rect, TRUE) ;
	UpdateWindow (hwnd) ;
}
void RUNTMNL::ClearLine (int x, int y)
{
	RECT rect ;
	int i ;

	I_UNUSED (x) ;
	for ( i = 0; i < bMaxCol - 1; i++ )
		BUFFER(i,y) = ' ';
	rect.left = 0 ;
	rect.top  = y*charHeight ;
	rect.right = cxClient ;
	rect.bottom = (y+1)*charHeight ;
	InvalidateRect (hwnd, &rect, TRUE) ;
	UpdateWindow (hwnd) ;
}
void RUNTMNL::InsertLine (int x, int y)
{
	I_UNUSED (x) ;
	_fmemmove(&BUFFER(0,(y+firstVisibleRow+1)), &BUFFER(0,(y+firstVisibleRow)), (bMaxRow-y-firstVisibleRow-1)*bMaxCol) ;
	_fmemset (&BUFFER(0,(y+firstVisibleRow)), ' ', bMaxCol) ;
	InvalidateRect (hwnd, NULL, FALSE) ;
	UpdateWindow (hwnd) ;		
}
void RUNTMNL::DeleteLine (int x, int y)
{
	I_UNUSED (x) ;
	_fmemmove(&BUFFER(0,(y+firstVisibleRow)), &BUFFER(0, (y+firstVisibleRow+1)), (bMaxRow-y-firstVisibleRow-1)*bMaxCol) ;
	_fmemset (&BUFFER(0,(bMaxRow-1)), ' ', bMaxCol) ;
	InvalidateRect (hwnd, NULL, FALSE) ;
	UpdateWindow (hwnd) ; 
}
void RUNTMNL::DeleteChar (int x, int y)
{
	I_UNUSED (x) ;
	_fmemmove ((char *)termBuffer[(y+firstVisibleRow)*bMaxCol+x], (char *)termBuffer[(y+firstVisibleRow)*bMaxCol+x+1], ((bMaxRow-(y+firstVisibleRow+1))*bMaxCol+(bMaxCol-x-1))) ;
	termBuffer[bMaxRow*bMaxCol-1]=' ' ;
	InvalidateRect (hwnd, NULL, FALSE) ;
	UpdateWindow(hwnd) ;
}
void RUNTMNL::ScrollUp (void)
{
	ScrollWindow( hwnd, 0, -charHeight, NULL, NULL );
	ValidateRect( hwnd, NULL );
	UpdateWindow( hwnd );
}
void RUNTMNL::ScrollDown (void)
{
	ScrollWindow( hwnd, 0, charHeight, NULL, NULL );
	ValidateRect( hwnd, NULL );
	UpdateWindow( hwnd );
}
//
//
//
void RUNTMNL::SetCurrAttr (WORD fg, WORD bg)
{
	currAttr = (bg<<4)|fg&0xff ;
}
//
//
//
void RUNTMNL::DisplayChar( char cChar )
{
	 switch ( cChar )
	 	{
    case VK_RETURN:  // process a carriage return
      nCurrCol = 0;
      break;

    case 0xA:        // process a new line
			nCurrRow++;
			bCurrRow = nCurrRow+firstVisibleRow ;
			if ( nCurrRow == vMaxRows )
			 {
       ScrollScreenChars() ;	
			 }
      break;

    case VK_BACK:   // process a backspace
			if ( nCurrCol )
      	{
				nCurrCol--;
        }
		 	break;

    default:
			bCurrCol = nCurrCol+firstVisibleCol ;
			bCurrRow = nCurrRow+firstVisibleRow ;
			BUFFER(bCurrCol,bCurrRow) = cChar ;
			ATTRBUF (bCurrCol,bCurrRow) = currAttr ;
			nCurrCol++;
			if ( nCurrCol == vMaxCols )
			 	{
			 	if ( bLineWrap )
			 		{
       		nCurrCol = 0;
					nCurrRow++;
			 		if ( nCurrRow == vMaxRows )
						{
            ScrollScreenChars() ;
						}
					}
				else
					{
					nCurrCol--;
          }
       	}
			break;
		}
	//
  // update the caret position and show it
	//
	if (nCurrCol<vMaxCols&&nCurrRow<vMaxRows)
		{
		MoveTo(nCurrCol, nCurrRow) ;
	 	}
}
//
//
//
long ALMCALLBACK TmlProc (
	HWND hwnd,
	WORD message,
	WORD wParam,
	LONG lParam)
{
	RUNTMNL      far *run_tmnl ;

	if (message!=WM_CREATE)
		{
		run_tmnl = (RUNTMNL far *) MAKELONG (GetProp(hwnd, "LoWord"), GetProp(hwnd, "HiWord")) ;
		if (run_tmnl==NULL)
			return DefWindowProc (hwnd, message, wParam, lParam)  ;
		}
	switch (message)
		{
		case WM_CREATE:
      {
			CREATESTRUCT far * lpcs = (CREATESTRUCT far *) lParam ;
			run_tmnl = (RUNTMNL far *) lpcs->lpCreateParams ;
			SetProp (hwnd, "LoWord", (HANDLE)LOWORD((DWORD)run_tmnl)) ;
			SetProp (hwnd, "HiWord", (HANDLE)HIWORD((DWORD)run_tmnl)) ;
			return 0 ;
      }

		case WM_SIZE:
			run_tmnl->GetSize (hwnd, lParam) ;
			run_tmnl->SetScrollBar(hwnd) ;
			return 0 ;

		case WM_VSCROLL:
			run_tmnl->DoVscroll (wParam, lParam) ;
			return 0 ;

		case WM_HSCROLL:
			run_tmnl->DoHscroll (wParam, lParam) ;
			return 0 ;

		case WM_PAINT:
			run_tmnl->RepaintTerminal () ;
			return 0 ;

		case WM_SETFOCUS :
			run_tmnl->SetupTmnlCursor(hwnd) ;
			return 0 ;

		case WM_KILLFOCUS:
			run_tmnl->DestroyTmnlCursor(hwnd) ;
			return 0 ;

		case WM_CHAR:
			run_tmnl->KeyDown(wParam) ;
			return 0 ;
		}
		return DefWindowProc (hwnd, message, wParam, lParam)  ;
}
