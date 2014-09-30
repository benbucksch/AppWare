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
	UINT		index;
	COLORREF	rgb;
	};

class ICONEDITOR;

class COLORPALETTE {

	private:

	static WINDOWLIST *	windowList;
	static HANDLE  					hAppInstance;
	HWND	   			 					hWndParent;
	HWND					 					hWndThis;
	ICONEDITOR *					iconEditor;
	PIXELWINDOW *				pixelWindow;
	int								horizCount;


	COLORINFO								colors[PALBOXHORZCNT][PALBOXVERTCNT];

	BOOL registerWindowClass(void);

	void DrawScreenColorBox(
		HDC			hDC,
		int         left,
		int  		top,
		BOOL		invertedFlag);

	public:

	COLORPALETTE(
		HWND	  					hWndMain,
		HANDLE 						hAppInstance,
		HANDLE 						hPrevInstance,
		ICONEDITOR *		iconEditor,
		PIXELWINDOW *	pixelWindow,
		int					colorCount);

	~COLORPALETTE(void);

	long WindowProc(
		HWND 			hWnd,
		unsigned 	message,
		WPARAM 			wParam,
		LPARAM 			lParam);

	void Show(void);

	void Draw(void);

	void COLORPALETTE::InitColorInfo(
		int		colorCount);

	void UpdateIndices(
		void);

	friend long CALLBACK ColorPaletteWndProc(
		HWND   hWnd,
		UINT   message,
		WPARAM wParam,
		LPARAM lParam);

	};

#undef ICONEDITOR

#endif
