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

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

static BOOL loadRuntimeInfo();
static void dumpRuntimeInfo();
static BOOL	SetupDelayTimer(AEvtInfo*);
static void	KillDelayTimer(UINT);

static void I_CALLBACK	DelayProc1(HWND, UINT, UINT, DWORD);
static void I_CALLBACK	DelayProc2(HWND, UINT, UINT, DWORD);

  #define NO_FLUSH     0x0
  #define YES_FLUSH    0x1

/* //////////////////////////////////////////////////////////////

Timer Discussion.  In order to minimize the use of timers within
AppWare applications that post events in the future, we share timers
across all the applications.  Doing so requires inter-task management
of the timers, especially because Windows doesn't allow a task other
than the one that created the timer to destroy it.

At the time that we want to create a timer event, we know that we
have processed every AppWare event in the current task and there is a
delayed event in *some* AppWare task.  We don't know, however, if we
need to process Windows messages.  There are two possibilities:  we
post an app message to the task that needs to set the timer, or we
set the timer in this task and when this task receives it, it posts
an app message to the correct task.  The more efficient method is to
send a message to the correct task and then call GetMessage().  The
reason for this is that, if GetMessage() starts a cascade of messages
in this task, when we finally get back to the stable state and we
want to set a timer in the other application, we might not need to.
Moreover, even if we did set the timer in the current task, we
wouldn't relinquish control to the other task anyway until the same
time that the app message would get there (the one that tells it to
create a timer).

Assumptions:

* we only need to look at all contexts for the smallest absolute
time when we receive a timer event

////////////////////////////////////////////////////////////// */

class DelayTimer
	{
	private:
		UINT			m_nID;								// external id of timer
		UINT			m_nOSTimerId;					// OS timerid
		LONG			m_nTargetTime;				// time event will be sent
		HTASK			m_hOwnerTask;					// task that created the timer
		TIMERPROC	m_pfnCallback;				// callback

	public:
							DelayTimer(UINT);
						 ~DelayTimer(void);

		BOOL			Set(UINT);
		void			Kill(void);

		BOOL			IsSet(void)				{return (m_nOSTimerId != 0);}
		UINT			Id(void)					{return m_nID;}
		HTASK			OwnerTask(void)		{return m_hOwnerTask;}
		LONG			TargetTime(void)	{return m_nTargetTime;}
	};

#define TID_ALL				0
#define TID_MAIN			1
#define TID_AUX				2

// Application messages
#define WM_KILLTIMER				(WM_USER + 0x100)
#define WM_SETTIMER					(WM_USER + 0x101)
#define WM_RECEIVEDTIMER		(WM_USER + 0x102)

static DelayTimer			s_Timer1(TID_MAIN);
static DelayTimer			s_Timer2(TID_AUX);
static DelayTimer*		s_pCurrentTimer			= &s_Timer1;
static DelayTimer*		s_pDyingTimer				= &s_Timer2;

////////////////////////////////////////////////////////////////
// METHOD:	DelayTimer::DelayTimer
//
// DESCR:		Initializes a DelayTimer object
////////////////////////////////////////////////////////////////
DelayTimer::DelayTimer(
	UINT		nId
	) :
		m_nID(nId),
		m_nOSTimerId(0),
		m_hOwnerTask(NULL),
		m_nTargetTime(0)
	{
	if (nId == TID_MAIN)
		m_pfnCallback = DelayProc1;
	else
		m_pfnCallback = DelayProc2;
	}

////////////////////////////////////////////////////////////////
// METHOD:	DelayTimer::~DelayTimer
//
// DESCR:		Cleans up a DelayTimer object
////////////////////////////////////////////////////////////////
DelayTimer::~DelayTimer(
	void
	)
	{
	I_ASSERT(m_nOSTimerId == 0);
	I_ASSERT(m_hOwnerTask == NULL);
	}

