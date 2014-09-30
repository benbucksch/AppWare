#include <windows.h>
#include <custcntl.h>

#define STRICT

struct RESIZER
	{
	int											iParentWidth;
	int											iParentHeight;
	int											iMinParentWidth;
	int											iMinParentHeight;
	WNDPROC									pfnOldDialogProc;
	};
typedef RESIZER  * LPRESIZER;
typedef unsigned short USHORT;

///Global data
#ifdef _MAIN_MODULE_
HINSTANCE g_hInstance;
LPSTR g_szClassName = "AWResizer";
LPSTR g_szCopyright = "Copyright Novell 1995";
#else
extern HINSTANCE g_hInstance;
extern LPSTR g_szClassName;
extern LPSTR g_szCopyright;
#endif



//Function Declarations
void ResizeThings(HWND hwndDlg, LPCSTR szResizeList, int iLeftChange, int iRightChange, int iTopChange, int	iBottomChange);
LPRESIZER FindResizerData(HWND hWndDialog);

extern "C" UINT CALLBACK CustomControlInfoA(LPCCINFOA acci);
extern "C" BOOL CALLBACK ResizerCtlStyleDlg (HWND , LPCCSTYLE );
extern "C" int CALLBACK ResizerSizeToText (DWORD , DWORD , HFONT , LPSTR );

LRESULT CALLBACK SubclassedDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ResizerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	

