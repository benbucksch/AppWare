#if !defined( __NAVCLS_H )
#define __NAVCLS_H

#include "memclass.h"
#include "tstring.h"
#include "seruid.h"
#include "classdef.h"

#define NAV_LEFT		350
#define NAV_TOP			200
#define NAV_WIDTH		150
#define NAV_HEIGHT 	150

#define ITEM_PROJECT	1
#define ITEM_SUBJECT	2
#define ITEM_COMMENT	3

class ITEM;

typedef ITEM *  LPITEM;

class ITEM:public MEMORY{
	public:
	TSTRING									name;
	short int								type;
	SERUID				 					itemUID;
	LPFILEOBJECT						itemContext;
	LPITEM									next;
	LPITEM									first;
	long    								status;

	ITEM(
		LPCSTR							namePtr,
		LPFILEOBJECT				filePtr,
		SERUID							itemUID,
		short int						type);

	LPITEM FindItem(
		SERUID        itemUID,
		LPFILEOBJECT  itemContext,
		short &       level);

	};


class NAVIGATOR:public MEMORY{

	private:

	static WINDOWLIST  *	windowList;
	static HINSTANCE				hAppInstance;
	static WNDPROC					originalLBWindowProc;
	HWND	   			 					hWndParent;
	HWND					 					hWndNavigator;
	HWND					 					hWndListBox;
	short										selectedIndex;
	LPITEM									headItem;
	short										clickLocation;

	BOOL registerWindowClass(void);

	void SetListBoxFont(
		void);

	void resizeWindow(
		short	width,
		short	height);

	void drawItem(
		DRAWITEMSTRUCT*	drawItemInfo);

	void insertItem(
		LPITEM								firstItem,
		LPCSTR								name,
		LPFILEOBJECT      		fileObjPtr,
		SERUID								itemUID,
		short int							type);

	LPITEM findItem(
		LPITEM	itemList,
		LPFILEOBJECT fileObjPtr,
		SERUID itemUID);

	void Expand(
		LPITEM		 	list,
		short				index);

	void Contract(
		LPITEM		 	list,
		short				index);

	void ExpandOrContract(
		void);

	short GetIndex(
		LPITEM 	item);

	void ActivateWindow(void);

	void RemoveSubItems(
		LPITEM	subItemList);

	BOOL RemoveItem(
		LPITEM 	parentItem,
		LPFILEOBJECT fileObjPtr,
		SERUID itemUID);

	void ChangeItemName(
		LPITEM	parentItem,
		LPCSTR	oldName,
		LPCSTR	newName);

	public:

	NAVIGATOR(
		HWND				hWndMain,
		HINSTANCE 	hAppInstance,
		HINSTANCE 	hPrevInstance,
		int     left = NAV_LEFT,
		int			top = NAV_TOP,
		int			width = NAV_WIDTH,
		int			height = NAV_HEIGHT);

	~NAVIGATOR(void);

	void show(void);

	void hide(void);

  HWND GetNavigatorWindow()
    {
    return hWndNavigator;
    }

	LRESULT WindowProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM		wParam,
		LPARAM		lParam);

	LRESULT ListBoxWindowProc(
		HWND 				hWnd,
		UINT		 		message,
		WPARAM			wParam,
		LPARAM			lParam);

	BOOL AddItem(
		SERUID								projectUID,
		SERUID		 						subjectUID,
		SERUID								commentUID,
		LPFILEOBJECT					fileObjPtr,
		LPCSTR								itemName);

	BOOL DeleteItem(
		LPFILEOBJECT					fileObjPtr,
		SERUID								projectUID,
		SERUID								subjectUID,
		SERUID								commentUID);

	BOOL SetItemName(
		SERUID       itemUID,
		LPFILEOBJECT itemContext,
		LPCSTR			 newName);

	friend LRESULT CALLBACK NAVIGATORWndProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM 		wParam,
		LPARAM 		lParam);

	friend LRESULT CALLBACK ListBoxWndProc(
		HWND 			hWnd,
		UINT		 	message,
		WPARAM		wParam,
		LPARAM		lParam);
	};

#endif    // __NAVCLS_H