////////////////////////////////////////////////////////////////
// METHOD:	DelayTimer::Set
//
// DESCR:		Sets the timer to go off after the given # of milliseconds.
////////////////////////////////////////////////////////////////
BOOL	DelayTimer::Set(
	UINT		nDelay
	)
	{
	BOOL		fSet		= FALSE;
	UINT		nId;

	// There is a small possibility that we will need another timer
  // before the task that owns the timer has received the request
  // to kill it.  This is okay, since we will just busy-wait.
	// (Don't ask me how this happens, but I know it happens
	// infrequently.)

	if (m_nOSTimerId == 0)
		{
		nId = SetTimer(NULL, m_nID, nDelay, m_pfnCallback);

		if (nId != 0)
			{
			m_nTargetTime = GetTickCount() + nDelay;
			m_nOSTimerId  = nId;
			m_hOwnerTask  = GetCurrentTask();
			fSet = TRUE;
			}
		}

	return fSet;
	}

////////////////////////////////////////////////////////////////
// METHOD:	DelayTimer::Kill
//
// DESCR:		Kills the timer.  If the timer is in a different task, then
//					an application message is sent to the task that owns the
//					timer so that it can be killed.
////////////////////////////////////////////////////////////////
void	DelayTimer::Kill(
	void
	)
	{
	if (IsSet())
		{
		if (GetCurrentTask() == m_hOwnerTask)		// can only kill what we created
			{
			KillTimer(NULL, m_nOSTimerId);

			m_nOSTimerId = 0;
			m_hOwnerTask = NULL;
			}
		else
			{
			PostAppMessage(m_hOwnerTask, WM_KILLTIMER, 0, (LPARAM) (VOID*) this);
			}
		}
	}

//----------------------------------------------------------------------
//
// DelayProcs
//
// Stops the timer and posts a null message to the application so the
// main GetMessage loop will go around one more time.
// 
//----------------------------------------------------------------------

static void I_CALLBACK	DelayProc1(
	HWND													hWnd,
	UINT													nMsg,
	UINT													idTimer,
	DWORD													dwTime
	)
	{
	I_UNUSED(hWnd);
	I_UNUSED(nMsg);
	I_UNUSED(idTimer);
	I_UNUSED(dwTime);

#ifdef AW_I_DEBUG
	if (s_Timer1.OwnerTask() != GetCurrentTask())
		{
		char			szMsg[80];
		wsprintf(szMsg, "DelayProc1:  current task(%x) != owner task(%x)\r\n",
							GetCurrentTask(), s_Timer1.OwnerTask());
		OutputDebugString(szMsg);
		}
#endif

	KillDelayTimer(TID_MAIN);

	PostAppMessage(GetCurrentTask(), WM_RECEIVEDTIMER, 0, 0);
	}

static void I_CALLBACK DelayProc2(
	HWND													hWnd,
	UINT													nMsg,
	UINT													idTimer,
	DWORD													dwTime
	)
	{
	I_UNUSED(hWnd);
	I_UNUSED(nMsg);
	I_UNUSED(idTimer);
	I_UNUSED(dwTime);

#ifdef AW_I_DEBUG
	if (s_Timer2.OwnerTask() != GetCurrentTask())
		{
		char	szMsg[80];
		wsprintf(szMsg, "DelayProc2:  current task(%x) != owner task(%x)\r\n",
							GetCurrentTask(), s_Timer2.OwnerTask());
		OutputDebugString(szMsg);
		}
#endif

	KillDelayTimer(TID_AUX);

	PostAppMessage(GetCurrentTask(), WM_RECEIVEDTIMER, 0, 0);
	}

