#include <stdlib.h>
#include "almsys.h"

#include "mdiapp.h"
#include "version.h"
#include "resconst.h"
#include "mdichild.h"
#include "devsys.h"
#include "openfile.h"
#include "cfglib.h"
#include "edcfgdlg.h"
#include "sereclib.h"
#include "rectypes.h"
#include "cfgdesc.h"
#include "recblk.h"

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

#define FRAME_CLASS_NAME	"mdiFrameWindow"

#define ABOUT_BUT_HEIGHT 25
#define ABOUT_BUT_WIDTH 50
#define ABOUT_BUT_BORDER 3

typedef BOOL (FAR PASCAL *CLOSEPROC)(HWND, LONG);


/////////////////////////////////////////////////////////////////////////////
// Initialize MDIAPP class static data members.
/////////////////////////////////////////////////////////////////////////////

MDIAPP far*   MDIAPP::windowList = NULL;
BOOL 					MDIAPP::isRegistered = FALSE;
HBITMAP				MDIAPP::titleScreen = NULL;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MDIAPP
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  Constructor which calls all the necessary procedures
//								to create the MDIAPP environment and initialize the global
//								system variables.
//
// PARAMETERS..:  standard parameters which are received by the Windows
//								WinMain procedure.
//
// ASSUMPTIONS.:  Only one MDIAPP object will ever be created in an
//								application.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

