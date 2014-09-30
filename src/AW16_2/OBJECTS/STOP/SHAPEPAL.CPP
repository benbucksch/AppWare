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
// FILE:					SHAPEPAL.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Manages the interaction between the user of the shape
//								library and the actual shape library.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define ASTRICT
#define STRICT

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>
#include <attribut.h>

#include "tmp.h"

#include "shp_attr.h"
#include "shp_prot.h"
#include "palette.h"
#include "shapepal.h"

//////////////// Internal defines

#define	COLOR_BLACK			RGB(0, 0, 0)
#define	COLOR_DKGRAY		RGB(128, 128, 128)
#define	COLOR_LTGRAY		RGB(192, 192, 192)
#define	COLOR_WHITE			RGB(255, 255, 255)

//////////////// Internal structures

// ToolInfo describes a shape-library-supplied tool completely.

typedef struct
	{
	short				nID;									// the external #
	short				nGID;									// TI_* group defines below
	short				fFlags;								// the TI_* flags below
	LONG				lCaps;								// passed back during OSHP_QUERYTOOLINFO
	HBITMAP			hImage;								// filled in if TI_HASIMAGE
	HCURSOR			hCursor;							// filled in if TI_HASCURSOR && passed back during OSHP_QUERYTOOLINFO
	}		ToolInfo;

#define	TI_HASIMAGE				0x1
#define TI_HASCURSOR			0x2
#define TI_CMDBUTTON			0x4
#define TI_RADIOBUTTON		0x8

#define TI_NOGROUP				0
#define TI_STDGROUP				1

//////////////// Global variables

TYPEID								g_StdShapeLibType			= MAKETYPEID('S', 'h', 'p', 'S');

//////////////// Static variables

static char						g_szStdShapeLibName[]	= "&Tools";
static short					g_nInitCount					= 0;
static OLayoutAdmin		g_pfnLayoutAdmin;
static LONG						g_lLayoutHandle;

static ToolInfo				g_tiToolInfo[SHP_STDTOOLCOUNT]	=
													{
														{SHP_TOOL_BITMAP, TI_NOGROUP,
															TI_HASIMAGE | TI_CMDBUTTON,
															OSHP_CAP_CREATEIMMEDIATE,
															0, 0},
														{SHP_TOOL_FILLCIRCLE, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_FILLRRECT, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_FILLSQUARE, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_GROUPBOX, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_LINE, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_TEXT, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_NOTIFYONCREATE,
															0, 0},
														{SHP_TOOL_UNFILLCIRCLE, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_UNFILLRRECT, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0},
														{SHP_TOOL_UNFILLSQUARE, TI_STDGROUP,
															TI_HASIMAGE | TI_HASCURSOR | TI_RADIOBUTTON,
															OSHP_CAP_CREATEONDOWN | OSHP_CAP_RESIZEONCREATE,
															0, 0}
													};

//////////////// Class static fields

ShapePalette*		ShapePalette::s_pCurrentPalette		= NULL;

////////////////////////////////////////////////////////////////
//
// FUNCTION:	InitShapeLibrary
//
// DESCR:			Initializes the shape library by loading cursors and bitmaps.
//
////////////////////////////////////////////////////////////////

static void		InitShapeLibrary(
	void
	)
	{
	HINSTANCE		hInst			= GetHInstance();
	int					nIdx;

	if (g_nInitCount == 0)
		{
		for (nIdx = 0; nIdx < SHP_STDTOOLCOUNT; nIdx++)
			{
			if (g_tiToolInfo[nIdx].fFlags & TI_HASCURSOR)
				g_tiToolInfo[nIdx].hCursor = LoadCursor(hInst,
																								ID(g_tiToolInfo[nIdx].nID));

			if (g_tiToolInfo[nIdx].fFlags & TI_HASIMAGE)
				g_tiToolInfo[nIdx].hImage = LoadBitmap(hInst,
																								ID(g_tiToolInfo[nIdx].nID));
			}
		}

	g_nInitCount++;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	CleanShapeLibrary
//
// DESCR:			Uninitializes the shape library.  Note that bitmaps
//						do NOT need to be freed since their ownership was transferred
//						to the palette during the add of the item.
//
////////////////////////////////////////////////////////////////

static void		CleanShapeLibrary(
	void
	)
	{
	int					nIdx;

	g_nInitCount--;

	if (g_nInitCount == 0)
		{
		for (nIdx = 0; nIdx < SHP_STDTOOLCOUNT; nIdx++)
			{
			if (g_tiToolInfo[nIdx].hCursor)
				DestroyCursor(g_tiToolInfo[nIdx].hCursor);
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	LineButton::LineButton
//
// DESCR:		Initializes a new line button object.
//
////////////////////////////////////////////////////////////////

LineButton::LineButton
  (
	PaletteWindow*	pPalette,
	UINT						nToolId
	) :
		PaletteItem(pPalette, nToolId, 0, PAL_STATELESS),
		m_LineAttributes()

	{
  // restore the line attributes from the appbldr.ini file
  char  szBuffer[255];
  if(GetPrivateProfileString("Shape Attributes", "Line", "", szBuffer, 255, "appbldr.ini")>0)
    {
    char* szLast = szBuffer;

    m_LineAttributes.SetFGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_LineAttributes.SetBGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_LineAttributes.SetLineWidth((int)strtoul(szLast, &szLast, 0));
    szLast++;
    m_LineAttributes.SetLineStyle((int)strtoul(szLast, &szLast, 0));
    }
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	LineButton::~LineButton
//
// DESCR:		Cleans up a line button object.
//
////////////////////////////////////////////////////////////////

LineButton::~LineButton
  (
	void
	)

	{
  // save the line attributes in the appbldr.ini file
  ATTR_LINE atrLine;
  char      szBuffer[255];

  m_LineAttributes.PrepForFile(&atrLine);
  wsprintf(szBuffer, "%lu, %lu, %hu, %hu", atrLine.clrFG, atrLine.clrBG, atrLine.nWidth, atrLine.nStyle);
  WritePrivateProfileString("Shape Attributes", "Line", szBuffer, "appbldr.ini");
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	LineButton::DrawItem
//
// DESCR:		Draws the line attribute rectangle.
//
////////////////////////////////////////////////////////////////

void LineButton::DrawItem
  (
  HDC   hDC,
	BOOL	fState
  )

  {
	I_UNUSED(fState);

  HRGN      hrgn, hrgnOld;
  RECT      rcClip;
  HPEN	    hpen, hpenOld;
  HBRUSH    hbr, hbrOld;
  int       nOldBkMode;

	// draw the 3D border....
	// black frame
	hpen = CreatePen(PS_SOLID, 0, COLOR_BLACK);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left, m_rcItem.top);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.top);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // white border
  hpen = CreatePen(PS_SOLID, 0, COLOR_WHITE);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+1, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left+1, m_rcItem.top+1);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.top+1);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left+1, m_rcItem.bottom-1);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // light gray
  hpen = CreatePen(PS_SOLID, 0, COLOR_LTGRAY);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+3, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left+2, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left+2, m_rcItem.top+2);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.top+2);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.top+3);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // the box
  hbr = CreateSolidBrush(m_LineAttributes.BGColor());
  hbrOld = (HBRUSH)SelectObject(hDC, hbr);
  ::Rectangle(hDC, m_rcItem.left+4, m_rcItem.top+4, m_rcItem.right-1, m_rcItem.bottom-1);
  SelectObject(hDC, hbrOld);
  DeleteObject(hbr);

  // create a clipping region for the inside of the button
  GetClipBox(hDC, &rcClip);
  hrgn = CreateRectRgn(m_rcItem.left+5, m_rcItem.top+5, m_rcItem.right-2, m_rcItem.bottom-2);
  SelectClipRgn(hDC, hrgn);
  DeleteObject(hrgn);

  // draw the line based on the LineAttrib
  nOldBkMode = SetBkMode(hDC, TRANSPARENT);
  hpen = m_LineAttributes.CreatePen();
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+5, m_rcItem.top + 4 + ((m_rcItem.bottom - m_rcItem.top - 5) / 2));
  LineTo(hDC, m_rcItem.right-3, m_rcItem.top + 4 + ((m_rcItem.bottom - m_rcItem.top - 5) / 2));
  SelectObject(hDC, hpenOld);
  m_LineAttributes.DeletePen(hpen);
  SetBkMode(hDC, nOldBkMode);

  // restore the clipping region as before
  hrgnOld = CreateRectRgnIndirect(&rcClip);
  SelectClipRgn(hDC, hrgnOld);
  DeleteObject(hrgnOld);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	LineButton::Edit
