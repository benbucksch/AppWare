////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  Launcher			
//
// Filename:	"launc_fn.cpp"
//
// Author:	Kevin Smart
//   Start:	August  6, 1992
//   Finish:	August  6, 1992
//
// Description:
//	Launch Object Functions
//	  1.  LaunchApplication
//	  2.  SetApplication
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<string.h>
#include	<dir.h>
#include	<ctype.h>
#include	<o_text.h>

#include	"launch.h"

////////////////////////////Global Container////////////////////////////////

extern GLOBAL	G;

////////////////////////////////////////////////////////////////////////////
//  LaunchApplication
//
//      inputs:
//        1.  Launcher (launcher object)
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after launching (even if error)
////////////////////////////////////////////////////////////////////////////
#pragma argsused        // disable warning:  "Parameter '' never used"
void far pascal _export	LaunchApplication	(pAEvtInfo theEvent,
						 pAObjMessage      theSystem)

{
  OBJECTID	theObject = AFuncGetTypedParameter(1L, OTYPE_LAUNCHER);

  if(!theObject) {
    AFuncReportResult(theSystem, 1L, "The input to the \"Launch Application\" function must be a Launcher Object.");
    return;
    }

  LAUNCH	launch(theObject);

  // temporarily store the current working directory and drive
  char	cwdir[256];
  getcwd(cwdir, 256);
  int	drive = getdisk();

  char*	startdir = launch.GetStartDir();
  if(strlen(startdir)) {
    if(startdir[1]==':') {
      int dr = toupper(startdir[0]) - 'A';
      setdisk(dr);
      }
    chdir(startdir);
    }
  UINT  ret = WinExec(launch.GetCmdLine(), launch.GetCmdShow());

  if(ret < 32) {
    switch(ret) {
      case  0: strcpy(theSystem->errorMessage, "System was out of memory, executable file was corrupt, or relocations were invalid.");
	       break;

      case  2: strcpy(theSystem->errorMessage, "File was not found.");
	       break;

      case  3: strcpy(theSystem->errorMessage, "Path was not found.");
	       break;

      case  5: strcpy(theSystem->errorMessage, "Attempt was made to dynamically link to a task, or there was a sharing or network-protection error.");
	       break;

      case  6: strcpy(theSystem->errorMessage, "Library required separate data segments for each task.");
	       break;

      case  8: strcpy(theSystem->errorMessage, "There was insufficient memory to start the application.");
      	       break;	

      case 10: strcpy(theSystem->errorMessage, "Windows version was incorrect.");
	       break;

      case 11: strcpy(theSystem->errorMessage, "Executable file was invalid. Either it was not a Windows application or there was an error in the .EXE image.");
	       break;

      case 12: strcpy(theSystem->errorMessage, "Application was designed for a different operating system.");
	       break;

      case 13: strcpy(theSystem->errorMessage, "Application was designed for MS-DOS 4.0.");
	       break;

      case 14: strcpy(theSystem->errorMessage, "Type of executable file was unknown.");
	       break;

      case 15: strcpy(theSystem->errorMessage, "Attempt was made to load a real-mode application (developed for an earlier version of Windows).");
	       break;

      case 16: strcpy(theSystem->errorMessage, "Attempt was made to load a second instance of an executable file containing multiple data segments that were not marked read-only.");
	       break;

      case 17: strcpy(theSystem->errorMessage, "Attempt in large-frame EMS mode to load a second instance of an application that links to certain nonshareable DLLs already in use.");
	       break;

      case 18: strcpy(theSystem->errorMessage, "Attempt in real mode to load an application marked for protected mode only.");
	       break;

      case 19: strcpy(theSystem->errorMessage, "Attempt was made to load a compressed executable file. The file must be decompressed before it can be loaded.");
	       break;

      case 20: strcpy(theSystem->errorMessage, "Dynamic-link library (DLL) file was invalid. One of the DLLs required to run this application was corrupt.");
	       break;

      case 21: strcpy(theSystem->errorMessage, "Application requires 32-bit extensions.");
	       break;

      default: strcpy(theSystem->errorMessage, "Unknown error.");
	       break;

      }
    }

  // restore the working directory and drvie
  setdisk(drive);
  chdir(cwdir);

  AFuncReportResult(theSystem, 1L, NULL);
}

////////////////////////////////////////////////////////////////////////////
//  SetApplication
//
//      inputs:
//        1.  Launcher Object
//	  2.  Command Line (optional text object)
//
//      outputs:
//        none
//
//      flows:
//        1.  Followed after the application has been set
//	  2.  Followed if the user cancels the dialog.
////////////////////////////////////////////////////////////////////////////
#pragma argsused        // disable warning:  "Parameter '' never used"
void far pascal _export	SetApplication	(pAEvtInfo theEvent,
					 pAObjMessage      theSystem)

{
  OBJECTID	launchObj = AFuncGetTypedParameter(1L, OTYPE_LAUNCHER);
  OBJECTID	textObj	  = AFuncGetTypedParameter(2L, OTYPE_TEXT);

  if(!launchObj) {
    AFuncReportResult(theSystem, 2L, "The first input to the \"Set Application\" function must be a Launcher Object.");
    return;
    }

  LAUNCH	launch(launchObj);

  if(!textObj) {
    launch.style = 2;
    if(DialogBoxParam(G.hInst, "LaunchDlg2", GetActiveWindow(),
		   (FARPROC)LAUNCH::LaunchDlg, (long)&launch)) {
      // OK button pressed
      AFuncReportResult(theSystem, 1L, NULL);
      return;
      }
    else {
      // Cancel button pressed
      AFuncReportResult(theSystem, 2L, NULL);
      return;
      }
    }
  else {
    MEMBLOCKID	theBlock = AMemAllocate(1L);
    AObjConvertToText(textObj, theBlock);
    char	szBuffer[256];
    strcpy(szBuffer, (char*)AMemLock(theBlock));
    AMemFree(theBlock);

    launch.SetCmdLine(szBuffer);
    char*	tmp;
    if(!(tmp = strrchr(szBuffer, '\\'))) {
      if(!(tmp=strrchr(szBuffer, ':'))) {
	launch.SetStartDir("");
	}
      else {
	tmp[1]=0;
	launch.SetStartDir(szBuffer);
        }
      }
    else {
      if(*(tmp-1)==':') {
	tmp[1]=0;
	}
      else {
	tmp[0]=0;
        }
      launch.SetStartDir(szBuffer);
      }

    launch.SetRunMin(FALSE);

    AFuncReportResult(theSystem, 1L, NULL);
    return;
    }
}

////////////////////////////////////////////////////////////////////////////
