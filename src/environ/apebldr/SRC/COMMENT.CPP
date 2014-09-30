#include "almsys.h"
#include <windowsx.h>
#include <mmsystem.h>

#include "comment.h"
#include "palette.h"
#include "subject.h"
#include "seruid.h"
#include "memlib.h"
#include "sjtwin.h"
#include "mdiclass.h"
#include "tstring.h"
#include "sereclib.h"
#include "system.h"
#include "progsys.h"
#include "navcls.h"
#include "project.h"
#include "zlist.h"
#include "upsfname.h"
#include "intdebug.h"
#include "apbutil.h"

#define C_TAILHEIGHT		10
#define C_BOXHEIGHT 		12
#define C_TAILINDENT		8
#define C_MIDBORDER			1
#define C_ENDBORDER			4
#define C_MINLENGTH			24
#define C_TAILBASE			10
#define C_NUMPOINTS     17
#define C_DELAY					30

HFONT COMMENT::font = NULL;
int COMMENT::count = 0;

COMMENT::COMMENT(
	SERUID theSubjectUID,
	POINT newLocation,
	LPCSTR commentName,
	LPCSTR commentBody):FUNCWINOBJ(theSubjectUID, newLocation)
	{
	SUBJECT * subjectPtr = GetSubjectPtr();
	PROJECT * projPtr = subjectPtr->GetParentProject();

	polyPts = (LPPOINT)Allocate(sizeof(POINT) * C_NUMPOINTS);
	if(count == 0)
		Initialize();
	
	SetCommentFlag();
	briefComment = commentName;
	fullComment = commentBody;
	BuildPoly();
	SetViewOrigin();
	count++;
	MDI::GetNavigatorPtr()->AddItem(GetUID(projPtr),GetUID(subjectPtr),
		GetUID(this), projPtr->fileObjPtr, briefComment);

	}

COMMENT::COMMENT(
	void * thisPtr):FUNCWINOBJ(thisPtr),
		briefComment(&briefComment),
		fullComment(&fullComment)
	{
	if(count == 0)
		Initialize();
	
	polyPts = (LPPOINT)Allocate(sizeof(POINT) * C_NUMPOINTS);
	BuildPoly();
	count++;
	}

short COMMENT::PostReadInit()
	{
	SUBJECT * subjectPtr;
	PROJECT * projPtr;

	subjectPtr = GetSubjectPtr();
	projPtr = subjectPtr->GetParentProject();

	MDI::GetNavigatorPtr()->AddItem(GetUID(projPtr), GetUID(subjectPtr),
		GetUID(this), projPtr->fileObjPtr, briefComment);
	return PRJ_READNOERROR; //signal that no error occured
	}


COMMENT::~COMMENT()
	{
	SUBJECT * subjectPtr;
	PROJECT * projPtr;

	count--;
	if(count == 0)
		Unitialize();
		
	subjectPtr = GetSubjectPtr();
	projPtr = subjectPtr->GetParentProject();
	Deallocate(polyPts);

	MDI::GetNavigatorPtr()->DeleteItem(projPtr->fileObjPtr, GetUID(projPtr),
		GetUID(subjectPtr), GetUID(this));
	}


void COMMENT::ChangeCommentName(
	LPCSTR shortName,
	LPCSTR fullName)
	{
	SUBJECT * subjectPtr;

	subjectPtr = GetSubjectPtr();
	if(shortName)
		briefComment = shortName;
	
	if(fullName)
		fullComment = fullName;
	
	BuildPoly();

  //update name in the navigator

	if(shortName)
		MDI::GetNavigatorPtr()->SetItemName(GetUID(this),
   		subjectPtr->GetParentProject()->fileObjPtr, briefComment);

	}

void COMMENT::Initialize()
	{
	LOGFONT lf;

	I_ASSERT(count == 0);
	
	lf.lfHeight = 10;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 400;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy((LPSTR)lf.lfFaceName, (LPSTR)"Helv");
	COMMENT::font = CreateFontIndirect((LPLOGFONT)&lf);
	}

void COMMENT::Unitialize()
	{
	I_ASSERT(count == 0);

	DeleteObject(font);
	}

