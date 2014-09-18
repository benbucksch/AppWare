
#define I_MAIN

#include "n_includ.h"

extern int _WinAllocFlag;

void RegisterEditSuperClass();

int far pascal _export HaveWeBeenLoadedYet()
	{
	static BOOL							beenHere = FALSE;
	BOOL										rVal;

	rVal = beenHere;
	beenHere = TRUE;
	return rVal;
	}	

extern "C" int far pascal LibMain (
	HINSTANCE								hIns,
	WORD 										wDataSeg,
	WORD 										cbHeapSize,
	LPSTR										lpCmdLine)
	{
	char										profileStr[2];

	I_UNUSED(wDataSeg);I_UNUSED(cbHeapSize);I_UNUSED(lpCmdLine);

	if (HaveWeBeenLoadedYet() )
		return 0;

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;
	
	RegisterEditSuperClass();

	*g_szNumberParts = 0;
	LoadString(g_hInstance, I_NUMBER_PARTS, g_szNumberParts, 100);


	GetProfileString("intl", "sCurrency", "$", profileStr, sizeof(profileStr) );
	g_chCurrency = *profileStr;

	GetProfileString("intl", "sThousand", ",", profileStr, sizeof(profileStr) );
	g_chThouSep = *profileStr;

	GetProfileString("intl", "sDecimal", ".", profileStr, sizeof(profileStr) );
	g_chDecSep = *profileStr;

	ValidateNumberFormatString(g_szIntegerNumberFormat, &g_integerNumberFormatInfo, TRUE);
	ValidateNumberFormatString(g_szRealNumberFormat, &g_realNumberFormatInfo, FALSE);

	return TRUE;
	}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

extern "C" int far pascal _export WEP(
	int											nParameter)
	{
	I_UNUSED(nParameter);

	UnregisterClass(g_numberObjectClassName, g_hInstance);
	return 1;
  }


static void RegisterEditSuperClass()
	{
	WNDCLASS								wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareNumberObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_numberObjectClassName;

  RegisterClass(&wc);
	}

