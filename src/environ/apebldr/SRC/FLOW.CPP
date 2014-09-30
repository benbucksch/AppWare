//8-31-93  	Always draw the signal in the normal text color - makes scrolling much easier if
//					you don't have to worry about the posibility of the flow being grayed.
//					Results in signalColor no longer being used.

#include "almsys.h"
#include <windowsx.h>

#include "flow.h"
#include "funcinst.h"
#include "fwinobj.h"
#include "exeflow.h"
#include "objinst.h"
#include "memblk.h"
#include "mdiclass.h" //needed so we can call MDI::AddMessage
#include "sjtwin.h"
#include "objgroup.h"
#include "subject.h"
#include "param.h"
#include "popupmen.h"
#include "project.h"
#include "printer.h"
#include "zlist.h"
#include "flowstak.h"
#include "uidlist.h"
#include "intdebug.h"
#include "upsfname.h"
#include "apbutil.h"
#include "rectypes.h"

#define EXTENSION 6
#define EDGE_CLEARANCE 3
#define CLEARANCE 6
#define FL_FUNC_SEPARATION 6
#define FL_VERT_TOLERANCE FUNCSIZE
#define SG_BORDER  1
#define FL_MINSPACE 8
#define FN_HALFHEIGHT ((FUNCSIZE+PAR_BORDER) / 2+BORDER)
#define FL_SIGNHEIGHT 9
#define FL_SIGNWIDTH  9
#define FL_SIGNBORDER 1

#define FL_NUMDRAWINGS 6
#define FL_NUMFLASHES  6
#define FL_ANIMATEINTERVAL 10


HFONT FLOW::font;
int FLOW::flowCount = 0;
HPEN		FLOW::flowPen;
HBITMAP FLOW::redStopSign;
HBITMAP FLOW::greenStopSign;
HBITMAP FLOW::redSignMask;
HBITMAP FLOW::greenSignMask;
RECT FLOW::animateRect;
RECT FLOW::dstRect;
HBITMAP FLOW::disabledStopSign;
POPUPMENU * FLOW::staticPopupPtr = NULL;

FLOW::FLOW(
	SERUID destinationUID,
	SERUID newSourceUID,
	long newStatus,
	LONG newSignal,
	WORD constructorType)
	{
	sourceUID = newSourceUID;
	status = newStatus;
	flowToUID = destinationUID;
	signalRect.top = signalRect.bottom = 0;
	if(!flowCount)
		Initialize();
	flowCount++;
	SetOrder(0);
	signal = newSignal;

	//if we update the integrity in a paste constructor or on a UPSF
	//constructor, objects will get a build signals message before the object id's
	//have been validated.
	if(constructorType == FLOW_NORMALCONSTRUCTOR)
  	{
		UpdateSignalIntegrity();
		CalculatePath();
		}
	else if(constructorType == FLOW_PASTECONSTRUCTOR)
		{
		name = "?";	//this is just really a place holder at this point
								//in time.
		CalculatePath();
		}
	}

