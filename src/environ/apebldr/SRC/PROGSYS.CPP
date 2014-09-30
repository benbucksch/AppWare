#include <stdlib.h>
#include "almsys.h"
#include <windowsx.h>

#include "progsys.h"
#include "exeinfo.h"
#include "oipexe.h"
#include "cliphead.h"
#include "rectypes.h"
#include "clipsjt.h"
#include "clipparm.h"
#include "clipfunc.h"
#include "clipflow.h"
#include "clipObj.h"
#include "clipcom.h"
#include "wrksheet.h"
#include "printer.h"
#include "seruid.h"
#include "memlib.h"
#include "mdiclass.h"
#include "prefdlg.h"
#include "project.h"
#include "intdebug.h"
#include "mtypelst.h"
#include "objdesc.h"
#include "cfglibp.h"
#include "resconst.h"
#include "oiputils.h"
#include "a_almutl.h"
#include "oipdebug.h"

#define SPLASH_CLASS_NAME	("ock_SplashClass")
#define VAB_DEF_HELPFILE ("appware.hlp")

HINSTANCE     PROGSYSTEM::hInst      = NULL;
HWND    	    PROGSYSTEM::hWndFrame  = NULL;
HWND      		PROGSYSTEM::hWndClient = NULL;
HINSTANCE    	PROGSYSTEM::hPrevInst  = NULL;
LPSTR        	PROGSYSTEM::lpCmdParam = NULL;
int         	PROGSYSTEM::CmdShow    = NULL;
LPFILEOBJECT 	PROGSYSTEM::delFileObj;
LPFILEOBJECT 	PROGSYSTEM::clipFileObj;
DWORD        	PROGSYSTEM::status;
DWORD				 	PROGSYSTEM::preferences;
FILEIDSTAMP		PROGSYSTEM::undoPrjID = NULL;
SERUID       	PROGSYSTEM::undoSjtUID = NULLSERUID;
EXEINFOAUX   	PROGSYSTEM::exeInfoAux;
EXEINFO      	PROGSYSTEM::exeInfo(&exeInfoAux);
HWND				 	PROGSYSTEM::hWndSplash=NULL;
BOOL    		 	PROGSYSTEM::splashRegistered=FALSE;
HWND				 	PROGSYSTEM::activeEditField = NULL;
DEBUGPROC		 	PROGSYSTEM::debugProcAddr;
SHOWVABPROC		PROGSYSTEM::showVABProcAddr;
OBJCREATEPROC	PROGSYSTEM::objCreateProcAddr;
WNDPROC				PROGSYSTEM::oldPrefEditProcAddr;
OBJDESTROYPROC 	PROGSYSTEM::objDestroyProcAddr;
OBJSETNAMEPROC	PROGSYSTEM::objSetNameProcAddr;

COLORREF			PROGSYSTEM::grayTextColor = NULL;
COLORREF 			PROGSYSTEM::normalTextColor = NULL;
COLORREF			PROGSYSTEM::dragColor = NULL;
COLORREF			PROGSYSTEM::highlightColor = NULL;
COLORREF			PROGSYSTEM::windowColor = NULL;
COLORREF			PROGSYSTEM::captionColor=NULL;
COLORREF			PROGSYSTEM::frameColor = NULL;
COLORREF			PROGSYSTEM::captionTextColor=NULL;
COLORREF			PROGSYSTEM::highlightTextColor=NULL;
COLORREF			PROGSYSTEM::inactiveCaptionColor;
COLORREF 			PROGSYSTEM::inactiveCaptionTextColor;

//the normalTextBrush MUST be set to NULL as an if statement in
//SetSysColors depends upon it.
HBRUSH 				PROGSYSTEM::normalTextBrush = NULL;
HBRUSH 				PROGSYSTEM::grayTextBrush = NULL;
HBRUSH 				PROGSYSTEM::dragBrush = NULL;
HBRUSH 				PROGSYSTEM::highlightBrush = NULL;
HBRUSH 				PROGSYSTEM::windowBrush = NULL;
HBRUSH				PROGSYSTEM::captionBrush=NULL;
HBRUSH				PROGSYSTEM::inactiveCaptionBrush=NULL;

HPEN					PROGSYSTEM::framePen=NULL;
HPEN 					PROGSYSTEM::normalTextPen = NULL;
HPEN 					PROGSYSTEM::grayTextPen = NULL;
HPEN 					PROGSYSTEM::dragPen = NULL;
HPEN 					PROGSYSTEM::dottedDragPen = NULL;
HPEN				 	PROGSYSTEM::highlightPen = NULL;
HPEN 					PROGSYSTEM::windowPen = NULL;
HPEN					PROGSYSTEM::focusPen = NULL;

////////////////////////////////////////////////////////////////
HICON					PROGSYSTEM::vabIcon=NULL;
HICON					PROGSYSTEM::prjIcon=NULL;
HICON					PROGSYSTEM::sjtIcon=NULL;

//magnitude of project and subject grid
int  PROGSYSTEM::sjtHGrid;
int  PROGSYSTEM::sjtVGrid;
int  PROGSYSTEM::prjHGrid;
int  PROGSYSTEM::prjVGrid;

//help file for visual appbuilder
char 	PROGSYSTEM::VABHelpFile[MAX_PATH + 1];

LPDIBITMAP PROGSYSTEM::diSplash = NULL;

//minimum distance to move before dragging is recognized
#define PROGSYS_MINMOVEDELTA 3
int	PROGSYSTEM::minMoveDelta;

PROJECT * PROGSYSTEM::GetCurrentProject()
	{
	if (WORKSHEET::WorksheetExists())
		return (PROJECT *)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
	else
		return NULL;
	}

void PROGSYSTEM::SetUpCompileContext()
	{
	exeInfo.version                   = EXEINFO_VERSION_NOW;
	exeInfo.flags                     = isCompile;
	exeInfo.pAuxInfo->fileObj         = pTheSystem->GetFileObject();
	exeInfo.pAuxInfo->seriusMem       = pTheSystem->GetTheMemory();
	exeInfo.pAuxInfo->tempStringSpace = pTheSystem->GetTempStringSpace();
	exeInfo.pAuxInfo->lpExeCmdLine    = NULL;
  
	// Pass memlib globals out to the oip...
	exeInfo.pAuxInfo->globalBlockAddr = GetGlobalBlockArrayAddr();
	exeInfo.pAuxInfo->globalBlockSize = GetGlobalBlockArraySize();

	// Pass other globals out to the kernel
	exeInfo.pAuxInfo->masterTypeList =  pTheSystem->GetMasterTypeList();

	exeInfo.pAuxInfo->debugger        = 0L;
	exeInfo.pAuxInfo->pfnObjCreate    = objCreateProcAddr;
	exeInfo.pAuxInfo->pfnObjDestroy   = objDestroyProcAddr;
	exeInfo.pAuxInfo->pfnObjSetName		= objSetNameProcAddr;
	exeInfo.pAuxInfo->pfnShowVAB			= showVABProcAddr;
	exeInfo.pAuxInfo->hVABFrame       = hWndFrame;
	}

