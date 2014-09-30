/*//////////////////////////////////////////////////////////////
//
//											Copyright 1993, Novell, Inc.
//
//		<standard disclaimers here>
//
//
//--------------------------------------------------------------
//
// FILE:					OIPEXE.CPP
//
// AUTHOR:
//
// DESCRIPTION:		Calls into OIP.DLL from a kernel
//
// CHANGES:
//	7/22/93		Scott
//		Rewrote exeContinueApp so that we don't put Windows messages on the
//		Serius queue.
// jon 22-Feb-93 exeRegisterContext now returns BOOL instead of LONG.
// jon 27-Aug-92 go ahead and exeUnregisterContext in exeContinueApp
//               if the test app has shutdown... helps Troy alot.
// jon 24-Aug-92 added exeResetApp
// jon  6-Aug-92 added exe<Run,Continue>App
//               and <load,dump>RuntimeInfo
// jon 14-Jun-92 created
//////////////////////////////////////////////////////////////*/

#include "almsys.h"
#include "intalm.h"
#include "intdebug.h"

#include "oipexe.h"
#include "eventque.h"

#include "exeinfo.h"
#include "exelibl.h"
#include "exeprocl.h"
#include "exetypes.h"
#include "exefunc.h"
#include "exeevntl.h"
#include "exebead.h"
#include "rectypes.h"
#include "oiputils.h"
#include "context.h"
#include "oipdebug.h"
#include "oiptime.h"
#include "fastrecm.h"
#include "tempobj.h"
#include "splshdis.h"
#include <ctl3d.h>
#include <a_almutl.h>

static BOOL loadRuntimeInfo();
static void dumpRuntimeInfo();
static BOOL	SetupDelayTimer(AEvtInfo*);

#define NO_FLUSH     0x0
#define YES_FLUSH    0x1


class DelayTimer
	{
	public:
							DelayTimer(void)	: m_nID(0)	{ }
						 ~DelayTimer(void)	{ }

		UINT			ID(void)					const		{return m_nID;}
		BOOL			IsSet(void)				const		{return (m_nID != 0);}
		LONG			TargetTime(void)	const		{return m_nTargetTime;}
		BOOL			Set(LONG);
		void			Kill(void);
		void			Triggered(void);

	private:
		UINT			m_nID;						// timer id returned from SetTimer()
		LONG			m_nTargetTime;		// time to go off
	};

//////////////// Module Statics

DelayTimer		s_CurrentTimer;

//----------------------------------------------------------------------
//
// TimerCallback
//
// Callback proc for the timer.  Posts an application message.
//
//----------------------------------------------------------------------

VOID CALLBACK	TimerCallback(
	HWND		hWnd,
	UINT		nMsg,
	UINT		nID,
	DWORD		dwTime
	)
	{
	I_UNUSED(hWnd);
	I_UNUSED(nMsg);
	I_UNUSED(dwTime);

	I_ASSERT(s_CurrentTimer.ID() == nID);

	s_CurrentTimer.Triggered();

	s_CurrentTimer.Kill();
	}

//----------------------------------------------------------------------
//
// Set
//
// Sets the timer for the given time (delta from now).
//
//----------------------------------------------------------------------
BOOL		DelayTimer::Set(
	LONG		tDelta						// in milliseconds
	)
	{
	BOOL		fSet	= FALSE;

	if (!IsSet())
		{
		m_nID = ::SetTimer(NULL, 0, tDelta, (TIMERPROC)TimerCallback);
		if (m_nID != 0)
			{
			m_nTargetTime = tDelta;
			fSet = TRUE;
			}
		}

	return fSet;
	}

//----------------------------------------------------------------------
//
// Kill
//
// Cleans up the delay timer.
//
//----------------------------------------------------------------------
void		DelayTimer::Kill(
	void
	)
	{
	if (IsSet())
		{
		::KillTimer(NULL, m_nID);

		m_nID         = 0;
		m_nTargetTime = 0;
		}
	}

//----------------------------------------------------------------------
//
// Triggered
//
// Called when the timer goes off.
//
//----------------------------------------------------------------------
void		DelayTimer::Triggered(
	void
	)
	{
	PostThreadMessage(GetCurrentThreadId(), WM_NULL, 0, 0);
	}

