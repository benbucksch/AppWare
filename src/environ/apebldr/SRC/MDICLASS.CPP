#include "almsys.h"
#include <windowsx.h>
#include <stdlib.h>
#include <ctl3d.h>

#include "progsys.h"
#include "resconst.h"
#include "subject.h"
#include "project.h"
#include "basewin.h"
#include "sjtwin.h"
#include "prjwin.h"
#include "mdiclass.h"
#include "helpcls.h"
#include "fileobj.h"
#include "version.h"
#include "sereclib.h"
#include "rectypes.h"
#include "objshare.h"
#include "cliphead.h"
#include "printer.h"
#include "compstat.h"
#include "upsf_env.h"
#include "iconedit.h"
#include "navcls.h"
#include "palette.h"
#include "list.h"
#include "srchmgr.h"
#include "flow.h"
#include "msgwin.h"
#include "funcinst.h"
#include "param.h"
#include "mtypelst.h"
#include "intdebug.h"
#include "oiputils.h"
#include "apbutil.h"
#include "utility.h"
#include "fnameobj.h"
#include "utility.h"
#include "a_almutl.h"

#ifdef ADBG_DEBUG
#include "cfglibp.h"
#endif

//the following file should only be included in the demo version
#ifdef DEMO
#include "demo.h"
#endif

#include "exeinfo.h"  // for IDM_RUN
#include "oipexe.h"   // for IDM_RUN

#define PREPCOMPTIME .02
#define COMPILETIME 1.0

#define ABOUT_BUT_HEIGHT 25
#define ABOUT_BUT_WIDTH 50
#define ABOUT_BUT_BORDER 3


////////////////////////////////////////////////////////////////////////////
//	MAJOR ASSUMPTION:	Only ONE instance of the MDI object will be created.
//										this is reasonable since it doesn't make sense to have
//										two Frame windows in the same application.
//										Since this assumption will always hold, the 'this'
//										pointer is stored in a static variable and the
//										WndProc is a static  procedure.  This allows the MDI
//										object to avoid using the list object.
////////////////////////////////////////////////////////////////////////////

typedef BOOL (APIENTRY *CLOSEPROC)(HWND, LONG);

char MDI::className[]="frameClass";
char MDI::dialogBuffer[SUBJECT_NAME_LENGTH];
HACCEL	MDI::hAccel;
HMENU	MDI::hMainMenu;
HMENU	MDI::hMainMenuWindows;
MDI* MDI::instancePtr = NULL;
NAVIGATOR* MDI::navigatorPtr = NULL;
PALETTE* MDI::palettePtr = NULL;
HELPCLS* MDI::pHelpPtr = NULL;
LIST* MDI::projList = NULL;
WNDPROC MDI::oldClientWndProc;
MESSAGEWIN* MDI::msgWinPtr = NULL;
SEARCHMANAGER * MDI::srchMgrPtr = NULL;
long MDI::status = 0L;
int	MDI::baseWindowsItemCount = 0;


// Stuff for run/continue
BOOL      MDI::isPaletteVisible   = FALSE;
BOOL      MDI::isNavigatorVisible = FALSE;
LPEXEFLOW MDI::exeFlow            = 0;
HWND      MDI::runActiveWindow    = NULL;
HWND      MDI::designActiveWindow = NULL;
SERUID		MDI::flowUID            = NULLSERUID;