//
// DESCR:		Edits the line attributes.
//
////////////////////////////////////////////////////////////////

void LineButton::Edit
  (
  short  nCurrentUnits
  )

  {
  m_LineAttributes.EditLine(GetActiveWindow(), nCurrentUnits);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	FillButton::FillButton
//
// DESCR:		Initializes a new fill button object.
//
////////////////////////////////////////////////////////////////

FillButton::FillButton
  (
	PaletteWindow*	pPalette,
	UINT						nToolId
	) :
		PaletteItem(pPalette, nToolId, 0, PAL_STATELESS),
		m_FillAttributes()

	{
  // restore the fill attributes from the appbldr.ini file
  char  szBuffer[255];
  if(GetPrivateProfileString("Shape Attributes", "Fill", "", szBuffer, 255, "appbldr.ini")>0)
    {
    char* szLast = szBuffer;

    m_FillAttributes.SetFGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_FillAttributes.SetBGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_FillAttributes.SetFillPattern((int)strtoul(szLast, &szLast, 0));
    }
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FillButton::~FillButton
//
// DESCR:		Cleans up a fill button object.
//
////////////////////////////////////////////////////////////////

FillButton::~FillButton
  (
	void
	)

	{
  // save the fill attributes in the appbldr.ini file
  ATTR_FILL atrFill;
  char      szBuffer[255];

  m_FillAttributes.PrepForFile(&atrFill);
  wsprintf(szBuffer, "%lu, %lu, %hu", atrFill.clrFG, atrFill.clrBG, atrFill.nPattern);
  WritePrivateProfileString("Shape Attributes", "Fill", szBuffer, "appbldr.ini");
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FillButton::DrawItem
//
// DESCR:		Draws the fill attribute rectangle.
//
////////////////////////////////////////////////////////////////

void FillButton::DrawItem
  (
  HDC   hDC,
	BOOL	fState
  )

  {
	I_UNUSED(fState);

  HPEN	    hpen, hpenOld;
  HBRUSH    hbr, hbrOld;
  COLORREF  clrOldFG, clrOldBG;

  // draw the 3D border
  // black frame
  hpen = CreatePen(PS_SOLID, 0, COLOR_BLACK);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left, m_rcItem.top);
  LineTo(hDC, m_rcItem.right-1, m_rcItem.top);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // white border
  hpen = CreatePen(PS_SOLID, 0, COLOR_WHITE);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left, m_rcItem.top+1);
  LineTo(hDC, m_rcItem.right-2, m_rcItem.top+1);
  MoveTo(hDC, m_rcItem.left+2, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.top+4);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // light gray
  hpen = CreatePen(PS_SOLID, 0, COLOR_LTGRAY);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+1, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left, m_rcItem.top+2);
  LineTo(hDC, m_rcItem.right-2, m_rcItem.top+2);
  LineTo(hDC, m_rcItem.right-2, m_rcItem.bottom-1);
  MoveTo(hDC, m_rcItem.right-1, m_rcItem.top+1);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.top+3);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // draw the fill pattern
  clrOldBG = SetBkColor(hDC, m_FillAttributes.IsBGTransparent() ? GetSysColor(COLOR_WINDOW) : m_FillAttributes.BGColor());
  clrOldFG = SetTextColor(hDC, m_FillAttributes.FGColor());
  hbr = m_FillAttributes.CreateBrush();
  hbrOld = (HBRUSH)SelectObject(hDC, hbr);
  ::Rectangle(hDC, m_rcItem.left+2, m_rcItem.top+4, m_rcItem.right-3, m_rcItem.bottom-1);
  SetBkColor(hDC, clrOldBG);
  SetTextColor(hDC, clrOldFG);
  SelectObject(hDC, hbrOld);
  m_FillAttributes.DeleteBrush(hbr);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	FillButton::Edit
//
// DESCR:		Edits the fill attributes.
//
////////////////////////////////////////////////////////////////

