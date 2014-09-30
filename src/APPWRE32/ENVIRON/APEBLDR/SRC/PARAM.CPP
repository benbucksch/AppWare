#include "almsys.h"
#include <windowsx.h>

#include "param.h"
#include "funcinst.h"
#include "fwinobj.h"
#include "resconst.h"
#include "memlib.h"
#include "objinst.h"
#include "exeparam.h"
#include "progsys.h"
#include "printer.h"
#include "subject.h"
#include "popupmen.h"
#include "intdebug.h"
#include "upsfname.h"
#include "project.h" 
#include "mdiclass.h"
#include "oiputils.h"

int PARAM::paramCount=0;
HFONT PARAM::requiredFont=NULL;
HFONT PARAM::optionalFont;
HFONT PARAM::boldOptional;
HFONT PARAM::boldRequired;
char PARAM::dlgBuffer[MAX_PARAM_NAME];
HBITMAP PARAM::grayInArrow;
HBITMAP PARAM::blackInArrow;
HBITMAP PARAM::arrowMask;

#define PARAM_ARROWHEIGHT 8
#define PARAM_ARROWWIDTH 4

#define PARAM_TEXTWIDTH 2 * FUNCSIZE

#define PARAM_EQUIVALENTBITS (PAR_TEMPOBJ | PAR_CANBECONSTANT | PAR_TEXTLEGAL)

PARAM::PARAM(
	LPPARAMDESC descPtr,
	int   info,
	SERUID theOwnerFunc,
	int count,
	int position,
	BOOL figureBounds)
	{
	ownerFunc = theOwnerFunc;
	name = descPtr->pGetName();
	typeID = descPtr->type;
	status = info;
	if(!paramCount)
		Initialize();
	paramCount++;

	if(figureBounds)
		CalculateRect(((FUNCINST *)GetObject(ownerFunc))->GetLocation(),
			position);
	
	paramDesc = descPtr;
	connectCount = 0;
	dataFrom = NULLSERUID;
	
	//determine if text interchange supported before setting the
	//CanBeConstant flag.

	DetermineTextValidity();
	
	if(IsInput())
		{
		ordinal = count;
		if(!paramDesc->TestNoTempObjectsFlag())
			status |= PAR_TEMPOBJ;

		if(TestTextFlag() && (TestTempObjFlag()))
			status |= PAR_CANBECONSTANT;
		}
	else
		ordinal = -(count + 1);
	
	}

PARAM::PARAM(
	void * thisPtr):PARAMMASK(thisPtr)
	{
	if(!paramCount)
		Initialize();
	paramCount++;
	}

PARAM::~PARAM()
	{
	paramCount--;
	if(!paramCount)
		{
		Uninitialize();
		}
	}

BOOL PARAM::Equivalent(
	const LPPARAM paramPtr)
	{
	return (typeID == paramPtr->typeID && 
		((status & PARAM_EQUIVALENTBITS) ==
		(paramPtr->status & PARAM_EQUIVALENTBITS)))?TRUE:FALSE;
	}

PARAM & PARAM::operator =(
	const PARAM &		srcParam)
	{
	this->status = srcParam.status;

	this->name = (LPCSTR)((PARAM &)srcParam).name;
	this->dataFrom = srcParam.dataFrom;
	this->connectCount = srcParam.connectCount;

	return *this;
	}

//static rouinte to perform cleanup of static data
void PARAM::Uninitialize()
	{
	DeleteObject(requiredFont);
	DeleteObject(optionalFont);
	DeleteObject(boldOptional);
	DeleteObject(boldRequired);
	DeleteObject(grayInArrow);
	DeleteObject(blackInArrow);
	DeleteObject(arrowMask);
	requiredFont = NULL;
	}

//static routine to be called externally
//Only initialize if params exists otherwise we could have multiple
//Initializations without an uninitialize.
BOOL PARAM::SetSysColors()
	{
	if(paramCount)
		{
		PARAM::Uninitialize();

		if(!PARAM::Initialize())
			return FALSE;
		}

	return TRUE;
	}


void PARAM::SetFont(
	HDC hdc)
	{
	if(status & PAR_REQUIRED)
		if(status & PAR_SELECTED)
			SelectObject(hdc, boldRequired);
		else
			SelectObject(hdc, requiredFont);
	else if(status & PAR_SELECTED)
		SelectObject(hdc, boldOptional);
	else
		SelectObject(hdc, optionalFont);

	}