void COMMENT::Draw(
	HDC hdc)
	{
	HBRUSH newBrush;
	HPEN		newPen;
	RECT	textRect;
	COLORREF textColor;
	int oldDC;


	oldDC = SaveDC(hdc);

	SetROP2(hdc, R2_COPYPEN);
	if(status & FWO_SELECTED)
		{
		newBrush = GetStockBrush(BLACK_BRUSH);
		newPen = GetStockPen(WHITE_PEN);
		textColor = RGB(255,255,255);
		}
	else
		{
		newBrush = GetStockBrush(WHITE_BRUSH);
		newPen = GetStockPen(BLACK_PEN);
		textColor = RGB(0,0,0);
		}

	SelectObject(hdc, newBrush);
	SetTextColor(hdc, textColor);
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, COMMENT::font);

	textRect.left = polyPts[0].x;
	textRect.top = polyPts[15].y;
	textRect.right = polyPts[6].x;
	textRect.bottom = polyPts[4].y;

	//always want black pen
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	Polygon(hdc, polyPts, C_NUMPOINTS);

	//newPen contains the text color
	SelectObject(hdc, newPen);
	DrawText(hdc, briefComment, briefComment.strlen(), (LPRECT)&textRect,
		DT_NOPREFIX | DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);

	RestoreDC(hdc, oldDC);

	}

void COMMENT::Identify(
	HDC hdc)
	{
	unsigned int i;
	int	parity;

	if(status & FWO_SELECTED)
		parity=1;
	else
		parity=0;
	for(i=0;i<8;i++)
		{
		if(parity)
			SetSelected(FALSE);
		else
			SetSelected(TRUE);
		Draw(hdc);

    SleepEx(C_DELAY, FALSE);
		parity^=0x0001;
		}

	}


void COMMENT::DrawSelection(
	HDC hdc)
	{
	Draw(hdc);
	}


void COMMENT::GetRect(
	LPRECT bounds)
	{
	bounds->left = location.x;
	bounds->top = location.y;
	bounds->right = polyPts[3].x;
	bounds->bottom = polyPts[8].y;
	}

void COMMENT::DrawDrag(
	HDC hdc,
	POINT offset)
	{
	HPEN oldPen;
	HBRUSH oldBrush;
	int oldMode;
	POINT pt[C_NUMPOINTS];
	int i;

	oldMode = SetROP2(hdc, R2_NOTXORPEN);
	oldPen = SelectPen(hdc, PROGSYSTEM::dottedDragPen);
	oldBrush = SelectBrush(hdc, GetStockBrush(NULL_BRUSH));
	for(i=0;i< C_NUMPOINTS; i++)
		{
		pt[i].x = polyPts[i].x + offset.x;
		pt[i].y = polyPts[i].y + offset.y;
		}
	Polygon(hdc,(LPPOINT)&pt, C_NUMPOINTS);
	SelectPen(hdc, oldPen);
	SelectBrush(hdc, oldBrush);
	SetROP2(hdc, oldMode);
	}




void COMMENT::BuildPoly()
	{
	HDC hdc;
	int	length;
	int height;;
  SIZE textSize;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SelectObject(hdc, COMMENT::font);
	I_VERIFY(GetTextExtentPoint32(hdc, briefComment, briefComment.strlen(), &textSize));
	DeleteDC(hdc);
	length = textSize.cx;
	height = textSize.cy;
	if(height < C_BOXHEIGHT)
		height = C_BOXHEIGHT;
	if(length < C_MINLENGTH)
		length = C_MINLENGTH;

	polyPts[0].x = location.x + C_ENDBORDER;
	polyPts[0].y = location.y;
	polyPts[1].x = polyPts[0].x + length;
	polyPts[1].y = polyPts[0].y;
	polyPts[2].x = polyPts[1].x + C_ENDBORDER - C_MIDBORDER;
	polyPts[2].y = polyPts[1].y + C_MIDBORDER;
	polyPts[3].x = polyPts[2].x + C_MIDBORDER;
	polyPts[3].y = polyPts[1].y + C_ENDBORDER;
	polyPts[4].x = polyPts[3].x;
	polyPts[4].y = polyPts[3].y + height;
	polyPts[5].x = polyPts[2].x;
	polyPts[5].y = polyPts[4].y + C_ENDBORDER - C_MIDBORDER;
	polyPts[6].x = polyPts[1].x;
	polyPts[6].y = polyPts[5].y + C_MIDBORDER;

	polyPts[16].x = location.x + C_MIDBORDER;
	polyPts[16].y = polyPts[2].y;
	polyPts[15].x = location.x;
	polyPts[15].y = polyPts[3].y;
	polyPts[14].x = polyPts[15].x;
	polyPts[14].y = polyPts[4].y;
	polyPts[13].x = polyPts[16].x;
	polyPts[13].y = polyPts[5].y;
	polyPts[12].x = polyPts[0].x;
	polyPts[12].y = polyPts[6].y;
	polyPts[11].x = polyPts[12].x + C_TAILINDENT;
	polyPts[11].y = polyPts[12].y;
	polyPts[10].x = polyPts[11].x + 1;
	polyPts[10].y = polyPts[11].y + 1;
	polyPts[9].x = polyPts[10].x+1;
	polyPts[9].y = polyPts[10].y + 1;
	polyPts[8].x = polyPts[10].x - 1;
	polyPts[8].y = polyPts[11].y + C_TAILHEIGHT;
	polyPts[7].x = polyPts[11].x + C_TAILBASE;
	polyPts[7].y = polyPts[6].y;

	}

