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
#include "context.h"

#define TEMP_STRING_BLOCK_SIZE	1024

//----------------------------------------------------------------------
//   SYSTEM::LinkContext
//
// Hang the specified context on to our list.
//----------------------------------------------------------------------

LPAPPCONTEXT		SYSTEM::LinkContext(
	LPAPPCONTEXT		newContext
	)
  {
  newContext->next = pFirstContext;
  pFirstContext = newContext;

  return newContext;
  }

//----------------------------------------------------------------------
//   SYSTEM::UnlinkContext
//
// Pull the specified context from our list and delete it. Returns
// FALSE if the specified context is invalid.
//----------------------------------------------------------------------

BOOL	SYSTEM::UnlinkContext(
	LPAPPCONTEXT	dying,
	BOOL			fDelete				// = TRUE
	)
  {
  LPAPPCONTEXT p, q;

  if (!dying)
    return TRUE;

  // Point p to the first context on the list
  p = pFirstContext;

  // Is the first context dying?
  if (dying == p)
		{
		// Yup, kill it and go home
		q = p->next;
		pCurrContext = p;

		if (fDelete)
			delete p;

		pFirstContext = pCurrContext = q;
		if (q != 0)
			q->SetGlobals();
		return TRUE;
		}

	// No, walk the rest of the list
	for (q = p->next ; q ; q = q->next)
		{
		if (q == dying)  // kill this node and leave
			{
			p->next = q->next;
			pCurrContext = q;

			if (fDelete)
				delete q;

			pCurrContext = p;
			p->SetGlobals();
			return TRUE;
			}
		p = q; // remember the predecessor node
		}

	// being here means it was an invalid context ptr
	return FALSE;
	}

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

