#include "almsys.h"
#include <windowsx.h>

#include "funcinst.h"
#include "palette.h"
#include "subject.h"
#include "seruid.h"
#include "fwinobj.h"
#include "flow.h"
#include "objinst.h"
#include "list.h"
#include "tstring.h"
#include "progsys.h"
#include "sereclib.h"
#include "rectypes.h"
#include "cfglibp.h"
#include "exeparam.h"
#include "exefunc.h"
#include "params.h"
#include "clipparm.h"
#include "exelibl.h"  // for Compile()
#include "objtype.h"
#include "errors.h"
#include "mdiclass.h"
#include "sjtwin.h"
#include "param.h"
#include "project.h"
#include "uidlist.h"
#include "pflwstak.h"
#include "prtlabel.h"
#include "printer.h"
#include "zlist.h"
#include "flowstak.h"
#include "objgroup.h"
#include "upsfname.h"
#include "apbutil.h"

// HL stands for highlight

#define HL_ARROWWIDTH  15  //make divisible by 3
#define HL_ARROWHEIGHT 16
#define HL_DRAWARROWWIDTH 12
#define HL_DRAWARROWHEIGHT 12
#define FUNC_SELECTEDBORDER 2
#define FUNC_SELECTEDSIZE  (FUNCSIZE + 2*FUNC_SELECTEDBORDER)
#define PARAM_DEFAULTWARNING "Couldn't import name"


#define FUNCARROWHEIGHT 8
#define FUNCARROWWIDTH 4

int FUNCINST::funcCount = 0;
HBITMAP FUNCINST::blackFuncArrow = NULL;
HBITMAP FUNCINST::arrowMask;
HBITMAP FUNCINST::selectionBmp;
HBITMAP FUNCINST::textBmp;
HFONT		FUNCINST::funcFont;

HBITMAP FUNCINST::shadeMask;
HBITMAP FUNCINST::shadeBmp;

//////////////////////////////////////////////////////////
FUNCINST::FUNCINST(
	SERUID 			theSubjectUID,
	ATYPEID		  theType,
	POINT 			newLocation):
		FUNCWINOBJ(theSubjectUID, newLocation),
		FLOWMGR()
	{
	subjectUID = theSubjectUID;
	infoPtr = cfgGetFunctionDesc(theType);
	if(infoPtr->GetFlowsCount() > 0)
		status|=FN_FLOWOUT;
	SetFunctionFlag();
	inFlowFlag = 0x0000;
	outFlowFlag = 0x0000;
	if(!funcCount)
		Initialize();
	funcCount++;
	GetParams();
	CalculateBounds();
	typeID = theType;
	
	MarkRequiredTypes(GetSubjectPtr()->GetParentProject()->TestUpsfImportFlag());
	}

//////////////////////////////////////////////////////////
FUNCINST::FUNCINST(
	void * thisPtr):FUNCWINOBJ(thisPtr),
		FLOWMGR(thisPtr)
	{
	if(!funcCount)
		Initialize();
	funcCount++;

	//some projects have incorrect status bits saved out so correct them here
	status &= ~(FN_LABELVISITED | FN_PRINTVISITED);
	}

//////////////////////////////////////////////////////////
FUNCINST::~FUNCINST()
	{
	funcCount--;
	if(!funcCount)
		UnInitialize();
    
	if(status & FN_INPUTS)
		{
    ((RECORDBLOCK *)GetObject(inputParams))->PurgeBlock();
		delete ((RECORDBLOCK *)GetObject(inputParams));
		}
	if(status & FN_OUTPUTS)
		{
		((RECORDBLOCK *)GetObject(outputParams))->PurgeBlock();
		delete ((RECORDBLOCK *)GetObject(outputParams));
		}
	}


//////////////////////////////////////////////////////////
//static routine to be called externally
BOOL FUNCINST::SetSysColors()
	{
	//See if the stuff already exists and if so, delete it before recreating
	//it.
	//Only want to Initialize if functions exist otherwise just wait
	//until a function is created to Inialize.  This will prevent multiple
	//Initializations without an Uninitialize.
	if(funcCount)
		{
		UnInitialize();

		if(!Initialize())
			return FALSE;
		}

	return TRUE;
	}


//////////////////////////////////////////////////////////
//static routine to initialize static data
BOOL FUNCINST::Initialize()
	{
	HDC hdc;
	HDC hdcMem1;
	HDC hdcMem2;
	HBITMAP selectionMask;
	HBITMAP arrowPattern;
	HBRUSH  tempBrush;
	RECT rect;
	LOGFONT lf;

	if(blackFuncArrow == NULL)
		{
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
		lf.lfPitchAndFamily = VARIABLE_PITCH/* | FF_SWISS */;
		lstrcpy((LPSTR)lf.lfFaceName, (LPSTR)"Small Fonts");
		funcFont = CreateFontIndirect((LPLOGFONT)&lf);

		arrowMask = LoadBitmap(PROGSYSTEM::hInst, (LPSTR)"ParmArrowMask");
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);

		if(!hdc)
			return FALSE;

		hdcMem1 = CreateCompatibleDC(hdc);
		hdcMem2 = CreateCompatibleDC(hdc);

		if(!hdcMem1 || !hdcMem2)
			return FALSE;

		arrowPattern = CreateCompatibleBitmap(hdc, FUNCARROWWIDTH,
			FUNCARROWHEIGHT);

		if(!arrowPattern)
			return FALSE;

		//the arrow pattern will be a bitmap with the arrow in white and the
		//rest in black so we need to load the arrowMask into it and then do
		//an invert.
		SelectObject(hdcMem2, arrowPattern);
		SelectObject(hdcMem1, arrowMask);
		BitBlt(hdcMem2, 0, 0, FUNCARROWWIDTH, FUNCARROWHEIGHT, hdcMem1, 0, 0,
			NOTSRCCOPY);

		//now build the blackFuncArrow with the arrow in the text color and the
		//rest in black.
		blackFuncArrow = CreateCompatibleBitmap(hdc, FUNCARROWWIDTH,
			FUNCARROWHEIGHT);

		if(!blackFuncArrow)
			return FALSE;

		SelectObject(hdcMem1, blackFuncArrow);
		rect.left = rect.top = 0;
		rect.right = FUNCARROWWIDTH;
		rect.bottom = FUNCARROWHEIGHT;
		FillRect(hdcMem1, (LPRECT)&rect, PROGSYSTEM::normalTextBrush);
		//Now apply the arrowPattern as a mask which will turn everything black
		//other than the arrow.
		//NOTE: hdcMem2 should still contain the arrowPattern
		BitBlt(hdcMem1, 0, 0, FUNCARROWWIDTH, FUNCARROWHEIGHT, hdcMem2, 0, 0,
			SRCAND);

		//now build the function selection stuff
		selectionMask = LoadBitmap(PROGSYSTEM::hInst, (LPSTR)"FuncMask");
		selectionBmp = CreateCompatibleBitmap(hdc, FUNC_SELECTEDSIZE,
			FUNC_SELECTEDSIZE);

		if(!selectionBmp)
			return FALSE;

		//build the selection bitmap.
		SelectObject(hdcMem1, selectionBmp);
		rect.right = rect.bottom = FUNC_SELECTEDSIZE;
		FillRect(hdcMem1, &rect, PROGSYSTEM::windowBrush);
		SetROP2(hdcMem1, R2_XORPEN);
		//I didn't use FillRect here because the drawing mode R2_XORPEN didn't
		//seem to work right.
		SelectObject(hdcMem1, PROGSYSTEM::highlightPen);
		SelectObject(hdcMem1, PROGSYSTEM::highlightBrush);
		Rectangle(hdcMem1, 0, 0, FUNC_SELECTEDSIZE, FUNC_SELECTEDSIZE);
		SelectObject(hdcMem2, selectionMask);
		BitBlt(hdcMem1, 0, 0, FUNC_SELECTEDSIZE, FUNC_SELECTEDSIZE, hdcMem2,0,0,
			SRCAND);

		//build the selection mask
		shadeBmp = CreateCompatibleBitmap(hdc,FUNCSIZE, FUNCSIZE);

		if(!shadeBmp)
			return FALSE;

		SelectObject(hdcMem2, shadeBmp);
		rect.right = rect.bottom = FUNCSIZE;
		FillRect(hdcMem2,&rect, PROGSYSTEM::highlightBrush);
		shadeMask = LoadBitmap(PROGSYSTEM::hInst, "ShadeMask");
		SelectObject(hdcMem1, shadeMask);
		BitBlt(hdcMem2,0,0,FUNCSIZE,FUNCSIZE,hdcMem1,0,0,SRCAND);
		BitBlt(hdcMem1,0,0,FUNCSIZE,FUNCSIZE,NULL,0,0,DSTINVERT);


		//Create the bitmap inwhich text will be displayed for the option
		//where function names are displayted.
		textBmp = CreateCompatibleBitmap(hdc, FUNCSIZE, FUNCSIZE);

		if(!textBmp)
			return FALSE;

		if(PROGSYSTEM::windowColor != RGB(192,192,192))
			tempBrush = CreateSolidBrush(RGB(192,192,192));
		else
			tempBrush = CreateSolidBrush(RGB(255,255,255));

		if(PROGSYSTEM::windowColor != RGB(0,0,0))
			SelectObject(hdcMem2, GetStockObject(BLACK_PEN));
		else
			SelectObject(hdcMem2, GetStockObject(WHITE_PEN));

		SelectObject(hdcMem2, tempBrush);
		SelectObject(hdcMem2, textBmp);

		Rectangle(hdcMem2, 0, 0, FUNCSIZE, FUNCSIZE);

		DeleteDC(hdcMem1);
		DeleteDC(hdcMem2);
		DeleteDC(hdc);
		DeleteObject(selectionMask);
		DeleteObject(arrowPattern);
		DeleteObject(tempBrush);
		}

	//signal no error
	return TRUE;
	}

