//======================================================================
// [*OIP*] system.cpp -- global variable manager for OIP.DLL
//
//  jon 12-Jun-92 created
//  jon 19-Jun-92 added GetNumColors()
//  jon 21-Aug-92 cleaned up comments
//  jon 21-Aug-92 moved GetCurrentContext to SYSTEM.H so it is inline!
//  jon 27-Aug-92 mod'd RegisterContext to re-use a context block!
//  jon 24-Feb-93 more comments for documentation purposes
//======================================================================

#include "almsys.h"

#include "system.h"
#include "memlib.h"
#include "strspace.h"
#include "intdebug.h"

#define TEMP_STRING_BLOCK_SIZE	1024

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Initialize
//
// CLASS.......: 	SYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

VOID		SYSTEMMIN::Initialize(
	VOID
	)
	{
	InitializeBlocks();						//initialize runtime only memory

	InitMemory();									//initialize memory saved to disk
	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	InitMemory
//
// CLASS.......:  SYSTEM
//
// DESCRIPTION.:	Initializes the static data member MEMORY.  This structure
//								manages a list of memory chunk records.  The memory
//								allocated by a serius block comes from the chunks.
//
/////////////////////////////////////////////////////////////////////////////

VOID		SYSTEMMIN::InitMemory(
	VOID
	)
	{
	theMemory.chunksHandle = GlobalAlloc(GMEM_MOVEABLE, 0);
	theMemory.chunks = (MEMCHUNK far *)GlobalLock(theMemory.chunksHandle);
	theMemory.chunkCount = 0;
	theMemory.chunkIDSeed = 0;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CleanUp
//
// CLASS.......: 	SYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
VOID			SYSTEMMIN::CleanUp(
	VOID
	)
	{
	CleanUpBlocks();									 		//release runtime memory

	tempStringSpace.FreeStringMemory();   //release runtime string space
	};

//----------------------------------------------------------------------
//    SYSTEMMIN::RegisterContext
//
// Links in a context for the current task.  If one already exists for
// this task, freshen the context info based on the passed EXEINFO.
// Returns FALSE if there is an error. (called from exeRegisterContext)
//----------------------------------------------------------------------
BOOL		SYSTEMMIN::RegisterContext(
	LPEXEINFO	theExeInfo
	)
  {
	I_ASSERT(FALSE);

	I_UNUSED(theExeInfo);

	return FALSE;
	}

//----------------------------------------------------------------------
//    SYSTEMMIN::UnregisterContext
//
// Unregisters the serius context associated with this task.  Returns
// false if there was no context for this task, else true.
//----------------------------------------------------------------------

BOOL		SYSTEMMIN::UnregisterContext(
	VOID
	)
  {
	I_ASSERT(FALSE);

	return FALSE;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PushContext
//
// CLASS.......: 	SYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL		SYSTEMMIN::PushContext(
	LPFILEOBJECT	newContext
	)
	{
	if (stackTop < MAXSTACKTOP)
		{
		contextStack[++stackTop] = newContext;
		return TRUE;
		}
	else
		{
		MessageBox(NULL, "Context Stack Overflow", "Context Stack Error",
		 MB_OK);
		return FALSE;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PopContext
//
// CLASS.......: 	SYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

LPFILEOBJECT	SYSTEMMIN::PopContext(
	VOID
	)
	{
	if (stackTop > 0)
		{
		return contextStack[stackTop--];
		}
	else
		{
		MessageBox(NULL, "Context Stack Overflow", "Context Stack Error",
		 MB_OK);
		return NULL;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NextContext
//
// CLASS.......: 	SYSTEM	(static)
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

LPAPPCONTEXT		SYSTEMMIN::NextContext(
	LPAPPCONTEXT		pContext
	)
	{
	I_ASSERT(FALSE);

	return NULL;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SYSTEMMIN::GetCurrentContext
//
////////////////////////////////////////////////////////////////

LPAPPCONTEXT		SYSTEMMIN::GetCurrentContext(
	VOID
	)
  {
	I_ASSERT(FALSE);

	return NULL;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMMIN::
//
////////////////////////////////////////////////////////////////

LPFILEOBJECT		SYSTEMMIN::GetFileObject(
	VOID
	)
	{
	if (stackTop < 0 || stackTop > MAXSTACKTOP)
		return NULL;
	else
		return contextStack[stackTop];
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetFileObject
//
// CLASS.......: 	SYSTEM
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

VOID		SYSTEMMIN::SetFileObject(
	LPFILEOBJECT	newContext
	)
	{
	if (stackTop <= 0)
		{
		stackTop = 0;
		contextStack[stackTop] = newContext;
		}
	else
		{
//		MessageBox(NULL, "Can't set context, multiple items on stack",
//			"Context Stack Error", MB_OK);
		contextStack[0] = newContext;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMMIN::
//
////////////////////////////////////////////////////////////////

LPSERIUSMEMORY	SYSTEMMIN::GetTheMemory(
	VOID
	)
	{
	return &theMemory;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMMIN::GetTempStringSpace
//
////////////////////////////////////////////////////////////////

LPSTRINGSPACE		SYSTEMMIN::GetTempStringSpace(
	VOID
	)
	{
	return &tempStringSpace;
	}

//----------------------------------------------------------------------
//   SYSTEMMIN::GetNumColors
//
// Returns the number of entries in the DISPLAY's color table.
//----------------------------------------------------------------------

int		SYSTEMMIN::GetNumColors(
	VOID
	)
  {
  HDC hIC;
	
	if (!m_fColorsValid)
		{
		hIC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		numberOfColors = ::GetDeviceCaps(hIC, NUMCOLORS);
		::DeleteDC(hIC);

		m_fColorsValid = TRUE;
		}

  return numberOfColors;
  }

////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

BOOL		SYSTEMMIN::IsRuntime(
	VOID
	)
	{
	return FALSE;
	}

////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

BOOL		SYSTEMMIN::IsExeStructures(
	VOID
	)
	{
	return FALSE;
	}

