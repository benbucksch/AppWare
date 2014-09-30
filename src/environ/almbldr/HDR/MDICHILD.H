#ifndef __MDICHILD_H_
#define __MDICHILD_H_

#include "memclass.h"
#include "tstring.h"
#include "classdef.h"
#include "seruid.h"

///////////////////////////////////////////////////////////////////////////

class MDICHILD:public MEMORY
	{
private:
	static MDICHILD * 	windowList;
	static BOOL						isRegistered;
	static HICON					hWindowIcon;
	static WNDPROC				origListboxWndProc;
	static WNDPROC				origButtonWndProc;

	MDICHILD *					next;

	HINSTANCE hAppInstance;
	HINSTANCE hPrevInstance;
	HWND	 hWndFrame;
	HWND	 hWndClient;
	HWND 	 hWindowHandle;

	// handles to child controls

	HWND	 		hListbox;
	HWND	 		hObjects;
	HWND	 		hFunctions;
	HWND   		hEvents;
	HWND	 		hBNGroup;
  HWND      hLBGroup;
  HWND      hCatLabel;
  HWND      hCatField;

  HWND      hInsert;
  HWND      hDelete;
  HWND      hEdit;

	HWND	 		hCurrentButton;

	LPFILEOBJECT	fileObject;

	short			objectNameSeed;
	short			functionNameSeed;
	short			eventNameSeed;

	BOOL Register(
		void);

	void SetCurrentButton(
		HWND	hButton);

	void AddCompToListbox(
		SERUID compUID);

	void OpenDescDialog(
		void);

	void DeleteComponent(
		void);

public:

	MDICHILD(
		HINSTANCE			hInstance,
		HINSTANCE			hPrevInstance,
		HWND					FrameWindow,
		HWND					ClientWindow,
		LPFILEOBJECT	fileObject);

	~MDICHILD();

	static LRESULT CALLBACK MDICHILDWndProc(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	static LRESULT CALLBACK ControlWndProc(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	virtual LRESULT WindowProcedure(
	  HWND   hwnd,
	  UINT   message,
	  WPARAM wParam,
	  LPARAM lParam);

	BOOL Show(
		int ShowCmd);

	BOOL Hide(
		void);

	void Update(
		void);

	static MDICHILD * GetWindowObject(
		HWND	hWindowHandle);

	void InitializeMenu(
		HMENU		hmenu);

	void AddComponent(
		void);

	static BOOL SaveModifiedConfigs(
		void);

  TSTRING GetCategoryName(
    void);

	};

#endif