//////////////////////////////////////////////////////////
PARAM * FUNCINST::GetParam(
	int index)
	{
	RUNTIMEUID paramBlock = NULLSERUID;

	if(index < 0)
		{
		if(status & FN_OUTPUTS)
			{
			paramBlock = outputParams;
			index = (index + 1)*-1;
			}
		}
	else if(status & FN_INPUTS)
		{
		paramBlock = inputParams;
		}
	if(paramBlock != NULLSERUID)
		return (PARAM *)GetObject(paramBlock, index);
	else
		return NULL;
	}

//////////////////////////////////////////////////////////
//static routine to cleanup static data
void FUNCINST::UnInitialize()
	{
	if(blackFuncArrow != NULL)
		{
		DeleteObject(blackFuncArrow);
		DeleteObject(arrowMask);
		DeleteObject(selectionBmp);
		DeleteObject(shadeBmp);
		DeleteObject(shadeMask);
		DeleteObject(textBmp);
		DeleteObject(funcFont);
		blackFuncArrow = NULL;
		}
	}

//////////////////////////////////////////////////////////
void FUNCINST::Draw(
	HDC hdc)
	{
	HDC hdcMem;
	int i=0;
	PARAM * tempParam;
	RECT textRect;
	int oldMode;
	COLORREF oldColor;
	HFONT oldFont;
	LPSTR name;
	int nameLength;
	UINT flags;
  SIZE size;

	hdcMem = CreateCompatibleDC(hdc);
	if(inFlowFlag)
		{
		SelectObject(hdcMem, arrowMask);
		BitBlt(hdc,location.x - FUNCARROWWIDTH, location.y + (FUNCSIZE -
			FUNCARROWHEIGHT)/2, FUNCARROWWIDTH, FUNCARROWHEIGHT,
			hdcMem, 0, 0, SRCAND);
		SelectObject(hdcMem, blackFuncArrow);
		BitBlt(hdc,location.x - FUNCARROWWIDTH, location.y + (FUNCSIZE -
			FUNCARROWHEIGHT)/2, FUNCARROWWIDTH, FUNCARROWHEIGHT,
			hdcMem, 0, 0, SRCPAINT);
		}
	if(status & FN_FLOWOUT && outFlowFlag)
		{
		SelectObject(hdcMem, arrowMask);
		BitBlt(hdc,location.x+FUNCSIZE, location.y + (FUNCSIZE -
			FUNCARROWHEIGHT)/2, FUNCARROWWIDTH, FUNCARROWHEIGHT,
			hdcMem, 0, 0, SRCAND);
		SelectObject(hdcMem, blackFuncArrow);
		BitBlt(hdc,location.x+FUNCSIZE, location.y + (FUNCSIZE -
			FUNCARROWHEIGHT)/2, FUNCARROWWIDTH, FUNCARROWHEIGHT,
			hdcMem, 0, 0, SRCPAINT);
		}

	if(PROGSYSTEM::TestFuncNamesFlag())
		{
    //first draw a rectangle the size of 
		textRect.left = location.x + 1;
		textRect.right = location.x + FUNCSIZE -1;
		textRect.top = location.y;
		textRect.bottom = location.y + FUNCSIZE -1;

		oldMode = SetBkMode(hdc, TRANSPARENT);
		oldFont = SelectFont(hdc, funcFont);
		name = infoPtr->pGetName();
		nameLength = lstrlen(name);

		flags = DT_NOPREFIX;
		if(HasSpaces(name))
			flags |= DT_WORDBREAK;
		else
			{
			flags|=DT_SINGLELINE|DT_VCENTER;
			//if the name fits with function boundaries then horizontally center it
			I_VERIFY(GetTextExtentPoint32(hdc, name, nameLength, &size));
			if(size.cx < FUNCSIZE)	
				flags|=DT_CENTER;
			}
		SelectBitmap(hdcMem, textBmp);
		BitBlt(hdc,location.x,location.y,FUNCSIZE,FUNCSIZE,hdcMem,0,0,SRCCOPY);
		if(status & FWO_SELECTED)
			{
			oldColor = SetTextColor(hdc, PROGSYSTEM::highlightColor);
			DrawSelectionRect(hdc);
			}
		else
			oldColor = SetTextColor(hdc, PROGSYSTEM::normalTextColor);

		DrawText(hdc, name, nameLength,
			(LPRECT)&textRect, flags);

		SelectFont(hdc, oldFont);
		SetTextColor(hdc, oldColor);
		SetBkMode(hdc, oldMode);
		}
	else
		{
		infoPtr->GetIcons()->Draw(hdc,location.x,location.y);

		if(status & FWO_SELECTED)
			{
			DrawSelectionRect(hdc);
			SelectBitmap(hdcMem, shadeMask);
			BitBlt(hdc,location.x,location.y,FUNCSIZE,FUNCSIZE,hdcMem,0,0,SRCAND);
			SelectBitmap(hdcMem, shadeBmp);
			BitBlt(hdc,location.x,location.y,FUNCSIZE,FUNCSIZE,hdcMem,0,0,SRCINVERT);
			}
		}

	DeleteDC(hdcMem);

	if(status & FN_INPUTS)
		{
		while(0 != (tempParam = (PARAM *)GetObject(inputParams, i)))
			{
			tempParam->Draw(hdc, FALSE);
			}
		}
	i=0;
	if(status & FN_OUTPUTS)
		{
		while(0 != (tempParam = (PARAM *)GetObject(outputParams, i)))
			{
			tempParam->Draw(hdc, FALSE);
			}
		}
	}


//////////////////////////////////////////////////////////
void FUNCINST::IncFlowCount(
	int which)
	{
	if(which & FLOW_IN)
		inFlowFlag++;
	else if(which & FLOW_OUT)
		outFlowFlag++;
	}

//////////////////////////////////////////////////////////
void FUNCINST::DecFlowCount(
	int which)
	{
	if(which & FLOW_IN)
		inFlowFlag--;
	if(which & FLOW_OUT)
		outFlowFlag--;
	}

//////////////////////////////////////////////////////////
void FUNCINST::DrawSelection(
	HDC hdc)
	{
	if(!(status & FWO_SELECTED))
		DrawSelectionRect(hdc); //need to erase previous rect
	Draw(hdc);
	}

//////////////////////////////////////////////////////////
void FUNCINST::DrawSelectionRect(
	HDC hdc)
	{
	HDC hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, selectionBmp);
	BitBlt(hdc, location.x-FUNC_SELECTEDBORDER, location.y-FUNC_SELECTEDBORDER,
		FUNC_SELECTEDSIZE, FUNC_SELECTEDSIZE, hdcMem, 0, 0,
		SRCINVERT);
	DeleteDC(hdcMem);
	}

//////////////////////////////////////////////////////////
void FUNCINST::DrawDrag(
	HDC hdc,
	POINT offset)
	{
	HPEN oldPen;
	HBRUSH oldBrush;
	RECT   border;
	int oldMode;

	oldMode = SetROP2(hdc, R2_NOTXORPEN);
	oldPen = SelectPen(hdc, PROGSYSTEM::dottedDragPen);
	oldBrush = SelectBrush(hdc, GetStockBrush(NULL_BRUSH));
	GetSelectedRect((LPRECT)&border);
	OffsetRect((LPRECT)&border, offset.x, offset.y);
	Rectangle(hdc, border.left, border.top, border.right, border.bottom);
	SelectPen(hdc, oldPen);
	SelectBrush(hdc, oldBrush);
	SetROP2(hdc, oldMode);
	}

