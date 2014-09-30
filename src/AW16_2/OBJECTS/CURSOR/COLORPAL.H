#if !defined( __COLORPALETTE_H )
#define __COLORPALETTE_H

#include "winlist.h"
#include "pixelwin.h"
#include "position.h"


#define MAXCOLORS	16

#define SCREENCOLOR		-1
#define INVERTEDCOLOR -2

struct COLORINFO
	{
	short int	index;
	COLORREF	rgb;
	};

class ICONEDITOR;

class COLORPALETTE:public MEMORY{

	private:

	static WINDOWLIST far*	windowList;
	static HANDLE  					hAppInstance;
	HWND	   			 					hWndParent;
	HWND					 					hWndThis;
	ICONEDITOR far*					iconEditor;
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
		HANDLE 						hAppInstance,
		HANDLE 						hPrevInstance,
		ICONEDITOR far*		iconEditor,
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

	friend long FAR PASCAL _export ColorPaletteWndProc(
		HWND   hWnd,
		UINT   message,
		WPARAM wParam,
		LPARAM lParam);

	};

#undef ICONEDITOR

#endif
