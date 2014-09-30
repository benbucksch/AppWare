#ifndef __SPLSHMGR_H
#define __SPLSHMGR_H

#include "memory.h"
#include "dibitmap.h"

class SPLASHMGR:public MEMORY
	{
	private:
		DIBITMAP* splashPtr;
		long status;
		WORD delayTime;
		static SPLASHMGR* thisPtr;
		static WNDPROC oldEditTimeProc;

		static BOOL CALLBACK ImportSplashDlgProc(
 			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);
			
		BOOL LoadBitmapFile();		
		
	static BOOL CheckDIBFile(
		HANDLE fileHandle,
		LPCSTR fileName);

	static LRESULT CALLBACK SPLASHMGR::EditTimeProc(
		HWND     	hWnd,
		UINT 			message,
		WPARAM    wParam,
		LPARAM    lParam);

	public:
		SPLASHMGR();
		
		~SPLASHMGR();
		
		BOOL ImportSplashScreen(DIBITMAP* newSplashPtr, WORD delay);

		DIBITMAP* GetSplashScreen()
			{
			return (splashPtr->Valid())?splashPtr:NULL;
			}
		
		void SetButtons(HWND hDlg);
		
		WORD GetDelayTime()
			{
			return delayTime;
			} 


	};

#endif