void FillButton::Edit
  (
  )

  {
  m_FillAttributes.EditFill(GetActiveWindow());
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	FontButton::FontButton
//
// DESCR:		Initializes a new font button object.
//
////////////////////////////////////////////////////////////////

FontButton::FontButton
  (
	PaletteWindow*	pPalette,
	UINT						nToolId
	) :
		PaletteItem(pPalette, nToolId, 0, PAL_STATELESS),
		m_FontAttributes(NULL, COLOR_WINDOWFOREGROUND, COLOR_WINDOWBACKGROUND, 200)

	{
  // restore the font attributes from the appbldr.ini file
  char  szBuffer[255];
  if(GetPrivateProfileString("Shape Attributes", "Font", "", szBuffer, 255, "appbldr.ini")>0)
    {
    char* szLast = szBuffer;

    m_FontAttributes.SetFGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_FontAttributes.SetBGColor((COLORREF)strtoul(szLast, &szLast, 0));
    szLast++;
    m_FontAttributes.SetFontHeight((int)strtoul(szLast, &szLast, 0));
    }
  if(GetPrivateProfileString("Shape Attributes", "Logfont", "", szBuffer, 255, "appbldr.ini")>0)
    {
    char*   szLast = szBuffer;
    LOGFONT lf;

    lf.lfHeight = (int)strtoul(szLast, &szLast, 0);           szLast++;
    lf.lfWidth = (int)strtoul(szLast, &szLast, 0);            szLast++;
    lf.lfEscapement = (int)strtoul(szLast, &szLast, 0);       szLast++;
    lf.lfOrientation = (int)strtoul(szLast, &szLast, 0);      szLast++;
    lf.lfWeight = (int)strtoul(szLast, &szLast, 0);           szLast++;
    lf.lfItalic = (BYTE)strtoul(szLast, &szLast, 0);          szLast++;
    lf.lfUnderline = (BYTE)strtoul(szLast, &szLast, 0);       szLast++;
    lf.lfStrikeOut = (BYTE)strtoul(szLast, &szLast, 0);       szLast++;
    lf.lfCharSet = (BYTE)strtoul(szLast, &szLast, 0);         szLast++;
    lf.lfOutPrecision = (BYTE)strtoul(szLast, &szLast, 0);    szLast++;
    lf.lfClipPrecision = (BYTE)strtoul(szLast, &szLast, 0);   szLast++;
    lf.lfQuality = (BYTE)strtoul(szLast, &szLast, 0);         szLast++;
    lf.lfPitchAndFamily = (BYTE)strtoul(szLast, &szLast, 0);  szLast+=2;
    lstrcpy(lf.lfFaceName, szLast);

    m_FontAttributes.SetLogfont(&lf);
    }
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FontButton::~FontButton
//
// DESCR:		Cleans up a font button object.
//
////////////////////////////////////////////////////////////////

FontButton::~FontButton
  (
	void
	)

	{
  // save the font attributes in the appbldr.ini file
  ATTR_FONT atrFont;
  char      szBuffer[255];

  m_FontAttributes.PrepForFile(&atrFont);
  wsprintf(szBuffer, "%lu, %lu, %hu", atrFont.clrFG, atrFont.clrBG, atrFont.nTwips);
  WritePrivateProfileString("Shape Attributes", "Font", szBuffer, "appbldr.ini");
  wsprintf(szBuffer, "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %s",
      atrFont.lfFont.lfHeight, atrFont.lfFont.lfWidth, atrFont.lfFont.lfEscapement,
      atrFont.lfFont.lfOrientation, atrFont.lfFont.lfWeight, (int)atrFont.lfFont.lfItalic,
      (int)atrFont.lfFont.lfUnderline, (int)atrFont.lfFont.lfStrikeOut,
      (int)atrFont.lfFont.lfCharSet, (int)atrFont.lfFont.lfOutPrecision,
      (int)atrFont.lfFont.lfClipPrecision, (int)atrFont.lfFont.lfQuality,
      (int)atrFont.lfFont.lfPitchAndFamily, atrFont.lfFont.lfFaceName);
  WritePrivateProfileString("Shape Attributes", "Logfont", szBuffer, "appbldr.ini");
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FontButton::DrawItem
//
// DESCR:		Draws the font attribute rectangle.
//
////////////////////////////////////////////////////////////////

void FontButton::DrawItem
  (
  HDC   hDC,
	BOOL	fState
  )

  {
	I_UNUSED(fState);

  HPEN	hpen, hpenOld;

  // draw the 3D border
  // black frame
  hpen = CreatePen(PS_SOLID, 0, COLOR_BLACK);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left, m_rcItem.top);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // white
  hpen = CreatePen(PS_SOLID, 0, COLOR_WHITE);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+1, m_rcItem.bottom-2);
  LineTo(hDC, m_rcItem.left+1, m_rcItem.top);
  MoveTo(hDC, m_rcItem.left+4, m_rcItem.bottom-3);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.bottom-3);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.top+2);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // light gray
  hpen = CreatePen(PS_SOLID, 0, COLOR_LTGRAY);
  hpenOld = (HPEN)SelectObject(hDC, hpen);
  MoveTo(hDC, m_rcItem.left+1, m_rcItem.bottom-1);
  LineTo(hDC, m_rcItem.left+3, m_rcItem.bottom-3);
  MoveTo(hDC, m_rcItem.right-2, m_rcItem.top);
  LineTo(hDC, m_rcItem.right-3, m_rcItem.top+1);
  MoveTo(hDC, m_rcItem.left+2, m_rcItem.bottom-2);
  LineTo(hDC, m_rcItem.left+2, m_rcItem.top);
  LineTo(hDC, m_rcItem.right-2, m_rcItem.top);
  LineTo(hDC, m_rcItem.right-2, m_rcItem.bottom-2);
  LineTo(hDC, m_rcItem.left+2, m_rcItem.bottom-2);
  SelectObject(hDC, hpenOld);
  DeleteObject(hpen);

  // Draw the font
  DrawFont(hDC);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	AttributeButton::DrawFont
//
// DESCR:		Draws the font attribute window.
//
////////////////////////////////////////////////////////////////