void COMMENT::Move(
	POINT offset)
	{
	location.x+=offset.x;
	location.y+=offset.y;
	BuildPoly();
	SetViewOrigin();
	}


void COMMENT::SetViewOrigin()
	{
	SUBJECT * subjectPtr=GetSubjectPtr();

	viewOrigin = subjectPtr->GetOriginOffset();
	}


void COMMENT::ShowItem()
	{
	SUBJECT * subjectPtr;
	HDC hdc;
	HWND hwnd = NULL;
	RECT winBounds;
	RECT cmtBounds;

	subjectPtr = GetSubjectPtr();
	subjectPtr->originOffset.x = viewOrigin.x;
	subjectPtr->originOffset.y = viewOrigin.y;
	subjectPtr->GetSubjectClientBounds((LPRECT)&winBounds);
	GetRect((LPRECT)&cmtBounds);

	if(subjectPtr->originOffset.x >cmtBounds.left)
		subjectPtr->originOffset.x = cmtBounds.left - SEPARATOR;
	else if(subjectPtr->originOffset.x + (winBounds.right - winBounds.left) <
		cmtBounds.right)
		subjectPtr->originOffset.x = cmtBounds.right - (winBounds.right -
			winBounds.left);

	if(subjectPtr->originOffset.y >cmtBounds.top)
		subjectPtr->originOffset.y = cmtBounds.top;
	else if(subjectPtr->originOffset.y + (winBounds.bottom - winBounds.top) <
		cmtBounds.bottom)
			subjectPtr->originOffset.y = cmtBounds.bottom - (winBounds.bottom -
			winBounds.top);

	if(subjectPtr->GetWindowPtr())
		{
		hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
		hdc = GetDC(hwnd);
		I_VERIFY(SetWindowOrgEx(hdc, subjectPtr->originOffset.x,
			subjectPtr->originOffset.y, NULL));
		InvalidateRect(hwnd, NULL, TRUE);
		ReleaseDC(hwnd, hdc);
		((SUBJECTWINDOW *)subjectPtr->GetWindowPtr())->SetAllScrollBars();
		}

	subjectPtr->CalculateAllFlowPaths();
	subjectPtr->ShowObjectWindow();
	UpdateWindow(hwnd);
	if(!hwnd)
		hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
			hdc = GetDC(hwnd);
	Identify(hdc);
	ReleaseDC(hwnd, hdc);
	}

void COMMENT::LocationChanged()
	{
	BuildPoly();
	SetViewOrigin();
	}


BOOL COMMENT::AddObjectToZlist(
	LPRECT area,
	LPZLIST zlistPtr)
	{
	RECT bounds;
	RECT dummy;

	GetRect((LPRECT)&bounds);

	if(IntersectRect((LPRECT)&dummy, (LPRECT)&bounds, area))
		{
		zlistPtr->Add((LPRECT)&bounds, Z_COMMENT, GetUID(this));
		return TRUE;
		}
	return FALSE;
	}


