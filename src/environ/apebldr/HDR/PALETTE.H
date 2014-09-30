#if (!defined(__palette_h))
	#define __palette_h

#include "classdef.h"
#include "memclass.h"
#include "popupmen.h"

#define ID_LISTBOX  	500
#define ID_LISTBOX2 	501
#define ID_LISTBOX3 	502
#define ID_NUMCAT   	503
#define ID_CATSTAT		504

#define APB_DRAGANDDROP_OBJECT 		(WM_USER + 2143)
#define APB_DRAGANDDROP_FUNCTION 	(WM_USER + 2144)
#define APB_GETSUBJECT						(WM_USER + 2145)
#define APB_DRAGANDDROP_OK 2
#define APB_HIGHLIGHTOBJECTINSERT	(WM_USER + 2146)
#define APB_CLEAROBJECTHIGHLIGHT	(WM_USER + 2147)

LRESULT CALLBACK PaletteWndProc (
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

class PALETTE:public MEMORY
	{
	private:
		static HWND lastValidDropWindow;
		static WORD dropType;
		static HCURSOR dropCursor;
		static POINT lastDropPoint;
		POINT palLoc;
		POINT turboPalLoc;

		LPPOPUPMENU categoryPopupPtr;
		LPPOPUPMENU objPopupPtr;
		LPPOPUPMENU funcPopupPtr;

		void PopUpCategoryList(
			POINT startPoint);

		void PopupObjList(
			POINT startPoint);

		void PopupFuncList(
			POINT startPoint);

		void MinimizeWindow();

		void MaximizeWindow();

		void ShowNeededWindows();
		
		void HideNeededWindows();

		static BOOL CALLBACK TurboPalConfigProc(
			HWND hDlg, 
			UINT msg, 
			WPARAM wParam, 
			LPARAM lParam);

		void ConfigureTurboPalette();

		void ReadPreferences();

		void WritePreferences();

	protected:
		BOOL           creating;
		HINSTANCE	     hInst;
    HFONT          hFont;
		HWND					 hwndPalette, hwndObject, hwndFunction, hwndCategory;
		HWND					 hwndNumCat, lastActiveWindow;
		HCURSOR				 hCursor;
		int						 nLeft, nTop, nWidth, nHeight;
		int					 catBegin, catHeight, catWidth, catStatHeight;
		int						 objWidth, funcWidth, nObjEnd, extraLine;
		static WNDPROC lpfnOldObj, lpfnOldFunc, lpfnOldCat, lpfnOldNumCat;
		static LPOBJDESC			 pAddObjDef;
		static LPAFUNCDESC			pAddFuncDef;
		static HBITMAP				upArrow;
		static HFONT 					tinyFont;
		long									status;
		HWND					hwndCatStatus;
 
		void Register();
		void CreatePaletteWindow(
			HWND parent);

	public:

		static WINDOWLIST * 	list;

		PALETTE(
			HWND hwnd);

		~PALETTE(
			void);

		LRESULT WndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void  DrawEntireItem(
			LPDRAWITEMSTRUCT lpdis);

		void  HandleFocus(
	    LPDRAWITEMSTRUCT lpdis);

		void show(
			void);

		void hide(
			void);

		void ActivateOldWindow();

		void FillFuncList (
			LPCFGDESC  pConfig);

		void SetFuncColumnWidth();

		void SetObjColumnWidth();

		HCURSOR MakeTheCursor(
			HDC					 hdc,
			LPSERIUSICON pIcon);

		HCURSOR PALETTE::MakeTheNameCursor(
			HDC			 		 hDC,
			LPSTR 			 theName);


		static LRESULT CALLBACK ObjProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK FuncProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK CategoryProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK NumCatProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void DrawExtraLine(
			int x);

		HWND GetPaletteHandle()
			{
			return hwndPalette;
			}

		void GetCursorRect(
			LPRECT rt,
			POINT refPt);

	};

#endif