void PROGSYSTEM::SetUpRunContext()
	{
	exeInfo.version                   = EXEINFO_VERSION_NOW;
	exeInfo.flags                     = isRuntime;
	exeInfo.pAuxInfo->fileObj         = pTheSystem->GetFileObject();
	exeInfo.pAuxInfo->seriusMem       = pTheSystem->GetTheMemory();
	exeInfo.pAuxInfo->tempStringSpace = pTheSystem->GetTempStringSpace();

	exeInfo.pAuxInfo->lpExeFileName   = 0;
	exeInfo.pAuxInfo->lpExeCmdLine    = 0;

	// Pass memlib globals out to the oip...
	exeInfo.pAuxInfo->globalBlockAddr = GetGlobalBlockArrayAddr();
	exeInfo.pAuxInfo->globalBlockSize = GetGlobalBlockArraySize();

	// Pass other globals out to the kernel
	exeInfo.pAuxInfo->masterTypeList = NULL;
	
	exeInfo.pAuxInfo->debugger        = debugProcAddr;
	exeInfo.pAuxInfo->pfnShowVAB			= NULL;
	exeInfo.pAuxInfo->pfnObjCreate    = NULL;
	exeInfo.pAuxInfo->pfnObjDestroy 	= NULL;
	exeInfo.pAuxInfo->pfnObjSetName		= NULL;
	exeInfo.pAuxInfo->hVABFrame       = NULL;
	}

void PROGSYSTEM::AddDebugInfo()
	{
	exeInfo.flags |= isDebug;
	}

void PROGSYSTEM::AddRunInfo()
	{
	exeInfo.flags |= isRuntime;
	exeInfo.pAuxInfo->pfnObjCreate    = NULL;
	exeInfo.pAuxInfo->pfnObjDestroy 	= NULL;
	exeInfo.pAuxInfo->pfnObjSetName		= NULL;
	exeInfo.pAuxInfo->pfnShowVAB		  = NULL;
	exeInfo.pAuxInfo->masterTypeList  = NULL;
	exeInfo.pAuxInfo->hVABFrame       = NULL;
	}

void PROGSYSTEM::AddCompileInfo()
	{
	exeInfo.flags |= isCompile;
	}

void PROGSYSTEM::AddAtStopInfo()
	{
	exeInfo.flags |= isAtStop;
	}

void PROGSYSTEM::ClearRunInfo()
	{
	exeInfo.flags &= ~isRuntime;
	exeInfo.pAuxInfo->pfnObjCreate  = objCreateProcAddr;
	exeInfo.pAuxInfo->pfnObjDestroy = objDestroyProcAddr;
	exeInfo.pAuxInfo->pfnObjSetName = objSetNameProcAddr;
	exeInfo.pAuxInfo->pfnShowVAB    = showVABProcAddr;
	exeInfo.pAuxInfo->masterTypeList = pTheSystem->GetMasterTypeList();
	exeInfo.pAuxInfo->hVABFrame      = hWndFrame;
	}

void PROGSYSTEM::ClearCompileInfo()
	{
	exeInfo.flags &= ~isCompile;
	}

void PROGSYSTEM::ClearAtStopInfo()
	{
	exeInfo.flags &= ~isAtStop;
	}

BOOL PROGSYSTEM::TestAtStopFlag()
	{
	return (0 != (exeInfo.flags & isAtStop));
	}

void PROGSYSTEM::SetContinueAppFlag()
	{
	exeInfo.flags |= isContinue;
	}

void PROGSYSTEM::ClearContinueAppFlag()
	{
	exeInfo.flags &= !isContinue;
	}

BOOL PROGSYSTEM::TestContinueAppFlag()
	{
	return (0 != (exeInfo.flags & isContinue));
	}

void PROGSYSTEM::RegisterNewContext()
	{
  if(TestSingleStepFlag())
  	exeInfo.flags |= doSingleStep;
    
	exeRegisterContext(&exeInfo);
  
  exeInfo.flags &= ~doSingleStep;
	}

LPFILEOBJECT PROGSYSTEM::GetRunContext()
	{
	return exeInfo.pAuxInfo->fileObj;
	}

void PROGSYSTEM::RegisterFileObj(
	LPFILEOBJECT fileObjPtr)
	{
	EXEINFOAUX	tempAuxInfo;
	EXEINFO			tempExeInfo(&tempAuxInfo);

	//If were debugging, let oip know this so we can examine object values
	if(fileObjPtr == PROGSYSTEM::exeInfo.pAuxInfo->fileObj && TestRunTimeFlag())
		tempExeInfo.flags = isRuntime;
	else
		tempExeInfo.flags = 0;

	tempExeInfo.version                   = EXEINFO_VERSION_NOW;
	tempExeInfo.pAuxInfo->fileObj         = fileObjPtr;
	tempExeInfo.pAuxInfo->seriusMem       = pTheSystem->GetTheMemory();
	tempExeInfo.pAuxInfo->tempStringSpace = pTheSystem->GetTempStringSpace();
	tempExeInfo.pAuxInfo->lpExeFileName   = 0;
	tempExeInfo.pAuxInfo->lpExeCmdLine    = 0;

	// Pass memlib globals out to the oip...
	tempExeInfo.pAuxInfo->globalBlockAddr = GetGlobalBlockArrayAddr();
	tempExeInfo.pAuxInfo->globalBlockSize = GetGlobalBlockArraySize();

	tempExeInfo.pAuxInfo->debugger = debugProcAddr;
	
	//see if we're a running context
	if(TestRunTimeFlag() && GetRunTimeFileID() == fileObjPtr->GetFileIDStamp())
		{
		tempExeInfo.pAuxInfo->masterTypeList = NULL;
 		tempExeInfo.pAuxInfo->pfnObjCreate   = NULL;
		tempExeInfo.pAuxInfo->pfnObjDestroy  = NULL;
		tempExeInfo.pAuxInfo->pfnObjSetName  = NULL;
		tempExeInfo.pAuxInfo->pfnShowVAB     = NULL;
		tempExeInfo.pAuxInfo->hVABFrame      = NULL;
		}
	else
		{
		// Pass other globals out to the kernel
		tempExeInfo.pAuxInfo->masterTypeList = pTheSystem->GetMasterTypeList();
 		tempExeInfo.pAuxInfo->pfnObjCreate   = objCreateProcAddr;
		tempExeInfo.pAuxInfo->pfnObjDestroy  = objDestroyProcAddr;
		tempExeInfo.pAuxInfo->pfnObjSetName  = objSetNameProcAddr;
		tempExeInfo.pAuxInfo->pfnShowVAB     = showVABProcAddr;
		tempExeInfo.pAuxInfo->hVABFrame      = hWndFrame;
		}

	exeRegisterContext((LPEXEINFO)&tempExeInfo);
	}