VOID		SYSTEMFULL::Initialize(
	VOID
	)
	{
	I_ASSERT(FALSE);
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
VOID			SYSTEMFULL::CleanUp(
	VOID
	)
	{
	I_ASSERT(FALSE);
	};

//----------------------------------------------------------------------
//    SYSTEMFULL::RegisterContext
//
// Links in a context for the current task.  If one already exists for
// this task, freshen the context info based on the passed EXEINFO.
// Returns FALSE if there is an error. (called from exeRegisterContext)
//----------------------------------------------------------------------
BOOL		SYSTEMFULL::RegisterContext(
	LPEXEINFO	theExeInfo
	)
  {
	LPAPPCONTEXT	pContext;

	if (pFirstContext != NULL)
    {
    // A context for this task has already been set up.  Freshen
    // it with this more recent EXEINFO.
    pFirstContext->Freshen(theExeInfo);
		pContext = pFirstContext;
    }
  else
    {
    // It's the first time this task has registered with us.
    // Temporarily hold onto the ptr to the EXEINFO in transitionInfo
    // (a static data member), so that its members will be
    // available before we have completely built the context.
    // This ensures that things work during the startup of the
    // microapp.
    transitionInfo = theExeInfo;

    pContext = (LPAPPCONTEXT) new APPCONTEXT(theExeInfo);
    if (!pContext->GetFileObject())
      {
      // Some kind of error happened during the Serius prolog
      delete pContext;
      transitionInfo = 0;
      return FALSE;
      }

    LinkContext(pContext);

    transitionInfo = 0;
    }

  pCurrContext = pContext;

  return TRUE;
  }

//----------------------------------------------------------------------
//    SYSTEMFULL::UnregisterContext
//
// Unregisters the serius context associated with this task.  Returns
// false if there was no context for this task, else true.
//----------------------------------------------------------------------

BOOL		SYSTEMFULL::UnregisterContext(
	VOID
	)
  {
  LPAPPCONTEXT		thisContext;
  BOOL						result;
	EXEINFOAUX			exeInfoAux;

  thisContext = pFirstContext;
  if (!thisContext)
    return FALSE;

  // Temporarily hold onto the ptr to the EXEINFO in transitionInfo
  // (a static data member), so that its members will be
  // available before we have completely built the context.
  // This ensures that things work during the startup of the
  // microapp.

//!!??	thisContext->SetGlobals();

  transitionInfo = (LPEXEINFO) new EXEINFO(&exeInfoAux);
  thisContext->GetCopyOfExeInfo(transitionInfo);
  result = UnlinkContext(thisContext);
  delete transitionInfo;
  transitionInfo = 0;

  return result;
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

BOOL		SYSTEMFULL::PushContext(
	LPFILEOBJECT	newContext
	)
	{
	I_UNUSED(newContext);
	I_ASSERT(FALSE);

	return FALSE;
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

LPFILEOBJECT	SYSTEMFULL::PopContext(
	VOID
	)
	{
	I_ASSERT(FALSE);

	return NULL;
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

LPAPPCONTEXT		SYSTEMFULL::NextContext(
	LPAPPCONTEXT		pContext
	)
	{
	if (pContext == NULL)
		pContext = pFirstContext;
	else
		pContext = pContext->next;

	return pContext;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:	SYSTEMFULL::GetCurrentContext
//
////////////////////////////////////////////////////////////////

LPAPPCONTEXT		SYSTEMFULL::GetCurrentContext(
	VOID
	)
  {
  LPAPPCONTEXT  pNewContext;

	// see if we are already in the correct context for this task
  if (pCurrContext != NULL)
    return pCurrContext;

	// new context... swap it in
  pNewContext = pFirstContext;
  if (!pNewContext)
    return 0;

//	pNewContext->SetGlobals();

  return pCurrContext = pNewContext;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMFULL::
//
////////////////////////////////////////////////////////////////

LPFILEOBJECT		SYSTEMFULL::GetFileObject(
	VOID
	)
	{
	return (transitionInfo != NULL)
         ? (transitionInfo->pAuxInfo->fileObj)
         : (GetCurrentContext()->GetFileObject());
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

VOID		SYSTEMFULL::SetFileObject(
	LPFILEOBJECT	newContext
	)
	{
  if (transitionInfo)
     transitionInfo->pAuxInfo->fileObj = newContext;
	else
     GetCurrentContext()->SetFileObject(newContext);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMFULL::
//
////////////////////////////////////////////////////////////////

LPSERIUSMEMORY	SYSTEMFULL::GetTheMemory(
	VOID
	)
	{
	return transitionInfo
         ? transitionInfo->pAuxInfo->seriusMem
         : GetCurrentContext()->GetTheMemory();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SYSTEMFULL::GetTempStringSpace
//
////////////////////////////////////////////////////////////////

LPSTRINGSPACE		SYSTEMFULL::GetTempStringSpace(
	VOID
	)
	{
	return transitionInfo
         ? transitionInfo->pAuxInfo->tempStringSpace
         : GetCurrentContext()->GetTempStringSpace();
	}

//----------------------------------------------------------------------
//   SYSTEMFULL::GetNumColors
//
// Returns the number of entries in the DISPLAY's color table.
//----------------------------------------------------------------------

int		SYSTEMFULL::GetNumColors(
	VOID
	)
  {
  HDC hIC;
	int planes;
  int bits;
  
	if (!m_fColorsValid)
		{
		hIC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		numberOfColors = ::GetDeviceCaps(hIC, NUMCOLORS);
    if(numberOfColors <= 0)
    	{
    	bits = ::GetDeviceCaps(hIC, BITSPIXEL);
    	planes = ::GetDeviceCaps(hIC, PLANES); 
    
    	numberOfColors = 1 << (planes * bits);
      }
      
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

BOOL		SYSTEMFULL::IsRuntime(
	VOID
	)
	{
	return (0 != GetCurrentContext()->IsRuntime());
	}

////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

BOOL		SYSTEMFULL::IsExeStructures(
	VOID
	)
	{
	return (GetCurrentContext()->IsExeStructures());
	}

