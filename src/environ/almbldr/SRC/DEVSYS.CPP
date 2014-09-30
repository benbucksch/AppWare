#include "almsys.h"
#include <windowsx.h>

#include "devsys.h"
#include "resconst.h"
#include "serdev.h"
#include "mdiapp.h"
#include "memlib.h"
#include "cfglib.h"

#define SPLASH_CLASS_NAME	(LPSTR)"ock_SplashClass"


SYSTEMMIN		theSystem;
SYSTEM*			pTheSystem		= &theSystem;

///////////////////////////////////////////////////////////////////////////
//	Allocate and initialize DEVSYSTEM class static data  members
///////////////////////////////////////////////////////////////////////////

LPMDIAPP			DEVSYSTEM::appWindow = NULL;
HWND					DEVSYSTEM::hWndSplash=NULL;
BOOL    			DEVSYSTEM::splashRegistered=FALSE;
HINSTANCE     DEVSYSTEM::hAppInstance = 0;
LPDIBITMAP 		DEVSYSTEM::diSplash = NULL;


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......: 	DEVSYSTEM	(static)
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
int DEVSYSTEM::Initialize(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		cmdLine,
	int			cmdShow)
	{
	int result;
	char buffer[32];

  hAppInstance = hInstance;

	ShowSplashScreen(hInstance);
	pTheSystem->Initialize();

	cfgReadConfigs();

	LoadString(hInstance, STR_APPNAME, buffer, sizeof(buffer)-1);

	appWindow = new MDIAPP(hInstance, hPrevInstance, cmdLine, cmdShow,
	 "MainMenu", WINDOWS_MENU_POSITION, "MainAccelTable",
    (LPSTR)buffer);

	result = appWindow->MainLoop();

	return result;

	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CleanUp
//
// CLASS.......: 	DEVSYSTEM	(static)
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
void DEVSYSTEM::Cleanup(
	void)
	{

	delete appWindow;

	pTheSystem->CleanUp();

	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetMdiApp
//
// CLASS.......: 	DEVSYSTEM	(static)
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
LPMDIAPP DEVSYSTEM::GetMdiApp(
	void)
	{

	return appWindow;

	}

void DEVSYSTEM::ShowSplashScreen(
	HINSTANCE hInst)
	{
	WNDCLASS wc;
	RECT clientRect;

	if(!splashRegistered)
		{
		splashRegistered = TRUE;

		wc.style 					= 0;
		wc.lpfnWndProc 		= DEVSYSTEM::SplashWndProc;
		wc.cbClsExtra 		= 0;
		wc.cbWndExtra 		= 0;
		wc.hInstance 			= hInst;
		wc.hIcon 					= NULL;
		wc.hCursor 				= LoadCursor(NULL, IDC_WAIT);
		wc.hbrBackground	= GetStockBrush(NULL_BRUSH);
		wc.lpszMenuName 	= NULL;
		wc.lpszClassName 	= SPLASH_CLASS_NAME;

		RegisterClass((LPWNDCLASS)&wc);
		}
	
	diSplash = new DIBITMAP((LPSTR)GetSplashResourceName(), hInst);

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
			hInst,
			NULL);
		UpdateWindow(hWndSplash);
		}

	}

void DEVSYSTEM::HideSplashScreen()
	{
	if(hWndSplash)
		DestroyWindow(hWndSplash);
	
	if(diSplash)
		{
		delete diSplash;
		diSplash=NULL;
		}
	}


LRESULT CALLBACK DEVSYSTEM::SplashWndProc(
	HWND   hwnd,
	UINT   message,
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


////////////////////////////////
LPCSTR DEVSYSTEM::GetSplashResourceName()
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