void PROGSYSTEM::SetContext(
	PROJECT * projPtr)
	{
	pTheSystem->SetFileObject(projPtr->fileObjPtr);
	RegisterFileObj(projPtr->fileObjPtr);
	}

BOOL PROGSYSTEM::InitSystem()
	{
	status = 0L;
	preferences = 0L;
	//PROJECTGRID defined in wrksheet.h
	sjtHGrid = sjtVGrid = prjHGrid = prjVGrid = PROJECTGRID;
	InitClipboard();
	SetAutoSaveFlag();
	SetConfirmAutoSaveFlag();

	debugProcAddr      = (DEBUGPROC)      MDI::EnterDebugger;
	objCreateProcAddr  = (OBJCREATEPROC)  PROGSYSTEM::CreateDynamicObject;
	objDestroyProcAddr = (OBJDESTROYPROC) PROGSYSTEM::DestroyObject;
	objSetNameProcAddr = (OBJSETNAMEPROC) PROGSYSTEM::RenameObject;
	showVABProcAddr    = (SHOWVABPROC)    MDI::ShowAppBuilder;
	
	if(!PROGSYSTEM::SetSysColors())
		return FALSE;

	ReadPreferences();
	//no errors if we get to here

	if(!BuildMasterTypeList())
		return FALSE;

	//tests are done on this location to see if 
	//we're at runtime.
	exeInfo.flags =0;

	return TRUE;
	}

void PROGSYSTEM::InitClipboard()
	{
	ClearUndoIDs();

	PROGSYSTEM::delFileObj = (LPFILEOBJECT)new FILEOBJECT();

	PROGSYSTEM::delFileObj->SetHeadBlock(GetUID(
		new RECORDBLOCK(sizeof(CLIPHEAD), RT_CLIPHEAD)));

	PROGSYSTEM::delFileObj->SetHeadRec(GetUID(
		new(PROGSYSTEM::delFileObj->GetHeadBlock()) CLIPHEAD()));

	PROGSYSTEM::clipFileObj = (LPFILEOBJECT)new FILEOBJECT();

	PROGSYSTEM::clipFileObj->SetHeadBlock(GetUID(
		new RECORDBLOCK(sizeof(CLIPHEAD), RT_CLIPHEAD)));

	PROGSYSTEM::clipFileObj->SetHeadRec(GetUID(
		new(PROGSYSTEM::clipFileObj->GetHeadBlock()) CLIPHEAD()));
  }

void PROGSYSTEM::PurgeDelFileObj()
  {
  SetDelContext();
  PurgeFileObj();
	PopAndRegisterContext();
	}

void PROGSYSTEM::PurgeClipFileObj()
  {
  SetClipContext();
  PurgeFileObj();
	PopAndRegisterContext();
  }

void PROGSYSTEM::PurgeFileObj()
	{
  delete (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadRec());

	pTheSystem->GetFileObject()->SetHeadRec(GetUID(
		new(PROGSYSTEM::clipFileObj->GetHeadBlock()) CLIPHEAD()));
	}

LPFILEOBJECT PROGSYSTEM::GetDelContext()
	{
	return PROGSYSTEM::delFileObj;
	}

void PROGSYSTEM::SetDelContext()
  {
	PushAndRegisterContext(PROGSYSTEM::delFileObj);
  }

void PROGSYSTEM::SetClipContext()
  {
	PushAndRegisterContext(PROGSYSTEM::clipFileObj);
	}

LPFILEOBJECT PROGSYSTEM::PopAndRegisterContext()
  {
  LPFILEOBJECT fileObj;

  fileObj=pTheSystem->PopContext();
	RegisterFileObj(pTheSystem->GetFileObject());
  return fileObj;
  }

void PROGSYSTEM::PushAndRegisterContext(
	LPFILEOBJECT pushObject)
  {
  pTheSystem->PushContext(pushObject);
	RegisterFileObj(pushObject);
	}

void PROGSYSTEM::CleanUpSystem()
	{
	PRINTER::PrinterCleanUp();

	CleanResources();

	DestroyMasterTypeList();
	}

//private routine to delete brushes and stuff
void PROGSYSTEM::CleanResources()
	{
	if(normalTextBrush)
		DeleteObject(normalTextBrush);
	if(grayTextBrush)
		DeleteObject(grayTextBrush);
	if(dragBrush)
		DeleteObject(dragBrush);
	if(highlightBrush)
		DeleteObject(highlightBrush);
	if(windowBrush)
		DeleteObject(windowBrush);
	if(captionBrush)
		DeleteObject(captionBrush);
	if(inactiveCaptionBrush)
		DeleteObject(inactiveCaptionBrush);

	if(normalTextPen)
		DeleteObject(normalTextPen);
	if(grayTextPen)
		DeleteObject(grayTextPen);
	if(dragPen)
		DeleteObject(dragPen);
	if(dottedDragPen)
		DeleteObject(dottedDragPen);
	if(highlightPen)
		DeleteObject(highlightPen);
	if(windowPen)
		DeleteObject(windowPen);
	if(framePen)
		DeleteObject(framePen);
	if(focusPen)
		DeleteObject(focusPen);

	if(vabIcon)
		DestroyIcon(vabIcon);

	if(prjIcon)
		DestroyIcon(prjIcon);

	if(sjtIcon)
		DestroyIcon(sjtIcon);

	//normalTextBrush must be set to Null as comparisons
	//in SetSysColors depend upon this.
	normalTextBrush = NULL;
	}


void PROGSYSTEM::ShowSplashScreen()
	{
	WNDCLASS wc;
	RECT clientRect;

	if(!splashRegistered)
		{
		splashRegistered = TRUE;

		wc.style 					= 0;
		wc.lpfnWndProc 		= PROGSYSTEM::SplashWndProc;
		wc.cbClsExtra 		= 0;
		wc.cbWndExtra 		= 0;
		wc.hInstance 			= PROGSYSTEM::hInst;
		wc.hIcon 					= NULL;
		wc.hCursor 				= LoadCursor(NULL, IDC_WAIT);
		wc.hbrBackground	= GetStockBrush(NULL_BRUSH);
		wc.lpszMenuName 	= NULL;
		wc.lpszClassName 	= SPLASH_CLASS_NAME;

		RegisterClass((LPWNDCLASS)&wc);
		}

	diSplash = new DIBITMAP((LPSTR)PROGSYSTEM::GetSplashResourceName(), hInst);

	I_ASSERT(diSplash->Valid());

	if(diSplash->Valid())
		{
		clientRect.left = (GetSystemMetrics(SM_CXSCREEN) - diSplash->Width())/2;
		clientRect.top =	(GetSystemMetrics(SM_CYSCREEN)- diSplash->Height())/2;
		clientRect.right = clientRect.left + diSplash->Width();
		clientRect.bottom = clientRect.top + diSplash->Height();
		AdjustWindowRect((LPRECT)&clientRect, WS_POPUP, FALSE);

		hWndSplash = CreateWindow(SPLASH_CLASS_NAME,(LPSTR)"",
			WS_POPUP|WS_VISIBLE,
			clientRect.left,
			clientRect.top,
			clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top,
			NULL,
			NULL,
			PROGSYSTEM::hInst,
			NULL);
		UpdateWindow(hWndSplash);
		}
	else
		delete diSplash;

	}

