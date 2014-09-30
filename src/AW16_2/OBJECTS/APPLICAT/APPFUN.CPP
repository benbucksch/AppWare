//======================================================================
//   appfun.cpp
//
// Functions for Serius Application object
//
// jon 14-Aug-92 [1.00] rework of Shaili's first cut
// jon 11-Feb-93 [1.01] added appMouseUp
// jon 16-Feb-93 [1.01] added app<Get,Set>Name
//======================================================================

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_number.h>

#include "app.h"


//----------------------------------------------------------------------
//   appFunctionPriority
//
// Puts this serius app into function priority mode.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appFunctionPriority(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
	I_UNUSED(theEvent);

  // Unlike Serius/M 3.0, we do not have to have an application
  // object on the object list to change application priority mode.

  AAppSetPriorityMode(AAPP_FUNCTIONPRIORITY);
  AFuncReportResult(theSystem, 1, "");
  }

//----------------------------------------------------------------------
//   appVisualPriority
//
// Puts this serius app into visual priority mode.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appVisualPriority(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
	I_UNUSED(theEvent);

 // Unlike Serius/M 3.0, we do not have to have an application
 // object on the object list to change application priority mode.

  AAppSetPriorityMode(AAPP_VISUALPRIORITY);
  AFuncReportResult(theSystem, 1, "");
  }

//----------------------------------------------------------------------
//   appGetMouseDown
//
// Returns flowSignals indicating which mouse button is DOWN
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appGetMouseDown(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
	pAppRuntimeInfo		pRunInfo	= (pAppRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_APPLICATION);
  int								flowID		= 0;

	I_UNUSED(theEvent);

	if (pRunInfo->mouseInfo & APP_LEFTBUTTON)
		flowID = 1;
	else if (pRunInfo->mouseInfo & APP_RIGHTBUTTON)
		flowID = 2;
	else if (pRunInfo->mouseInfo & APP_MIDDLEBUTTON)
		flowID = 3;
	else
		flowID = 4;

  AFuncReportResult(theSystem, flowID, "");
  }

//----------------------------------------------------------------------
//   appStartIdle
//
// Executes the function chain attached to its "At each idle" signal
//   whenever the specified number of ticks has passed.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appStartIdle(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
  char  errBuf[sizeof(theSystem->errorMessage)] = "";
  long  flowOut = -1;

  if (theEvent->modifiers)
    {
    // This idler is already started
    flowOut = 2;
    }
  else
    {
    // -- Valid number as input param?
    OBJECTID  numObject = AFuncGetTypedParameter(1, OTYPE_NUMBER);
    if (!numObject)
      {
      OBJECTID  objID;
      TYPEID    typeID;

      objID = AFuncGetParameter(1);
		if (objID)
        typeID = AObjGetType(objID);
      else
        typeID = 0;

      if (!typeID)
        typeID = MAKETYPEID('?','?','?','?');

      wsprintf(errBuf, "Start Idle function can't convert a '%c%c%c%c' "
                       "to a Number object...",
                       ((LPBYTE)&typeID)[3],
                       ((LPBYTE)&typeID)[2],
                       ((LPBYTE)&typeID)[1],
                       ((LPBYTE)&typeID)[0]);

      flowOut = -1;
			theEvent->modifiers = 0;
      }
    else
      {
			theEvent->modifiers = ONmbrGetInteger(numObject);
      if (theEvent->modifiers == 0)
        theEvent->modifiers = -1;
		flowOut = 1;
      }
    }

  if (flowOut > 0)
    {
    theEvent->when = AAppGetTime() + theEvent->modifiers;
    theEvent->how  = AEVT_ATTAIL;
    AEvtPost(theEvent);
    }

  AFuncReportResult(theSystem, flowOut, errBuf);
  return;
  }

//----------------------------------------------------------------------
//   appStopIdle
//
// Stops all idlers set up for a particular time interval.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appStopIdle(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
  // -- Valid number as input param?
  OBJECTID  numObject = AFuncGetTypedParameter(1, OTYPE_NUMBER);
  if (!numObject)
    {
    OBJECTID  objID;
    TYPEID    typeID;
    char      errBuf[sizeof(theSystem->errorMessage)];

    objID = AFuncGetParameter(1);
    if (objID)
      typeID = AObjGetType(objID);
    else
      typeID = 0;

    if (!typeID)
      typeID = MAKETYPEID('?','?','?','?');

    wsprintf(errBuf, "Stop Idle function can't convert a "
                     "'%c%c%c%c' to a Number object...",
                     ((LPBYTE)&typeID)[3],
							((LPBYTE)&typeID)[2],
                     ((LPBYTE)&typeID)[1],
                     ((LPBYTE)&typeID)[0]);


    AFuncReportResult(theSystem, -1, errBuf);
    return;
    }

	theEvent->modifiers = ONmbrGetInteger(numObject);

  theEvent->how = AEVT_KILLWHAT | AEVT_KILLMODIFIERS | AEVT_KILLALLQUEUES | AEVT_KILLSELF;

  AEvtPost(theEvent);

  AFuncReportResult(theSystem, 1, "");
  return;
  }

//----------------------------------------------------------------------
//   appUpdateWindows
//
// Forces an immediate update of all windows in the serius app.
//----------------------------------------------------------------------

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //   appUpdateThisWindow
  //
  // The callback for EnumTaskWindows() invoked in appUpdateWindows.
  // Uses oipEmulateOSEvent() to emulate the posting to the serius
  // queue of a WM_PAINT message for the specified window, if it has
  // an invalid region.
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  static BOOL ALMCALLBACK	 appUpdateThisWindow(
    HWND		hWnd,
    LPARAM	)
    {
		UpdateWindow(hWnd);
    return TRUE;
    }

