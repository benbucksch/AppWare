#if !defined( __ICONEDIT_H )
#define __ICONEDIT_H

#include "winlist.h"
#include "sericon.h"

#define FILE_NEW		202
#define FILE_OPEN		203
#define FILE_SAVE		204
#define FILE_SAVEAS	205
#define FILE_EXIT		206

#define COLOR16				1
#define BLACKANDWHITE	2
#define BUTTONSPACE     4

#define OKBUTTONTOP      PIXELWNDTOP // + BUTTON2HEIGHT + BUTTONSPACE * 6//PALTOP
#define CANCELBUTTONTOP  OKBUTTONTOP + yCtrlExt + BUTTONSPACE
#define OKBUTTONLEFT     CLEARBUTTONLEFT //
#define CANCELBUTTONLEFT OKBUTTONLEFT
#define HELPBUTTONTOP    CANCELBUTTONTOP + yCtrlExt + BUTTONSPACE

#define CLEARBUTTONLEFT  LEFTBUTTONLEFT + BUTTON2WIDTH + 10// NEWBUTTONLEFT
#define CLEARBUTTONTOP   HELPBUTTONTOP + yCtrlExt + BUTTONSPACE * 10 //(PIXELWNDTOP+PIXELWNDHEIGHT/2-(2*ICONBORDERWIDTH+PIXELCOUNT)/2) - 40 + 10
#define DELBUTTONLEFT   CLEARBUTTONLEFT
#define DELBUTTONTOP    CLEARBUTTONTOP + yCtrlExt + BUTTONSPACE
#define NEWBUTTONTOP    DELBUTTONTOP + yCtrlExt + BUTTONSPACE // BUTTONHEIGHT + LEFTBUTTONTOP  + BUTTONSPACE * 3
#define NEWBUTTONLEFT   OKBUTTONLEFT

#define LOADBUTTONTOP   NEWBUTTONTOP + yCtrlExt + BUTTONSPACE
#define LOADBUTTONLEFT  NEWBUTTONLEFT
#define TESTBUTTONTOP   LOADBUTTONTOP + yCtrlExt + BUTTONSPACE


#define BLANKBUTTONLEFT CANCELBUTTONLEFT
#define BLANKBUTTONTOP  0



/*
#define DELAYID	        450
#define FRAMEID	        451
#define ICONEDIT        452
#define COLOREDIT       453
#define BWEDIT          454
#define DELICONID       455
#define LOADICONID      456
#define NEWID           457
#define CLEARID         458
*/





class PIXELWINDOW;
class COLORPALETTE;

class ICONEDITOR:public MEMORY{

private:

	static WINDOWLIST far *	windowList;
	static HANDLE  					hAppInstance;
	static short						colorCount;
	HWND	   			 					hWndParent;
	COLORPALETTE far*				colorPalette;
	short int								currentColor;
	HMENU										hMenu;
	BOOL										haveFileInfo;

	BOOL static CheckIconFile(
		int	fileHandle);

	BOOL registerWindowClass(void);

	void Draw(void);

	void DrawCurrentColor(
		HDC	hDC);

	BOOL GetIconBitmap(
		int	fileHandle);

	void ICONEDITOR::DrawLabel(
		HDC			hDC,
		LPSTR		text,
		short		left,
		short		top,
		short		width);

	BOOL VerifyClose(
		void);

public:

	LPSERIUSICON						theIcon;
	HWND					 					hWndIconEditor;
	PIXELWINDOW far*				pixelWindow;
	HWND	    							hLeftButton;
	HWND										hRightButton;
	HWND                    hOKButton;
	HWND                    hCancelButton;
	HWND                    hNewButton;
	HWND                    hDelButton;
	HWND                    hLoadButton;
	HWND                    hBlankButton;
	HWND                    hClearButton;
	HWND                    hTestButton;
	HWND                    hDelayTimeEdit;
	HWND                    hNFrames;
	ICONEDITOR(
		HWND	  			hWndMain,
		HANDLE 				hAppInstance,
		HANDLE 				hPrevInstance,
		LPSERIUSICON	iconInfo,
		short int			iconType);

	~ICONEDITOR(void);

	long WindowProc(
		HWND 			hWnd,
		unsigned 	message,
		WORD 			wParam,
		LONG 			lParam);

	void Show(void);

	void SetCurrentColor(
		short	colorIndex);

	friend long FAR PASCAL _export IconEditorWndProc(
		HWND   hWnd,
		UINT   message,
		WPARAM wParam,
		LPARAM lParam);

	void OpenIconFile(
		int hFile);

	};


#endif