void PROGSYSTEM::HideSplashScreen()
	{
	if(hWndSplash)
		DestroyWindow(hWndSplash);

	if(diSplash)
		{
		delete diSplash;
		diSplash = NULL;
		}

	hWndSplash = NULL;
	}


LRESULT CALLBACK PROGSYSTEM::SplashWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	HDC hdc;
	PAINTSTRUCT ps;
	BOOL palChanged;

	switch(message)
		{
		case WM_PAINT:
			I_ASSERT(diSplash != NULL);
			
			hdc = BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
			diSplash->Draw(hdc, 0, 0);
			EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
			return 0;

		case WM_PALETTECHANGED:
			if(((HWND) wParam) == hwnd)
				break;
			//else fall though
		case WM_QUERYNEWPALETTE:
			hdc = GetDC(hwnd);
			palChanged = diSplash->DrawIfPaletteChanged(hdc, 0, 0);
			ReleaseDC(hwnd, hdc);
			
			if(palChanged)
				return TRUE;

			break;


		}


	return DefWindowProc(hwnd, message, wParam, lParam);
	}


//It is assumed that drag colors will be drawn with a NOTXOR operation.
//The reason for this is that using a NOTXOR operation to draw any color
//on white will result in that particular color.  White on white will
//result in black.
//Alos, white seems to be a very common window background.
BOOL PROGSYSTEM::SetSysColors()
	{
	BOOL result = TRUE; //assume no error

	if(normalTextBrush)
		CleanResources();

	windowColor = GetSysColor(COLOR_WINDOW);
	grayTextColor = GetSysColor(COLOR_GRAYTEXT);

	//If the gray text is the same color as the window, change it
	if(grayTextColor == windowColor)
		{
		//The graytext color is the same as the window so:
		//If the grayText isn't the light gray then set it to the light gray.
		//If the grayText is light gray, then set it to darkgray
		if(grayTextColor != RGB(192,192,192))
			grayTextColor = RGB(192,192,192);
		else
			grayTextColor = RGB(128, 128, 128);
		}

	//Apply the same logic as above for the normalTextColor
	normalTextColor = GetSysColor(COLOR_WINDOWTEXT);
	if(normalTextColor == windowColor)
		{
		if(normalTextColor != RGB(0,0,0))
			normalTextColor = RGB(0,0,0);
		else
			normalTextColor = RGB(255,255,255);
		}

	dragColor = RGB(0,0,0); //set the drag color to black which will be OK as
													//as long as a NOTXOR op is used

	highlightColor = GetSysColor(COLOR_HIGHLIGHT);
	//Make sure that both the highlight color and window color are not the
	//same since the highlight color will sometimes be drawn onto  the window
	//using a copy operation.
	if(highlightColor ==  windowColor)
		{
		if(windowColor != RGB(0,0,0))
			highlightColor = RGB(0,0,0);
		else
			//Set the highlight to light gray since we know the window is black.
			//NOTE: don't set the highlight to white since if an NOTXOR op is
			//used to draw the result will be black!
			highlightColor = RGB(192,192,192);
		}

	//make sure both the highlightTextColor and highlightColor are not the
	//same.
	highlightTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	if(highlightTextColor == highlightColor)
		{
		if(highlightTextColor != RGB(0,0,0))
			highlightTextColor = RGB(0,0,0);
		else
			highlightTextColor = RGB(192,192,192);
		}

	frameColor =  GetSysColor(COLOR_WINDOWFRAME);
	captionColor = GetSysColor(COLOR_ACTIVECAPTION);
	captionTextColor = GetSysColor(COLOR_CAPTIONTEXT);
	inactiveCaptionColor = GetSysColor(COLOR_INACTIVECAPTION);
	inactiveCaptionTextColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);


	//Create all the system Brushes and Pens
	normalTextBrush = CreateSolidBrush(normalTextColor);
	if(!normalTextBrush)
		result = FALSE;

	grayTextBrush = CreateSolidBrush(grayTextColor);
	if(!grayTextBrush)
		result = FALSE;

	dragBrush = CreateSolidBrush(dragColor);
	if(!dragBrush)
		result = FALSE;

	highlightBrush = CreateSolidBrush(highlightColor);
	if(!highlightBrush)
		result = FALSE;

	windowBrush = CreateSolidBrush(windowColor);
	if(!windowBrush)
		result = FALSE;

	captionBrush = CreateSolidBrush(captionColor);
	if(!captionBrush)
		result = FALSE;

	inactiveCaptionBrush = CreateSolidBrush(inactiveCaptionColor);
	if(!inactiveCaptionBrush)
		result = FALSE;

	normalTextPen = CreatePen(PS_SOLID, 1, normalTextColor);
	if(!normalTextPen)
		result = FALSE;

	grayTextPen = CreatePen(PS_SOLID, 1, grayTextColor);
	if(!grayTextPen)
		result = FALSE;

	dragPen = CreatePen(PS_SOLID, 1, dragColor);
	if(!dragPen)
		result = FALSE;

	dottedDragPen = CreatePen(PS_DOT, 1, dragColor);
	if(!dottedDragPen)
		result = FALSE;

	highlightPen = CreatePen(PS_SOLID, 1, highlightColor);
	if(!highlightPen)
		result = FALSE;

	focusPen = CreatePen(PS_DOT, 1, highlightColor);
	if(!focusPen)
		result = FALSE;

	windowPen = CreatePen(PS_SOLID, 1, windowColor);
	if(!windowPen)
		result = FALSE;

	framePen = CreatePen(PS_SOLID, 1, frameColor);
	if(!framePen)
		result = FALSE;

	vabIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDIC_APPWARE));
	if(!vabIcon)
		result = FALSE;

	prjIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDIC_PROJECT));
	if(!prjIcon)
		result = FALSE;

	sjtIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDIC_SUBJECT));
	if(!sjtIcon)
		result = FALSE;

	if(!result)
		{
		MessageBeep(0);
		MessageBox(NULL, "There are not enough system resources"
			" available to continue running Sersius Workshop.",
			"Fatal Application Error", MB_OK|MB_TASKMODAL|
			MB_ICONEXCLAMATION);
		}
	return result;
	}

