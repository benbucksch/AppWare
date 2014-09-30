//----------------------------------------------------------------------
// EXEINFO -- struct passed to the shell at registration time
//  jon 18-Jun-92 created
//----------------------------------------------------------------------

#ifndef  __EXEINFO_H
#define  __EXEINFO_H

#include "classdef.h"

typedef BOOL			(APIENTRY *DEBUGPROC)(LONG exeFlow);
typedef AOBJECTID	(ALMAPI* OBJCREATEPROC)(ATYPEID, APROTOCOLID);
typedef BOOL			(ALMAPI* OBJDESTROYPROC)(AOBJECTID);
typedef BOOL			(ALMAPI* OBJSETNAMEPROC)(AOBJECTID, LPCSTR);
typedef BOOL			(ALMAPI* SHOWVABPROC)(BOOL);

 // values for EXEINFO.flags
	const WORD isRuntime  = 0x0001;   // TRUE --> runtime
	const WORD isDebug    = 0x0002;   // TRUE --> at least one stop is set
	const WORD isCompile  = 0x0004;   // TRUE --> we're compiling
	const WORD isAtStop   = 0x0008;   // TRUE --> we're at a stop point
	const WORD isMicroApp = 0x0010;   // TRUE --> we're a sealed off app
	const WORD isContinue = 0x0020;   // TRUE --> we're continuing from a STOP
	const WORD doSingleStep = 0x0040; // TRUE --> we're single stepping
////////////////////////////////////////////////////////////////

struct EXEINFOAUX
	{
  // Memory structures (design, runtime & shell)
	LPFILEOBJECT			fileObj;
	LPSERIUSMEMORY		seriusMem;
	LPSTRINGSPACE			tempStringSpace;     

  LPSTR							lpExeFileName;				// shell -- path to .EXE
  LPSTR							lpExeCmdLine;					// shell -- app's cmd line

  void*							globalBlockAddr;			// run & shell
  int		globalBlockSize;			// run & shell

	LPMASTERTYPELIST	masterTypeList;			// VAB and KERNEL

  DEBUGPROC					debugger;							// run -- callback to enter debugger
	OBJCREATEPROC			pfnObjCreate;					// run -- callback to create object
	OBJDESTROYPROC		pfnObjDestroy;				// run -- callback to destroy object
	OBJSETNAMEPROC		pfnObjSetName;		// run -- callback to object's name
	SHOWVABPROC				pfnShowVAB;      // run -- callback to show/hide Visual AppBuilder
	HWND							hVABFrame;						// VAB frame window
	};

////////////////////////////////////////////////////////////////

class EXEINFO
	{
	private:
										EXEINFO(void);
	public:

		LONG						version;
		WORD						flags;					// miscellaneous flags
		LONG						lUnused1;
		LONG						lUnused2;
		LONG						lUnused3;

		LPEXEINFOAUX		pAuxInfo;

										EXEINFO(LPEXEINFOAUX p)		{pAuxInfo = p;}
	};

////////////////////////////////////////////////////////////////
//
// -- VERSIONING INFO --
//

  #define EXEINFO_VERSION_100  0L   // before we had versions...
  #define EXEINFO_VERSION_101  101L
	#define EXEINFO_VERSION_102  102L
  #define EXEINFO_VERSION_NOW  EXEINFO_VERSION_102

  #define EXEINFO_SIZE_NOW  (sizeof(EXEINFO))
    // 1.01 added 'debugger'
  #define EXEINFO_SIZE_100  32
	  // 1.02 moved runtime into to *pAuxInfo; added unused fields
	#define EXEINFO_SIZE_101	36

#endif // __EXEINFO_H
