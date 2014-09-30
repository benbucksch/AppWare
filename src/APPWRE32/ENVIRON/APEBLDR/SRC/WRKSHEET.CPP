#include "almsys.h"

#include "basewin.h"
#include "wrksheet.h"
#include "mdiclass.h"
#include "progsys.h"
#include "winlist.h"

HFONT						WORKSHEET::subFont=NULL;
HFONT						WORKSHEET::numSubFont;
HICON						WORKSHEET::subjectIcon;
HICON						WORKSHEET::projectIcon;
unsigned 				WORKSHEET::worksheetCount=0;
HPEN						WORKSHEET::shadowPen = NULL;
HBITMAP					WORKSHEET::rightScrollArrow = NULL;
HBITMAP					WORKSHEET::leftScrollArrow = NULL;
HBITMAP					WORKSHEET::upScrollArrow = NULL;
HBITMAP 				WORKSHEET::downScrollArrow = NULL;
WINDOWLIST *    WORKSHEET::indexList=NULL;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WORKSHEET
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  constructor.  If neccessary, perform initialization.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
WORKSHEET::WORKSHEET()
	{
	status = 0x0000;
	InitConstructor();
	}

WORKSHEET::WORKSHEET(
	void * /*thisPtr*/)
	{
	InitConstructor();
	}

void WORKSHEET::InitConstructor()
	{
	if(!WORKSHEET::worksheetCount)
		WORKSHEET::Initialize();
	WORKSHEET::worksheetCount++;
	SetWindowPtr(NULL);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~WORKSHEET
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  destructor.  If neccessary, clean up from
//									initialization.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
WORKSHEET::~WORKSHEET()
	{
	WORKSHEET::worksheetCount--;
	if(!WORKSHEET::worksheetCount)
		UnInitialize();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  perform neccessary initialization.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::Initialize()
	{
	LOGFONT	lf;

	if(!WORKSHEET::subFont)
		{
		lf.lfHeight = 10;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = 500;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = ANSI_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = PROOF_QUALITY;
		lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		lstrcpy((LPSTR)lf.lfFaceName, (LPSTR)"Helv");
		WORKSHEET::subFont = CreateFontIndirect((LPLOGFONT)&lf);
		lf.lfWeight = 800;
		lf.lfHeight = 11;
		WORKSHEET::numSubFont = CreateFontIndirect((LPLOGFONT)&lf);
		WORKSHEET::shadowPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
		WORKSHEET::subjectIcon = PROGSYSTEM::GetSubjectIcon();
		WORKSHEET::projectIcon = PROGSYSTEM::GetProjectIcon();
		WORKSHEET::rightScrollArrow = LoadBitmap(PROGSYSTEM::hInst,
			(LPSTR)"rightArrow");
		WORKSHEET::leftScrollArrow = LoadBitmap(PROGSYSTEM::hInst,
			(LPSTR)"leftArrow");
		WORKSHEET::upScrollArrow = LoadBitmap(PROGSYSTEM::hInst,
			(LPSTR)"upArrow");
		WORKSHEET::downScrollArrow = LoadBitmap(PROGSYSTEM::hInst,
			(LPSTR)"downArrow");
		indexList = (WINDOWLIST *)new WINDOWLIST();
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	UnInitialize
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  clean up from initialization.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::UnInitialize()
	{
	if(WORKSHEET::subFont)
		{
		DeleteObject(WORKSHEET::subFont);
		DeleteObject(WORKSHEET::shadowPen);
		DeleteObject(WORKSHEET::numSubFont);
		DeleteObject(WORKSHEET::rightScrollArrow);
		DeleteObject(WORKSHEET::leftScrollArrow);
		DeleteObject(WORKSHEET::upScrollArrow);
		DeleteObject(WORKSHEET::downScrollArrow);
		WORKSHEET::subFont = NULL;
		delete indexList;
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ShowItem
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  calls the virtual function ShowObjectWindow.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::ShowItem(void)
{
	ShowObjectWindow();
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ActivateTheWindow
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:  brings the window to the front and activates it.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::ActivateTheWindow()
{
	if (GetTopWindow(GetDesktopWindow()) != PROGSYSTEM::hWndFrame)
		BringWindowToTop(PROGSYSTEM::hWndFrame);

	BringWindowToTop(windowPtr->GetWindowHandle());
	UpdateWindow(windowPtr->GetWindowHandle());
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetWindowColor
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::SetWindowColor(
	COLORREF theColor)
	{
	windowColor = theColor;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetWindowColor
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
COLORREF WORKSHEET::GetWindowColor()
	{
	return windowColor;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetWindowBounds
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::SetWindowBounds(
	LPRECT theRect)
	{
	bounds = *theRect;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetWindowBounds
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::GetWindowBounds(
	LPRECT winBounds)
	{
	*winBounds = bounds;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetOrder
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
unsigned int WORKSHEET::GetOrder()
	{
	return order;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetOrder
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
unsigned int WORKSHEET::SetOrder(
	unsigned int newOrder)
	{
	unsigned int oldOrder = order;

	order = newOrder;
	return oldOrder;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  IncrementOrder
//
// CLASS.......:  WORKSHEET
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void WORKSHEET::IncrementOrder()
	{
	order++;
	}