//Preference Stuff
////////////////////////////////////////////////////////////////////
//String constants used in preference file
static char PREF_ENVIRONKEY[] = 				 	{"Environment"};
static char PREF_COMPILERKEY[] =					{"Compiler"}; 							
static char PREF_ON[]=         						{"on"};
static char PREF_OFF[]=										{"off"};
static char PREFSTRING_AUTOSAVE[]=				{"Auto-Save"};
static char PREFSTRING_CONFIRMSAVES[]=		{"Confirm Auto-Saves"};
static char PREFSTRING_NAVIGATOR[]=				{"Show Navigator"};
static char PREFSTRING_SJTGRID[]=					{"Subject Grid"};
static char PREFSTRING_SJTHGRID[]=				{"Subject Grid Horizontal Spacing"};
static char PREFSTRING_SJTVGRID[]=				{"Subject Grid Vertical Spacing"};
static char PREFSTRING_PRJGRID[]=					{"Project Grid"};
static char PREFSTRING_PRJHGRID[]=				{"Project Grid Horizontal Spacing"};
static char PREFSTRING_PRJVGRID[]=				{"Project Grid Vertical Spacing"};
static char PREFSTRING_SIGNALNAMES[]=			{"Show Signal Names"};
static char PREFSTRING_FUNCNAMES[]=				{"Show Function Names"};
static char PREFSTRING_LARGEICONS[]=		 	{"Large Object Icons"};
static char PREFSTRING_SIGNALVALUES[]= 	 	{"Object Signal Values"};
static char	PREFSTRING_BUFFEREDREDRAW[]= 	{"Buffered Redraw"};
static char	PREFSTRING_MINMOVEDELTA[]=   	{"Delta Distance"};
static char PREFSTRING_MAKEBACKUPS[]=		 	{"Backup Before Saving"};
static char PREFSTRING_VABHELPFILE[]=		 	{"AppWare Help File"};
static char PREFSTRING_UNINITPARAMS[]=		{"Uninitialized Parameter Warning"};

/////////////////////////////////////////////////////////////////////
void PROGSYSTEM::SetPreferences()
	{
	char		buffer[64];
  HKEY vabKey;
  HKEY environKey=NULL;
  HKEY compilerKey=NULL;
  DWORD action;

	if(DialogBox(PROGSYSTEM::hInst, "PREFDIALOG", GetActiveWindow(),
						(DLGPROC)PROGSYSTEM::PreferenceDlgProc))
		{
	
		//get key to where enironment preferences go
  	if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER)) != NULL)  	
    	{
    	RegCreateKeyEx(vabKey, PREF_ENVIRONKEY, 0, NULL,
  			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &environKey, &action);
    	
    	RegCreateKeyEx(vabKey, PREF_COMPILERKEY, 0, NULL,
      	REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &compilerKey, &action);
    	}
      
		//set all the preference strings in the INI_FILE_NAME file

		if(TestAutoSavePref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);
		
		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_AUTOSAVE, buffer));

		if(TestConfirmSavePref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_CONFIRMSAVES, buffer));

		if(TestShowNavigatorPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_NAVIGATOR, buffer));

		if(TestSjtGridPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_SJTGRID, buffer));

		I_VERIFY(AUtlWriteKeyLong(environKey, PREFSTRING_SJTHGRID, sjtHGrid));

		I_VERIFY(AUtlWriteKeyLong(environKey, PREFSTRING_SJTVGRID, sjtVGrid));

	  I_VERIFY(AUtlWriteKeyLong(environKey, PREFSTRING_MINMOVEDELTA, minMoveDelta));


		if(TestPrjGridPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_PRJGRID, buffer));

		I_VERIFY(AUtlWriteKeyLong(environKey, PREFSTRING_PRJHGRID, prjHGrid));

		I_VERIFY(AUtlWriteKeyLong(environKey, PREFSTRING_PRJVGRID, prjVGrid));

		if(TestSignalNamesPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_SIGNALNAMES, buffer));

		if(TestFuncNamesPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_FUNCNAMES, buffer));

		if(TestLargeIconsPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_LARGEICONS, buffer));

		if(TestObjSignalValuesPref())
    	lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_SIGNALVALUES, buffer));

		if(TestBufferedRedrawPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_BUFFEREDREDRAW, buffer));

		if(TestMakeBackUpsPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_MAKEBACKUPS, buffer));

		I_VERIFY(AUtlWriteKeyString(environKey, PREFSTRING_VABHELPFILE, VABHelpFile));
   
		if(TestUninitParamsWarningPref())
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);

		I_VERIFY(AUtlWriteKeyString(compilerKey, PREFSTRING_UNINITPARAMS, buffer));
   	
   	if(environKey)
  		RegCloseKey(environKey);
      
    if(compilerKey)
    	RegCloseKey(compilerKey);
    
  	if(vabKey)
  		AUtlCloseProductSubKey(vabKey);
  
		}
	
	}
  
