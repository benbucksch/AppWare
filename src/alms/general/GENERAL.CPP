//======================================================================

// general.cpp -- General Functions for Serius
//
//  jon 20-Jul-92 [3.0] created
//======================================================================
#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <commdlg.h>
#include <intdebug.h>

#include <o_text.h>
#include <o_number.h>
#include <o_genrl.h>

//////////////// Globals

HINSTANCE		g_hDLL;

////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	g_hDLL = hInstDLL;

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}


//----------------------------------------------------------------------
//  gen_Assign -- assign [Source] object to [Destination] object
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Assign(
  pAEvtInfo                                     theEvent,
  pAObjMessage      theSystem)
  {
  char    theError[80];
  AOBJECTID source = AFuncGetTypedParameter(1, NULL);
  AOBJECTID dest   = AFuncGetTypedParameter(2, NULL);

	I_UNUSED(theEvent);

  theSystem->message1 = 1;
  if (source && dest)
		{
    if (!AObjAssign(source, dest))              // Assign fails
			{
			ATYPEID srcTypeId = AObjGetType(source);    // get source object id
			ATYPEID dstTypeId = AObjGetType(dest);                    // get dest object id

			// Get type id character
			char c1 = (char)HIBYTE(HIWORD(srcTypeId));
			char c2 = (char)LOBYTE(HIWORD(srcTypeId));
			char c3 = (char)HIBYTE(LOWORD(srcTypeId));
			char c4 = (char)LOBYTE(LOWORD(srcTypeId));

			char c5 = (char)HIBYTE(HIWORD(dstTypeId));
			char c6 = (char)LOBYTE(HIWORD(dstTypeId));
			char c7 = (char)HIBYTE(LOWORD(dstTypeId));
			char c8 = (char)LOBYTE(LOWORD(dstTypeId));

			wsprintf(theError, "The Object Type '%c%c%c%c' and the Object Type '%c%c%c%c' don't match",
							c1,c2,c3,c4,c5,c6,c7,c8);

			AFuncReportResult(theSystem, theSystem->message1, theError);
			}
		}
  }

