#include "almsys.h"
#include <windowsx.h>

#include "mdichild.h"
#include "serdev.h"
#include "resconst.h"
#include "compdesc.h"
#include "objdesc.h"
#include "newcomp.h"
#include "objdsdlg.h"
#include "fundsdlg.h"
#include "evtdsdlg.h"
#include "rplcdll.h"
#include "sereclib.h"
#include "devsys.h"
#include "rectypes.h"
#include "mdiapp.h"
#include "cfgdesc.h"
#include "funcdesc.h"
#include "evntdesc.h"
#include "serblk.h"
#include "rtreclib.h"
#include "rplchelp.h"

#define SCALE_FACTOR	2

#define MDICHILD_WIDTH		(196*SCALE_FACTOR)
#define MDICHILD_HEIGHT		(149*SCALE_FACTOR)

#define LISTBOX_LEFT      (13*SCALE_FACTOR)
#define LISTBOX_TOP				(16*SCALE_FACTOR)
#define LISTBOX_WIDTH     (105*SCALE_FACTOR)
#define LISTBOX_HEIGHT    (90*SCALE_FACTOR)

#define OBJECTS_LEFT      (133*SCALE_FACTOR)
#define OBJECTS_TOP       (22*SCALE_FACTOR)
#define OBJECTS_WIDTH     (43*SCALE_FACTOR)
#define OBJECTS_HEIGHT    (14*SCALE_FACTOR)

#define	FUNCTIONS_LEFT    (133*SCALE_FACTOR)
#define	FUNCTIONS_TOP     (35*SCALE_FACTOR)
#define	FUNCTIONS_WIDTH   (46*SCALE_FACTOR)
#define	FUNCTIONS_HEIGHT  (12*SCALE_FACTOR)

#define EVENTS_LEFT       (133*SCALE_FACTOR)
#define EVENTS_TOP        (47*SCALE_FACTOR)
#define EVENTS_WIDTH      (41*SCALE_FACTOR)
#define EVENTS_HEIGHT     (12*SCALE_FACTOR)

#define BNGROUP_LEFT        (128*SCALE_FACTOR)
#define BNGROUP_TOP         (12*SCALE_FACTOR)
#define BNGROUP_WIDTH       (53*SCALE_FACTOR)
#define BNGROUP_HEIGHT      (49*SCALE_FACTOR)

#define LBGROUP_LEFT        (6*SCALE_FACTOR)
#define LBGROUP_TOP         (4*SCALE_FACTOR)
#define LBGROUP_WIDTH       (182*SCALE_FACTOR)
#define LBGROUP_HEIGHT      (108*SCALE_FACTOR)

#define CATLABEL_LEFT        (12*SCALE_FACTOR)
#define CATLABEL_TOP         (121*SCALE_FACTOR)
#define CATLABEL_WIDTH       (62*SCALE_FACTOR)
#define CATLABEL_HEIGHT      (8*SCALE_FACTOR)

#define CATFIELD_LEFT        (76*SCALE_FACTOR)
#define CATFIELD_TOP         (119*SCALE_FACTOR)
#define CATFIELD_WIDTH       (107*SCALE_FACTOR)
#define CATFIELD_HEIGHT      (12*SCALE_FACTOR)


#define INSERT_LEFT      (134*SCALE_FACTOR)
#define INSERT_TOP       (64*SCALE_FACTOR)
#define INSERT_WIDTH     (41*SCALE_FACTOR)
#define INSERT_HEIGHT    (12*SCALE_FACTOR)

#define EDIT_LEFT        (134*SCALE_FACTOR)
#define EDIT_TOP         (80*SCALE_FACTOR)
#define EDIT_WIDTH       (41*SCALE_FACTOR)
#define EDIT_HEIGHT      (12*SCALE_FACTOR)

#define DELETE_LEFT      (134*SCALE_FACTOR)
#define DELETE_TOP       (96*SCALE_FACTOR)
#define DELETE_WIDTH     (41*SCALE_FACTOR)
#define DELETE_HEIGHT    (12*SCALE_FACTOR)

#define IDC_CONTROL_BASE	500