FLOW::FLOW(
	void * /*thisPtr*/):
		name(&name)
	{

	if(!flowCount)
		Initialize();
	flowCount++;
	//clear this bit because even though the signal was once updated, it
	//may become outdated and again need to be updated.
	status&= ~FL_SIGNALUPDATED;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			FLOW::ImportUPSF
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL FLOW::ImportUPSF()
	{
	long result;
	BOOL tempBool;

	I_ALWAYSASSERT((result = AUpsfImportField(FLOW_STOP, 0L, AUPSF_NATV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
		return FALSE);
	
	if(result == AUPSF_NOERROR)
		{
		if(tempBool)
			SetStopBits();
		}
	else
		apb_LogUpsfError(result, "flow stop field");
	
	return TRUE; //no error if we reach here
	} 

																			 

BOOL FLOW::PostUPSFImport()
	{
	//signal should be holding onto the new signal value passed in to the
	//constructor.
	UpdateSignalIntegrity();
	return TRUE;  //signal no error
	}

void FLOW::PostPaste()
	{
	//make sure the display status matches the subject's status
	if(((LPFUNCINST)GetObject(flowToUID))->
		GetSubjectPtr()->TestSignalNamesFlag())
		status |= FL_SIGNALNAME;
	else
		status &= ~FL_SIGNALNAME;
	
	UpdateSignalIntegrity();
	}


void FLOW::UpdateSignalIntegrity()
	{
	AObjSignalRecord * signals;
	LPMEMBLOCK signalBlock;
	long byteCount;
	LPOBJINST srcObjPtr;

	if(signal == DEFAULT_SIGNAL)
		{
		//set the signal value to the appropriate signal value
		if(status & FL_OBJSOURCE) //get default signal for object
			{
			signalBlock = new MEMBLOCK();
			srcObjPtr = (LPOBJINST)GetObject(sourceUID);
			//if object is an alias set scrObjPtr to the REAL object
			if(srcObjPtr->TestAliasFlag() && (srcObjPtr->GetSharedObjectUID() != NULLSERUID))
				srcObjPtr = (LPOBJINST)GetObject(srcObjPtr->GetSharedObjectUID());
			srcObjPtr->GetSignals(GetUID(signalBlock),AOBJ_BUILDSIGNALS);
			signals = (AObjSignalRecord *)signalBlock->GetData();
			byteCount = signalBlock->GetByteCount();
			if((int)(byteCount / (long)sizeof(AObjSignalRecord)) > 0)
				signal = signals[0].theID;
			delete signalBlock;
			}
		else
			signal = 1; //default function flow
		}

	SetSignalText();
	}

FLOW::~FLOW()
	{
	flowCount--;
	if(!flowCount)
		{
		Uninitialize();
		}
	}

//static routine to cleanup static data
void FLOW::Uninitialize()
	{
	DeleteObject(font);
	DeleteObject(redStopSign);
	DeleteObject(redSignMask);
	DeleteObject(greenStopSign);
	DeleteObject(greenSignMask);
	DeleteObject(disabledStopSign);
	DeleteObject(flowPen);
	}

//static routine to be called externally
//Only Initialize if flows exists otherwise we could have multiple
//Initializations without an uninitialize.
BOOL FLOW::SetSysColors()
	{
	if(flowCount)
		{
		Uninitialize();

		if(!Initialize())
			return FALSE;
		}
	return TRUE;
	}


//static routine to intialize static data
BOOL FLOW::Initialize()
	{
	LOGFONT lf;
	HDC hdc, hdcMem1, hdcMem2;
	RECT bounds;


	lf.lfHeight = 12;
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
	lstrcpy((LPSTR)lf.lfFaceName, "Small Fonts");
	font = CreateFontIndirect((LPLOGFONT)&lf);

	greenSignMask = LoadBitmap(PROGSYSTEM::hInst, "GREENSIGNMASK");
	greenStopSign = LoadBitmap(PROGSYSTEM::hInst, "GREENSTOPSIGN");
	redSignMask = LoadBitmap(PROGSYSTEM::hInst, "REDSIGNMASK");
	redStopSign = LoadBitmap(PROGSYSTEM::hInst, "REDSTOPSIGN");
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);

	if(!hdc)
		return FALSE;

	hdcMem1 = CreateCompatibleDC(hdc);
	hdcMem2 = CreateCompatibleDC(hdc);

	if(!hdcMem1 || !hdcMem2)
		return FALSE;

	//build the disabled stop sign
	disabledStopSign = CreateCompatibleBitmap(hdc, FL_SIGNWIDTH,
		FL_SIGNHEIGHT);

	if(!disabledStopSign)
		return FALSE;

	SelectObject(hdcMem1, disabledStopSign);
	bounds.left = bounds.top = 0;
	bounds.right = FL_SIGNWIDTH;
	bounds.bottom = FL_SIGNHEIGHT;
	FillRect(hdcMem1, &bounds, PROGSYSTEM::grayTextBrush);
	SelectObject(hdcMem2, redSignMask);
	BitBlt(hdcMem2, 0, 0, FL_SIGNWIDTH, FL_SIGNHEIGHT, NULL, 0, 0,
		DSTINVERT);
	BitBlt(hdcMem1, 0, 0, FL_SIGNWIDTH, FL_SIGNHEIGHT, hdcMem2, 0, 0,
		SRCAND);
	//return redMask to its original state.
	BitBlt(hdcMem2, 0, 0, FL_SIGNWIDTH, FL_SIGNHEIGHT, NULL, 0, 0,
		DSTINVERT);

	flowPen = CreatePen(PS_SOLID, 2, PROGSYSTEM::normalTextColor);

	DeleteDC(hdc);
	DeleteDC(hdcMem1);
	DeleteDC(hdcMem2);

	//No errors if we make it to here
	return TRUE;
	}


void FLOW::Draw(
	HDC hdc,
	LPRECT /*drawBounds*/)
	{
  POINT from;
	POINT to;
	HPEN oldPen;
	LPOBJINST objPtr;
	FUNCINST * dstFuncPtr = (FUNCINST *)GetObject(flowToUID);

	//Get flow's starting point
	if(TestSrcObjFlag())
		{
		objPtr = (LPOBJINST)GetObject(sourceUID);
		objPtr->GetGroupPtr()->GetNippleTip((LPPOINT)&from, objPtr);
		}
	else
		{
		from = ((FUNCINST *)GetObject(sourceUID))->GetArrowTip(FLOW_OUT);
		}

/*	//determine if left edge of destination function lies to the left of
	//the object
	to = dstFuncPtr->GetLocation();
	to.x+=FUNCSIZE;
	if(to.x < from.x && (status & FL_OBJSOURCE))
		status&=~FL_DRAWSIGNAL;
	else */
		status|=FL_DRAWSIGNAL;

	to = dstFuncPtr->GetArrowTip(FLOW_IN);

//	if(PtInRect(drawBounds, to) && PtInRect(drawBounds, from))
//		{
//		signalColor = PROGSYSTEM::normalTextColor;
//		}
//		else
//		{
//		signalColor = PROGSYSTEM::grayTextColor;
//		}
//	drawPen = CreatePen(PS_SOLID, 2, signalColor);
	oldPen = SelectPen(hdc, flowPen);

	if((status & FL_DRAW)  && (status & FL_DRAWSIGNAL))
		{
		I_VERIFY(MoveToEx(hdc, from.x, from.y, NULL));
		LineTo(hdc, controlPt1.x, from.y);
		LineTo(hdc, controlPt1.x, controlPt1.y);
		LineTo(hdc, controlPt2.x, controlPt2.y);
		LineTo(hdc, controlPt2.x, to.y);
		LineTo(hdc, to.x, to.y);
		}
	SelectObject(hdc, oldPen);
//	DeleteObject(drawPen);
	}

void FLOW::DrawSignalRect(
	HDC hdc,
	BOOL offScreen)
	{
	HPEN oldPen,newPen;
	HBRUSH oldBrush;
	HFONT oldFont;
	int oldMode;
	COLORREF oldColor;
	HDC hdcMem;
	int hold;
	HBITMAP mask;
	HBITMAP sign;
	TSTRING visualName;
	HBITMAP bmp = NULL;
	HDC drawDC;

	if(offScreen)
		{
		bmp = CreateCompatibleBitmap(hdc, 
			signalRect.right - signalRect.left,
			signalRect.bottom - signalRect.top);

		if(bmp)
			{
			drawDC = CreateCompatibleDC(hdc);

			I_ASSERT(drawDC); //make sure not out of DC's

			SelectObject(drawDC, bmp);

			SetWindowOrgEx(drawDC, signalRect.left, signalRect.top, NULL);
			}
		else
			drawDC = hdc;
		}
	else
		drawDC = hdc;

	if(status & FL_DRAWSIGNAL)
		{
		GetVisualName((LPTSTRING)&visualName);

		if(status & FL_SELECTED)
			{
			//if the the signal is not grayed then draw the rectangle in the
			//highlight color.
//			if(signalColor == PROGSYSTEM::grayTextColor)
//				newPen = PROGSYSTEM::grayTextPen;
//			else
				newPen = PROGSYSTEM::highlightPen;
			}
		else
			newPen = PROGSYSTEM::windowPen;

		oldColor = SetTextColor(drawDC, PROGSYSTEM::normalTextColor);

		oldBrush = SelectBrush(drawDC, PROGSYSTEM::windowBrush);
		oldPen = SelectPen(drawDC, newPen);
		oldMode = SetBkMode(drawDC, TRANSPARENT);
		oldFont = SelectFont(drawDC, font);
		Rectangle(drawDC, signalRect.left, signalRect.top, signalRect.right,
			signalRect.bottom);

		if(status & FL_STOP)
			{
			if(status & FL_STOPPED)
				{
				mask = greenSignMask;
				sign = greenStopSign;
				}
			else if(status & FL_STOPDISABLED)
				{
				mask = redSignMask;
				sign = disabledStopSign;
				}
			else
				{
				mask =redSignMask;
				sign =redStopSign;
				}
			hdcMem = CreateCompatibleDC(hdc);
			SelectBitmap(hdcMem, mask);
			BitBlt(drawDC, signalRect.left + FL_SIGNBORDER+1,
				(signalRect.top + signalRect.bottom - FL_SIGNHEIGHT)/2,
				FL_SIGNWIDTH, FL_SIGNHEIGHT, hdcMem, 0, 0, SRCAND);
			SelectBitmap(hdcMem, sign);
			BitBlt(drawDC, signalRect.left + FL_SIGNBORDER+1,
				(signalRect.top + signalRect.bottom - FL_SIGNHEIGHT)/2,
				FL_SIGNWIDTH, FL_SIGNHEIGHT, hdcMem, 0, 0, SRCPAINT);
			hold = signalRect.left;
			signalRect.left+=(FL_SIGNWIDTH + FL_SIGNBORDER);
			DeleteDC(hdcMem);
			}

		DrawText(drawDC,visualName, visualName.strlen(),
			(LPRECT)&signalRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER|
			DT_NOCLIP|DT_NOPREFIX);

		if(status & FL_STOP)
			signalRect.left = hold;


		//clean up.
		//dont delete the newpen since its set to the value of a system pen
		SetBkMode(hdc, oldMode);
		SetTextColor(hdc, oldColor);
		SelectBrush(hdc, oldBrush);
		SelectPen(hdc, oldPen);
		SelectFont(hdc, oldFont);
		
		if(bmp)
			{
			BitBlt(hdc, signalRect.left, signalRect.top,
				signalRect.right - signalRect.left,
				signalRect.bottom - signalRect.top,
				drawDC,
				signalRect.left,
				signalRect.top, SRCCOPY);

			DeleteDC(hdcMem);
			DeleteBitmap(bmp);
			}
		}
	}

///////////////////////////////////////////////////////////////////////
//line equation: y=[ x(y0 - y1) + y1x0 - y0x1 / (x0-x1)
//               y= (Ax + B) / C
//
//	A = y0 - y1
//	B = y1x0 - y0x1
//	C = x0 - x1
///////////////////////////////////////////////////////////////////////
void FLOW::CalculatePath()
	{
	POINT newFrom;
	POINT newTo;
	long A,B,C;
	long botY,topY;
	int a,b;
	RECT toRect;
	POINT to;
	FUNCINST * funcPtr;
	BOOL drawDirect = FALSE;
	int offset = 0;
	BOOL done=FALSE;
	POINT from;
	RECT fromRect;
	LPOBJINST objPtr;

	if(TestSrcObjFlag())
		{
		objPtr = (LPOBJINST)GetObject(sourceUID);
		objPtr->GetGroupPtr()->GetNippleTip((LPPOINT)&from, objPtr);
		objPtr->GetGroupPtr()->GetRect((LPRECT)&fromRect);
		}
	else //the source is a function
		{
		funcPtr = (FUNCINST *)GetObject(sourceUID);
		from = funcPtr->GetArrowTip(FLOW_OUT);
		funcPtr->GetRect((LPRECT)&fromRect);
		}

	if(signalRect.top != signalRect.bottom)
		offset = (signalRect.bottom + signalRect.top)/2 -
			(controlPt1.y + controlPt2.y)/2;

	funcPtr = (FUNCINST *)GetObject(flowToUID);
	to = funcPtr->GetArrowTip(FLOW_IN);
	funcPtr->GetRect((LPRECT)&toRect);
	newFrom = from;
	newTo = to;

	newFrom.x+=EXTENSION;
	if(newFrom.x < fromRect.right + EDGE_CLEARANCE)
		newFrom.x = fromRect.right + EDGE_CLEARANCE;
	newTo.x-=EXTENSION;


	if(from.x + EXTENSION < (newTo.x - FL_MINSPACE))
		{
		C = from.x + EXTENSION - newTo.x;
		A = from.y - to.y;

		B = (long)((long)((long)to.y * (long)(from.x + EXTENSION)) -
			(long)((long)from.y * (long)newTo.x));

		topY = (A * (long)fromRect.right + B) / C;

		if((topY >= (long)(from.y - FN_HALFHEIGHT))  &&
			(topY <= (long)(from.y + FN_HALFHEIGHT)))
			{
			// can route directly using the unmodified to and from points
			controlPt1.x = from.x + EXTENSION;
			controlPt1.y = from.y;
			controlPt2.x = newTo.x;
			controlPt2.y = newTo.y;
			done=TRUE;
			flowBounds.left = from.x;
			flowBounds.right = to.x+1;
			if(from.y < to.y)
				{
				flowBounds.top = controlPt1.y;
				flowBounds.bottom = controlPt2.y+1;
				}
			else
				{
				flowBounds.top = controlPt2.y;
				flowBounds.bottom = controlPt1.y+1;
				}
			}
		}
	if(!done)
		{
		C = newFrom.x - newTo.x;
		A = newFrom.y - newTo.y;

		B = (long)(newTo.y * newFrom.x) - (long)(newFrom.y * newTo.x);

		if(newTo.x < newFrom.x)
			{
			// may need to route

			if(newFrom.y < newTo.y)
				{
				//we now know 'from' rectangle on top

				topY = (A * fromRect.right + B)/C;
				botY = (A * toRect.left + B)/C;

				if((topY > (long)fromRect.bottom) &&
					(botY < (long)toRect.top))
					{
					// OK to draw AEVT_DIRECT flow
					drawDirect = TRUE;
					}
				else
					{
					// need to route
					if(fromRect.bottom < toRect.top - 2 * FL_FUNC_SEPARATION)
						{
						if(toRect.top - fromRect.bottom <= FL_VERT_TOLERANCE)
							{
							//route with horizontal line between rectangles
							a = fromRect.bottom + ((toRect.top - fromRect.bottom)/2);
							controlPt1.x = newFrom.x;
							controlPt1.y = a;
							controlPt2.x = newTo.x;
							controlPt2.y = a;
							}
						else
							{
							//route with diaganal line connecting vertical lines drawn from
							// 'from' to fromRect.bottom and 'to' to toRect.top
							controlPt1.x = newFrom.x;
							controlPt1.y = fromRect.bottom + FL_FUNC_SEPARATION;
							controlPt2.x = newTo.x;
							controlPt2.y = toRect.top - FL_FUNC_SEPARATION;
							}
						flowBounds.left = controlPt2.x;
						flowBounds.right = controlPt1.x+1;
						flowBounds.top = newFrom.y;
						flowBounds.bottom = newTo.y+1;
						}
					else
						{
						//route up and over the 'from' rectangle then down and to the
						// 'to' rectangle destination
						if(fromRect.left - CLEARANCE < newTo.x)
							b = fromRect.left - CLEARANCE;
						else
							b = newTo.x;

						controlPt1.x = newFrom.x;
						controlPt2.x = b;
						if(fromRect.top < toRect.top)
							controlPt1.y = fromRect.top - CLEARANCE;
						else
							controlPt1.y = toRect.top - CLEARANCE;
						controlPt2.y = controlPt1.y;
						flowBounds.left = controlPt2.x;
						flowBounds.right = controlPt1.x+1;
						flowBounds.top = controlPt1.y;
						flowBounds.bottom = newTo.y + 1;
						}
					}
				}
			else
				{
				// we now know 'from' rectangle on bottom

				topY = (A * toRect.left + B)/C;
				botY = (A * fromRect.right + B)/C;

				if((topY > (long)toRect.bottom) &&
					(botY < (long)fromRect.top))
					{
					// OK to draw AEVT_DIRECT flow
					drawDirect = TRUE;
					}
				else
					{
					// need to route
					if(toRect.bottom < fromRect.top - 2 * FL_FUNC_SEPARATION)
						{
						if(fromRect.top - toRect.bottom <= FL_VERT_TOLERANCE)
							{
							//route with horizontal line between rectangles
							a = toRect.bottom + ((fromRect.top - toRect.bottom)/2);
							controlPt1.x = newFrom.x;
							controlPt1.y = a;
							controlPt2.x = newTo.x;
							controlPt2.y = a;
							}
						else
							{
							//route with diaganal line connecting vertical lines drawn from
							// 'to' to toRect.bottom and 'from' to fromRect.top
							controlPt1.x = newFrom.x;
							controlPt1.y = fromRect.top - FL_FUNC_SEPARATION;
							controlPt2.x = newTo.x;
							controlPt2.y = toRect.bottom + FL_FUNC_SEPARATION;
							}
						flowBounds.left = controlPt2.x;
						flowBounds.right = controlPt1.x+1;
						flowBounds.top = newTo.y;
						flowBounds.bottom = newFrom.y+1;
						}
					else
						{
						//route down and over the fromRect.hen up and to the toRect
						if(newTo.x > fromRect.left - CLEARANCE)
							b = fromRect.left - CLEARANCE;
						else
							b = newTo.x;
						controlPt1.x = newFrom.x;
						controlPt2.x = b;
						if(fromRect.bottom > toRect.bottom)
							controlPt1.y = fromRect.bottom + CLEARANCE;
						else
							controlPt1.y = toRect.bottom + CLEARANCE;
						controlPt2.y = controlPt1.y;
						flowBounds.left = controlPt2.x;
						flowBounds.right = controlPt1.x+1;
						flowBounds.top = newTo.y;
						flowBounds.bottom = controlPt2.y+1;
						}
					}
				}
			}

		else
			{
			// draw a AEVT_DIRECT connection
			drawDirect = TRUE;
			}

		if(drawDirect)
			{
			controlPt1.x = newFrom.x;
			controlPt1.y = newFrom.y;
			controlPt2.x = newTo.x;
			controlPt2.y = newTo.y;
			if(from.x < to.x)
				{
				flowBounds.left = from.x;
				flowBounds.right = to.x+1;
				}
			else
				{
				flowBounds.left = controlPt2.x;
				flowBounds.right = controlPt1.x+1;
				}
			if(from.y < to.y)
				{
				flowBounds.top = controlPt1.y;
				flowBounds.bottom = controlPt2.y+1;
				}
			else
				{
				flowBounds.top = controlPt2.y;
				flowBounds.bottom = controlPt1.y+1;
				}

			}

		}


	to.x = (controlPt1.x + controlPt2.x)/2;
	to.y = (controlPt1.y + controlPt2.y)/2;
	CalculateSignalRect(to);
	if(offset)
		OffsetRect((LPRECT)&signalRect, 0, offset);

	}


void	FLOW::CalculateSignalRect(
	POINT midPoint)
	{
	SIZE size;
	TEXTMETRIC tm;
	HDC hdc;
	int width;
	int height;
	TSTRING visualName;

	GetVisualName((LPTSTRING)&visualName);
	hdc = CreateDC("DISPLAY",NULL,NULL,NULL);
	SelectObject(hdc, font);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	I_VERIFY(GetTextExtentPoint32(hdc, visualName, visualName.strlen(),
		&size));
	DeleteDC(hdc);

	//the width and height must always be odd for the midpoint to be
	//maintained.

	width = size.cx + 4 * SG_BORDER;
	if(status & FL_STOP)
		width+=(FL_SIGNWIDTH + 2*FL_SIGNBORDER);

	//make width odd

	if((width/2) * 2 == width)
		width++;

	height = tm.tmHeight + 2 * SG_BORDER;

	//make height odd
	if((height/2)*2 == height)
		height++;

	//example of how it all works:
	//the midpoint of 3 and 9 is 6 while the
	//width is 7. So 6 - 7/2 will put the left edge at 3 which is right where
	//we want it.
	signalRect.left = midPoint.x - width/2;

	//we must subtract 1 otherwise the rectangle will be too large.
	//From above, 3 + 7 = 10 and we want the left edge at 9
	signalRect.right = signalRect.left + width - 1;

	//the same logic for the width applies for the height.
	signalRect.top = midPoint.y - height/2;
	signalRect.bottom = signalRect.top + height - 1;
	}

void FLOW::ChangeSignal(
	long newSignal)
	{
	POINT midPoint;

	//add 1 to round up
	midPoint.x = (signalRect.left + signalRect.right)/2;
	midPoint.y = (signalRect.top + signalRect.bottom)/2;

	signal = newSignal;
	SetSignalText();
	CalculateSignalRect(midPoint);
	}

/////////////////////////////////////////////////////////////
void FLOW::ShiftSignalRect(
	int amount)
	{
	int offset;

	offset = (signalRect.bottom - signalRect.top)/2;
	//decreas the magnitude of the offset by 1 since we want the rectangles
	//to overlap by 1 instead of there edges being on top of each other.  This
	//is because when they are drawn, the stupif window's rectangle function
	//doesn't draw the bottommost edge and be overlapping by 1, we prevent
	//a thick black line between signals.
	if(offset<0)
		offset++;
	else
		offset--;


	offset *= amount;

	OffsetRect((LPRECT)&signalRect,0,offset);
	}

/////////////////////////////////////////////////////////////
void FLOW::SetSignalText()
	{
	LPOBJINST         objPtr;
	LPMEMBLOCK        signalBlock;
	AObjSignalRecord * signals;
	char              signalText[256] = {"?"};
	LPFLOWDESC        flowDesc;
	LONG              signalSize;
	UINT       i;
	long              maxSignals;
	BOOL              sigFound=FALSE;

	if(status & FL_FUNCSOURCE)
		{
		flowDesc = ((FUNCINST *)GetObject(sourceUID))->GetDescPtr()->
		 GetFlow((int)(signal-1));
		if(flowDesc)
			{
			lstrcpy((LPSTR)&(signalText[0]), flowDesc->name.pGetPointer());
			ClearSignalNotLinkedFlag();
			}
		else
			SetSignalNotLinkedFlag();
		}
	else if(status & FL_OBJSOURCE)
		{
		objPtr = (OBJINST *)GetObject(sourceUID);
		//if object is an alias set objPtr to the REAL object
		if(objPtr->TestAliasFlag() && (objPtr->GetSharedObjectUID() != NULLSERUID))
			objPtr = (LPOBJINST)GetObject(objPtr->GetSharedObjectUID());
		signalBlock = new MEMBLOCK();
		objPtr->GetSignals(GetUID(signalBlock), AOBJ_BUILDSIGNALS);
		signals = (AObjSignalRecord *)signalBlock->GetData();
		signalSize = AMemGetSize(GetUID(signalBlock).id);
		maxSignals = signalSize / sizeof(AObjSignalRecord);
		
		for(i=0 ; i < maxSignals ; i++)
			{
			if(signals[i].theID == signal)
				{
				lstrcpy(signalText, (LPSTR)signals[i].theName);
				i = (UINT)maxSignals;
				sigFound = TRUE;
				}
			}

		if(!sigFound)
			SetSignalNotLinkedFlag();
		else
			ClearSignalNotLinkedFlag();

		delete signalBlock;
		}

	name = (LPSTR)signalText;
	}

BOOL FLOW::IsSelected()
	{
	if(status & FL_SELECTED)
		return TRUE;
	else
		return FALSE;
	}

void FLOW::SetSelected(
	BOOL set)
	{
	if(set)
		status|=FL_SELECTED;
	else
		{
		status&= ~FL_SELECTED;
		}
	}

void FLOW::SetDraw(
	BOOL set)
	{
	if(set)
		status|=FL_DRAW;
	else
		status&= ~FL_DRAW;
	}


void FLOW::BringUpMenu(
	HWND parent)
	{
	POINT pt;
	HDC hdc;
	OBJINST * objPtr;
	FUNCINST * funcPtr;
	int numFlows;

	AObjSignalRecord * signals;
	LPMEMBLOCK signalBlock;
	long byteCount;
	int i;
	RECT rect;
	RECT unionRect;
  LPPOPUPMENU popupPtr;

	popupPtr = new POPUPMENU(parent, font, POPUP_MESSAGEHOG|POPUP_LEFTBUTTON);
	if(status & FL_FUNCSOURCE)
		{
		funcPtr = (FUNCINST *)GetObject(sourceUID);
		numFlows = funcPtr->GetDescPtr()->GetFlowsCount();
		for(int i=0;i<numFlows;i++)
			{
			popupPtr->AppendItem((LPSTR)(funcPtr->GetDescPtr()->
				GetFlow(i)->name.pGetPointer()), i+1, i);
			}
		}
	else if(status & FL_OBJSOURCE)
		{
		objPtr = (OBJINST *)GetObject(sourceUID);
		//if the object is an alias, set objPtr to the REAL object
		if(objPtr->TestAliasFlag() && (objPtr->GetSharedObjectUID() != NULLSERUID))
			objPtr = (LPOBJINST)GetObject(objPtr->GetSharedObjectUID());
		signalBlock = new MEMBLOCK();
		objPtr->GetSignals(GetUID(signalBlock),AOBJ_BUILDSIGNALS);
		signals = (AObjSignalRecord *)signalBlock->GetData();
		byteCount = signalBlock->GetByteCount();
		numFlows = (int)(byteCount / (long)sizeof(AObjSignalRecord));

		for(i=0;i<numFlows;i++)
			{
			if(PROGSYSTEM::TestObjSignalValuesPref())
				popupPtr->AppendItem((LPSTR)signals[i].theName,signals[i].theID,
					SHOW_ORDINAL);
			else
				popupPtr->AppendItem((LPSTR)signals[i].theName,signals[i].theID,
					NO_ORDINAL);
			}

		delete signalBlock;
		}

	pt.x = signalRect.left;
	pt.y = signalRect.top;
	hdc = GetDC(parent);
	LPtoDP(hdc, (LPPOINT)&pt, 1);
	ClientToScreen(parent, (LPPOINT)&pt);
	if(popupPtr->ShowPopup(pt, signal))
		{
		GetSignalRect((LPRECT)&rect);
		ChangeSignal(popupPtr->GetItemData());
		GetSignalRect((LPRECT)&unionRect);
		UnionRect((LPRECT)&unionRect, (LPRECT)&unionRect, (LPRECT)&rect);
		LPtoDP(hdc, (LPPOINT)&unionRect, 2);
		InvalidateRect(parent, (LPRECT)&unionRect, TRUE);
		delete popupPtr;
		UpdateWindow(parent);
		PROGSYSTEM::GetCurrentProject()->MakeDirty();
		}
	else
		delete popupPtr;

	ReleaseDC(parent, hdc);
	}


//Static routine to bring up a menus when the user is dragging a flow
//over an object or function connection point.  The menu should disappear
//either when the mouse moves horizontally out of bounds or the user
//releases the mouse button.
//returns TRUE if a signal was chosen else returns false
BOOL FLOW::ChooseInitialSignal(
	SERUID fillerUID,
	SERUID fillerType,
	POINT screenLocation)
	{
	OBJINST * objPtr;
	FUNCINST * funcPtr;
	int numFlows;
	AObjSignalRecord * signals;
	LPMEMBLOCK signalBlock;
	long byteCount;
	int i;
	long tempSignal;

	if(!flowCount)
		Initialize();

	FLOW::staticPopupPtr = new POPUPMENU(NULL, font, POPUP_MESSAGEHOG |
		POPUP_NOOUTOFBOUNDS | POPUP_HCENTER | POPUP_LEFTBUTTON);

	if(fillerType == FLOW_FROMFUNCTION)
		{
		funcPtr = (FUNCINST *)GetObject(fillerUID);
		numFlows = funcPtr->GetDescPtr()->GetFlowsCount();
		for(int i=0;i<numFlows;i++)
			{
			staticPopupPtr->AppendItem((LPSTR)(funcPtr->GetDescPtr()->
				GetFlow(i)->name.pGetPointer()), i+1, i);
			}
		tempSignal = DEFAULT_SIGNAL;
		}
	else if(fillerType == FLOW_FROMOBJECT)
		{
		objPtr = (OBJINST *)GetObject(fillerUID);
		//if the object is an alias, set objPtr to the REAL object
		if(objPtr->TestAliasFlag() && (objPtr->GetSharedObjectUID() == NULLSERUID))
			objPtr = (LPOBJINST)GetObject(objPtr->GetSharedObjectUID());
		signalBlock = new MEMBLOCK();
		objPtr->GetSignals(GetUID(signalBlock),AOBJ_BUILDSIGNALS);
		signals = (AObjSignalRecord *)signalBlock->GetData();
		byteCount = signalBlock->GetByteCount();
		numFlows = (int)(byteCount / (long)sizeof(AObjSignalRecord));

		for(i=0;i<numFlows;i++)
			{
			if(PROGSYSTEM::TestObjSignalValuesPref())
				staticPopupPtr->AppendItem((LPSTR)signals[i].theName,signals[i].theID,
					SHOW_ORDINAL);
			else
				staticPopupPtr->AppendItem((LPSTR)signals[i].theName,signals[i].theID,
					NO_ORDINAL);
			}

		if(numFlows > 0)
			tempSignal = signals[0].theID;

		delete signalBlock;
		}

	if(staticPopupPtr->ShowPopup(screenLocation, tempSignal))
		return TRUE;
	else
		{
		delete FLOW::staticPopupPtr;
		FLOW::staticPopupPtr = NULL;
		if(!flowCount)
			Uninitialize();
		return FALSE;
		}
	}


//should be called after ChooseInitialSignal returns TRUE to destroy the
//popup and get the chosen signal
long FLOW::GetInitialSignal()
	{
	long tempSignal = 0L;

	if(staticPopupPtr)
		{
		tempSignal = FLOW::staticPopupPtr->GetItemData();
		delete FLOW::staticPopupPtr;
		FLOW::staticPopupPtr = NULL;
		if(!flowCount)
			Uninitialize();
		}
	return tempSignal;
	}

void FLOW::UpdateUIDs()
	{
	LPEXEFLOW exeFlow;

	exeFlow = (LPEXEFLOW)GetObject(copyUID);
	exeFlow->SetFlowToUID(((FUNCINST *)GetObject(exeFlow->GetFlowToUID()))->copyUID);
	}

void FLOW::FlashFlow()
	{
	RECT clientBounds;
	POINT originOffset;
	HWND hwnd;
	HDC hdc;
	SUBJECT * subjectPtr;

	subjectPtr = ((FUNCINST *)GetObject(flowToUID))->GetSubjectPtr();
	subjectPtr->GetSubjectClientBounds((LPRECT)&clientBounds);
	originOffset = subjectPtr->GetOriginOffset();
	OffsetRect((LPRECT)&clientBounds, originOffset.x, originOffset.y);
	if(signalRect.left < clientBounds.left || signalRect.right >
		clientBounds.right || signalRect.top < clientBounds.top ||
		signalRect.bottom > clientBounds.bottom)
		{
		originOffset.x = (signalRect.left + signalRect.right)/2 -
			(clientBounds.right - clientBounds.left)/2;
		originOffset.y = (signalRect.top + signalRect.bottom)/2 -
			(clientBounds.bottom - clientBounds.top)/2;
		subjectPtr->SetOriginOffset(originOffset);

		subjectPtr->CalculateAllFlowPaths();

		if(subjectPtr->GetWindowPtr())
			{
			hwnd = subjectPtr->GetWindowPtr()->GetWindowHandle();
			hdc = GetDC(hwnd);
			SetWindowOrgEx(hdc, originOffset.x, originOffset.y, NULL);
			InvalidateRect(hwnd, NULL, TRUE);
			ReleaseDC(hwnd, hdc);
			((SUBJECTWINDOW *)subjectPtr->GetWindowPtr())->SetAllScrollBars();
			}
		}
	subjectPtr->ShowObjectWindow();
	hwnd = subjectPtr->GetWindowPtr()->GetWindowHandle();
	UpdateWindow(hwnd); // make sure window is updated before animation
	AnimateStop(subjectPtr);
	SetStoppedAtFlowFlag();
	hdc = GetDC(hwnd);
	DrawSignalRect(hdc);
	ReleaseDC(hwnd, hdc);
	}

////////////////////////////////////////////////////////////
void FLOW::AnimateStop(
	SUBJECT * subjectPtr)
	{
	HDC hdc;
	RECT bounds;
	HWND hwnd;
	int i;
	LPOBJINST objPtr;

	hwnd = subjectPtr->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, (LPRECT)&bounds);
	DPtoLP(hdc, (LPPOINT)&bounds, 2);
	if(status & FL_OBJSOURCE)
		{
		objPtr = (LPOBJINST)GetObject(sourceUID);
    objPtr->GetGroupPtr()->GetObjectRect(objPtr->GetIndex(), (LPRECT)&animateRect);
		}
	else
		{
		((FUNCINST *)GetObject(sourceUID))->GetSelectedRect(
			(LPRECT)&animateRect);
		}
	dstRect = signalRect;
	LPtoDP(hdc, (LPPOINT)&dstRect,2);
	ClientToScreen(hwnd, (LPPOINT)&dstRect.left);
	ClientToScreen(hwnd, (LPPOINT)&dstRect.right);
	LPtoDP(hdc, (LPPOINT)&animateRect, 2);
	ClientToScreen(hwnd, (LPPOINT)&animateRect.left);
	ClientToScreen(hwnd, (LPPOINT)&animateRect.right);
	ReleaseDC(hwnd, hdc);
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);

	for(i=FL_NUMDRAWINGS;i>-FL_NUMFLASHES;i--)
		{
		DoStopAnimation(hdc, i);
		SleepEx(FL_ANIMATEINTERVAL, FALSE);
		}
	DeleteDC(hdc);
	}