//----------------------------------------------------------------------
//
// FindMinTimeWaiter
//
// Go through all registered contexts and search for the earliest event
// that was posted to the wait queue.  The task and the event are returned.
// An optional skiptask can be passed in.  This task will not be checked
// for a min time (necessary for a few special cases).  If an invalid
// task is detected during the traversal, it is unlinked.
//
//----------------------------------------------------------------------
static void		FindMinTimeWaiter(
	HTASK&				hFirstWaitTask,
	AEvtInfo*&		pMinWaiter,
	HTASK					hSkipTask
	)
	{
	LPAPPCONTEXT			pContext				= pTheSystem->NextContext(NULL);
	LONG					nMinTime				= -1;
	LPEVENTQUEUE	pQueue;
	HTASK					hContextTask;
	AEvtInfo*			pWaiter;

	pMinWaiter			= NULL;
	hFirstWaitTask	= GetCurrentTask();
	
	while (pContext != NULL)
		{
		pQueue = pContext->GetEventQueue();

		if (pQueue != NULL)
			pWaiter = pQueue->WaitQueueHead();
		else
			pWaiter = NULL;

		hContextTask = pContext->GetTaskID();

		if (!pTheSystem->IsValidContext(pContext))		// removes if invalid
			{
			pContext = NULL;				// if removed, then start over
			}

		// If we're supposed to get all tasks, or this task is not the skiptask,
		// and there are wait events, and either this is the first time
		// through ( == -1) or the wait time < minimum found wait time...
		else if ((hSkipTask == NULL  || hSkipTask != hContextTask) &&
						 (pWaiter   != NULL) &&
						 (nMinTime  == -1    || (pWaiter->when < nMinTime)))
			{
			pMinWaiter     = pWaiter;
			nMinTime       = pWaiter->when;
			hFirstWaitTask = hContextTask;
			}

		pContext = pTheSystem->NextContext(pContext);
		}
	}

//----------------------------------------------------------------------
//
// HandoffTimer
//
// Determines which task needs the timer next.  Whenever a notification
// from a timer is received, this procedure must be called, since we
// need to set up a timer for the next future event.  "hTask" will
// be non-null if we don't want to hand the control off to the current
// task because the event is already past its trigger time.
//
//----------------------------------------------------------------------

static VOID			HandoffTimer(
	HTASK						hTask
	)
	{
	AEvtInfo*			pMinWaiter;
	HTASK					hFirstWaitTask;

	FindMinTimeWaiter(hFirstWaitTask, pMinWaiter, hTask);

	if (hFirstWaitTask != GetCurrentTask())
		PostAppMessage(hFirstWaitTask, WM_SETTIMER, 0, 0);
	else if (pMinWaiter != NULL)
		SetupDelayTimer(pMinWaiter);
	}

//----------------------------------------------------------------------
//
// KillDelayTimer
//
// Cleans up the specified delay timer.
//
//----------------------------------------------------------------------
static void		KillDelayTimer(
	UINT					nId
	)
	{
	if (nId == TID_ALL || nId == TID_MAIN)
		s_Timer1.Kill();

	if (nId == TID_ALL || nId == TID_AUX)
		s_Timer2.Kill();
	}

//----------------------------------------------------------------------
//
// KillDelayTimerTask
//
// Cleans up the delay timer associated with the task.  If there is
// such a beast, then we need to post a SETTIMER appmessage to the
// next task that needs a timer.  This is used when a task is terminating
// and the task contains the next future event.  If this happens we need
// give the timer to the task that has the next future event.
//
//----------------------------------------------------------------------
static void		KillDelayTimerTask(
	HTASK					hTask
	)
	{
	if (s_pCurrentTimer->IsSet() &&
			s_pCurrentTimer->OwnerTask() == hTask)
		{
		KillDelayTimer(s_pCurrentTimer->Id());
		HandoffTimer(hTask);
		}
	}

