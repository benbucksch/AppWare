
//////////////////////////////////////////
/// This file contains the member functions
///  for the class: WINDOWRUNTIME
//////////////////////////////////////////////

#include "w_includ.h"

//////////////////////////////////////////////
//////////////////////////////////////////////


WINDOWRUNTIME::WINDOWRUNTIME(
  int           numWindows,
  BOOL          bLayoutTime)
  {
  m_oiActive = m_oiModal = 0;
  m_wiCapture =  m_wiMouseIn = NULL;

  m_bAppIsActive = FALSE;
  m_hCursor = m_hCursorMaster = LoadCursor(NULL, IDC_ARROW);
  m_bArrowCursor = TRUE;
  m_bIsColor = TRUE;
  m_numWindowsTotal = numWindows;
  m_numVisibleWindows = 0; //Nothing is visible yet

  m_bIsLayoutTime = bLayoutTime;


  m_miZorder = AMemAllocate(sizeof(OBJECTID)*numWindows);
  m_oiptrZorder = (OBJECTID*) AMemLock(m_miZorder);         //Left locked until the destructor
  _fmemset(m_oiptrZorder, 0, sizeof(OBJECTID)*numWindows);
  m_bZorderDone = FALSE;

  m_miShapeDlls = AMemAllocate(0);

  ATypeSetRuntimeInfo (OTYPE_WINDOW, (LONG)this);
  }

//////////////////////////////////////////////

WINDOWRUNTIME::~WINDOWRUNTIME()
  {

  FreeShapeLibrarys(); //Will free all shape dlls

  AMemUnlock(m_miZorder);
  AMemFree(m_miZorder);

  AMemFree(m_miShapeDlls);

  ATypeSetRuntimeInfo (OTYPE_WINDOW, 0);
  }

//////////////////////////////////////////////

HWND WINDOWRUNTIME::GetActiveWindowObjectHandle()
	{
	HWND										hwndActive = NULL;
	WINDOW*									wnd;

	if (m_oiActive  &&  (wnd = LockWindowData(m_oiActive)) != NULL)
		{
		hwndActive = wnd->GetWindowHandle();
		UnlockWindowData(m_oiActive);
		}

	return hwndActive;
	}

//////////////////////////////////////////////

OpWndItemD* WINDOWRUNTIME::SetCaptureItem(
  OpWndItemD*             wi)
  {
  OpWndItemD*             prevCaptureItem = m_wiCapture;
  HWND                    targetCaptureWindow;

  if (wi->m_status & I_SF_LITEITEM)
    targetCaptureWindow = wi->m_hwndParent;
  else if (wi->m_status & I_SF_HWNDITEM)
    targetCaptureWindow = ((OpHwndWndItem*)wi)->m_hwndItem;
  else
    ADBG_EVAL( ADbgPrintRes(ADBG_ERROR, wi->Type(), g_hInstance, ERR_UNKNOWN_WI_TYPE, "OWndSetCaptureItem") );

  SetCapture(targetCaptureWindow);
  if (GetCapture() != targetCaptureWindow)
    {
    ADBG_EVAL( ADbgPrintRes(ADBG_WARNING, wi->Type(), g_hInstance, WARN_CAPTUREFAILED) );
    m_wiCapture = NULL;  //Capture FAILED
    }
  else
    m_wiCapture = wi;

  return prevCaptureItem;
  }

//////////////////////////////////////////////

void WINDOWRUNTIME::ReleaseCapture()
  {
  ::ReleaseCapture();
  m_wiCapture = NULL;
  }

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

HCURSOR WINDOWRUNTIME::SetCursor(
  HCURSOR                 hCursor,
  BOOL                    master)
  {
  HCURSOR                 rVal;

  rVal = m_hCursor;

  if (hCursor != m_hCursor)
    {
    m_hCursor = hCursor;
    m_bArrowCursor = (m_hCursor == LoadCursor(NULL, IDC_ARROW) );
    }

  if (master)
    m_hCursorMaster = m_hCursor;

  return rVal;
  }