////////////////////////////////////////////////////////////
void FLOW::DoStopAnimation(
	HDC hdc,
	int loopNum)
	{
	static int yStep;
	static int xStep;
	static int xGrow;
	static int yGrow;
	static RECT oldRect;
	static RECT currentRect;

	if(loopNum>0)
		{
		if(loopNum == 1)
			currentRect = dstRect;
		if(loopNum == FL_NUMDRAWINGS)
			{
			currentRect = animateRect;
			SetROP2(hdc, R2_NOTXORPEN);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			SelectObject(hdc, PROGSYSTEM::highlightPen);
			yStep = (currentRect.top + currentRect.bottom -
				(dstRect.top + dstRect.bottom))
					/(2 * FL_NUMDRAWINGS);
			xStep = (currentRect.left + currentRect.right -
				(dstRect.left + dstRect.right))
					/(2 * FL_NUMDRAWINGS);

			yGrow = ((currentRect.bottom - currentRect.top) -
				(dstRect.bottom - dstRect.top))/
				(FL_NUMDRAWINGS);
			xGrow = ((currentRect.right - currentRect.left) -
				(dstRect.right - dstRect.left))/
				(FL_NUMDRAWINGS);
			}
		else
			{
			Rectangle(hdc, oldRect.left, oldRect.top, oldRect.right,
				oldRect.bottom);
			}
		Rectangle(hdc, currentRect.left, currentRect.top, currentRect.right,
			currentRect.bottom);
		oldRect = currentRect;
		OffsetRect((LPRECT)&currentRect, -xStep, -yStep);
		InflateRect((LPRECT)&currentRect, -xGrow, -yGrow);
		}
	else
		{
		if(loopNum == 0)
			{
			SetROP2(hdc, R2_NOTXORPEN);
			Rectangle(hdc, oldRect.left, oldRect.top, oldRect.right,
				oldRect.bottom);
			}
		BitBlt(hdc, dstRect.left, dstRect.top, (dstRect.right - dstRect.left),
			(dstRect.bottom - dstRect.top), NULL, 0, 0, DSTINVERT);
		}
	}

