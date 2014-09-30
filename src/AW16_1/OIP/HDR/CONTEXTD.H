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

#ifndef __CONTEXTD_H
#define __CONTEXTD_H

#include "exehead.h"
#include "fileobj.h"
#include "exeinfo.h"
#include "exefunc.h"
#include "strspace.h"
#include "eventque.h"
#include "mtypelst.h"

#ifndef SM_CXDOUBLECLK

#define SM_CXDOUBLECLK       36
#define SM_CYDOUBLECLK       37

#endif

#define CONTEXT_DUPINSTANCE		0x00000001

class APPCONTEXT
	{

  friend class SYSTEM;
	friend class SYSTEMFULL;

private:

	APPCONTEXT   far *next;
	HTASK          taskID;

  EXEINFO        exeInfo;
	EXEINFOAUX		 exeInfoAux;

	LPEXEHEAD      exeHead;        // list headers + misc info
  LONG           osVersion;      // 00 00 xx yy: xx=major, yy=minor

	LPEVENTQUEUE   eventQueue;     // runtime only
	LONG           priorityMode;   // runtime only
	LPEXEFUNCINST  currFunc;       // runtime only currently executing function

  LPFASTRECMGR   eventQueueNodeMgr;
  LPFASTRECMGR   eventQueueMsgMgr;

  HICON          hIcon;

	long					 status;

  LPSTR GetExeFileName();

  void far *GetGlobalBlockAddr();

  unsigned GetGlobalBlockSize();

  void  FreshenExeInfo(
    LPEXEINFO theExeInfo);

  BOOL  LoadMicroApp();
  int   OpenExeFile();

  int   OpenPersistents(
    LPINT hPersFile,
    LPSTR  iniFileName);

  BOOL APPCONTEXT::FetchPdfPathFromIni(
    LPSTR   iniPath,
    LPSTR   pdfPath,
    size_t  pdfPathSize);

  BOOL APPCONTEXT::ComplainAboutPdfPath(
    LPSTR pdfPath,
    LPSTR iniPath);

  BOOL  ReadPersistents(
    int  hFile);

public:

	void far * operator new(
    size_t size);

	void operator delete(
    void far *deadObject,
    size_t    t);

	APPCONTEXT(
    HTASK      taskNow,
    LPEXEINFO  theExeInfo);

	~APPCONTEXT();

  BOOL Freshen(
    LPEXEINFO theExeInfo);

  BOOL GetCopyOfExeInfo(LPEXEINFO theCurrentExeInfo);

	HTASK GetTaskID();

	LPFILEOBJECT GetFileObject();

	LPEXEHEAD  GetExeHead();

	RUNTIMEUID  GetObjTypeList();

	LPEXESERIUSRECORD  GetEventList();

	LPEXESERIUSRECORD  GetFuncList();

	LPSERIUSMEMORY  GetTheMemory();

	LPSTRINGSPACE	 GetTempStringSpace();

  DEBUGPROC				GetDebugProc();

	OBJCREATEPROC		GetObjCreateProc();

	OBJDESTROYPROC	GetObjDestroyProc();

	OBJSETNAMEPROC	GetObjSetNameProc();

	SHOWVABPROC GetShowVABProc();
	
	void SetFileObject(
    LPFILEOBJECT theFileObject);

	LONG  GetOSVersion();

	LONG  GetAppPriorityMode();

	LONG  SetAppPriorityMode(
    LONG newMode);

	LPEVENTQUEUE GetEventQueue();

	LPEXEFUNCINST GetCurrFunc();

  BOOL IsMicroApp();

  BOOL IsDebug();

  BOOL IsRuntime();

	BOOL IsExeStructures();

  BOOL IsDesignTime();
  
  void SetIsAtStop();

  void ClearIsAtStop();

  HINSTANCE GetTaskHInstance();

  HICON GetMasterIcon();

  BOOL IsShutdownTime();

	LPFASTRECMGR GetEventQueueNodeMgr();

	LPFASTRECMGR GetEventQueueMsgMgr();

	void SetGlobals();

	BOOL EnterDebugger(LONG exeFlow);

	BOOL  SavePersistents(
		BOOL displayErrorsFlag = TRUE);

	LPMASTERTYPELIST GetMasterTypeList();

	BOOL MaybeWritePersistentData();

	BOOL TestSingleStepFlag();

	void ClearSingleStepFlag();
	
	BOOL TestDupInstanceFlag();

	void SetDupInstanceFlag();

	void ClearDupInstanceFlag();
	
	HINSTANCE GetPrevInstance();
	};

typedef APPCONTEXT far *LPAPPCONTEXT;

#endif  // __CONTEXT_H

