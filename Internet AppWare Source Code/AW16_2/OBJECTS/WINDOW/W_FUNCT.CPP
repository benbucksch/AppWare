////////////////////////////////////////////////////////
/////////  Window Object FUNctions   ////////

#include "w_includ.h"


#define OTYPE_ANYTHING		0

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

extern "C" void ALMAPI WndFunction_ActivateItem(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow;
	OBJECTID								oiItem;
	WINDOW*									wnd;
	OpWndItemD*							wi;

  I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);


	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	if (oiItem)
		wi = wnd->GetItemByObjectID(oiItem);
	else
		wi = wnd->GetNextTabstop();

	if (wi)
		wnd->SetActiveItem(wi, OWND_ACTIVATED_BY_OTHER, TRUE);

	UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_ActiveItem(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiActive;
  WINDOW*									wnd;
  OpWndItemD*							wiActive;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wiActive = wnd->GetActiveItem();
	if (wiActive)
		oiActive = wiActive->Object();
	else
  	oiActive = 0L;

	AFuncSetParameter(1, oiActive);

	UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_CancelClose(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	AEvtPostStandard(0, OWND_EVT_CLOSEWINDOWOBJECT, 0, AEVT_KILLSIMILARS|AEVT_KILLSELF);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_CenterWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow;
	WINDOW*									wnd;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wnd->Center( AFuncGetTypedParameter(2, OTYPE_WINDOW) );

  UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_CloseWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow;
	WINDOW*									wnd;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	if (wnd->GetWindowHandle())
		AEvtPostStandard(oiWindow, OWND_EVT_CLOSEWINDOWOBJECT, (long)oiWindow, AEVT_ATMARK);

  UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_FrontWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	WINDOWRUNTIME*					wr;
	OBJECTID								oiActive;

	I_UNUSED(theEvent);

	wr = GetWindowRuntime();
  I_VERIFY_POINTER(wr, return);

	oiActive = wr->GetActiveWindowObject();

	if (oiActive)
    {
	  AFuncSetParameter(1, oiActive);
    theSystem->message1 = 1;
    }
  else
    {
		theSystem->message1 = 2;
    }
	}

//////////////////////

extern "C" void ALMAPI WndFunction_GetTitle(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiText;
	WINDOW*									wnd;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiText = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiText, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	OTextSetValueFromBuffer(oiText, wnd->GetTitle() );

	UnlockWindowData(oiWindow);

	AFuncSetParameter(1, oiText);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_HideItem(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
  OpWndItemD*							wi;
	OBJECTID								oiWindow, oiItem;
	WINDOW*									wnd;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiItem, 2, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	if ( (wi=wnd->GetItemByObjectID(oiItem)) != NULL)
		OWndHideItem(wi);


	UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_IsWindowOpen(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow;
	WINDOW*									wnd;

	I_UNUSED(theEvent);

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	if (wnd->GetWindowHandle())				theSystem->message1 = 1;
	else                              theSystem->message1 = 2;

  UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_MoveWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiLeft, oiTop;
  WINDOW*									wnd;
	int											left, top, width, height;
	RECT										rect;
	WINDOWPLACEMENT					wp;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	rect = wnd->GetCurrentWindowRect();
	left = rect.left;
	top =  rect.top;
	width = (rect.right - rect.left);
	height = (rect.bottom - rect.top);

	oiLeft = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	if (oiLeft)
		left = (int)ONmbrGetInteger(oiLeft);

	oiTop = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	if (oiTop)
		top = (int)ONmbrGetInteger(oiTop);

	if (wnd->GetWindowHandle())
		{
		wp.length = sizeof(WINDOWPLACEMENT);
		wp.flags = 0;
		wp.showCmd = SW_SHOWNOACTIVATE;
		wp.ptMaxPosition.x = wp.ptMaxPosition.y = 0;
		SetRect(&wp.rcNormalPosition, left, top, left+width, top+height);
		SetWindowPlacement(wnd->GetWindowHandle(), &wp);
		}
	
	wnd->SetCurrentWindowRect(left, top, left+width, top+height); //store the new position for upcomming resizes

  UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_OpenWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow;
	WINDOWRUNTIME*					wr;
  WINDOW*									wnd;

	I_UNUSED(theEvent);

	theSystem->message1 = 2;		//Default to 'Failed'

	wr = GetWindowRuntime();
  I_VERIFY_POINTER(wr, return);

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	//we will open if no modal window, or opening a new modal window
 	if ( (wnd->IsModal() && !wnd->IsWindowOpen())||  !wr->GetModalWindowObject()  ||  wr->GetModalWindowObject() == oiWindow)
	 	{
	  AEvtPostStandard(oiWindow, OWND_EVT_OPENWINDOWOBJECT, 0L, AEVT_ATMARK);
		theSystem->message1 = 1;		//Can do
	 	}

	UnlockWindowData(oiWindow);

	}

//////////////////////

extern "C" void ALMAPI WndFunction_SetTitle(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiText;
	WINDOW*									wnd;
	MEMBLOCKID							miNewTitle;
	LPSTR										newTitle;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiText = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 2, return);

	miNewTitle = AMemAllocate(I_TITLE_LENGTH+1);
	I_VERIFY_MI(miNewTitle, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	OTextGetToMemblock(oiText, miNewTitle);

	newTitle = (LPSTR)AMemLock(miNewTitle);
	wnd->SetTitle(newTitle);

	UnlockWindowData(oiWindow);
	AMemUnlock(miNewTitle);
	AMemFree(miNewTitle);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_ShowItem(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OpWndItemD*							wi;
	OBJECTID								oiWindow, oiItem;
	WINDOW*									wnd;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiItem, 2, return);


	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	if ( (wi = wnd->GetItemByObjectID(oiItem)) != NULL)
		OWndShowItem(wi);

	UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_SizeWindow(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiWidth, oiHeight;
  WINDOW*									wnd;
	int											left, top, width, height;
	RECT										rect;
	WINDOWPLACEMENT				 	wp;
	POINT										ptChange;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	rect = wnd->GetCurrentWindowRect();
	left = rect.left;
	top = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	oiWidth = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	if (oiWidth)
		width = (int)ONmbrGetInteger(oiWidth);

	oiHeight = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	if (oiHeight)
		height = (int)ONmbrGetInteger(oiHeight);


	if (wnd->GetWindowHandle())
		{
		wp.length = sizeof(WINDOWPLACEMENT);
		wp.flags = 0;
		wp.showCmd = SW_SHOWNOACTIVATE;
		wp.ptMaxPosition.x = wp.ptMaxPosition.y = 0;
		SetRect(&wp.rcNormalPosition, left, top, left+width, top+height);
		SetWindowPlacement(wnd->GetWindowHandle(), &wp);
		}
	else
		{//cause if no handle, no WM_SIZE message, so need to resize items now
		ptChange.x = left + width - rect.right;
		ptChange.y = top + height - rect.bottom;
		wnd->WndDStoLS(&ptChange, 1); //ptChange now has the values in logical units
		rect = wnd->GetCurrentClientRect();
		rect.right += ptChange.x;
		rect.bottom += ptChange.y;
		wnd->ResizeWindowItems(&rect);
		wnd->SetCurrentClientRect(&rect);//Store the new current client rect
		}
	
	wnd->SetCurrentWindowRect(left, top, left+width, top+height); //store the new size for upcomming moves

	UnlockWindowData(oiWindow);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_MoveItem(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
  OBJECTID								oiWindow, oiItem, oiValue;
	WINDOW*									wnd;
	OpWndItemD*							wi;
  RECT										rect;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiWindow, 2, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wi = wnd->GetItemByObjectID(oiItem);
  UnlockWindowData(oiWindow);

	if (wi)
		{
		CopyRect(&rect, &wi->m_rcItem);
		OWndLStoDS(oiWindow, (LPPOINT)&rect, 2);

		if ( (oiValue = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
			rect.left = (int)ONmbrGetInteger(oiValue);
		if ( (oiValue = AFuncGetTypedParameter(4, OTYPE_NUMBER)) != 0)
			rect.top = (int)ONmbrGetInteger(oiValue);
		if ( (oiValue = AFuncGetTypedParameter(5, OTYPE_NUMBER)) != 0)
			rect.right = (int)ONmbrGetInteger(oiValue);
		if ( (oiValue = AFuncGetTypedParameter(6, OTYPE_NUMBER)) != 0)
			rect.bottom = (int)ONmbrGetInteger(oiValue);

		OWndDStoLS(oiWindow, (LPPOINT)&rect, 2);

		OWndMoveItem(wi, &rect);
		}

	}

//////////////////////

extern "C" void ALMAPI WndFunction_MouseLocation(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiOutput;
  HWND										hwndWindow;
	WINDOW*									wnd;
	POINT										pt;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	GetCursorPos(&pt);

	if ( (oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW)) != 0)
  	{
		wnd = LockWindowData(oiWindow);
		I_VERIFY_POINTER(wnd, return);
		hwndWindow = wnd->GetWindowHandle();
		I_VERIFY_WINDOW(hwndWindow, ;);
		ScreenToClient(hwndWindow, &pt);
		OWndDPtoLP(oiWindow, &pt, 1);		//This will add the scrolled amounts to the point
		OWndLStoDS(oiWindow, &pt, 1);		//
    UnlockWindowData(oiWindow);
		}

	oiOutput	= AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, (LONG)pt.x, TRUE);
	AFuncSetParameter(1, oiOutput);

	oiOutput = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, (LONG)pt.y, TRUE);
	AFuncSetParameter(2, oiOutput);
	}

//////////////////////

extern "C" void ALMAPI WndFunction_ItemLocation(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiItem, oiOutput;
	WINDOW*									wnd;
  OpWndItemD*							wi;
	RECT										rcTemp;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiWindow, 2, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wi = wnd->GetItemByObjectID(oiItem);

	UnlockWindowData(oiWindow);

	if (wi)
		{
		CopyRect(&rcTemp, &wi->m_rcItem);
		OWndLStoDS(wi->m_oiParent, (LPPOINT)&rcTemp, 2);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, rcTemp.left, TRUE);
		AFuncSetParameter(1, oiOutput);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, rcTemp.top, TRUE);
		AFuncSetParameter(2, oiOutput);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, rcTemp.right, TRUE);
		AFuncSetParameter(3, oiOutput);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, rcTemp.bottom, TRUE);
		AFuncSetParameter(4, oiOutput);
		}
	}