extern "C"
void ALMCALLBACK	 appUpdateWindows(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
	I_UNUSED(theEvent);

  EnumTaskWindows(GetCurrentTask(), appUpdateThisWindow, 0);
  AFuncReportResult(theSystem, 1, "");
  }

//----------------------------------------------------------------------
//   appMouseUp
//
// Pretend that the mouseup for the last mousedown came in.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appMouseUp(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
	pAppRuntimeInfo		pRunInfo				= (pAppRuntimeInfo) ATypeGetRuntimeInfo(OTYPE_APPLICATION);
  long							lastInputInfo;
 	MSG								emulatedMsg;
  POINT							point;

	I_UNUSED(theEvent);

  lastInputInfo = pRunInfo->lastMouse;


  // set the message accoring to what button was last down, if none then
  // default to the left button.

  if (lastInputInfo & APP_LEFTBUTTON)
    emulatedMsg.message = WM_LBUTTONUP;
  else if (lastInputInfo & APP_RIGHTBUTTON)
    emulatedMsg.message = WM_RBUTTONUP;
  else if (lastInputInfo & APP_MIDDLEBUTTON)
    emulatedMsg.message = WM_MBUTTONUP;
  else
    emulatedMsg.message = WM_LBUTTONUP;

  // set the target window

  emulatedMsg.hwnd = GetCapture();
  if (emulatedMsg.hwnd == 0)
    emulatedMsg.hwnd = GetActiveWindow();

  //set wParam to indicate what special keys were down.

  emulatedMsg.wParam = 0;

//!! do we need to do this?
//  if (lastInputInfo & isCtrl)
//    emulatedMsg.wParam &= MK_CONTROL;

//  if (lastInputInfo & isShift)
//	  emulatedMsg.wParam &= MK_SHIFT;

  //set lParam to contain the coordinates of the current cursor postion

  GetCursorPos(&point);

  emulatedMsg.lParam = MAKELONG(point.x, point.y);

	PostMessage(emulatedMsg.hwnd, emulatedMsg.message,
							emulatedMsg.wParam, emulatedMsg.lParam);

  AFuncReportResult(theSystem, 1, "");
  }


//----------------------------------------------------------------------
//   appGetName
//
// Get the application's name as it shows up in the task list.  
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appGetName(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
  OBJECTID    aText;
  MEMBLOCKID  aBlock;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;

  aText = AObjCreate(OTYPE_TEXT);
	if (!aText)
		{
		lstrcpy(theSystem->errorMessage, "[Appl:GetName] Unable to create Text for application name.");
		return;
		}

  aBlock = AMemAllocate(0);
	if (!aBlock)
		{
		lstrcpy(theSystem->errorMessage, "[Appl:GetName] Unable to create memblock for application name.");
		return;
		}

  if (!AAppGetName(aBlock))
		{
		lstrcpy(theSystem->errorMessage, "[Appl:GetName] Error in AAppGetName.");
		return;
		}

  AObjConvertFromText(aText, aBlock);
	AFuncSetParameter(1, aText);
  }

//----------------------------------------------------------------------
//   appGetCmdLine
//
// Get the application's command line  
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	appGetCmdLine(
  pAEvtInfo		 	theEvent,
  pAObjMessage	 	theSystem)

  {
  OBJECTID 	  		textObject;
  char		  		buffer[256];

  I_UNUSED(theEvent);

  theSystem->message1 = 1;

  textObject = AObjCreate(OTYPE_TEXT);	
  if (textObject == NULL)
    {
    ADBG_PRINT_E("Get Command Line: Unable to create Text Object.");
    }
  else
    {
	AAppGetCmdLine(buffer, sizeof(buffer));
	OTextSetValueFromBuffer(textObject, buffer);
	AFuncSetParameter(1, textObject);
    }
  AFuncReportResult(theSystem, 1, "");
  }


//----------------------------------------------------------------------
//   appSetName
//
// Set the application's name as it shows up in the task list.  
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appSetName(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
  {
  OBJECTID    aText;
  MEMBLOCKID  aBlock;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;

  aText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (!aText)
		{
    AFuncReportResult(
      theSystem, 2,
      "[Appl:SetName] Unable to fetch application name Text object.");
		return;
		}

  aBlock = AMemAllocate(0);
	if (!aBlock)
		{
		AFuncReportResult(
      theSystem, 2,
		  "[Appl:SetName] Unable to create memblock for application name.");
		return;
		}

  AObjConvertToText(aText, aBlock);
  if (!AAppSetName(aBlock))
		{
    AFuncReportResult(
			theSystem, 2,
			"[Appl:SetName] Error in AAppSetName.");
		return;
		}
	}

//----------------------------------------------------------------------
//   appSavePersistentData
//
// Puts this serius app into function priority mode.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	 appSavePersistentData(
  pAEvtInfo					theEvent,
  pAObjMessage			theSystem
	)
	{

	I_UNUSED(theEvent);

	// Unlike Serius/M 3.0, we do not have to have an application
	// object on the object list to change application priority mode.

	if (AAppSave() == TRUE)
		AFuncReportResult(theSystem, 1, "");
	else
    AFuncReportResult(theSystem, 2, "");
  }
