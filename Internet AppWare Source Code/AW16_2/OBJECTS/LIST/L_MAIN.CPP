#define I_MAIN
#include "l_includ.h"

extern int _WinAllocFlag;
static void RegisterClasses();

extern "C" int far pascal LibMain (
	HINSTANCE 							hIns, 
	WORD,
	WORD 										wHeapSize, 
	LPSTR)
	{
	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = hIns;

	if ( wHeapSize != 0 )
		UnlockData(0);

	RegisterClasses();

	g_uDoubleClickTime = GetDoubleClickTime();
	g_ptDoubleClick.x = GetSystemMetrics(SM_CXDOUBLECLK);
	g_ptDoubleClick.y = GetSystemMetrics(SM_CYDOUBLECLK);

	return TRUE;   // Indicate that the DLL was initialized successfully.
	}

//////////////////

static void RegisterClasses()
	{
	WNDCLASS 							wndclass;

	GetClassInfo(NULL, "LISTBOX", &wndclass);
	g_iListboxThisPointerOffset = wndclass.cbWndExtra;
	wndclass.cbWndExtra += (sizeof(void *) );

	g_defListboxProc = wndclass.lpfnWndProc ;
	wndclass.lpfnWndProc = NewListboxProc;

	wndclass.style &= ~CS_GLOBALCLASS ;
	wndclass.hInstance = g_hInstance ;
	wndclass.lpszClassName = g_listboxObjectClassName;
	RegisterClass(&wndclass) ;

	////////////////

	GetClassInfo(NULL, "COMBOBOX", &wndclass) ;
	g_iComboboxThisPointerOffset = wndclass.cbWndExtra ;
	wndclass.cbWndExtra += sizeof(void *);

	g_defComboboxProc = wndclass.lpfnWndProc ;
	wndclass.lpfnWndProc = NewComboboxProc;

	wndclass.style &= ~CS_GLOBALCLASS ;
	wndclass.hInstance = g_hInstance ;
	wndclass.lpszClassName = g_comboboxObjectClassName;
	RegisterClass(&wndclass);
	}

