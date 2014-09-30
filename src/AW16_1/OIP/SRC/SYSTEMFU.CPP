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

//needed so I_ASSERT will work
I_ENABLE_ASSERT; 

#define TEMP_STRING_BLOCK_SIZE	1024

//----------------------------------------------------------------------
//   SYSTEM::FindContext
//
// Map the specified Windows taskID to its context record.
// Returns 0 if no matching context is found.
//----------------------------------------------------------------------

LPAPPCONTEXT		SYSTEM::FindContext(
	HTASK			theTask
	)
  {
  LPAPPCONTEXT p;

	for (p = pFirstContext ; p ; p = p->next)
		if (p->GetTaskID() == theTask)
      {
      // Make sure Troy's MemLib globals are set up...
      p->SetGlobals();
      return p;
      }

  return 0;
  }


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
// FUNCTION....:	IsValidContext
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
BOOL		SYSTEM::IsValidContext(
	LPAPPCONTEXT		pContext
	)
	{
	BOOL				fValid;

	if (!IsTask(pContext->GetTaskID()))
		{
		UnlinkContext(pContext, FALSE);			// unlink, but don't delete
		fValid = FALSE;
		}
	else
		fValid = TRUE;

	return fValid;
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
  HTASK        taskNow = ::GetCurrentTask();
  LPAPPCONTEXT contextNow;

  contextNow = FindContext(taskNow);
  if (contextNow)
    {
    // A context for this task has already been set up.  Freshen
    // it with this more recent EXEINFO.
    contextNow->Freshen(theExeInfo);
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

    contextNow = (LPAPPCONTEXT) new APPCONTEXT(taskNow, theExeInfo);
    if (!contextNow->GetFileObject())
      {
      // Some kind of error happened during the Serius prolog
      HINSTANCE hPrevInst=NULL;
      
      if(contextNow->TestDupInstanceFlag())
      	hPrevInst = contextNow->GetPrevInstance();	
      
      delete contextNow;
      transitionInfo = 0;
      
      if(hPrevInst)
      	{
	    	HWND topWin = GetTopWindow(NULL);
  	  	HINSTANCE hInst;
    
	  	  while(topWin)
	    		{
					// hInstance is the word at offset 1Ch in the TDB    
	  			hInst = (HINSTANCE) (*((WORD far *)MK_FP(GetWindowTask(topWin), 0x1C)));
	      
	      	if(hInst == hPrevInst)
	   				{
	   				BringWindowToTop(topWin);
	        	break;
	        	}
	      
	      	topWin = GetNextWindow(topWin, GW_HWNDNEXT);
      		}
        }
      //reset the globals
      if(pCurrContext)
      	pCurrContext->SetGlobals(); 
            
      return FALSE;
      }

    LinkContext(contextNow);

    transitionInfo = 0;
    }

  pCurrContext = contextNow;

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
  HTASK						taskNow = ::GetCurrentTask();
  LPAPPCONTEXT		thisContext;
  BOOL						result;
	EXEINFOAUX			exeInfoAux;

  thisContext = FindContext(taskNow);
  if (!thisContext)
    return FALSE;

  // Temporarily hold onto the ptr to the EXEINFO in transitionInfo
  // (a static data member), so that its members will be
  // available before we have completely built the context.
  // This ensures that things work during the startup of the
  // microapp.

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
  if (pCurrContext != NULL &&
			pCurrContext->GetTaskID() == ::GetCurrentTask())
    return pCurrContext;

	// new context... swap it in
  pNewContext = FindContext(::GetCurrentTask());
  if (!pNewContext)
    return 0;

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
	return transitionInfo
         ? transitionInfo->pAuxInfo->fileObj
         : GetCurrentContext()->GetFileObject();
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