//----------------------------------------------------------------------
//
// SetupDelayTimer
//
// Sets up a timer to go off in time for the next future event.  If there
// is a timer already that is set to go off before this event, then
// no work needs to be done, otherwise we need to kill the current timer
// and create one.
//
//----------------------------------------------------------------------
static BOOL		SetupDelayTimer(
	AEvtInfo*			pNextDelayedEvent
	)
	{
	LONG					lTime;
	BOOL					fSet;

	// Convert from 60ths of a second to milliseconds
	lTime = (pNextDelayedEvent->when * 1000L) / 60L;

	if (!s_CurrentTimer.IsSet() ||
			lTime < s_CurrentTimer.TargetTime())
		{
		s_CurrentTimer.Kill();

		// Get # of milliseconds to delay
		lTime -= GetTickCount();

		if (lTime > 0)
			{
			if (lTime > 65535ul)
				lTime = 65535u;

			fSet = s_CurrentTimer.Set((UINT) lTime);
			}
		else
			fSet = FALSE;
		}
	else
		fSet = TRUE;					// we still have a timer, just not one
													// specifically for this time (but earlier)

	return fSet;
	}

extern "C" {

//----------------------------------------------------------------------
//
//    exeRegisterContext
//
//  Called by the shell to register the serius context for this task.
//
//----------------------------------------------------------------------
BOOL ALMAPI	exeRegisterContext (LPEXEINFO theContext)
  {
  return pTheSystem->RegisterContext(theContext);
  }


//----------------------------------------------------------------------
//
//    exeUnregisterContext
//
//  Called by the shell to unregister the serius context for this task.
//
//----------------------------------------------------------------------
BOOL ALMAPI	exeUnregisterContext()
  {
  return pTheSystem->UnregisterContext();
  }


//----------------------------------------------------------------------
//
//    exeRunApp
//
//----------------------------------------------------------------------
void ALMAPI	exeRunApp()
  {
  LPEVENTQUEUE   mainQueue;
  AEvtInfo    myEvent;

#ifdef AW_I_TIME
	StartHiResTimer();
#endif

  if (!loadRuntimeInfo())
    {
    MessageBox(
      NULL,
      "This application cannot be run"
      " because of previously reported errors.",
      "Application Error",
      MB_OK|MB_TASKMODAL|MB_ICONSTOP);

    dumpRuntimeInfo();
    exeUnregisterContext();
    return;
    }

  mainQueue = pTheSystem->GetCurrentContext()->GetEventQueue();

  myEvent.targetObject = 0;
  myEvent.sourceObject = 0;
  myEvent.details      = 0;
  myEvent.modifiers    = 0;
  myEvent.variation    = 0;

  myEvent.what = AEVENT_STARTUP;
  myEvent.when = AAppGetTime();
  myEvent.how  = AEVT_ATMARK;
  mainQueue->Push(&myEvent);

  exeContinueApp(FALSE);

#ifdef AW_I_TIME
	PrintHandlerTimes();
	EndHiResTimer();
#endif
  }

//----------------------------------------------------------------------
//
//    exeContinueApp
//
//  We were running and then we hit a stop... now we wanna go again.
//
//----------------------------------------------------------------------
void ALMAPI	exeContinueApp(
	BOOL		fReset
	)
  {
	BOOL						fKill;
 	BOOL						fValidMsg;
	BOOL						fMainIsEmpty;
	BOOL						fWaitIsEmpty;
	BOOL						fHaveTimer;
	AEvtInfo*				pWaiter;
	MSG							msg;
  LPEVENTQUEUE		mainQueue;
  pAEvtInfo				poppedEvent;
  LPAPPCONTEXT				context;

  context = pTheSystem->GetCurrentContext();

	// Flush the Window's message queue since we are delaying the processing
	// of Window's messages until no ATMARK events are popped.  This will
	// cause problems with accelerators causing this procedure to be
	// invoked (such as Ctrl-R to run an application).
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOYIELD | PM_REMOVE) != 0)
		{
		TranslateMessage(&msg);
		
		if((msg.message == WM_SYSCOLORCHANGE) &&
			(AUtlGetOSVersion() == AUTL_OS_WINNT))
  		{
      Ctl3dColorChange();
    	}
		 
		DispatchMessage(&msg);
		}

  // We are not now at a stop point
  context->ClearIsAtStop();

  // Get our queue
  mainQueue = context->GetEventQueue();

  //only call setjmp if we're not the micro app becausing calling setjmp
  //in the kernel compiled with Borland 4.5 with a shell compiled with
  //Borland 3.1 causes a crash.
	if(!context->IsMicroApp())
  	{
	  if (!mainQueue->GotBailOut())
			{
	    jmp_buf  jumper;
	    int      bailing;

	    bailing = setjmp(jumper);
	    if (bailing)
	      return;
	    else
	      mainQueue->SetBailOut(&jumper);
	    }
    }

  for (;;)
    {
		// We want to interleave Windows' message processing and internal
		// queue processing.

		fValidMsg = FALSE;

    poppedEvent = mainQueue->Pop();
		if (poppedEvent)
			{
			mainQueue->HandleEvent(poppedEvent);
			}

		fMainIsEmpty = (mainQueue->GetHead() == NULL);
		fWaitIsEmpty = ((pWaiter = mainQueue->WaitQueueHead()) == NULL);

		if (fMainIsEmpty && !fWaitIsEmpty && mainQueue->IsOpen())
			{
			// We have nothing to do except wait for delayed events.
			// At this point we try to get a timer to wait for us, otherwise
			// we do busy waiting.
			fHaveTimer = SetupDelayTimer(pWaiter);
			}
		else
			fHaveTimer = FALSE;

		if (fMainIsEmpty && (fWaitIsEmpty || fHaveTimer || !mainQueue->IsOpen()))
			{
			// All main internal messages are processed.  Since we are idle, get the
			// next Window's message and dispatch if we aren't shutting down

			if (!mainQueue->IsOpen())
				{
				// We are shutting down and
				// all ATTAIL events must have been dispatched, so it is safe to
				// quit.

				// At this point, kill any timer that this task owns (transfer
				// ownership if necessary).
				s_CurrentTimer.Kill();

				fKill = TRUE;
				break;
				}

			else if (GetMessage(&msg, NULL, NULL, NULL))
				fValidMsg = TRUE;
#ifdef AW_I_DEBUG
			else
				I_ASSERT(FALSE);
#endif
			}
		else if ((poppedEvent == NULL) || !(poppedEvent->how & AEVT_ATMARK))
			{
			// Get Windows' message (if one exists) and dispatch it.  Don't
			// block since there are messages either in the wait queue or in
			// the main queue.

			if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				fValidMsg = TRUE;
			}

		// If we have a valid Windows' message, dispatch it.
		if (fValidMsg)
			{
			ADBG_EVAL(PrintMsgInfo(&msg));

			TranslateMessage(&msg);

			if((msg.message == WM_SYSCOLORCHANGE) &&
				(AUtlGetOSVersion() == AUTL_OS_WINNT))
  			{
      	Ctl3dColorChange();
    		}
		 

			DispatchMessage(&msg);
			}
    }

	// If we are not killing the application (i.e. we are at a pause),
	// then don't kill the info
  if (!fReset && fKill)
		{
		pTheSystem->GetCurrentContext()->MaybeWritePersistentData();
		dumpRuntimeInfo();
		exeUnregisterContext();
		}
  }