void PARAM::Erase(
	HDC hdc)
	{
	int oldDC;

	oldDC = SaveDC(hdc);
	SetFont(hdc);
	DoErase(hdc);
	RestoreDC(hdc, oldDC);
	}


void PARAM::DoErase(
	HDC hdc)
		{
		FillRect(hdc, (LPRECT)&location, PROGSYSTEM::windowBrush);
		}

void PARAM::Draw(
	HDC hdc,
	BOOL erase)
	{
	int oldDC;
	HDC hdcMem;
	TSTRING tempName;


	oldDC = SaveDC(hdc);
	hdcMem = CreateCompatibleDC(hdc);

	SetBkMode(hdc, TRANSPARENT);

	if(status & PAR_NOTLINKED)
		SetTextColor(hdc, PROGSYSTEM::grayTextColor);
	else
		{
		if(status & PAR_SELECTED)
			SetTextColor(hdc, PROGSYSTEM::highlightColor);
		else
			SetTextColor(hdc, PROGSYSTEM::normalTextColor);
		}
	SetFont(hdc);

	tempName = MakeName(hdc, TRUE);

	if(erase)
		DoErase(hdc);

	DrawText(hdc, tempName, tempName.strlen(), (LPRECT)&location,
		DT_NOPREFIX|DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
	SelectObject(hdcMem, arrowMask);
	BitBlt(hdc,location.left, (location.top + location.bottom -
		PARAM_ARROWHEIGHT)/2,PARAM_ARROWWIDTH, PARAM_ARROWHEIGHT,
		hdcMem, 0, 0, SRCAND);
	if(status & PAR_NOTLINKED)
		SelectObject(hdcMem, grayInArrow);
	else
		SelectObject(hdcMem, blackInArrow);
	BitBlt(hdc,location.left, (location.top + location.bottom -
		PARAM_ARROWHEIGHT)/2,PARAM_ARROWWIDTH, PARAM_ARROWHEIGHT,
		hdcMem, 0,0,SRCPAINT);
	DeleteDC(hdcMem);
	RestoreDC(hdc, oldDC);
	}



void PARAM::SetSelected(
	BOOL set)
	{
	if(set)
		status|=PAR_SELECTED;
	else
		status&=~PAR_SELECTED;
	AdjustWidth();
	}


TSTRING PARAM::MakeName(
	HDC hdc,
	BOOL truncate)
	{
	TSTRING tempName;
	LPSTR buffer;
	LPSTR tempBuffer;
	char  quoteChar;
	int strLength;
	SIZE text;
	int endOffset;
	int shift;
	TEXTMETRIC tm;
	char endBuffer[5];
	BOOL useTempName = FALSE;

	//See if we need quotes
	if(status & (PAR_SINGLEQUOTED | PM_CONSTANT))
		{
		//allocate buffer big enough to hold string plus quotes at each end
		//plus three dots
		strLength = name.strlen();
		buffer = (LPSTR) GlobalAlloc(GPTR, strLength + 8);

		//put quotes at each end
		if(status & (PAR_SINGLEQUOTED))
			quoteChar = '\'';
		else
			quoteChar = '\"';
		buffer[0] = quoteChar;
		lstrcpy((LPSTR)&buffer[1], name);
		buffer[strLength+1]=quoteChar;
		buffer[strLength+2]='\0';

		//set flag to signal that buffer holds the modified name
		useTempName = TRUE;
		}
	if(truncate) //flag that if necessary truncate the name
		{
		if(!useTempName) //see if buffer already allocated
			{
			useTempName = TRUE;
			//allocate buffer big enough to hold string plus 3 dots since we
			//don't need quotes.
			strLength = name.strlen();
			buffer = (LPSTR) GlobalAlloc(GPTR, strLength + 6);
			lstrcpy(buffer, name);
			}
		GetTextExtentPoint32(hdc, buffer, lstrlen(buffer), &text);
		if(text.cx > PARAM_TEXTWIDTH)
			{
      SIZE size;
			//allocate temp buffer big enough to string plus 2 quotes and 3 dots
			tempBuffer = (LPSTR) GlobalAlloc(GPTR, strLength + 8);
			GetTextMetrics(hdc, &tm);
			if(status & PAR_SINGLEQUOTED)
				{
				tempBuffer[0]='\'';
				tempBuffer[1]='\0';
				endOffset = 1;
				lstrcpy(endBuffer, "...'");
				}
			else if(status & PM_CONSTANT)
				{
				tempBuffer[0]='\"';
				tempBuffer[1]='\0';
				endOffset = 1;
				lstrcpy(endBuffer, "...\"");
				}
			else
				{
				tempBuffer[0]='\0';
				endOffset = 0;
				lstrcpy(endBuffer, "...");
				}
			GetTextExtentPoint32(hdc, endBuffer, lstrlen(endBuffer), &size);
			shift = size.cx;
			
			while(GetTextExtentPoint32(hdc, tempBuffer, lstrlen(tempBuffer), &text),
						text.cx + shift < PARAM_TEXTWIDTH)
				{
				tempBuffer[endOffset] = buffer[endOffset];
				endOffset++;
				tempBuffer[endOffset] = '\0';
				}
			lstrcat(tempBuffer, endBuffer);
			lstrcpy(buffer, tempBuffer);
			GlobalFree((HGLOBAL) tempBuffer);
			}
		}

	if(useTempName)
		{
		tempName = buffer;
		GlobalFree((HGLOBAL) buffer);
		return tempName;
		}
	else
		return name;
	}


void PARAM::CalculateRect(
	POINT refPt,
	int count)
	{
	HDC hdc;
	SIZE text;
	TEXTMETRIC tm;
	TSTRING tempName;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SetFont(hdc);
	tempName = MakeName(hdc, TRUE);
	GetTextExtentPoint32(hdc, tempName, tempName.strlen(), &text);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	location.left = refPt.x;
	location.right = location.left + (text.cx) + PARAM_ARROWWIDTH +
		PAR_BORDER;
	if(status & PAR_INPUT)
		{
		location.top = refPt.y - count * (tm.tmHeight/*+PAR_BORDER*/) - PAR_BORDER;
		}
	else
		{
		location.top = refPt.y + FUNCSIZE - (count+1)*(tm.tmHeight/*+PAR_BORDER*/)
			+ PAR_BORDER;
		}
	location.bottom = location.top + tm.tmHeight/* + PAR_BORDER*/;
	DeleteDC(hdc);

	}

void PARAM::UpdateLocation(
	int position,
	LPFUNCINST funcPtr)
	{
	CalculateRect(funcPtr->GetLocation(), position);
	AdjustWidth();
	}

void PARAM::AdjustWidth()
	{
	SIZE width;
	HDC hdc;
	TSTRING tempName;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SetFont(hdc);
	tempName = MakeName(hdc, TRUE);
	GetTextExtentPoint32(hdc, tempName, tempName.strlen(), &width);
	location.right = location.left + (width.cx) + PARAM_ARROWWIDTH;
	DeleteDC(hdc);
	}

//static routine to initialize static data
BOOL PARAM::Initialize()
	{
	LOGFONT lf;
	HDC hdc;
	HDC hdcMem1;
	HDC hdcMem2;
	RECT rect;
	HBITMAP patternMask;

	if(! requiredFont)
		{
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
		requiredFont = CreateFontIndirect((LPLOGFONT)&lf);


		lf.lfItalic = TRUE;
		optionalFont = CreateFontIndirect((LPLOGFONT)&lf);

		lf.lfWeight = 800;
		boldOptional = CreateFontIndirect((LPLOGFONT)&lf);

		lf.lfItalic = 0;
		boldRequired = CreateFontIndirect((LPLOGFONT)&lf);

		arrowMask = LoadBitmap(PROGSYSTEM::hInst, "ParmArrowMask");
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
		if(!hdc)
			return FALSE;

		patternMask = CreateCompatibleBitmap(hdc, PARAM_ARROWWIDTH,
			PARAM_ARROWHEIGHT);

		if(!patternMask)
			return FALSE;

		hdcMem1 = CreateCompatibleDC(hdc);
		hdcMem2 = CreateCompatibleDC(hdc);

		if(!hdcMem1 || !hdcMem2)
			return FALSE;

		blackInArrow = CreateCompatibleBitmap(hdc, PARAM_ARROWWIDTH,
			PARAM_ARROWHEIGHT);

		if(!blackInArrow)
			return FALSE;

		//create a pattern with the arrow in white and the rest in black
		//the original mask has a black arrow with the rest in white
		SelectObject(hdcMem2, patternMask);
		SelectObject(hdcMem1, arrowMask);
		BitBlt(hdcMem2, 0, 0, PARAM_ARROWWIDTH, PARAM_ARROWHEIGHT, hdcMem1,
			0, 0, NOTSRCCOPY);

		//create the black arrow with the arrow the normaltext color and
		//the rest black
		SelectObject(hdcMem1, blackInArrow);
		rect.left = 0;
		rect.top = 0;
		rect.right = PARAM_ARROWWIDTH+1;
		rect.bottom = PARAM_ARROWHEIGHT + 1;
		FillRect(hdcMem1, (LPRECT)&rect, PROGSYSTEM::normalTextBrush);

		BitBlt(hdcMem1,0,0,PARAM_ARROWWIDTH,PARAM_ARROWHEIGHT,
			hdcMem2,0,0,SRCAND);

		//create the gray arrow with the arrow the graytext color and the
		//rest in black
		grayInArrow = CreateCompatibleBitmap(hdc, PARAM_ARROWWIDTH,
			PARAM_ARROWHEIGHT);

		if(!grayInArrow)
			return FALSE;

		rect.left = rect.top = 0;
		rect.right = PARAM_ARROWWIDTH;
		rect.bottom = PARAM_ARROWHEIGHT;
		SelectObject(hdcMem1, grayInArrow);
		FillRect(hdcMem1, &rect, PROGSYSTEM::grayTextBrush);
		BitBlt(hdcMem1,0,0,PARAM_ARROWWIDTH,PARAM_ARROWHEIGHT,
			hdcMem2,0,0,SRCAND);

		//clean up
		DeleteDC(hdcMem1);
		DeleteDC(hdcMem2);
		DeleteDC(hdc);
		DeleteObject(patternMask);

		}
	return TRUE;
	}

void PARAM::DrawHighlight(
	HDC hdc)
	{
	int oldDC;

	oldDC = SaveDC(hdc);

	SelectPen(hdc, PROGSYSTEM::highlightPen);
	SelectBrush(hdc, GetStockBrush(NULL_BRUSH));
	SetROP2(hdc, R2_NOTXORPEN);
	//extend the rectangle 1 passed the horizontal boundaries so we don't
	//overwrite part of the parameter name.
	Rectangle(hdc, location.left-1, location.top,
		location.right+1,location.bottom);
	RestoreDC(hdc, oldDC);
	}

void PARAM::AddLinkUID(
	SERUID dataUID)
	{
	SetDataFromUID(dataUID);
	}


void PARAM::ClearInfo()
	{
	connectCount--;
	if(connectCount==0)
		{
		name = paramDesc->pGetName();
		status&=(PAR_REQUIRED | PAR_OPTIONAL | PAR_INPUT | PAR_OUTPUT |
			PAR_CANBECONSTANT | PAR_TEMPOBJ | PAR_TEXTLEGAL);
		status|=(PAR_NOTLINKED | PAR_DELETED);
		AdjustWidth();
		//Clear dataFrom so that we won't think are name needs to change
		//if another parameter's or object's name changes which happens to be
		//the same ID as the old dataFrom.
		dataFrom = NULLSERUID;
		}
	else if(connectCount<0)
		connectCount = 0;
	}

void PARAM::HardClearInfo()
	{
	//force the connection count to go to 0 when ClearInfo
	//if called.
	connectCount = 1;

	I_ASSERT(paramDesc);
	typeID = paramDesc->type;
	
	ClearInfo();
	}

void PARAM::UpdateUIDs()
	{
	LPEXEPARAM exeParm;
	void * dataPtr;
  LPOBJINST pObjInst;

	exeParm = (LPEXEPARAM)GetObject(copyUID);
	if(TestLinkFlag() && (!TestConstantFlag()))
		{
		if(TestObjConnectFlag())
			{
			pObjInst = (LPOBJINST)GetObject(exeParm->GetDataFromUID());
			if (pObjInst->TestAliasFlag() && (pObjInst->GetSharedObjectUID() != 0))
        pObjInst = (LPOBJINST)GetObject(pObjInst->GetSharedObjectUID());
			exeParm->SetDataFromUID(pObjInst->copyUID);
			}
		else if(TestParamConnectFlag() && IsInput())
			{
			dataPtr = GetObject(exeParm->GetDataFromUID());
			exeParm->SetDataFromUID(((PARAM *)dataPtr)->copyUID);
			}
		}
	}

BOOL PARAM::ChangeName(
	HINSTANCE hInst,
	HWND ownerWindow)
	{
	int response;

	MDI::ModalizeVAB(TRUE);

	response = DialogBoxParam(hInst, "PARAMDLG",
		ownerWindow, (DLGPROC)PARAM::GetParamName, (long)this);

	MDI::ModalizeVAB(FALSE);

	if(response != -1 && response)
		{
		SetName((LPSTR)PARAM::dlgBuffer);
		return TRUE;
		}
	else
		return FALSE;

	}

BOOL PARAM::EnterConstant(
	HINSTANCE hInst,
	HWND ownerWindow)
	{
	int response;

	MDI::ModalizeVAB(TRUE);
	response = DialogBoxParam(hInst, "PARAMCONSTDLG",
		ownerWindow, (DLGPROC)PARAM::GetConstant, (long)this);
	MDI::ModalizeVAB(FALSE);

	if(response != -1 && response)
		{
		ClearInfo();
		AddInfo(PM_CONSTANT);
		SetName((LPSTR)PARAM::dlgBuffer);
		return TRUE;
		}
	else
		return FALSE;

	}



void PARAM::AddInfo(
	long info)
	{
	connectCount++;
	status&= ~(PAR_NOTLINKED|PAR_DELETED);
	status|=info;
	if(status & PAR_FROMPARAM)
		status|=PAR_SINGLEQUOTED;
	AdjustWidth();
	}


BOOL CALLBACK PARAM::GetConstant(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	PARAM * paramPtr;
	HWND		hEdit;

	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			paramPtr = (PARAM *)lParam;
			hEdit = GetDlgItem(hDlg, PAR_EDIT);

			if(paramPtr->TestConstantFlag())
				{
				Edit_SetText(hEdit, paramPtr->name);
				}
			SetDlgItemText(hDlg, PAR_PARMNAME, paramPtr->paramDesc->pGetName());
			Edit_LimitText(hEdit, MAX_PARAM_NAME-1);
			Edit_SetSel(hEdit, 0L, 0L);
			return TRUE;

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch(wID)
			{
				case ID_OK:
					GetDlgItemText(hDlg, PAR_EDIT,
						PARAM::dlgBuffer, MAX_PARAM_NAME-1);
					EndDialog(hDlg, TRUE);
					return(TRUE);

				case ID_CANCEL:
					EndDialog(hDlg, FALSE);
					return(TRUE);

			}
			break;
			}
		}
	return FALSE;
	}


