//======================================================================
// context.h -- task-specific information for OIP.DLL
//
//  jon 10-Jun-92 created
//  jon 23-Jun-92 class/typedef LPEVENTQUEUE instead of #include'ing
//                the file "eventque.h"
//  jon 16-Aug-92 added lastInputInfo data member + access functions
//  jon 21-Aug-92 added lastClickTime data member + access functions
//  jon 27-Aug-92 added FillFromExeInfo() to aid debug world.
//  jon 29-Aug-92 changed body of GetEventList() to return the pointer
//                returned by EXEHEAD::GetExeEventList... not the
//                (incorrect) GetObject() of same.  Brian removed the
//                SERUID constructor that took a pointer. See version
//                2 of MEMIO\HDR\SERUID.H.
//  jon 28-Sep-92 Added support for 'isAtStop' flag
//  jon 30-Sep-92 removed IsAtStop() member function.
//  jon 30-Sep-92 IsRuntime now returns TRUE if we are at a stop point
//  jon 11-Dec-92 added IsAppRunning() cuz isRuntime returns TRUE for
//                compile-time and stop-time too...
//  jon 16-Feb-93 double-click deltas: doubleClick<X,Y>
//======================================================================

#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "contextd.h"
#include "rtreclib.h"

inline LPSTR APPCONTEXT::GetExeFileName()
  {
  return exeInfo.pAuxInfo->lpExeFileName;
  }

inline void  * APPCONTEXT::GetGlobalBlockAddr()
  {
  return exeInfo.pAuxInfo->globalBlockAddr;
  }

inline unsigned APPCONTEXT::GetGlobalBlockSize()
  {
  return exeInfo.pAuxInfo->globalBlockSize;
  }

inline LPMASTERTYPELIST APPCONTEXT::GetMasterTypeList()
	{
	return exeInfo.pAuxInfo->masterTypeList;
	}

inline DEBUGPROC APPCONTEXT::GetDebugProc()
  {
  return exeInfo.pAuxInfo->debugger;
  }

inline OBJCREATEPROC APPCONTEXT::GetObjCreateProc()
  {
  return exeInfo.pAuxInfo->pfnObjCreate;
	}

inline OBJDESTROYPROC APPCONTEXT::GetObjDestroyProc()
	{
	return exeInfo.pAuxInfo->pfnObjDestroy;
	}

inline OBJSETNAMEPROC APPCONTEXT::GetObjSetNameProc()
	{
	return exeInfo.pAuxInfo->pfnObjSetName;
	}

inline SHOWVABPROC APPCONTEXT::GetShowVABProc()
	{
	return exeInfo.pAuxInfo->pfnShowVAB;
	}

inline LPFILEOBJECT APPCONTEXT::GetFileObject()
  {
  return exeInfo.pAuxInfo->fileObj;
  }

inline LPEXEHEAD APPCONTEXT::GetExeHead()
  {
  return exeHead;
  }

inline RUNTIMEUID APPCONTEXT::GetObjTypeList()
  {
  return 0;
//  (LPEXESERIUSRECORD)
//         ::GetObject(exeHead->GetExeObjTypeList());
  }

inline LPEXESERIUSRECORD APPCONTEXT::GetEventList()
  {
  return (LPEXESERIUSRECORD) exeHead->GetExeEventList();
  }

inline LPEXESERIUSRECORD APPCONTEXT::GetFuncList()
  {
  return 0;
  //(LPEXESERIUSRECORD)
  //       ::GetObject(exeHead->GetExeFuncList());
  }

inline LPSERIUSMEMORY APPCONTEXT::GetTheMemory()
  {
  return exeInfo.pAuxInfo->seriusMem;
  }

inline LPSTRINGSPACE	APPCONTEXT::GetTempStringSpace()
  {
  return exeInfo.pAuxInfo->tempStringSpace;
  }

inline void APPCONTEXT::SetFileObject(
  LPFILEOBJECT theFileObject)
	{
	exeInfo.pAuxInfo->fileObj = theFileObject;
	}

inline LONG APPCONTEXT::GetAppPriorityMode()
  {
  return priorityMode;
  }

inline LPEVENTQUEUE APPCONTEXT::GetEventQueue()
	{
	return eventQueue;
	}

inline LPEXEFUNCINST APPCONTEXT::GetCurrFunc()
	{
	return eventQueue->GetCurrFunc();
	}

inline BOOL APPCONTEXT::IsMicroApp()
	{
  return 0 != (exeInfo.flags & isMicroApp);
  }

inline BOOL APPCONTEXT::IsDebug()
	{
	return  0 != (exeInfo.flags & isDebug);
	}

inline BOOL APPCONTEXT::IsRuntime()
	{
	return  0 != (exeInfo.flags & isRuntime);
	}

inline BOOL APPCONTEXT::IsExeStructures()
	{
	return 0 != (exeInfo.flags
                 &
               (isRuntime | isCompile | isAtStop));
	}

inline void APPCONTEXT::SetIsAtStop()
  {
  exeInfo.flags |= isAtStop;
  }

inline void APPCONTEXT::ClearIsAtStop()
  {
  exeInfo.flags &= ~isAtStop;
  }

inline HICON APPCONTEXT::GetMasterIcon()
  {
  return hIcon;
  }

inline BOOL APPCONTEXT::IsShutdownTime()
  {
  return (eventQueue->IsOpen() == FALSE);
  }

inline LPFASTRECMGR APPCONTEXT::GetEventQueueNodeMgr()
	{
	return eventQueueNodeMgr;
	}

inline LPFASTRECMGR APPCONTEXT::GetEventQueueMsgMgr()
	{
	return eventQueueMsgMgr;
	}

inline BOOL APPCONTEXT::TestSingleStepFlag()
	{
	return (exeInfo.flags & doSingleStep)?TRUE:FALSE;
	}

inline void APPCONTEXT::ClearSingleStepFlag()
	{
	exeInfo.flags &= ~doSingleStep;
	}

#endif  // __CONTEXT_H