//////////////////////////////////////////////////////////
void FUNCINST::CalculateBounds()
	{
	int i;
	PARAM * temp;
	RECT paramBounds;

	boundsRect.left = location.x-HL_ARROWWIDTH;
	boundsRect.top = location.y-BORDER;
	boundsRect.right = boundsRect.left + FUNCSIZE+2*HL_ARROWWIDTH;
	boundsRect.bottom = boundsRect.top + FUNCSIZE + 2*BORDER;

	if(status & FN_INPUTS)
		{
		i = 0;
		while(0 != (temp = (PARAM *)GetObject(inputParams, i)))
			{
			temp->GetRect((LPRECT)&paramBounds);
			if(paramBounds.right + BORDER > boundsRect.right)
				boundsRect.right = paramBounds.right + BORDER;
			if(paramBounds.top - BORDER < boundsRect.top)
				boundsRect.top = paramBounds.top - BORDER;
			}
		}
	if(status & FN_OUTPUTS)
		{
		i = 0;
		while(0 != (temp = (PARAM *)GetObject(outputParams, i)))
			{
			temp->GetRect((LPRECT)&paramBounds);
			if(paramBounds.right + BORDER > boundsRect.right)
				boundsRect.right = paramBounds.right + BORDER;
			if(paramBounds.bottom + BORDER > boundsRect.bottom)
				boundsRect.bottom = paramBounds.bottom + BORDER;
			}
		}
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetTotalBounds(
	LPRECT bounds)
	{
	I_ASSERT(bounds);

	GetRect(bounds);
	GetUnionFlowBounds(bounds);
	GetUnionIncomingFlows(bounds);
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetUnionIncomingFlows(
	LPRECT bounds)
	{
	LPOBJINST objPtr;
	LPFUNCINST funcPtr;
	int i;
	RUNTIMEUID block;
	
	block = GetSubjectPtr()->GetObjectList();
	i=0;
	while(0 != (objPtr = (LPOBJINST)GetObject(block, i)))
		objPtr->GetBoundsOfIntersectingFlows(bounds, GetUID(this));

	block = GetSubjectPtr()->GetFunctionList();
	i=0;
	while(0 != (funcPtr = (LPFUNCINST)GetObject(block, i)))
		funcPtr->GetBoundsOfIntersectingFlows(bounds, GetUID(this));

	}

//////////////////////////////////////////////////////////
void FUNCINST::GetSelectedRect(
	LPRECT bounds)
	{
	bounds->left=location.x;
	bounds->top=location.y;
	bounds->bottom=bounds->top + FUNCSIZE;
	bounds->right=bounds->left + FUNCSIZE;
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetMaxScreenSize(
	LPRECT bounds)
	{
	GetSelectedRect(bounds);
	InflateRect(bounds, FUNC_SELECTEDBORDER, FUNC_SELECTEDBORDER);
	}

//////////////////////////////////////////////////////////
void FUNCINST::Move(
	POINT  offset)
	{
	location.x+=offset.x;
	location.y+=offset.y;
	ReCalculateParams(offset);
	}

//////////////////////////////////////////////////////////
int FUNCINST::PointInFlowArea(
	POINT pt)
	{
	RECT bounds;
	int result = 0;

	bounds.left = location.x - (2 * HL_ARROWWIDTH/3);
	bounds.right = bounds.left + HL_ARROWWIDTH;
	bounds.top = location.y + (FUNCSIZE - HL_ARROWHEIGHT)/2;
	bounds.bottom = bounds.top + HL_ARROWHEIGHT;

	if(PtInRect((LPRECT)&bounds,pt))
		result = FLOW_IN;
	else if(status & FN_FLOWOUT)
		{
		bounds.left = location.x + FUNCSIZE - (HL_ARROWWIDTH/3);
		bounds.right = bounds.left + HL_ARROWWIDTH;

		if(PtInRect((LPRECT)&bounds,pt))
			result = FLOW_OUT;
		}

	return result;
	}

//////////////////////////////////////////////////////////
BOOL FUNCINST::ObjectIntersectsRect(
			LPRECT bigRect)
	{
	RECT bounds;

	GetRect((LPRECT)&bounds);
	return IntersectRect((LPRECT)&bounds, (LPRECT)&bounds, bigRect);
	}


//////////////////////////////////////////////////////////
POINT FUNCINST::PointToArrowTip(
	POINT mouse)
	{
	POINT newPoint;

	if(mouse.x <= location.x + FUNCSIZE/2)
		newPoint.x = location.x - FUNCARROWWIDTH;
	else
		newPoint.x = location.x + FUNCSIZE + FUNCARROWWIDTH;

	newPoint.y = location.y + FUNCSIZE/2;

	return newPoint;
	}

//////////////////////////////////////////////////////////
POINT FUNCINST::GetArrowTip(
	int whichTip)
	{
	POINT pt;

	pt.y = location.y + FUNCSIZE/2;
	if(whichTip & FLOW_IN)
		pt.x = location.x - FUNCARROWWIDTH;
	else
		pt.x = location.x + FUNCSIZE + FUNCARROWWIDTH;

	return pt;
	}

//////////////////////////////////////////////////////////
void FUNCINST::ReCalculateParams(
	POINT offset)
	{
	int i=0;
	PARAM * temp;

	if(status & FN_INPUTS)
		{
		while(0 != (temp = (PARAM *)GetObject(inputParams, i)))
			{
			temp->ReCalculateRect(offset);
			}
		}
	i=0;
	if(status & FN_OUTPUTS)
		{
		while(0 != (temp = (PARAM *)GetObject(outputParams, i)))
			{
			temp->ReCalculateRect(offset);
			}
		}
	CalculateBounds();
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetHighlightBox(
	LPRECT box,
	int where)
	{
	if(where & FLOW_IN)
		{
		box->left = location.x - HL_DRAWARROWWIDTH;
		box->right = box->left + HL_DRAWARROWWIDTH;
		box->top = location.y + (FUNCSIZE - HL_DRAWARROWHEIGHT)/2;
		box->bottom = box->top + HL_DRAWARROWHEIGHT;
		}
	else if(where & FLOW_OUT)
		{
		box->left = location.x + FUNCSIZE;
		box->right = box->left + HL_DRAWARROWWIDTH;
		box->top = location.y + (FUNCSIZE - HL_DRAWARROWHEIGHT)/2;
		box->bottom = box->top + HL_DRAWARROWHEIGHT;
		}
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetUnionBoundsAndBox(
	LPRECT bounds,
	int where)
	{
	RECT rt;

	GetRect(&rt);
	UnionRect(bounds, bounds, &rt);
	GetHighlightBox(&rt, where);
	UnionRect(bounds, bounds, &rt);
	}

//////////////////////////////////////////////////////////
void FUNCINST::DrawHighlight(
	HDC hdc,
	int where)
	{
	int oldDC;
	RECT drawRect;

	oldDC = SaveDC(hdc);

	SelectObject(hdc, GetStockObject(BLACK_PEN));
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	SetROP2(hdc, R2_NOTXORPEN);

	GetHighlightBox(&drawRect, where);
	
	if(where)
		Rectangle(hdc, drawRect.left, drawRect.top, drawRect.right,
			drawRect.bottom);
	RestoreDC(hdc, oldDC);
	}

//////////////////////////////////////////////////////////
void FUNCINST::GetParams(
	BOOL figureBounds)
	{
	int numParams;
	int i;
	LPPARAMDESC paramDesc;

	if((numParams = infoPtr->GetInputsCount())>0)
		{
		inputParams = GetUID(new RECORDBLOCK(sizeof(PARAM), RT_PARAM));
		status|=FN_INPUTS;
		for(i=0;i<numParams;i++)
			{
			paramDesc = infoPtr->GetInput(i);
			if(paramDesc->TestOptionalFlag())
				{
				new(inputParams) PARAM(paramDesc,
					PAR_NOTLINKED|PAR_OPTIONAL|PAR_INPUT, GetUID(this),i,numParams-i, figureBounds);
				}
			else
				{
				new(inputParams) PARAM(paramDesc,
					PAR_NOTLINKED|PAR_REQUIRED|PAR_INPUT, GetUID(this), i,numParams-i, figureBounds);
				}
			}
		}

	if((numParams = infoPtr->GetOutputsCount())>0)
		{
		outputParams = GetUID(new RECORDBLOCK(sizeof(PARAM), RT_PARAM));
		status|=FN_OUTPUTS;
		for(i=0;i<numParams;i++)
			{
			paramDesc = infoPtr->GetOutput(i);
			if(paramDesc->TestOptionalFlag())
				{
				new(outputParams) PARAM(paramDesc,
					PAR_NOTLINKED|PAR_OPTIONAL|PAR_OUTPUT, GetUID(this),i,-(i+1), figureBounds);
				}
			else
				{
				new(outputParams) PARAM(paramDesc,
					PAR_NOTLINKED|PAR_REQUIRED|PAR_OUTPUT, GetUID(this),i ,-(i+1), figureBounds);
				}
			}
		}
	}

//////////////////////////////////////////////////////////
short FUNCINST::PostReadInit()
	{
	PARAM * paramPtr;
	int i = 0;
	char buffer[256];
	char idString[5];
	short result = PRJ_READNOERROR;
	short tempResult;
	static char msg[] = {"%s Parameter: #%i of function '%s' in subject: '%s' "}; 
	LPPROJECT projectPtr = GetSubjectPtr()->GetParentProject();
  
	infoPtr = cfgGetFunctionDesc(typeID);

	if(infoPtr)
		{
    if(TestTypeNeedsAddingFlag())
    	{
      if(!projectPtr->MarkRequiredType(infoPtr->pGetRequiredType()))
      	RequiredTypeNotFoundMessage(FALSE);
      }
      
		if(infoPtr->GetFlowsCount() > 0)
			status|=FN_FLOWOUT;
		else
			status &= ~FN_FLOWOUT;

		if(!RecreateParams())
			{
			if(status & FN_INPUTS)
				{
				while(!(result & PRJ_READERROR) &&
					(0 != (paramPtr = (PARAM *)GetObject(inputParams,i))))
					{
					tempResult = paramPtr->UpdateParamDesc(infoPtr);
					
					if(!tempResult & PRJ_READERROR)
          	{
            if(paramPtr->TestTypeNeedsAddingFlag())
            	{
              if(!projectPtr->MarkRequiredType(paramPtr->GetTypeID()))
			          ParamNotFoundMessage(paramPtr, FALSE); 
              	
              }
            }
            
					if(tempResult & PRJ_READWARNING)
						{
						wsprintf((LPSTR)buffer, msg, "Input", i, 
							infoPtr->pGetName(), GetSubjectPtr()->GetName());
						
						lstrcat(buffer, "does not match the configuration information.\r\n");
						
						MDI::AddMessage((LPSTR)buffer,TRUE);
						paramPtr->HardClearInfo();
						}
					else if(tempResult & PRJ_READBADPARAM)
						{
						wsprintf((LPSTR)buffer, msg, "Input", i, infoPtr->pGetName(),
							GetSubjectPtr()->GetName());

						lstrcat(buffer, "is invalid and caused the read to fail.");

						MDI::AddMessage((LPSTR)buffer, TRUE);
						}
					result |= tempResult;
					}
				}
			if(status & FN_OUTPUTS)
				{
				i=0;
				while(!(result & PRJ_READERROR) &&
					(0 != (paramPtr = (PARAM *)GetObject(outputParams, i))))
					{
					tempResult = paramPtr->UpdateParamDesc(infoPtr);
					if(!tempResult & PRJ_READERROR)
          	{
            if(paramPtr->TestTypeNeedsAddingFlag())
            	{
              if(!projectPtr->MarkRequiredType(paramPtr->GetTypeID()))
			          ParamNotFoundMessage(paramPtr, FALSE);               	
              }
            }
					if(tempResult & PRJ_READWARNING)
						{
						wsprintf((LPSTR)buffer, msg, "Output", i, 
							infoPtr->pGetName(), GetSubjectPtr()->GetName());
						
						lstrcat(buffer, "does not match the configuration information.\r\n");
						
						MDI::AddMessage((LPSTR)buffer,TRUE);
						paramPtr->HardClearInfo();
						}
					else if(tempResult & PRJ_READBADPARAM)
						{
						wsprintf((LPSTR)buffer, msg, "Output", i, infoPtr->pGetName(),
							GetSubjectPtr()->GetName());

						lstrcat(buffer, "is invalid and caused the read to fail.\r\n");

						MDI::AddMessage((LPSTR)buffer, TRUE);
						}
					result |= tempResult;
					}
				}
			}
		else
			{
			wsprintf((LPSTR)buffer, "The number of parameters changed for "
				"function '%s' in subject '%s'.\r\n",
				infoPtr->pGetName(), GetSubjectPtr()->GetName());

			MDI::AddMessage((LPSTR)buffer, TRUE);
			
			result = PRJ_READWARNING;
			}
		}
	else
		{
		CopyMemory((void *)idString, (void *)&typeID, sizeof(COMPUID));
		idString[4] = '\0';
		// these two lines swap the outer bytes and inner bytes
		// to reverse the uid character sequence.
		idString[0]^=idString[3]^=idString[0]^=idString[3];
		idString[1]^=idString[2]^=idString[1]^=idString[2];

		wsprintf((LPSTR)buffer, "A function with type ID: %s in subject '%s'"
			" could not be found in your configuration information.  As many"
			" parameter connections were preserved as possible.\r\n",
			(LPSTR)idString, GetSubjectPtr()->GetName());
		MDI::AddMessage(buffer, TRUE);
		result = PRJ_READWARNING | PRJ_READBADFUNC;
		}
	return result;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			FUNCINST::RecreateParams
//						 
// PARAMETERS:
//
// DESCRIPTION:		If the # of params in the config info differs
//								from the number of params in the record blocks,
//								recreate and copy over any necessary info.
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL FUNCINST::RecreateParams()
	{
	int inCount;
	int outCount;
	BOOL recreate = FALSE;
	RUNTIMEUID oldInputs;
	RUNTIMEUID oldOutputs;
	int i,j;
	LPPARAM oldParam;
	LPPARAM newParam;
	long oldStatus;
  LPPROJECT projectPtr = GetSubjectPtr()->GetParentProject();

	inCount= (status & FN_INPUTS)?(int)((LPRECORDBLOCK)GetObject(inputParams))->
		GetCount() : 0;

	outCount = (status & FN_OUTPUTS)?(int)((LPRECORDBLOCK)GetObject(outputParams))->
		GetCount() : 0;

	if((inCount != infoPtr->GetInputsCount()) || 
		(outCount != infoPtr->GetOutputsCount()))
		{
		recreate = TRUE;

		oldStatus = status;
		oldInputs = inputParams;
		oldOutputs = outputParams;

		status &= ~(FN_INPUTS | FN_OUTPUTS);

		//FALSE means don't recalculate the bounds
		GetParams(FALSE);
		MarkRequiredTypes(FALSE);
    
		//now try to copy new params over
		
		if(oldStatus & FN_INPUTS)
			{
			j=0;
			while(i=0, 0 != (oldParam = (LPPARAM)GetObject(oldInputs, i)))
				{
				if(0 != (newParam = (LPPARAM)GetObject(inputParams, j)))
					{
					if(newParam->Equivalent(oldParam))
						*newParam = *oldParam;
					}

				//we always want to clear the old type since we incremented
        //all the new types when we created the new params
				if(oldParam->GetTypeID() != NULL)
        	projectPtr->ClearRequiredType(oldParam->GetTypeID());
          
				delete oldParam;
				}
			}

		if(oldStatus & FN_OUTPUTS)
			{
			j=0;
			while(i=0, 0 != (oldParam = (LPPARAM)GetObject(oldOutputs, i)))
				{
				if(0 != (newParam = (LPPARAM)GetObject(outputParams, j)))
					{
					if(newParam->Equivalent(oldParam))
						*newParam = *oldParam;
					}
				if(oldParam->GetTypeID() != NULL)
        	projectPtr->ClearRequiredType(oldParam->GetTypeID());
          

				delete oldParam;
				}
			}
		
		}
		
	return recreate;
	}

//////////////////////////////////////////////////////////
PARAM * FUNCINST::ParamFromPoint(
	POINT pt)
	{
	PARAM * paramPtr = NULL;
	int i=0;

	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->PointInParam(pt))
				break;
			}
		}
	if(!paramPtr && (status & FN_OUTPUTS))
		{
		int i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(paramPtr->PointInParam(pt))
				break;
			}
		}

	return paramPtr;
	}

//////////////////////////////////////////////////////////
BOOL FUNCINST::PtInSelectionArea(
	POINT mouse)
	{
	RECT bounds;

	GetSelectedRect((LPRECT)&bounds);
	return PtInRect((LPRECT)&bounds, mouse);
	}

//////////////////////////////////////////////////////////
BOOL FUNCINST::UpdateParamNames(
	SERUID paramUID,
	BOOL isParam,
	LPRECT refreshRect)
	{
	PARAM * temp;
	int i=0;
	TSTRING newName;
	BOOL result = FALSE;
	RECT rt;

	if(isParam)
		newName = ((PARAM *)GetObject(paramUID))->GetName();
	else
		newName = ((OBJINST *)GetObject(paramUID))->GetName();

	//If a parameter name changed then we only need to update the input
	//parameters since only an output name can change
	if(status & FN_INPUTS)
		{
		while(0 != (temp = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramUID == temp->GetDataFromUID())
				{
				if(!result && refreshRect)
					GetRect(&rt); //store old bounds
					 
				temp->SetName(newName);
				result = TRUE;
				}
			}
		}

	//if isParam is false then we're updating because an object changed
	//so check the outputs as well
	if(!isParam && status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (temp = (PARAM *)GetObject(outputParams, i)))
			{
			if((paramUID == temp->GetDataFromUID()) && temp->TestObjConnectFlag())
				{
				if(!result && refreshRect)
					GetRect(&rt); //store old bounds
					 
				temp->SetName(newName);
				result = TRUE;
				}
			}
		}

	if(result)
		{
		CalculateBounds();

		if(refreshRect)
			{
			//make sure rt has been set to old bounds
			GetRect(refreshRect);
			UnionRect(refreshRect, refreshRect, &rt);
			}
		}

	return result;
	}

//////////////////////////////////////////////////////////
void FUNCINST::AddSelectedParamsToList(
	LIST * listPtr)
	{
	PARAM * paramPtr;
	int i=0;
	LPUIDLIST temp;

	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->IsSelected())
				{
				temp = new UIDLIST(GetUID(paramPtr));
				listPtr->Add(temp);
				}
			}
		}
	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(paramPtr->IsSelected())
				{
				temp = new UIDLIST(GetUID(paramPtr));
				listPtr->Add(temp);
				}
			}
		}
	}

