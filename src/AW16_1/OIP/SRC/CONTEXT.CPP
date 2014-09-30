//======================================================================
// context.cpp -- task-specific information for OIP.DLL
//
//  jon 10-Jun-92 created
//  jon 23-Jun-92 added "sereclib.h" to solve(?) link problem with
//                GetObject(SERUID) ???
//  jon 24-Jun-92 don't attempt to get the pointer for <exeHead> if
//                the passed file object has a null SERID for it, ie
//                the EXE structures haven't been filled in yet because
//                we are pre-compile.
//  jon 24-Jun-92 overload new and delete operators, so we can cope
//                with crashed client tasks... i.e., any client that
//                doesn't exeUnregisterContext() before dying.  Now
//                the DLL will own the memory, not the client apps.
//  jon 24-Jun-92 changed documentation style to appease the master.
//  jon  9-Aug-92 added GetCopyOfExeInfo()
//  jon 24-Aug-92 if it's debug it *must* be runtime...
//  jon 27-Aug-92 added FillFromExeInfo() to aid debug world.
//  jon 12-Oct-92 REMOVED GetCopyOfExeInfo() [never used]
//======================================================================
  #include <new.h>
  #include <dir.h>
  #include <string.h>
	#include "almsys.h"

  #include "context.h"
  #include "sereclib.h"
  #include "memlib.h"
  #include "persdata.h"
  #include "exeobjs.h"
  #include "inifile.h"
	#include "oiputils.h"
	#include "oiptime.h"

  class   EXEEVENTLIST;
  typedef EXEEVENTLIST far * LPEXEEVENTLIST;

  #include "exetypes.h"

  #include "eventqnd.i"
  #include "eventqms.i"

//----------------------------------------------------------------------
//   CONTEXT::operator new
//
// Use LocalAlloc to avoid problems with crashed clients.
// Algorithm from Scott Meyers, "Effective C++", p 27.
//----------------------------------------------------------------------
void far * APPCONTEXT::operator new(size_t size)  {
  HANDLE hMem;

  if (size != sizeof(APPCONTEXT))
    return ::new char[size];

 // -- get the address of the current new_handler
#ifdef __BORLANDC__
  typedef void (*PEHF) (void);  // pointer to error handler function
#else
  typedef int (*PEHF) (size_t);  // pointer to error handler function
	#define set_new_handler(f)	_set_new_handler(f)
#endif

  PEHF  currentHandler = set_new_handler(0);
  set_new_handler(currentHandler);

  while (TRUE)
    {
    hMem = LocalAlloc(LMEM_ZEROINIT, size + sizeof(HANDLE));
    if (hMem)
      {
     // lock it down and remember the handle
      PBYTE chunk = (PBYTE) LocalLock(hMem);
      *((HANDLE *)chunk) = hMem;
      return chunk + sizeof(HANDLE);
      }
    if (currentHandler)
#ifdef __BORLANDC__
      (*currentHandler)();
#else
      (*currentHandler)(size);
#endif
    else
      return 0;
    }
  }

//----------------------------------------------------------------------
//   APPCONTEXT::operator delete
//
// Use LocalFree to avoid problems with crashed clients.
// Algorithm from Scott Meyers, "Effective C++", p 33.
//----------------------------------------------------------------------
void APPCONTEXT::operator delete(void far *deadObject, size_t size)
  {
  HANDLE hMem;

  if (size != sizeof(APPCONTEXT))
    {
    ::delete [] deadObject;
    return;
    }

  hMem = *((HANDLE *)deadObject - 1);
  LocalUnlock(hMem);
  LocalFree(hMem);
  }

//----------------------------------------------------------------------
//   APPCONTEXT::APPCONTEXT
//
// Instantiates the context.
//----------------------------------------------------------------------
APPCONTEXT::APPCONTEXT (HTASK taskNow, LPEXEINFO theExeInfo)
	: exeInfo(&exeInfoAux),status(0L)

  {
  this->next         = 0;
  this->taskID       = taskNow;
  this->priorityMode = AAPP_OBJECTPRIORITY;
  this->eventQueue   = 0;
  this->hIcon        = 0;

  // Save Windows version... flip bytes
  WORD rawVersion = GetVersion();
  this->osVersion = (LOBYTE(rawVersion) << 8) + HIBYTE(rawVersion);

  this->Freshen(theExeInfo);
  }

