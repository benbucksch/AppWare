#include "include.h"


extern "C" LONG ALMCALLBACK PolybuttonAdminProc(
	AOBJECTID 								oiPoly,
	AObjMessage*					  theSystem)
  {
	pApQueryInfo						pQueryInfo;
	LONG										rVal = A_NOTHANDLED;
	LPPOLY									poly;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			new(sizeof(POLY), oiPoly) POLY(oiPoly, NULL, 0, I_UID_INITIAL_VALUE, TRUE);
			rVal = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			rVal = POLY::DoAssign(oiPoly, (AOBJECTID)theSystem->message3);
			break;

		case AOBJ_EDITOBJECT:
			rVal = POLY::Edit(oiPoly);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			poly = LockPoly(oiPoly);
			I_VERIFY_POINTER(poly, return A_ERROR);
			rVal = poly->ValidateObjectIDs(oiPoly);
			UnlockPoly(oiPoly);
			break;

		case AOBJ_READ:	
			rVal = POLY::Read(oiPoly);
			break;

		case AOBJ_CHECKOBJECT:
			rVal = A_OK;
			break;

		case AOBJ_BUILDSIGNALS:
			poly = LockPoly(oiPoly);
			I_VERIFY_POINTER(poly, return A_ERROR);
			rVal = poly->BuildSignals((AMEMBLOCKID)theSystem->message2);
			UnlockPoly(oiPoly);
			break;

		case AOBJ_QUERYPROTOCOL:
			pQueryInfo = (ApQueryInfo*) theSystem->message4;
			if (pQueryInfo->protocol == APROT_WND)
				{
				rVal = AP_SUPPORTED;
				pQueryInfo->pfnCreate = (APPCFN) CreateVisualInstance;
				}
			break;

		case AOBJ_IMPORTOBJECT:
			rVal = POLY::Import(oiPoly);
			break;

		case AOBJ_EXPORTOBJECT:
			poly = LockPoly(oiPoly);
			I_VERIFY_POINTER(poly, return A_ERROR);
			rVal = poly->Export();
			UnlockPoly(oiPoly);
			break;

    case AOBJ_REPORTREQUIREDFILES:
      AObjAddRequiredFile(theSystem, "RESIZE32.DLL");
      rVal = A_OK;
      break;


		}

  return rVal;
	}


////////////////////////////////////////////

