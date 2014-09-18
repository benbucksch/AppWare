#include "a_alm.h"
#include "windows.h"
#include "picture.h"
#include "stdlib.h"
#include "o_wnd.h"
#include "op_wnd.h"

// used to set def. size during WIDR
#define PICT_DEFWT 100;
#define PICT_DEFHT 100;

extern  HANDLE hInst;  // Initialized in the DESC routine

extern void DrawPicture (HDC hDC, PICTOBJECT *picture);
// function to compare the size of two rects
extern BOOL EqualSizeRect(LPRECT srcRect, LPRECT rect);
// Function to delete all picture Handles for different visual instances
extern int DeletePictHandle(PICTOBJECT *po);


extern "C" {

long far pascal   _export PictWidrProc(
	HWND		hwnd,
	WORD		message,
	WORD		wParam,
	LONG		lParam);
        }
// typedef Rect far *RectPtr;
// typedef RectPtr RectHandle;


// PictureDraw ( OBJECTID theObject, RectHandle listRect );

//implementation






long far pascal   _export PictWidrProc(
	HWND		hwnd,
	WORD		message,
	WORD		wParam,
	LONG		lParam)
	{
	POINT			pt;

	switch(message)
		{
		case WM_MOUSEACTIVATE:
			wParam = (WORD)GetParent(hwnd);
			return SendMessage(GetParent(hwnd), message, wParam, lParam);
		case WM_SETCURSOR:
			return TRUE;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
			//Convert lParam to the parents coords
			pt = MAKEPOINT(lParam);
			ClientToScreen(hwnd, &pt);
			ScreenToClient(GetParent(hwnd), &pt);
			lParam = MAKELONG(pt.x, pt.y);
			return SendMessage(GetParent(hwnd), message, wParam, lParam);

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);

		}
	}





extern "C" void PASCAL FAR _export widrProc (
	pAEvtInfo		theEvent,
	pAObjMessage   			theSystem)
	{
	long 							rVal;
	OBJECTID 					theObject;
	WIDR_BLOCK 				*widrBlock = (WIDR_BLOCK *)theEvent->details;
	PICTOBJECT 				*pObject;

	theObject = theEvent->targetObject;

	if (widrBlock->flag == WIDR_FLAG_INIT)
		{ // during start
		pObject = new PICTOBJECT((OBJECTID) theObject, widrBlock->hWndParent);
		if (widrBlock->rectPtr->bottom == 0)
			{ // resize
			widrBlock->rectPtr->right = widrBlock->rectPtr->left + PICT_DEFWT;
			widrBlock->rectPtr->bottom = widrBlock->rectPtr->top + PICT_DEFHT;
			}

		 pObject->pVInst = (PICTURE_VINST *) new char[sizeof(PICTURE_VINST)];

		 pObject->pVInst->hPaintWnd = widrBlock->hWndParent;
		 pObject->bClientRect = FALSE;
     pObject->pVInst->hPict = NULL;  // create a new one
		 pObject->drawRect = *(widrBlock->rectPtr);
		 widrBlock->data = (LONG)pObject;
		}
	else if (widrBlock->flag == WIDR_FLAG_DRAW)
		{
		HDC hItemDC =    widrBlock->hdc;

		pObject = (PICTOBJECT *)widrBlock->data;

		// if the picture has been resized than recreate picture handle
		if (EqualSizeRect(&(pObject->drawRect), widrBlock->rectPtr) == FALSE)
			{
			if (pObject->pVInst->hPict)
				DeletePictHandle(pObject);
				
			pObject->pVInst->hPict = NULL;

			}
		
		

		pObject->drawRect = *(widrBlock->rectPtr);


	
		// This will create a picture handle only if we already do not have one
		pObject->CreatePictH(hItemDC);

		// SKJ, 3/08/93.. This will intimate to the window
		// object that there is transparent region in the
		// picture layout rect
	
		widrBlock->isTransparent = TRUE;
      

		DrawPicture(hItemDC,pObject);


		FrameRect(hItemDC, &(pObject->drawRect), GetStockObject(BLACK_BRUSH));

		}
	else {  // during close  free all widr specific datas

	  pObject = (PICTOBJECT *)widrBlock->data;

		// pObject->pVInst = (PICTURE_VINST *)new char[sizeof(PICTURE_VINST)];

		// delete pict handle
		DeletePictHandle(pObject);
    delete pObject->pVInst;
		delete (PICTOBJECT *)(pObject);
		}

 }

