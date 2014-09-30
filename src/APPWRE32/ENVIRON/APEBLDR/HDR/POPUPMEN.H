#ifndef __POPUPMEN_H
#define __POPUPMEN_H

#include "memclass.h"
#include "classdef.h"

#define POPUP_MENU_CODE 			(WM_USER+1000)

// POPUP_DESTROY message send to owner window when menu is closed
#define POPUP_DESTROY 				(POPUP_MENU_CODE + 1)
#define POPUP_OUTOFBOUNDS			(POPUP_MENU_CODE + 2)

#define NO_ORDINAL -1
#define SHOW_ORDINAL 1

//One of these types must be passed in throught the constructor
//The default will be POPUP_PARENTNOTIFY
#define POPUP_PARENTNOTIFY 	0x00000001
#define POPUP_MESSAGEHOG 		0x00000002

//what mouse button activated the menu
#define POPUP_LEFTBUTTON		0x00000004
#define POPUP_RIGHTBUTTON		0x00000008

//signal to close the popup when we move horizontally out of bounds
#define POPUP_NOOUTOFBOUNDS	0x00000010

//centers the menu horizontally over the point given in show window
#define POPUP_HCENTER 			0x00000020

//creates a popup of maximum size
//popup will not show an initial selected item.
#define POPUP_MAXSIZE				0x00000040

#define POPUP_SORTED				0x00000080

//popup has extended style
#define POPUP_EXTENDED	0x00000100

//the data field must point to a AOBJDESC structure
#define POPUP_OBJICONS			0x00000200

// the data field must point ot a AFUNCDESC structure
#define POPUP_FUNCICONS			0x00000400

//extended selection possibilites
#define POPUP_EXT_DRAGSELECTION 	-1
#define POPUP_EXT_NOSELECTION 		0
#define POPUP_EXT_NORMALSELECTION 1


#define POPUP_NOSELECTED		-1


class POPUPMENU:public MEMORY
	{
	private:
		static BOOL registered;
		static WINDOWLIST * menuList;
		static WINDOWLIST * listboxList;
		static int count;
		HWND hWndOwner;
		HWND hListMenu;
		HFONT hFont;
		int longestItem;
		HWND hWndMenu;
		int selectedItem;
		long selectedData;
		WNDPROC oldProc;
		long flags;
		BOOL okToScroll;
		UINT hTimer;
		long popupType;
		HWND hWndOldCapture;
		int m_itemHeight;
		POINT m_textOffset;
		int m_validSelect;
		int m_startSelect;
		int m_startX;
		DWORD m_startTime;			

		//used for extended popup
		RECT selectedLocation;
		int	selectionStatus;

		void DrawItem(
			HDC hdc,
			LPRECT bounds,
			int index);

		void HandleTimer();

	public:

		POPUPMENU(
			HWND owner,
			HFONT font,
			long type = POPUP_PARENTNOTIFY|POPUP_LEFTBUTTON);

		~POPUPMENU();

		void AppendItem(
			LPCSTR item,
			long  data,
			BOOL includeOrdinal);

		BOOL ShowPopup(
			POINT leftPt,
			long selected);

		int ShowPopupExt(
			POINT leftPt, 
			long selected,
			LPRECT selLocation);

		int GetItemIndex()
			{
			return selectedItem;
			}

		long GetItemData()
			{
			return selectedData;
			}

		LRESULT ListProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		LRESULT OuterWinProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK POPUPMENU::ListBoxWndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK POPUPMENU::WndProc(
			HWND hWnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		void PurgeList();

 		POPUPMENU& operator= (const POPUPMENU& rhs);

	 long GetType()
			{
			return popupType;
			}

	 };

#endif