////////////////////////////////////////////////////////////////////
void PROGSYSTEM::ReadPreferences()
	{
	char buffer[64];
  HKEY vabKey;
  HKEY environKey=NULL;
  HKEY compilerKey = NULL;
	
	//get key to where enironment preferences go
  if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER)) != NULL)  	
    {
    RegOpenKeyEx(vabKey, PREF_ENVIRONKEY, 0, KEY_ALL_ACCESS, &environKey);
    RegOpenKeyEx(vabKey, PREF_COMPILERKEY, 0, KEY_ALL_ACCESS, &compilerKey);
    }
	
	//Do non-case sensitive string comparison by using lstrcmpi


	AUtlGetKeyString(environKey, PREFSTRING_AUTOSAVE,
		buffer, sizeof(buffer), PREF_ON);

	if(!lstrcmpi(buffer, PREF_ON))
		{
		SetAutoSavePref();
		SetAutoSaveFlag();
		}
	else
		{
		ClearAutoSavePref();
		ClearAutoSaveFlag();
		}

	AUtlGetKeyString(environKey, PREFSTRING_CONFIRMSAVES, buffer,
		sizeof(buffer), PREF_ON);

	if(!lstrcmpi(buffer, PREF_ON))
		{
		SetConfirmSavePref();
		SetConfirmAutoSaveFlag();
		}
	else
		{
		ClearConfirmSavePref();
		ClearConfirmAutoSaveFlag();
		}
	
	AUtlGetKeyString(environKey, PREFSTRING_NAVIGATOR,
		buffer, sizeof(buffer), PREF_OFF);

	if(!lstrcmpi(buffer, PREF_ON))
		SetShowNavigatorPref();
	else
		ClearShowNavigatorPref();

	AUtlGetKeyString(environKey, PREFSTRING_SJTGRID, buffer,
		sizeof(buffer), PREF_OFF);

	if(!lstrcmpi(buffer, PREF_ON))
		SetSjtGridPref();
	else
		ClearSjtGridPref();

	sjtHGrid = (WORD)AUtlGetKeyLong(environKey, PREFSTRING_SJTHGRID, PROJECTGRID);

	sjtVGrid = (WORD)AUtlGetKeyLong(environKey, PREFSTRING_SJTVGRID, PROJECTGRID);

	minMoveDelta = AUtlGetKeyLong(environKey, PREFSTRING_MINMOVEDELTA,
		PROGSYS_MINMOVEDELTA);

	if(minMoveDelta < 0)
		minMoveDelta = PROGSYS_MINMOVEDELTA;

	AUtlGetKeyString(environKey, PREFSTRING_PRJGRID, buffer,
		sizeof(buffer), PREF_OFF);

	if(!lstrcmpi(buffer, PREF_ON))
		SetPrjGridPref();
	else
		ClearPrjGridPref();

	prjHGrid = (WORD)AUtlGetKeyLong(environKey, PREFSTRING_PRJHGRID, PROJECTGRID);

	prjVGrid = (WORD)AUtlGetKeyLong(environKey, PREFSTRING_PRJVGRID, PROJECTGRID);

	AUtlGetKeyString(environKey, PREFSTRING_SIGNALNAMES,
		buffer, sizeof(buffer), PREF_ON);

	if(!lstrcmpi(buffer, PREF_ON))
		SetSignalNamesPref();
	else
		ClearSignalNamesPref();

	AUtlGetKeyString(environKey, PREFSTRING_FUNCNAMES,
		buffer, sizeof(buffer), PREF_OFF);

	if(!lstrcmpi(buffer, PREF_ON))
		{
		SetFuncNamesPref();
		SetFuncNamesFlag();
		}
	else
		{
		ClearFuncNamesPref();
		ClearFuncNamesFlag();
		}

	AUtlGetKeyString(environKey, PREFSTRING_LARGEICONS,
		buffer, sizeof(buffer), PREF_OFF);

	if(!lstrcmpi(buffer, PREF_ON))
		SetLargeIconsPref();
	else
		ClearLargeIconsPref();

	AUtlGetKeyString(environKey, PREFSTRING_SIGNALVALUES,
		buffer, sizeof(buffer), PREF_OFF);

	if(!lstrcmp(buffer, PREF_ON))
		SetObjSignalValuesPref();
	else
		ClearObjSignalValuesPref();

	AUtlGetKeyString(environKey, PREFSTRING_BUFFEREDREDRAW,
		buffer, sizeof(buffer), PREF_ON);

	if(!lstrcmp(buffer, PREF_ON))
		SetBufferedRedrawPref();
	else
		ClearBufferedRedrawPref();

	AUtlGetKeyString(environKey, PREFSTRING_MAKEBACKUPS,
		buffer, sizeof(buffer), PREF_ON);

	if(!lstrcmp(buffer, PREF_ON))
		SetMakeBackUpsPref();
	else
		ClearMakeBackUpsPref();

	AUtlGetKeyString(environKey, PREFSTRING_VABHELPFILE,
	  VABHelpFile, sizeof(VABHelpFile), VAB_DEF_HELPFILE);
    
	AUtlGetKeyString(compilerKey, PREFSTRING_UNINITPARAMS,
		buffer, sizeof(buffer), PREF_ON);

	if(!lstrcmp(buffer, PREF_ON))
		SetUninitParamsWarningPref();
	else
		ClearUninitParamsWarningPref();
    
  if(environKey)
  	RegCloseKey(environKey);
    
  if(compilerKey)
  	RegCloseKey(compilerKey);
    
  if(vabKey)
  	AUtlCloseProductSubKey(vabKey);
  
	}
  
