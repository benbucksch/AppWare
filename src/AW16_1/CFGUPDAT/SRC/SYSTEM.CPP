#include <windows.h>
#include "system.h"
#include "memlib.h"
#include "strspace.h"

#define TEMP_STRING_BLOCK_SIZE	1024

///////////////////////////////////////////////////////////////////////////
//	Allocate and initialize system class static data  members
///////////////////////////////////////////////////////////////////////////

int						  SYSTEM::stackTop = -1;
LPFILEOBJECT	  SYSTEM::contextStack[MAXSTACKTOP+1];
LPSERIUSMEMORY	SYSTEM::theMemory = 0;
int						  SYSTEM::numberOfColors = 0;

LPSTRINGSPACE		SYSTEM::tempStringSpace = 0;


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
void SYSTEM::Initialize(
	void)
	{
	HDC	hDC;

	InitializeBlocks();						//initialize runtime only memory

	InitMemory();    //initialize memory saved to disk

	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

	numberOfColors =
	 1 << (GetDeviceCaps(hDC, PLANES) * GetDeviceCaps(hDC, BITSPIXEL));

	DeleteDC(hDC);

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

void SYSTEM::InitMemory(void)
	{

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
void SYSTEM::CleanUp(
	void)
	{


	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetContext
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
void SYSTEM::SetFileObject(
	LPFILEOBJECT	newContext)
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
BOOL SYSTEM::PushContext(
	LPFILEOBJECT	newContext)
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
LPFILEOBJECT SYSTEM::PopContext(
	void)
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


