#ifndef __PROGSYS_H
#define __PROGSYS_H


#include "classdef.h"
#include "system.h"
#include "exeinfo.h"
#include "fileobj.h"
#include "dibitmap.h"

#define PROJECT_EXTENSION "PJT"
#define PRJSYS_MAXGRIDSIZE 1000
#define PROGSYS_BACKUPEXTENSION "BAK"

#define UPSF_EXTENSION "UPS"
#define UPSF_LOGEXTENSION "LOG"
#define	UPSF_WORKSHOP_VERSION 0
#define UPSF_APPWARE_VERSION 100
#define UPSF_IMPORTPREFIXSTRING ((LPSTR)"*****")
#define UPSF_GOODSTRINGRESULT(x) ((x == AUPSF_NOERROR) || (x == AUPSF_SIZEERROR))
#define UPSF_BADSTRINGRESULT(x)  (!UPSF_GOODSTRINGRESULT(x))


#define LASTOP_DELETE 			0x00000001L
#define LASTOP_PASTE  			0x00000002L
#define LASTOP_CUT					0x00000004L
#define LASTOP_CLEAR    		0x00000008L
#define LASTOP_COPY     		0x00000010L
#define LASTOP_UNDO     		0x00000020L
#define LASTOP_UNDODELETE 	0x00000040L
#define LASTOP_COPYFUNC   	0x00000080L
#define LASTOP_UNDOCOPYFUNC 0x00000100L

#define STATE_GRAYED      	0x00000200L
#define STATE_UNDO			  	0x00000400L
#define STATE_PASTE       	0x00000800L

#define STATE_RUNTIME     	0x00002000L
#define STATE_ACTIVEAPP   	0x00004000L
#define STATE_PREEMPTED     0x00008000L

#define STATE_NOALIASWARNINGS 0x00010000L
#define STATE_IMPORTERROR			0x00020000L
//#define STATE_EXPORTERROR			0x00040000L

#define ACTIVE_PROJECT			0x00100000L
#define ACTIVE_FUNCWIN			0x00200000L
#define ACTIVE_OBJLIST			0x00400000L

#define STATE_AUTOSAVE						0x01000000L
#define STATE_CONFIRMAUTOSAVE			0x02000000L
#define STATE_FUNCNAMES						0x04000000L
#define STATE_SHUTDOWN						0x08000000L

#define STATE_CREATEAPP						0x10000000L
#define STATE_SINGLESTEP					0x20000000L

#define PREFERENCE_AUTOSAVE				0x00000001L
#define PREFERENCE_CONFIRMSAVE		0x00000002L
#define PREFERENCE_SHOWNAV				0x00000004L
#define PREFERENCE_SJTGRID				0x00000008L
#define PREFERENCE_PRJGRID				0x00000010L
#define PREFERENCE_FUNCNAMES			0x00000020L
#define	PREFERENCE_SIGNALNAMES		0x00000040L
#define PREFERENCE_LARGEICONS			0x00000080L
#define PREFERENCE_SIGNALVALUES		0x00000100L
#define PREFERENCE_BUFFEREDREDRAW	0x00000200L
#define PREFERENCE_MAKEBACKUPS    0x00000400L

#define PREFERENCE_UNINITPARAMS   0x00010000L


//UPSF export possible return values
#define EXPORT_NOERROR 0
#define EXPORT_GENERALERROR 0x0001
#define EXPORT_OUTOFDISK 0x0002

//USEFULL macros
#define MAKE_TYPEID_READABLE(buffer, typeID) \
	CopyMemory((void *)buffer, (void *)&typeID, sizeof(COMPUID)); \
	buffer[0]^=buffer[3]^=buffer[0]^=buffer[3]; \
	buffer[1]^=buffer[2]^=buffer[1]^=buffer[2]; \
	(buffer[4] = '\0')