///////////////////////////////////////////////////////////////
BOOL CALLBACK PROGSYSTEM::PreferenceDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM ) // lParam)
	{
	HWND		hAutoSave			= GetDlgItem(hDlg, PREF_AUTOSAVE);
	HWND		hBufferRedraw	= GetDlgItem(hDlg, PREF_BUFFEREDREDRAW);
	HWND		hConfirmSave	= GetDlgItem(hDlg, PREF_CONFIRMSAVE);
	HWND		hFuncNames		= GetDlgItem(hDlg, PREF_SJTFUNCNAMES);
	HWND		hLargeIcons		= GetDlgItem(hDlg, PREF_SJTLARGEICONS);
	HWND		hPrjGridOn		= GetDlgItem(hDlg, PREF_PRJGRIDON);
	HWND		hPrjHGrid			= GetDlgItem(hDlg, PREF_PRJHORIZONGRID);
	HWND		hPrjVGrid			= GetDlgItem(hDlg, PREF_PRJVERTGRID);
	HWND		hShowNav			= GetDlgItem(hDlg, PREF_SHOWNAVIGATOR);
	HWND		hSigNames			= GetDlgItem(hDlg, PREF_SJTSIGNALNAMES);
	HWND		hSjtGridOn		= GetDlgItem(hDlg, PREF_SJTGRIDON);
	HWND		hSjtHGrid			= GetDlgItem(hDlg, PREF_SJTHORIZONGRID);
	HWND		hSjtVGrid			= GetDlgItem(hDlg, PREF_SJTVERTGRID);
	char buffer[32];

	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			//Initialize the save and autosave check boxes
			if(PROGSYSTEM::TestAutoSavePref())
				{
				Button_SetCheck(hAutoSave, 1);
				if(PROGSYSTEM::TestConfirmSavePref())
					Button_SetCheck(hConfirmSave, 1);
				else
					Button_SetCheck(hConfirmSave, 0);
				}
			else
				{
				Button_SetCheck(hAutoSave, 0);
				if(PROGSYSTEM::TestConfirmSavePref())
					Button_SetCheck(hConfirmSave, 1);
				else
					Button_SetCheck(hConfirmSave, 0);

				EnableWindow(hConfirmSave, FALSE);
				}

			//Initialize show navigator check box
			if(PROGSYSTEM::TestShowNavigatorPref())
				Button_SetCheck(hShowNav, 1);
			else
				Button_SetCheck(hShowNav, 0);

			//Initialize Subject Grid
			wsprintf(buffer, "%d", sjtHGrid);
			Edit_SetText(hSjtHGrid, buffer);
			wsprintf(buffer, "%d", sjtVGrid);
			Edit_SetText(hSjtVGrid, buffer);

			if(PROGSYSTEM::TestSjtGridPref())
				Button_SetCheck(hSjtGridOn, 1);
			else
				{
				Button_SetCheck(hSjtGridOn, 0);
				EnableWindow(hSjtHGrid, FALSE);
				EnableWindow(hSjtVGrid, FALSE);
				}

			//Initialize func names checkbox
			if(PROGSYSTEM::TestFuncNamesPref())
				Button_SetCheck(hFuncNames, 1);
			else
				Button_SetCheck(hFuncNames, 0);

			//Initialize signal names checkbox
			if(PROGSYSTEM::TestSignalNamesPref())
				Button_SetCheck(hSigNames, 1);
			else
				Button_SetCheck(hSigNames, 0);

			//Initialize Large Icons checkbox
			if(PROGSYSTEM::TestLargeIconsPref())
				Button_SetCheck(hLargeIcons, 1);
			else
				Button_SetCheck(hLargeIcons, 0);

			//Initialize Buffered Redraw checkbox
			Button_SetCheck(hBufferRedraw, (PROGSYSTEM::TestBufferedRedrawPref())?1:0);

			//Initialize Project Grid
			wsprintf(buffer, "%d", prjHGrid);
			Edit_SetText(hPrjHGrid, buffer);
			wsprintf(buffer, "%d", prjVGrid);
			Edit_SetText(hPrjVGrid, buffer);

			if(PROGSYSTEM::TestPrjGridPref())
				Button_SetCheck(hPrjGridOn, 1);
			else
				{
				Button_SetCheck(hPrjGridOn, 0);
				EnableWindow(hPrjHGrid, FALSE);
				EnableWindow(hPrjVGrid, FALSE);
				}

			//only need to fill in the old proc address once
			oldPrefEditProcAddr = (WNDPROC)SetWindowLong(
														hSjtVGrid, GWL_WNDPROC,
														(long)PROGSYSTEM::PrefGridEditProc);

			SetWindowLong(hSjtHGrid, GWL_WNDPROC,
														(long)PROGSYSTEM::PrefGridEditProc);

			SetWindowLong(hPrjVGrid, GWL_WNDPROC,
														(long)PROGSYSTEM::PrefGridEditProc);

			SetWindowLong(hPrjHGrid, GWL_WNDPROC,
														(long)PROGSYSTEM::PrefGridEditProc);

			break;

		case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			switch(wID)
				{
				case IDOK:
					//Set all appropriate flags and values
					if(Button_GetCheck(hAutoSave))
						SetAutoSavePref();
					else
						ClearAutoSavePref();

					if(Button_GetCheck(hConfirmSave))
						SetConfirmSavePref();
					else
						ClearConfirmSavePref();

					if(Button_GetCheck(hShowNav))
						SetShowNavigatorPref();
					else
						ClearShowNavigatorPref();

					if(Button_GetCheck(hSigNames))
						SetSignalNamesPref();
					else
						ClearSignalNamesPref();

					if(Button_GetCheck(hFuncNames))
						SetFuncNamesPref();
					else
						ClearFuncNamesPref();

					if(Button_GetCheck(hLargeIcons))
						SetLargeIconsPref();
					else
						ClearLargeIconsPref();

					if(Button_GetCheck(hSjtGridOn))
						SetSjtGridPref();
					else
						ClearSjtGridPref();

					if(Button_GetCheck(hPrjGridOn))
						SetPrjGridPref();
					else
						ClearPrjGridPref();

					if(Button_GetCheck(hBufferRedraw))
						SetBufferedRedrawPref();
					else
						ClearBufferedRedrawPref();

					if(Edit_GetText(hSjtHGrid, buffer, sizeof(buffer)))
						sjtHGrid = atoi(buffer);

					if(Edit_GetText(hSjtVGrid, buffer, sizeof(buffer)))
						sjtVGrid = atoi(buffer);

					if(Edit_GetText(hPrjHGrid, buffer, sizeof(buffer)))
						{
						prjHGrid = atoi(buffer);
						if(prjHGrid > PRJSYS_MAXGRIDSIZE)
							prjHGrid = PRJSYS_MAXGRIDSIZE;

						}

					if(Edit_GetText(hPrjVGrid, buffer, sizeof(buffer)))
						{
						prjVGrid = atoi(buffer);
						if(prjVGrid > PRJSYS_MAXGRIDSIZE)
							prjVGrid = PRJSYS_MAXGRIDSIZE;
						}

					EndDialog(hDlg, TRUE);
					break;

				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case PREF_AUTOSAVE:
					if(wNotify == BN_CLICKED)
						{
						if(Button_GetCheck(hAutoSave))
							EnableWindow(hConfirmSave, TRUE);
						else
							EnableWindow(hConfirmSave, FALSE);
						}
					break;

				case PREF_SJTGRIDON:
					if(wNotify == BN_CLICKED)
						{
						if(Button_GetCheck(hSjtGridOn))
							{
							EnableWindow(hSjtHGrid, TRUE);
							EnableWindow(hSjtVGrid, TRUE);
							}
						else
							{
							EnableWindow(hSjtHGrid, FALSE);
							EnableWindow(hSjtVGrid, FALSE);
							}
						}
					break;

				case PREF_PRJGRIDON:
					if(wNotify == BN_CLICKED)
						{
						if(Button_GetCheck(hPrjGridOn))
							{
							EnableWindow(hPrjHGrid, TRUE);
							EnableWindow(hPrjVGrid, TRUE);
							}
						else
							{
							EnableWindow(hPrjHGrid, FALSE);
							EnableWindow(hPrjVGrid, FALSE);
							}
						}
					break;

				}
			break;
			}

		case WM_CLOSE:
			//replace the subclassed window procs
			SetWindowLong(hSjtVGrid,	GWL_WNDPROC,
				(long)oldPrefEditProcAddr);

			SetWindowLong(hSjtHGrid, GWL_WNDPROC,
				(long)oldPrefEditProcAddr);

			SetWindowLong(hPrjVGrid, GWL_WNDPROC,
				(long)oldPrefEditProcAddr);

			SetWindowLong(hPrjHGrid, GWL_WNDPROC,
				(long)oldPrefEditProcAddr);

			break;
		}
	return 0;
	}

LRESULT CALLBACK PROGSYSTEM::PrefGridEditProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	switch(message)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			//if its a number then break;
			if(wParam >= 0x30 && wParam <= 0x39)
				break;

			//otherwise, see if it's some other acceptable key.
			//If its not, then just return.
			switch(wParam)
				{
				case VK_HOME:
				case VK_END:
				case VK_INSERT:
				case VK_DELETE:
				case VK_RIGHT:
				case VK_LEFT:
					//check to see if its an enhanced key and if so, break
					//It it's not enhanced, then its an unwanted key

					//bit 24 of lParam will be set if its an enhanced key
					if(!((1L<<24) & lParam))
						return 0;
					//else fall through

				case VK_MENU: //alt key
				case VK_ESCAPE:
				case VK_TAB:
				case VK_RETURN:
				case VK_BACK:
				case VK_SHIFT:
				case VK_CONTROL:
					break;

				default:
					return 0;
				}
		}
	return CallWindowProc((FARPROC)oldPrefEditProcAddr, hwnd, message, wParam, lParam);
	}

//creates a objects based on the typeID.
//returns 0 if no object could be created.
AOBJECTID APIENTRY PROGSYSTEM::CreateDynamicObject(
	ATYPEID typeID,
	APROTOCOLID protocolID)
	{
	AOBJECTID objID = 0;
	LPPROJECT projectPtr = GetCurrentProject();

	if(projectPtr)
		{
		objID = projectPtr->CreateDynamicObject(typeID, protocolID);
    }
  return objID;
  }


