/////////////////////////////////////////////////
////////    Window Object Methods         ///////
/////////////////////////////////////////////////
#include "w_includ.h"

////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_Startup(
  AEvtInfo*  							theEvent,
	AObjMessage*       			theSystem)
	{
	WINDOWRUNTIME*					wr;
	int											numWindows=0;
	LONG              			cookie = AOBJ_GETFIRSTKEY;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	while ( AObjGetNextObject(OTYPE_WINDOW, &cookie) != 0)
  	numWindows++;

	wr = new WINDOWRUNTIME(numWindows, FALSE);

  wr->InstallHooks();

  AEvtPostStandard(0L, OWND_EVT_WINDOWSTARTUP, 0L, AEVT_ATMARK);
  }

///////////////////////////////////////////

extern "C" void ALMAPI WndEvent_windowStartup(
  AEvtInfo*								theEvent,
  AObjMessage*      			theSystem)
  {
	OBJECTID          			oi;
  WINDOW*									wnd;
	LONG              			cookie = AOBJ_GETFIRSTKEY;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

  while ( (oi = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
		{
		wnd = LockWindowData(oi);
		I_VERIFY_POINTER(wnd, continue);
		wnd->Startup(oi);
    UnlockWindowData(oi);
		}

  return;
  }


////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_Shutdown(
	AEvtInfo*  							theEvent,
	AObjMessage*       			theSystem)
	{
  WINDOWRUNTIME*					wr = GetWindowRuntime();
	OBJECTID          			oi;
  WINDOW*									wnd;
	LONG              			cookie;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	//first, hid all but the active window
	cookie = AOBJ_GETFIRSTKEY;
  while ( (oi = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
    {
		if (!wr  ||  oi != wr->GetActiveWindowObject())  //if we can tell which is the active window
			{
			wnd = LockWindowData(oi);
			I_VERIFY_POINTER(wnd, return);
			wnd->Hide();
	    UnlockWindowData(oi);
			}
		}
	//second, hid the active window
	if (wr)
		{
		oi = wr->GetActiveWindowObject();	//store in temp, cause Hide will change the wr->m_oiActive
		if (oi)
			{
			wnd = LockWindowData(oi);
			I_VERIFY_POINTER(wnd, return);
			wnd->Hide();
	    UnlockWindowData(oi);
			}
		}


	//Now, shutdown
	cookie = AOBJ_GETFIRSTKEY;
  while ( (oi = AObjGetNextObject(OTYPE_WINDOW, &cookie)) != 0)
    {
		wnd = LockWindowData(oi);
		I_VERIFY_POINTER(wnd, return);
		wnd->Shutdown();
    UnlockWindowData(oi);
		}

	if (wr)
		{
		wr->RemoveHooks();
		delete wr;
		}
	I_VERIFY_POINTER(wr, ); //to get an error messare if wr==NULL
	}

////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_openWindowObject(
	AEvtInfo*  							theEvent,
	AObjMessage*       			theSystem)
	{
	WINDOW*									wnd;

	I_UNUSED(theSystem);

	wnd = LockWindowData(theEvent->targetObject);
	I_VERIFY_POINTER(wnd, return);
	wnd->Open();
	UnlockWindowData(theEvent->targetObject);

  return;

	}


//////////////////////////////////////////////////
///////////////////////////////////////////////////
////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_objectChanged(
  AEvtInfo*  							theEvent,
  AObjMessage*       			theSystem)
	{
	AObjOwnerInfo  					ownerInfo;
  OpWndItemD*							wi;

	I_UNUSED(theSystem);

  // Get each visual instance of the item that is in a window object
  ownerInfo.key = AOBJ_GETFIRSTKEY;
	while (AObjGetNextOwner(OTYPE_WINDOW, theEvent->targetObject, &ownerInfo))
		{
		wi = (OpWndItemD*)ownerInfo.ownerData;
		wi->DataChanged(theEvent->details, theEvent->modifiers, theEvent->variation);
    }

  return;
  }

///////////////////////////////////////////////////
///////////////////////////////////////////////////
//////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_closeWindowObject(
  AEvtInfo*								theEvent,
	pAObjMessage       			theSystem)
  {
	WINDOW*								  wnd = LockWindowData((OBJECTID)theEvent->details);

	I_UNUSED(theSystem);

	I_VERIFY_POINTER(wnd, return);

	wnd->Close();
	UnlockWindowData((OBJECTID)theEvent->details);
  return;

	}

////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_Print(
  AEvtInfo  							*theEvent,
  AObjMessage       			*theSystem)
	{
	WINDOW*								  wnd = LockWindowData(theEvent->targetObject);

	I_UNUSED(theSystem);

	I_VERIFY_POINTER(wnd, return);

	wnd->Print((LPPIDR_BLOCK)theEvent->details);
	UnlockWindowData(theEvent->targetObject);
  return;
	}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

extern "C" void ALMAPI WndEvent_CutCopyPasteUndo(
  AEvtInfo*  							theEvent,
	AObjMessage*       			theSystem)
	{
	WINDOWRUNTIME*					wr;
	OpWndItemD*							wiActive;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	wr = GetWindowRuntime();
	I_VERIFY_POINTER(wr, return);

	wiActive = wr->GetActiveItem();
	if (wiActive)
		{
		switch((int)theEvent->what)
			{
			case OGEN_EVT_CUT:	 wiActive->Cut();			break;
			case OGEN_EVT_COPY:	 wiActive->Copy();		break;
			case OGEN_EVT_PASTE: wiActive->Paste();		break;
			case OGEN_EVT_UNDO:	 wiActive->Undo();		break;
			}
		}
  }