//////////////////////

extern "C" void ALMAPI WndFunction_GetSelection(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiItem, oiOutput;
	WINDOW*									wnd;
	OpWndItemD*							wi;
  LONG										start=0, count=0;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiWindow, 2, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wi = wnd->GetItemByObjectID(oiItem);
	UnlockWindowData(oiWindow);

	if(wi)
		{
		wi->GetSelection(&start, &count);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, start, TRUE);
		AFuncSetParameter(1, oiOutput);

		oiOutput = AObjCreate(OTYPE_NUMBER);
		I_VERIFY_PSUEDO(oiOutput, return);
		ONmbrSetInteger(oiOutput, count, TRUE);
		AFuncSetParameter(2, oiOutput);
		}

	}


//////////////////////
///////////////////////////////////
//////////////////////

extern "C" void ALMAPI WndFunction_SetSelection(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiItem, oiValue;
	LONG										start, count;
	WINDOW*									wnd;
  OpWndItemD*							wi;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;		//Only one flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	oiItem = AFuncGetTypedParameter(2, OTYPE_ANYTHING);
	I_VERIFY_PARAMETER(oiItem, 2, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	wi = wnd->GetItemByObjectID(oiItem);
	UnlockWindowData(oiWindow);

	if (wi)
		{
		oiValue = AFuncGetTypedParameter(3, OTYPE_NUMBER);
		I_VERIFY_PARAMETER(oiValue, 3, return);
		start = ONmbrGetInteger(oiValue); //This is a one-based index

		oiValue = AFuncGetTypedParameter(4, OTYPE_NUMBER);
		I_VERIFY_PARAMETER(oiValue, 4, return);
		count = ONmbrGetInteger(oiValue);

	  wi->SetSelection(start, count);
		}
	}

////////////////////////////

extern "C" void ALMAPI WndFunction_WindowPosition(
	pAEvtInfo 							theEvent,
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiWindow, oiOutput;
	WINDOW*									wnd;
  RECT										rect;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiWindow = AFuncGetTypedParameter(1, OTYPE_WINDOW);
	I_VERIFY_PARAMETER(oiWindow, 1, return);

	wnd = LockWindowData(oiWindow);
	I_VERIFY_POINTER(wnd, return);

	rect = wnd->GetCurrentWindowRect();
	UnlockWindowData(oiWindow);

	oiOutput = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, rect.left, TRUE);
	AFuncSetParameter(1, oiOutput);

	oiOutput = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, rect.top, TRUE);
	AFuncSetParameter(2, oiOutput);

	oiOutput = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, rect.right - rect.left, TRUE);
	AFuncSetParameter(3, oiOutput);

	oiOutput = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiOutput, return);
	ONmbrSetInteger(oiOutput, rect.bottom - rect.top, TRUE);
	AFuncSetParameter(4, oiOutput);
	}



