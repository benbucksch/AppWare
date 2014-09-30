#ifndef __MDICLASS_H
#define __MDICLASS_H

#include "classdef.h"
#include "memclass.h"

#define MAIN_MENU_POS 5

//////////////////////////////////////////////////////////////////////////

class MDI:public MEMORY
{

	private:
		static long							status;
		static MDI*			 				instancePtr;
		static PALETTE*			 		palettePtr;
		static NAVIGATOR*			 	navigatorPtr;
		static LIST*		        projList;
		static WNDPROC					oldClientWndProc;
		static SEARCHMANAGER*		srchMgrPtr;

		// These variables for the run/continue portion of the wndProc
		static BOOL         isPaletteVisible;
		static BOOL         isNavigatorVisible;
		static LPEXEFLOW    exeFlow;
		static HWND         runActiveWindow;
		static HWND         designActiveWindow;
		static SERUID flowUID;
		static MESSAGEWIN*  msgWinPtr;
		static int baseWindowsItemCount;

		void LightsOut(
			BOOL doHide);

		void LightsOn (
			BOOL doShow);
		
		void ShowWorkshopWindows();

		BOOL Show(int ShowCmd);

		BOOL Hide();

		void Update();

		BOOL Register();

		BOOL Initialize();

		static void OpenProject(
			HANDLE fileHandle,
			LPCSTR path,
			LPCSTR name,
			BOOL needNewName);

			void HandleFreeTime();

			static void MDI::ShowAllButRunningProject(
				LPFILEOBJECT runningFileObj,
				BOOL show);

			static int MDI::GetMenuItemLocation(
				HMENU menu,
				LPSTR itemName,
				int startLocation);


	public:
		static char 						className[16];
		static char 						dialogBuffer[];
		static HACCEL						hAccel;
		static HMENU						hMainMenu;
		static HMENU						hMainMenuWindows;
		static HELPCLS*					pHelpPtr;

		static LRESULT CALLBACK FrameWndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK ClientWndProc(
			HWND hwnd,
			UINT message,
			WPARAM sParam,
			LPARAM lParam);

		MDI();

		~MDI();

		int MainLoop();

		static BOOL CALLBACK About(
			HWND hDlg,
			UINT message,
			WPARAM  wParam,
			LPARAM lParam);

		static BOOL CALLBACK NewSubject(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);


		static void SetupForRunTime();

		static void SetupForDesignTime();

		static void SetNavigatorPtr(
			NAVIGATOR * newNavigator);

		static NAVIGATOR * GetNavigatorPtr();

		static void SetPalettePtr(
			PALETTE * newPalette);

		static PALETTE * GetPalettePtr();

		static LIST * GetProjectList()
			{
			return projList;
			}

		BOOL DuplicateProject(
			LPCSTR fileName,
			LPCSTR path);

		static BOOL DuplicateProjectName(
			LPSTR name);

		void HandleCommandLine();

		static BOOL MaybeCloseAllProjects();

		static void RefreshTitleBars(
			BOOL active);

		static void ImportUPSF();

		static BOOL AutoSave();

		static int ConfirmAutoSave(
			PROJECT * projectPtr);

    static BOOL APIENTRY EnterDebugger(
			LPEXEFLOW exeFlow);

		static BOOL AddMessage(
			LPSTR messagePtr,
			BOOL addIndex);

		static BOOL SetSysColors();

		static void ToggleFunctionDisplay();

		static LPMDI GetInstancePtr()
			{
			return instancePtr;
			}

    static  BOOL ALMAPI ShowAppBuilder(
			BOOL hide);

		static LPSEARCHMANAGER GetSearchPtr()
			{
			return MDI::srchMgrPtr;
			}

		void Dispatch(
			LPMSG msg);

		static BOOL ModalizeVAB(
			BOOL modalize);

	};

#endif
