#ifndef __SPLASHDIS_H
#define __SPLASHDIS_H

#include "memclass.h"
#include "dibitmap.h"
#include "winlist.h"

#define SPLASH_FILE_LENGTH 13

class SPLASHDISPLAY:public MEMORY
	{
	private:
		static WINDOWLIST* winList;
		static WORD count;
		DIBITMAP* splashPtr;
		HWND hWndSplash;
		WORD timeDelay;
		DWORD startTime;
		
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

	LRESULT SplashWndProc(
		HWND hwnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

	public:
		SPLASHDISPLAY();

		~SPLASHDISPLAY();

		void ShowSplashScreen();

		void HideSplashScreen();

		BOOL Read(HANDLE hFile, int footerOffset=0);

		BOOL Write(HANDLE hFile);

		DIBITMAP* GetSplashPtr()
			{
			return splashPtr;
			}

		void SetSplashPtr();

		BOOL ValidSplash()
			{
			return (splashPtr && splashPtr->Valid())?TRUE:FALSE;
			}

		WORD GetTimeDelay()
			{
			return timeDelay;
			}

		void SetTimeDelay(WORD delay)
			{
			timeDelay = delay;
			}

	};

#endif