//status bit
#define MDI_NEEDIDLEPROCESSING	0x00000001L
#define MDI_NEEDCONTEXTPOP      0x00000002L
#define MDI_CLEARFLOWSTOPFLAG		0x00000004L


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  WriteConfigInfo
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
#ifdef ADBG_DEBUG
static void WriteConfigInfo(
  HWND    hWndFrame)
	{
	SFilePath				filePath;
	HANDLE					hFile;
	OPENFILENAME		ofn;

	filePath = "cfgtext.txt";

	InitOpenFileName(&ofn, hWndFrame, "Config Files (*.txt)\0*.txt\0",
										filePath, "Write Config Info", 
										OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
										"txt", TRUE);

	if (GetSaveFileName(&ofn))
		{
		hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL,
											CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		I_ASSERT(hFile != INVALID_HANDLE_VALUE);

		cfgDumpConfigInfo(hFile);
		CloseHandle(hFile);
		}
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MDI
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Constructor which calls all the necessary procedures
//								to create the MDI environment and initialize the global
//								system variables.
//
// PARAMETERS..:  standard parameters which are received by the Windows
//								WinMain procedure.
//
// ASSUMPTIONS.:  Only one MDI object will ever be created in an application.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

MDI::MDI()
	{
	instancePtr = this;
	if (!PROGSYSTEM::hPrevInst)
		Register();
	Initialize();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MDI
//
// CLASS.......:  MDI
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
////////////////////////////////////////////////////////////////////s/////////
MDI::~MDI()
	{
	delete navigatorPtr;
	delete palettePtr;
	delete projList;
	delete pHelpPtr;
	delete srchMgrPtr;

	if(AUtlGetOSVersion() == AUTL_OS_WINNT)
  	{
    Ctl3dUnregister(PROGSYSTEM::hInst);
    }
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MainLoop
//
// CLASS.......:  MDI
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

int MDI::MainLoop()
	{

	MSG msg;

	for(;;)
		{
		if(MDI::status & MDI_NEEDIDLEPROCESSING)
			{
			if(!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
				HandleFreeTime();
				continue;	//back to start of loop
				}
			}
		else
 			GetMessage((LPMSG)&msg,NULL,0,0);			

		if(msg.message == WM_QUIT)
			break;
		else
			Dispatch(&msg);
		}
			
	return (msg.wParam);
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			MDI::Dispatch
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void MDI::Dispatch(
	LPMSG msg)
	{
		if(!TranslateMDISysAccel(PROGSYSTEM::hWndClient, msg) &&
			!TranslateAccelerator(PROGSYSTEM::hWndFrame, hAccel, msg))
			{
			TranslateMessage(msg);

			//See if we need to bring up the help screen
			if(msg->message == WM_KEYDOWN)
				{
				switch((int)msg->wParam)
					{
					case VK_F1:
						{
						if(GetKeyState(VK_SHIFT))
							SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, 
								MAKELPARAM(ID_HELPCURSOR, 0), 0L);
						else
							WinHelp(msg->hwnd, MDI::pHelpPtr->GetHelpFileName(), HELP_INDEX, 0L);
						}
						break;

					case VK_F12:
						if(0x8000 & GetKeyState(VK_CONTROL))
							{
							//toggle wheter or not functions are displayed by name
							//or icon
							MDI::ToggleFunctionDisplay();
							}
						else if(0x8000 & GetKeyState(VK_SHIFT))
							{
							PROGSYSTEM::SetPreferences();
							}
						break;
            
          case VK_F11:
          	if(0x8000 & GetKeyState(VK_CONTROL))
            	{
              PROGSYSTEM::ShowBuildInfo();
              }
             break;

#ifdef ADBG_DEBUG
          case 'D':
          case 'd':
            if ((0x8000 & GetKeyState(VK_CONTROL)) &&
                (0x8000 & GetKeyState(VK_SHIFT)))
              {
              WriteConfigInfo(PROGSYSTEM::hWndFrame);
              }
            break;
#endif

					default:
						break;

					}
				}
			DispatchMessage(msg);
			}
	
	} 

																			 

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Register
//
// CLASS.......:  MDI
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

BOOL MDI::Register()
 {
	WNDCLASS    wc;

	wc.style = 				CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc = 	MDI::FrameWndProc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = PROGSYSTEM::hInst;
	wc.hIcon = PROGSYSTEM::GetVABIcon();
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE+1);
	wc.lpszMenuName =  NULL;

	wc.lpszClassName = MDI::className;

	return RegisterClass((LPWNDCLASS)&wc);

 }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Creates the Frame Window and Initializes all appropriate
//								instance variables.
//
// PARAMETERS..:  int- value passed in through WinMain which should be
//								passed to the Windows ShowWindow function for the
//								first time.
//
// RETURN VALUE:  non-zero if the Frame Window was created else zero.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL MDI::Initialize()
  {
	char buffer[128];

	if(AUtlGetOSVersion() == AUTL_OS_WINNT)
  	{
    Ctl3dRegister(PROGSYSTEM::hInst);
    Ctl3dAutoSubclass(PROGSYSTEM::hInst);
    }
	
	hAccel = LoadAccelerators(PROGSYSTEM::hInst, (LPSTR)"AccelTable");
	hMainMenu = LoadMenu(PROGSYSTEM::hInst, (LPSTR)"MainMenu");
	hMainMenuWindows = GetSubMenu(hMainMenu, MAIN_MENU_POS);
	
	MDI::baseWindowsItemCount = GetMenuItemCount(hMainMenuWindows);
	I_ASSERT(baseWindowsItemCount > 0);

  PROGSYSTEM::GetFrameWndName(buffer, sizeof(buffer));

	PROGSYSTEM::HideSplashScreen();

	PROGSYSTEM::hWndFrame = CreateWindow(
		(LPSTR)className,
		(LPSTR)buffer,
		(DWORD)(WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN),
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		hMainMenu,
		PROGSYSTEM::hInst,
		NULL);

	oldClientWndProc = (WNDPROC)SetWindowLong(PROGSYSTEM::hWndClient,
		GWL_WNDPROC, (DWORD)MDI::ClientWndProc);

	if (!PROGSYSTEM::hWndFrame)
		return (FALSE);

	PROGSYSTEM::SetActiveAppFlag();
	navigatorPtr = (NAVIGATOR *)new NAVIGATOR(
		PROGSYSTEM::hWndFrame,	PROGSYSTEM::hInst, PROGSYSTEM::hPrevInst);
	palettePtr = (PALETTE *)new PALETTE(PROGSYSTEM::hWndFrame);
	
	Show(PROGSYSTEM::CmdShow);
	Update();

	projList = (LIST *)new LIST;
	pHelpPtr = (HELPCLS *)new HELPCLS(PROGSYSTEM::hInst);
	srchMgrPtr = (SEARCHMANAGER *)new SEARCHMANAGER();
	HandleCommandLine();

	if(PROGSYSTEM::TestShowNavigatorPref())
		navigatorPtr->show();

	status|=MDI_NEEDIDLEPROCESSING;
	
	return (TRUE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FrameWndProc
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Windows will AEVT_DIRECT all messages for the Frame Window
//								through FrameWndProc.  Since the function is static this
//								is allowable.
//
// PARAMETERS..:  standard parameters for a WndProc procedure.
//
// RETURN VALUE:  standard return value for a WndProc.
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MDI::FrameWndProc(
	HWND     	hWnd,
	UINT 			message,
	WPARAM    wParam,
	LPARAM    lParam)
	{
	CLIENTCREATESTRUCT  clientcreate;
	HWND                hWndChild;
	SUBJECT        * theSubject;
	PROJECT        * theProject;
	SUBJECTWINDOW  * pSubjWnd;
	HELPCLS        * pHelpPtr;
	HANDLE              fileHandle;
	char	              buffer[32];
	char	              buffer2[64];
	LPFILEOBJECT        fileObject;
	LPCLIPHEAD          clipHead;
	LPPRINTER           printerPtr;
	SFilePath           masterPath;
	DWORD								messageResult;
	HWND								activeEditField;

	static char filters[] =
		{
		"AppWare Projects(*."PROJECT_EXTENSION")""\0*."PROJECT_EXTENSION"\0"
		};

	BOOL               setCutCopyMenu;
	BOOL               setPasteMenu;
	BOOL               setClearMenu;
	BOOL               enableProjectMenu;
	BOOL               enableSubjectMenu;
	BOOL               showSelectMenu;
	int 							 tempIndex;
	long 							 compTime;

	pHelpPtr = MDI::pHelpPtr;

	switch (message)
		{
		case WM_CREATE:
			clientcreate.hWindowMenu = hMainMenuWindows;
			clientcreate.idFirstChild = IDM_FIRSTCHILD;
			PROGSYSTEM::hWndClient = CreateWindow("MDICLIENT", NULL,
				(WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL |
					WS_HSCROLL | MDIS_ALLCHILDSTYLES),
				0,0,0,0, hWnd, (HMENU) 1, PROGSYSTEM::hInst,
				(LPSTR)(LPCLIENTCREATESTRUCT)&clientcreate);
			return 0;

		case WM_COMMAND:
			if (pHelpPtr->TestHelpFlag() && GET_WM_COMMAND_ID(wParam, lParam)
				 != ID_ESCAPE)
				{
				if (!pHelpPtr->GetMenuContextId(GET_WM_COMMAND_ID(wParam, lParam)))
					{
					MessageBox(hWnd, "Help not available for menu item",
					 "AppWare", MB_OK|MB_TASKMODAL);
					return (DefFrameProc(hWnd,PROGSYSTEM::hWndClient,message,
						wParam,lParam));
					}
				pHelpPtr->ClearHelpFlag();
				WinHelp(hWnd, MDI::pHelpPtr->GetHelpFileName(), HELP_CONTEXT,
				 pHelpPtr->GetMenuContextId(GET_WM_COMMAND_ID(wParam, lParam)));
				break;
				}

			switch (GET_WM_COMMAND_ID(wParam, lParam))
				{
				case ID_HELPCURSOR:
					pHelpPtr->SetHelpFlag();
					SetCursor(pHelpPtr->GetHelpCursor());
					return (DefFrameProc(hWnd,PROGSYSTEM::hWndClient,message,
						wParam,lParam));

				case ID_ESCAPE:
					if(pHelpPtr->TestHelpFlag())
          	{
						pHelpPtr->ClearHelpFlag();
						SetCursor((HCURSOR)GetClassLong(hWnd,GCL_HCURSOR));
						}
					else if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_UNDO, NULL,
							NULL);

					return(0);


				case IDM_NEWPROJECT:
					apb_ShowWaitCursor();
					
					PROJECT::MakeProjectName(buffer, PROJECT::GetIndex()+1);
					//make sure the default name doesn't already exist.
					while(DuplicateProjectName(buffer))
						{
						//increment the index and get a new project name and continue
						//the loop until a default name is found that doesn't already
						//exist.
						PROJECT::IncIndex();
						PROJECT::MakeProjectName(buffer, PROJECT::GetIndex()+1);
						}
					fileObject = (LPFILEOBJECT)new FILEOBJECT(buffer,
						MAJOR_VERSION, MINOR_VERSION, DATABASE_VERSION,
						FILETYPE_PROJECT);

					fileObject->SetHeadBlock(GetUID(new RECORDBLOCK(sizeof(PROJECT),
						RT_PROJECT)));

					//the fileobjects head record will be set in the project
					//constructor.
					new(fileObject->GetHeadBlock())
						PROJECT(PRJ_NORMAL_CONSTRUCTOR);

					projList->Add(fileObject);

					PROGSYSTEM::ClearUndoFlag();

					PROJECT::IncIndex();

					apb_RestoreOldCursor();
					return(0);

				case IDM_NEWSUBJECT:
					theProject = PROGSYSTEM::GetCurrentProject();
          if(!theProject)
          	return 0;

#ifdef DEMO
          if(theProject->GetSubjectCount() >= DEMO_MAXSUBJECTS)
          	{
          	demo_SubjectMessage();
            return 0;
            }
#endif
					
					ModalizeVAB(TRUE);
					if (DialogBox(PROGSYSTEM::hInst, "NEWSUBJECT",
						PROGSYSTEM::hWndFrame, (DLGPROC)MDI::NewSubject))
						{
            apb_ShowWaitCursor();
						new(theProject->GetSubjectBlock())
							SUBJECT((LPSTR)MDI::dialogBuffer,
								GetUID(theProject),
								SJT_NORMALCONSTRUCTOR);
                
            apb_RestoreOldCursor();
						}
					ModalizeVAB(FALSE);
					PROGSYSTEM::ClearUndoFlag();

					return(0);


				case IDM_OPEN:
					{
					SFilePath			filePath;
					SFilePath			fileName;
					OPENFILENAME	ofn;

					ModalizeVAB(TRUE);

					InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters,
												filePath, "Open File",
												OFN_READONLY | OFN_PATHMUSTEXIST | OFN_SHOWHELP |
												OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
												PROJECT_EXTENSION, FALSE);

					SetupHook(&ofn, PROJECT::VerifyFile);

					if (GetOpenFileName(&ofn))
						{
						fileHandle = CreateFile(filePath, GENERIC_READ, 0, NULL,
																		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

						I_ASSERT(fileHandle != INVALID_HANDLE_VALUE);

						// check to see if duplicate project exists
						filePath.GetFullName(fileName);
						filePath.RemoveFromEnd();

						if(instancePtr->DuplicateProject(fileName, filePath))
							{
							MessageBeep(1);
							tempIndex = PROJECT::GetIndex();
							PROJECT::MakeProjectName(buffer, tempIndex+1);
							while(DuplicateProjectName(buffer))
								{
								//increment the index and get a new project name and
								//continue the loop until a default name is found that
								//doesn't already exist.
								tempIndex++;
								PROJECT::MakeProjectName(buffer, tempIndex+1);
								}
							wsprintf(buffer2,
								"FILE ALREADY OPEN.\n\n Open as %s ?", buffer);
							if(MessageBox(PROGSYSTEM::hWndFrame, buffer2, "WARNING",
								MB_OKCANCEL | MB_ICONEXCLAMATION | MB_TASKMODAL) == IDOK)
								{
								while(PROJECT::GetIndex() != tempIndex)
									PROJECT::IncIndex();

								PROJECT::MakeProjectName(buffer, PROJECT::GetIndex()+1);
								OpenProject(fileHandle, filePath, buffer, TRUE);
								PROJECT::IncIndex();
								}
							else
								{
								CloseHandle(fileHandle);
								}
							}
						else
							{
							OpenProject(fileHandle, filePath, fileName, FALSE);
							}
						}

					ModalizeVAB(FALSE);
					return(0);
					}

				case IDM_CLOSE:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						if(theProject->GetWindowPtr())
							{
							if((theSubject = theProject->GetActiveSubject())!=NULL)
								{
								SendMessage(theSubject->GetWindowPtr()->GetWindowHandle(),
									WM_CLOSE, NULL, NULL);
								}
							else
								{
								SendMessage(theProject->GetWindowPtr()->GetWindowHandle(),
									WM_CLOSE, NULL, NULL);
								}
							}
						}
					return 0;

				case IDM_TILE:
					SendMessage(PROGSYSTEM::hWndClient, WM_MDITILE, MDITILE_HORIZONTAL, NULL);
					return 0;

				case IDM_CASCADE:
					SendMessage(PROGSYSTEM::hWndClient, WM_MDICASCADE, NULL, NULL);
					return 0;

				case IDM_ARRANGE:
					SendMessage(PROGSYSTEM::hWndClient, WM_MDIICONARRANGE, NULL, NULL);
					return 0;

				case IDM_STEP:
        	theProject = PROGSYSTEM::GetCurrentProject();
          
          if(theProject)
          	{
            if(PROGSYSTEM::TestRunTimeFlag())
            	{
              if(theProject->fileObjPtr->GetFileIDStamp() == PROGSYSTEM::GetRunTimeFileID())
              	{
                if(PROGSYSTEM::TestAtStopFlag())
                	{
                  //We're already at a debug stop
                  PROGSYSTEM::SetSingleStepFlag();
                  SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_CONTINUERUN, 0), NULL); 
                  }
                else
                	{
                  I_ASSERT(FALSE);
                  }
                }
              }
            else //we're not running
            	{
              PROGSYSTEM::SetSingleStepFlag();
              SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_RUN, 0), NULL);                                    
              }            
            }
          else
          	{
            I_ASSERT(FALSE);
            }
            
          break;
          

				case IDM_RUN:
					theProject = PROGSYSTEM::GetCurrentProject();
					if (!theProject)
						return 0;

					if(PROGSYSTEM::TestAutoSaveFlag())
						{
						if(!MDI::AutoSave())
							return 0;
						}

					PROGSYSTEM::ClearUndoFlag();
					flowUID            = NULLSERUID;
					runActiveWindow    = NULL;
					designActiveWindow = NULL;

					LoadString(PROGSYSTEM::hInst, STR_COMPILING, buffer2,
						sizeof(buffer2)-1);

					apb_ShowWaitCursor();

					//0 is the initial points the bar should start out with
					//NULL means use default caption
					PROJECT::compStatPtr = new COMPILESTATUS(hWnd, buffer2,
						theProject->GetName(), 1, 0, NULL, COMPSTAT_COMPILE);

					compTime = theProject->GetCompileTime();
					compTime = (long)((double)compTime * (1.0 + PREPCOMPTIME));
					PROJECT::compStatPtr->ChangeSize(compTime);
					PROJECT::compStatPtr->UpdateStatus((long)((double)compTime *
						PREPCOMPTIME));

          // Derive the path to the project we're compiling to pass to the compiler
					masterPath = theProject->GetPath();
					masterPath.AddToEnd(theProject->GetName());
					if (theProject->Compile(compTime, COMPILETIME, masterPath))
						{
						PROJECT::compStatPtr->FillToEnd();
						PROJECT::compStatPtr->ShowFinish();
						delete PROJECT::compStatPtr;
						apb_RestoreOldCursor();
            
						// Flip into runtime mode
						PROGSYSTEM::SetUpRunContext();

						// Let the OIP know if there are any stops set
						if (theProject->FlowStopsSet()
							&& !theProject->TestStopsDisabledFlag())
							PROGSYSTEM::AddDebugInfo();

						PROGSYSTEM::RegisterNewContext();
						designActiveWindow = GetActiveWindow();

						//clear active app flag so title bars can be drawn as inactive
						PROGSYSTEM::ClearActiveAppFlag();

						// Turn the environment off
						instancePtr->LightsOut(TRUE);

						SetupForRunTime();

						//this flag is used when processing the WM_ACTIVATEAPP message
						//Set this flag to true so we can determine wheter or not we
						//were preempted from running the app
						PROGSYSTEM::SetPreEmptedFlag();

						// Run the app
						exeRunApp();

						PROGSYSTEM::ClearSingleStepFlag();
						
						// Turn the environment back on
						instancePtr->LightsOn(TRUE);

						//Theres a chance that the flow was deleted during the debug
						//stop so make sure the UID is valid and of RT_FLOW type.  Even
						//if it's the wrong flow we don't care since the only thing that
						//will happend is the stop flow flag will be cleared.  What we
						//do care about is that the flowUID points to a valid flow
						//pointer so we don't crash.
						if (flowUID != NULLSERUID && CheckUID(flowUID, RT_FLOW))
							{
              //we must clear the stoppedAtFlowFlag first so the
              //area under the stop sign will always be erased
							((FLOW *)GetObject(flowUID))->ClearStoppedFlow();
              
              if(status & MDI_CLEARFLOWSTOPFLAG)
              	{
                ((FLOW *)GetObject(flowUID))->ClearStop(NULL);
                status &= ~MDI_CLEARFLOWSTOPFLAG;
                }
							}

						if(status & MDI_NEEDCONTEXTPOP)
							{
							//context was pushed in IDM_RESETRUN
							PROGSYSTEM::PopAndRegisterContext();
							status &= ~MDI_NEEDCONTEXTPOP;
							}

						if(PROGSYSTEM::TestShutDownFlag())
							PostMessage(hWnd, WM_CLOSE, 0, 0L);
						}
					else
						{
            //couldn't compile
						delete PROJECT::compStatPtr;
						apb_RestoreOldCursor();
						if(!theProject->TestCancelCompileFlag())
							MessageBox(GetActiveWindow(),
							"Could not run application because of errors in the project.",
							"Project Errors", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
						
						PROGSYSTEM::ClearSingleStepFlag();
						}
					PROJECT::compStatPtr = NULL;
					return 0;

				case IDM_CONTINUERUN:
          if (!PROGSYSTEM::TestAtStopFlag())
            return 0;

					theProject = PROGSYSTEM::GetCurrentProject();
					if (theProject != (PROJECT *)GetObject(
						PROGSYSTEM::GetRunContext()->GetHeadRec()))
						return 0;


					//Theres a chance that the flow was deleted during the debug
					//stop so make sure the UID is valid and of RT_FLOW type.  Even
					//if it's the wrong flow we don't care since the only thing that
					//will happend is the stop flow flag will be cleared.  What we
					//do care about is that the flowUID points to a valid flow
					//pointer so we don't crash.
					if (flowUID != NULLSERUID && CheckUID(flowUID, RT_FLOW))
						{
            //we must clear the stoppedAtFlowFlag first so the
            //area under the stop sign will always be erased
						((FLOW *)GetObject(flowUID))->ClearStoppedFlow();
            
            if(status & MDI_CLEARFLOWSTOPFLAG)
            	{
              ((FLOW *)GetObject(flowUID))->ClearStop(NULL);
              status &= ~MDI_CLEARFLOWSTOPFLAG;
              }
						}

					//allow title bars to be drawn as inactive
					PROGSYSTEM::ClearActiveAppFlag();

					PROGSYSTEM::ClearAtStopInfo();
					PROGSYSTEM::AddRunInfo();
					PROGSYSTEM::RegisterNewContext();

					//set to true so we no when processing the WM_ACTIVATEAPP message
					//that we were preempted from running the app
					PROGSYSTEM::SetPreEmptedFlag();

					// Turn the environment off
					instancePtr->LightsOut(FALSE);

					PROGSYSTEM::ClearSingleStepFlag();
					PROGSYSTEM::SetContinueAppFlag();
					return 0;

				case IDM_RESETRUN:
					if (!PROGSYSTEM::TestAtStopFlag())
						return 0;

					if(pTheSystem->GetFileObject() != PROGSYSTEM::GetRunContext())
						{
						PROGSYSTEM::PushAndRegisterContext(PROGSYSTEM::GetRunContext());
						status |= MDI_NEEDCONTEXTPOP;
						}

					theProject = PROGSYSTEM::GetCurrentProject();
					if (theProject != (PROJECT *)GetObject(
						PROGSYSTEM::GetRunContext()->GetHeadRec()))
						return 0;

					apb_ShowWaitCursor();
          
					PROGSYSTEM::ClearAtStopInfo();
					PROGSYSTEM::AddRunInfo();
					PROGSYSTEM::RegisterNewContext();
					exeResetApp();

					PROGSYSTEM::ClearContinueAppFlag();
					
					apb_RestoreOldCursor();
					return 0;

				case IDM_CREATEAPP:
					if(PROGSYSTEM::TestAutoSaveFlag())
						{
						if(!MDI::AutoSave())
							return 0;
						}

					ModalizeVAB(TRUE);
					if (0 != (theProject = PROGSYSTEM::GetCurrentProject()))
						{
						PROGSYSTEM::ClearUndoFlag();
						if (!theProject->SaveAsExecutable())
							{
							if(!theProject->TestCancelCompileFlag())
								MessageBox(GetActiveWindow(),
								"Could not create application because of errors in the project.",
								"Project Errors", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
							}
						}
					ModalizeVAB(FALSE);
					return(0);

				case IDM_CLEARSTOPS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						if((theSubject = theProject->GetActiveSubject())!=NULL)
							theSubject->ClearAllFlowStops();
					return 0;

				case IDM_TOGGLESTOPS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						if((theSubject = theProject->GetActiveSubject())!=NULL)
							{
							theSubject->ToggleFlowStops();
              
              if(PROGSYSTEM::TestRunTimeFlag())
                {
                if(status & MDI_CLEARFLOWSTOPFLAG)
                	{	
                	if(flowUID != NULLSERUID)
                		{
                  	if(((FLOW*)GetObject(flowUID))->TestStopFlag())
                    	{
                      //the user changed the state of the flag so
                      //don't mess with it any more.
                      status &= !MDI_CLEARFLOWSTOPFLAG;
                      }
                  	}
                  else
                  	{
                    I_ASSERT(FALSE);
                    //unexpected error
                    status &= ~MDI_CLEARFLOWSTOPFLAG;
                    }
                  }
                }
              }
					return 0;

				case IDM_PROJCLEARSTOPS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						theProject->ClearAllStops();
					return 0;

				case IDM_ENABLESTOPS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						if(theProject->TestStopsDisabledFlag())
							theProject->EnableAllStops();
						else
							theProject->DisableAllStops();
						}
					return 0;

				case IDM_SUBJERRORS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						if((theSubject = theProject->GetActiveSubject())!=NULL)
							theSubject->CheckForErrors();
					return 0;

				case IDM_SHOWSELECT:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						if((theSubject = theProject->GetActiveSubject())!=NULL)
							theSubject->CenterSelection();
						else
							theProject->CenterSelection();
						}
					return 0;

				case IDM_SELECTALL:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						theProject->SelectAll();
					return 0;


				case IDM_SAVEAS:
				case IDM_SAVE:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						ModalizeVAB(TRUE);
						
						if(wParam == IDM_SAVEAS)
							theProject->SaveAs();
						else
							theProject->Save();

						ModalizeVAB(FALSE);
						}
					return(0);


				case IDM_PALETTE:
					if(IsWindowVisible(instancePtr->palettePtr->GetPaletteHandle()))
						instancePtr->palettePtr->hide();
					else
						instancePtr->palettePtr->show();

					return(0);

				case IDM_NAVIGATOR:
					if(IsWindowVisible(instancePtr->navigatorPtr->GetNavigatorWindow()))
						instancePtr->navigatorPtr->hide();
          else
						instancePtr->navigatorPtr->show();

					return(0);

				case IDM_HELP_INDEX:
					WinHelp(hWnd, MDI::pHelpPtr->GetHelpFileName(), HELP_INDEX, 0L);
					return 0;

				case IDM_USING_HELP:
					WinHelp(hWnd, NULL, HELP_HELPONHELP, 0L);
					return 0;

				case IDM_ABOUT:
					ModalizeVAB(TRUE);
					DialogBox(PROGSYSTEM::hInst,
						"AboutBox",
						hWnd,
						(DLGPROC)MDI::About);
					ModalizeVAB(FALSE);

					return(0);

				case IDM_UNDO:
					//If there is an active edit field, then the context
					//should already be set up appropriately.
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_UNDO, NULL,
							NULL);
					else
						PROGSYSTEM::GetCurrentProject()->DoUndo();

					return(0);

				case IDM_PASTE:
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_PASTE, NULL,
							NULL);
					else
						PROGSYSTEM::GetCurrentProject()->DoPaste();
					return(0);

				case IDM_QUIT:
					return SendMessage(hWnd, WM_CLOSE, NULL, NULL);

				case IDM_CLEAR:
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_CLEAR, NULL,
							NULL);
					else
						PROGSYSTEM::GetCurrentProject()->DoClear();
					return 0;

				case IDM_DODELETE:
					//If there is an active edit field, then the context
					//should already be set up appropriately.
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_CLEAR, NULL,
							NULL);
					else if(PROGSYSTEM::GetCurrentProject() != NULL)
						PROGSYSTEM::GetCurrentProject()->DoDelete();
					return 0;

				case IDM_CUT:
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_CUT, NULL,
							NULL);
					else
						PROGSYSTEM::GetCurrentProject()->DoCut();
					return 0;

				case IDM_COPY:
					if(PROGSYSTEM::GetActiveEditField() != NULL)
						SendMessage(PROGSYSTEM::GetActiveEditField(), WM_COPY, NULL,
							NULL);
					else
						PROGSYSTEM::GetCurrentProject()->DoCopy();
					return 0;

				case IDM_PRINT:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						ModalizeVAB(TRUE);
						theProject->DoPrint();
						ModalizeVAB(FALSE);
						}
					return 0;

				case IDM_PAGESETUP:
					printerPtr = new PRINTER(PROGSYSTEM::hWndFrame);
					ModalizeVAB(TRUE);
					printerPtr->SetUp();
					ModalizeVAB(FALSE);
					delete printerPtr;
					return 0;

				case IDM_PROJERRORS:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						theProject->CheckProjectForErrors();
					return 0;

				case IDM_OBJERRORS:
					//FALSE means don't update the status bar
					theProject = PROGSYSTEM::GetCurrentProject();
					if(theProject != 0)
						{
						theSubject = theProject->GetActiveSubject();

						// check the subject - con't update the status, check only selected objects and
						// disply the  results.
						if(theSubject)
							theSubject->CheckObjects(FALSE, TRUE, TRUE);
						}
          return 0;


				case IDM_IMPORTUPSF:
					ModalizeVAB(TRUE);
					MDI::ImportUPSF();
					ModalizeVAB(FALSE);
					return 0;

				case IDM_EXPORTUPSF:
					if((theProject = PROGSYSTEM::GetCurrentProject())!=NULL)
						{
						ModalizeVAB(TRUE);
						theProject->ExportUPSF();
						ModalizeVAB(FALSE);
						}
					else
						MessageBeep(0);

					return 0;

				case IDM_AUTOSAVE:
					if(PROGSYSTEM::TestAutoSaveFlag())
						PROGSYSTEM::ClearAutoSaveFlag();
					else
						PROGSYSTEM::SetAutoSaveFlag();
					return 0;

				case IDM_CONFIRMAUTOSAVES:
					if(PROGSYSTEM::TestConfirmAutoSaveFlag())
						PROGSYSTEM::ClearConfirmAutoSaveFlag();
					else
						PROGSYSTEM::SetConfirmAutoSaveFlag();
					return 0;

				case IDM_EDITICON:
					theProject = PROGSYSTEM::GetCurrentProject();
					if(theProject)
						{
						ModalizeVAB(TRUE);
						
						if(ICONEDITOR::Dialog(PROGSYSTEM::hWndFrame,
							PROGSYSTEM::hInst, PROGSYSTEM::hPrevInst, 
							theProject->GetIconBlock(), COLOR16))
							{
							theProject->MakeDirty();
							}

						ModalizeVAB(FALSE);
						}
					return 0;

        case IDM_IMPORTSPLASH:
					theProject = PROGSYSTEM::GetCurrentProject();
					if(theProject)
						{
						ModalizeVAB(TRUE);
						
						theProject->ImportSplashScreen();

						ModalizeVAB(FALSE);
						}
					return 0;
        	
				case IDM_FIND:
					ModalizeVAB(TRUE);
					srchMgrPtr->ShowFindDialog();
					ModalizeVAB(FALSE);
					return 0;

				case IDM_FINDNEXT:
					srchMgrPtr->SearchAgain(pTheSystem->GetFileObject());
					return 0;

				default:
					hWndChild = (HWND) SendMessage(PROGSYSTEM::hWndClient,
						WM_MDIGETACTIVE, 0, 0L);
					if(IsWindow(hWndChild))
						SendMessage(hWndChild, WM_COMMAND, wParam, lParam);
					break;

			}
			break;

		case WM_ACTIVATEAPP:
			if(wParam)
				{
				//if we weren't preempted from a running app, bring the frame window
				//to the top and draw windows as active
				if(!PROGSYSTEM::TestPreEmptedFlag())
					{
					PROGSYSTEM::SetActiveAppFlag();
					BringWindowToTop(hWnd);
					if(navigatorPtr)
						SendMessage(navigatorPtr->GetNavigatorWindow(), WM_NCACTIVATE,
							1, NULL);
					if(palettePtr)
						SendMessage(palettePtr->GetPaletteHandle(), WM_NCACTIVATE, 1,
							NULL);

					//Check to see if a subject is active and if so, set the focus
					//to the objlists if necessary
					if(NULL != (theProject = PROGSYSTEM::GetCurrentProject()))
						{
						if((NULL != (theSubject = theProject->GetActiveSubject())) &&
							(theSubject->GetWindowPtr() != NULL))
							{
							SetFocus(theSubject->GetWindowPtr()->GetWindowHandle());
							}
						}
					}
				}
			else
				{
				//if we weren't preempted, draw all title bars as inactive
				if(!PROGSYSTEM::TestPreEmptedFlag())
					{
					PROGSYSTEM::ClearActiveAppFlag();
					MDI::RefreshTitleBars(TRUE);
					}
				}
			break;

		case WM_NCACTIVATE:
			wParam = 0;
			if(PROGSYSTEM::TestActiveAppFlag())
				wParam = 1;
			break;

		case WM_NCLBUTTONDOWN:
			if(pHelpPtr->TestHelpFlag())
				{
				if (!pHelpPtr->GetMenuContextId(GET_WM_COMMAND_ID(wParam, lParam)))
					return (DefFrameProc(hWnd,PROGSYSTEM::hWndClient,message,wParam,lParam));
				pHelpPtr->ClearHelpFlag();
				WinHelp(hWnd, MDI::pHelpPtr->GetHelpFileName(), HELP_CONTEXT,
				 pHelpPtr->GetMenuContextId(GET_WM_COMMAND_ID(wParam, lParam)));
				}
			break;

		case WM_SETCURSOR:
			if (pHelpPtr->TestHelpFlag())
				{
				SetCursor(pHelpPtr->GetHelpCursor());
				return TRUE;
				}
