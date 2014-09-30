#if !defined( __COLORPALETTE_H )
#define __COLORPALETTE_H

#include "classdef.h"
#include "memclass.h"
#include "position.h"

#define MAXCOLORS	16

#define SCREENCOLOR		-1
#define INVERTEDCOLOR -2

struct COLORINFO
	{
	short int	index;
	COLORREF	rgb;
	};

class COLORPALETTE:public MEMORY{

  friend long I_CALLBACK ColorPaletteWndProc(
	         HWND   hwnd,
	         UINT   message,
	         WPARAM wParam,
	         LPARAM lParam);

	private:

	static WINDOWLIST far*	windowList;
	static HINSTANCE				hAppInstance;
	HWND	   			 					hWndParent;
	HWND					 					hWndThis;
	PIXELWINDOW far*				pixelWindow;
	short int								horizCount;


	COLORINFO								colors[PALBOXHORZCNT][PALBOXVERTCNT];

	BOOL registerWindowClass(void);

	void DrawScreenColorBox(
		HDC			hDC,
		short		left,
		short		top,
		BOOL		invertedFlag);

	public:

	COLORPALETTE(
		HWND	  					hWndMain,
		HINSTANCE					hAppInstance,
		HINSTANCE					hPrevInstance,
		PIXELWINDOW far*	pixelWindow,
		short int					colorCount);

	~COLORPALETTE(void);

	long WindowProc(
		HWND 			hWnd,
		unsigned 	message,
		WORD 			wParam,
		LONG 			lParam);

	void Show(void);

	void Draw(void);

	void COLORPALETTE::InitColorInfo(
		short int		colorCount);

	void UpdateIndices(
		void);

  void UpdateColors(
	  void);

	friend long I_CALLBACK ColorPaletteWndProc(
		HWND 			hWnd,
		unsigned 	message,
		WORD 			wParam,
		LONG 			lParam);

	};

#endif