/////////////////////////////////////

HCURSOR WINDOWRUNTIME::ReleaseCursor()
  {

  if (m_hCursor != m_hCursorMaster)
    {
    m_hCursor = m_hCursorMaster;
    m_bArrowCursor = (m_hCursor == LoadCursor(NULL, IDC_ARROW) );
    }
  return m_hCursor; 
  }

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

BOOL WINDOWRUNTIME::ActivateWindow(
  OBJECTID                oi)
  {
  WINDOW*                 wnd;
  BOOL                    rVal;

  wnd = LockWindowData(oi);
  I_VERIFY_POINTER(wnd, return FALSE);

  if ( (m_oiModal && !wnd->IsModal()) || !wnd->IsWindowOpen() )
    rVal = FALSE; //Prevented by a modal window .. or .. the window is not opened
  else
    {
    wnd->BringToTop();
    rVal = TRUE;
    }

  UnlockWindowData(oi);
  return rVal;
  }

/////////////////////////////////////////
/////////////////////////////////////////
OpWndItemD* WINDOWRUNTIME::GetActiveItem()   //the active in the active window
  {
  OpWndItemD*             rVal;
  OBJECTID                activeID = GetActiveWindowObject();
  WINDOW*                 wnd;

  if(!activeID)  //No active window
    return NULL; 

  wnd = LockWindowData(activeID);
  I_VERIFY_POINTER(wnd, return FALSE);

  rVal = wnd->GetActiveItem();
  UnlockWindowData(activeID);
  return rVal;
  }

////////////////////////////

BOOL WINDOWRUNTIME::IsHwndAWindowObject(
  HWND                    hwnd)
  {
  char                    className[100];

  GetClassName(hwnd, className, 99);
  if (lstrcmp(className, g_windowObjectClassName)==0)
    return TRUE;
  else
    return FALSE;
  }

//////////////////

OBJECTID WINDOWRUNTIME::WindowObjectFromPoint(
  POINT                   pt) //Point is in screen coords
  {
  WINDOW*                 wnd;
  OBJECTID                oi, rVal;
  int                     i;

  if (!m_bZorderDone)
    return 0L;

  for(rVal=0,i=0; i<m_numVisibleWindows && !rVal; i++)
    {
		oi = m_oiptrZorder[i];
    wnd = LockWindowData(oi);
    I_VERIFY_POINTER(wnd, continue);

    if (wnd->PtInWindow(pt))
      rVal = m_oiptrZorder[i];
    UnlockWindowData(oi);
    }

  return rVal;
  }

///////////////////////////////////////////////

OBJECTID WINDOWRUNTIME::GetWindowObjectFromHwnd(
  HWND                    hwnd)
  {
  WINDOW*                 wnd;
  OBJECTID                oi, rVal;
  int                     i;

  if (!m_bZorderDone)
    return 0L;

  I_VERIFY_WINDOW(hwnd, return 0);

  for(rVal=0,i=0; i<m_numVisibleWindows && !rVal; i++)
    {
		oi = m_oiptrZorder[i];
    wnd = LockWindowData(oi);
    I_VERIFY_POINTER(wnd, continue);
    if (hwnd == wnd->GetWindowHandle())
      rVal = m_oiptrZorder[i];
    UnlockWindowData(oi);
    }

  return rVal;
  }

//////////////////////////////////

OpLiteWndItemD* WINDOWRUNTIME::GetLiteItemFromPointOrCapture(
  HWND                    hwnd,
  POINT                   pt)
  {
  OpLiteWndItemD*         lwi;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;

  if (m_wiCapture)
    {
		if (WINDOWAPI::IsLiteItem(m_wiCapture))
			return (OpLiteWndItemD*)m_wiCapture;
		else
			return NULL;
    }
    
  oiWindow = GetWindowObjectFromHwnd(hwnd);
  if (oiWindow)
    {
    wnd = LockWindowData(oiWindow);
    I_VERIFY_POINTER(wnd, return NULL);

    lwi = (OpLiteWndItemD*)wnd->ItemFromLogicalPoint(pt);
    UnlockWindowData(oiWindow);

    if (lwi && WINDOWAPI::IsLiteItem(lwi))
      return lwi;
    }

  return NULL;
  }