#define IDC_LISTBOX			(IDC_CONTROL_BASE + 1)
#define IDC_OBJECTS     (IDC_CONTROL_BASE + 2)
#define IDC_FUNCTIONS   (IDC_CONTROL_BASE + 3)
#define IDC_EVENTS      (IDC_CONTROL_BASE + 5)
#define	IDC_TEXTTYPE    (IDC_CONTROL_BASE + 6)
#define IDC_BNGROUP     (IDC_CONTROL_BASE + 7)
#define IDC_INSERT      (IDC_CONTROL_BASE + 8)
#define IDC_DELETE      (IDC_CONTROL_BASE + 9)
#define IDC_LBGROUP     (IDC_CONTROL_BASE + 10)
#define IDC_CATLABEL    (IDC_CONTROL_BASE + 11)
#define IDC_CATFIELD    (IDC_CONTROL_BASE + 12)
#define IDC_EDIT        (IDC_CONTROL_BASE + 13)

#define WM_NEXTBUTTON		(WM_USER + 1)
#define WM_PREVBUTTON   (WM_USER + 2)


#define MDICHILD_WIN_CLASS "mdiChildWindow"

////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static data members of MDICHILD class
////////////////////////////////////////////////////////////////////////////

BOOL 						MDICHILD::isRegistered = FALSE;
MDICHILD*       MDICHILD::windowList = NULL;
HICON						MDICHILD::hWindowIcon = 0;
WNDPROC					MDICHILD::origListboxWndProc;
WNDPROC					MDICHILD::origButtonWndProc;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MDICHILD
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  Constructor which creates the static WindowList object
//							  and stores the this pointer in list
//
// ASSUMPTIONS.:	assumes enough memory to create the WindowList
/////////////////////////////////////////////////////////////////////////////