//////////////////////////////////////////////////////////
void FUNCINST::AddLinkedInputsToList(
	HDC hdc,
	LIST * list,
	SERUID outputUID)
	{
	PARAM * temp;
	int i=0;
	LPUIDLIST member;

	if(status & FN_INPUTS)
		{
		while(0 != (temp = (PARAM *)GetObject(inputParams, i)))
			{
			if(temp->TestLinkFlag() && (temp->GetDataFromUID() == outputUID))
				{
				member = new UIDLIST(GetUID(temp));
				list->Add(member);
				temp->SetSelected(TRUE);
				temp->Draw(hdc, TRUE);
				}
			}
		}
	}


//////////////////////////////////////////////////////////
//returns TRUE if any params are added
BOOL FUNCINST::AddParamsInRectToList(
	HDC hdc,
	LPRECT rect,
	LIST * list)
	{
	PARAM * temp;
	int i=0;
	LPUIDLIST member;
	BOOL paramsAdded = FALSE;

	if(status & FN_INPUTS)
		{
		while(0 != (temp = (PARAM *)GetObject(inputParams, i)))
			{
			if(temp->ParamInRect(rect) && temp->TestLinkFlag() &&
				 !temp->IsSelected())
				{
        paramsAdded = TRUE;
				member = new UIDLIST(GetUID(temp));
				list->Add(member);
				temp->SetSelected(TRUE);
				if(hdc)
					temp->Draw(hdc, TRUE);
				}
			}
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (temp = (PARAM *)GetObject(outputParams, i)))
			{
			if(temp->ParamInRect(rect) && temp->TestLinkFlag() &&
				!temp->IsSelected())
				{
        paramsAdded = TRUE;
				member = new UIDLIST(GetUID(temp));
				list->Add(member);
				temp->SetSelected(TRUE);
				if(hdc)
					temp->Draw(hdc, TRUE);
				}
			}
		}

	return paramsAdded;
	}


//////////////////////////////////////////////////////////
void FUNCINST::Compile()
	{
	PARAM * paramPtr;
	RUNTIMEUID params;
	int i;
	LPEXEFUNCINST exeFuncPtr = (LPEXEFUNCINST)GetObject(copyUID);

	if(status & FN_INPUTS)
		{
		i=0;
		params = exeFuncPtr->GetInputParams();
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			paramPtr->copyUID = GetUID(new(params) EXEPARAM(
				paramPtr->GetStatus(),
				paramPtr->GetName(),
				paramPtr->GetTypeID(),
				paramPtr->GetDataFromUID()));
			}
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		params = exeFuncPtr->GetOutputParams();
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			paramPtr->copyUID = GetUID(new(params) EXEPARAM(
				paramPtr->GetStatus(),
				paramPtr->GetName(),
				paramPtr->GetTypeID(),
				paramPtr->GetDataFromUID()));

			}
		}

	CompileFlows(exeFuncPtr->GetFlowList());

 // Let the library list know which DLL/entryPoint should be
 // associated with this FUNCINST at runtime

  EXELIBLIST::BindUIDToExeProc(copyUID,
                               NULL,
                               infoPtr->GetLibName(),
                               infoPtr->pGetEntryPointName()
                              );

	}

