#ifndef __DEVSYS_H__
#define __DEVSYS_H__

#include "system.h"
#include "dibitmap.h"

#define MAXSTACKTOP	9

#define ABOUT_ADJUST 40 //amount to vertically shrink screen since
												//there is no button.

class DEVSYSTEM:public SYSTEM
	{
private:
	static 	LPMDIAPP  appWindow;
	static 	HWND		  hWndSplash;
	static LPDIBITMAP diSplash;
	static	BOOL      splashRegistered;
  static  HINSTANCE hAppInstance;

public:

	static int Initialize(
		HINSTANCE	hInstance,
		HINSTANCE	hPrevInstance,
		LPSTR		lpCmdLine,
		int			CmdShow);

	static LRESULT CALLBACK SplashWndProc(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	static void ShowSplashScreen(
		HINSTANCE hInst);

	static void HideSplashScreen();


	static void Cleanup(
	 void);

	static LPMDIAPP GetMdiApp(
		void);

  static HINSTANCE GetAppInstance(void)
    {
    return hAppInstance;
    }

	static LPCSTR GetSplashResourceName();

	};

#endif