BOOL CALLBACK PARAM::GetParamName(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	PARAM * paramPtr;
	HWND		hEdit;

	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			paramPtr = (PARAM *)lParam;
			hEdit = GetDlgItem(hDlg, PAR_EDIT);

			Edit_SetText(hEdit, paramPtr->name);
			Edit_LimitText(hEdit, MAX_PARAM_NAME-1);
			Edit_SetSel(hEdit, 0L, 0);
			return TRUE;

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch(wID)
			{
				case ID_OK:
					GetDlgItemText(hDlg, PAR_EDIT,
						PARAM::dlgBuffer, MAX_PARAM_NAME-1);
					EndDialog(hDlg, TRUE);
					return(TRUE);

				case ID_CANCEL:
					EndDialog(hDlg, FALSE);
					return(TRUE);

			}
			break;
			}
		}
	return FALSE;
	}

//return TRUE if the param description can be updated
short PARAM::UpdateParamDesc(
        LPAFUNCDESC funcDesc)
	{
	short rVal = PRJ_READNOERROR;

	if(status & PAR_INPUT)
		paramDesc =  funcDesc->GetInput(ordinal);
	else
		paramDesc = funcDesc->GetOutput(-1 * (ordinal+1));

	if(paramDesc)
		{
		//reset the required or optional flag on read from file in case the
		//configuration info has changed
		status &= ~(PAR_OPTIONAL | PAR_REQUIRED);
		if(paramDesc->TestOptionalFlag())
			status|=PAR_OPTIONAL;
		else
			status|=PAR_REQUIRED;
		

		//make sure the typeID matches the config info
		if(typeID != paramDesc->type)
			rVal = PRJ_READWARNING;

		DetermineTextValidity();
		}
	else
		rVal = PRJ_READERROR | PRJ_READBADPARAM;

	return rVal;
	}