//////////////////////////////////////////////////////////
void FUNCINST::UpdateUIDs()
	{
	PARAM * paramPtr;
	int i = 0;

	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			paramPtr->UpdateUIDs();
			}
		}
	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			paramPtr->UpdateUIDs();
			}
		}

	FLOWMGR::UpdateUIDs();

	}

//////////////////////////////////////////////////////////
void FUNCINST::ClearInputs(
	RUNTIMEUID clipParamBlock,
	BOOL       clearAll)
	{
	PARAM * paramPtr;
	PARAM * linkParamPtr;
	FUNCINST * linkFuncPtr;
	OBJINST * linkObjPtr;
	int i = 0;
	LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->TestLinkFlag())
				{
				linkParamPtr = NULL;
				linkObjPtr = NULL;
				linkFuncPtr=NULL;
				if(paramPtr->TestParamConnectFlag())
					{
					linkParamPtr = (PARAM *)GetObject(paramPtr->GetDataFromUID());
					linkFuncPtr = (FUNCINST *)GetObject(linkParamPtr->ownerFunc);
					}
				else if(paramPtr->TestObjConnectFlag())
					linkObjPtr = (OBJINST *)GetObject(paramPtr->GetDataFromUID());

				if(paramPtr->IsSelected() || (linkFuncPtr &&
					linkFuncPtr->IsSelected()) || (linkParamPtr &&
					(linkParamPtr->TestDeletedFlag() || linkParamPtr->IsSelected()))
					|| this->IsSelected() ||
					(linkObjPtr && (linkObjPtr->TestSelectFlag() ||
						linkObjPtr->GetGroupPtr()->IsSelected())) || clearAll)
					{
					if(linkParamPtr && !(linkParamPtr->TestDeletedFlag()))
						{
						if(linkParamPtr->GetConnectCount() == 1)
							{
							PROGSYSTEM::PushAndRegisterContext(holdContext);
							linkParamPtr->copyUID = GetUID(
								new(clipParamBlock) CLIPPARAM(linkParamPtr, clearAll));
							holdContext = PROGSYSTEM::PopAndRegisterContext();
							}
						linkParamPtr->ClearInfo();
						}
					PROGSYSTEM::PushAndRegisterContext(holdContext);
					paramPtr->copyUID = GetUID(
						new(clipParamBlock) CLIPPARAM(paramPtr, clearAll));
					holdContext = PROGSYSTEM::PopAndRegisterContext();
					paramPtr->ClearInfo();
					}
				}
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


//////////////////////////////////////////////////////////
void FUNCINST::ClearOutputs(
  RUNTIMEUID clipParamBlock,
	BOOL       clearAll)
	{
	PARAM * paramPtr;
	int i = 0;
	LPFILEOBJECT holdContext;
	OBJINST * linkObjPtr;

	holdContext = PROGSYSTEM::PopAndRegisterContext();

	if(status & FN_OUTPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(paramPtr->TestLinkFlag())
				{
				if(paramPtr->TestObjConnectFlag())
					linkObjPtr = (OBJINST *)GetObject(paramPtr->GetDataFromUID());
				else
					linkObjPtr = NULL;

				if(paramPtr->IsSelected() || this->IsSelected() ||
					(linkObjPtr && (linkObjPtr->TestSelectFlag() ||
					linkObjPtr->GetGroupPtr()->IsSelected())) || clearAll)
					{
					PROGSYSTEM::PushAndRegisterContext(holdContext);
					paramPtr->copyUID = GetUID(
						new(clipParamBlock) CLIPPARAM(paramPtr, clearAll));
					holdContext = PROGSYSTEM::PopAndRegisterContext();
					paramPtr->ClearInfo();
					}
				}
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}

//////////////////////////////////////////////////////////
// copies qualified inputs as well as outputs that are
// connected to qualified inputs.
void FUNCINST::CopyInputs(
	RUNTIMEUID clipParamBlock,
	BOOL       copyAll)
	{
	PARAM * paramPtr;
	PARAM * linkParamPtr;
	FUNCINST * linkFuncPtr;
	OBJINST * linkObjPtr;
	int i = 0;
	LPFILEOBJECT holdContext;

	holdContext = PROGSYSTEM::PopAndRegisterContext();
	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->TestLinkFlag())
				{
				linkObjPtr = NULL;
				linkFuncPtr=NULL;
				if(paramPtr->TestParamConnectFlag())
					{
					linkParamPtr = (PARAM *)GetObject(paramPtr->GetDataFromUID());
					linkFuncPtr = (FUNCINST *)GetObject(linkParamPtr->ownerFunc);
					}
				else if(paramPtr->TestObjConnectFlag())
					linkObjPtr = (OBJINST *)GetObject(paramPtr->GetDataFromUID());

				if(copyAll || (this->IsSelected() && paramPtr->IsSelected() &&
					(paramPtr->TestConstantFlag() ||
					(linkFuncPtr && linkFuncPtr->IsSelected()) ||
					(linkObjPtr && (linkObjPtr->TestSelectFlag() ||
					linkObjPtr->GetGroupPtr()->IsSelected())))))
					{
					PROGSYSTEM::PushAndRegisterContext(holdContext);
					new(clipParamBlock) CLIPPARAM(paramPtr, copyAll);
					holdContext = PROGSYSTEM::PopAndRegisterContext();
					}
				}
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


//////////////////////////////////////////////////////////
void FUNCINST::CopyOutputs(
	RUNTIMEUID clipParamBlock,
	BOOL       copyAll)
	{
	PARAM * paramPtr;
	int i = 0;
	LPFILEOBJECT holdContext;
	OBJINST * linkObjPtr;

	holdContext = PROGSYSTEM::PopAndRegisterContext();

	if(status & FN_OUTPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(paramPtr->TestLinkFlag())
				{
				if(paramPtr->TestObjConnectFlag())
					linkObjPtr = (OBJINST *)GetObject(paramPtr->GetDataFromUID());
				else
					linkObjPtr = NULL;

				if(copyAll || (paramPtr->IsSelected() && this->IsSelected() &&
					(linkObjPtr && (linkObjPtr->TestSelectFlag() ||
						linkObjPtr->GetGroupPtr()->IsSelected()))))
					{
					PROGSYSTEM::PushAndRegisterContext(holdContext);
					new(clipParamBlock) CLIPPARAM(paramPtr, copyAll);
					holdContext = PROGSYSTEM::PopAndRegisterContext();
					}
				}
			}
		}
	PROGSYSTEM::PushAndRegisterContext(holdContext);
	}


//////////////////////////////////////////////////////////
// should only be called to push a function's flows
void FUNCINST::PushPrintFlows(
	PRINTFLOWSTACK * stackPtr,
	int indent)
	{
	FLOW *   flowPtr;
	RUNTIMEUID  flowList = GetFlowList();
	int         numsigs = GetFlowCount();
	int         count =infoPtr->GetFlowsCount();
	int         i=0;
	int         j;

	if((numsigs>0 && count>1) || numsigs>1)
		{
		j=1;
		stackPtr->PushFlow(indent+j, (FLOW *)FN_DRAWBRACKET);
		}
	else
		j=0;

	// push signals twice so on second time, closing bracket will be printed
	for(i=numsigs-1; i>-1; i--)
		{
		if((flowPtr = (FLOW *)GetObject(flowList, i))!=NULL)
			{
			//clean status bits - they should not be dirty but clean them just
			//in case.
			flowPtr->ClearPrintingFlags();
			stackPtr->PushFlow(indent + j, flowPtr);
			stackPtr->PushFlow(indent + j, flowPtr);
			if(j==1)
				flowPtr->SetPrintFlag();
			}
		i--;//needed to offset the fact that i is incremented in GetObject
		}
	if(j==1)
		stackPtr->PushFlow(-(indent+j), (FLOW *)FN_DRAWBRACKET);

	}

//////////////////////////////////////////////////////////
void FUNCINST::DoPrint(
	LPPRINTER printPtr,
	PRINTFLOWSTACK * stackPtr,
	int indent)
	{
	LPSTR buffer;
	int size;
	PARAM * paramPtr;
	int i;
	LPSTR typeName = cfgGetFunctionCategoryName(typeID);

	I_ALWAYSASSERT(typeName, return);

	if(!TestPrintVisitedFlag())
		{
		size = 	lstrlen((LPSTR)infoPtr->pGetName()) + lstrlen(typeName) + 20;

		buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, size);
		if(TestPrintLabelFlag())
			printPtr->PrintLabel(PROJECT::GetLabelIndent(),
				PROJECT::GetPrintLabelPtr()->RetrieveLabel(this));
		wsprintf(buffer, (LPSTR)"%s [%s function]", infoPtr->pGetName(),
			typeName);
		printPtr->PrintLine(indent, buffer);
		GlobalFree(buffer);
		if(status & FN_INPUTS)
			{
			i=0;
			while((paramPtr = (PARAM *)GetObject(inputParams, i))!=NULL)
				paramPtr->DoPrint(printPtr, indent+1);
			}
		if(status & FN_OUTPUTS)
			{
			i=0;
			while((paramPtr = (PARAM *)GetObject(outputParams, i))!=NULL)
				paramPtr->DoPrint(printPtr, indent+1);
			}
		PushPrintFlows(stackPtr, indent);
		SetPrintVisitedFlag();
		}
	else
		{
		buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, 32);
		wsprintf(buffer, (LPSTR)"<Goto %i.>",
			PROJECT::GetPrintLabelPtr()->RetrieveLabel(this));
		printPtr->PrintLine(indent, buffer);
		GlobalFree(buffer);
		}
	}


