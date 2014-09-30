#if !defined( __ICONEDIT_H )
#define __ICONEDIT_H

#include "classdef.h"

#define FILE_NEW		202
#define FILE_OPEN		203
#define FILE_SAVE		204
#define FILE_SAVEAS	205
#define FILE_EXIT		206

#define COLOR16				1
#define BLACKANDWHITE	2

class ICONEDITOR{

private:

	static WINDOWLIST far *		windowList;
	static HINSTANCE					hAppInstance;
	static HINSTANCE					hPrevInstance;
	static short							colorCount;

	static HWND	   			 			hWndParent;
	static HWND					 			hWndIconEditor;
	static PIXELWINDOW far*		pixelWindow;
	static COLORPALETTE far*	colorPalette;
	static short int					currentColor;
	static HWND								hLeftButton;
	static HWND								hRightButton;
	static BOOL								haveFileInfo;
	static LPSERIUSICON				theIcon;
	static short int					iconType;

	static BOOL CheckIconFile(
		int	fileHandle,
		LPSTR fileName);

	static BOOL registerWindowClass(void);

	static void Draw(void);

	static void DrawCurrentColor(
		HDC	hDC);

	static BOOL GetIconBitmap(
		int	fileHandle);

	static void DrawLabel(
		HDC			hDC,
		LPSTR		text,
		short		left,
		short		top,
		short		width);

	static BOOL VerifyClose(
		void);

	static void InitDialog(
		HWND	hDlg);

	static BOOL SaveIconInFile(
		void);

public:

	static int Dialog(
		HWND	  			hWndMain,
		HINSTANCE			hAppInstance,
		HINSTANCE			hPrevInstance,
		LPSERIUSICON	iconInfo,
		short int			iconType);

	static BOOL I_CALLBACK IconEditorDlgProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM		wParam,
		LPARAM		lParam);

	static void Show(void);

	static void SetCurrentColor(
		short	colorIndex);

	static void OpenIconFile(
		void);

	};


#endif