MDICHILD::MDICHILD(
	HINSTANCE			hInstance,
	HINSTANCE			hPrevInstance,
	HWND	 				hFrameWindow,
	HWND					hClientWindow,
	LPFILEOBJECT	fileObject)
	{
	MDICREATESTRUCT 	mdiCreate;
  LPCFGDESC         cfgDesc;

  cfgDesc = (LPCFGDESC)GetObject(fileObject->GetHeadRec());

	this->next = windowList;
	windowList = this;

	this->hAppInstance = hInstance;
	this->hPrevInstance = hPrevInstance;
	this->hWndFrame = hFrameWindow;
	this->hWndClient = hClientWindow;

	this->fileObject = fileObject;

	objectNameSeed = functionNameSeed =	eventNameSeed =	1;

	if (hWindowIcon == 0)
		hWindowIcon = LoadIcon(hInstance, "childWindowIcon");

	if (!hPrevInstance)
		{
		if (!isRegistered)
			isRegistered = Register();
		}
	else
		isRegistered = TRUE;

	if (isRegistered == TRUE)
		{
		mdiCreate.szClass = MDICHILD_WIN_CLASS;
		mdiCreate.szTitle = fileObject->GetName();
		mdiCreate.hOwner = hAppInstance;
		mdiCreate.x = CW_USEDEFAULT;
		mdiCreate.y = CW_USEDEFAULT;
		mdiCreate.cx = MDICHILD_WIDTH;
		mdiCreate.cy = MDICHILD_HEIGHT;
		mdiCreate.style = 0L;
		mdiCreate.lParam =(long)this;
		if (SendMessage(hWndClient, WM_MDICREATE, 0, (LONG)(LPSTR)&mdiCreate))
			{
			hLBGroup = CreateWindow("BUTTON", "Objects", WS_VISIBLE|WS_CHILD|BS_GROUPBOX,
								LBGROUP_LEFT, LBGROUP_TOP, LBGROUP_WIDTH, LBGROUP_HEIGHT,
								hWindowHandle, (HMENU) IDC_LBGROUP, hAppInstance, NULL);
			hListbox = CreateWindow("LISTBOX", NULL,
			 WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER|LBS_EXTENDEDSEL|
			 LBS_SORT,
			 LISTBOX_LEFT, LISTBOX_TOP, LISTBOX_WIDTH, LISTBOX_HEIGHT,
			 hWindowHandle, (HMENU) IDC_LISTBOX, hAppInstance,	NULL);

			origListboxWndProc =
					(WNDPROC)GetWindowLong(hListbox, GWL_WNDPROC);
			SetWindowLong(hListbox, GWL_WNDPROC, (long)MDICHILD::ControlWndProc);


			hObjects = CreateWindow("BUTTON", "Objects", WS_VISIBLE|WS_CHILD|BS_RADIOBUTTON,
									OBJECTS_LEFT, OBJECTS_TOP, OBJECTS_WIDTH, OBJECTS_HEIGHT,
									hWindowHandle, (HMENU) IDC_OBJECTS, hAppInstance,	NULL);

			origButtonWndProc =
					(WNDPROC)GetWindowLong(hObjects, GWL_WNDPROC);
			SetWindowLong(hObjects, GWL_WNDPROC, (long)MDICHILD::ControlWndProc);

			hFunctions = CreateWindow("BUTTON", "Functions",
										WS_VISIBLE|WS_CHILD|BS_RADIOBUTTON,
										FUNCTIONS_LEFT, FUNCTIONS_TOP,
										FUNCTIONS_WIDTH, FUNCTIONS_HEIGHT,
										hWindowHandle, (HMENU) IDC_FUNCTIONS, hAppInstance,	NULL);

			SetWindowLong(hFunctions, GWL_WNDPROC, (long)MDICHILD::ControlWndProc);

			hEvents = CreateWindow("BUTTON", "Events", WS_VISIBLE|WS_CHILD|BS_RADIOBUTTON,
								 EVENTS_LEFT, EVENTS_TOP, EVENTS_WIDTH, EVENTS_HEIGHT,
								 hWindowHandle, (HMENU) IDC_EVENTS, hAppInstance,	NULL);

			SetWindowLong(hEvents, GWL_WNDPROC, (long)MDICHILD::ControlWndProc);

			hBNGroup = CreateWindow("BUTTON", "View", WS_VISIBLE|WS_CHILD|BS_GROUPBOX,
								BNGROUP_LEFT, BNGROUP_TOP, BNGROUP_WIDTH, BNGROUP_HEIGHT,
								hWindowHandle, (HMENU) IDC_BNGROUP, hAppInstance, NULL);

			hCatLabel = CreateWindow("STATIC", "Function Category:",
							 WS_VISIBLE|WS_CHILD|SS_LEFTNOWORDWRAP|SS_NOPREFIX,
							 CATLABEL_LEFT, CATLABEL_TOP, CATLABEL_WIDTH, CATLABEL_HEIGHT,
							 hWindowHandle, (HMENU) IDC_CATLABEL, hAppInstance, NULL);

			hCatField = CreateWindow("EDIT", cfgDesc->GetFuncGroupName(),
							 WS_VISIBLE|WS_CHILD|ES_LEFT|WS_BORDER|ES_AUTOHSCROLL,
							 CATFIELD_LEFT, CATFIELD_TOP, CATFIELD_WIDTH, CATFIELD_HEIGHT,
							 hWindowHandle, (HMENU) IDC_CATFIELD, hAppInstance, NULL);

      if (hCatField != NULL)
        Edit_LimitText(hCatField, 80);

      hInsert = CreateWindow("BUTTON", "Insert...",
							 WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
               INSERT_LEFT, INSERT_TOP, INSERT_WIDTH, INSERT_HEIGHT,
               hWindowHandle, (HMENU) IDC_INSERT, hAppInstance, NULL);

      hEdit = CreateWindow("BUTTON", "Edit...",
							 WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
               EDIT_LEFT, EDIT_TOP, EDIT_WIDTH, EDIT_HEIGHT,
               hWindowHandle, (HMENU) IDC_EDIT, hAppInstance, NULL);

      hDelete = CreateWindow("BUTTON", "Delete",
							 WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
               DELETE_LEFT, DELETE_TOP, DELETE_WIDTH, DELETE_HEIGHT,
               hWindowHandle, (HMENU) IDC_DELETE, hAppInstance, NULL);


      hCurrentButton = NULL;
			SetCurrentButton(hObjects);

			}
		}

  fileObject->ClearDirtyFlag();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Register
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  registers window class
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL MDICHILD::Register(
	void)
	{
	WNDCLASS wc;

	wc.style = CS_VREDRAW | CS_HREDRAW|CS_DBLCLKS;;
	wc.lpfnWndProc = MDICHILD::MDICHILDWndProc;
	wc.hInstance = hAppInstance;
	wc.hIcon = hWindowIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = MDICHILD_WIN_CLASS;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	if (0 != RegisterClass((LPWNDCLASS)&wc))
		return TRUE;
	else
		return FALSE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~MDICHILD
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.: 	Removes the object from the window list.
//
/////////////////////////////////////////////////////////////////////////////

MDICHILD::~MDICHILD(
	VOID)
	{

	if (windowList == this)
		windowList = this->next;
	else
		{
		MDICHILD * previous	= windowList;

		while (previous->next && (previous->next != this))
			previous = previous->next;
		previous->next = this->next;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Show
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  shows the window hWnd
//
// PARAMETERS..:  ShowCmd - integer which specifies how the window is to
//								be shown.  Uses the same flags as used in the Windows
//								ShowWindow function.
//
// RETURN VALUE:  non-zero if the window was previously visible else zero
/////////////////////////////////////////////////////////////////////////////

BOOL MDICHILD::Show(
	int ShowCmd)
	{
	BOOL result;

	result = ShowWindow(hWindowHandle, ShowCmd);

	SetFocus(hListbox);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Hide
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  Hides the window hWnd
//
// RETURN VALUE:	non-zero if the window was previously visible else zero.
/////////////////////////////////////////////////////////////////////////////
BOOL MDICHILD::Hide()
	{
	return(ShowWindow(hWindowHandle, SW_HIDE));
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  forces the window hWnd's client area to be updated
/////////////////////////////////////////////////////////////////////////////

void MDICHILD::Update()
	{
	UpdateWindow(hWindowHandle);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MDICHILDWndProc
//
// CLASS.......:  None
//
// DESCRIPTION.:  Windows sends all messages intended for the MDICHILD
//								objects and its child classes through this function.
//							  BaseWndProc then directs the messages to the appropriate
//                objects.
//
// PARAMETERS..:  standard WndProc parameters
//
// RETURN VALUE:  standard WndProc return value
/////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MDICHILD::MDICHILDWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	MDICHILD *				 windowObject;
	LPCREATESTRUCT  		 lpcs;
	LPMDICREATESTRUCT 	 mdics;
	long 								 result;

	windowObject =	GetWindowObject(hwnd);

	if (!windowObject)
		{
		if (message == WM_CREATE)
			{
			lpcs = (LPCREATESTRUCT) lParam;
			mdics = (LPMDICREATESTRUCT)lpcs->lpCreateParams;
			windowObject = (MDICHILD *)mdics->lParam;
			windowObject->hWindowHandle = hwnd;
			result = windowObject->WindowProcedure(hwnd, message, wParam, lParam);
			}
		else
			result = DefMDIChildProc(hwnd, message, wParam, lParam);
		}
	else
		{
		result = windowObject->WindowProcedure(hwnd, message, wParam, lParam);
		if (message == WM_DESTROY)
			delete windowObject;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ControlWndProc
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:
//
// PARAMETERS..:  standard paramaters for a WndProc procedure.
//
// RETURN VALUE:  standard return value for a WndProc.
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MDICHILD::ControlWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LONG controlID;
	LONG result;
	BOOL msgHandled = FALSE;

	controlID = GetWindowLong(hwnd, GWL_ID);

	if (message == WM_KEYDOWN)
		switch (wParam)
			{
			case VK_TAB:
				result = SendMessage(GetParent(hwnd), WM_NEXTDLGCTL, controlID, 0L);
				msgHandled = TRUE;
				break;
			case VK_UP:
				if (controlID != IDC_LISTBOX)
					{
					result = SendMessage(GetParent(hwnd), WM_PREVBUTTON, controlID, 0L);
					msgHandled = TRUE;
					}
				break;
			case VK_DOWN:
				if (controlID != IDC_LISTBOX)
					{
					result = SendMessage(GetParent(hwnd), WM_NEXTBUTTON, controlID, 0L);
					msgHandled = TRUE;
					}
				break;

			default:
				msgHandled = FALSE;
				break;
			}

	if (msgHandled == FALSE)
		switch (controlID)
			{
			case IDC_OBJECTS:
			case IDC_FUNCTIONS:
			case IDC_EVENTS:
				result = CallWindowProc((FARPROC)MDICHILD::origButtonWndProc, hwnd, message,
				 wParam, lParam);
				break;
			case IDC_LISTBOX:
				result = CallWindowProc((FARPROC)MDICHILD::origListboxWndProc, hwnd, message,
				 wParam, lParam);
				break;
			}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WindowProcedure
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  Windows will direct all messaes for the Frame Window
//								through FrameWndProc.  Since the function is static this
//								is allowable.
//
// PARAMETERS..:  standard paramaters for a WndProc procedure.
//
// RETURN VALUE:  standard return value for a WndProc.
//
// ASSUMPTIONS.:
//
//                             WARNING!!!!
//
//    Do not embed a return in the middle of this routine as it will mess up
//    the pushing and popping the file context.
//
/////////////////////////////////////////////////////////////////////////////
LRESULT MDICHILD::WindowProcedure(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	BOOL		msgHandled = TRUE;
	LPPOINT	pt;
	long		result = NULL;
	int			messageAnswer;
	char		buffer[81];		//long enough to hold message.
  int     selectCount;

	pTheSystem->PushContext(fileObject);

	switch (message)
		{
		case WM_NEXTBUTTON:
			switch (wParam)
				{
				case IDC_OBJECTS:
					SetCurrentButton(hFunctions);
					break;
				case IDC_FUNCTIONS:
					SetCurrentButton(hEvents);
					break;
				case IDC_EVENTS:
					SetCurrentButton(hObjects);
					break;
				default:
					break;
				}
			break;

		case WM_PREVBUTTON:
			switch (wParam)
				{
				case IDC_OBJECTS:
					SetCurrentButton(hEvents);
					break;
				case IDC_FUNCTIONS:
					SetCurrentButton(hObjects);
					break;
				case IDC_EVENTS:
					SetCurrentButton(hFunctions);
					break;
				default:
					break;
				}
			break;

		case WM_NEXTDLGCTL:
			switch (wParam)
				{
				case IDC_OBJECTS:
				case IDC_FUNCTIONS:
				case IDC_EVENTS:
					SetFocus(hListbox);
					break;
				case IDC_LISTBOX:
					SetFocus(hCurrentButton);
					break;
				default:
					break;
				}
			break;

		case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			if (wID > IDC_CONTROL_BASE)
				{
        if (wID == IDC_INSERT)
   				AddComponent();
        else if (wID == IDC_DELETE)
					DeleteComponent();
        else if (wID == IDC_EDIT)
          {
					if (ListBox_GetSelCount(hListbox) == 1)
						OpenDescDialog();
          }
				else if (wNotify == BN_CLICKED &&
						wID != GetWindowLong(hCurrentButton, GWL_ID))
					{
					switch (wID)
						{
						case IDC_OBJECTS:
							SetCurrentButton(hObjects);
							break;
						case IDC_FUNCTIONS:
							SetCurrentButton(hFunctions);
							break;
						case IDC_EVENTS:
							SetCurrentButton(hEvents);
							break;
						default:
							break;
            }
					}
				else if (wID==IDC_LISTBOX)
					{
					switch (wNotify)
						{
						case LBN_DBLCLK:
							if (ListBox_GetSelCount(hListbox) == 1)
								OpenDescDialog();
							break;
						case LBN_SELCHANGE:
              selectCount = ListBox_GetSelCount(hListbox);
							if (selectCount == 1)
                {
                EnableWindow(hEdit, TRUE);
                }
              else
                {
                EnableWindow(hEdit, FALSE);
                }
              if (selectCount > 0)
                {
                EnableWindow(hDelete, TRUE);
                }
              else
                {
                EnableWindow(hDelete, FALSE);
                }
							msgHandled = TRUE;
							break;
						default:
							msgHandled = FALSE;
							break;
						}
					}
        else if (wID == IDC_CATFIELD)
          {
					switch (wNotify)
						{
						case EN_CHANGE:
              fileObject->SetDirtyFlag();
              break;
						default:
							msgHandled = FALSE;
							break;
						}
          }
				}
			else   							//handle menu selections
				switch (wID)
					{
					case IDM_NEWCOMP:
						AddComponent();
						break;
					case IDM_DELETE:
						DeleteComponent();
						break;
					case IDM_EDITCOMP:
						if (ListBox_GetSelCount(hListbox) == 1)
							OpenDescDialog();
						break;
          case IDM_REPLACEDLL:
            ReplaceDLLDlgBox(hAppInstance, hwnd,
             (LPCFGDESC)GetObject(fileObject->GetHeadRec()));
            break;

					case IDM_REPLACEHELPFILE:
						ReplaceHelpFileDlgBox(hAppInstance, hwnd,
							(LPCFGDESC)GetObject(fileObject->GetHeadRec()));
						break;

					default:
						msgHandled = FALSE;
					}
			break;
			}

		case WM_GETMINMAXINFO:
			pt = LPPOINT(lParam);
			(pt+3)->x = MDICHILD_WIDTH;
			(pt+3)->y = MDICHILD_HEIGHT;
			(pt+4)->x = MDICHILD_WIDTH;
			(pt+4)->y = MDICHILD_HEIGHT;
			break;

		case WM_MDIACTIVATE:
			{
			BOOL		fActivate	= GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wParam, lParam);

			if (fActivate)
				{
				DEVSYSTEM::GetMdiApp()->SetActiveChild(this);
				pTheSystem->SetFileObject(fileObject);
				}
			else
				{
				DEVSYSTEM::GetMdiApp()->SetActiveChild(NULL);
				pTheSystem->SetFileObject(NULL);
				}
			break;
			}

		case WM_CLOSE:
		case WM_QUERYENDSESSION:
			if(fileObject->TestDirtyFlag())
				{
				if(!IsIconic(hWindowHandle))
					BringWindowToTop(hWindowHandle);
				wsprintf((LPSTR)buffer,
					(LPSTR)"Save changes to %s ?",
					(LPCSTR)fileObject->GetName());
				MessageBeep(1);
				messageAnswer = MessageBox(hWndFrame,(LPSTR)buffer,"WARNING",
					MB_YESNOCANCEL | MB_ICONEXCLAMATION |MB_SYSTEMMODAL);
				if(messageAnswer == IDCANCEL)
					result = FALSE;
				else if(messageAnswer == IDYES)
					{
					Edit_GetText(hCatField, buffer, 81);
          ((LPCFGDESC)GetObject(fileObject->GetHeadRec()))->
           SetFuncGroupName(buffer);
					if (fileObject->TestNewFlag())
						result = DEVSYSTEM::GetMdiApp()->SaveAs();
					else
						result = fileObject->Save();
					if (result)
						msgHandled = FALSE;	//call default winproc to close window
					}
				else if (messageAnswer == IDNO)
					{
					fileObject->ClearDirtyFlag();
					msgHandled = FALSE;
					}
				}
			else
				{
				msgHandled = FALSE;   //call default winproc to close window
				result = TRUE;
				}

			//window is responsible for restoring itself from an iconized state
			//before being destroyed.
			SendMessage(hWndClient, WM_MDIRESTORE, (WPARAM) hWindowHandle, NULL);
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	if (msgHandled == FALSE)
		result = DefMDIChildProc(hwnd, message, wParam, lParam);

	pTheSystem->PopContext();

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetCurrentButton
//
// CLASS.......:  MDICHILD
//
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::SetCurrentButton(
	HWND	hButton)
	{
	char				buffer[10];
	LPCOMPDESC  comp;
	RECT				rect;
	LPCFGDESC		cfg;
	int					newID;
  BOOL        haveSome = FALSE;

	cfg = (LPCFGDESC)GetObject(fileObject->GetHeadRec());

	if (hCurrentButton != NULL)
		Button_SetCheck(hCurrentButton, FALSE);

	Button_SetCheck(hButton, TRUE);
	SetFocus(hButton);
	hCurrentButton = hButton;

	Button_GetText(hButton, buffer, sizeof(buffer));

  //Set the text to a bunch of spaces. ??why
	Button_SetText(hLBGroup, "                        ");
	Button_SetText(hLBGroup, buffer);

	SendMessage(hListbox, WM_SETREDRAW, FALSE, 0L);
	ListBox_ResetContent(hListbox);

	comp = NULL;

	if (hButton == hObjects)
		while (0 != (comp = cfg->NextObjectDesc((LPOBJDESC)comp)))
			{
			newID = ListBox_AddString(hListbox, (LPCSTR)comp->GetName());
			ListBox_SetItemData(hListbox, newID, GetUID(comp).id);
      haveSome = TRUE;
			}
	else if (hButton == hFunctions)
		while (0 != (comp = cfg->NextFunctionDesc((LPAFUNCDESC)comp)))
			{
			newID = ListBox_AddString(hListbox, (LPCSTR)comp->GetName());
			ListBox_SetItemData(hListbox, newID, GetUID(comp).id);
      haveSome = TRUE;
			}
	else if (hButton == hEvents)
		while (0 != (comp = cfg->NextEventDesc((LPEVENTDESC)comp)))
			{
			newID = ListBox_AddString(hListbox, (LPCSTR)comp->GetName());
			ListBox_SetItemData(hListbox, newID, GetUID(comp).id);
      haveSome = TRUE;
			}

  if (haveSome == TRUE)
    {
	  ListBox_SetSel(hListbox, TRUE, 0L);
    EnableWindow(hEdit, TRUE);
    EnableWindow(hDelete, TRUE);
    }
  else
    {
    EnableWindow(hEdit, FALSE);
    EnableWindow(hDelete, FALSE);
    }

  SendMessage(hListbox, WM_SETREDRAW, TRUE, 0L);
	GetClientRect(hListbox, &rect);
	InvalidateRect(hListbox, &rect, TRUE);
	UpdateWindow(hListbox);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetWindowObject
//
// CLASS.......:  MDIAPP
//
/////////////////////////////////////////////////////////////////////////////
MDICHILD *	MDICHILD::GetWindowObject(
	HWND 	windowHandle)
	{
	MDICHILD * temp = windowList;

	while (temp && (temp->hWindowHandle != windowHandle))
		temp = temp->next;

	return temp;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	InitializeMenu
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::InitializeMenu(
	HMENU		hMenu)
	{
	char		buffer[50];

	if (hCurrentButton == hObjects)
		lstrcpy(buffer, "New Object Type\tIns");
	else if (hCurrentButton == hFunctions)
		lstrcpy(buffer, "New Function\tIns");
	else if (hCurrentButton == hEvents)
		lstrcpy(buffer, "New Event\tIns");

	ModifyMenu(hMenu, IDM_NEWCOMP, MF_BYCOMMAND|MF_STRING, IDM_NEWCOMP,
	 buffer);

	EnableMenuItem(hMenu, IDM_NEWCOMP, MF_BYCOMMAND|MF_ENABLED);

	if (hCurrentButton == hObjects)
		lstrcpy(buffer, "Edit Object Type\tEnter");
	else if (hCurrentButton == hFunctions)
		lstrcpy(buffer, "Edit Function\tEnter");
	else if (hCurrentButton == hEvents)
		lstrcpy(buffer, "Edit Event\tEnter");

	ModifyMenu(hMenu, IDM_EDITCOMP, MF_BYCOMMAND|MF_STRING, IDM_EDITCOMP,
	 buffer);

	if (1 == ListBox_GetSelCount(hListbox))
		{
		EnableMenuItem(hMenu, IDM_EDITCOMP, MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND|MF_ENABLED);
//		EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND|MF_ENABLED);
//		EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND|MF_ENABLED);
		}
  else
    {
		EnableMenuItem(hMenu, IDM_EDITCOMP, MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND|MF_GRAYED);
    }

	EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(hMenu, IDM_SAVEAS, MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(hMenu, IDM_REPLACEDLL, MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(hMenu, IDM_REPLACEHELPFILE, MF_BYCOMMAND|MF_ENABLED);
  EnableMenuItem(hMenu, IDM_CASCADE, MF_BYCOMMAND|MF_ENABLED);

	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddComponent
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::AddComponent(
	void)
	{
	SERUID			descRec;
	NEWCOMPDATA compData;
	char				nameBuffer[50];
	LPCFGDESC		cfg;

	cfg = (LPCFGDESC)GetObject(fileObject->GetHeadRec());

	if (hCurrentButton == hObjects)
		{
		wsprintf(nameBuffer, "Object %d", objectNameSeed);
		objectNameSeed++;
		if (NewCompDlgBox(hAppInstance, hWindowHandle, nameBuffer, "Object Type",
				 RT_OBJDESC, &compData))
			{
			descRec = cfg->CreateObjectDesc(compData.compName, compData.compUID);
			AddCompToListbox(descRec);
			fileObject->SetDirtyFlag();
			}
		}
	else if (hCurrentButton == hFunctions)
		{
		wsprintf(nameBuffer, "Function %d", functionNameSeed);
		functionNameSeed++;
		if (NewCompDlgBox(hAppInstance, hWindowHandle, nameBuffer, "Function",
				 RT_FUNCDESC, &compData))
			{
			descRec = cfg->CreateFunctionDesc(compData.compName, compData.compUID);
			AddCompToListbox(descRec);
			fileObject->SetDirtyFlag();
			}
		}
	else if (hCurrentButton == hEvents)
		{
		wsprintf(nameBuffer, "Event %d", eventNameSeed);
		eventNameSeed++;
		if (NewCompDlgBox(hAppInstance, hWindowHandle, nameBuffer, "Event",
				 RT_EVENTDESC, &compData))
			{
			descRec = cfg->CreateEventDesc(compData.compName, compData.compUID);
			AddCompToListbox(descRec);
			fileObject->SetDirtyFlag();
			}
		}

	SetFocus(hListbox);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddComponentToListbox
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::AddCompToListbox(
	SERUID	compUID)
	{
	LPCOMPDESC	obj;
	int					topId;
	int					newId;

	obj = (LPCOMPDESC)GetObject(compUID);

	newId = ListBox_AddString(hListbox, (LPCSTR)obj->GetName());
	ListBox_SetItemData(hListbox, newId, compUID.id);
	ListBox_SetSel(hListbox, FALSE, -1);
	ListBox_SetSel(hListbox, TRUE, newId);

  EnableWindow(hDelete, TRUE);
  EnableWindow(hEdit, TRUE);

	topId = ListBox_GetTopIndex(hListbox);

	if ((newId-topId>4) || (newId<topId))
		{
		if (newId-4 < 1)
			ListBox_SetTopIndex(hListbox, 0);
		else
			ListBox_SetTopIndex(hListbox, newId-4);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteComponent
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::DeleteComponent(
	void)
	{
	long            selectCnt;
	RUNTIMEUID      blockUID;
	LPSERIUSBLOCK   pBlock;
	LPINT           pSelectList;
	SERUID	        compUID;
	LPCFGDESC	      cfg;
	int             i;

	cfg = (LPCFGDESC)GetObject(fileObject->GetHeadRec());

	selectCnt = SendMessage(hListbox, LB_GETSELCOUNT, 0, 0);

	if (selectCnt > 0)
		{
		pBlock = new SERIUSBLOCK(sizeof(int) * selectCnt);
		blockUID = GetUID(pBlock);
		pSelectList = (LPINT)pBlock->GetData();
		ListBox_GetSelItems(hListbox, selectCnt, (DWORD)pSelectList);
		for (i=0; i<selectCnt; i++)
			{
			compUID.id = ListBox_GetItemData(hListbox,pSelectList[i]-i);
			if (hCurrentButton == hObjects)
				cfg->DeleteObjectDesc(compUID);
			else if (hCurrentButton == hFunctions)
				cfg->DeleteFunctionDesc(compUID);
			else if (hCurrentButton == hEvents)
				cfg->DeleteEventDesc(compUID);
			ListBox_DeleteString(hListbox, pSelectList[i]-i);
			}
		fileObject->SetDirtyFlag();

    // If there is nothing to select then disable edit and delete buttons
    if (((LB_ERR == ListBox_SetSel(hListbox, TRUE, pSelectList[0])) &&
         (pSelectList[0] > 0) &&
         (LB_ERR == ListBox_SetSel(hListbox, TRUE, pSelectList[0]-1)))
        || (0 == ListBox_GetCount(hListbox)))
      {
      EnableWindow(hDelete, FALSE);
      EnableWindow(hEdit, FALSE);
      }

		pBlock = (LPSERIUSBLOCK)GetObject(blockUID);
		delete pBlock;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	OpenDescDialog
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:  One and only one item is selected.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void MDICHILD::OpenDescDialog(
	void)
	{
	int 		itemIndex;
	long		result;
	SERUID	compUID;

	if (1 ==
			ListBox_GetSelItems(hListbox, 1, &itemIndex))
		{
		compUID.id = ListBox_GetItemData(hListbox, itemIndex);
		if (hCurrentButton == hObjects)
			result =
			 ObjDescDlgBox(hAppInstance, hPrevInstance, hWindowHandle, compUID);
		else if (hCurrentButton == hFunctions)
			result =
			 FuncDescDlgBox(hAppInstance, hPrevInstance, hWindowHandle, compUID);
		else if (hCurrentButton == hEvents)
			result =
			 EventDescDlgBox(hAppInstance, hPrevInstance, hWindowHandle, compUID);
		}

	if (result)
		{
		ListBox_DeleteString(hListbox, itemIndex);
		AddCompToListbox(compUID);
		fileObject->SetDirtyFlag();
		}

	SetFocus(hListbox);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SaveModifiedConfigs
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  Sends messages to all the project windows informing them to
//									shut down.  If a window responds false to the
//									WM_QUERYENDSESSION message the while loop is terminated
//									  and the procedure returns FALSE.  If all projects
//										are close, TRUE is returned.
//
/////////////////////////////////////////////////////////////////////////////
BOOL MDICHILD::SaveModifiedConfigs()
	{
	BOOL 				result = TRUE;
	LPMDICHILD 	pWindow = windowList;

	while (pWindow)
		{
		if(!SendMessage(pWindow->hWindowHandle,	WM_QUERYENDSESSION, 0, 0L))
			{
			result = FALSE;
			break;
			}
		SendMessage(DEVSYSTEM::GetMdiApp()->GetClientWindow(), WM_MDIDESTROY,
		 (WPARAM) pWindow->hWindowHandle, 0L);
		pWindow = windowList;
		}

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetCategoryName
//
// CLASS.......:  MDICHILD
//
// DESCRIPTION.:  Gets the text in the category name field.
//
/////////////////////////////////////////////////////////////////////////////
TSTRING MDICHILD::GetCategoryName(
  void)
  {
  char   buffer[81];

  Edit_GetText(hCatField, buffer, sizeof(buffer));

  return (TSTRING) buffer;
  }
