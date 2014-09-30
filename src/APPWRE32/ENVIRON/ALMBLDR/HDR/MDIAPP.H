#ifndef __MDIAPP_H
#define __MDIAPP_H

#include "memclass.h"
#include "classdef.h"
#include "seruid.h"

/////////////////////////////////////////////////////////////////////////////

class MDIAPP:public MEMORY
	{
private:
	static MDIAPP	* windowList;
	static BOOL				 isRegistered;
	static HBITMAP		 titleScreen;

	MDIAPP *		next;
	HWND					windowHandle;
	HWND 					hWndClient;
	HWND 					hWndFrame;
	HINSTANCE 		hInst;
	HACCEL				hAccel;
	HMENU					hMainMenu;
	HMENU					hWindowsSubmenu;
	HINSTANCE			hPrevInst;

	short					newNameCount;
	LPMDICHILD		activeChild;

	BOOL Hide();

	void Update();

	BOOL Register();

	BOOL Initialize(
		int nCmdShow);

	void EditConfigFile(
		void);

	void CreateNewConfig(
		void);

	void Save(
		void);

public:

	static MDIAPP * GetObject(
		HWND	windowHandle);

	static LRESULT CALLBACK FrameWndProc(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	virtual LRESULT CALLBACK WindowProcedure(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	MDIAPP(
		HINSTANCE	hInstance,
		HINSTANCE	hPrevInstance,
		LPSTR   lpCmdParam,
		int			CmdShow,
		LPSTR		defMenu,
		short		windowMenuPosition,
		LPSTR		defAccelTable,
		LPSTR		frameTitle);

	~MDIAPP();

	int APIENTRY MainLoop();

	static BOOL CALLBACK About(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	BOOL Show(int ShowCmd);

	void SetActiveChild(
		LPMDICHILD child);

	BOOL SaveAs(
		void);

	void OpenConfigFile(
		void);

	HWND	GetClientWindow(
		void)
		{
		return hWndClient;
		}

  HINSTANCE GetAppInstance(void)
    {
    return hInst;
    }

	BOOL WriteToTextFile(
		void);
	};

#endif
