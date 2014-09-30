#define I_MAIN
#include "l_includ.h"

static void RegisterClasses();

///////////////////////////////////////////////////////////////////////////
//DllEntryPoint
///////////////////////////////////////////////////////////////////////////
extern "C" BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			{
			g_hInstance = hInstDLL;

			RegisterClasses();

			g_uDoubleClickTime = GetDoubleClickTime();
			g_ptDoubleClick.x = GetSystemMetrics(SM_CXDOUBLECLK);
			g_ptDoubleClick.y = GetSystemMetrics(SM_CYDOUBLECLK);
			}
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}

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

