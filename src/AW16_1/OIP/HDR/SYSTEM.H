//======================================================================
// [*OIP*] system.h -- global variable manager for OIP.DLL
//
//  jon 12-Jun-92 created
//  jon 19-Jun-92 added GetNumColors()
//  jon 21-Aug-92 made GetCurrentContext inline
//  jon 30-Sep-92 removed IsAtStop() member function.
//  jon 11-Dec-92 added IsAppRunning() cuz isRuntime returns TRUE for
//                compile-time and stop-time too...
//  shaili 5-May-93 Resource Manager instantiation
//======================================================================

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "fileobj.h"
#include "ememstr.h"
#include "contextd.h"
#include "exeinfo.h"

class SER_WINRESOURCES;   // defined in rsrcmgr.h

#define MAXSTACKTOP	9

////////////////////////////////////////////////////////////////
//
// CLASS:		SYSTEM
//
// DESC:		Centralizes system-level methods and information.
//
////////////////////////////////////////////////////////////////

class SYSTEM
	{
	private:
		static LPMASTERTYPELIST masterTypeList;

	protected:
		static BOOL								m_fColorsValid;
		static SERIUSMEMORY				theMemory;
		static LPFILEOBJECT				contextStack[MAXSTACKTOP + 1];
		static int								stackTop;
		static STRINGSPACE				tempStringSpace;
		static int								numberOfColors;
		static LPAPPCONTEXT				pFirstContext;
		static LPAPPCONTEXT				pCurrContext;
		static LPEXEINFO					transitionInfo; // info used during context build/teardown

		// private methods
		LPAPPCONTEXT							FindContext(HTASK taskID);
		LPAPPCONTEXT							LinkContext(LPAPPCONTEXT newContext);
		BOOL											UnlinkContext(LPAPPCONTEXT dying, BOOL fDelete = TRUE);

															SYSTEM(VOID)					{}
														 ~SYSTEM(VOID)					{}

	public:
		// Public fields:
		static SER_WINRESOURCES*	serWResources; // Resource manager

		// Public methods:
		BOOL											IsValidContext(LPAPPCONTEXT);

		virtual VOID							Initialize(VOID)	= 0;
		virtual VOID							CleanUp(VOID)			= 0;

		virtual BOOL							RegisterContext(LPEXEINFO setupBlock)	= 0;
		virtual BOOL							UnregisterContext(VOID)								= 0;
		virtual BOOL							PushContext(LPFILEOBJECT)							= 0;
		virtual LPFILEOBJECT			PopContext(VOID)											= 0;
		virtual LPAPPCONTEXT			NextContext(LPAPPCONTEXT)								= 0;

		virtual LPAPPCONTEXT			GetCurrentContext(VOID)											= 0;
		virtual LPFILEOBJECT			GetFileObject(VOID)													= 0;
		virtual VOID							SetFileObject(LPFILEOBJECT theFileObject)		= 0;
		virtual LPSERIUSMEMORY		GetTheMemory(VOID)													= 0;
		virtual LPSTRINGSPACE			GetTempStringSpace(VOID)										= 0;
		virtual int								GetNumColors(VOID)													= 0;
		virtual BOOL							IsRuntime(VOID)															= 0;
		virtual BOOL							IsExeStructures(VOID)												= 0;
	
		void SetMasterTypeList(
			LPMASTERTYPELIST newList)
			{
			SYSTEM::masterTypeList = newList;
			}

		LPMASTERTYPELIST GetMasterTypeList()
			{
			return SYSTEM::masterTypeList;
			}
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		SYSTEMFULL
//
// DESC:		Implements a full system
//
////////////////////////////////////////////////////////////////

class SYSTEMFULL : public SYSTEM
	{
	public:
															SYSTEMFULL(VOID)					{}
														 ~SYSTEMFULL(VOID)					{}

		VOID											Initialize(VOID);
		VOID											CleanUp(VOID);

		BOOL											RegisterContext(LPEXEINFO setupBlock);
		BOOL											UnregisterContext(VOID);
		BOOL											PushContext(LPFILEOBJECT);
		LPFILEOBJECT							PopContext(VOID);
		LPAPPCONTEXT							NextContext(LPAPPCONTEXT);

		LPAPPCONTEXT							GetCurrentContext(VOID);
		LPFILEOBJECT							GetFileObject(VOID);
		VOID											SetFileObject(LPFILEOBJECT theFileObject);
		LPSERIUSMEMORY						GetTheMemory(VOID);
		LPSTRINGSPACE							GetTempStringSpace(VOID);
		int												GetNumColors(VOID);
		BOOL											IsRuntime(VOID);
		BOOL											IsExeStructures(VOID);
	};

////////////////////////////////////////////////////////////////
//
// CLASS:		SYSTEMMIN
//
// DESC:		Implements a minimum system.
//
////////////////////////////////////////////////////////////////

class SYSTEMMIN : public SYSTEM
	{
	private:
		VOID											InitMemory(VOID);

	public:
															SYSTEMMIN(VOID)						{}
														 ~SYSTEMMIN(VOID)						{}

		VOID											Initialize(VOID);
		VOID											CleanUp(VOID);

		BOOL											RegisterContext(LPEXEINFO setupBlock);
		BOOL											UnregisterContext(VOID);
		BOOL											PushContext(LPFILEOBJECT);
		LPFILEOBJECT							PopContext(VOID);
		LPAPPCONTEXT							NextContext(LPAPPCONTEXT);

		LPAPPCONTEXT							GetCurrentContext(VOID);
		LPFILEOBJECT							GetFileObject(VOID);
		VOID											SetFileObject(LPFILEOBJECT theFileObject);
		LPSERIUSMEMORY						GetTheMemory(VOID);
		LPSTRINGSPACE							GetTempStringSpace(VOID);
		int												GetNumColors(VOID);
		BOOL											IsRuntime(VOID);
		BOOL											IsExeStructures(VOID);
	};

////////////////////////////////////////////////////////////////
//
// Assume large model.  There will be multiple SYSTEM objects (one per
// .DLL or .EXE).

extern SYSTEM*			pTheSystem;

#endif // __SYSTEM_H__