//----------------------------------------------------------------------
//   APPCONTEXT::~APPCONTEXT
//
// Kills the queue, its node managers and the application icon.  Also
// writes out the persistent data if there is any.
//----------------------------------------------------------------------
APPCONTEXT::~APPCONTEXT()
  {
  delete this->eventQueue;
  delete this->eventQueueNodeMgr;
  delete this->eventQueueMsgMgr;

  if (this->hIcon)
    DestroyIcon(this->hIcon);

  }

//----------------------------------------------------------------------
//   APPCONTEXT::Freshen
//
// Whenever a context is Freshen'd, the caller should check for a
// fileObj of zero to determine an error has occurred.
//----------------------------------------------------------------------
BOOL APPCONTEXT::Freshen(
  LPEXEINFO  theExeInfo)
  {
  LPEXEEVENTLIST  eventList;

  this->FreshenExeInfo(theExeInfo);

  if (this->IsMicroApp() && !LoadMicroApp())
    return FALSE;

 // *** [ CAUTION! ] *** [ CAUTION! ] *** [ CAUTION! ] *************
 //
 //  The assumption here is that no EXEHEAD type serius records
 //  will be constructed or destroyed at runtime. If this is not
 //  the case, it is very likely this exeHead pointer will go stale!
 //
 // *****************************************************************
  if (this->IsExeStructures())
    {
    SERUID exeHeadUID = this->GetFileObject()->GetHeadExeRec();
    if (exeHeadUID == NULLSERUID)
      this->exeHead = 0;
    else
      this->exeHead = (LPEXEHEAD) this->GetFileObject()
                                  ->GetUIDTable()
                                  ->GetPointer(exeHeadUID);
    }
  else
    this->exeHead = 0;

  if (this->IsRuntime())
    {
    // Ensure a queue is instanciated
    if (!this->eventQueue)
      {
      this->eventQueueNodeMgr = new FASTRECMGR(32, sizeof(EVENTQUEUENODE));
      this->eventQueueMsgMgr = new FASTRECMGR(32, sizeof(EVENTQUEUEMSG));
      this->eventQueue = new EVENTQUEUE;
      eventList = (LPEXEEVENTLIST)
                  GetFileObject()
                  -> GetUIDTable()
                  -> GetPointer(exeHead->GetExeEventListUID());
      this->eventQueue->SetEventList(eventList);
      }

    // Are there any stops set? (Speeds function execution if
    // there are none)
    eventQueue->SetIsDebugging(this->IsDebug());

		if(TestSingleStepFlag())
			eventQueue->SetSingleStepFlag();
		else
			eventQueue->ClearSingleStepFlag();

    // Get the icon deposited by the compiler if we're standalone
    // else use serws's icon
    
    //IMPORTANT: 4002 is the value of IDIC_PROJECTEXE in resconst.h 
    this->hIcon = LoadIcon(this->GetTaskHInstance(),
                           this->IsMicroApp()
                             ? "ShellIcon"
                             : MAKEINTRESOURCE(4002));
		}

  return TRUE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::FreshenExeInfo
//
// Copies the specified EXEINFO into this->exeInfo.
//----------------------------------------------------------------------
void APPCONTEXT::FreshenExeInfo(LPEXEINFO theExeInfo)
  {
	this->exeInfo.version    = 0;
	this->exeInfo.flags      = 0;

	this->exeInfo.lUnused1   =
		this->exeInfo.lUnused2 =
		this->exeInfo.lUnused3 = 0;

	memset(this->exeInfo.pAuxInfo, 0, sizeof(EXEINFOAUX));

  switch (theExeInfo->version)
    {
		// We do NOT need to check before version 102 since we renamed
		// DLLs in going to AppWare and therefore required a recompile.

		case EXEINFO_VERSION_102:
    default:
			this->exeInfo.version = theExeInfo->version;
			this->exeInfo.flags   = theExeInfo->flags;

			*(this->exeInfo.pAuxInfo) = *(theExeInfo->pAuxInfo);
      break;
    }

  // Make sure Troy's MemLib globals are set up...
  this->SetGlobals();
  }

//----------------------------------------------------------------------
//   APPCONTEXT::GetCopyOfExeInfo
//
// Fetches a copy of the current EXEINFO for this context.
//----------------------------------------------------------------------
BOOL APPCONTEXT::GetCopyOfExeInfo(LPEXEINFO copy)
  {
	copy->version = this->exeInfo.version;
	copy->flags   = this->exeInfo.flags;

	*(copy->pAuxInfo) = *(this->exeInfo.pAuxInfo);

  return TRUE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::LoadMicroApp
//
// Build up the FILEOBJECT for this app.  We do it here to reduce the
// amount of code in the microapp stub.
//----------------------------------------------------------------------
BOOL APPCONTEXT::LoadMicroApp()
  {
  int        hExeFile;
  long       startOfExe;
  int        hPersFile;
  LPEXEHEAD  headRec;

#ifdef AW_I_TIME
	long			nLoadTime	= HiResTime();
	char			szMsg[80];
#endif

  hExeFile = OpenExeFile();

  SetFileObject(0);

  if (hExeFile == -1)
    return FALSE;

  // Start of app def is at offset spec'd in last four bytes of the exe
  _llseek(hExeFile, -(long)sizeof(startOfExe), 2);
  _lread(hExeFile, (LPSTR)&startOfExe, sizeof(startOfExe));

  // Go to the start of the app def
  _llseek(hExeFile, startOfExe, 0);

  SetFileObject(new FILEOBJECT(hExeFile, GetPrevInstance()));

  _lclose(hExeFile);

#ifdef AW_I_TIME
	nLoadTime	= HiResTime() - nLoadTime;
	wsprintf(szMsg, "Load time (ms):  %ld\r\n", nLoadTime);
	OutputDebugString(szMsg);
#endif

  if (!GetFileObject()->TestOkFlag())
    goto BadBye;

  // --- Get persistence info.

  headRec = (LPEXEHEAD)GetObject(GetFileObject()->GetHeadExeRec());
  if (headRec->HasPersistentData())
    {
#ifdef AW_I_TIME
		nLoadTime	= HiResTime();
#endif

    if (!OpenPersistents(&hPersFile, headRec->GetIniFileName()))
      goto BadBye;

    if (hPersFile != -1)
      {
      BOOL  persOK;

      persOK = ReadPersistents(hPersFile);
      _lclose(hPersFile);
      if (!persOK)
        goto BadBye;
      }
#ifdef AW_I_TIME
		nLoadTime	= HiResTime() - nLoadTime;
		wsprintf(szMsg, "Persistent load time (ms):  %ld\r\n", nLoadTime);
		OutputDebugString(szMsg);
#endif
    }

  return TRUE;

BadBye:
	if(GetFileObject()->TestOneInstanceFlag() && GetPrevInstance())
    {
		SetDupInstanceFlag();
	  }

	delete GetFileObject();
 	SetFileObject(0);
    
  return FALSE;

  }


//----------------------------------------------------------------------
//   OpenExeFile
//
// Opens the .EXE that is calling us.
//----------------------------------------------------------------------
int APPCONTEXT::OpenExeFile()
  {
  int            hFile;
  OFSTRUCT       ofStruct;

  hFile = OpenFile(GetExeFileName(), &ofStruct, OF_READ);
  return hFile;
  }


//----------------------------------------------------------------------
//   APPCONTEXT::OpenPersistents
//
// Determines path to persistent data file, 'pdfPath', and opens the
// persistent data file.
//
// The pdfPath is saved into this context's FILEOBJECT [via the
// SetFilePath() method]. Fills in hPersFile with the DOS file
// handle to the persistent data file. A hPersFile of -1 indicates
// a persistent data file was not opened. Returns FALSE on error.
//----------------------------------------------------------------------
BOOL APPCONTEXT::OpenPersistents(
  LPINT  hPersFile,
  LPSTR  iniFileName)
  {
  static const char  nul = '\0';

  char        pdfPath[MAXPATH] = "";
  char        iniPath [MAXPATH];
  OFSTRUCT    ofStruct;

  char        exeDrive [MAXDRIVE];
  char        exeDir   [MAXDIR];
  char        exeName  [MAXFILE];
  char        exeExt   [MAXEXT];

  BOOL        foundPathInIni;

#ifdef __BORLANDC__
  fnsplit(GetExeFileName(), exeDrive, exeDir, exeName, exeExt);
#else
  _splitpath(GetExeFileName(), exeDrive, exeDir, exeName, exeExt);
#endif

  if (iniFileName)
    {
    //------------------------------------------------------------------
    //   A path to the .INI file for this app was passed in. Fetch
    //   the path specified in it.
    //------------------------------------------------------------------
    foundPathInIni = FetchPdfPathFromIni(
                       iniFileName, pdfPath, sizeof(pdfPath));
    }
  else
    {
    //------------------------------------------------------------------
    //   A .INI path was not passed in.  Search for a file named
    //   <app>.ini where <app> is the simple file name of this app.
    //   The pdfPath will be equal to noPathString as long as this
    //   app's .INI file is not found or there is no setting for the
    //   pdfPath in that .INI 
    //------------------------------------------------------------------

    // 1st -- Look for <app>.ini in startup directory...
    lstrcpy(iniPath, ".\\");
    lstrcat(iniPath, exeName);
    lstrcat(iniPath, ".INI");
    foundPathInIni = FetchPdfPathFromIni(
                       iniPath, pdfPath, sizeof(pdfPath));

    // 2nd -- Look by Windows rules [i.e., in Windows dir/System dir]
    if (!foundPathInIni)
      {
      lstrcpy(iniPath, exeName);
      lstrcat(iniPath, ".INI");
      foundPathInIni = FetchPdfPathFromIni(
                         iniPath, pdfPath, sizeof(pdfPath));
      }

    // 3rd -- Look in home directory of app
    if (!foundPathInIni)
      {
      lstrcpy(iniPath, exeDrive);
      lstrcat(iniPath, exeDir);
      lstrcat(iniPath, exeName);
      lstrcat(iniPath, ".INI");
      foundPathInIni = FetchPdfPathFromIni(
                         iniPath, pdfPath, sizeof(pdfPath));
      }
    }

  if (foundPathInIni)
    {
    if (pdfPath[0] == nul)
      {
      // The pdfPath was specified as a null string in the .INI file
      this->GetFileObject()->SetFilePath("");
      *hPersFile = -1;
      return TRUE;
      }

    //------------------------------------------------------------------
    //   Ensure that the pdfPath from the .INI is valid
    //------------------------------------------------------------------

    int badChar = strcspn(pdfPath, "*?");
    if (badChar < strlen(pdfPath)
          ||
        -1 == OpenFile(pdfPath, &ofStruct, OF_PARSE))
      {
      // The user has a bad path in the .INI file...
      this->GetFileObject()->SetFilePath("");
      *hPersFile = -1;

      return ComplainAboutPdfPath(pdfPath, iniPath);
      }
    }
  else
    {

    // The pdfPath was not specified in the INI. Look for a file
    // named <app>.pdt in the home directory of the executable.

    lstrcpy(pdfPath, exeDrive);
    lstrcat(pdfPath, exeDir);
    lstrcat(pdfPath, exeName);
    lstrcat(pdfPath, PERSDATA_EXTENSION);
    }

  ofStruct.szPathName[0] = nul;
  *hPersFile = OpenFile(pdfPath, &ofStruct, OF_READ);

  this->GetFileObject()->SetFilePath(
        ofStruct.szPathName[0] != nul
          ? (LPSTR)&(ofStruct.szPathName)
          : (LPSTR)pdfPath);

  return TRUE;
  }


//----------------------------------------------------------------------
//   APPCONTEXT::FetchPdfPathFromIni
//----------------------------------------------------------------------
BOOL APPCONTEXT::FetchPdfPathFromIni(
  LPSTR iniPath,
  LPSTR pdfPath,
  size_t pdfPathSize)
  {
  OFSTRUCT    ofStruct;
  static char noPathString[] = " x ";  // must be an invalid pathname char,,,


  if (!OpenFile(iniPath, &ofStruct, OF_EXIST))
    return FALSE;

  GetPrivateProfileString(
    INI_SETTINGS, PERSDATA_INIKEY, noPathString,
    pdfPath, pdfPathSize, iniPath);

  if (0 == lstrcmp(pdfPath, noPathString))
    {
    pdfPath[0] = '\0';
    return FALSE;
    }

  return TRUE;
  }

//------------------------------------------------------------------
//    APPCONTEXT::ComplainAboutPdfPath
//
// Tell'em the pdfPath from the .INI is invalid. Returns TRUE if
// the user wants to continue without a persistent data file.
//------------------------------------------------------------------
BOOL APPCONTEXT::ComplainAboutPdfPath(
  LPSTR pdfPath,
  LPSTR iniPath)
  {
  OFSTRUCT  ofStruct;
  char      msgText[MAXPATH+MAXPATH+100];
  char      msgCap [MAXPATH+30];
  int       ans;

  // The user has a bad path in the .INI file... Get the
  // full path to .INI file into ofStruct so we can report it

  OpenFile(iniPath, &ofStruct, OF_PARSE);

  // Scream and die...
  wsprintf(
    msgText,
    "INI File:   %s\n"
    "Section:   [%s]\n"
    "Keyword:   \"%s=\"\n"
    "Specified path:   %s"
    "\n\n"
    "The specified path is invalid.  "
    "Do you want to continue without persistent data?",
    (LPSTR)ofStruct.szPathName,
    (LPSTR)INI_SETTINGS,
    (LPSTR)PERSDATA_INIKEY,
    (LPSTR)pdfPath);

  wsprintf(msgCap, "Error in INI File: %s", (LPSTR)ofStruct.szPathName);

  ans = MessageBox(
           NULL,
           msgText,
           msgCap,
           MB_YESNO|MB_DEFBUTTON2|MB_ICONSTOP|MB_TASKMODAL);

  return (ans == IDYES);
  }

//----------------------------------------------------------------------
//   APPCONTEXT::ReadPersistents
//----------------------------------------------------------------------
BOOL APPCONTEXT::ReadPersistents(
  int  hFile)
  {
  PERSISTENTDATAHEADER    pdHdr;
  LPPERSISTENTDATAHEADER  exePdHdr;
  OBJECTHDR               poHdr;
  LPEXEOBJINST            poPtr;
  LPEXEHEAD               headRec;
  int                     idx;
  SERUID                  typeBlock;
  char                    msgText[MAXPATH+120] = "";

  headRec = (LPEXEHEAD)GetObject(GetFileObject()->GetHeadExeRec());
  exePdHdr = headRec->GetPersistentDataHeader();

  if (_lread(hFile, (LPSTR)&pdHdr, sizeof(pdHdr)) != sizeof(pdHdr)
        ||
      pdHdr.GetTag() != exePdHdr->GetTag())
    {
    wsprintf(msgText,
             "%s is not a persistent data file.  "
             "You need to delete it or rename it before"
             " you can run this application.",
             (LPSTR)(GetFileObject()->GetFilePath()));

    goto BadBye;
    }

  if (memcmp(&pdHdr, exePdHdr, sizeof(pdHdr)) != 0)
    {
    // It's a valid persistent data file but it doesn't match
    // this app, so don't read it in!
    goto GoodBye;
    }

  while (_lread(hFile, (LPSTR)&poHdr, sizeof(poHdr)) == sizeof(poHdr))
    {
    poPtr = (LPEXEOBJINST) GetSeriusObject(poHdr.objectID);
    if (!poPtr || !poPtr->TestPersistentsFlag() || poHdr.tag != EXEOBJ_TAG)
      {
      wsprintf(msgText,
               "%s has become corrupted.  "
               "You need to delete it or rename it before"
               " you can run this application.",
               (LPSTR)(GetFileObject()->GetFilePath()));

      goto BadBye;
      }

    poPtr->SetDatasCount(poHdr.numDatas);

    for (idx = 0 ; idx < poHdr.numDatas ; idx++)
      {
      if (!poPtr->LoadPersistentDataBlock(hFile, idx))
        {
        wsprintf(msgText,
                 "%s contains bad persistent data!  "
                 "You need to delete it or rename it before"
                 " you can run this application.",
                 (LPSTR)(GetFileObject()->GetFilePath()));

        goto BadBye;
        }
      }
    }

GoodBye:

  return TRUE;

BadBye:

  MessageBox(NULL,
             msgText,
             "Invalid Persistent Data File",
             MB_OK|MB_ICONSTOP|MB_TASKMODAL);

  return FALSE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::SavePersistents
//----------------------------------------------------------------------
BOOL APPCONTEXT::SavePersistents(
	BOOL displayErrorsFlag)
	{
	LPSTR                 pdPath;
	int                   hPersFile = -1;
	OFSTRUCT              of;
	PERSISTENTDATAHEADER  pdh;
	int                   i;
	RUNTIMEUID            typeBlock;
	LPEXEOBJTYPE          typePtr;
	char                  msgText[300];


	if (!this->exeHead->HasPersistentData())
		goto GoodBye;

	pdPath = GetFileObject()->GetFilePath();
	if (!pdPath || pdPath[0] == '\0')
		{
		// It's supposed to have persistent data ... but they
		// turned it off in the .INI file
		goto GoodBye;
		}

	hPersFile = OpenFile(this->GetFileObject()->GetFilePath(),
											 &of, OF_CREATE|OF_WRITE);

	if (hPersFile == -1)
		{
		wsprintf(msgText, "Can't open %s for writing.",
											(LPSTR)(this->GetFileObject()->GetFilePath()));
		goto BadBye;
		}

	_lwrite(hPersFile, (LPSTR)(this->exeHead->GetPersistentDataHeader()),
														 sizeof(PERSISTENTDATAHEADER));


	typeBlock = this->exeHead->GetExeObjTypeList();

	i = 0;
	while (0 != (typePtr = (LPEXEOBJTYPE)GetObject(typeBlock, i)))
		if (!typePtr->StorePersistentObjects(hPersFile))
			{
			TYPEID  typeID;
			char    typeAlphaID[5];

			typeID = typePtr->GetTypeID();
			typeAlphaID[0] = (0xFF000000L & typeID) >> 24;
			typeAlphaID[1] = (0x00FF0000L & typeID) >> 16;
			typeAlphaID[2] = (0x0000FF00L & typeID) >>  8;
			typeAlphaID[3] = (0x000000FFL & typeID);
			typeAlphaID[4] = '\0';

			wsprintf(
				msgText,
				"Can't save persistent info for a '%s' type object into %s.",
				(LPSTR)typeAlphaID,
				(LPSTR)(this->GetFileObject()->GetFilePath()));

			goto BadBye;
			}

GoodBye:
	if (hPersFile != -1)
		_lclose(hPersFile);
	return TRUE;

BadBye:

	if (displayErrorsFlag == TRUE)
		MessageBox(NULL, msgText, "Error Saving Persistent Data",
			MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);

	if (hPersFile != -1)
		_lclose(hPersFile);

	return FALSE;
	}

//----------------------------------------------------------------------
//   APPCONTEXT::SetAppPriorityMode
//
// Returns the old priority mode or zero if the newMode is
// invalid.
//----------------------------------------------------------------------
LONG APPCONTEXT::SetAppPriorityMode(LONG newMode)
  {
  if (newMode == AAPP_VISUALPRIORITY ||
      newMode == AAPP_FUNCTIONPRIORITY ||
      newMode == AAPP_OBJECTPRIORITY)
    {
    LONG hold = priorityMode;
    priorityMode = newMode;
    return hold;
    }
  else
    return 0;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::SetGlobals
//
// Make sure Troy's MemLib globals are set up...
// Also, make sure the masterTypeList is set up.
//----------------------------------------------------------------------
void APPCONTEXT::SetGlobals()
  {
  ::SetGlobalBlockArrayAddr(this->GetGlobalBlockAddr());
  ::SetGlobalBlockArraySize((unsigned)(this->GetGlobalBlockSize()));

	pTheSystem->SetMasterTypeList(this->GetMasterTypeList());
  }

//----------------------------------------------------------------------
//   APPCONTEXT::EnterDebugger
//
// Call into the debugger using the address passed when this context
// was registered.
//----------------------------------------------------------------------
BOOL APPCONTEXT::EnterDebugger(LONG exeFlow)
  {
  return (this->GetDebugProc())(exeFlow);
  }



//----------------------------------------------------------------------
//   APPCONTEXT::MaybeWritePersistentData
//
// Writes the persistent data if it's a micro app
//----------------------------------------------------------------------
BOOL APPCONTEXT::MaybeWritePersistentData()
	{
	BOOL rval = FALSE;

  if (this->IsMicroApp())
		{
    if (this->exeHead->HasPersistentData() && this->GetFileObject())
			{
			this->SavePersistents();
			rval = TRUE;
			}
		}
	
	return rval;
	}