//----------------------------------------------------------------------
//
//    exeResetApp
//
//  We were running and then we hit a stop... now we wanna kill the app!
//  Do it by posting a AEVENT_SHUTDOWN.
//
//----------------------------------------------------------------------
void ALMAPI	exeResetApp()
  {
  AAppPostShutdown(TRUE);
  exeContinueApp(TRUE);
  }


} // extern "C"


//----------------------------------------------------------------------
//
//  FinishLoadTiming (debug only)
//
//----------------------------------------------------------------------

#ifdef AW_I_TIME

void	FinishLoadTiming(
	long		nStart
	)
	{
	char						szMsg[80];

	nStart = HiResTime() - nStart;
	wsprintf(szMsg, "DLL load time (ms):  %ld\r\n", nStart);
	OutputDebugString(szMsg);
	}
#endif

//----------------------------------------------------------------------
//
//    loadRuntimeInfo
//
//  Load all the DLLs and fill in proc addrs for functions and events.
//
//----------------------------------------------------------------------
static BOOL loadRuntimeInfo()
	{
	LPEXELIBLIST    exeLibList;
	LPEXEEVENTLIST  exeEventList;
	LONG            libIdx;
	LONG            numLibs;
	BOOL            cool = TRUE;
	int             ans;
	LONG						lTmp;
  LPAPPCONTEXT  context = pTheSystem->GetCurrentContext();

#ifdef AW_I_TIME
	long						nDllLoadTime	= HiResTime();
	char						szMsg[80];
#endif

	if(!context->IsMicroApp())
  	{
    context->GetFileObject()->ShowSplashScreen(); 
    }
	
	exeLibList =   ((LPEXEHEAD)GetObject(context->GetFileObject()->GetHeadExeRec()))->GetExeLibList();
	exeEventList = ((LPEXEHEAD)GetObject(context->GetFileObject()->GetHeadExeRec()))->GetExeEventList();

	numLibs = exeLibList->GetNumNodes();
	for (libIdx = 0 ; libIdx < numLibs ; libIdx++)
		{
		LPEXELIB        exeLib;
		LPEXEPROCLIST   exeProcList;
    HINSTANCE       hLib;
    LONG            procIdx;
    LONG            numProcs;

    exeLib = (LPEXELIB)exeLibList->IdxGetNode(libIdx);

    LPCSTR          libName		= exeLib->GetName();

    int oldErrorMode = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
    hLib = LoadLibrary(libName);
    ::SetErrorMode(oldErrorMode);

		if (hLib == NULL)
      {
      exeLib->SetHLib(0);
      char caption[200], text[200];
      wsprintf(caption, "Error Loading a DLL (application prolog)");
      wsprintf(text, "Error %x occurred while attempting to load %s.",
         GetLastError(), libName);
			ans = MessageBox(NULL, text, caption, MB_OKCANCEL|MB_TASKMODAL);
      cool = FALSE;
      if (ans == IDOK)
        continue;  // on to the next DLL
      else
#ifdef AW_I_TIME
				FinishLoadTiming(nDllLoadTime);
#endif
        return FALSE;
      }

    exeLib->SetHLib(hLib);

    exeProcList = exeLib->GetProcList();
    numProcs = exeProcList->GetNumNodes();
    for (procIdx = 0 ; procIdx < numProcs ; procIdx++)
      {
      PROC    procAddr;
      LPEXEPROC  exeProc;
      LONG       beadKey;
      LPEXEBEAD  beadNow;

      exeProc = (LPEXEPROC)exeProcList->IdxGetNode(procIdx);

			LPCSTR      procName	= exeProc->GetName();

			procAddr = GetProcAddress(hLib, procName);
      if (!procAddr)
        {
        char caption[200], text[200];
        wsprintf(caption, "Error (application prolog)");
        wsprintf(text, "Cannot find an entry point named %s in %s",
                 (LPSTR) procName,
                 (LPSTR) libName);
        ans = MessageBox(NULL, text, caption, MB_OKCANCEL|MB_TASKMODAL);
        cool = FALSE;
				if (ans == IDOK)
					continue;   // on to the next proc
				else
#ifdef AW_I_TIME
					FinishLoadTiming(nDllLoadTime);
#endif
					return FALSE;
				}

			exeProc->SetAddress(procAddr);

			// Extract the beads from this proc's strand
			beadKey = -1;
			while (exeProc->GetNextBead(&beadNow, &beadKey))
				{
				SERUID         targetUID;
        LONG           uidType;
        LPEXEOBJTYPE   pObjType;
        LPEXEFUNCINST  pFuncInst;

        targetUID = beadNow->GetUID().id;
        if (targetUID == NULLSERUID)
          {
         // it's an event...
          exeEventList->SetMethodAddr(
										procAddr,
                    beadNow->GetTypeID(),
                    beadNow->GetEventID());
          }
				else
          {
          uidType = GetUIDType(targetUID);
					if (uidType == RT_EXEOBJTYPE)
            {
            // The only thing we register for an Object is the ADMN
            pObjType = (LPEXEOBJTYPE)GetObject(targetUID);
            pObjType->SetAdmnProc((ADMNPROC)procAddr);
            }
          else if (uidType == RT_EXEFUNCINST)
            {
            // It's a Function ... set its address
            pFuncInst = (LPEXEFUNCINST)GetObject(targetUID);
            pFuncInst->SetMethodAddr((METHODPROC)procAddr);
            }
          else
            {
            char caption[200], text[200];
						wsprintf(caption, "AppWare Internal Error (application prolog)");
            wsprintf(text,
              "The entry point %s in %s has"
              " an invalid UID type of %li.",
							(LPSTR) libName,
              (LPSTR) procName,
              uidType);
						ans = MessageBox(NULL, text, caption, MB_OKCANCEL|MB_TASKMODAL);
            cool = FALSE;
            if (ans == IDOK)
              continue;   // on to the next bead in this proc's strand
            else
#ifdef AW_I_TIME
							FinishLoadTiming(nDllLoadTime);
#endif
              return FALSE;
            }
					}
				}
			}
 
		// dump this lib's proc list here!

		}

	// dump lib list here!
	// dump beadlist here!

  // if we are the microapp then check the versions

  // We are not now at a stop point
  if (context->IsMicroApp())
    {
	  int i;
	  LPEXEOBJTYPE pType;
		RUNTIMEUID typeList =
	   ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))->
		  GetExeObjTypeList();

