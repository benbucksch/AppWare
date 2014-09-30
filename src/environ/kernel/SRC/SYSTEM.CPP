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

#define TEMP_STRING_BLOCK_SIZE	1024

///////////////////////////////////////////////////////////////////////////
//	Allocate and initialize system class static data  members
///////////////////////////////////////////////////////////////////////////

LPAPPCONTEXT		SYSTEM::pFirstContext											= 0;
LPAPPCONTEXT		SYSTEM::pCurrContext											= 0;
LPEXEINFO				SYSTEM::transitionInfo										= 0;
BOOL						SYSTEM::m_fColorsValid										= FALSE;

int							SYSTEM::stackTop													= -1;
LPFILEOBJECT		SYSTEM::contextStack[MAXSTACKTOP + 1];
SERIUSMEMORY		SYSTEM::theMemory;
int							SYSTEM::numberOfColors										= 0;

STRINGSPACE			SYSTEM::tempStringSpace(TEMP_STRING_BLOCK_SIZE);
LPMASTERTYPELIST SYSTEM::masterTypeList 									=NULL;
