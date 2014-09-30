
#include "w_includ.h"

typedef long (pascal far * EDITLAYOUTPROC)(OBJECTID oiWindow);
static LONG EditWindowObject(OBJECTID oiWindow);


typedef OpWndItemD* (pascal far *ITEMFROMUID)(HWND hwnd, long UID);

ITEMFROMUID     g_lpfnItemFromUID = NULL;
EDITLAYOUTPROC  g_lpfnEditWnd     = NULL;
HINSTANCE       g_hinstLayout     = NULL;
int             g_nLayoutUseCount = 0;


extern "C" LONG ALMCALLBACK WndAdminProc(
  OBJECTID                oiWindow,
  AObjMessage*            theSystem)
  {
  WINDOW*                 wnd;
  LONG                    rVal = A_NOTHANDLED;

  switch(theSystem->message1)
    {
    case AOBJ_AWAKENED:
      #ifdef AW_I_DEBUG
      g_nLayoutUseCount++;
      if (!g_hinstLayout)
        {
        I_ASSERT(g_nLayoutUseCount==1);
        g_hinstLayout = LoadLibrary("ALM_WLAY.DLL");
        ADBG_PRINT_I("Preloading Layout Editor");
        }
      #endif
      rVal = A_OK;
      break;

    case AOBJ_CREATED:
      #ifdef AW_I_DEBUG
      g_nLayoutUseCount++;
      if (!g_hinstLayout)
        {
        I_ASSERT(g_nLayoutUseCount==1);
        g_hinstLayout = LoadLibrary("ALM_WLAY.DLL");
        ADBG_PRINT_I("Preloading Layout Editor");
        }
      #endif

      wnd = new(oiWindow) WINDOW(oiWindow);  //The locks the data
      delete wnd;  //Just unlocks the data -- DOES NOT DELETE THE DATAS!!!
      rVal = A_OK;
      break;

    case AOBJ_DESTROYED:
    case AOBJ_ASLEEP:
      #ifdef AW_I_DEBUG
      g_nLayoutUseCount--;
      if(g_nLayoutUseCount==0)
        {
        FreeLibrary(g_hinstLayout);
        g_hinstLayout = NULL;
        g_lpfnEditWnd = NULL;
        g_lpfnItemFromUID = NULL;
        ADBG_PRINT_I("Unloading Layout Editor");
        }
      #endif
      rVal = A_CONTINUE;
      break;

    case AOBJ_ASSIGNOBJECT:
      //Assignment of a window object is not yet defined
      break;

    case AOBJ_PREWRITE:
      if (theSystem->message4 != /*AOBJ_PROJECTSAVE*/2)
        {
        wnd = LockWindowData(oiWindow);
        I_VERIFY_POINTER(wnd, break);
        if(wnd->PutItemsIntoDatas())
          rVal = A_CONTINUE;
        else
          rVal = A_NOTHANDLED;
        UnlockWindowData(oiWindow);
        }
      else
        rVal = A_NOTHANDLED;
      break;

    case AOBJ_EDITOBJECT:
      rVal = EditWindowObject(oiWindow);
      break;

    case AOBJ_VALIDATEOBJECTIDS:
      wnd = (WINDOW*)AObjLockData(oiWindow, I_DATA_WINDOW); //Don't call LockWindowData cause it checks m_oiSelf(soon to be fixed)
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->ValidateObjectIDs(oiWindow, theSystem);
      AObjUnlockData(oiWindow, I_DATA_WINDOW);
      break;

    case AOBJ_READ:
      AObjResizeData(oiWindow, I_DATA_WINDOW, sizeof(WINDOW) );
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->Read();
      UnlockWindowData(oiWindow);
      break;

    case AOBJ_CHECKOBJECT:
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->CheckObject();
      UnlockWindowData(oiWindow);
      break;

    case AOBJ_BUILDSIGNALS:
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->BuildSignals((MEMBLOCKID)theSystem->message2);
      UnlockWindowData(oiWindow);
      break;

    case AOBJ_IMPORTOBJECT:
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->Import();
      UnlockWindowData(oiWindow);
      break;

    case AOBJ_EXPORTOBJECT:
      wnd = LockWindowData(oiWindow);
      I_VERIFY_POINTER(wnd, break);
      rVal = wnd->Export();
      UnlockWindowData(oiWindow);
      break;

    case AOBJ_REPORTREQUIREDFILES:
      AObjAddRequiredFile(theSystem, "ALM_SHAP.DLL");
      rVal = A_OK;
      break;
    }

  return rVal;
  }

////////////////////////////////////////

static LONG EditWindowObject(
  OBJECTID                oiWindow)
  {
  char                    dllName[15];
  char                    tmpStr[100];
  LONG                    rVal;
  WINDOWRUNTIME*          wr;

  wr = new WINDOWRUNTIME(1, TRUE);

  #ifdef AW_I_DEBUG
  // no reason to load the library....
  I_ASSERT(g_hinstLayout);

  if(!g_lpfnEditWnd)
    g_lpfnEditWnd = (EDITLAYOUTPROC)GetProcAddress(g_hinstLayout, "OWndEditLayout");

  if (!g_lpfnEditWnd)
    {
    wsprintf(tmpStr, "Unable to find the EditLayout function in %s -- Unable to edit the window object", dllName);
    MessageBox(NULL, tmpStr, "Error", MB_OK);
    rVal = A_ERROR;
    }
  else
    rVal = g_lpfnEditWnd(oiWindow);
  #else
  GetPrivateProfileString("Window Object", "EditLayoutDLL", "ALM_WLAY.DLL", dllName, 14, "appware.ini");

  g_nLayoutUseCount++;
  if(!g_hinstLayout)
    {
    I_ASSERT(g_nLayoutUseCount==1);
    g_hinstLayout = LoadLibrary(dllName);
    }

  if ((UINT) g_hinstLayout < 32)
    {
    wsprintf(tmpStr, "Unable to load %s -- Unable to edit the window object", dllName);
    MessageBox(NULL, tmpStr, "Error", MB_OK);
    g_nLayoutUseCount--;
    rVal = A_ERROR;
    }
  else
    {
    g_lpfnEditWnd = (EDITLAYOUTPROC)GetProcAddress(g_hinstLayout, "OWndEditLayout");
    if (!g_lpfnEditWnd)
      {
      wsprintf(tmpStr, "Unable to find the EditLayout function in %s -- Unable to edit the window object", dllName);
      MessageBox(NULL, tmpStr, "Error", MB_OK);
      rVal = A_ERROR;
      }
    else
      {
      rVal = g_lpfnEditWnd(oiWindow);
      }
    g_nLayoutUseCount--;
    if(g_nLayoutUseCount==0)
      {
      FreeLibrary(g_hinstLayout);
      g_hinstLayout = NULL;
      g_lpfnEditWnd = NULL;
      g_lpfnItemFromUID = NULL;
      }
    }
  #endif

  delete wr;

  return rVal;
  }