void PARAM::DoPrint(
	LPPRINTER printPtr,
	int indent)
	{
	int size;
	LPSTR buffer;
	LPSTR paramType;
	TSTRING data;

	size = lstrlen(paramDesc->pGetName()) + 20;
	if(status & PAR_NOTLINKED)
		{
		if((status & PAR_OPTIONAL) || (status & PAR_OUTPUT))
			data = "<default>";
		else
			data = "<input required>";
		}
	else
		data = MakeName(NULL, FALSE);

	size+=data.strlen() + 2;
	if(status & PAR_INPUT)
		paramType = "in";
	else
		paramType = "out";
	buffer = (LPSTR) GlobalAlloc(GPTR, size);
	wsprintf(buffer, "[%s] %s: %s",
		paramType, paramDesc->pGetName(), (LPCSTR)data);
	printPtr->PrintLine(indent, buffer);
	GlobalFree((HGLOBAL) buffer);
	}

//only need to check if we're out of disk space when exporting the last
//field.
WORD PARAM::ExportUPSF()
	{
	long tempLong;
	int tempInt;
	FUNCINST * funcPtr;
	OBJINST * objPtr;
	PARAM * dstParam;
	LONG result;
	WORD returnValue = EXPORT_NOERROR;
	long upsfTypeID;

	//values for connection types were set by the Mac specifications
	if(TestParamConnectFlag())
		{
		tempLong = 4;
		if(IsInput())
			{
			dstParam = (PARAM *)GetObject(dataFrom);
			funcPtr = (FUNCINST *)GetObject(dstParam->ownerFunc);
			tempInt = funcPtr->GetSubjectPtr()->
				UPSFFuncToIndex(dstParam->ownerFunc);

			I_ASSERT(tempInt != SJT_UPSFINDEXERROR);
			}
		}
	else if(TestConstantFlag())
		tempLong = 3;
	else if(TestObjConnectFlag())
		{
		objPtr = (OBJINST *)GetObject(dataFrom);
		tempInt = objPtr->GetSubjectPtr()->UPSFObjToIndex(dataFrom);
		
		I_ASSERT(SJT_UPSFINDEXERROR);
		
		tempLong = 1;
		}
	else
		tempLong = 0;		//not connected to anything.

	I_ALWAYSASSERT(AUpsfExportField(PARAMETER_CONNECTTYPE, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempLong, (long)sizeof(long)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	if((TestParamConnectFlag() && IsInput()) || TestObjConnectFlag())
		{
		I_ALWAYSASSERT(AUpsfExportField(PARAMETER_CONNECTINDEX, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
			(LPVOID)&tempInt, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);
		}

	//export parameter code if we're an input connected to an output
	if(IsInput() && TestParamConnectFlag())
		{
		//dstParam should have been set previously
		tempInt = dstParam->GetOrdinal();
		
		I_ALWAYSASSERT(AUpsfExportField(PARAMETER_PARAM, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
			(LPVOID)&tempInt, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);
		}

	//export type of compatible object
	//if it's a general type (zero on our platform) convert it to a -1 for
	//the MAC
	upsfTypeID = typeID ? typeID : -1L;
	
	I_ALWAYSASSERT(AUpsfExportField(PARAMETER_TYPEID, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER,
		(LPVOID)&upsfTypeID, (long)sizeof(long)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);

	//export param name	and check to see if we're out of disk space
	result = AUpsfExportField(PARAMETER_NAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPVOID)((LPCSTR)name), (long)name.strlen());

	I_ASSERT(result == AUPSF_NOERROR);
	
	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	return returnValue;
	}

SERUID PARAM::PopUpParamList(
	HWND subjectWindow,
	RUNTIMEUID objectBlock)
	{
	int i=0;
	LPOBJINST objPtr;
	LPPOPUPMENU popupPtr;
	SERUID selectedUID = NULLSERUID;
	LPOBJINST dataObj;
	POINT popupCorner;
  HDC hdc;

	//create the popup as a message hog so we won't exit this routine until the popup
	//has been closed.
	popupPtr = new POPUPMENU(NULL, requiredFont, 
		POPUP_MESSAGEHOG | POPUP_RIGHTBUTTON | POPUP_SORTED);

	//fill the popup with the sorted list
	while(0 != (objPtr = (LPOBJINST)GetObject(objectBlock, i)))
		{
		if(ConnectionLegal(objPtr, NULL, TRUE))
			popupPtr->AppendItem(objPtr->GetName(), (long)objPtr, NO_ORDINAL);
		}

	if(TestObjConnectFlag())
		dataObj = (LPOBJINST)GetObject(dataFrom);
	else
		dataObj = NULL;

	//figure the upper left corner of the popupmenu in screen coordinates.
	popupCorner.x = location.left + PARAM_ARROWWIDTH - 1;
	popupCorner.y = location.top;
	hdc = GetDC(subjectWindow);
	LPtoDP(hdc, (LPPOINT)&popupCorner, 1);
	ClientToScreen(subjectWindow, (LPPOINT)&popupCorner);
	ReleaseDC(subjectWindow, hdc);

	//show the popup
	if(popupPtr->ShowPopup(popupCorner, (long)dataObj))
		{
		//get the uid of the selected object
		objPtr = (LPOBJINST)popupPtr->GetItemData();
		
		//return a successful find only if the match is different than
		//the original link.
		if(objPtr && (dataObj != objPtr))
			selectedUID = GetUID(objPtr);
		
		}

	delete popupPtr;

	return selectedUID;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			PARAM::ConnectionLegal
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PARAM::ConnectionLegal(
	LPOBJINST objPtr,
	LPPARAM paramPtr,
	BOOL ignoreConnectStatus)
	{
	BOOL rVal = FALSE;

	if(IsInput())
		{
			if((paramPtr && ValidInputOutputConnection(this, paramPtr, ignoreConnectStatus))	||			
				(objPtr && (!TestLinkFlag() || (ignoreConnectStatus && TestObjConnectFlag())) &&
				((typeID == objPtr->GetTypeID()) || (typeID == COMPUID_GENERAL) ||
				((status & PAR_CANBECONSTANT) && 
				objPtr->objDesc->pGetResponse(AEVENT_TEXTTOOBJECT)))))
				{
				rVal = TRUE;
				}
		}
	else
		{
		if((paramPtr && ValidInputOutputConnection(paramPtr, this,
			ignoreConnectStatus)) ||
			(objPtr && (!TestLinkFlag() || (ignoreConnectStatus && TestObjConnectFlag())) &&
			((typeID == objPtr->GetTypeID()) ||
			(typeID == COMPUID_GENERAL) ||
			(TestTextFlag() && objPtr->objDesc->pGetResponse(AEVENT_TEXTTOOBJECT)))))
			{
			rVal = TRUE;
			}
		}
	
	return rVal;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PARAM::ValidInputOutputConnection
//						 
// PARAMETERS:
//
// DESCRIPTION:		static routine
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL PARAM::ValidInputOutputConnection(
	LPPARAM input,
	LPPARAM output,
	BOOL ignoreConnectStatus)
	{
	return (((!input->TestLinkFlag() && !output->TestObjConnectFlag())
		|| ignoreConnectStatus) &&	(
		(input->GetTypeID() == output->GetTypeID()) ||
		(input->GetTypeID() == COMPUID_GENERAL || output->GetTypeID() ==
		COMPUID_GENERAL) ||
		((input->status & PAR_CANBECONSTANT) &&
		output->TestTextFlag()))) ? TRUE : FALSE;
		
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			PARAM::DetermineTextValidity
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void PARAM::DetermineTextValidity()
	{
	
	LPOBJDESC objDesc;

	if((typeID == COMPUID_GENERAL) ||
		((0 != (objDesc = cfgGetObjectDesc(typeID))) &&
		objDesc->pGetResponse(AEVENT_TEXTTOOBJECT)))
		{
		status |= PAR_TEXTLEGAL;
		}
	else
		{
		status &= ~PAR_TEXTLEGAL;
		}

	}