#ifdef AW_I_TIME
		long			nAwakenTime	= HiResTime();
#endif

		// In order to follow the "read, then awaken" message flow, we need
		// to check the version (which currently sends the AOBJ_READ message)
		i = 0;
	  while (0 != (pType = (LPEXEOBJTYPE)GetObject(typeList, i)))
		  {
		  if (!pType->CheckObjectVersions())
        cool = FALSE;
		  }

		// Awaken all the objects.  This only needs to happen in the Micro-app since
		//AppBuilder will send the message if the project is run from within the
		//environment.
		lTmp = 0;
		SendMessageToObject(AOBJ_AWAKENED, lTmp, lTmp, lTmp, 0, 0);

#ifdef AW_I_TIME
		nAwakenTime	= HiResTime() - nAwakenTime;
		wsprintf(szMsg, "Awaken time (ms):  %ld\r\n", nAwakenTime);
		OutputDebugString(szMsg);
#endif
    }

#ifdef AW_I_TIME
	FinishLoadTiming(nDllLoadTime);
#endif

	context->GetFileObject()->HideSplashScreen();
	
	return cool;
	}

//----------------------------------------------------------------------
//
//    dumpRuntimeInfo
//
//  Unload all the DLLs.
//
//----------------------------------------------------------------------
static void dumpRuntimeInfo()
  {
	LONG					lTmp	= 0;
  LPEXELIBLIST  exeLibList;
  LPEXELIB      exeLib;
  LONG          libIdx;
  LONG          numLibs;
	LPTEMPOBJECT	tempObjPtr;
	long						key;
	LPFASTRECMGR  tempObjects;

	// Put all objects to sleep.
	SendMessageToObject(AOBJ_ASLEEP, lTmp, lTmp, lTmp);

	//delete any remaining temporaryobjects
	tempObjPtr = NULL;
	tempObjects = pTheSystem->GetCurrentContext()->GetFileObject()->GetTempObjects();
	while(NULL != (tempObjPtr = 
		(LPTEMPOBJECT)tempObjects->GetNextUsedRecord((LPSTR)tempObjPtr, key)))
		{
		//clearing the use count forces the object to be destroyed in
		//AObjDestroy.
		tempObjPtr->ClearUseCount();
		AObjDestroy(FASTRECMGR::GetUID(tempObjPtr));
		}

	exeLibList = ((LPEXEHEAD)GetObject(pTheSystem->GetCurrentContext()->GetFileObject()->GetHeadExeRec()))->GetExeLibList();

	numLibs = exeLibList->GetNumNodes();
	for (libIdx = 0 ; libIdx < numLibs ; libIdx++)
		{
		exeLib = (LPEXELIB)exeLibList->IdxGetNode(libIdx);
		if (exeLib->GetHLib() != NULL)
			FreeLibrary(exeLib->GetHLib());
		}
	}

//----------------------------------------------------------------------
//    postShutdown
//----------------------------------------------------------------------
extern "C"
void ALMAPI	AAppPostShutdown(
  BOOL	fFlush
	)
  {
  LPEVENTQUEUE   mainQueue;
  AEvtInfo    myEvent;
  LPAPPCONTEXT      context;

  context = pTheSystem->GetCurrentContext();

  // We are not now at a stop point
  context->ClearIsAtStop();

  // Get our queue
  mainQueue = context->GetEventQueue();

  // Flush the queue
  if (fFlush)
    while(mainQueue->Pop())
      ;

  // Post the shutDown
  myEvent.targetObject = 0;
  myEvent.sourceObject = 0;
  myEvent.details      = 0;
  myEvent.modifiers    = 0;
  myEvent.variation    = 0;
  myEvent.what = AEVENT_SHUTDOWN;
  myEvent.how  = AEVT_ATTAIL;
  myEvent.when = AAppGetTime();

  mainQueue->Push(&myEvent);
  }
