#define STRICT

struct RESIZER
	{
	int											iParentWidth;
	int											iParentHeight;
	int											iMinParentWidth;
	int											iMinParentHeight;
	WNDPROC									pfnOldDialogProc;
	};
typedef RESIZER far * LPRESIZER;
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
extern "C" int far pascal LibMain (HINSTANCE hIns, WORD, WORD, LPSTR);
extern "C" int far pascal _export WEP(int);
void ResizeThings(HWND hwndDlg, LPCSTR szResizeList, int iLeftChange, int iRightChange, int iTopChange, int	iBottomChange);
LPRESIZER FindResizerData(HWND hWndDialog);
extern "C" LRESULT far pascal _export SubclassedDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern "C" LRESULT far pascal _export ResizerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);