///////////////////////////////////////////////////

OpHwndWndItemD* WINDOWRUNTIME::GetItemFromHwnd(
  HWND                    hwndItem)
  {
  HWND                    hwndParent;
  OBJECTID                oiWindow;
  WINDOW*                 wnd;
  int                     controlID;
  OpHwndWndItemD*         hwi;

  do
    {
    hwndParent = GetParent(hwndItem);
    if (hwndParent  &&  (GetWindowLong(hwndItem, GWL_STYLE) & WS_CHILD) )
      {
      oiWindow = GetWindowObjectFromHwnd(hwndParent);
      if (oiWindow)
        {
        wnd = LockWindowData(oiWindow);
        I_VERIFY_POINTER(wnd, return NULL);

        controlID = GetWindowWord(hwndItem, GWW_ID);
        hwi = wnd->GetItemByControlID(controlID);
        UnlockWindowData(oiWindow);

        return hwi;
        }
      }
    }while ( (hwndItem=GetParent(hwndItem)) != 0);

  return NULL;
  }


////////////////////////////////////////////////////
///  Set the z-order list /////////////
////////////////////////////////////////////////////
void WINDOWRUNTIME::SetZOrderList()
  {
	HWND										hwnd;
  OBJECTID                oiWindow;

  memset(m_oiptrZorder, 0, sizeof(OBJECTID)*m_numWindowsTotal);
  m_bZorderDone = FALSE;  //Prevent hooks until done
  m_numVisibleWindows = 0;  

	hwnd = GetTopWindow(NULL);
	while(hwnd)
		{
    if (GetCurrentTask() == GetWindowTask(hwnd)  && IsWindowVisible(hwnd)  &&  IsHwndAWindowObject(hwnd))
			{
  		oiWindow = GetIDfromExtraBytes(hwnd);
  		I_VERIFY_TYPEID(oiWindow, OTYPE_WINDOW, break);

  		m_oiptrZorder[m_numVisibleWindows] = oiWindow;
  		m_numVisibleWindows++;
			}
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
		}

  m_bZorderDone = TRUE;
  }


////////////////////////////////////////

PWICreateFN WINDOWRUNTIME::GetShapeCreateAndAddToList(    //Will load the dll if it is not there
  LPSTR                   szDllName,
  TYPEID                  tiShape)
  {
  int                     numTypes, i;
  LPSHAPE_DLLS            shapeDlls;
  OShapeAdmin             shapeAdmin;
  PWICreateFN             shapeCreate;
  HINSTANCE               hInstDll;
  AObjMessage             adminMessage;

  numTypes = (int)AMemGetSize(m_miShapeDlls)/sizeof(SHAPE_DLLS);
  shapeAdmin = NULL;

  if (numTypes)
    {
    shapeDlls = (LPSHAPE_DLLS)AMemLock(m_miShapeDlls);
    I_VERIFY_POINTER(shapeDlls, return NULL);

    for(i=0; i<numTypes && !shapeAdmin; i++)
      {
      if (shapeDlls[i].tiShape == tiShape)
        shapeAdmin = shapeDlls[i].shapeAdminProc;
      }
    AMemUnlock(m_miShapeDlls);
    }

  if (!shapeAdmin)   //The shape dll is NOT yet loaded.....yet is the key word
    {
    hInstDll = LoadLibrary(szDllName);
    if (hInstDll)  
      { //The dll is now sucessfully loaded (Hip, Hip, Hurray)
      if ( (shapeAdmin = (OShapeAdmin) GetProcAddress(hInstDll, "SHAPELIBADMIN")) != 0)
        {//And the admin entry point was found
        numTypes++;
        AMemSetSize(m_miShapeDlls, numTypes*sizeof(SHAPE_DLLS));
        shapeDlls = (LPSHAPE_DLLS)AMemLock(m_miShapeDlls);
        I_VERIFY_POINTER(shapeDlls, ;);

        shapeDlls[numTypes-1].tiShape = tiShape;
        lstrcpy(shapeDlls[numTypes-1].szDllName, szDllName);
        shapeDlls[numTypes-1].hInstShape = hInstDll;
        shapeDlls[numTypes-1].shapeAdminProc = shapeAdmin;
        AMemUnlock(m_miShapeDlls);
        }
      else
        {
        FreeLibrary(hInstDll); //The entry point was not found
        ADBG_EVAL(ADbgPrintRes(ADBG_ERROR, 0, g_hInstance, ERR_SHAPE_LIB_ADMIN));
        }
      }
    }

  //At this point, the shape library will be loaded if the admin was found

  if (shapeAdmin)
    {
    _fmemset(&adminMessage, 0, sizeof(AObjMessage) );
    shapeAdmin(OSHP_QUERYLIBINFO, &adminMessage);
    shapeCreate = (PWICreateFN)adminMessage.message1;
    }
  else
    shapeCreate = NULL;

  return shapeCreate;
  }  