//////////////////////////////////////////////////////////
void FUNCINST::CleanPrint()
	{
	ClearPrintVisitedFlag();
	ClearPrintLabelFlag();
	ClearLabelVisitedFlag();
	}

void FUNCINST::BuildLabels(
	PRINTFLOWSTACK * stackPtr)
	{
	if(TestLabelVisitedFlag())
		{
		if(!TestPrintLabelFlag())
			{
			SetPrintLabelFlag();
			PROJECT::GetPrintLabelPtr()->NewLabel(this);
			}
		}
	else
		{
		SetLabelVisitedFlag();
		PushPrintFlows(stackPtr, 0);
		}
	}

//////////////////////////////////////////////////////////
void FUNCINST::PrintBracket(
	LPPRINTER printPtr,
	int indent)
	{
	if(indent < 0)
		printPtr->PrintLine(-indent, (LPSTR)"{");
	else
		printPtr->PrintLine(indent, (LPSTR)"}");
	}


//////////////////////////////////////////////////////////
BOOL FUNCINST::MoveIfSelected(
	POINT offset,
	LPFLOWSTACK flowStack)
	{
	BOOL result = TRUE;

	if(!TestChainVisitedFlag())
		{
		if(IsSelected())
			Move(offset);

		SetChainVisitedFlag();
		PushFlows(flowStack);
		}
	return result;
	}


//////////////////////////////////////////////////////////
void FUNCINST::PushAndSelectFuncChain(
	HDC hdc,
	LPLIST selectedFuncs,
	LPLIST /*selectedFlows*/,
	LPFLOWSTACK flowStack)
	{
	LPUIDLIST member;

	if(!TestChainVisitedFlag())
		{
		if(!IsSelected())
    	{
			Select(hdc);
			member = new UIDLIST(GetUID(this));
			selectedFuncs->Add(member);
      }
		PushFlows(flowStack);
		SetChainVisitedFlag();
		}
	}


//////////////////////////////////////////////////////////
void FUNCINST::SelectFuncChain(
	HDC hdc,
	LIST * selectedFuncs,
	LIST * selectedFlows)
	{
	LPFLOWSTACK flowStack;
	LPFLOW flowPtr;
  LPUIDLIST member;

	flowStack = new FLOWSTACK();

	PushAndSelectFuncChain(hdc, selectedFuncs, selectedFlows, flowStack);
	while(NULL != (flowPtr = flowStack->Pop()))
		{
		if(!flowPtr->IsSelected())
			{
			member = new UIDLIST(GetUID(flowPtr));
			selectedFlows->Add(member);
			flowPtr->SetSelected(TRUE);
			flowPtr->DrawSignalRect(hdc);
			}

		flowPtr->GetDestinationFunc()->
			PushAndSelectFuncChain(hdc, selectedFuncs, selectedFlows, flowStack);
		}

  delete flowStack;
	}   


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  CheckFunction
//
// CLASS.......:  FUNCINST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
short FUNCINST::CheckFunction(
	short checkType)
	{
	PARAM * paramPtr;
	int        i;
	short				rVal = FUNC_CHECK_NOERROR;

	if(status & FN_INPUTS)
		{
		i = 0;
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(!paramPtr->TestLinkFlag() && paramPtr->TestRequiredFlag())
				{
				if(checkType & FUNC_CHECK_UNLINKED)
					rVal |= FUNC_CHECK_UNLINKED;
				}
			else if(paramPtr->TestParamConnectFlag())
				{
				if(checkType & FUNC_CHECK_INBEFOREOUT)
					{
					if(!paramPtr->TestInitializedInputFlag())
						rVal |= FUNC_CHECK_INBEFOREOUT;
					else
						paramPtr->ClearInitializedInputFlag();
					}
				}			
			}
    }

	if(checkType & FUNC_CHECK_UNLINKED)
		{ 
		if(status & FN_OUTPUTS)
			{
			i=0;
			while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
				if(!paramPtr->TestLinkFlag() && paramPtr->TestRequiredFlag())
					{
					rVal |= FUNC_CHECK_UNLINKED;
					break;
					}
			}
		}

	return rVal;

	}

//////////////////////////////////////////////////////////
BOOL FUNCINST::AddObjectToZlist(
	LPRECT area,
	LPZLIST zlistPtr)
	{
	PARAM * paramPtr;
	int i;
	RECT rect;

	if(ObjectIntersectsRect(area))
		{
		GetSelectedRect((LPRECT)&rect);
		zlistPtr->Add((LPRECT)&rect, Z_FUNC, GetUID(this));
		//add flow areas

		rect.left = location.x - (2 * HL_ARROWWIDTH/3);
		rect.right = rect.left + HL_ARROWWIDTH;
		rect.top = location.y + (FUNCSIZE - HL_ARROWHEIGHT)/2;
		rect.bottom = rect.top + HL_ARROWHEIGHT;
		zlistPtr->Add((LPRECT)&rect, Z_INFUNC, GetUID(this));

		if(status & FN_FLOWOUT)
			{
			rect.left = location.x + FUNCSIZE - (HL_ARROWWIDTH/3);
			rect.right = rect.left + HL_ARROWWIDTH;
			zlistPtr->Add((LPRECT)&rect, Z_OUTFUNC, GetUID(this));
			}

		if(status & FN_INPUTS)
			{
			i = 0;
			while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
				{
				paramPtr->GetRect((LPRECT)&rect);
				zlistPtr->Add((LPRECT)&rect, Z_PARAM, GetUID(paramPtr));
				}
			}

		if(status & FN_OUTPUTS)
			{
			i=0;
			while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
				{
				paramPtr->GetRect((LPRECT)&rect);
				zlistPtr->Add((LPRECT)&rect, Z_PARAM, GetUID(paramPtr));
				}
			}

		return TRUE;
		}
	return FALSE;
	}

//////////////////////////////////////////////////////////
void FUNCINST::SetLocation(POINT p)
	{
	POINT offset;

	offset.x = p.x - location.x;
	offset.y = p.y - location.y;
	location = p;
	ReCalculateParams(offset);
	}

//////////////////////////////////////////////////////////
void FUNCINST::UnselectParams()
	{
	PARAM * paramPtr;
	int        i;

	if(status & FN_INPUTS)
		{
		i = 0;
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			if(paramPtr->IsSelected())
				paramPtr->SetSelected(FALSE);
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			if(paramPtr->IsSelected())
				paramPtr->SetSelected(FALSE);
		}

	CalculateBounds();

	}