void FLOW::SetStop(
	LPRECT bounds)
	{
	if(bounds)
		UnionRect(bounds, bounds, &signalRect);

	SetStopBits();
	ChangeSignal(signal);

	if(bounds)
		UnionRect(bounds, bounds, &signalRect);
	}

void FLOW::SetStopBits()
	{
	status|=FL_STOP;
	if(TestLinkToExeFlag())
		{
		((LPEXEFLOW)GetObject(copyUID))->AddInfo(FL_STOP);
		}
	}

/////////////////////////////////////////////////////////////
void FLOW::ClearStopBits()
	{
  status &= ~FL_STOP;
  if(TestLinkToExeFlag())
  	{
    ((LPEXEFLOW)GetObject(copyUID))->RemoveInfo(FL_STOP);
    }
  }
  
/////////////////////////////////////////////////////////////
void FLOW::ClearStop(
	LPRECT bounds)
	{
	
	if(bounds)
		UnionRect(bounds, bounds, &signalRect);
	
  ClearStopBits();
	ChangeSignal(signal);
	
	if(bounds)
		UnionRect(bounds, bounds, &signalRect);
	}

////////////////////////////////////////////////////
void FLOW::DisableStop()
	{
	status|=FL_STOPDISABLED;
	if(TestLinkToExeFlag())
		{
		((LPEXEFLOW)GetObject(copyUID))->AddInfo(FL_STOPDISABLED);
		}
	}