//----------------------------------------------------------------------
//  gen_Beep -- make a beep sound
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Beep(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
  {
	I_UNUSED(theEvent);

  ::MessageBeep(0);
  theSystem->message1 = 1;
  }

//----------------------------------------------------------------------
//  gen_Copy -- copy selection to clipboard
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Copy(
  pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
  {
  AEvtPostStandard((AOBJECTID)0, OGEN_EVT_COPY, 0, AEVT_ATMARK);

  theSystem->message1 = 1;
  }

//----------------------------------------------------------------------
//  gen_Cut -- cut selection to clipboard
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Cut(
  pAEvtInfo                     theEvent,
  pAObjMessage  theSystem)
  {
	I_UNUSED(theEvent);

  AEvtPostStandard((AOBJECTID)0, OGEN_EVT_CUT, 0, AEVT_ATMARK);

  theSystem->message1 = 1;
  }

//----------------------------------------------------------------------
//  gen_Delay -- wait for [Ticks] serius ticks (60 ticks in a second)
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Delay(
  pAEvtInfo			theEvent,
  pAObjMessage	theSystem)
  {
  AOBJECTID			tickObj = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	LONG					lDelay;
	LONG					start;

	I_UNUSED(theEvent);

  if (tickObj)
    {
		lDelay = ONmbrGetInteger(tickObj);

		start = AAppGetTime();
		while (AAppGetTime() - start < lDelay)
			;
		}

	theSystem->message1 = 1;
  }

//----------------------------------------------------------------------
//  gen_FreeMemory -- put number of bytes of free RAM in [FreeMemory]
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_FreeMemory(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
  {
  AOBJECTID  numberObj = AObjCreate(OTYPE_NUMBER);

	I_UNUSED(theEvent);

  if (!numberObj)
    {
    AFuncReportResult(theSystem,
		    -1,
		    "The 'Free Memory' general function "
		    "can't create an output number object");
    return;
    }

//!!	ONmbrSetInteger(numberObj, ::GetFreeSpace(0), FALSE);	//!! port
  AFuncSetParameter(1, numberObj);

 // good bye
  theSystem->message1 = 1;
  }

//----------------------------------------------------------------------
//  gen_IsColorOn -- branch on color-ness of this box...
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_IsColorOn(
  pAEvtInfo                             theEvent,
	pAObjMessage    theSystem)
  {
  HDC hIC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
  int numColors = ::GetDeviceCaps(hIC, NUMCOLORS);

	I_UNUSED(theEvent);

  ::DeleteDC(hIC);

  if (numColors > 2)
    theSystem->message1 = 1;  // color is ON
  else
    theSystem->message1 = 2;  // color is OFF
  }

//----------------------------------------------------------------------
//  gen_IsSameObject -- compare [Object 1] to [Object 2]
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_IsSameObject(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
  {
	AOBJECTID source = AFuncGetTypedParameter(1, NULL);
	AOBJECTID target = AFuncGetTypedParameter(2, NULL);

	I_UNUSED(theEvent);

	if (source == target)
		theSystem->message1 = 1;    // they are the same object
	else
		theSystem->message1 = 2;    // they are NOT the same object
	}

//----------------------------------------------------------------------
//  gen_IsSameObjectType -- compare [Object 1]'s type to [Object 2]'s
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_IsSameType(
	pAEvtInfo                               theEvent,
	pAObjMessage    theSystem)
	{
	AOBJECTID source = AFuncGetTypedParameter(1, NULL);
	AOBJECTID target = AFuncGetTypedParameter(2, NULL);

	I_UNUSED(theEvent);

	if (source && target
				&&
			AObjGetType(source) == AObjGetType(target))
		{
		theSystem->message1 = 1;    // they're the same type
		}
	else
		{
		theSystem->message1 = 2;    // they're NOT the same type
		}
	}

//----------------------------------------------------------------------
//  gen_Paste -- paste from clipboard to selection
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Paste(
	pAEvtInfo                       theEvent,
	pAObjMessage  theSystem)
	{
	I_UNUSED(theEvent);

	AEvtPostStandard((AOBJECTID)0, OGEN_EVT_PASTE, 0, AEVT_ATMARK);

	theSystem->message1 = 1;
	}

//----------------------------------------------------------------------
//  gen_Quit -- get outta town
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Quit(
	pAEvtInfo                       theEvent,
	pAObjMessage  theSystem)
	{
	I_UNUSED(theEvent);

	AEvtPostStandard((AOBJECTID)0, AEVENT_SHUTDOWN, 0, AEVT_ATTAIL);

	theSystem->message1 = 1;
	}

//----------------------------------------------------------------------
//  gen_Undo -- undo last operation
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_Undo(
	pAEvtInfo                               theEvent,
	pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	AEvtPostStandard((AOBJECTID)0, OGEN_EVT_UNDO, 0, AEVT_ATMARK);

	theSystem->message1 = 1;
	}

//----------------------------------------------------------------------
//  gen_GetTicks -- return the number of ticks in 1/60 seconds since the
//                                                              system started.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_GetTicks(
	pAEvtInfo                               theEvent,
	pAObjMessage    theSystem)
	{
	AOBJECTID number;

	I_UNUSED(theEvent);

	number = AObjCreate(OTYPE_NUMBER);
	if(number)
		{
		ONmbrSetInteger(number, AAppGetTime(), FALSE);
		AFuncSetParameter(1, number);
		}
	else
		{
		lstrcpy(theSystem->errorMessage, "Error creating Number object."
			"  Cound not get the number of ticks.");
		}
	theSystem->message1 = 1;
	}

//----------------------------------------------------------------------
//  gen_YesNoCancel -- ask the user the [Question]
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK gen_YesNoCancel(
	pAEvtInfo                               theEvent,
	pAObjMessage    theSystem)
	{
	AOBJECTID    questionObj = AFuncGetTypedParameter(1, OTYPE_TEXT);
	AMEMBLOCKID  questionBlock = 0;
	AOBJECTID    titleObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
	AMEMBLOCKID  titleBlock = 0;
	int         sheSaid;

	I_UNUSED(theEvent);

	if (!questionObj)
		{
		AFuncReportResult(theSystem,
										-1,
										"The 'Yes No Cancel' function "
										"received an invalid object for the Question");
		goto ByeNow;
		}

	questionBlock = AMemAllocate(0);
  if (!questionBlock)
    {
    AFuncReportResult(theSystem,
		    -1,
		    "The 'Yes No Cancel' function can't "
		    "allocate a block to hold the Question's text");
    goto ByeNow;
    }

  if (!AObjConvertToText(questionObj, questionBlock))
    {
    AFuncReportResult(theSystem,
		    -1,
		    "The 'Yes No Cancel' function can't fetch "
		    "the textual version of the Question parameter");
    goto ByeNow;
    }

  titleBlock = AMemAllocate(0);
  if (!titleBlock)
    {
    AFuncReportResult(theSystem,
		  -1,
		  "The 'Yes No Cancel' function can't "
		  "allocate a block to hold the Title's text");
    goto ByeNow;
    }

  if (titleObj)
    {
    if (!AObjConvertToText(titleObj, titleBlock))
      {
      AFuncReportResult(theSystem,
		      -1,
		      "The 'Yes No Cancel' function can't fetch "
		      "the textual version of the Title parameter");

      goto ByeNow;
      }
    }
  else
    {
    AMemSetSize(titleBlock, 20);
    lstrcpy((LPSTR)AMemLock(titleBlock), "Notice");
    }

  sheSaid = MessageBox(NULL,
		       (LPSTR) AMemLock(questionBlock),
		       (LPSTR) AMemLock(titleBlock),
		       MB_YESNOCANCEL|MB_TASKMODAL);

  switch (sheSaid)
    {
    case IDYES:                   // she loves it
      theSystem->message1 = 1;
      break;

    case IDNO:                    // she hates it
      theSystem->message1 = 2;
      break;

    case IDCANCEL:                // she hates it
      theSystem->message1 = 3;
      break;

    default:                      // ? ? ?
      AFuncReportResult(theSystem,
		      -1,
		      "The 'Yes No Cancel' general function "
		      "can't cope with response...");
      break;

    }

ByeNow:

  if (questionBlock)
    AMemFree(questionBlock);

  if (titleBlock)
    AMemFree(titleBlock);
	}



//----------------------------------------------------------------------
//  CallWinHelp  -- to invoke the WINHELP function (WIN API 3.1)
//               Optional input:  FILENAME (*.hlp)  
//                                SEARCH STRING 
//                                CONTEXT INDEX
//  Author:     Shaili
//  date:       April 23, 1993
//  Function ID:
//
//----------------------------------------------------------------------

extern "C"
void ALMCALLBACK CallWinHelp(
  pAEvtInfo                     theEvent,
	pAObjMessage    system
	)
	{
	OPENFILENAME ofn;
	AOBJECTID textObject;
	char FileName[256];
  char PathName[256];
	HWND hWnd = GetActiveWindow     ();
	AOBJECTID keyObject, contextObject;
	int size = 0;
	BOOL bDone;
	int  rVal = 1;

	I_UNUSED(theEvent);

	textObject= AFuncGetTypedParameter(1, OTYPE_TEXT) ; // get the optional input parameter

	keyObject  = AFuncGetTypedParameter(2, OTYPE_TEXT);  // help key

	contextObject = AFuncGetTypedParameter(3, OTYPE_NUMBER);  // context Index

	
	if (textObject)
    {
		size = (int) AObjGetDataSize(textObject, 1);
		
		CopyMemory(PathName,AObjLockData(textObject, 1), size);
		}

  else
		PathName[0] = '\0';

	LPSTR lpext = strchr(PathName, '.');	//!!FIX 

	if ((lpext == NULL) || (lstrcmpi(lpext, ".hlp") != 0))
		{
    char szDirName[80];
		char szFilter[30];
		
		FillMemory(&ofn, sizeof(OPENFILENAME), 0);
		lstrcpy(FileName, "*.hlp");
    lstrcpy(PathName, FileName);

		lstrcpy(szFilter, "Help File (*.hlp)");

		GetSystemDirectory(szDirName, sizeof(szDirName));

		ofn.lStructSize       = sizeof(OPENFILENAME);
		ofn.hwndOwner         = GetActiveWindow();
		// ofn.hInstance      = NULL;
		ofn.lpstrFilter       = szFilter;
		ofn.lpstrCustomFilter = NULL;
		// ofn.nMaxCustFilter    = 0L;
		ofn.nFilterIndex      = 1L;
		ofn.lpstrFile         = PathName;
		ofn.nMaxFile          = sizeof(FileName);
		ofn.lpstrFileTitle    = FileName;
		ofn.nMaxFileTitle     = 80;
		ofn.lpstrInitialDir   = szDirName;
		ofn.lpstrTitle        = "Open Help File";
		
	  ofn.Flags             = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

		if (GetOpenFileName(&ofn) == FALSE)
		  {
			size = 0;
			}
		else
			{
			size = lstrlen(PathName) + 1 ;
      }
	  } 
	
	system->message1 = 1;
	if (size == 0)
    {
		system->message1 = 2;

		AFuncSetParameter(1, textObject);

		return;
		}


	bDone = FALSE;

  if (keyObject)
		{
    LPSTR lpKey;
		size = (int) AObjGetDataSize(keyObject, 1);

		lpKey = new char[size + 1];

		CopyMemory(lpKey,AObjLockData(keyObject, 1), size);

    
		if (lpKey[size - 2] == ' ') // a space at the end
			lpKey[size - 2] = '\0';
   
		rVal = WinHelp(hWnd, PathName, HELP_PARTIALKEY, (DWORD)lpKey);
		delete lpKey;
    bDone = TRUE;
		}

	if (!bDone && contextObject)
	  {
		if (AObjGetType(contextObject) == OTYPE_NUMBER)
      {
			rVal = WinHelp(hWnd, PathName, HELP_CONTEXT,
											ONmbrGetInteger(contextObject));
		  bDone = TRUE;
      }
		}

  if (!bDone)  // if help has not been called
	  rVal = WinHelp(hWnd, PathName, HELP_INDEX, NULL);


    

  AMEMBLOCKID fileID = AMemAllocate(lstrlen(PathName) + 1);
	LPSTR fileStr =(LPSTR)AMemLock(fileID);
	AOBJECTID outputID = AObjCreate(OTYPE_TEXT);
	lstrcpy(fileStr, PathName);

  AObjConvertFromText(outputID, fileID);
	AMemFree(fileID);
	AFuncSetParameter(1, outputID);

	if (rVal == 0)
		{
		system->message1 = 2;
    
		return;
    }
	}



//----------------------------------------------------------------------
//  CloseWinHelp  -- to exit the WINHELP function (WIN API 3.1)
//                input:  FILENAME (*.hlp)  
//              This will facilitate properly closing Help window when a
//              serius application shuts down (by tying this function to
//              shutdown call ?)
//  Author:     Shaili
//  date:       April 23, 1993
//
//----------------------------------------------------------------------

extern "C"
void ALMCALLBACK CloseWinHelp(
  pAEvtInfo                     theEvent,
	pAObjMessage    system
	)
	{
	AOBJECTID textObject = AFuncGetTypedParameter(1, OTYPE_TEXT); // the picture object to load New picture into
  LPSTR lpHelpFile	= NULL;
	HWND hWnd = GetActiveWindow();

	I_UNUSED(theEvent);

	if (textObject)
		{
		int size =  (int) AObjGetDataSize(textObject, 1);
    lpHelpFile = new char[size];
		CopyMemory(lpHelpFile,AObjLockData(textObject, 1), size);
		}

	system->message1 = 1;

	if (lstrlen(lpHelpFile) == 0)
    {
		system->message1 = 2;
		return;
    }
	
	if (WinHelp(hWnd, lpHelpFile, HELP_QUIT, NULL))
	  system->message1 = 1;
	else
    system->message1 = 2;

	delete lpHelpFile;
	}