class PROGSYSTEM:public SYSTEM
	{
	private:
		static DWORD status;
		static DWORD preferences;

		static LPFILEOBJECT delFileObj;
		static LPFILEOBJECT clipFileObj;
		static FILEIDSTAMP  undoPrjID;
		static SERUID undoSjtUID;
		static EXEINFO     exeInfo;
		static EXEINFOAUX     exeInfoAux;
		static 	HWND		hWndSplash;
		static	BOOL    splashRegistered;
		static HWND			activeEditField;
		static DEBUGPROC  debugProcAddr;
		static OBJCREATEPROC  objCreateProcAddr;
		static OBJDESTROYPROC objDestroyProcAddr;
		static OBJSETNAMEPROC objSetNameProcAddr;
		static WNDPROC oldPrefEditProcAddr;
		static SHOWVABPROC showVABProcAddr;
		static char VABHelpFile[];
		static LPDIBITMAP diSplash;
		static void PurgeFileObj();

		static HICON prjIcon;
		static HICON sjtIcon;
		static HICON vabIcon;

		static void SetPasteFlag()
			{
			status|=STATE_PASTE;
			}

		static void CleanResources();

	public:
		static HINSTANCE  hInst;
		static HWND       hWndFrame;
		static HWND       hWndClient;
		static HINSTANCE  hPrevInst;
		static LPSTR  lpCmdParam;
		static int		CmdShow;
		static COLORREF normalTextColor;
		static COLORREF grayTextColor;
		static COLORREF dragColor;
		static COLORREF highlightColor;
		static COLORREF windowColor;
		static COLORREF frameColor;
		static COLORREF captionColor;
		static COLORREF captionTextColor;
		static COLORREF highlightTextColor;
		static COLORREF inactiveCaptionColor;
		static COLORREF inactiveCaptionTextColor;
		static HBRUSH inactiveCaptionBrush;
		static HBRUSH captionBrush;
		static HBRUSH normalTextBrush;
		static HBRUSH grayTextBrush;
		static HBRUSH dragBrush;
		static HBRUSH highlightBrush;
		static HBRUSH windowBrush;
		static HPEN normalTextPen;
		static HPEN grayTextPen;
		static HPEN dragPen;
		static HPEN dottedDragPen;
		static HPEN highlightPen;
		static HPEN windowPen;
		static HPEN framePen;
		static HPEN focusPen;
		static int  sjtHGrid;
		static int  sjtVGrid;
		static int  prjHGrid;
		static int  prjVGrid;
		static int minMoveDelta;

		static LRESULT CALLBACK SplashWndProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static void ShowSplashScreen();

		static void HideSplashScreen();


		static LPFILEOBJECT GetClipContext()
			{
			return clipFileObj;
			}

		static void RegisterFileObj(
			 LPFILEOBJECT fileObjPtr);

		static PROJECT * GetCurrentProject();

		static void SetContext(
			PROJECT * projPtr);

		static BOOL InitSystem();

		static void InitClipboard();

		static void CleanUpSystem();

		static void SetDelContext();

		static void SetClipContext();

		static LPFILEOBJECT GetDelContext();

		static LPFILEOBJECT PopAndRegisterContext();

		static void PurgeDelFileObj();

		static void PurgeClipFileObj();


		static void PushAndRegisterContext(
			LPFILEOBJECT pushObject);

		static void SetUpRunContext();

		static void SetUpCompileContext();

		static void AddDebugInfo();

		static void AddRunInfo();

		static void AddCompileInfo();

		static void AddAtStopInfo();

		static void ClearRunInfo();

		static void ClearCompileInfo();

		static void ClearAtStopInfo();

    static BOOL TestAtStopFlag();

    static void SetContinueAppFlag();
    static void ClearContinueAppFlag();
    static BOOL TestContinueAppFlag();

		static void RegisterDesignContext();

		static void RegisterNewContext();

		static LPFILEOBJECT GetRunContext();

		static void ClearLastOpFlag()
			{
			status&= ~(LASTOP_PASTE|LASTOP_CLEAR|LASTOP_CUT|
				LASTOP_COPY|LASTOP_DELETE|LASTOP_UNDODELETE | LASTOP_COPYFUNC |
				LASTOP_UNDOCOPYFUNC);
			}

		static void SetPasteOpFlag()
			{
			ClearLastOpFlag();
			status|= LASTOP_PASTE;
			}

		static BOOL TestPasteOpFlag()
			{
			return (status & LASTOP_PASTE)?TRUE:FALSE;
			}

		static void SetCutOpFlag()
			{
			ClearLastOpFlag();
			SetPasteFlag();
			status|= LASTOP_CUT;
			}

		static BOOL TestCutOpFlag()
			{
			return (status & LASTOP_CUT)?TRUE:FALSE;
			}

		static void SetCopyOpFlag()
			{
			ClearLastOpFlag();
			status|= LASTOP_COPY;
			SetPasteFlag();
			}

		static BOOL TestCopyOpFlag()
			{
			return (status & LASTOP_COPY)?TRUE:FALSE;
			}

		static void SetClearOpFlag()
			{
			ClearLastOpFlag();
			status|=LASTOP_CLEAR;
			}

		static BOOL TestClearOpFlag()
			{
			return (0 != (status & LASTOP_CLEAR));
			}

		static BOOL TestDeleteOpFlag()
			{
			return (0 != (status & LASTOP_DELETE));
			}

		static void SetDeleteOpFlag()
			{
			ClearLastOpFlag();
			status|= LASTOP_DELETE;
			}

		static void SetUndoFlag()
			{
			status|=STATE_UNDO;
			}

		static BOOL TestUndoFlag()
			{
			return (status & STATE_UNDO)?TRUE:FALSE;
			}

		static void ClearUndoFlag()
			{
			status&= ~STATE_UNDO;
			ClearUndoOpFlag();
			ClearUndoIDs();
			}

		static BOOL TestPasteFlag()
			{
			return (status & STATE_PASTE)?TRUE:FALSE;
			}

		static BOOL TestUndoOpFlag()
			{
			return (status & LASTOP_UNDO)?TRUE:FALSE;
			}

		static void SetUndoOpFlag()
			{
			status|=LASTOP_UNDO;
			}

		static void ClearUndoOpFlag()
			{
			status&= ~LASTOP_UNDO;
			}

		static BOOL TestUndoDeleteOpFlag()
			{
			return (0 != (status & LASTOP_UNDODELETE));
			}

		static void SetUndoDeleteOpFlag()
			{
			ClearLastOpFlag();
			status|=LASTOP_UNDODELETE;
			}

		static void SetUndoCopyFuncOpFlag()
			{
			ClearLastOpFlag();
			status|=LASTOP_UNDOCOPYFUNC;
			}

		static BOOL TestUndoCopyFuncFlag()
			{
			return (0 != (status & LASTOP_UNDOCOPYFUNC));
			}

		static void SetCopyFuncOpFlag()
			{
			ClearLastOpFlag();
			status|=LASTOP_COPYFUNC;
			}

		static BOOL TestCopyFuncOpFlag()
			{
			return (0 != (status & LASTOP_COPYFUNC));
			}

		static FILEIDSTAMP GetUndoPrjID()
			{
			return undoPrjID;
			}

		static SERUID GetUndoSjtUID()
			{
			return undoSjtUID;
			}

		static void SetUndoPrjID(
			FILEIDSTAMP id)
			{
			undoPrjID = id;
			}

		static void SetUndoSjtUID(
			SERUID id)
			{
			undoSjtUID =id;
			}

		static void ClearUndoIDs()
			{
			undoPrjID = NULL;
			undoSjtUID = NULLSERUID;
			}

		static void ClearUndoPrjUID()
			{
			undoPrjID = NULL;
			}

		static void ClearUndoSjtUID()
			{
			undoSjtUID = NULLSERUID;
			}

		static BOOL TestRunTimeFlag()
			{
			return (status & STATE_RUNTIME)?TRUE:FALSE;
			}

		static void SetRunTimeFlag()
			{
			status|=STATE_RUNTIME;
			}

		static void ClearRunTimeFlag()
			{
			status&= ~STATE_RUNTIME;
			}

		static BOOL TestActiveAppFlag()
			{
			return (status & STATE_ACTIVEAPP)?TRUE:FALSE;
			}

		static void SetActiveAppFlag()
			{
			status|=STATE_ACTIVEAPP;
			}

		static void ClearActiveAppFlag()
			{
			status&= ~STATE_ACTIVEAPP;
			}

		static FILEIDSTAMP GetRunTimeFileID()
			{
			return (TestRunTimeFlag())?
				exeInfo.pAuxInfo->fileObj->GetFileIDStamp():NULL;
			}

		static void SetPreEmptedFlag()
			{
			status|=STATE_PREEMPTED;
			}

		static void ClearPreEmptedFlag()
			{
			status&= ~STATE_PREEMPTED;
			}

		static BOOL TestPreEmptedFlag()
			{
			return (status & STATE_PREEMPTED)?TRUE:FALSE;
			}

		static BOOL TestNoAliasWarningsFlag()
			{
			return (status & STATE_NOALIASWARNINGS)?TRUE:FALSE;
			}

		static void SetNoAliasWarningsFlag()
			{
			status|=STATE_NOALIASWARNINGS;
			}

		static void ClearNoAliasWarningsFlag()
			{
			status&= ~STATE_NOALIASWARNINGS;
			}

		static void ClearActiveProjectFlag()
			{
			status&= ~ACTIVE_PROJECT;
			}

		static void SetActiveProjectFlag()
			{
			status|=ACTIVE_PROJECT;
			}

		static BOOL TestActiveProjectFlag()
			{
			return (status & ACTIVE_PROJECT)?TRUE:FALSE;
			}


		static BOOL TestImportErrorFlag()
			{
			return (status & STATE_IMPORTERROR)?TRUE:FALSE;
			}

		static void SetImportErrorFlag()
			{
			status|=STATE_IMPORTERROR;
			}

		static void ClearImportErrorFlag()
			{
			status&= ~STATE_IMPORTERROR;
			}

		static void SetAutoSaveFlag()
			{
			status|=STATE_AUTOSAVE;
			}

		static void ClearAutoSaveFlag()
			{
			status&= ~STATE_AUTOSAVE;
			}

		static BOOL TestAutoSaveFlag()
			{
			return (status & STATE_AUTOSAVE)?TRUE:FALSE;
			}

		static void SetConfirmAutoSaveFlag()
			{
			status|=STATE_CONFIRMAUTOSAVE;
			}

		static void ClearConfirmAutoSaveFlag()
			{
			status&= ~STATE_CONFIRMAUTOSAVE;
			}

		static BOOL TestConfirmAutoSaveFlag()
			{
			return (status & STATE_CONFIRMAUTOSAVE)?TRUE:FALSE;
			}

		static void SetCreateAppFlag()
			{
			status |= STATE_CREATEAPP;
			}

		static void ClearCreateAppFlag()
			{
			status &= ~STATE_CREATEAPP;
			}

		static BOOL TestCreateAppFlag()
			{
			return (status & STATE_CREATEAPP)?TRUE:FALSE;
			}

		static HWND GetActiveEditField()
			{
			return activeEditField;
			}

		static void SetActiveEditField(
			HWND newField)
			{
			activeEditField = newField;
			}

		static void SetFuncNamesFlag()
			{
			status |= STATE_FUNCNAMES;
			}

		static void ClearFuncNamesFlag()
			{
			status &= ~STATE_FUNCNAMES;
			}

		static BOOL TestFuncNamesFlag()
			{
			return (status & STATE_FUNCNAMES)?TRUE:FALSE;
			}

		static BOOL SetSysColors();


		//Preferences Routines

		static void SetAutoSavePref()
			{
			preferences|=PREFERENCE_AUTOSAVE;
			}

		static void ClearAutoSavePref()
			{
			preferences&= ~PREFERENCE_AUTOSAVE;
			}

		static BOOL TestAutoSavePref()
			{
			return (preferences & PREFERENCE_AUTOSAVE)?TRUE:FALSE;
			}

		static void SetConfirmSavePref()
			{
			preferences|=PREFERENCE_CONFIRMSAVE;
			}

		static void ClearConfirmSavePref()
			{
			preferences &= ~PREFERENCE_CONFIRMSAVE;
			}

		static BOOL TestConfirmSavePref()
			{
			return (preferences & PREFERENCE_CONFIRMSAVE)?TRUE:FALSE;
			}

		static void SetShowNavigatorPref()
			{
			preferences |= PREFERENCE_SHOWNAV;
			}

		static void ClearShowNavigatorPref()
			{
			preferences &= ~PREFERENCE_SHOWNAV;
			}

		static BOOL TestShowNavigatorPref()
			{
			return (preferences & PREFERENCE_SHOWNAV)?TRUE:FALSE;
			}

		static void SetSjtGridPref()
			{
			preferences |= PREFERENCE_SJTGRID;
			}

		static void ClearSjtGridPref()
			{
			preferences &= ~PREFERENCE_SJTGRID;
			}

		static BOOL TestSjtGridPref()
			{
			return (preferences & PREFERENCE_SJTGRID)?TRUE:FALSE;
			}

		static void SetPrjGridPref()
			{
			preferences |= PREFERENCE_PRJGRID;
			}

		static void ClearPrjGridPref()
			{
			preferences &= ~PREFERENCE_PRJGRID;
			}

		static BOOL TestPrjGridPref()
			{
			return (preferences & PREFERENCE_PRJGRID)?TRUE:FALSE;
			}

		static void SetFuncNamesPref()
			{
			preferences |= PREFERENCE_FUNCNAMES;
			}

		static void ClearFuncNamesPref()
			{
			preferences &= ~PREFERENCE_FUNCNAMES;
			}

		static BOOL TestFuncNamesPref()
			{
			return (preferences & PREFERENCE_FUNCNAMES)?TRUE:FALSE;
			}

		static void SetSignalNamesPref()
			{
			preferences |= PREFERENCE_SIGNALNAMES;
			}

		static void ClearSignalNamesPref()
			{
			preferences &= ~PREFERENCE_SIGNALNAMES;
			}

		static BOOL TestSignalNamesPref()
			{
			return (preferences & PREFERENCE_SIGNALNAMES)?TRUE:FALSE;
			}

		static void SetLargeIconsPref()
			{
			preferences |= PREFERENCE_LARGEICONS;
			}

		static void ClearLargeIconsPref()
			{
			preferences &= ~PREFERENCE_LARGEICONS;
			}

		static BOOL TestLargeIconsPref()
			{
			return (preferences & PREFERENCE_LARGEICONS)?TRUE:FALSE;
			}

		static void SetObjSignalValuesPref()
			{
			preferences |= PREFERENCE_SIGNALVALUES;
			}

		static void ClearObjSignalValuesPref()
			{
			preferences &= ~PREFERENCE_SIGNALVALUES;
			}

		static BOOL TestObjSignalValuesPref()
			{
			return (preferences & PREFERENCE_SIGNALVALUES)?TRUE:FALSE;
			}

		static void SetBufferedRedrawPref()
			{
			preferences |= PREFERENCE_BUFFEREDREDRAW;
			}

		static BOOL TestBufferedRedrawPref()
			{
			return (preferences & PREFERENCE_BUFFEREDREDRAW)?TRUE:FALSE;
			}

		static void ClearBufferedRedrawPref()
			{
			preferences &= ~PREFERENCE_BUFFEREDREDRAW;
			}

    static void SetMakeBackUpsPref()
      {
      preferences |= PREFERENCE_MAKEBACKUPS;
      }

    static void ClearMakeBackUpsPref()
      {
      preferences &= ~PREFERENCE_MAKEBACKUPS;
      }

    static BOOL TestMakeBackUpsPref()
      {
      return (preferences & PREFERENCE_MAKEBACKUPS)?TRUE:FALSE;
      }

		static void ReadPreferences();

		static void SetPreferences();

		static BOOL CALLBACK PreferenceDlgProc(
			HWND hDlg,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static LRESULT CALLBACK PrefGridEditProc(
			HWND hwnd,
			UINT message,
			WPARAM wParam,
			LPARAM lParam);

		static AOBJECTID APIENTRY CreateDynamicObject(
			ATYPEID typeID,
			APROTOCOLID protocolID);

		static BOOL CALLBACK DestroyObject(
			AOBJECTID objID);

		static BOOL CALLBACK RenameObject(
			AOBJECTID objID,
			LPSTR newName);

		static BOOL BuildMasterTypeList();

		static void DestroyMasterTypeList();

		static void SetShutDownFlag()
			{
			status |= STATE_SHUTDOWN;
			}

		static void ClearShutDownFlag()
			{
			status &= ~STATE_SHUTDOWN;
			}

		static BOOL TestShutDownFlag()
			{
			return (status & STATE_SHUTDOWN)?TRUE:FALSE;
			}

		static LPSTR GetVABHelpFile()
			{
			return (LPSTR)VABHelpFile;
			}

		static LPCSTR GetSplashResourceName();

		static HICON GetVABIcon()
			{
			return vabIcon;
			}

		static HICON GetSubjectIcon()
			{
			return sjtIcon;
			}

		static HICON GetProjectIcon()
			{
			return prjIcon;
			}

		static void SetSingleStepFlag()
			{
			status |= STATE_SINGLESTEP;
			}

		static void ClearSingleStepFlag()
			{
			status &= ~STATE_SINGLESTEP;
			}

		static BOOL TestSingleStepFlag()
			{
			return (status & STATE_SINGLESTEP)?TRUE:FALSE;
			}


	static char* GetBuildNum(char* buffer, int bufLength);
  
	static char* GetBuildDate(char* buffer, int bufLength);

	static char* GetFrameWndName(char* buffer, int bufLength);

	static char* GetBuildInfoFrameWndName(char* buffer, int bufLength);

  static void ShowBuildInfo();

	static void SetUninitParamsWarningPref()
		{
    preferences|= PREFERENCE_UNINITPARAMS;
		}

	static void ClearUninitParamsWarningPref()
		{
		preferences &= ~PREFERENCE_UNINITPARAMS;
		}

  static BOOL TestUninitParamsWarningPref()
		{
		return (preferences & PREFERENCE_UNINITPARAMS)?TRUE:FALSE;
		}

	};

#endif