MDIAPP::MDIAPP(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR   /*lpCmdParam*/,
	int			nCmdShow,
	LPSTR		defMenu,
	short		windowMenuPosition,
	LPSTR		defAccelTable,
	LPSTR		frameTitle)
	{

	next = windowList;
	windowList = this;

	hInst = hInstance;
	hPrevInst = hPrevInstance;

	newNameCount = 1;
	activeChild = NULL;

	if (!hPrevInstance)
		{
		if (!isRegistered)
			isRegistered = Register();
		}
	else
		isRegistered = TRUE;

	if (isRegistered == TRUE)
		{
		hAccel = LoadAccelerators(hInst, defAccelTable);
		hMainMenu = LoadMenu(hInst, defMenu);
		hWindowsSubmenu = GetSubMenu(hMainMenu, windowMenuPosition);

		//Hide the splash screen
		DEVSYSTEM::HideSplashScreen();
		hWndFrame = CreateWindow(FRAME_CLASS_NAME, frameTitle,
		 (DWORD)(WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN), 0,	0,
		 GetSystemMetrics(SM_CXSCREEN),
		 GetSystemMetrics(SM_CYSCREEN)*4/5, NULL,
		 hMainMenu, hInst, (LPSTR)this);
		if (hWndFrame)
			{
			Show(nCmdShow);
			Update();
			}
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~MDIAPP
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  destructor which cleans up.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

MDIAPP::~MDIAPP(
	void)
	{
	MDIAPP far* previous = NULL;

	if (windowList == this)
		windowList = this->next;
	else
		{
		previous = windowList;
		while (previous->next && (previous->next != this))
			previous = previous->next;
		previous->next = this->next;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Register
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  registers the window class for the Frame Window
//
// PARAMETERS..:
//
// RETURN VALUE:  non-zero if the class was registered else zero
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL MDIAPP::Register(
	void)
	{
	WNDCLASS    wc;

	wc.style 					= CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc 		= MDIAPP::FrameWndProc;
	wc.cbClsExtra 		= 0;
	wc.cbWndExtra 		= 0;
	wc.hInstance 			= hInst;
	wc.hIcon 					= LoadIcon(hInst, "seriusDeveloperIcon");
	wc.hCursor 				= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH) (COLOR_APPWORKSPACE+1);
	wc.lpszMenuName 	= NULL;
	wc.lpszClassName 	= FRAME_CLASS_NAME;

	if (0 != RegisterClass((LPWNDCLASS)&wc))
		return TRUE;
	else
		return FALSE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FrameWndProc
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
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

LRESULT I_CALLBACK MDIAPP::FrameWndProc(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LPCREATESTRUCT  lpcs;
	MDIAPP far* 		windowObject;
	int 						result;

  windowObject = MDIAPP::GetObject(hwnd);

	if (windowObject != 0)
		result = windowObject->WindowProcedure(hwnd, message, wParam, lParam);
	else if (message == WM_CREATE)
		{
		lpcs = (LPCREATESTRUCT) lParam;
		windowObject = (MDIAPP far*)(lpcs->lpCreateParams);
		windowObject->windowHandle = hwnd;
		result = windowObject->WindowProcedure(hwnd, message, wParam, lParam);
		}
	else
		result = DefWindowProc(hwnd, message, wParam, lParam);

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
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

LRESULT MDIAPP::WindowProcedure(
	HWND   hwnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	BOOL								msgHandled = TRUE;
	CLIENTCREATESTRUCT  clientcreate;
	DLGPROC 						lpProcAbout;
	HWND 								hWndChild;
	HMENU								hMenu;

	switch (message)
		{
		case WM_CREATE:
			clientcreate.hWindowMenu = hWindowsSubmenu;
			clientcreate.idFirstChild = IDM_FIRSTCHILD;
			hWndClient = CreateWindow("MDICLIENT", NULL,
				(WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE),
				0,0,0,0, hwnd, (HMENU) 1, hInst,
				(LPSTR)(LPCLIENTCREATESTRUCT)&clientcreate);
			break;

		case WM_INITMENU:
			hMenu = (HMENU) wParam;
			if (activeChild)
			 activeChild->InitializeMenu(hMenu);
      else
        {
        EnableMenuItem(hMenu, IDM_EDITCOMP, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_SAVEAS, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_REPLACEDLL, MF_BYCOMMAND|MF_GRAYED);
				EnableMenuItem(hMenu, IDM_REPLACEHELPFILE, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_CASCADE, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_EDITCOMP, MF_BYCOMMAND|MF_GRAYED);
		    EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND|MF_GRAYED);
        EnableMenuItem(hMenu, IDM_NEWCOMP, MF_BYCOMMAND|MF_GRAYED);
        }
			break;

		case WM_COMMAND:
			if (wParam >= IDM_FIRSTCHILD)
					msgHandled = FALSE;
			else
				switch(wParam)
					{
					case IDM_NEW:
						CreateNewConfig();
						break;

					case IDM_OPEN:
						OpenConfigFile();
						break;

					case IDM_EDITCONFIG:
						EditConfigFile();
						break;

					case IDM_SAVEAS:
						SaveAs();
						break;

					case IDM_SAVE:
						Save();
						break;

					case IDM_WRITE:
						WriteToTextFile();
						break;

#ifdef AW_I_DEBUG
          case IDM_WRITEBMP:
            cfgWriteBitmaps();
            break;
#endif //#ifdef AW_I_DEBUG

					case IDM_ABOUT:
						lpProcAbout = (DLGPROC) MakeProcInstance((FARPROC)MDIAPP::About, hInst);
						DialogBoxParam(hInst, "AboutBox", hwnd, lpProcAbout, (LPARAM)this);
						FreeProcInstance((FARPROC) lpProcAbout);
						break;

					case IDM_QUIT:
						SendMessage(hwnd, WM_CLOSE, 0, 0L);
						break;

				  case IDM_CASCADE:
					  SendMessage(hWndClient, WM_MDICASCADE, NULL, NULL);
					  break;

					default:
						hWndChild = (HWND) LOWORD(SendMessage(hWndClient,
																					WM_MDIGETACTIVE, 0, 0L));
						if(IsWindow(hWndChild))
							SendMessage(hWndChild, WM_COMMAND, wParam, lParam);
						break;

					}
			break;

		case WM_QUERYENDSESSION:
		case WM_CLOSE:
			if(MDICHILD::SaveModifiedConfigs())
				msgHandled = FALSE;   //do default processing if close not canceled
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	if (msgHandled == FALSE)
		return DefFrameProc(hwnd, hWndClient, message,wParam, lParam);
	else
		return NULL;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SaveAs
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  dialog proc which tells info about this version of Serius
//
// PARAMETERS..:  standard dialog proc parameters
//
// RETURN VALUE:  standard dialog proc return value
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
BOOL MDIAPP::SaveAs(
	void)
	{
	char 					 filePath[256];
	char					 fileName[13] = "config1.cfg";
	int						 hFile;
	BOOL					 result;
	LPFILEOBJECT	 fileObj;
	HWND					 hWndChild;
  LPMDICHILD     pMdiChild;

	fileObj = pTheSystem->GetFileObject();

	hWndChild = (HWND) LOWORD(SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0L));

	if(IsWindow(hWndChild))
		{
		SendMessage(hWndChild, WM_GETTEXT, 13, (long)fileName);
    pMdiChild = MDICHILD::GetWindowObject(hWndChild);
    if (pMdiChild != 0)
      {
      //get function category name
      ((LPCFGDESC)::GetObject(fileObj->GetHeadRec())   )->
       SetFuncGroupName(pMdiChild->GetCategoryName());
      }
    }

	hFile = SaveAsDlgBox(hInst, hWndFrame, "Config Files (*.CFG)\0*.cfg\0", 1,
	 fileObj->GetName(), ".",
	 "Save Config File", OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
	 "cfg", filePath, fileName);

	if(hFile != -1)
		{
		result = fileObj->Write(hFile);
		fileObj->SetName(fileName);
		fileObj->SetFilePath(filePath);
		fileObj->ClearDirtyFlag();
		fileObj->ClearNewFlag();
		hWndChild = (HWND) LOWORD(SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0L));
		if(IsWindow(hWndChild))
			SendMessage(hWndChild, WM_SETTEXT, 0, (long)fileName);
		}
	else
		result = FALSE;

	_lclose(hFile);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Save
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  dialog proc which tells info about this version of Serius
//
// PARAMETERS..:  standard dialog proc parameters
//
// RETURN VALUE:  standard dialog proc return value
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
void MDIAPP::Save(
	void)
	{
	LPFILEOBJECT	 fileObject;
	HWND					 hWndChild;
  LPMDICHILD     pMdiChild;

	fileObject = pTheSystem->GetFileObject();

	if (fileObject->TestNewFlag())
		SaveAs();
	else
    {
	  hWndChild = (HWND) LOWORD(SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0L));

	  if(IsWindow(hWndChild))
      {
      pMdiChild = MDICHILD::GetWindowObject(hWndChild);
			if (pMdiChild != 0)
        {
        //get function category name
        ((LPCFGDESC)::GetObject(fileObject->GetHeadRec()))->
         SetFuncGroupName(pMdiChild->GetCategoryName());
        }
      }
		fileObject->Save();
    }

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	OpenConfigFile
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  dialog proc which tells info about this version of Serius
//
// PARAMETERS..:  standard dialog proc parameters
//
// RETURN VALUE:  standard dialog proc return value
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
void MDIAPP::OpenConfigFile(
	void)
	{
	LPMDICHILD		childWindow;
	LPFILEOBJECT	fileObject;
	char 	        filePath[256];
	char	        fileName[15];
	int		        hFile;
  WORD          dbVersion;

	hFile = OpenFileDlgBox(hInst, hWndFrame, "Config Files\0*.cfg\0", 1, "",
	 ".", "Open Config File",
	 OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, "*.cfg", filePath,
	 fileName, (CHECKPROC)cfgCheckConfigFile);

	if(hFile != -1)
		{
    dbVersion = cfgGetDatabaseVersion(hFile);
    if (dbVersion == DATABASE_VERSION)
      {
			fileObject = new FILEOBJECT(hFile, filePath, fileName);
			if (fileObject->TestOkFlag())
				{
				cfgAddConfig(fileObject);
				childWindow = (LPMDICHILD)new MDICHILD(hInst, hPrevInst,
				 hWndFrame, hWndClient, fileObject);
				childWindow->Show(SW_SHOW);
				}
      }
    else
      {
      fileObject = NULL;
			MessageBox(GetActiveWindow(), "Invalid configuration file.",
									"Read Config File", MB_OK);

      }
		_lclose(hFile);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EditConfigFile
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  dialog proc which tells info about this version of Serius
//
// PARAMETERS..:  standard dialog proc parameters
//
// RETURN VALUE:  standard dialog proc return value
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
void MDIAPP::EditConfigFile(
	void)
	{
	LPMDICHILD			childWindow;
	EDITCONFIGDATA	data;

	data.mdiApp = this;

	if (EditConfigDlgBox(hInst, hWndFrame, &data))
		{
    pTheSystem->SetFileObject(data.fileObject);
		childWindow = (LPMDICHILD)new MDICHILD(hInst, hPrevInst,
		 hWndFrame, hWndClient, data.fileObject);
		childWindow->Show(SW_SHOW);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	About
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  dialog proc which tells info about this version of Serius
//
// PARAMETERS..:  standard dialog proc parameters
//
// RETURN VALUE:  standard dialog proc return value
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK MDIAPP::About(
	HWND   hDlg,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	RECT winBounds;
	RECT clientRect;
	RECT newSize;
	POINT dif;
	PAINTSTRUCT ps;
	HDC hdc;
	static LPDIBITMAP diSplash = NULL;
	char buffer[48] = {"About "};
	LPSTR afterAbout = &(buffer[lstrlen(buffer)]);

	BOOL palChanged;

	switch (message) 
		{
		case WM_INITDIALOG:
			//dif.x will be the width of both horizontal borders and
			//dif.y will be the width of both vertical borders.

			I_ASSERT(diSplash == NULL);

			if(diSplash == NULL)
				diSplash = new DIBITMAP((LPSTR)DEVSYSTEM::GetSplashResourceName(), 
					((MDIAPP far*)lParam)->hInst);
			I_ASSERT(diSplash->Valid());

			if(diSplash->Valid())
				{
				//set the caption text
				LoadString(((MDIAPP far*)lParam)->hInst, STR_APPNAME, afterAbout, 
					sizeof(buffer) - lstrlen(buffer) -1);

				SetWindowText(hDlg, buffer);

				GetWindowRect(hDlg, (LPRECT)&winBounds);
				GetClientRect(hDlg, (LPRECT)&clientRect);
	 			dif.x = (winBounds.right - winBounds.left) - (clientRect.right -
					clientRect.left);
				dif.y = (winBounds.bottom - winBounds.top) - (clientRect.bottom -
					clientRect.top);


				newSize.left = (GetSystemMetrics(SM_CXSCREEN) - diSplash->Width()-dif.x)/2;
				newSize.top = (GetSystemMetrics(SM_CYSCREEN) - diSplash->Height()-dif.y)/2;
				newSize.right = newSize.left + diSplash->Width() + dif.x;
				newSize.bottom = newSize.top + diSplash->Height() + dif.y;

				MoveWindow(hDlg, newSize.left, newSize.top,
					newSize.right - newSize.left, newSize.bottom - newSize.top, FALSE);

				GetClientRect(hDlg, (LPRECT)&clientRect);
				
				//Move OK button into a cool position

				MoveWindow(GetDlgItem(hDlg, IDOK),
					(clientRect.right - ABOUT_BUT_WIDTH)/2,
					clientRect.bottom - ABOUT_BUT_HEIGHT - ABOUT_BUT_BORDER,
					ABOUT_BUT_WIDTH, ABOUT_BUT_HEIGHT, FALSE);	


				}
			else
				EndDialog(hDlg, FALSE);

			return (TRUE);

		case WM_COMMAND:
			if (wParam == IDOK
					|| wParam == IDCANCEL) {
			EndDialog(hDlg, TRUE);
			return (TRUE);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hDlg, (LPPAINTSTRUCT)&ps);
			if(diSplash)
				diSplash->Draw(hdc, 0, 0);

			EndPaint(hDlg, (LPPAINTSTRUCT)&ps);
			return 0;

		case WM_PALETTECHANGED:
			if(((HWND) wParam) == hDlg)
				break;
			//else fall though
		case WM_QUERYNEWPALETTE:
			hdc = GetDC(hDlg);
			palChanged = diSplash->DrawIfPaletteChanged(hdc, 0, 0);
			ReleaseDC(hDlg, hdc);
			
			if(palChanged)
				return TRUE;

			break;

		case WM_DESTROY:
			ReleaseCapture();
			if(diSplash)
				{
				delete diSplash;
				diSplash = NULL;
				}
			break;
		}
	
	return (FALSE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Show
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  shows the FrameWindow identifed by the instance handle
//								hWndFrame.
//
// PARAMETERS..:  ShowCmd- type int, standard ShowWindow messaged which
//								identifies how the window is to be shown
//
// RETURN VALUE:  non-zero if the window was previously being shown else zero
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////


BOOL MDIAPP::Show(int ShowCmd)
	{
	return ShowWindow(hWndFrame, ShowCmd);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Hide
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  Hides the window identified by the instance handle
//								hWndFrame
//
// PARAMETERS..:  none
//
// RETURN VALUE:  non-zero if the window was previously being shown else zero
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

BOOL MDIAPP::Hide()
	{
	return ShowWindow(hWndFrame, SW_HIDE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Forces the hWndFrame window to be updated
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

void MDIAPP::Update()
	{

	UpdateWindow(hWndFrame);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetObject
//
// CLASS.......:  MDIAPP
//
/////////////////////////////////////////////////////////////////////////////
MDIAPP far*	MDIAPP::GetObject(
	HWND 	windowHandle)
	{
	MDIAPP far* temp = windowList;

	while (temp && (temp->windowHandle != windowHandle))
		temp = temp->next;

	return temp;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MainLoop
//
// CLASS.......:  MDIAPP
//
// DESCRIPTION.:  Functions as a Windows WinMain procedure in that MainLoop
//								translates and directs all the messages for the application
//
// PARAMETERS..:
//
// RETURN VALUE:  int- whatever windows returns to signal the end of the
//								application.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

int PASCAL MDIAPP::MainLoop(
	void)
	{
	MSG msg;

	while (GetMessage((LPMSG)&msg,NULL,0,0))
		{
		if (!TranslateMDISysAccel(hWndClient, (LPMSG)&msg) &&
				!TranslateAccelerator(hWndFrame, hAccel,(LPMSG)&msg))
			{
			TranslateMessage((LPMSG)&msg);
			DispatchMessage((LPMSG)&msg);
			}
		}

	return msg.wParam;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateNewConfig
//
// CLASS.......:  MDIAPP
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
void MDIAPP::CreateNewConfig(
	void)
	{
	LPMDICHILD		childWindow;
	LPFILEOBJECT	fileObject;
	char					nameBuffer[10] = "config";

	wsprintf(nameBuffer, "config%d", newNameCount);

	newNameCount++;

	fileObject = (LPFILEOBJECT)new FILEOBJECT(nameBuffer,
	 MAJOR_VERSION, MINOR_VERSION, DATABASE_VERSION, FILETYPE_CONFIG);

	fileObject->SetHeadBlock(
	 GetUID(new RECORDBLOCK(sizeof(CFGDESC), RT_CFGDESC)));

	fileObject->SetHeadRec(GetUID(new(fileObject->GetHeadBlock()) CFGDESC()));

	cfgAddConfig(fileObject);

	childWindow = (LPMDICHILD)new MDICHILD(hInst, hPrevInst,
	 hWndFrame, hWndClient, fileObject);

	childWindow->Show(SW_SHOW);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetActiveChild
//
// CLASS.......:  MDIAPP
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
void MDIAPP::SetActiveChild(
	LPMDICHILD child)
	{

	activeChild = child;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WriteToTextFile
//
// CLASS.......:  MDIAPP
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
BOOL MDIAPP::WriteToTextFile(
	void)
	{
	char 					 filePath[256];
	char					 fileName[13] = "cfgtext.txt";
	int						 hFile;
	BOOL					 result;


	hFile = SaveAsDlgBox(hInst, hWndFrame, "Config Files (*.txt)\0*.txt\0", 1,
	 "cfgtext.txt", ".",
	 "Write Config Info", OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
	 "txt", filePath, fileName);

	if(hFile != -1)
		{
		cfgWrite(hFile);
		}
	else
		result = FALSE;

	_lclose(hFile);

	return result;

	}