//////////////////////////////////////////////////////////
WORD FUNCINST::ExportUPSF()
	{
	int exportCount;
	int i;
	PARAM * paramPtr;
	long result;
	WORD returnValue = EXPORT_NOERROR;

	PROJECT::AddUpsfFuncType(typeID, infoPtr->pGetName());

	I_ALWAYSASSERT(AUpsfExportField(FUNCTION_TYPE, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&typeID, (long)sizeof(COMPUID)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//export x coordinate
	I_ALWAYSASSERT(AUpsfExportField(FUNCTION_LOCATION, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.x, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);

	//export y coordinate
	I_ALWAYSASSERT(AUpsfExportField(FUNCTION_LOCATION, 1L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.y, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);

	exportCount = 0;
	if(status & FN_INPUTS)
		{
		i=0;
		while(!(returnValue & EXPORT_OUTOFDISK)
			&& 0!=(paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(AUpsfCreateScope(FUNCTION_INPUTDATA, (long)exportCount) == AUPSF_NOERROR)
				{
				exportCount++;
				returnValue |= paramPtr->ExportUPSF();
				AUpsfCloseScope();
				}
			else
				{
				returnValue|=EXPORT_GENERALERROR;
				I_PRINT("Error creating input scope.");
				}
			}
		}

	I_ALWAYSASSERT(AUpsfExportField(FUNCTION_INPUTCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	exportCount = 0;
	if(status & FN_OUTPUTS)
		{
		i=0;
		while(!(returnValue & EXPORT_OUTOFDISK) &&
			0!=(paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(AUpsfCreateScope(FUNCTION_OUTPUTDATA, (long)exportCount) == AUPSF_NOERROR)
				{
				exportCount++;
				returnValue |= paramPtr->ExportUPSF();
				AUpsfCloseScope();
				}
			else
				{
				returnValue|=EXPORT_GENERALERROR;
				I_PRINT("Error creating output scope.");
				}
			}
		}

	I_ALWAYSASSERT(AUpsfExportField(FUNCTION_OUTPUTCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);


	//make call to export flows
	returnValue |= FlowExportUPSF(FUNCTION_FLOWDATA, (int *)&exportCount);

	//do final check to see if we're out of disk space
	result = AUpsfExportField(FUNCTION_FLOWCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int));

	I_ASSERT(result == AUPSF_NOERROR);

	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	PROJECT::IncStatusBar();

	return returnValue;
	}


//////////////////////////////////////////////////////////
BOOL FUNCINST::ImportUPSF()
	{
	int paramCount;
	int i;
	long result;

	// read in flows
	//TRUE means a function
	if(!FlowImportUPSF(FUNCTION_FLOWDATA, FUNCTION_FLOWCOUNT, GetUID(this), TRUE))
		return FALSE;

	//read in inputs

	I_ALWAYSASSERT((result = AUpsfImportField(FUNCTION_INPUTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&paramCount, (long)sizeof(int))) !=
		AUPSF_MEMORYERROR, return FALSE);

	if(result != AUPSF_NOERROR)
		{
		paramCount = 0;
		apb_LogUpsfError(result, "input parameter count");
		}
	else if(PROJECT::UpsfPreAppwareImportVersion())
		paramCount++;


	for(i=0;i<paramCount;i++)
		{
		I_ALWAYSASSERT((result = AUpsfOpenScope(FUNCTION_INPUTDATA, (long)i))
			!= AUPSF_FILEERROR, return FALSE);

		if(result == AUPSF_NOERROR)
			{
			PROJECT::PushScopeStack(ST_FULL);

			if(!ImportUpsfInputParamData(i))
				return FALSE;
			
			PROJECT::PopScopeStack();
			AUpsfCloseScope();
			}
		else
			{
			apb_LogUpsfError(result, "function's input parameter scope", APB_UPSFOPEN);
			}
		}


	//read in outputs

	I_ALWAYSASSERT((result = AUpsfImportField(FUNCTION_OUTPUTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&paramCount, (long)sizeof(int))) !=
		AUPSF_MEMORYERROR, return FALSE);

	if(result != AUPSF_NOERROR)
		{
		paramCount = 0;
		apb_LogUpsfError(result, "output parameter count");
		}
	else if(PROJECT::UpsfPreAppwareImportVersion())
		paramCount++;

	for(i=0;i<paramCount;i++)
		{
		I_ALWAYSASSERT((result = AUpsfOpenScope(FUNCTION_OUTPUTDATA, (long)i))
			!= AUPSF_FILEERROR, return FALSE);

		if(result == AUPSF_NOERROR)
			{
			PROJECT::PushScopeStack(ST_FULL);

			if(!ImportUpsfOutputParamData(i))
				return FALSE;
			
			PROJECT::PopScopeStack();
			AUpsfCloseScope();
			}
		else
			{
			apb_LogUpsfError(result, "function's output parameter scope", APB_UPSFOPEN);
			}
		}

	return TRUE;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			FUNCINST::ImportUpsfOutputParamData
//						 
// PARAMETERS:
//
// DESCRIPTION:		If an output is connected to an input we do not need to do anything
//								since the parameter would have been linked when we went through
// 								the inputs.
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL FUNCINST::ImportUpsfOutputParamData(
	int index)
	{
	long paramType;
	long result;
	SERUID linkUID;
	int tempInt;
	PARAM * paramPtr;
	char buffer[256];

	//Get pointer to the parameter were importing
	paramPtr = GetParam(-(index+1));
	
	if(!paramPtr)
		{
		wsprintf(buffer, "Error importing output parameter #%d for"
			"  function of type: %s.",-(index + 1), infoPtr->pGetName());

		AUpsfLogComment(buffer);
		}
	else
		{
		// read in the param type
		I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_CONNECTTYPE, 0L, AUPSF_NATV_LONG,
			(LPVOID)&paramType, (long)sizeof(long))) != AUPSF_MEMORYERROR,
			return FALSE);

		if(result != AUPSF_NOERROR)
			apb_LogUpsfError(result, "output parameter's connection type");
		else if(paramType == 1)
			{
			//only want to know if we're connected to an object since
			//ouput parameters connected to inputs will have been
			//set up when input parameters were imported.
			
			I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_CONNECTINDEX, 0L, 
				AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) !=
				AUPSF_MEMORYERROR, return FALSE);

			if(result != AUPSF_NOERROR)
				apb_LogUpsfError(result, "output parameter's connection index");
			else
				{
				//linked to an object so get uid of that object
				linkUID = GetSubjectPtr()->UPSFIndexToObj(tempInt);
				
				if(linkUID == NULLSERUID)
					AUpsfLogComment("Invalid object connection index for an output parameter.");
				else
					{
					paramPtr->AddInfo(PAR_FROMOBJ);
					//tell the parameter the uid of the link
					paramPtr->AddLinkUID(linkUID);

					//set name to that of the object
					paramPtr->SetName(((LPOBJINST)GetObject(linkUID))->GetName());
					}
				}
			}	//else if paramType == 1
		}	 //else (if the paramPtr is valid)

	return TRUE; //no error if we make it to here
	}	
 

/////////////////////////////////////////////////////////////
// FUNCTION:			FUNCINST::ImportUpsfInputParamData
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL FUNCINST::ImportUpsfInputParamData(
	int index)
	{
	long paramType;
	long result;
	SERUID linkUID;
	int tempInt;
	FUNCINST * dstFuncPtr;
	PARAM * dstParamPtr;
	PARAM * paramPtr;
	char buffer[256];

	//Get pointer to the parameter were importing
	paramPtr = GetParam(index);

	if(!paramPtr)
		{
		wsprintf(buffer, "Error importing input parameter #%d for"
			"  function of type: %s.",index+1, infoPtr->pGetName());
			AUpsfLogComment(buffer);
		}
	else
		{

		dstParamPtr = NULL; //initialize since a check will be made later
												//to see if it has a value or not.
	
		// read in the param type
		I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_CONNECTTYPE, 
			0L, AUPSF_NATV_LONG, (LPVOID)&paramType, (long)sizeof(long))) !=
			AUPSF_MEMORYERROR, return FALSE);
		
		if(result != AUPSF_NOERROR)
			apb_LogUpsfError(result, "parameter connection type field");
		else
			{
			if(paramType == 1 || paramType == 4)
				{
				//we're connected to an object (1) or a parameter (4)
				//get index of the object or owner function of the parameter
				
				I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_CONNECTINDEX, 0L, 
					AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) !=
					AUPSF_MEMORYERROR, return FALSE);
					
				if(result != AUPSF_NOERROR)
					apb_LogUpsfError(result, "input parameter connection index field");
				else
					{
					if(paramType == 1)
						{
						//linked to an object so get uid of that object
						linkUID = GetSubjectPtr()->UPSFIndexToObj(tempInt);
						
						if(linkUID == NULLSERUID)
							AUpsfLogComment((LPSTR)"Invalid object connection index for input parameter.");
						else
							{
							paramPtr->AddInfo(PAR_FROMOBJ);
							
							//get name from the object in case the object had to use a 
							//default name.
							paramPtr->SetName(((LPOBJINST)GetObject(linkUID))->GetName());
							}
						}
					else	//we're connected to a parameter
						{
						//linked to a function so get ptr to that function
						linkUID = GetSubjectPtr()->UPSFIndexToFunc(tempInt);
            if(linkUID != NULLSERUID)
            	{
              dstFuncPtr = (FUNCINST*)GetObject(linkUID);
              }
           	else
            	dstFuncPtr = NULL;
						
						linkUID = NULLSERUID;

						if(dstFuncPtr)
							{
							//read in index of parameter
							I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_PARAM, 0L, 
								AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) !=
								AUPSF_MEMORYERROR, return FALSE);
							
							if(result != AUPSF_NOERROR)
								{
								apb_LogUpsfError(result, "output connection index "
									"for input parameter");
								}
							else
								{
								//get ptr to the destination parameter
								dstParamPtr = dstFuncPtr->GetParam(tempInt);
							
								if(!dstParamPtr)
									{
									AUpsfLogComment("Invalid output connection index "
										"for input parameter.");
									}
								else
									{
									linkUID = GetUID(dstParamPtr);
									dstParamPtr->AddInfo(PAR_FROMPARAM);
									paramPtr->AddInfo(PAR_FROMPARAM);
									}
								}
							}	//if(dstFunction)
						else
							{
							AUpsfLogComment("Invalid function connection index for input parameter.");
							}

						}	//end of else for being connected to a parameter
						
					//tell the parameter the uid of the link
					if(linkUID != NULLSERUID)
						paramPtr->AddLinkUID(linkUID);
											
					}

				}	//if(paramType == 1 || paramType == 4

			else if(paramType == 3)
				paramPtr->AddInfo(PM_CONSTANT);

			} //end else for checking the the paramType was imported.

		if(paramType != 0 && paramType != 1)
			{
			I_ALWAYSASSERT((result = AUpsfImportField(PARAMETER_NAME, 0L, 
				AUPSF_NATV_STRING, (LPVOID)buffer, (long)sizeof(buffer))) !=
				AUPSF_MEMORYERROR, return FALSE);

			if(result == AUPSF_NOERROR)
				paramPtr->SetName(buffer);
			else
				{
				apb_LogUpsfError(result, "input parameter's name");
				//give parameter the default warning name
				paramPtr->SetName((LPSTR)PARAM_DEFAULTWARNING);
				}

			//if were hooked to an output, set the name of the output to be
			//that of the input.
			if(dstParamPtr)
				dstParamPtr->SetName(paramPtr->GetName());
			}
		}

	return TRUE; //no error if we made it to here
	}	
										

//////////////////////////////////////////////////////////
//Clears all params in the function as bad - we want to do this if the
//function doesn't match the config info.
void FUNCINST::MarkAllParamsBad()
	{
	PARAM * paramPtr;
	int        i;

	if(status & FN_INPUTS)
		{
		i = 0;
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			paramPtr->SetBadParamFlag();
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			paramPtr->SetBadParamFlag();
		}

	}