void FLOW::EnableStop()
	{
	status&= FL_STOPDISABLED;
	if(TestLinkToExeFlag())
		{
		((LPEXEFLOW)GetObject(copyUID))->RemoveInfo(FL_STOPDISABLED);
		}
	}




void FLOW::ClearStoppedFlow()
	{
	SUBJECT * subjectPtr;
	BASEWINDOW * windowPtr;
	HWND hwnd;
	HDC hdc;
	RECT rect;

	subjectPtr = ((FUNCINST *)GetObject(flowToUID))->GetSubjectPtr();
	if((windowPtr = subjectPtr->GetWindowPtr())!=NULL)
		{
		hwnd = windowPtr->GetWindowHandle();
		hdc = GetDC(hwnd);
		rect = signalRect;
		LPtoDP(hdc, (LPPOINT)&rect, 2);
		ReleaseDC(hwnd, hdc);
		InvalidateRect(hwnd, (LPRECT)&rect, FALSE);
		}
	ClearStoppedAtFlowFlag();
	}


void FLOW::DoPrint(
	LPPRINTER printPtr,
	PRINTFLOWSTACK * flowStackPtr,
	int indent)
	{
	LPSTR buffer;
	FUNCINST * funcPtr;

	if(TestClosePrintFlag())
		{
		if(TestPrintFlag())
			{
			printPtr->PrintLine(indent+1, "}");
			ClearPrintFlag();
			}
		ClearClosePrintFlag();
		return;
		}

	if(TestPrintFlag())
		{
		buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT, name.strlen() + 20);
		if(status & FL_OBJSOURCE)
			wsprintf(buffer, "For signal: \"%s\"",(LPCSTR)name);
		else
			wsprintf(buffer, "For flow: \"%s\"", (LPCSTR)name);
		printPtr->PrintLine(indent, buffer);
		printPtr->PrintLine(indent+1, "{");
		GlobalFree((HGLOBAL)buffer);
		indent++;
		}
	SetClosePrintFlag();
	funcPtr = (FUNCINST *)GetObject(flowToUID);
	funcPtr->DoPrint(printPtr, flowStackPtr, indent);
	}