//			else
//				SetCursor((HCURSOR)GetClassWord(hWnd, GCW_HCURSOR));
			break;

		case WM_INITMENU:
			//First go through and set up the menus normally.  Then check if there
			//is an active edit field and if so make the necessary changes to the
			//edit menu.
			setCutCopyMenu = FALSE;
			setClearMenu = FALSE;
			setPasteMenu = FALSE;
			enableProjectMenu = TRUE;
			enableSubjectMenu = FALSE;
			showSelectMenu = FALSE;

			activeEditField = PROGSYSTEM::GetActiveEditField();

			if(((HMENU) wParam) == GetMenu(PROGSYSTEM::hWndFrame))
				{
				theProject = PROGSYSTEM::GetCurrentProject();
				if(PROGSYSTEM::TestRunTimeFlag() && theProject &&
					theProject->fileObjPtr->GetFileIDStamp() == PROGSYSTEM::GetRunTimeFileID())
					{
					EnableMenuItem((HMENU) wParam, IDM_CONTINUERUN, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_RESETRUN, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_EXPORTUPSF, MF_GRAYED|MF_BYCOMMAND);
					}
				else
					{
					EnableMenuItem((HMENU) wParam, IDM_CONTINUERUN, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_RESETRUN, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_EXPORTUPSF, MF_ENABLED|MF_BYCOMMAND);
					}

				if(IsWindowVisible(instancePtr->palettePtr->GetPaletteHandle()))
					ModifyMenu((HMENU) wParam, IDM_PALETTE, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_PALETTE, (LPSTR)"Hide &Palette\tCtrl+L");
				else
					ModifyMenu((HMENU) wParam, IDM_PALETTE, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_PALETTE, (LPSTR)"Show &Palette\tCtrl+L");

				if(IsWindowVisible(instancePtr->navigatorPtr->GetNavigatorWindow()))
					ModifyMenu((HMENU) wParam, IDM_NAVIGATOR, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_NAVIGATOR, (LPSTR)"Hide &Navigator\tCtrl+H");
				else
					ModifyMenu((HMENU) wParam, IDM_NAVIGATOR, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_NAVIGATOR, (LPSTR)"Show &Navigator\tCtrl+H");

        	

				if(PROGSYSTEM::TestConfirmAutoSaveFlag())
					CheckMenuItem((HMENU) wParam, IDM_CONFIRMAUTOSAVES, MF_CHECKED|
						MF_BYCOMMAND);
				else
					CheckMenuItem((HMENU) wParam, IDM_CONFIRMAUTOSAVES, MF_UNCHECKED|
						MF_BYCOMMAND);

				if(PROGSYSTEM::TestAutoSaveFlag())
					{
					CheckMenuItem((HMENU) wParam, IDM_AUTOSAVE, MF_CHECKED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CONFIRMAUTOSAVES, MF_ENABLED|
						MF_BYCOMMAND);
					}
				else
					{
					CheckMenuItem((HMENU) wParam, IDM_AUTOSAVE, MF_UNCHECKED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CONFIRMAUTOSAVES, MF_GRAYED|
						MF_BYCOMMAND);
					}
				EnableMenuItem((HMENU) wParam, IDM_AUTOSAVE, MF_ENABLED|MF_BYCOMMAND);

				if(PROGSYSTEM::TestRunTimeFlag())
					{
					EnableMenuItem((HMENU) wParam, IDM_IMPORTUPSF, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_OPEN, MF_GRAYED|MF_BYCOMMAND);
					}
				else
					{
					EnableMenuItem((HMENU) wParam, IDM_IMPORTUPSF, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_OPEN, MF_ENABLED|MF_BYCOMMAND);
					}

				if(PROGSYSTEM::TestActiveProjectFlag())
					lstrcpy(buffer2, "Select &All Subjects\tCtrl+A");
        else
					lstrcpy(buffer2, "Select &All\tCtrl+A");

				ModifyMenu((HMENU) wParam, IDM_SELECTALL, MF_BYCOMMAND|MF_STRING,
					IDM_SELECTALL, (LPSTR)buffer2);

				if (theProject && (!activeEditField))
					{
          EnableMenuItem((HMENU) wParam, IDM_FIND, MF_ENABLED|MF_BYCOMMAND);

					if(MDI::srchMgrPtr->OkToSearchAgain())
						EnableMenuItem((HMENU) wParam, IDM_FINDNEXT, MF_ENABLED|MF_BYCOMMAND);
					else
						EnableMenuItem((HMENU) wParam, IDM_FINDNEXT, MF_GRAYED|MF_BYCOMMAND);

					EnableMenuItem((HMENU) wParam, IDM_EDITICON, MF_ENABLED | MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_IMPORTSPLASH, MF_ENABLED | MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CLOSE, MF_ENABLED|MF_BYCOMMAND);
					if(!PROGSYSTEM::TestRunTimeFlag())
						{
						EnableMenuItem((HMENU) wParam, IDM_CREATEAPP, MF_ENABLED|MF_BYCOMMAND);
						EnableMenuItem((HMENU) wParam, IDM_RUN, MF_ENABLED|MF_BYCOMMAND);
						}
					else
						{
						EnableMenuItem((HMENU) wParam, IDM_RUN, MF_GRAYED|MF_BYCOMMAND);
						EnableMenuItem((HMENU) wParam, IDM_CREATEAPP, MF_GRAYED|MF_BYCOMMAND);
						}
					
					if(PROGSYSTEM::TestRunTimeFlag() && (theProject->fileObjPtr->GetFileIDStamp() != PROGSYSTEM::GetRunTimeFileID()))
          	EnableMenuItem((HMENU) wParam, IDM_STEP, MF_GRAYED|MF_BYCOMMAND);
          else
						EnableMenuItem((HMENU) wParam, IDM_STEP, MF_ENABLED|MF_BYCOMMAND);
					
					EnableMenuItem((HMENU) wParam, IDM_TILE, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CASCADE, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_ARRANGE, MF_ENABLED|MF_BYCOMMAND);

					//check to see if its a demo and if so, execute the conditional
					//If the conditional is true, then execute the following line.
					//If its not a demo then execute the line after the #else
#ifdef DEMO
					if(theProject->GetSubjectCount() < DEMO_MAXSUBJECTS)
						EnableMenuItem((HMENU) wParam, IDM_NEWSUBJECT, MF_ENABLED|MF_BYCOMMAND);
					else
						EnableMenuItem((HMENU) wParam, IDM_NEWSUBJECT, MF_GRAYED|MF_BYCOMMAND);
#else
					EnableMenuItem((HMENU) wParam, IDM_NEWSUBJECT, MF_ENABLED|MF_BYCOMMAND);
#endif

					EnableMenuItem((HMENU) wParam, IDM_SAVE, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SAVEAS, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PRINT, MF_ENABLED|MF_BYCOMMAND);
					if(theProject->TestStopsDisabledFlag())
						{
						CheckMenuItem((HMENU) wParam, IDM_ENABLESTOPS,
							MF_UNCHECKED|MF_BYCOMMAND);
						}
					else
						{
						CheckMenuItem((HMENU) wParam, IDM_ENABLESTOPS,
							MF_CHECKED|MF_BYCOMMAND);
						}
					if(theProject->FlowStopsSet())
						{
						EnableMenuItem((HMENU) wParam, IDM_PROJCLEARSTOPS, MF_ENABLED|
							MF_BYCOMMAND);
						EnableMenuItem((HMENU) wParam, IDM_ENABLESTOPS, MF_ENABLED|
							MF_BYCOMMAND);
						}
					else
						{
						EnableMenuItem((HMENU) wParam, IDM_PROJCLEARSTOPS, MF_GRAYED|
							MF_BYCOMMAND);
						EnableMenuItem((HMENU) wParam, IDM_ENABLESTOPS, MF_GRAYED|
							MF_BYCOMMAND);
						}
					theSubject = theProject->GetActiveSubject();
					if (theSubject)
						{
						if(theSubject->GetFunctionCount() ||
							theSubject->GetObjectCount() || theSubject->GetCommentCount())
							{
							EnableMenuItem((HMENU) wParam, IDM_SELECTALL,MF_ENABLED|
								MF_BYCOMMAND);
							}
						else
							EnableMenuItem((HMENU) wParam, IDM_SELECTALL,MF_GRAYED|
								MF_BYCOMMAND);
						enableProjectMenu = FALSE;
						enableSubjectMenu = TRUE;
						if(theSubject->ObjectsSelected())
							{
							showSelectMenu = TRUE;
							}
						pSubjWnd = (SUBJECTWINDOW *)theSubject->GetWindowPtr();
						pSubjWnd->GetObjSharePtr()->SetupObjectMenu();
						if(theSubject->ObjectsSelected())
							setCutCopyMenu = setClearMenu = TRUE;
						else if(theSubject->ItemsSelected())
							setClearMenu = TRUE;
						}
					else
						{
						if(theProject->SubjectsSelected())
							{
							setCutCopyMenu = setClearMenu = TRUE;
							showSelectMenu = TRUE;
							}
						if(PROGSYSTEM::TestActiveProjectFlag() &&
							theProject->GetSubjectCount()>0)
							{
							EnableMenuItem((HMENU) wParam, IDM_SELECTALL, MF_ENABLED|
								MF_BYCOMMAND);
							}
						else
							EnableMenuItem((HMENU) wParam, IDM_SELECTALL,MF_GRAYED|
								MF_BYCOMMAND);
						}
					if(PROGSYSTEM::TestPasteFlag())
						{
						PROGSYSTEM::SetClipContext();
						clipHead = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->
							GetHeadRec());
						PROGSYSTEM::PopAndRegisterContext();
						if((theProject && (theSubject == NULL) &&
							clipHead->TestCompleteSubjectFlag()) ||
							(theProject && (theSubject != NULL) &&
							!clipHead->TestCompleteSubjectFlag()))
							{
							setPasteMenu = TRUE;
							}
						}
					}
				else //no current project
					{
					enableProjectMenu = FALSE;
					showSelectMenu = FALSE;
					EnableMenuItem((HMENU) wParam, IDM_EDITICON, MF_GRAYED | MF_BYCOMMAND);
          EnableMenuItem((HMENU) wParam, IDM_IMPORTSPLASH, MF_GRAYED | MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_EXPORTUPSF, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_RUN, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_STEP, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CLOSE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SAVE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SAVEAS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PRINT, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CREATEAPP, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_NEWSUBJECT, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_TILE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CASCADE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_ARRANGE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PROJCLEARSTOPS, MF_GRAYED|
						MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_ENABLESTOPS, MF_GRAYED|
						MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SELECTALL, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_FIND, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_FINDNEXT, MF_GRAYED|MF_BYCOMMAND);
					}

				if(enableProjectMenu)
					{
					EnableMenuItem((HMENU) wParam, IDM_ARRANGESUBJECTS, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PROJERRORS, MF_ENABLED|MF_BYCOMMAND);
					if(theProject->GridIsOn())
						{
						CheckMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_CHECKED|MF_BYCOMMAND);
						}
						else
						{
						CheckMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_UNCHECKED|MF_BYCOMMAND);
						}
					}
				else
					{
					EnableMenuItem((HMENU) wParam, IDM_ARRANGESUBJECTS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_PROJERRORS, MF_GRAYED|MF_BYCOMMAND);
					if((theProject!=NULL) && (theProject->GridIsOn()))
						{
						CheckMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_CHECKED|MF_BYCOMMAND);
						}
						else
						{
						CheckMenuItem((HMENU) wParam, IDM_PROJECTGRID, MF_UNCHECKED|MF_BYCOMMAND);
						}
					}

				if(enableSubjectMenu)
					{
					EnableMenuItem((HMENU) wParam, IDM_FUNCGRID, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_COMMENT, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SMALLICONS, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SUBJERRORS, MF_ENABLED|MF_BYCOMMAND);
				//	EnableMenuItem((HMENU) wParam, IDM_ZOOMFUNCWIN, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SIGNALNAME, MF_ENABLED|MF_BYCOMMAND);
					if (theSubject->TestBigIconFlag())
						CheckMenuItem((HMENU) wParam, IDM_SMALLICONS, MF_UNCHECKED|MF_BYCOMMAND);
					else
						CheckMenuItem((HMENU) wParam, IDM_SMALLICONS, MF_CHECKED|MF_BYCOMMAND);

					if(theSubject->GroupsSelected())
						{
						EnableMenuItem((HMENU) wParam, IDM_SETSMALLICONS, MF_ENABLED|MF_BYCOMMAND);
            EnableMenuItem((HMENU) wParam, IDM_SETLARGEICONS, MF_ENABLED|MF_BYCOMMAND);
						}
					else
						{
						EnableMenuItem((HMENU) wParam, IDM_SETSMALLICONS, MF_GRAYED|MF_BYCOMMAND);
						EnableMenuItem((HMENU) wParam, IDM_SETLARGEICONS, MF_GRAYED|MF_BYCOMMAND);
						}

					if(theSubject->IsGridOn())
						CheckMenuItem((HMENU) wParam, IDM_FUNCGRID, MF_CHECKED|MF_BYCOMMAND);
					else
						CheckMenuItem((HMENU) wParam, IDM_FUNCGRID, MF_UNCHECKED|MF_BYCOMMAND);

					if(theSubject->TestSignalNamesFlag())
						CheckMenuItem((HMENU) wParam, IDM_SIGNALNAME, MF_CHECKED|MF_BYCOMMAND);
					else
						CheckMenuItem((HMENU) wParam, IDM_SIGNALNAME, MF_UNCHECKED|MF_BYCOMMAND);

					if(theSubject->FlowsSelected())
						EnableMenuItem((HMENU) wParam, IDM_TOGGLESTOPS, MF_ENABLED|MF_BYCOMMAND);
					else
						EnableMenuItem((HMENU) wParam, IDM_TOGGLESTOPS, MF_GRAYED|MF_BYCOMMAND);

					if(theSubject->FlowStopsSet())
						EnableMenuItem((HMENU) wParam, IDM_CLEARSTOPS, MF_ENABLED|MF_BYCOMMAND);
					else
						EnableMenuItem((HMENU) wParam, IDM_CLEARSTOPS, MF_GRAYED|MF_BYCOMMAND);
					}
				else
					{
					EnableMenuItem((HMENU) wParam, IDM_SUBJERRORS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_OBJERRORS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_FUNCGRID, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_COMMENT, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SMALLICONS, MF_GRAYED|MF_BYCOMMAND);
				//	EnableMenuItem((HMENU) wParam, IDM_ZOOMFUNCWIN, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SIGNALNAME, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_CLEARSTOPS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_TOGGLESTOPS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_OBJERRORS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SHARE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_UNSHARE, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_MAKEPERSIST, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_MAKENONPERSIST, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_ALIAS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SETSMALLICONS, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_SETLARGEICONS, MF_GRAYED|MF_BYCOMMAND);

					}



				//See if there is an active edit field and if so adjust the
				//edit menu items appropriately.
				if(activeEditField)
					{
					if(IsClipboardFormatAvailable(CF_TEXT))
						setPasteMenu=TRUE;
					else
						setPasteMenu=FALSE;

					messageResult = SendMessage(PROGSYSTEM::GetActiveEditField(),
						EM_GETSEL, 0, 0L);
					if(LOWORD(messageResult) != HIWORD(messageResult))
						{
						setCutCopyMenu = TRUE;
						setClearMenu = TRUE;
						}
					else
						{
						setCutCopyMenu=FALSE;
						setClearMenu=FALSE;
						}

					}



				if(setPasteMenu)
					EnableMenuItem((HMENU) wParam, IDM_PASTE, MF_ENABLED|MF_BYCOMMAND);
				else
					EnableMenuItem((HMENU) wParam, IDM_PASTE, MF_GRAYED|MF_BYCOMMAND);

				if(setCutCopyMenu)
					{
					EnableMenuItem((HMENU) wParam, IDM_CUT, MF_ENABLED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_COPY, MF_ENABLED|MF_BYCOMMAND);
					}
				else
					{
					EnableMenuItem((HMENU) wParam, IDM_CUT, MF_GRAYED|MF_BYCOMMAND);
					EnableMenuItem((HMENU) wParam, IDM_COPY, MF_GRAYED|MF_BYCOMMAND);
					}

				if(setClearMenu)
					EnableMenuItem((HMENU) wParam, IDM_CLEAR, MF_ENABLED|MF_BYCOMMAND);
				else
					EnableMenuItem((HMENU) wParam, IDM_CLEAR, MF_GRAYED|MF_BYCOMMAND);

				if(showSelectMenu)
					EnableMenuItem((HMENU) wParam, IDM_SHOWSELECT, MF_ENABLED|MF_BYCOMMAND);
				else
					EnableMenuItem((HMENU) wParam, IDM_SHOWSELECT, MF_GRAYED|MF_BYCOMMAND);

				PROGSYSTEM::SetDelContext();
				clipHead = (LPCLIPHEAD)GetObject(pTheSystem->GetFileObject()->
					GetHeadRec());
				PROGSYSTEM::PopAndRegisterContext();

				//See if there is an active edit field and if so, set the undo flag
				//appropriated.  If no edit field is active, then continue checking
				//to see if some workshop operation can be undone.
				if(PROGSYSTEM::GetActiveEditField())
					{
					ModifyMenu((HMENU) wParam, IDM_UNDO, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_UNDO, (LPSTR)"&Undo Edit\t Ctrl+Z");
					}
				else if(PROGSYSTEM::TestUndoFlag() && (theProject && theProject->
					fileObjPtr->GetFileIDStamp() == PROGSYSTEM::GetUndoPrjID()) &&
					(( clipHead->TestCompleteSubjectFlag() &&
						PROGSYSTEM::GetUndoSjtUID() == NULLSERUID && !theSubject) ||
					(!clipHead->TestCompleteSubjectFlag() && theSubject &&
					(GetUID(theSubject) == PROGSYSTEM::GetUndoSjtUID()))))
					{
					if(PROGSYSTEM::TestUndoOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Undo\tCtrl+Z");
					else if(PROGSYSTEM::TestPasteOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Paste\tCtrl+Z");
					else if(PROGSYSTEM::TestCutOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Cut\tCtrl+Z");
					else if(PROGSYSTEM::TestClearOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Delete\tCtrl+Z");
					else if(PROGSYSTEM::TestDeleteOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Delete\tCtrl+Z");
					else if(PROGSYSTEM::TestCopyFuncOpFlag())
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo Function Copy\tCtrl+Z");
					else
						lstrcpy((LPSTR)buffer, (LPSTR)"&Undo\tCtrl+Z");
					ModifyMenu((HMENU) wParam, IDM_UNDO, MF_ENABLED|MF_BYCOMMAND|MF_STRING,
						IDM_UNDO, (LPSTR)buffer);
					}
				else
					ModifyMenu((HMENU) wParam, IDM_UNDO, MF_GRAYED|MF_BYCOMMAND|MF_STRING,
						IDM_UNDO, (LPSTR)"&Undo\tCtrl+Z");


				if (pHelpPtr->TestHelpFlag())
					SetCursor(pHelpPtr->GetHelpCursor());

				}
			return 0;

		case WM_ENTERIDLE:
			if ((wParam == MSGF_MENU) && (GetKeyState(VK_F1) & 0x8000))
				{
				pHelpPtr->SetHelpFlag();
				PostMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0L);
				}
			else if(wParam == VK_DELETE)
				{
				SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, IDM_DODELETE, NULL);
				return 0;
				}
			break;

		case WM_QUERYENDSESSION:
		case WM_CLOSE:
			if(!MDI::MaybeCloseAllProjects())
				return (0);
			break;

		case WM_DESTROY:
			SetWindowLong(PROGSYSTEM::hWndClient, GWL_WNDPROC,
				(DWORD)oldClientWndProc);
			WinHelp(hWnd, MDI::pHelpPtr->GetHelpFileName(), HELP_QUIT, 0L);
			PostQuitMessage(0);
			break;

		case WM_SYSCOMMAND:
			if (((wParam & 0xFFF0) == SC_RESTORE) &&
						PROGSYSTEM::TestRunTimeFlag())
				return 0;
			else
				break;

		case WM_SYSCOLORCHANGE:
			if(AUtlGetOSVersion() == AUTL_OS_WINNT)
  			{
        Ctl3dColorChange();
    		}

			if(!MDI::SetSysColors())
				{
				MessageBeep(0);
				MessageBox(NULL, (LPSTR)"There are not enough system resources "
					"to create application resources.  Save all work and close "
					"AppWare.", (LPSTR)"Fatal ApplicationError",
					MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
				}
			return 0;

		case WM_QUERYOPEN:
			if(PROGSYSTEM::TestPreEmptedFlag())
				{
				BringWindowToTop(hWnd);
				return 0;
				}
			break;

		}
	return (DefFrameProc(hWnd,PROGSYSTEM::hWndClient,message,wParam,lParam));
}


//this is where keyboard input can be handled if its directed at the
//frame or client Window
LRESULT CALLBACK MDI::ClientWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	switch(message)
		{
		case WM_KEYDOWN:
      switch(wParam)
        {
        case VK_DELETE:
  				SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, IDM_DODELETE, NULL);
	  			return 0;

				case VK_ESCAPE:
          SendMessage(GetParent(hWnd), WM_COMMAND, ID_ESCAPE, 0L);
          break;
				}
			break;
		}
	return CallWindowProc((FARPROC)oldClientWndProc, hWnd, message, wParam, lParam);
	}