//////////////////////////////////////////////////////////
//Checks all inputs connected to outputs to make sure the outputs are exist
//and are not marked as bad.
//Returns PRJ_READNOERROR if all parameters were connected to good parameters
short FUNCINST::ValidateParams()
	{
	PARAM * paramPtr;
	int i;
	PARAM * dstParam;
	short result = PRJ_READNOERROR;
	char buffer[256];
	int numParams;

	if(status & FN_INPUTS)
		{
		i=0;
		numParams = (int)((RECORDBLOCK *)GetObject(inputParams))->GetCount();
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->TestParamConnectFlag())
				{
				dstParam = (PARAM *)GetObject(paramPtr->GetDataFromUID());
				//Check to see if the destination parameter exists or is bad
				if(!dstParam || dstParam->TestBadParamFlag())
					{
					paramPtr->HardClearInfo();
					wsprintf((LPSTR)buffer, "Input parameter #%d called '%s' in "
						"function '%s' of subject '%s' was connected to an invalid "
						"output and has been cleared.\r\n", i, paramPtr->GetName(),
						infoPtr->pGetName(), GetSubjectPtr()->GetName());
					MDI::AddMessage(buffer, TRUE);
					result = PRJ_READWARNING;
					}
				}
			else if(paramPtr->TestObjConnectFlag())
				{
				//Check to see if the object exists or is bad
				if(!GetObject(paramPtr->GetDataFromUID()))
					{
					paramPtr->ClearInfo();
					wsprintf((LPSTR)buffer, "Input parameter #%d called '%s' in "
						"function '%s' of subject '%s' was connected to a nonexistant "
						"object and has been cleared.\r\n", i, paramPtr->GetName(),
						infoPtr->pGetName(), GetSubjectPtr()->GetName());
					MDI::AddMessage(buffer, TRUE);
					result = PRJ_READWARNING;
					}
				}

			//subtract an additional 1 from i since i has been incremented
			paramPtr->UpdateLocation((int)(numParams-(i-1)), this);
			}
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			if(paramPtr->TestObjConnectFlag())
				{
				//Check to see if the destination object exists or is bad
				if(!GetObject(paramPtr->GetDataFromUID()))
					{
					paramPtr->ClearInfo();
					wsprintf((LPSTR)buffer, "Output parameter #%d called '%s' in "
						"function '%s' of subject '%s' was connected to a nonexistant "
						"object and has been cleared.\r\n", i, paramPtr->GetName(),
						infoPtr->pGetName(), GetSubjectPtr()->GetName());
					MDI::AddMessage(buffer, TRUE);
					result = PRJ_READWARNING;
					}
				}

			paramPtr->UpdateLocation(-i, this);			
			}
		}

	CalculateBounds();

	return result;
	}


//////////////////////////////////////////////////////////
//returns TRUE if at least one space is founc before the null terminator.
BOOL FUNCINST::HasSpaces(
	LPSTR theString)
	{
	LPSTR strng;

	strng = theString;
	while(*strng != '\0' && *strng != ' ')
		strng++;

	if(*strng == ' ')
		return TRUE;
	else
		return FALSE;

	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PARAM::GetNextParam
//						 
// PARAMETERS:
//
// DESCRIPTION:		Increments the index after retrieving the parameter
//								If the parameter is an input and there are no more
//								inputs, the index is incremented to point to the
//								first output.
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPPARAM FUNCINST::GetNextParam(
	int * indexPtr)
	{
	LPPARAM paramPtr = GetParam(*indexPtr);

	if(!paramPtr)
		{
		if(*indexPtr >= 0)
			*indexPtr = -1;
		
		paramPtr = GetParam(*indexPtr);
		if(paramPtr)
			(*indexPtr)--;	
		}
	else
		{
		if(*indexPtr >= 0)
			(*indexPtr)++;
		else
			(*indexPtr)--;
		}

	return paramPtr;
	} 

//////////////////////////////////////////////////////////
BOOL FUNCINST::PostUPSFImport()
	{
	return FlowPostUPSFImport();
	}																		 

////////////////////////////////////////////////////////////
void FUNCINST::MarkUninitParams()
	{
	int i;
	LPPARAM paramPtr;

	if(!TestChainVisitedFlag())
		{
		if(status & FN_OUTPUTS)
			{
			i=0;
			while(NULL != (paramPtr = (LPPARAM)GetObject(outputParams, i)))
				{
				MarkInputsAfterOutput(paramPtr);
				}
			}
		}
	}

////////////////////////////////////////////////////////////
void FUNCINST::MarkInputsAfterOutput(
	LPPARAM outputPtr)
	{
	LPFLOWSTACK flowStack;
	LPVOIDSTACK chainedStack;	//keeps track of functions we've visited
	LPFLOW			flowPtr;
	SERUID			outputUID;
	LPFUNCINST  funcPtr;
	
	if(outputPtr->TestParamConnectFlag() && !outputPtr->IsInput())
		{
		outputUID = GetUID(outputPtr); 
		flowStack = new FLOWSTACK();
		chainedStack = new FLOWSTACK();

		//Get the stack Going
		PushFlows(flowStack);
		
		//If the input is in the output func, it's still invalid so make sure
		//we don't visit this func.
		SetChainVisitedFlag();
		chainedStack->Push(this);
		
		while(NULL != (flowPtr = flowStack->Pop()))
			{
			funcPtr = (LPFUNCINST)GetObject(flowPtr->GetDestinationUID());
			I_ASSERT(funcPtr);
			if(funcPtr && !funcPtr->TestChainVisitedFlag())
				{
				funcPtr->MaybeMarkInput(outputUID);
				
				funcPtr->PushFlows(flowStack);
				chainedStack->Push(funcPtr);
				funcPtr->SetChainVisitedFlag();
				}
			}

		//clean the stacks
		while(flowStack->Pop());

		while(NULL != (funcPtr = (LPFUNCINST)chainedStack->Pop()))
			funcPtr->ClearChainVisitedFlag();

		delete flowStack;
		delete chainedStack;
		}
	}

/////////////////////////////////////////////////////////////
void FUNCINST::MaybeMarkInput(
	SERUID outputUID)
	{
	int i=0;
	LPPARAM paramPtr;

	if(status & FN_INPUTS)
		{
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
			if(paramPtr->TestParamConnectFlag() &&
				(paramPtr->GetDataFromUID() == outputUID))
				{
				paramPtr->SetInitializedInputFlag();
				}	
			}
		}
	}

////////////////////////////////////////////////////////////
void FUNCINST::MarkRequiredTypes(
	BOOL upsfing)
	{
  ATYPEID	tType;
  LPPROJECT projectPtr = GetSubjectPtr()->GetParentProject();
  LPPARAM paramPtr;
  int i;
  
	// Pull in the required type record if directed to
	tType = infoPtr->pGetRequiredType();
	if (tType != NULL)
		if(!projectPtr->MarkRequiredType(tType))
    	{
      RequiredTypeNotFoundMessage(upsfing);
      SetTypeNeedsAddingFlag();
      }

	if(status & FN_INPUTS)
		{
    i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
 			if(paramPtr->GetTypeID() != COMPUID_GENERAL)
 				{
				if(!projectPtr->MarkRequiredType(paramPtr->GetTypeID()))
					{
          ParamNotFoundMessage(paramPtr, upsfing);             
          paramPtr->SetTypeNeedsAddingFlag();
					}
				}
			}
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			//are checked.
			if(paramPtr->GetTypeID() != COMPUID_GENERAL)
				{
				if(!projectPtr->MarkRequiredType(paramPtr->GetTypeID()))
					{
          ParamNotFoundMessage(paramPtr, upsfing); 
          paramPtr->SetTypeNeedsAddingFlag();
					}
				}
			}
		}   
  }
  
////////////////////////////////////////////////////////////
void FUNCINST::ClearRequiredTypes()
	{
  ATYPEID	tType;
  LPPROJECT projectPtr = GetSubjectPtr()->GetParentProject();
  LPPARAM paramPtr;
  int i;
  
	// Pull in the required type record if directed to
	tType = infoPtr->pGetRequiredType();
	if (tType != NULL && (!TestTypeNeedsAddingFlag()))
		projectPtr->ClearRequiredType(tType);

	if(status & FN_INPUTS)
		{
    i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(inputParams, i)))
			{
 			if((paramPtr->GetTypeID() != COMPUID_GENERAL) &&
 				(!paramPtr->TestTypeNeedsAddingFlag()))
				projectPtr->ClearRequiredType(paramPtr->GetTypeID());
			}
		}

	if(status & FN_OUTPUTS)
		{
		i=0;
		while(0 != (paramPtr = (PARAM *)GetObject(outputParams, i)))
			{
			//are checked.
 			if((paramPtr->GetTypeID() != COMPUID_GENERAL) &&
 				(!paramPtr->TestTypeNeedsAddingFlag()))
			  projectPtr->ClearRequiredType(paramPtr->GetTypeID());
			}
		}  
  }


///////////////////////////////////////////////////////////////
void FUNCINST::RequiredTypeNotFoundMessage(
	BOOL upsfing)
  {
  char buffer[256];
  char typeName[8];
  ATYPEID tempID = infoPtr->pGetRequiredType();
  
	MAKE_TYPEID_READABLE(typeName, tempID);

  wsprintf(buffer, "The required type [%s] for function '%s' "
  	"cound not be found in any of your configuration files."
    "  This project may not run as expected.",
    typeName, infoPtr->pGetName());
		
	if(upsfing)
  	AUpsfLogComment(buffer);
  else
		MessageBox(NULL, (LPSTR)buffer, "Warning",
			MB_OKCANCEL|MB_TASKMODAL);
      
	} 

///////////////////////////////////////////////////////////////
void FUNCINST::ParamNotFoundMessage(
	LPPARAM paramPtr,
	BOOL upsfing)
	{
  char buffer[256];
  char typeName[8];
  char paramType[16];
  ATYPEID tempID = paramPtr->GetTypeID();
  
  if(paramPtr->IsInput())
  	lstrcpy(paramType, "input");
  else
  	lstrcpy(paramType, "output");
    
	MAKE_TYPEID_READABLE(typeName, tempID);

	//the type couldn't be found
	wsprintf((LPSTR)buffer, "A type with ID of [ %s ] could not be "
		"found in the object dll's for the %s parameter '%s' of "
		"function '%s'.  The project may not run as expected.",
		typeName, paramType, paramPtr->GetParamDesc()->pGetName(),
		infoPtr->pGetName());

	if(upsfing)
 		AUpsfLogComment(buffer);
  else
		MessageBox(NULL, (LPSTR)buffer, "Warning",
			MB_OKCANCEL|MB_TASKMODAL);
  }	