////////////////////////////////////////

void WINDOWRUNTIME::FreeShapeLibrarys() //Will free all shape dlls
  {
  int                     numTypes, i;
  LPSHAPE_DLLS            shapeDlls;
  
  numTypes = (int)AMemGetSize(m_miShapeDlls)/sizeof(SHAPE_DLLS);

  if (numTypes)
    {
    shapeDlls = (LPSHAPE_DLLS)AMemLock(m_miShapeDlls);
    I_VERIFY_POINTER(shapeDlls, return);

    for(i=0; i<numTypes; i++)
      {
      FreeLibrary(shapeDlls[i].hInstShape);
      shapeDlls[i].hInstShape = 0;
      shapeDlls[i].shapeAdminProc = 0;
      }

    AMemUnlock(m_miShapeDlls);
    }

  }

////////////////////////////////////////

BOOL WINDOWRUNTIME::GetShapeLibraryName(//Returns TRUE if type is found
  TYPEID                  tiShape,
  LPSTR                   szName)  //must be 13 chars long
  {
  BOOL                    rVal = FALSE;
  int                     numTypes, i;
  LPSHAPE_DLLS            shapeDlls;
  
  numTypes = (int) AMemGetSize(m_miShapeDlls)/sizeof(SHAPE_DLLS);

  shapeDlls = (LPSHAPE_DLLS)AMemLock(m_miShapeDlls);
  I_VERIFY_POINTER(shapeDlls, return FALSE);

  for(i=0; i<numTypes && !rVal; i++)
    {
    if (tiShape == shapeDlls[i].tiShape)
      {
      lstrcpy(szName, shapeDlls[i].szDllName);
      rVal = TRUE;
      }
    }

  return rVal;
  }

////////////////////////////////////////

OShapeAdmin WINDOWRUNTIME::GetShapeAdmin(
  TYPEID                  tiShape)
  {
  int                     numTypes    = (int)AMemGetSize(m_miShapeDlls)/sizeof(SHAPE_DLLS);
  OShapeAdmin             shapeAdmin  = NULL;

  if (numTypes)
    {
    int                     i;
    LPSHAPE_DLLS            shapeDlls;

    shapeDlls = (LPSHAPE_DLLS)AMemLock(m_miShapeDlls);
    I_VERIFY_POINTER(shapeDlls, return NULL);

    for(i=0; i<numTypes && !shapeAdmin; i++)
      {
      if (shapeDlls[i].tiShape == tiShape)
        shapeAdmin = shapeDlls[i].shapeAdminProc;
      }
    AMemUnlock(m_miShapeDlls);
    }

  return shapeAdmin;
  }  

////////////////////////////////////////