////////////////////////////////////////////////////////////////////////
void MDI::SetupForRunTime()
	{
	PROGSYSTEM::SetRunTimeFlag();
	}

////////////////////////////////////////////////////////////////////////
void MDI::SetupForDesignTime()
	{
	PROJECT * projectPtr;
	SUBJECT * subjectPtr;

	if((projectPtr = PROGSYSTEM::GetCurrentProject())!=NULL)
		{
		if((subjectPtr = projectPtr->GetActiveSubject())!=NULL)
			{
			((SUBJECTWINDOW *)subjectPtr->GetWindowPtr())->RefreshClientArea();
			}
		}
	PROGSYSTEM::ClearRunTimeFlag();

	}


////////////////////////////////////////////////////////////////////////
void MDI::RefreshTitleBars(
	BOOL active)
	{
	char buffer[128];

	SendMessage(PROGSYSTEM::hWndFrame, WM_NCACTIVATE, active, NULL);
	SendMessage(PROGSYSTEM::hWndFrame, WM_NCPAINT, active, NULL);

	PROGSYSTEM::GetFrameWndName(buffer, sizeof(buffer));
	SetWindowText(PROGSYSTEM::hWndFrame, buffer);

	if(palettePtr && IsWindow(palettePtr->GetPaletteHandle()))
		{
		SendMessage(palettePtr->GetPaletteHandle(), WM_NCACTIVATE, active,
			NULL);
		SendMessage(palettePtr->GetPaletteHandle(), WM_NCPAINT, active, NULL);
		GetWindowText(palettePtr->GetPaletteHandle(), buffer,
			sizeof(buffer)-1);
		SetWindowText(palettePtr->GetPaletteHandle(), buffer);
		}
	if(navigatorPtr && IsWindow(navigatorPtr->GetNavigatorWindow()))
		{
		SendMessage(navigatorPtr->GetNavigatorWindow(), WM_NCACTIVATE, active,
			NULL);
		SendMessage(navigatorPtr->GetNavigatorWindow(), WM_NCPAINT, active, NULL);
		GetWindowText(navigatorPtr->GetNavigatorWindow(), buffer,
			sizeof(buffer)-1);
		SetWindowText(navigatorPtr->GetNavigatorWindow(), buffer);
		}
	}