//assumes will be double visited for each flow
void FLOW::BuildLabels(
	PRINTFLOWSTACK * flowStackPtr)
	{
	FUNCINST * funcPtr;

	if(!TestClosePrintFlag())
		{
		funcPtr = (FUNCINST *)GetObject(flowToUID);
		funcPtr->BuildLabels(flowStackPtr);
		SetClosePrintFlag();
		}
	else
		ClearClosePrintFlag();
	}


WORD FLOW::ExportUPSF()
	{
	int	tempInt;
	BOOL tempBool;
	long result;
	WORD returnValue = EXPORT_NOERROR;
	long upsfSignal;

	tempInt = ((FUNCINST *)GetObject(flowToUID))->GetSubjectPtr()->
			UPSFFuncToIndex(flowToUID);

	I_ASSERT(tempInt != SJT_UPSFINDEXERROR);

	I_ALWAYSASSERT(AUpsfExportField(FLOW_CONNECTINDEX, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempInt, (long)sizeof(int)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	// UPSF flow IDs are zero-based, while we are one-based
	upsfSignal = this->TestSrcObjFlag()
											? this->signal
											: this->signal - 1;

	I_ALWAYSASSERT(AUpsfExportField(FLOW_SIGNALID, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&upsfSignal, (long)sizeof(long)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);


	if(TestStopFlag())
		tempBool = TRUE;
	else
		tempBool = FALSE;

	//only need to check last field to see if  we're out of disk space
	result = AUpsfExportField(FLOW_STOP, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL));

	I_ASSERT(result == AUPSF_NOERROR);

	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	return returnValue;
	}

BOOL FLOW::AddFlowToZlist(
	LPRECT area,
	LPZLIST zlistPtr)
	{
	if(SignalInRect(area))
		{
		zlistPtr->Add((LPRECT)&signalRect, Z_FLOW, GetUID(this));
		return TRUE;
		}
	return FALSE;
	}

short FLOW::ValidateFlow()
	{
	char message[] = {"The flow with signal %s coming "
				"from %s %s in subject %s did not flow into a valid function "
				"and was therefore cleared.\r\n"};
	char flowFromString[16];
	LPSTR buffer;
	FUNCINST * funcPtr;
	LPOBJINST objPtr;
	short result = PRJ_READNOERROR;


	if((!GetObject(flowToUID))  || (GetUIDType(flowToUID) != RT_FUNCINST))
		{
		result = PRJ_READWARNING|PRJ_READBADFLOW;
		//function we're flowing into doesn't exist so display message.
		if(TestSrcObjFlag())
			{
			lstrcpy((LPSTR)flowFromString, "object");
			
			//objPtr must be good otherwise this routine should never have
			//been called.
			objPtr = (LPOBJINST)GetObject(sourceUID);
			I_ASSERT(objPtr);

			//add an extra 8 for good measure and to hold the null terminator
			buffer = (LPSTR)GlobalAlloc(0, lstrlen(name) +
				lstrlen(objPtr->GetName()) +
				lstrlen(message) + lstrlen(flowFromString) +
				lstrlen(objPtr->GetSubjectPtr()->GetName()) + 8);
			if(buffer)
				{
				wsprintf(buffer, message, (LPCSTR)name, (LPCSTR)flowFromString,
					objPtr->GetName(), objPtr->GetSubjectPtr()->GetName());
				}
      else
      	{
        I_ASSERT(FALSE);
        }
			}
		else //we must be coming from a function
			{
			lstrcpy((LPSTR)flowFromString, "function");

			//Funcptr must be good otherwise this routine should never have
			//been called.
			funcPtr = (FUNCINST *)GetObject(sourceUID);
			I_ASSERT(funcPtr);
			funcPtr->DecFlowCount(FLOW_OUT);
			//add an extra 8 for good measure and to hold the null terminator
			buffer = (LPSTR)GlobalAlloc(0, lstrlen(name) +
				lstrlen(funcPtr->GetDescPtr()->
				pGetName()) +  lstrlen((LPSTR)message) +
				lstrlen((LPSTR)flowFromString) + lstrlen((LPSTR)funcPtr->
				GetSubjectPtr()->GetName()) + 8);
			if(buffer)
				{
				wsprintf(buffer, message, (LPCSTR)name, (LPCSTR)flowFromString,
					funcPtr->GetDescPtr()->pGetName(), funcPtr->GetSubjectPtr()->
					GetName());
				}
      else
      	{
        I_ASSERT(FALSE);
        }
			}
		if(buffer)
			{
			MDI::AddMessage(buffer, TRUE);
			GlobalFree(buffer);
			}
		}
	else //cause the signal to make sure its text matches its signal.
		{
		SetSignalText();
		CalculatePath();
		}

	return result;
	}

//This routine is mainly for the debugging aid of being able to display
//the signal values off of objects.
void FLOW::GetVisualName(
	LPTSTRING visName)
	{
	char visBuffer[256];

	if(PROGSYSTEM::TestObjSignalValuesPref() && (status & FL_OBJSOURCE))
		{
		//append signal onto name
		wsprintf(visBuffer, "%lu %s",
			signal, (LPCSTR)name);

		*visName = (LPSTR)visBuffer;
		}
	else if(status & FL_FUNCSOURCE && !(status & FL_SIGNALNAME))
		{
		//display flow by signal number
		wsprintf((LPSTR)visBuffer, "%lu", signal);
		*visName = (LPSTR)visBuffer;
		}
	else //display by name
		*visName = name;

	}


void FLOW::SelectChain(
	HDC hdc,
	LIST * selectedFuncs,
	LIST * selectedFlows)
	{
	LPFLOWSTACK flowStack;
	LPFLOW flowPtr;
  LPUIDLIST member;

	flowStack = new FLOWSTACK();

	//push ourself onto the stack and jump into the while loop.
	flowStack->Push(this);
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

