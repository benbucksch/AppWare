#include "almsys.h"

#include "memlib.h"
#include "shellsys.h"
#include "exeinfo.h"
#include "oipexe.h"
#include <ctl3d.h>
#include <a_almutl.h>

static char myPath[MAX_PATH];
static char myCmdLine[300];

/////////////////////////////////////////////////////////////////////////////
//   WinMain
//
/////////////////////////////////////////////////////////////////////////////
int WINAPI	WinMain(
  HINSTANCE		hInstance,
  HINSTANCE		hPrevInstance,
  LPSTR				lpCmdLine,
  int					cmdShow)
  {
  int           result;
	EXEINFOAUX		exeAux;
  EXEINFO       exeInfo(&exeAux);     // for IDM_RUN & IDM_CONTINUE

	if(AUtlGetOSVersion() == AUTL_OS_WINNT)
  	{
    Ctl3dRegister(hInstance);
    Ctl3dAutoSubclass(hInstance);
    }
	
  SHELLSYSTEM::Initialize(hInstance, hPrevInstance, lpCmdLine, cmdShow);
  GetModuleFileName(hInstance, myPath, MAX_PATH-1);
	lstrcpy(myCmdLine, lpCmdLine);

  exeInfo.version         = EXEINFO_VERSION_NOW;
  exeInfo.flags           = isRuntime | isMicroApp;

	exeInfo.pAuxInfo->fileObj         = 0;
	exeInfo.pAuxInfo->seriusMem       = pTheSystem->GetTheMemory();
	exeInfo.pAuxInfo->tempStringSpace = pTheSystem->GetTempStringSpace();
	exeInfo.pAuxInfo->lpExeFileName   = myPath;
	exeInfo.pAuxInfo->lpExeCmdLine    = myCmdLine;
	exeInfo.pAuxInfo->globalBlockAddr = GetGlobalBlockArrayAddr();
	exeInfo.pAuxInfo->globalBlockSize = GetGlobalBlockArraySize();

  //initialize environment call backs to 0
  exeInfo.pAuxInfo->pfnObjCreate    = 0;
	exeInfo.pAuxInfo->debugger				= 0;
	exeInfo.pAuxInfo->pfnObjDestroy 	= 0;
	exeInfo.pAuxInfo->pfnObjSetName		= 0;

  if (exeRegisterContext(&exeInfo))
    {
    exeRunApp();
    exeUnregisterContext();
    result = 0;
    }
  else
    result = 1;

  SHELLSYSTEM::Cleanup();

	if(AUtlGetOSVersion() == AUTL_OS_WINNT)
  	{
    Ctl3dUnregister(hInstance);
    }

  return result;
  }