////////////////////////////////////////////////////////////////////////
void MDI::HandleCommandLine()
	{
	HANDLE hFile;
	SFilePath path;
	SFilePath	name;

	if(PROGSYSTEM::lpCmdParam[0] != '\0')
		{
		path = PROGSYSTEM::lpCmdParam;
		path.MakeFromCurrentDirectory();

		if((hFile = CreateFile(path, 
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL)) == INVALID_HANDLE_VALUE)
			{
			if((0xffff & GetLastError()) == 2L)
				MessageBox(NULL, "File not found.", "Command Line Error",
					MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
			else if((0xffff & GetLastError()) == 3L)
				MessageBox(NULL, "Bad path in command line", "Command Line Error",
					MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
			else
				MessageBox(NULL, "Bad command line", "Command Line Error",
					MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
			}
		else //file was opened
			{
			if(!PROJECT::VerifyFile(hFile, path))
				MessageBox(NULL, "File not recognized as a project", "File Error",
						MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
			else //Open the project
				{
				path.GetFullName(name);
				path.RemoveFromEnd();

				OpenProject(hFile, path, name, FALSE);
				}
			}
		}
	}

////////////////////////////////////////////////////////////////////////
void MDI::ImportUPSF()
	{
	static char filters[] = {
														"UPSF Files(*."UPSF_EXTENSION")""\0*."UPSF_EXTENSION"\0"
													};
	SFilePath						filePath;
	SFilePath						fileName;
	SFilePath						logPath;
	long								result;
	LPFILEOBJECT        fileObject;
	LPFILEOBJECT 				holdContext;
	BOOL 								doImport = FALSE;
	OPENFILENAME				ofn;

	//at this point in execution, the current fileobject will be equivalent to
	//the context on the stack which will be replaced by the new FILEOBJECT
	holdContext = pTheSystem->GetFileObject();

	InitOpenFileName(&ofn, PROGSYSTEM::hWndFrame, filters, filePath,
									"Import UPSF File",
									OFN_READONLY | OFN_PATHMUSTEXIST | OFN_SHOWHELP |
									OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
									PROJECT_EXTENSION, FALSE);

	if (GetOpenFileName(&ofn))
		{
		apb_ShowWaitCursor();

		filePath.GetFullName(fileName);

		//get current working directory for the log path
		logPath.SetName(fileName);
		logPath.MakeFromCurrentDirectory();
		logPath.SetExtension(UPSF_LOGEXTENSION);

		//TRUE means to try and open the logfile
		result = upsfInitImport(filePath, TRUE, logPath);

		I_ASSERT(result == AUPSF_NOERROR);
		
		if(result == AUPSF_LOGERROR)
			{
			if(MessageBox(GetActiveWindow(), "Could not open the log file "
			"for logging comments during import.  Continue with import anyway?",
				"Import Notice",
				MB_TASKMODAL|MB_ICONINFORMATION|MB_OKCANCEL) == IDOK)
				{
				doImport = TRUE;
				logPath = "";
				}
			}

		if(result == AUPSF_NOERROR || doImport)
			{
			PROGSYSTEM::ClearImportErrorFlag();
			fileObject = (LPFILEOBJECT)new FILEOBJECT((LPSTR)"",
				MAJOR_VERSION, MINOR_VERSION, DATABASE_VERSION,
				FILETYPE_PROJECT);

			fileObject->SetHeadBlock(GetUID(new RECORDBLOCK(sizeof(PROJECT),
				RT_PROJECT)));

			//the file objects head rec will be set in the project constructor
			new(fileObject->GetHeadBlock())
				PROJECT(PRJ_UPSF_CONSTRUCTOR);

			if(PROGSYSTEM::TestImportErrorFlag())
				{
				delete (PROJECT *)GetObject(fileObject->GetHeadRec());
				delete fileObject;
				if(holdContext) //set context to what it was
					pTheSystem->SetFileObject(holdContext);
				AUpsfLogComment("Import prematurely terminated due to errors"
					" encountered.");
				MessageBox(NULL, "Import terminated due to errors.",
					"Import Error", MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);
				upsfCloseImport();
				}
			else
				{
				SFormattedMsg		msg;

				projList->Add(fileObject);
				upsfCloseImport();

				msg.Format("Import of project %s is complete.",
												(char*) PROGSYSTEM::GetCurrentProject()->GetName());

				if(logPath.Length() > 0)
					{
					msg.AppendFmt("\n\rYou may want to look at the log file %s"
												" for information pertinent to the import's success.",
												(const char*) logPath);
					}
					
				MessageBeep(0);
				MessageBox(NULL, msg, "Finished!", MB_OK|MB_ICONINFORMATION|
					MB_TASKMODAL);
				}
			}
		//if result is AUPSF_LOGERROR a message box has already been brought up.
		else
			{ 
			if(result != AUPSF_LOGERROR)
				{
				MessageBox(NULL, "Error Opening Import File.", "UPSF Error",
					MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);

      	}
        
      //close the import so the file doesn't remain open.
      upsfCloseImport();
			}

		apb_RestoreOldCursor();
		}
	}


////////////////////////////////////////////////////////////////////////
void MDI::OpenProject(
	HANDLE fileHandle,
	LPCSTR path,
	LPCSTR name,
	BOOL needNewName)
	{
	LPFILEOBJECT fileObject;
	PROJECT * projectPtr;
	LPFILEOBJECT holdContext;

	//at this point in execution, the current fileobject will be equivalent to
	//the context on the stack which will be replaced by the new FILEOBJECT
	holdContext = pTheSystem->GetFileObject();
	apb_ShowWaitCursor();
  
	msgWinPtr = new MESSAGEWIN();

	fileObject = (LPFILEOBJECT)new FILEOBJECT(fileHandle,path, name);
	projectPtr = (PROJECT *)GetObject(fileObject->GetHeadRec());

	PROGSYSTEM::SetContext(projectPtr);

	if (projectPtr)
		PROGSYSTEM::SetContext(projectPtr);
	if(projectPtr && !(projectPtr->PostReadInit() & PRJ_READERROR))
		{
		if(needNewName)
			{
			fileObject->SetDirtyFlag();
			fileObject->SetSaveAsFlag();
			}
		CloseHandle(fileHandle);
		projList->Add(fileObject);
		apb_RestoreOldCursor();
		}
	else
		{
		CloseHandle(fileHandle);
		delete projectPtr;
		delete fileObject;
		if(holdContext)
    	{
			pTheSystem->SetFileObject(holdContext); //Set Context to what it was
			PROGSYSTEM::RegisterFileObj(holdContext); //register with oip
			}

		apb_RestoreOldCursor();
		if(projectPtr == NULL)
			MessageBox(NULL, "The project has become corrupt and can not be"
				" opened.",(LPSTR)"Notice:", MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);
		else
			MessageBox(NULL, "Could not open project because\n"
				"of errors.", "Notice:", MB_ICONEXCLAMATION|MB_OK|
				MB_TASKMODAL);
		}
	PROGSYSTEM::ClearUndoFlag();

	if(msgWinPtr->TestMessagesExistFlag())
		msgWinPtr->ShowMessages();

	delete msgWinPtr;
	msgWinPtr = NULL;
	}




/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	About
//
// CLASS.......:  MDI
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

BOOL CALLBACK MDI::About(
	HWND hDlg,
	UINT message,
	WPARAM  wParam,
	LPARAM /*lParam*/)
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

	switch (message) {
	case WM_INITDIALOG:
		//dif.x will be the width of both horizontal borders and
		//dif.y will be the width of both vertical borders.

		I_ASSERT(diSplash == NULL);

		if(diSplash == NULL)
			diSplash = new DIBITMAP((LPSTR)PROGSYSTEM::GetSplashResourceName(), 
				PROGSYSTEM::hInst);
		I_ASSERT(diSplash->Valid());

		if(diSplash->Valid())
			{
			//set the caption text
			LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, (LPSTR)afterAbout, 
				sizeof(buffer) - lstrlen(buffer));

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
		if (GET_WM_COMMAND_ID(wParam, lParam) == IDOK
				|| GET_WM_COMMAND_ID(wParam, lParam) == IDCANCEL) 
			{
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
		if((HWND) wParam == hDlg)
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
// CLASS.......:  MDI
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


BOOL MDI::Show(int ShowCmd)
	{
	return ShowWindow(PROGSYSTEM::hWndFrame, ShowCmd);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Hide
//
// CLASS.......:  MDI
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

BOOL MDI::Hide()
	{
	return ShowWindow(PROGSYSTEM::hWndFrame, SW_HIDE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Update
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Forces the hWndFrame window to be updated
//
// PARAMETERS..:  none
//
// RETURN VALUE:  none
//
// ASSUMPTIONS.:  none
//
// SIDE-EFFECTS:  none
/////////////////////////////////////////////////////////////////////////////

void MDI::Update()
	{
	UpdateWindow(PROGSYSTEM::hWndFrame);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  NewSubject
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  Dialog Box which is brought up to query the user for the
//								new subjects name.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MDI::NewSubject(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM /*lParam*/)
{
	char tempName[20];
	char buffer[10];
	static int tempIndex;
	PROJECT * projPtr;

	switch(message)
	{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			LoadString(PROGSYSTEM::hInst, STR_SUBJECTNAME, (LPSTR)buffer, 10);
			tempIndex = SUBJECT::GetIndex();
			wsprintf((LPSTR)tempName, (LPSTR)"%s%u", (LPSTR)buffer,tempIndex);
			projPtr = PROGSYSTEM::GetCurrentProject();
			if(projPtr)
				{
				//make sure the default name doesn't already exist
				while(projPtr->DuplicateSubjectName((LPSTR)tempName))
					{
					tempIndex++;
					wsprintf((LPSTR)tempName, (LPSTR)"%s%u", (LPSTR)buffer,tempIndex);
					}
				}
			SetDlgItemText(hDlg, ID_NEWNAME, (LPSTR)tempName);
			Edit_LimitText(hDlg, SUBJECT_NAME_LENGTH);
			Edit_SetSel(hDlg, 0, 0);

			return(TRUE);

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam))
			{
				case ID_OK:
					GetDlgItemText(hDlg, ID_NEWNAME,
											 (LPSTR)MDI::dialogBuffer,SUBJECT_NAME_LENGTH);

					if(lstrlen((LPSTR)MDI::dialogBuffer) > 0)
						{
						if(tempIndex != -1)
							{
							//add 1 to increment the index beyond the index which is being
							//used
							SUBJECT::IncrementIndex(tempIndex - SUBJECT::GetIndex() + 1);
							}
						EndDialog(hDlg, TRUE);
						return(TRUE);
						}
					else
						MessageBeep(0);
					
					break;

				case ID_CANCEL:
					EndDialog(hDlg, FALSE);
					return(TRUE);

				case ID_NEWNAME:
					//see if the user entered something in the edit control
					if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
						tempIndex = -1;
					break;


			}
			break;
	}
	return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetNavigatorPtr
//
// CLASS.......:  MDI
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
void MDI::SetNavigatorPtr(
	NAVIGATOR * newNavigator)
	{
	MDI::navigatorPtr = newNavigator;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetNavigatorPtr
//
// CLASS.......:  MDI
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
NAVIGATOR * MDI::GetNavigatorPtr()
	{
	return MDI::navigatorPtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetPalettePtr
//
// CLASS.......:  MDI
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
void MDI::SetPalettePtr(
	PALETTE * newPalette)
	{
	MDI::palettePtr = newPalette;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetPalettePtr
//
// CLASS.......:  MDI
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
PALETTE * MDI::GetPalettePtr()
	{
	return MDI::palettePtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DuplicateProject
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  returns TRUE if a project with the same name and path
//								as the name pointed to by fileName and path already exists.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL MDI::DuplicateProject(
	LPCSTR fileName,
	LPCSTR path)
	{
	LPFILEOBJECT temp = NULL;
	BOOL duplicate = FALSE;

	while(0 != (temp = (LPFILEOBJECT)MDI::projList->GetNextMember(temp))
				 &&
				!duplicate)
		{
		duplicate = !(lstrcmp((LPSTR)fileName, temp->GetName()) ||
			lstrcmp((LPSTR)path, temp->GetFilePath()));
		}
		return duplicate;
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DuplicateProjectName
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  returns TRUE if a project with the same name only as
//								the name pointed to by name already exists.
//								Extensions for names are ignored.
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL MDI::DuplicateProjectName(
	LPSTR name)
	{
	LPFILEOBJECT temp = NULL;
	BOOL duplicate = FALSE;
	SFilePath fname1;
	SFilePath fname2;
	SFilePath name1;
	SFilePath name2;

	fname1 = name;
	fname1.GetName(name1);

	while(0 != (temp = (LPFILEOBJECT)MDI::projList->GetNextMember(temp))
				 &&
				!duplicate)
		{
		fname2 = temp->GetName();			// no path is included (but has extension)
		fname2.GetName(name2);

		if (lstrcmpi(name1, name2) == 0)
			{
			duplicate = TRUE;
			break;
			}
		}

	return duplicate;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MaybeCloseAllProjects
//
// CLASS.......:  MDI
//
// DESCRIPTION.:  	if runtime, returns true if we want to close down.
//									If design time, sends messages to all the project 
//									windows informing them to
//									shut down.  If a window responds false to the
//									WM_QUERYENDSESSION message the while loop is terminated
//									  and the procedure returns FALSE.  If all projects
//										are close, TRUE is returned.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL MDI::MaybeCloseAllProjects()
	{
	PROJECT * tempProject;
	LPFILEOBJECT tempFile;
	BOOL result;
	BOOL reset;
	char buffer[32];

	if(PROGSYSTEM::TestRunTimeFlag())			// at runtime
		{
		I_ASSERT(PROGSYSTEM::TestShutDownFlag() == FALSE);

		result = FALSE; //we don't want the WM_CLOSE to generate
										//a WM_DESTROY message yet
		reset = TRUE;	//assume we want to reset the running app

		if(PROGSYSTEM::TestAtStopFlag())
			{
			//we're debugging so see if we really want to quit
			MessageBeep(0);

			LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, (LPSTR)buffer, 
				sizeof(buffer));
			
			if(MessageBox(NULL, "An application is running.  Quit"
				" Application and exit AppWare anyway?", buffer,
				MB_ICONQUESTION|MB_OKCANCEL|MB_TASKMODAL) == IDCANCEL)
				{
				reset = FALSE;
				}
			else
				PROGSYSTEM::SetShutDownFlag();

			}
		
		if(reset)
			{
			if(!PROGSYSTEM::TestAtStopFlag())
				PROGSYSTEM::AddAtStopInfo();
		
			SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, IDM_RESETRUN, 0);
			}
		}
	else
		{
		result = TRUE; //assume we want to close down
		
		while(0 != (tempFile = (LPFILEOBJECT)MDI::projList->GetNextMember(NULL)))
			{
			PROGSYSTEM::PushAndRegisterContext(tempFile);
			tempProject = (PROJECT *)GetObject(tempFile->GetHeadRec());
			PROGSYSTEM::PopAndRegisterContext();
			if(!SendMessage(tempProject->GetWindowPtr()->GetWindowHandle(),
				WM_QUERYENDSESSION, 0, 0L))
				{
				result = FALSE;
				
				if(PROGSYSTEM::TestShutDownFlag())
					PROGSYSTEM::ClearShutDownFlag();

				break;
				}
			else
				{
				SendMessage(PROGSYSTEM::hWndClient, WM_MDIDESTROY,
					(WPARAM) tempProject->GetWindowPtr()->GetWindowHandle(), 0L);
				}
			}
		}
	return result;
	}

////////////////////////////////////////////////////////////////////////
//   MDI::LightsOut
//
// Turn the Workshop windows off.
////////////////////////////////////////////////////////////////////////
void MDI::LightsOut(
	BOOL doHide)
  {
	HMENU		hMenu;

  //Unselect everything in the project per Brian's request
  //so things will be in a stable state when we come back from running.
  PROGSYSTEM::GetCurrentProject()->UnselectEverything();

	// Disable the workshop windows
	
	if(doHide)
		ShowAllButRunningProject(pTheSystem->GetFileObject(), FALSE);

	ShowWindow(PROGSYSTEM::hWndFrame, SW_MINIMIZE);

	// Remove disallowed menu picks
	hMenu = GetSystemMenu(PROGSYSTEM::hWndFrame, FALSE);
	DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND);
	DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	
	//Modify menu items which don't make sense
	ModifyMenu(hMenu, SC_CLOSE,	MF_BYCOMMAND|MF_STRING|MF_ENABLED, SC_CLOSE,
		(LPSTR)"&Return to AppWare");
  
  
  isNavigatorVisible = FALSE;
  if (navigatorPtr)
		{
    HWND h = navigatorPtr->GetNavigatorWindow();
    if (IsWindowVisible(h))
      {
      isNavigatorVisible = TRUE;
      ShowWindow(h, SW_HIDE);
      }
    }

  isPaletteVisible = FALSE;
  if (palettePtr)
    {
    HWND h = palettePtr->GetPaletteHandle();
    if (IsWindowVisible(h))
      {
      isPaletteVisible = TRUE;
      ShowWindow(h, SW_HIDE);
      }
    }

  // Force the title bars to be drawn as inactive
  RefreshTitleBars(FALSE);
  }

////////////////////////////////////////////////////////////////////////
//   MDI::LightsOn
//
// Turn the Workshop windows on.  Called from IDM_RUN and
// IDM_CONTINUERUN cases of the wndProc after we've gone to the oip
// to run the app.
////////////////////////////////////////////////////////////////////////
void MDI::LightsOn(
	BOOL doShow)
  {

	// Clear the preemption flag
	PROGSYSTEM::ClearPreEmptedFlag();

  // Go back to design time
	PROGSYSTEM::ClearRunInfo();
	PROGSYSTEM::RegisterNewContext();

	//SetupForDesingTime must be called first to the runtimeflag will be
	//be cleared so when the active windows cause the context to be register,
	//the context won't register itself with oip as being a runtime context.
	SetupForDesignTime();

	if(doShow)
		ShowAllButRunningProject(pTheSystem->GetFileObject(), TRUE);

	ShowWorkshopWindows();

	if (designActiveWindow)
		{
		SetActiveWindow(designActiveWindow);
		}

	PROGSYSTEM::GetCurrentProject()->CleanCompile();

  return;
	}


////////////////////////////////////////////////////////////////////////
//   MDI::ShowAllButRunningProject
////////////////////////////////////////////////////////////////////////
void MDI::ShowAllButRunningProject(
	LPFILEOBJECT runningFileObj,
	BOOL show)
	{
	HWND currentChild;
	LPBASEWINDOW basePtr;
	int showOp = (show)?SW_SHOW:SW_HIDE;
	int orderIndex = 0;
	char itemName[128];
	int location;
	
	//get first child of the client
	currentChild = GetWindow(PROGSYSTEM::hWndClient, GW_CHILD);

	//iterate through all children
	while(currentChild != NULL)
		{
		basePtr = (LPBASEWINDOW)BASEWINDOW::list->GetObject(currentChild);

		//make sure we're a subject of project
		if(basePtr)
			{
			if(basePtr->fileObjPtr->GetFileIDStamp() != runningFileObj->GetFileIDStamp())
				{
				if(!show)
					{
					//if we're hiding the window, also grey out the menu option
					PROGSYSTEM::PushAndRegisterContext(basePtr->fileObjPtr);
					
					GetWindowText(basePtr->GetWindowHandle(), itemName,
						sizeof(itemName)-1);

					location = MDI::GetMenuItemLocation(hMainMenuWindows, itemName,
						MDI::baseWindowsItemCount);
					
					
					//if the location is less than zero, its possibly because we could not
					//get to the window because it was in the listbox of additional windows
					//which the menu brought up.
					if(location >= 0)
						EnableMenuItem(hMainMenuWindows, location, 
							MF_GRAYED|MF_BYPOSITION);

					PROGSYSTEM::PopAndRegisterContext();
					}
				ShowWindow(basePtr->GetWindowHandle(), showOp);
				}
			}

		if(show)
			EnableMenuItem(hMainMenuWindows,
				MDI::baseWindowsItemCount + orderIndex, MF_ENABLED|MF_BYPOSITION);

		orderIndex++;
		currentChild = GetWindow(currentChild, GW_HWNDNEXT);
		}

	//make sure the context is set appropiately	for running
	pTheSystem->SetFileObject(runningFileObj);
	PROGSYSTEM::RegisterNewContext();
	}

////////////////////////////////////////////////////////////////////////
//   MDI::GetMenuItemLocation
//	 Gets location of enabled menu item
////////////////////////////////////////////////////////////////////////
int MDI::GetMenuItemLocation(
	HMENU menu,
	LPSTR itemName,
	int startLocation)
	{
	int location = -1; //default to no location
	int count = GetMenuItemCount(menu);
	int i;
	char buffer[128];
	LPSTR actualName;

	for(i=startLocation; i < count; i++)
		{
		//skip if it's grayed or if we couldn't get the text
		if((GetMenuState(menu, i, MF_BYPOSITION) != MF_GRAYED) && 
			GetMenuString(menu, i, buffer, sizeof(buffer) -1, MF_BYPOSITION) > 0)
			{
			//set actualName to point past the numbering
			//A space will separte the ordinal from the text
			actualName = buffer;
			while((*actualName != ' ') && (*actualName != '\0'))
				actualName++;

			if(*actualName != '\0')
				{
				actualName++;

				if(lstrcmpi(actualName, itemName) == 0)
					{
					location = i;
					break;
					}
				}
			}
		}
	
	
	return location;
	}

////////////////////////////////////////////////////////////////////////
//   MDI::ShowWorkshopWindows
////////////////////////////////////////////////////////////////////////
void MDI::ShowWorkshopWindows()
  {
	// Allow title bars to be drawn as active
	PROGSYSTEM::SetActiveAppFlag();

  if (isNavigatorVisible)
    ShowWindow(navigatorPtr->GetNavigatorWindow(), SW_SHOWNA);

  if (isPaletteVisible)
    ShowWindow(palettePtr->GetPaletteHandle(), SW_SHOWNA);

//!! Doesn't restore maximized windows.
	ShowWindow(PROGSYSTEM::hWndFrame, SW_RESTORE);
	ShowWindow(PROGSYSTEM::hWndFrame, SW_SHOWNA);

  // Force the title bars to be drawn as active
  RefreshTitleBars(TRUE);

	//forces the system menu to refresh
	GetSystemMenu(PROGSYSTEM::hWndFrame, TRUE);
  }

////////////////////////////////////////////////////////////////////////
//   MDI::EnterDebugger
////////////////////////////////////////////////////////////////////////
BOOL APIENTRY MDI::EnterDebugger(
	LPEXEFLOW exeFlow)
	{
	PROJECT  * theProject = PROGSYSTEM::GetCurrentProject();
	SUBJECT  * subjectPtr;
	MSG           msg;
	FLOW * flowPtr;

	flowPtr = theProject->GetDesignFlow(exeFlow);
	if (flowPtr)
		{
		//We only want to stop in the debuffer if there is a flow to stop
		//on.  The reason a flow wouldn't be here is if it was deleted during
		//a previous debugging stop.

		// Clear the preemption flag
		PROGSYSTEM::ClearPreEmptedFlag();

		PROGSYSTEM::ClearRunInfo();
		PROGSYSTEM::AddAtStopInfo();

		if(!flowPtr->TestStopFlag())
    	{
      //we're single stepping
      status |= MDI_CLEARFLOWSTOPFLAG;
      flowPtr->SetStop(NULL);
      }
		
		//show the windows before registering the context since showing
		//the windows will cause runtime objects to get a deactivate message.
		instancePtr->ShowWorkshopWindows();

		PROGSYSTEM::RegisterNewContext();

		(flowPtr)->FlashFlow();
		flowUID = GetUID(flowPtr);

		// See if the objectlist has any selected objects and if so, give it
		// the focus else give the focus to the functionwindow.
		subjectPtr = theProject->GetActiveSubject();
		if(subjectPtr != NULL)
				SetFocus(subjectPtr->GetWindowPtr()->GetWindowHandle());

		//This message loop should be an exact duplicate of the main message loop.
		while (PROGSYSTEM::TestAtStopFlag() && GetMessage((LPMSG)&msg,NULL,0,0))
			{
			if (!TranslateMDISysAccel(PROGSYSTEM::hWndClient, (LPMSG)&msg)
						&&
					!TranslateAccelerator(PROGSYSTEM::hWndFrame, hAccel,(LPMSG)&msg))
				{
				TranslateMessage((LPMSG)&msg);
				//See if we need to bring up the help screen
				if(msg.message == WM_KEYDOWN)
					{
					switch(msg.wParam)
						{
						case VK_F1:
							{
							if(GetKeyState(VK_SHIFT))
								SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND, ID_HELPCURSOR, 0L);
							else
								WinHelp(msg.hwnd, MDI::pHelpPtr->GetHelpFileName(), HELP_INDEX, 0L);
							}
							break;

						case VK_F12:
							if(0x8000 & GetKeyState(VK_CONTROL))
								{
								//toggle wheter or not functions are displayed by name
								//or icon
								MDI::ToggleFunctionDisplay();
								}
							else if(0x8000 & GetKeyState(VK_SHIFT))
								{
								PROGSYSTEM::SetPreferences();
								}
							break;

	          case VK_F11:
	          	if(0x8000 & GetKeyState(VK_CONTROL))
	            	{
	              PROGSYSTEM::ShowBuildInfo();
	              }
	             break;

						default:
							break;

						}
					}
				DispatchMessage((LPMSG)&msg);
				}
			}
		}
	else
		{
		//we couldn't stop so signal that we want to keep running.
		PROGSYSTEM::SetContinueAppFlag();
		}

	return PROGSYSTEM::TestContinueAppFlag();
	}

///////////////////////////////////////////////////////////////////////////
BOOL ALMAPI MDI::ShowAppBuilder(
	BOOL show)
	{
	if(show)
		{
		MDI::instancePtr->ShowWorkshopWindows();
		}
	else
		{
		ShowWindow(PROGSYSTEM::hWndFrame, SW_HIDE);

		isNavigatorVisible = FALSE;
		if(navigatorPtr && IsWindowVisible(navigatorPtr->GetNavigatorWindow()))
			{
			ShowWindow(navigatorPtr->GetNavigatorWindow(), SW_HIDE);
      isNavigatorVisible = TRUE;
			}

		isPaletteVisible = FALSE;
		if(palettePtr && IsWindowVisible(palettePtr->GetPaletteHandle()))
			{
			ShowWindow(palettePtr->GetPaletteHandle(), SW_HIDE);
      isPaletteVisible = TRUE;
			}
		}

	return TRUE; //signal no error
	}


///////////////////////////////////////////////////////////////////////////
//if modalize is TRUE, we want to disable
BOOL MDI::ModalizeVAB(
	BOOL modalize)
	{
	if(navigatorPtr && IsWindowVisible(navigatorPtr->GetNavigatorWindow()))
		EnableWindow(navigatorPtr->GetNavigatorWindow(), !modalize);

	if(palettePtr && IsWindowVisible(palettePtr->GetPaletteHandle()))
		EnableWindow(palettePtr->GetPaletteHandle(), !modalize);

	return TRUE; //signal no error
	}



////////////////////////////////////////////////////////////////////////
//   MDI::AutoSave
////////////////////////////////////////////////////////////////////////
BOOL MDI::AutoSave()
	{
	PROJECT * tempProject;
	LPFILEOBJECT tempFile;
	PROJECT * currentProject = PROGSYSTEM::GetCurrentProject();
	int result;

	if(currentProject)
		{
		if(currentProject->ProjectIsDirty())
			{
			result = IDYES;
			if(PROGSYSTEM::TestConfirmAutoSaveFlag())
				result = MDI::ConfirmAutoSave(currentProject);
			if(result == IDYES)
				{
				if(!currentProject->Save())
					return FALSE;
				}
			else if(result == IDCANCEL)
				return FALSE;
			}

		tempFile = NULL;
		while(0 != (tempFile = (LPFILEOBJECT)MDI::projList->
			GetNextMember(tempFile)))
			{
			if(tempFile->GetFileIDStamp() != 
				currentProject->fileObjPtr->GetFileIDStamp())
				{
				PROGSYSTEM::PushAndRegisterContext(tempFile);
				tempProject = (PROJECT *)GetObject(tempFile->GetHeadRec());
				result=IDYES;
				if(tempProject->ProjectIsDirty())
					{
					if(PROGSYSTEM::TestConfirmAutoSaveFlag())
						result = ConfirmAutoSave(tempProject);
					if(result == IDYES)
						{
						if(!tempProject->Save())
							{
							PROGSYSTEM::PopAndRegisterContext();
							return FALSE;
							}
						}
					else if(result == IDCANCEL)
						{
						PROGSYSTEM::PopAndRegisterContext();
						return FALSE;
						}
					}
				PROGSYSTEM::PopAndRegisterContext();
				}
			}
		return TRUE;
		}
	else
		return FALSE;
	}

////////////////////////////////////////////////////////////////////////
//    MDI::ConfirmAutoSave
////////////////////////////////////////////////////////////////////////
int MDI::ConfirmAutoSave(
	PROJECT * projectPtr)
	{
	LPSTR buffer;
	int result = IDCANCEL; //default if some memory error occurs

	buffer = (LPSTR)GlobalAlloc(GMEM_ZEROINIT,
		lstrlen(projectPtr->GetName()) + 64);
	if(buffer)
		{
		wsprintf(buffer, (LPSTR)"Save project: %s before compiling?",
			projectPtr->GetName());
		result=MessageBox(NULL, buffer, (LPSTR)"Confirm Save",
			MB_TASKMODAL|MB_YESNOCANCEL|MB_ICONQUESTION);
		GlobalFree((HGLOBAL)buffer);
		}
	return result;
	}

///////////////////////////////////////////////////////////////////////////
//static routine to add a message to the MessageWindow instance
//If addIndex is True then the messages are numbered automatically
BOOL MDI::AddMessage(
	LPSTR messagePtr,
	BOOL addIndex)
	{
	if(msgWinPtr)
		return msgWinPtr->AddMessage(messagePtr, addIndex);
	else
		return FALSE;
	}

///////////////////////////////////////////////////////////////////////////
BOOL MDI::SetSysColors()
	{
	LPFILEOBJECT tempFile;
	PROJECT * projectPtr;

	if(!PROGSYSTEM::SetSysColors())
		return FALSE;

	if(!PROJECT::SetSysColors())
		return FALSE;

	if(!OBJGROUP::SetSysColors())
		return FALSE;
	
	if(!FUNCINST::SetSysColors())
		return FALSE;

	if(!FLOW::SetSysColors())
		return FALSE;

	if(!PARAM::SetSysColors())
		return FALSE;

	if(MDI::projList)
		{
		tempFile = NULL;
		while(0 != (tempFile = (LPFILEOBJECT)MDI::projList->
			GetNextMember(tempFile)))
			{
			projectPtr = (PROJECT *)GetObject(tempFile->GetHeadRec());
			if(projectPtr && !projectPtr->SetPrivateColors())
				return FALSE;
			}
		}

	//No errors if we get to here
	return TRUE;
	}


///////////////////////////////////////////////////////////////////////////
void MDI::ToggleFunctionDisplay()
	{
	PROJECT * tempProject;
	LPFILEOBJECT tempFile = NULL;

	if(PROGSYSTEM::TestFuncNamesFlag())
		PROGSYSTEM::ClearFuncNamesFlag();
	else
		PROGSYSTEM::SetFuncNamesFlag();

	while(0 != (tempFile=(LPFILEOBJECT)MDI::projList->GetNextMember(tempFile)))
		{
		PROGSYSTEM::PushAndRegisterContext(tempFile);
		tempProject = (PROJECT *)GetObject(tempFile->GetHeadRec());
		tempProject->RedrawAllFunctions();
		PROGSYSTEM::PopAndRegisterContext();
		}
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			MDI::HandleFreeTime
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void MDI::HandleFreeTime()
	{
 	if(!pTheSystem->GetMasterTypeList()->LoadNextDLL())
		MDI::status &= ~MDI_NEEDIDLEPROCESSING;

	}