//called from within oip to destroy an object
BOOL APIENTRY PROGSYSTEM::DestroyObject(
	AOBJECTID objID)
	{
	LPPROJECT projectPtr = GetCurrentProject();
	BOOL result = FALSE;

	if(projectPtr)
		{
		result = projectPtr->DestroyObject(objID);
		}

  return result;
  }


//called from within oip to change an object's name
BOOL APIENTRY PROGSYSTEM::RenameObject(
	AOBJECTID objID,
	LPSTR newName)
	{
	BOOL result = FALSE;
	LPPROJECT projectPtr = GetCurrentProject();

	if(projectPtr)
		result = projectPtr->RenameObject(objID, newName);

	return result;
  }

BOOL PROGSYSTEM::BuildMasterTypeList()
	{
	LPOBJDESC objDesc;

	pTheSystem->SetMasterTypeList(new MASTERTYPELIST());

	I_ASSERT(pTheSystem->GetMasterTypeList());
	
	objDesc=0;
	while(0 != (objDesc = cfgGetNextObjDesc(objDesc)))
		{
		pTheSystem->GetMasterTypeList()->AddTypeNode(objDesc->GetUID(),
			objDesc->pGetName(), 
			objDesc->pGetDLL(ADMIN_ROUTINE_INDEX),
			objDesc->pGetFunction(ADMIN_ROUTINE_INDEX));
		}

	pTheSystem->GetMasterTypeList()->PrepareNodes();
	
	return TRUE; //signal no error
	}

void PROGSYSTEM::DestroyMasterTypeList()
	{
	delete pTheSystem->GetMasterTypeList();
	}

////////////////////////////////
LPCSTR PROGSYSTEM::GetSplashResourceName()
	{
	HDC hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	int resourceID = IDBMP_8BIT_SPLASH; //default to 256
	int planes;
	int bits;
	int numColors;

	I_ASSERT(hdc);

	bits = GetDeviceCaps(hdc, BITSPIXEL);
	planes = GetDeviceCaps(hdc, PLANES);

	//I'm assuming that a 256 or greater color machine will at least
	//have 20 pure colors while a 4 bit machine will have at most 16
	//pure colors.

	numColors = GetDeviceCaps(hdc, NUMCOLORS);

	//the bits * planes may be greater than 8 bit but
	//the color resolution could be set less than 8 bit color.
	//i.e. ATI driver set to 16 colors
	if(((bits * planes) < 8) || (numColors <= 16))
		resourceID = IDBMP_4BIT_SPLASH;

	DeleteDC(hdc);

	return MAKEINTRESOURCE(resourceID);
	}

////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
char* PROGSYSTEM::GetBuildNum(char* buffer, int bufLength)
	{
  buffer[0] = '\0';
  LoadString(PROGSYSTEM::hInst, IDS_BUILDVER, buffer, bufLength-1);
  return buffer;
  }
  
////////////////////////////////////////////////////////////
char* PROGSYSTEM::GetBuildDate(char* buffer, int bufLength)
	{
  buffer[0] = '\0';
  LoadString(PROGSYSTEM::hInst, IDS_BUILDDATE, buffer, bufLength-1);
	
  return buffer;
	}  


static char DEBUG_NAME_STRING[] = {"[Dbg]"};
static char DEBUG_BUILD_NUM_STRING[] = {"  Build: "};
static char DEBUG_BUILD_DATE_STRING[] = {"  Date: "};
static char DEBUG_SEPARATOR[] = {" | "};


/////////////////////////////////////////////////////////////
char* PROGSYSTEM::GetFrameWndName(char* buffer, int bufLength)
	{
  
  buffer[0] = '\0';
  
	#ifdef ADBG_DEBUG
  GetBuildInfoFrameWndName(buffer, bufLength);
  #else
	LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, (LPSTR)buffer, bufLength-1);
	#endif	
  
  return buffer;
  }

/////////////////////////////////////////////////////////////
char* PROGSYSTEM::GetBuildInfoFrameWndName(char* buffer, int bufLength)
	{
	char buf1[16];
  char buf2[16];
	char kernel[64];
  
  buffer[0] = '\0';
  
	LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, (LPSTR)buffer, bufLength-1);
		
	#ifdef ADBG_DEBUG
  //construct the build name
  if(bufLength <= sizeof(DEBUG_NAME_STRING) + lstrlen(buffer))
  	{
    I_ASSERT(FALSE);
    }
  else
  	{
		lstrcat(buffer, DEBUG_NAME_STRING);
  	}
  #endif  
    
  ADBGGetKernelName(kernel, sizeof(kernel));
	
	if(lstrlen(buffer) + lstrlen(DEBUG_SEPARATOR) + lstrlen(kernel) < bufLength)
  	{
 		lstrcat(buffer, DEBUG_SEPARATOR);
    lstrcat(buffer, kernel);
 		
 		GetBuildNum(buf1, sizeof(buf1));
		ADBGGetKernelBuildNum(buf2, sizeof(buf2));

  	if(bufLength > lstrlen(buffer) + lstrlen(buf1) + 
  		lstrlen(DEBUG_BUILD_NUM_STRING) + lstrlen(buf2) + 
  		lstrlen(DEBUG_SEPARATOR))
  		{
    	lstrcat(buffer, DEBUG_BUILD_NUM_STRING);
    	lstrcat(buffer, buf1);
      lstrcat(buffer, DEBUG_SEPARATOR);
      lstrcat(buffer, buf2);
   		
   		//now add the dates
   		GetBuildDate(buf1, sizeof(buf1));
  		ADBGGetKernelBuildDate(buf2, sizeof(buf2));
 
    	if(bufLength > lstrlen(buffer) + lstrlen(buf1) + 
    		lstrlen(DEBUG_BUILD_DATE_STRING) + lstrlen(buf2) + 
    		lstrlen(DEBUG_SEPARATOR))
    		{
      	lstrcat(buffer, DEBUG_BUILD_DATE_STRING);
      	lstrcat(buffer, buf1);
        lstrcat(buffer, DEBUG_SEPARATOR);
        lstrcat(buffer, buf2);	      
      	}
      else
      	{
        I_ASSERT(FALSE);
        }
     	}
    else
    	{
      I_ASSERT(FALSE);
      }
    }
  else
  	{
    I_ASSERT(FALSE);
    }

	return buffer;
  }


static char BUILD_TITLE_POSTFIX[] = {" Build Information"};
//////////////////////////////////////////////////////////////////////////
void PROGSYSTEM::ShowBuildInfo()
	{
  char buffer[128];
  char title[128];
  
  
  LoadString(hInst, STR_FRAMENAME, title, sizeof(title)-1);
  
  if(sizeof(title) > lstrlen(title) + lstrlen(BUILD_TITLE_POSTFIX))
  	lstrcat(title, BUILD_TITLE_POSTFIX);
    
  GetBuildInfoFrameWndName(buffer, sizeof(buffer));
  
  MessageBox(GetActiveWindow(), buffer, title, MB_OK|MB_TASKMODAL); 
    
  }