//----------------------------------------------------------------------
//
// SetupDelayTimer
//
// Sets up a timer to go off in time for the next future event.  If there
// is a timer already that is set to go off before this event, then
// no work needs to be done, otherwise we need to kill the current timer
// and create one.  The assumption is that the current task needs this
// timer to trigger the next current event.
//
//----------------------------------------------------------------------
static BOOL		SetupDelayTimer(
	AEvtInfo*			pNextDelayedEvent
	)
	{
	LONG					lTime;
	DelayTimer*		pTmp;
	BOOL					fSet;

	// It is assumed that task that executes this code is the one
	// that wants to receive the timer event.

	// Convert from 60ths of a second to milliseconds
	lTime = (pNextDelayedEvent->when * 1000L) / 60L;

	if (!s_pCurrentTimer->IsSet() ||
			lTime < s_pCurrentTimer->TargetTime())
		{
		KillDelayTimer(s_pCurrentTimer->Id());

		// Swap them.  The reason we need a dying timer is that the killing
		// of the timer might have posted an app message to another task
		// to actually perform the kill.
		pTmp = s_pCurrentTimer;
		s_pCurrentTimer = s_pDyingTimer;
		s_pDyingTimer   = pTmp;

		// Get # of milliseconds to delay
		lTime -= GetTickCount();

		if (lTime > 0)
			{
			if (lTime > 65535ul)
				lTime = 65535u;

			fSet = s_pCurrentTimer->Set((UINT) lTime);
			}
		else
			fSet = FALSE;
		}
	else
		fSet = TRUE;					// we still have a timer, just not one
													// specifically for us

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
	BOOL						fKill				= FALSE;
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

		// Ensure we have the proper context, since we
		// may have swapped at Peek/GetMessage time...
		pTheSystem->GetCurrentContext();

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
				KillDelayTimerTask(GetCurrentTask());

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

			if (msg.hwnd == NULL)
				{
				if (msg.message == WM_KILLTIMER)
					((DelayTimer*) msg.lParam)->Kill();
				else if (msg.message == WM_SETTIMER)
					{
					pWaiter = mainQueue->WaitQueueHead();

					if (pWaiter != NULL)
						{
						LONG	lTime = (pWaiter->when * 1000L) / 60L;

						if (lTime < GetTickCount())
							HandoffTimer(GetCurrentTask());	// don't let this task have it
						else
							SetupDelayTimer(pWaiter);				// we need the timer
						}
					else
						HandoffTimer(GetCurrentTask());
					}
				else if (msg.message == WM_RECEIVEDTIMER)
					HandoffTimer(NULL);									// let any task have it
				}

			TranslateMessage(&msg);
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
	
	exeLibList =   ((LPEXEHEAD)GetObject(pTheSystem->GetCurrentContext()->GetFileObject()->GetHeadExeRec()))->GetExeLibList();
	exeEventList = ((LPEXEHEAD)GetObject(pTheSystem->GetCurrentContext()->GetFileObject()->GetHeadExeRec()))->GetExeEventList();

	numLibs = exeLibList->GetNumNodes();
	for (libIdx = 0 ; libIdx < numLibs ; libIdx++)
		{
		LPEXELIB        exeLib;
		LPEXEPROCLIST   exeProcList;
    LPSTR           libName;
    HINSTANCE       hLib;
    LONG            procIdx;
    LONG            numProcs;

    exeLib = (LPEXELIB)exeLibList->IdxGetNode(libIdx);
    libName = exeLib->GetName();

    // **************************************************************
    //  All this _FPMATH / _fperr stuff is to make sure things work
    //  ok when we load a DLL that does floating point math.  The
    //  Borland DLL startup code (3.0) for DLLs attempts to load the
    //  address that is stored as the floating point error handler,
    //  which may be invalid if the DLL that last stored its error
    //  handler has been freed from memory.
    // **************************************************************

    // *** _FPMATH is described in Chapter 13 of the Windows 3.1 SDK
    // *** Programmer's Reference: Volume 1.  _fperr is the default
    // *** floating point error handler provided by Borland.
#ifdef __BORLANDC__
    extern far long _FPMATH;
		extern far long _fperr;

    // *** Set the interrupt handler back to our default handler.
    asm {
      mov bx, 3
      mov dx, seg     _fperr
      mov ax, offset  _fperr
      call far ptr _FPMATH
			}
#endif

    WORD oldErrorMode = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
    hLib = LoadLibrary(libName);
    ::SetErrorMode(oldErrorMode);

#ifdef __BORLANDC__
    // *** Set the interrupt handler back to our default handler, again.
    asm {
      mov bx, 3
      mov dx, seg     _fperr
      mov ax, offset  _fperr
      call far ptr _FPMATH
      }
#endif

		if (hLib < HINSTANCE_ERROR)
      {
      exeLib->SetHLib(0);
      char caption[200], text[200];
      wsprintf(caption, "Error Loading a DLL (application prolog)");
      wsprintf(text, "Error %d occurred while attempting to load %s.",
         hLib,
         (LPSTR) libName);
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
			LPSTR      procName;
      FARPROC    procAddr;
      LPEXEPROC  exeProc;
      LONG       beadKey;
      LPEXEBEAD  beadNow;

      exeProc = (LPEXEPROC)exeProcList->IdxGetNode(procIdx);
      procName = exeProc->GetName();
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
		if (exeLib->GetHLib() >= HINSTANCE_ERROR)
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
