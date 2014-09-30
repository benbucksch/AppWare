/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include "almsys.h"

#include "recblk.h"
#include "object.h"
#include "system.h"
#include "sereclib.h"
#include "objinst.h"
#include "exeobjs.h"
#include "tempobj.h"
#include "rectypes.h"
#include "exetypes.h"
#include "project.h"
#include "subject.h"
#include "objtype.h"
#include "oiputils.h"
#include "a_almutl.h"
//--------------------------------------------------------------------
//   GetObjectRecord
//--------------------------------------------------------------------
LPOBJECTRECORD GetObjectRecord(
  OBJECTID  theObject)
  {
  LPOBJECTRECORD  pObjectRecord = 0;
  void  *      pObject;
  long            type;

  if (theObject == 0)
    return 0;

  if (theObject < 0)
    return TEMPOBJECT::GetObject(theObject);

  //Call GetObject rather then GetSeriusObject because we have to check the
  //uid type here anyway

  pObject = (LPRECORDPREFIX)GetObject((SERUID)theObject);

  if (!pObject)
    return 0;

  type = ((LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->
	 recBlockUID))->GetType();

  if (type == RT_EXEOBJINST)
    pObjectRecord = (LPOBJECTRECORD)(LPEXEOBJINST)pObject;
  else if (type == RT_OBJINST)
    {
    pObjectRecord = (LPOBJECTRECORD)(LPOBJINST)pObject;
		if (pObjectRecord->objectStatus & OBJ_ALIASFLAG)
			{
			//we only want the shared object if one exists
			if(((LPOBJINST)pObject)->sharedObjectUID != 0)
      	pObjectRecord =  (LPOBJECTRECORD)(LPOBJINST)GetSeriusObject(
					((LPOBJINST)pObject)->sharedObjectUID);
      }
    }

  return pObjectRecord;
  }


//--------------------------------------------------------------------
//   GetObjectsTypeRecord
//--------------------------------------------------------------------
LPBASETYPE GetObjectsTypeRecord(
  OBJECTID  theObject)
  {
  LPOBJECTRECORD  pObjectRecord;
  LPBASETYPE      pBaseType = 0;
  void  *      pObject;
  long            type;

  if (theObject == 0)
    return 0;

  if (theObject < 0)
		{
    pObjectRecord = TEMPOBJECT::GetObject(theObject);
		pBaseType = (LPEXEOBJTYPE) GetObject(pObjectRecord->GetTypeRecUID());
		}
	else
		{
		//Call GetObject rather then GetSeriusObject because we have to check the
		//uid type here anyway

		pObject = (LPRECORDPREFIX)GetObject((SERUID)theObject);

		if (!pObject)
			return 0;

		type = ((LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->
									recBlockUID))->GetType();

	  if (type == RT_EXEOBJINST)
			{
			pObjectRecord = (LPOBJECTRECORD)(LPEXEOBJINST)pObject;
			pBaseType     = (LPEXEOBJTYPE) GetObject(pObjectRecord->GetTypeRecUID());
			}
		else if (type == RT_OBJINST)
			{
			pObjectRecord = (LPOBJECTRECORD)(LPOBJINST)pObject;
			pBaseType = (LPOBJTYPE) GetObject(pObjectRecord->GetTypeRecUID());
			
			if(((LPOBJTYPE)pBaseType)->TestBadTypeFlag() ||
				(pBaseType->GetAdmnProc() == NULL))
				pBaseType = NULL;
			}
		}

  return pBaseType;
  }