WORD COMMENT::ExportUPSF()
	{
	long result;
	WORD returnValue = EXPORT_NOERROR;

	if(AUpsfExportField(COMMENT_BRIEFNAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPCSTR)briefComment, (long)briefComment.strlen()) != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	if(AUpsfExportField(COMMENT_FULLNAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPCSTR)fullComment, (long)fullComment.strlen()) != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	//export x coordinate
	if(AUpsfExportField(COMMENT_LOCATION, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.x, (long)sizeof(int))!=AUPSF_NOERROR)
			returnValue|=EXPORT_GENERALERROR;

	//export y coordinate
	if(AUpsfExportField(COMMENT_LOCATION, 1L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.y, (long)sizeof(int))!=AUPSF_NOERROR)
			returnValue|=EXPORT_GENERALERROR;

	//export horizonatl worksheet scroll to positon
	if(AUpsfExportField(COMMENT_LOCATION, 3L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&viewOrigin.x, (long)sizeof(int))!=AUPSF_NOERROR)
			returnValue|=EXPORT_GENERALERROR;

	//export vertical worksheet scroll to positon
	//Do a check to see if we've run out of disk space
	result =  AUpsfExportField(COMMENT_LOCATION, 4L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&viewOrigin.y, (long)sizeof(int));
	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result !=AUPSF_NOERROR)
			returnValue|=EXPORT_GENERALERROR;

	PROJECT::IncStatusBar();
	return returnValue;
	}


BOOL COMMENT::ImportUPSF()
	{
	LPSTR briefPtr=NULL;
	LPSTR fullPtr=NULL;
	ATYPEID uniType;
	long result;
	POINT tempPt;
	long bufSize;


	I_ALWAYSASSERT((result = AUpsfFieldInfo(COMMENT_BRIEFNAME, 0L, 
		AUPSF_NATV_STRING, (LPTYPEID)&uniType, (LPLONG)&bufSize)) != 
		AUPSF_MEMORYERROR, return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		//allocate buffer to hold brief comment
		briefPtr = (LPSTR)GlobalAlloc(GMEM_ZEROINIT,
			(WORD)(bufSize + 1));

		I_ALWAYSASSERT(briefPtr != NULL, return FALSE);
			
		//import the name
		I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_BRIEFNAME, 0L, 
			AUPSF_NATV_STRING, (LPVOID)briefPtr, bufSize)) != AUPSF_MEMORYERROR,
			return FALSE);

		}

	if(result != AUPSF_NOERROR)
		apb_LogUpsfError(result, "comment's brief name");
		
	I_ALWAYSASSERT((result = AUpsfFieldInfo(COMMENT_FULLNAME, 0L, AUPSF_NATV_STRING,
		(LPTYPEID)&uniType, (LPLONG)&bufSize)) != AUPSF_MEMORYERROR,
		return FALSE);

	if(result == AUPSF_NOERROR)
		{
		//allocate buffer to hold full comment
		fullPtr = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, bufSize + 1);
			
		I_ALWAYSASSERT(fullPtr != NULL, return FALSE);
		
		//import name
		I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_FULLNAME, 0L, 
			AUPSF_NATV_STRING, (LPVOID)fullPtr, bufSize)) != AUPSF_MEMORYERROR,
			return FALSE);
		}

	if(result != AUPSF_NOERROR)
		apb_LogUpsfError(result, "comment's full name");

	if(fullPtr || briefPtr)
		ChangeCommentName(briefPtr, fullPtr);

	//free memory
	if(fullPtr)
		GlobalFree(fullPtr);

	if(briefPtr)
		GlobalFree(briefPtr);


	//Read in worksheet display postion
	//This must be done after changing the location since changing the
	//location resets the vieworigin

	//read in x location
	I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_LOCATION, 3L, 
		AUPSF_NATV_INT, (LPVOID)&tempPt.x, (long)sizeof(int))) != AUPSF_MEMORYERROR,
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		//read in y location
		I_ALWAYSASSERT((result = AUpsfImportField(COMMENT_LOCATION, 4L, 
			AUPSF_NATV_INT, (LPVOID)&tempPt.y, (long)sizeof(int))) !=
			AUPSF_MEMORYERROR, return FALSE);
		}

	if(result == AUPSF_NOERROR)
		viewOrigin = tempPt;
	else	
		apb_LogUpsfError(result, "comments worksheet display "
			"postion");

	return TRUE; //no error if we reach here
	}

