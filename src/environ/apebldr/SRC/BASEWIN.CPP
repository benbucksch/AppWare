#include "almsys.h"

#include "prjwin.h"
#include "wrksheet.h"
#include "project.h"
#include "progsys.h"
#include "mdiclass.h"
#include "fileobj.h"
#include "system.h"
#include "resconst.h"
#include "winlist.h"
#include "list.h"

WINDOWLIST * BASEWINDOW	::list 					 =(WINDOWLIST *)NULL;
HFONT           BASEWINDOW	::numberSubFont = NULL;
HRGN						BASEWINDOW	::clipRegion = NULL;
int							BASEWINDOW  ::windowCount=0;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	BASEWINDOW
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  Constructor which creates the static WindowList object
//							  and stores the this pointer in list
//
// ASSUMPTIONS.:	assumes enough memory to create the WindowList
/////////////////////////////////////////////////////////////////////////////

BASEWINDOW::BASEWINDOW(
	SERUID theProjectUID)

	{
	if (!list)
		list =(WINDOWLIST *)new WINDOWLIST;

	status = 0L;
	if(!windowCount)
		Initialize();
	windowCount++;
	fileObjPtr = pTheSystem->GetFileObject();
	projectUID = theProjectUID;
	}

BASEWINDOW::~BASEWINDOW()
	{
		windowCount--;
		if(!windowCount)
			{
			DeInitialize();
			delete list;
			list = NULL;
			}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetWindowHandle
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  returns the window handles hWnd
//
// RETURN VALUE:  HWND
/////////////////////////////////////////////////////////////////////////////

HWND BASEWINDOW::GetWindowHandle()
	{
	return (hWnd);
	}

void BASEWINDOW::SetWindowHandle(
	HWND newHandle)
	{
	hWnd = newHandle;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Show
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  shows the window hWnd
//
// PARAMETERS..:  ShowCmd- integer which specifies how the window is to
//								be shown.  Uses the same flags as used in the Windows
//								ShowWindow function.
//
// RETURN VALUE:  non-zero if the window was previously visible else zero
/////////////////////////////////////////////////////////////////////////////

BOOL BASEWINDOW::Show(
	int ShowCmd)
	{
	return(ShowWindow(hWnd, ShowCmd));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Hide
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  Hides the window hWnd
//
// RETURN VALUE:	non-zero if the window was previously visible else zero.
/////////////////////////////////////////////////////////////////////////////

BOOL BASEWINDOW::Hide()
	{
	return(ShowWindow(hWnd, SW_HIDE));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  forces the window hWnd's client area to be updated
/////////////////////////////////////////////////////////////////////////////

void BASEWINDOW::Update()
	{
	UpdateWindow(hWnd);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  function to create the various window objects and
//								tools used by this class and its child classes
/////////////////////////////////////////////////////////////////////////////

void BASEWINDOW::Initialize()
	{
	if (!clipRegion)
		{
		clipRegion = CreateRectRgn(0,0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN));
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeInitialize
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  function to delete the objects used by this class
//								and its child classes
/////////////////////////////////////////////////////////////////////////////

void BASEWINDOW::DeInitialize()
	{
	if (clipRegion)
		{
		DeleteObject(clipRegion);
		clipRegion = NULL;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	BaseWndProc
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  Windows sends all messages intended for the BASEWINDOW
//								objects and its child classes through this function.
//							  BaseWndProc then directs the messages to the appropriate
//                objects.
//
// PARAMETERS..:  standard WndProc parameters
//
// RETURN VALUE:  standard WndProc return value
/////////////////////////////////////////////////////////////////////////////
LPARAM CALLBACK BASEWINDOW::BaseWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	BASEWINDOW *				 		objectPtr;
	LPMDICREATESTRUCT 				mdi;
	LPCREATESTRUCT 						cs;
	long 											success=NULL;
	PROJECT *							theProject = NULL;
	LPFILEOBJECT							fileObjPtr;
	BOOL 											doPop=FALSE;

	if (!BASEWINDOW::list)
		return DefMDIChildProc(hWnd, message, wParam, lParam);
	objectPtr = (BASEWINDOW *)(BASEWINDOW::list->GetObject(hWnd));

	switch (message)
		{
		case WM_KEYDOWN:
			if(wParam == VK_DELETE  && objectPtr != NULL && !objectPtr->Busy())
				{
				PROGSYSTEM::PushAndRegisterContext(objectPtr->fileObjPtr);
				SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, IDM_DODELETE, NULL);
				PROGSYSTEM::PopAndRegisterContext();
				return 0;
				}
			break;

		case WM_CREATE:
			if (!objectPtr)
				{
				cs = (LPCREATESTRUCT)lParam;
				mdi = (LPMDICREATESTRUCT)cs->lpCreateParams;
				BASEWINDOW::list->Add(hWnd, (void *)mdi->lParam);
				objectPtr =(BASEWINDOW *) BASEWINDOW::list->GetObject(hWnd);
				if(pTheSystem->GetFileObject() != objectPtr->fileObjPtr)
					{
					PROGSYSTEM::PushAndRegisterContext(objectPtr->fileObjPtr);
					doPop=TRUE;
					}
				success = objectPtr->WndProc(hWnd, message, wParam, lParam);
				if(doPop)
					PROGSYSTEM::PopAndRegisterContext();
				}
			return success;

		case WM_DESTROY:
			if (objectPtr && BASEWINDOW::list->Remove(hWnd))
				{
				// the context should always be pushed to insure that it will
				// remain the active context throughout the delete process even
				// if SetFileObject is called.

				PROGSYSTEM::PushAndRegisterContext(objectPtr->fileObjPtr);
				success = objectPtr->WndProc(hWnd, message, wParam, lParam);
				if (objectPtr->TestProjectFlag())
					theProject = ((PROJECTWINDOW *)objectPtr)->GetProjectPtr();
				delete objectPtr;

				// if its a projectwindow then delete the associated project.
				if (theProject)
					{
					MDI::GetProjectList()->Remove(theProject->fileObjPtr);
					fileObjPtr = theProject->fileObjPtr;
					delete theProject;
					delete fileObjPtr;
					}
				PROGSYSTEM::PopAndRegisterContext();
				}
      return success;

		}
		if (objectPtr)
			{
			if(pTheSystem->GetFileObject() != objectPtr->fileObjPtr)
				{
				PROGSYSTEM::PushAndRegisterContext(objectPtr->fileObjPtr);
				doPop=TRUE;
				}
			success= objectPtr->WndProc(hWnd, message, wParam, lParam);
			if(doPop)
				PROGSYSTEM::PopAndRegisterContext();
			}
		else
			success=DefMDIChildProc(hWnd, message, wParam, lParam);

	return success;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetMouseWindow
//
// CLASS.......:  BASEWINDOW
//
// DESCRIPTION.:  determines which of the basewindow class windows, if any,
//								contains the cursor
//
// PARAMETERS..:  **IMPORTANT** point must be in screen coordinates
//
// RETURN VALUE:  the pointer to the object of the window containing the
//								cursor, or NULL if none of the children of the client
//								contains the cursor
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BASEWINDOW * BASEWINDOW::GetMouseWindow(
	POINT pt)
	{
	HWND	hWnd;
	BASEWINDOW * basePtr;

	ScreenToClient(PROGSYSTEM::hWndClient, (LPPOINT)&pt);
	hWnd = ChildWindowFromPoint (PROGSYSTEM::hWndClient, pt);
	basePtr = (BASEWINDOW *)list->GetObject (hWnd);

	return basePtr;
	}