//----------------------------------------------------------------------
//   GetTypeRecord
//----------------------------------------------------------------------
LPBASETYPE GetTypeRecord(TYPEID  theTypeID)
  {
  int             i=0;
  LPBASETYPE      pBaseType;
  SERUID          uidHeadRec;
  RUNTIMEUID      recordBlock;

  pTheSystem->GetCurrentContext();

  if (pTheSystem->IsExeStructures())
    {
    uidHeadRec = pTheSystem->GetFileObject()->GetHeadExeRec();
    recordBlock = ((LPEXEHEAD)GetObject(uidHeadRec))->GetExeObjTypeList();
    while (0 != (pBaseType = (LPBASETYPE)(LPEXEOBJTYPE)GetObject(recordBlock, i)))
      {
      if (theTypeID == pBaseType->GetTypeID())
        break;
      }
    }
  else
    {
    //this is for the design time type
    uidHeadRec = pTheSystem->GetFileObject()->GetHeadRec();
    recordBlock = ((PROJECT *)GetObject(uidHeadRec))->typeBlock;
    while (0 != (pBaseType = (LPBASETYPE)(LPOBJTYPE)GetObject(recordBlock, i)))
      {
      if (theTypeID == pBaseType->GetTypeID())
        break;
      }
		}

  return pBaseType;

  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SendMessageToType
//
// DESC:			Sends the given ADMN message to all objects of the type.
//
////////////////////////////////////////////////////////////////

static VOID		SendMessageToType(
	AObjMessage*	pMsg,
	TYPEID				tid
	)
	{
  ADMNPROC			admnProc;
  LPBASETYPE		pType;

	pType = GetTypeRecord(tid);

	if ((pType != NULL)                             &&
			((admnProc = pType->GetAdmnProc()) != NULL) &&
			(admnProc != NULL))
		{
		if (pTheSystem->IsExeStructures())		// type holds list of objects
			{
			RUNTIMEUID		exeObjInstList	= ((LPEXEOBJTYPE) pType)->GetExeObjInstList();
			int						idx							= 0;
			LPEXEOBJINST	pObject;

			// Type holds onto all objects of that type
			while ((pObject = (LPEXEOBJINST) GetObject(exeObjInstList, idx)) != NULL)
				{
				if (admnProc(::GetUID(pObject).id, pMsg) != A_CONTINUE)
					break;
				}
			}

		else
			{
			int						idx;
			int						idx2;
			LPPROJECT			pProject;
			LPSUBJECT			pSubject;
			LPSUBTYPE			pSubjectType;
			LPOBJINST			pObject;
			RUNTIMEUID		subjectBlock;
			RUNTIMEUID		dataBlock;
			SERUID				idType;
			LONG					lCount;

			pProject = (LPPROJECT) GetObject(pTheSystem->GetFileObject()->GetHeadRec());
			subjectBlock = pProject->GetSubjectBlock();

			idx = 0;
			while ((pSubject = (LPSUBJECT) GetObject(subjectBlock, idx)) != NULL)
				{
				lCount    = 0;
				dataBlock = pSubject->GetSubTypesList();
				idx2      = 0;

				while ((pSubjectType = (LPSUBTYPE) GetObject(dataBlock, idx2)) !=
								NULL)
					{
					if (pSubjectType->typeID == tid)
						{
						lCount = pSubjectType->objCount;
						idType = ::GetUID(pSubjectType);
						break;
						}
					}

				dataBlock = pSubject->GetObjectList();
				idx2 = 0;
				while ((lCount > 0) &&
							 ((pObject = (LPOBJINST) GetObject(dataBlock, idx2)) != NULL))
					{
					if (pObject->GetTypeRecUID() == idType)
						{
						if (!pObject->TestAliasFlag())
							{
							if (admnProc(::GetUID(pObject).id, pMsg) != A_CONTINUE)
								break;
							}
						lCount--;
						}
					}
				}
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	SendMessageToObject
//
// DESC:			Sends the given ADMN message to the specified object(s).  If
//						oid and tid are NULL, then all objects are sent the message.
//						If oid is non-NULL, then that particular object receives the
//						message.  If tid is non-NULL, then all objects of that type
//						receive the message.
//						
//						forceToExeObj forces the message to go to the exe object.
//						This message is only valid if oid and tid are both NULL.
//						IMPORTANT: the copyUID field of the object must be set in
//						forceToExeObj is TRUE.
//						
//
//						This function works with both runtime and design structures.
//						This is internal-only.  It assumes the context has been set up
//						already.
//
////////////////////////////////////////////////////////////////

VOID		DoObjectSendMessage(
	LONG				lMsg1,					// main message
	LONG&				lMsg2,					// extra info
	LONG&				lMsg3,					// extra info
	LONG&				lMsg4,					// extra info
	OBJECTID		oid,						
	TYPEID			tid,
	BOOL				forceToExeObj							
	)
	{
  ADMNPROC			admnProc;
	AObjMessage		systemRec;
  LPBASETYPE		pType;

	systemRec.message1 = lMsg1;
	systemRec.message2 = lMsg2;
	systemRec.message3 = lMsg3;
	systemRec.message4 = lMsg4;

	I_ASSERT(!(((oid != NULL) || (tid != NULL)) && forceToExeObj));
	
	if (oid != NULL)						// send to exactly one object
		{
		pType = GetObjectsTypeRecord(oid);

		if ((pType != NULL)                             &&
				((admnProc = pType->GetAdmnProc()) != NULL) &&
				(admnProc != NULL))
			{
			admnProc(oid, &systemRec);

			lMsg2 = systemRec.message2;
			lMsg3 = systemRec.message3;
			lMsg4 = systemRec.message4;
			}
		}

	else if (tid != NULL)				// send to all of a given type
		{
		SendMessageToType(&systemRec, tid);
		}

	else												// send to all
		{
		if (pTheSystem->IsExeStructures())
			{
			LONG			idx		= AOBJ_GETFIRSTKEY;

			while ((tid = ATypeGetNext(&idx)) != NULL)
				SendMessageToType(&systemRec, tid);
			}
		else
			{
			int						idx;
			int						idx2;
			LPPROJECT			pProject;
			LPSUBJECT			pSubject;
			LPOBJINST			pObject;
			RUNTIMEUID		subjectBlock;
			RUNTIMEUID  	dataBlock;

			pProject = (LPPROJECT) GetObject(pTheSystem->GetFileObject()->GetHeadRec());
			subjectBlock = pProject->GetSubjectBlock();

			idx = 0;
			while ((pSubject = (LPSUBJECT) GetObject(subjectBlock, idx)) != NULL)
				{
				dataBlock = pSubject->GetObjectList();
				idx2 = 0;
				while ((pObject = (LPOBJINST) GetObject(dataBlock, idx2)) != NULL)
					{
					if (!pObject->TestAliasFlag())
						{
            //we want the copyUID if we're forcing to an exe object. 
						oid = (forceToExeObj)?
							pObject->copyUID.id :
							::GetUID(pObject).id;
              
						pType = (LPOBJTYPE) GetObject(pObject->GetTypeRecUID());

						if ((pType != NULL)                             &&
								((admnProc = pType->GetAdmnProc()) != NULL) &&
								(admnProc != NULL))
							{
							admnProc(oid, &systemRec);
							}
						}
					}
				}
			}
		}
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			CenterRectangle
//						 
// PARAMETERS:		
//
// DESCRIPTION:		centers r1 in r2
//
// RETURNS:
/////////////////////////////////////////////////////////////
void CenterRectangle(
	LPRECT r1,
	const LPRECT r2)
	{
	int r1Width  = r1->right  - r1->left;
	int r1Height = r1->bottom - r1->top;

	r1->left = (r2->right + r2->left - r1Width) / 2;
	r1->right = r1->left + r1Width;

	r1->top = (r2->bottom + r2->top - r1Height)/2;
	r1->bottom = r1->top + r1Height;
	} 


/////////////////////////////////////////////////////////////
// FUNCTION:			CenterDialog
//						 
// PARAMETERS:
//
// DESCRIPTION:	 	centers the dialog box hDlg, in window 
//								centeringHwnd.  If centering hWnd is NULL,
//								the dialog is centered in the active window.
//
// RETURNS:
/////////////////////////////////////////////////////////////
void CenterDialog(
	HWND hDlg,
	HWND centeringHwnd,
	BOOL redraw)
	{
	RECT	winRect;
	RECT	screenRect;
	HWND	hParent;
	int		smWidth;
	int		smHeight;
	int		nDlgWidth;
	int		nDlgHeight;

	GetWindowRect(hDlg, &winRect);

	if(!centeringHwnd)
		centeringHwnd = GetActiveWindow();

	if(!centeringHwnd)
		centeringHwnd = GetDesktopWindow();

	GetWindowRect(centeringHwnd, &screenRect);

	hParent = GetParent(centeringHwnd);

	if (hParent != NULL)
		{
		//screenRect is in client corrdinates of it's centeringHwnd's parent window.
		ClientToScreen(hParent, (LPPOINT)&screenRect);
		ClientToScreen(hParent, (LPPOINT)&screenRect.right);
		}
	
	CenterRectangle(&winRect, &screenRect);

	smWidth  = GetSystemMetrics(SM_CXSCREEN);
	smHeight = GetSystemMetrics(SM_CYSCREEN);

	// Need to record original width and height since we might be adjusting
	//  the rectangle's width (just an efficiency concern).
	nDlgWidth  = winRect.right  - winRect.left;
	nDlgHeight = winRect.bottom - winRect.top;

	if (winRect.left < 0)
		winRect.left = 0;
	else if (winRect.right > smWidth)
		{
		winRect.right = smWidth - 3;				//!! change to .1" or something
		winRect.left  = smWidth - nDlgWidth;
		}

	if (winRect.top < 0)
		winRect.top = 0;
	else if (winRect.bottom > smHeight)
		{
		winRect.bottom = smHeight - 3;			//!! change to .1" or something
		winRect.top    = smHeight - nDlgHeight;
		}

	//dialogs always moved in screen coordinates
	MoveWindow(hDlg, winRect.left, winRect.top, nDlgWidth, nDlgHeight, redraw);
	} 

///////////////////////////////////////////////////////////////////
// Returns the operating system under which we're running.
///////////////////////////////////////////////////////////////////
WORD ALMAPI AUtlGetOSVersion()
	{
	OSVERSIONINFO osv;
  WORD rVal = AUTL_OS_UNKNOWN;
	
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  
  if(GetVersionEx(&osv))
  	{
    switch(osv.dwPlatformId)
    	{
      case 1: // VER_PLATFORM_WIN32_WINDOWS:	(just guessing at the value)
      	rVal = AUTL_OS_CHICAGO;
      	break;
        
      case VER_PLATFORM_WIN32_NT:
      	rVal = AUTL_OS_WINNT;
     	}
    
    }
  
  
  	// Alternate method from Tom Freeman if above method doesn't work
    
		// Get OS version info, and store in a 
		// global variable.
		// dwVersion = GetVersion();
		// Is there anything specific we need to 
		// do on different platforms?

	 //	if (dwVersion < 0x80000000) 
		//{
		// Windows NT.
	 //	} 
	 //	else if (LOBYTE(LOWORD(dwVersion))<4) 
	 //	{
		// Win32s.
	 //	} 
	 //	else 
	 //	{
		// Chicago.
	 //	}
  
  
  
  I_ASSERT(rVal != AUTL_OS_UNKNOWN);
  
  return rVal;
	}