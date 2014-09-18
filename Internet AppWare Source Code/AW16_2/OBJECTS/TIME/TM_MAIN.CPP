#define TM_MAIN

#include "tm_incld.h"

extern int _WinAllocFlag;

void RegisterEditSuperClass();

extern "C" int far pascal LibMain (HINSTANCE hIns, WORD	wDataSeg,	WORD cbHeapSize, LPSTR lpCmdLine)
{

	I_UNUSED(wDataSeg);
	I_UNUSED(cbHeapSize);
	I_UNUSED(lpCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;
	
	RegisterEditSuperClass();

	GetProfileString("intl", "s2359", "PM", g_pmProfileSting, sizeof(g_pmProfileSting) );
	GetProfileString("intl", "s1159", "AM", g_amProfileSting, sizeof(g_amProfileSting) );

	GetProfileString("intl", "sTime", ":", g_timeSepProfileChar, sizeof(g_timeSepProfileChar) );

	g_leadingZeroProfileBool = (BOOL)GetProfileInt("intl", "iTLZero", TRUE);
	g_is24hourProfileBool = (BOOL)GetProfileInt("intl", "iTime", 0);

	return TRUE;
}


extern "C" int far pascal _export WEP(int	nParameter)
{
	I_UNUSED(nParameter);

	UnregisterClass(g_timeObjectClassName, g_hInstance);
	return 1;
}


static void RegisterEditSuperClass()
{
	WNDCLASS	wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareTimeObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_timeObjectClassName;

  RegisterClass(&wc);
}