void FontButton::DrawFont
  (
	HDC			hDC
	)

	{
	LOGFONT			lf;
	HFONT				hfont, hfontOld;
	HBRUSH			hbr, hbrOld;
	int					nPointSize, nTwips;

  // draw the background
 	hbr = CreateSolidBrush(m_FontAttributes.BGColor());
	if (hbr)
		hbrOld = (HBRUSH) SelectObject(hDC, hbr);
	::Rectangle(hDC, m_rcItem.left+4, m_rcItem.top+2, m_rcItem.right-3, m_rcItem.bottom-3);
	if (hbr)
		{
		SelectObject(hDC, hbrOld);
		DeleteObject(hbr);
		}

  // create the font
	m_FontAttributes.GetLogfont(&lf);
  nTwips = m_FontAttributes.FontHeight();
	nPointSize  = (nTwips + 10) / 20;

	// Make cell height 7/8 of window height, but don't sacrifice quality
	lf.lfHeight = (int) MulDiv(7, (m_rcItem.bottom - m_rcItem.top-6), 8);
  lf.lfQuality = PROOF_QUALITY;

	hfont = CreateFontIndirect(&lf);

	if (hfont)
		{
    RECT      rc;
  	COLORREF  crOldFG;
  	int       nOldBGMode;
  	char      szSample[16];

		hfontOld = (HFONT) SelectObject(hDC, hfont);
		wsprintf(szSample, "AbyZ %d", nPointSize);
		crOldFG = SetTextColor(hDC, m_FontAttributes.FGColor());
		nOldBGMode = SetBkMode(hDC, TRANSPARENT);

		SetRect(&rc, m_rcItem.left+5, m_rcItem.top+3, m_rcItem.right-4, m_rcItem.bottom-4);
		DrawText(hDC, szSample, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		SetTextColor(hDC, crOldFG);
    SetBkMode(hDC, nOldBGMode);
		SelectObject(hDC, hfontOld);
		DeleteObject(hfont);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	FontButton::Edit
//
// DESCR:		Edits the font attributes.
//
////////////////////////////////////////////////////////////////

void FontButton::Edit
  (
  )

  {
  m_FontAttributes.EditFont(GetActiveWindow(),
                            "Edit Font",
                            "AaBbYyZz123",
                            FALSE,
                            TRUE,
                            FALSE);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::ShapePalette
//
// DESCR:		Initializes a new shape palette
//
////////////////////////////////////////////////////////////////

ShapePalette::ShapePalette(
	HWND		hOwner
	) :
		PaletteWindow(PaletteWindow::popupPalette | PaletteWindow::titleBar,
									hOwner),
		m_nCurrentTool(-1)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::~ShapePalette
//
// DESCR:		Destroys a shape palette.
//
////////////////////////////////////////////////////////////////

ShapePalette::~ShapePalette(
	void
	)
	{
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::Closing
//
// DESCR:		Handles the close notification from the palette by hiding
//					the palette.
//
////////////////////////////////////////////////////////////////

void	ShapePalette::Closing(
	void
	)
	{
	ShowWindow(Hwnd(), SW_HIDE);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::ItemChosen
//
// DESCR:		Handles the item chosen notification from the palette.
//
////////////////////////////////////////////////////////////////

void	ShapePalette::ItemChosen(
	PaletteItem*		pItem
	)
	{
	short				nID		= pItem->ItemID();

	if (nID == SHP_TOOL_BITMAP)
;//		CreateBitmap();

	else if (nID < SHP_TOOL_BASE)
		SetActiveTool(0, nID);

	else if (nID <= SHP_TOOL_LASTSTDTOOL)
		SetActiveTool(g_StdShapeLibType, nID);

	else if (nID <= SHP_TOOL_LASTTOOL)
		{
    EnablePalettes(FALSE);
    if(nID == SHP_TOOL_LINEATTRIBS)
  		((LineButton*) pItem)->Edit(GetCurrentUnits());
    else if(nID == SHP_TOOL_FILLATTRIBS)
  		((FillButton*) pItem)->Edit();
    else if(nID == SHP_TOOL_FONTATTRIBS)
  		((FontButton*) pItem)->Edit();
    EnablePalettes(TRUE);

		InvalidateRect(Hwnd(), pItem->Rectangle(), TRUE);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::SetActiveTool
//
// DESCR:		Sets the active tool to the given one by sending a message
//					to the layout editor.  The actual change of the tool is
//					not done until the layout editor informs us of a new tool
//					being active.
//
////////////////////////////////////////////////////////////////

void		ShapePalette::SetActiveTool(
	TYPEID				tLibType,
	short					nToolID
	)
	{
	AObjMessage		aoMsg;

	I_ASSERT(g_pfnLayoutAdmin != NULL);

	aoMsg.message1 = g_lLayoutHandle;
	aoMsg.message2 = (LONG) tLibType;
	aoMsg.message3 = (LONG) nToolID;
	aoMsg.message4 = 0;

	(*g_pfnLayoutAdmin)(OSHP_MSG_SETACTIVETOOL, &aoMsg);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::GetCurrentUnits
//
// DESCR:		This retrieves the current units from the layout.
//
////////////////////////////////////////////////////////////////

short ShapePalette::GetCurrentUnits
  (
  void
  )
  
  {
  short         nRet;
	AObjMessage		aoMsg;

	I_ASSERT(g_pfnLayoutAdmin != NULL);

	aoMsg.message1 = g_lLayoutHandle;
	aoMsg.message2 = 0;
	aoMsg.message3 = 0;
	aoMsg.message4 = 0;

	if((*g_pfnLayoutAdmin)(OSHP_MSG_GETCURRENTUNITS, &aoMsg) == A_OK)
    nRet = (short)aoMsg.message2;
  else
    nRet = OSHP_UNITS_ENGLISH_IN;

  return nRet;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::EnablePalettes
//
// DESCR:		This will cause the layout to enable (or disable)
//          all of the palettes.
//
////////////////////////////////////////////////////////////////

void ShapePalette::EnablePalettes
  (
  BOOL  fEnable
  )

  {
	AObjMessage		aoMsg;

	I_ASSERT(g_pfnLayoutAdmin != NULL);

	aoMsg.message1 = g_lLayoutHandle;
	aoMsg.message2 = (LONG) fEnable;
	aoMsg.message3 = (LONG) g_StdShapeLibType;
	aoMsg.message4 = 0;

	(*g_pfnLayoutAdmin)(OSHP_MSG_ENABLEPALETTES, &aoMsg);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::ChangeToolState
//
// DESCR:		Changes the given tool's state.  If necessary, the
//					current tool is deactivated.
//
////////////////////////////////////////////////////////////////

void	ShapePalette::ChangeToolState(
	TYPEID		tShapeLibType,
	short			nToolId,
	LONG			lFlags
	)
	{
	BOOL			fState;

	if (lFlags & (OSHP_TS_UP | OSHP_TS_INACTIVE))
		fState = FALSE;
	else
		fState = TRUE;

	if (tShapeLibType == NULL)				// standard tool being activated
		{
		SetItemState(nToolId, fState);
		m_nCurrentTool    = nToolId;
		s_pCurrentPalette = NULL;
		}

	else if (tShapeLibType == g_StdShapeLibType)
		{
		SetItemState(nToolId, fState);

		if (fState == FALSE)
			{
			m_nCurrentTool    = -1;
			s_pCurrentPalette = NULL;
			}
		else
			{
			m_nCurrentTool    = nToolId;
			s_pCurrentPalette = this;
			}
		}

	else															// turn off any current tool
		{
		if (m_nCurrentTool != -1)
			{
			SetItemState(m_nCurrentTool, FALSE);
			m_nCurrentTool    = -1;
			s_pCurrentPalette = NULL;
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::ShapeCreated
//
// DESCR:		
//
////////////////////////////////////////////////////////////////

void	ShapePalette::ShapeCreated
  (
	ShpItem* pShape
	)

	{
	pShape->SizeToShape();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::LineAttributes
//
// DESCR:		Returns a pointer to the current line attributes contained
//					in the line attribute button.
//
////////////////////////////////////////////////////////////////

LineAttrib* ShapePalette::LineAttributes
  (
	void
	)

	{
	I_ASSERT(m_pLineButton);

	return m_pLineButton->LineAttributes();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::FillAttributes
//
// DESCR:		Returns a pointer to the current fill attributes contained
//					in the fill attribute button.
//
////////////////////////////////////////////////////////////////

FillAttrib* ShapePalette::FillAttributes
  (
	void
	)

	{
	I_ASSERT(m_pFillButton);

	return m_pFillButton->FillAttributes();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::FontAttributes
//
// DESCR:		Returns a pointer to the current font attributes contained
//					in the font attribute button.
//
////////////////////////////////////////////////////////////////

FontAttrib* ShapePalette::FontAttributes
  (
	void
	)

	{
	I_ASSERT(m_pFontButton);

	return m_pFontButton->FontAttributes();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::GetShapeID
//
// DESCR:		Converts the tool id to a shape id
//
////////////////////////////////////////////////////////////////

short ShapePalette::GetShapeID
	(
	)

	{
	short	idShape;

	switch(s_pCurrentPalette->CurrentTool())
		{
		case SHP_TOOL_BITMAP:
			idShape = SHP_BITMAP;
			break;

		case SHP_TOOL_FILLCIRCLE:
			idShape = SHP_ELLIPSEFILL;
			break;

		case SHP_TOOL_FILLRRECT:
			idShape = SHP_ROUNDRECTFILL;
			break;

		case SHP_TOOL_FILLSQUARE:
			idShape = SHP_RECTFILL;
			break;

		case SHP_TOOL_GROUPBOX:
			idShape = SHP_GROUPBOX;
			break;

		case SHP_TOOL_LINE:
			idShape = SHP_LINE;
			break;

		case SHP_TOOL_TEXT:
			idShape = SHP_TEXT;
			break;

		case SHP_TOOL_UNFILLCIRCLE:
			idShape = SHP_ELLIPSE;
			break;

		case SHP_TOOL_UNFILLRRECT:
			idShape = SHP_ROUNDRECT;
			break;

		case SHP_TOOL_UNFILLSQUARE:
			idShape = SHP_RECT;
			break;

		default:
			idShape = -1;
      break;
		}

  return idShape;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::ShapeCreate
//
// DESCR:		Create function to create a new shape.  All the work is done
//					in the constructor.  If mExtraData is NULL, then we assume that
//					we are creating the shape the first time.  If this is the case,
//					then we pass in the palette for access to the current attributes.
//
////////////////////////////////////////////////////////////////

OpWndItemD* ALMAPI		ShapePalette::ShapeCreate(
	OBJECTID							oiItem,
	OBJECTID							oiParent,
	RECT*									rcItem,
	long									lStatus,
	MEMBLOCKID						mExtraData
	)
	{
	OpWndItemD*						pShape;

	I_ASSERT((mExtraData == NULL && s_pCurrentPalette != NULL) ||
						(mExtraData != NULL));

	if(mExtraData)
    pShape = CreateVIFromMemBlock(oiItem, oiParent, rcItem, lStatus, mExtraData);
	else if(s_pCurrentPalette)
    pShape = CreateVIFromPalette(oiItem, oiParent, rcItem, lStatus);

	return pShape;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::CreateVIFromPalette
//
// DESCR:		Calls the "new" operator for the proper visual instance.
//          This method will call the constructor with the current
//          palette pointer.
//
////////////////////////////////////////////////////////////////

OpWndItemD* ShapePalette::CreateVIFromPalette
  (
	OBJECTID  oiItem,
	OBJECTID  oiParent,
	RECT*     rcItem,
	long      lStatus
  )

  {
  OpWndItemD* pShape;
	short       nType = s_pCurrentPalette->GetShapeID();
  ATTR_LINE   line;
  ATTR_FILL   fill;
  ATTR_FONT   font;

	switch(nType)
		{
		case SHP_RECTFILL:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
      (s_pCurrentPalette->FillAttributes())->PrepForFile(&fill);
			pShape = (OpWndItemD*)new ShpRectangleFill(oiItem, oiParent, rcItem, lStatus, &line, &fill);
      break;

		case SHP_RECT:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
			pShape = (OpWndItemD*)new ShpRectangle    (oiItem, oiParent, rcItem, lStatus, &line);
			break;

		case SHP_ROUNDRECTFILL:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
      (s_pCurrentPalette->FillAttributes())->PrepForFile(&fill);
			pShape = (OpWndItemD*)new ShpRoundRectFill(oiItem, oiParent, rcItem, lStatus, &line, &fill);
      break;

		case SHP_ROUNDRECT:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
			pShape = (OpWndItemD*)new ShpRoundRect    (oiItem, oiParent, rcItem, lStatus, &line);
			break;

		case SHP_ELLIPSEFILL:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
      (s_pCurrentPalette->FillAttributes())->PrepForFile(&fill);
			pShape = (OpWndItemD*)new ShpEllipseFill  (oiItem, oiParent, rcItem, lStatus, &line, &fill);
      break;

		case SHP_ELLIPSE:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
			pShape = (OpWndItemD*)new ShpEllipse      (oiItem, oiParent, rcItem, lStatus, &line);
			break;

		case SHP_TEXT:
      (s_pCurrentPalette->FontAttributes())->PrepForFile(&font);
			pShape = (OpWndItemD*)new ShpText         (oiItem, oiParent, rcItem, lStatus, &font);
			break;

		case SHP_LINE:
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
			pShape = (OpWndItemD*)new ShpLine         (oiItem, oiParent, rcItem, lStatus, &line);
			break;

		case SHP_GROUPBOX:
      (s_pCurrentPalette->FontAttributes())->PrepForFile(&font);
      (s_pCurrentPalette->LineAttributes())->PrepForFile(&line);
			pShape = (OpWndItemD*)new ShpGroupBox     (oiItem, oiParent, rcItem, lStatus, &font, &line);
			break;

		case SHP_BITMAP:
			pShape = (OpWndItemD*)new ShpBitmap       (oiItem, oiParent, rcItem, lStatus, NULL);
			break;

		default:
			pShape = NULL;
      break;
    }

  return pShape;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::CreateVIFromMemBlock
//
// DESCR:		Calls the "new" operator for the proper visual instance.
//          This method will call the constructor with a saved
//          Memory Block.
//
////////////////////////////////////////////////////////////////

OpWndItemD* ShapePalette::CreateVIFromMemBlock
  (
	OBJECTID		oiItem,
	OBJECTID		oiParent,
	RECT*				rcItem,
	long				lStatus,
  MEMBLOCKID  mExtraData
  )

  {
  OpWndItemD* pShape;
  short       nVer  = -1;
	short       nType = -1;
	void*       pMem = AMemLock(mExtraData);
  ATTR_LINE*  pLine = NULL;
  ATTR_FILL*  pFill = NULL;
  ATTR_FONT*  pFont = NULL;
  int         xRad = -1;
  int         yRad = -1;
  UINT        nAlign = DT_LEFT | DT_TOP | DT_WORDBREAK; 
  int         nTextLen;
  char*       pText;

	if(pMem)
   	{
    nVer = ((SHAPE_BASE*)pMem)->sVer;
		nType = ((SHAPE_BASE*)pMem)->sType;
    }

  // this is to keep shapes reverse compatible with Beta 1.00.
  if(nVer < SHP_VERSION)
    nType = nVer;

	switch(nType)
		{
		case SHP_RECTFILL:
      if(nVer==SHP_VERSION)
        {
        pLine = &((SHAPE_RECTFILL*)pMem)->line;
        pFill = &((SHAPE_RECTFILL*)pMem)->fill;
        }
			pShape = (OpWndItemD*)new ShpRectangleFill(oiItem, oiParent, rcItem, lStatus, pLine, pFill);
			break;

		case SHP_RECT:
      if(nVer==SHP_VERSION)
        pLine = &((SHAPE_RECT*)pMem)->line;
			pShape = (OpWndItemD*)new ShpRectangle    (oiItem, oiParent, rcItem, lStatus, pLine);
			break;

		case SHP_ROUNDRECTFILL:
      if(nVer==SHP_VERSION)
        {
        pLine = &((SHAPE_ROUNDRECTFILL*)pMem)->line;
        pFill = &((SHAPE_ROUNDRECTFILL*)pMem)->fill;
        xRad = ((SHAPE_ROUNDRECTFILL*)pMem)->ptRadius.x;
        yRad = ((SHAPE_ROUNDRECTFILL*)pMem)->ptRadius.y;
        }
			pShape = (OpWndItemD*)new ShpRoundRectFill(oiItem, oiParent, rcItem, lStatus, pLine, pFill, xRad, yRad);
			break;

		case SHP_ROUNDRECT:
      if(nVer==SHP_VERSION)
        {
        pLine = &((SHAPE_ROUNDRECT*)pMem)->line;
        xRad = ((SHAPE_ROUNDRECT*)pMem)->ptRadius.x;
        yRad = ((SHAPE_ROUNDRECT*)pMem)->ptRadius.y;
        }
			pShape = (OpWndItemD*)new ShpRoundRect    (oiItem, oiParent, rcItem, lStatus, pLine, xRad, yRad);
			break;

		case SHP_ELLIPSEFILL:
      if(nVer==SHP_VERSION)
        {
        pLine = &((SHAPE_ELLIPSEFILL*)pMem)->line;
        pFill = &((SHAPE_ELLIPSEFILL*)pMem)->fill;
        }
			pShape = (OpWndItemD*)new ShpEllipseFill  (oiItem, oiParent, rcItem, lStatus, pLine, pFill);
			break;

		case SHP_ELLIPSE:
      if(nVer==SHP_VERSION)
        pLine = &((SHAPE_ELLIPSE*)pMem)->line;
			pShape = (OpWndItemD*)new ShpEllipse      (oiItem, oiParent, rcItem, lStatus, pLine);
			break;

		case SHP_TEXT:
      if(nVer==SHP_VERSION)
        {
        pFont = &((SHAPE_TEXT*)pMem)->font;
        nAlign = ((SHAPE_TEXT*)pMem)->nAlign;
        nTextLen = ((SHAPE_TEXT*)pMem)->nTextLen;
        pText = ((SHAPE_TEXT*)pMem)->szText;
        }
      else if(nVer<SHP_VERSION) // can be deleted for Beta 3 (or V1.00)
        {
        pFont = new ATTR_FONT;
        if(pFont)
          {
          pFont->sVer = ATTR_VERSION;
          pFont->clrFG = COLOR_WINDOWFOREGROUND;
          pFont->clrBG = COLOR_TRANSPARENT;
          pFont->nTwips = ((ShapeExtra*)pMem)->Attributes.AttrGetHeight();
          if(pFont->nTwips<0)
            pFont->nTwips = -pFont->nTwips;
          pFont->nTwips = (short)(((double)pFont->nTwips * 1440. / 254. + 0.5));
          memset(&(pFont->lfFont), 0, sizeof(LOGFONT));
          pFont->lfFont.lfWeight = ((ShapeExtra*)pMem)->Attributes.AttrGetWeight();
          pFont->lfFont.lfItalic = ((ShapeExtra*)pMem)->Attributes.AttrIsItalic() ? 1 : 0;
          pFont->lfFont.lfUnderline = ((ShapeExtra*)pMem)->Attributes.AttrIsUnderlined() ? 1 : 0;
          pFont->lfFont.lfStrikeOut = ((ShapeExtra*)pMem)->Attributes.AttrIsStruckout() ? 1 : 0;
          ((ShapeExtra*)pMem)->Attributes.AttrGetFace(pFont->lfFont.lfFaceName, LF_FACESIZE);
          }

        pText = ((ShapeExtra*)pMem)->uExtra.szName;
        nTextLen = lstrlen(pText)+1;
        }
      else
        {
        nTextLen = 5;
        pText = "Text";
        }
			pShape = (OpWndItemD*)new ShpText         (oiItem, oiParent, rcItem, lStatus, pFont, nAlign, nTextLen, pText, FALSE);
      if(pFont && (nVer<SHP_VERSION))  // can be deleted for Beta 3 (or V1.00)
        delete pFont;
			break;

		case SHP_LINE:
      if(nVer==SHP_VERSION)
        pLine = &((SHAPE_LINE*)pMem)->line;
			pShape = (OpWndItemD*)new ShpLine         (oiItem, oiParent, rcItem, lStatus, pLine);
			break;

		case SHP_GROUPBOX:
      if(nVer==SHP_VERSION)
        {
        pFont = &((SHAPE_GROUPBOX*)pMem)->font;
        pLine = &((SHAPE_GROUPBOX*)pMem)->line;
        nTextLen = ((SHAPE_GROUPBOX*)pMem)->nTextLen;
        pText = ((SHAPE_GROUPBOX*)pMem)->szText;
        }
      else
        {
        nTextLen = 10;
        pText = "Group Box";
        }
			pShape = (OpWndItemD*)new ShpGroupBox     (oiItem, oiParent, rcItem, lStatus, pFont, pLine, nTextLen, pText);
			break;

		case SHP_BITMAP:
			pShape = (OpWndItemD*)new ShpBitmap       (oiItem, oiParent, rcItem, lStatus, mExtraData);
			break;

		default:
			pShape = NULL;
      break;
    }

	if(pMem)
    AMemUnlock(mExtraData);

  return pShape;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::AddStdButton
//
// DESCR:		Creates and adds a new standard button to the palette.
//					The information for the tool is retrieved from the layout
//					editor.
//
////////////////////////////////////////////////////////////////

void		ShapePalette::AddStdButton(
	UINT							nStdID
	)
	{
	UINT							nGID				= 0;
	PaletteItem*			pItem;
	OShpStdToolInfo		sToolInfo;
	AObjMessage				aoMsg;
	LONG							lStyle;
	LONG							lRes;

	I_ASSERT(g_pfnLayoutAdmin != NULL);

	sToolInfo.lStyle = 0;
	sToolInfo.hImage = NULL;

	aoMsg.message1 = g_lLayoutHandle;
	aoMsg.message2 = nStdID;
	aoMsg.message3 = (LONG) &sToolInfo;

	lRes = (*g_pfnLayoutAdmin)(OSHP_MSG_GETSTDTOOLINFO, &aoMsg);

	if (lRes != A_ERROR)
		{
		if (sToolInfo.lStyle & OSHP_STDCAP_TOGGLE)
			lStyle = PAL_CHECKBOX;
		else if (sToolInfo.lStyle & OSHP_STDCAP_PUSH)
			lStyle = PAL_PUSHBUTTON;
		else if (sToolInfo.lStyle & OSHP_STDCAP_RADIO)
			{
			lStyle = PAL_RADIOBUTTON;
			nGID   = 1;
			}

		pItem = new PaletteItem(this, nStdID, nGID,
													lStyle | PaletteItem::doesntOwnBitmap);

		ADBG_ASSERT_E(sToolInfo.hImage != NULL, ;, "Invalid standard tool bitmap");
		pItem->SetImage(sToolInfo.hImage);

		AddItem(pItem);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::AddButton
//
// DESCR:		Creates and adds a new button to the palette.  The
//					information for the tool is taken from g_tiToolInfo[].
//
////////////////////////////////////////////////////////////////

void		ShapePalette::AddButton(
	UINT							nToolID							// external tool ID (user range of tools)
	)
	{
	PaletteItem*			pItem;
	HBITMAP						hImage;
	LONG							lStyle;
	UINT							nGID;
	short							nTID;								// internal tool ID

	I_ASSERT(nToolID>= SHP_TOOL_BASE && nToolID <= SHP_TOOL_LASTTOOL);

	nTID = nToolID - SHP_TOOL_BASE;

	if (g_tiToolInfo[nTID].fFlags & TI_CMDBUTTON)
		lStyle = PAL_PUSHBUTTON;
	else if (g_tiToolInfo[nTID].fFlags & TI_RADIOBUTTON)
		lStyle = PAL_RADIOBUTTON;

	nGID = g_tiToolInfo[nTID].nGID;

	pItem  = new PaletteItem(this, nToolID, nGID, lStyle);
	hImage = g_tiToolInfo[nTID].hImage;

	pItem->SetImage(hImage);
	AddItem(pItem);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::AddAttrButton
//
// DESCR:		Creates and adds an attribute button to the palette.
//
////////////////////////////////////////////////////////////////

void		ShapePalette::AddAttrButton
  (
	UINT						nToolID
	)

	{
  switch(nToolID)
    {
    case SHP_TOOL_LINEATTRIBS:
      {
    	LineButton* pAttrs  = new LineButton(this, nToolID);

   		pAttrs->SetSize(3, 2);						// 1.5 units across, 1 unit down
    	AddItem(pAttrs);
  		m_pLineButton = pAttrs;
      break;
      }

    case SHP_TOOL_FILLATTRIBS:
      {
    	FillButton* pAttrs  = new FillButton(this, nToolID);

   		pAttrs->SetSize(3, 2);						// 1.5 units across, 1 unit down
    	AddItem(pAttrs);
  		m_pFillButton = pAttrs;
      break;
      }

    case SHP_TOOL_FONTATTRIBS:
      {
    	FontButton*  pAttrs		= new FontButton(this, nToolID);

   		pAttrs->SetSize(6, 2);						// 3   units across, 1 unit down
    	AddItem(pAttrs);
  		m_pFontButton = pAttrs;
      break;
      }

    default:
      break;
    }
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	ShapePalette::InitPalette
//
// DESCR:		Puts the items into the palette.
//
////////////////////////////////////////////////////////////////

void		ShapePalette::InitPalette
  (
	void
	)

	{
	SetWindowText(Hwnd(), "Tools");

	SetColumns(3);
	SetUnitSize(24, 24, 2);			// resolution is down to 1/2 a column&row

	// Items MUST be added left to right, top to bottom!!!

	AddStdButton(OSHP_STDTOOL_ARROW);
	AddButton   (SHP_TOOL_FILLCIRCLE);
	AddButton   (SHP_TOOL_UNFILLCIRCLE);

	// Second row
	AddStdButton(OSHP_STDTOOL_REORDER);
	AddButton   (SHP_TOOL_FILLSQUARE);
	AddButton   (SHP_TOOL_UNFILLSQUARE);

	// Third row
	AddStdButton(OSHP_STDTOOL_TABSTOP);
	AddButton   (SHP_TOOL_FILLRRECT);
	AddButton   (SHP_TOOL_UNFILLRRECT);

	// Fourth row
	AddStdButton(OSHP_STDTOOL_GROUPSTOP);
	AddButton   (SHP_TOOL_BITMAP);
	AddButton   (SHP_TOOL_GROUPBOX);

	// Fifth row
	AddStdButton(OSHP_STDTOOL_VISIBILITY);
	AddButton   (SHP_TOOL_LINE);
	AddButton   (SHP_TOOL_TEXT);

	// Sixth row
	AddStdButton(OSHP_STDTOOL_ANCHOR);
	AddStdButton(OSHP_STDTOOL_SELECTALL);
	AddStdButton(OSHP_STDTOOL_BACKGROUND);

	// Owner-draw attribute buttons
	AddAttrButton(SHP_TOOL_LINEATTRIBS);
	AddAttrButton(SHP_TOOL_FILLATTRIBS);
	AddAttrButton(SHP_TOOL_FONTATTRIBS);

	EndAdd();

	SetItemState(OSHP_STDTOOL_ARROW, TRUE);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ShapeLibAdmin
//
// DESCR:			Admin routine for the standard shape library.
//
////////////////////////////////////////////////////////////////

extern "C"
LONG ALMAPI		ShapeLibAdmin(
	UINT						nMsg,
	pAObjMessage		pMsg
	)
	{
	LONG						lRes			= A_OK;
	ShapePalette*		pPalette;
	UINT						fFlags;
	RECT						rcPalette;
	RECT*						pRect;
	char*						pStr;
	ShpItem*     		pShape;
	short						nTID;

	ADBG_ASSERT_E(pMsg != NULL, return A_ERROR, "Invalid pmsg");

	switch (nMsg)
		{
		case OSHP_SETLAYOUTINFO:
			g_lLayoutHandle  = pMsg->message1;
			g_pfnLayoutAdmin = (OLayoutAdmin) pMsg->message2;

			I_ASSERT(g_lLayoutHandle != 0L);
			I_ASSERT(g_pfnLayoutAdmin != NULL);
			break;

		case OSHP_QUERYLIBINFO:
			pMsg->message1 = (LONG) ShapePalette::ShapeCreate;
			pMsg->message2 = (LONG) g_StdShapeLibType;
			pMsg->message3 = OSHP_LIBCAP_HASPALETTE;
			break;

		case OSHP_MENUSTRING:
			pStr = (char*) (pMsg->message1);

			I_ASSERT(pStr);

			strncpy(pStr, g_szStdShapeLibName, (int) (pMsg->message2));
			break;

		case OSHP_QUERYTOOLINFO:
			nTID = ((short) pMsg->message1) - SHP_TOOL_BASE;
			pMsg->message2 = g_tiToolInfo[nTID].lCaps;
			pMsg->message3 = (LONG)g_tiToolInfo[nTID].hCursor;
			break;

		case OSHP_GETSHAPENAME:
			pShape = (ShpItem*) (pMsg->message1);
			pStr   = (char*)    (pMsg->message2);

			I_ASSERT(pShape != NULL);
			I_ASSERT(pStr   != NULL);

			pShape->ShapeName(pStr, (int) (pMsg->message3));
			break;

		case OSHP_CREATEPALETTE:
			InitShapeLibrary();

			pPalette = new ShapePalette((HWND) (pMsg->message2));
			pPalette->InitPalette();

			pMsg->message1 = (LONG) pPalette;

			GetWindowRect(pPalette->Hwnd(), &rcPalette);
			pMsg->message3 = rcPalette.right  - rcPalette.left;
			pMsg->message4 = rcPalette.bottom - rcPalette.top;

			break;

		case OSHP_DESTROYPALETTE:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			delete pPalette;

			CleanShapeLibrary();
			break;

		case OSHP_SHOWPALETTE:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			if (pMsg->message2)
				fFlags = SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW;
			else
				fFlags = SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE;

			SetWindowPos(pPalette->Hwnd(), HWND_TOP, 0, 0, 0, 0, fFlags);
			break;

		case OSHP_HIDEPALETTE:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			ShowWindow(pPalette->Hwnd(), SW_HIDE);
			break;

		case OSHP_ENABLEPALETTE:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			pPalette->Enable((BOOL) pMsg->message2);
			break;

		case OSHP_SETPALETTEPOS:
			pPalette = (ShapePalette*) (pMsg->message1);
			pRect    = (RECT*) (pMsg->message2);

			I_ASSERT(pPalette != NULL);
			I_ASSERT(pRect    != NULL);

			SetWindowPos(pPalette->Hwnd(), NULL,
										pRect->left, pRect->top, 0, 0,
										SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			break;

		case OSHP_GETPALETTEPOS:
			pPalette = (ShapePalette*) (pMsg->message1);
			pRect    = (RECT*) (pMsg->message2);

			I_ASSERT(pPalette != NULL);
			I_ASSERT(pRect    != NULL);

			GetWindowRect(pPalette->Hwnd(), pRect);
			break;

		case OSHP_GETPALETTESTATUS:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			if (IsWindowVisible(pPalette->Hwnd()))
				pMsg->message2 = OSHP_PS_VISIBLE;
			else
				pMsg->message2 = 0;
			break;

		case OSHP_SETTOOLSTATE:
			pPalette = (ShapePalette*) (pMsg->message1);

			I_ASSERT(pPalette != NULL);

			pPalette->ChangeToolState((TYPEID) pMsg->message2,
																(short)  pMsg->message3,
																(LONG)   pMsg->message4);
			break;

		case OSHP_SHAPECREATED:
			pPalette = (ShapePalette*) (pMsg->message1);
			pShape   = (ShpItem*)      (pMsg->message2);

			I_ASSERT(pPalette != NULL);
			I_ASSERT(pShape != NULL);

			pPalette->ShapeCreated(pShape);
			break;

		default:
			lRes = A_NOTHANDLED;
			break;
		}

	return lRes;
	}
