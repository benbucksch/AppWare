#include "l_includ.h"

extern LONG EditListObject(AOBJECTID oiList);


extern "C" LONG ALMCALLBACK ListAdminProc(
	AOBJECTID 								oiList,
	AObjMessage*					  theSystem)
	{
	LONG										rVal = A_NOTHANDLED;
	LPLIST									list;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			if ( (list = new(oiList) LIST(oiList)) != 0)
				rVal = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			if (list->Copy((AOBJECTID)theSystem->message3))
				rVal = A_OK;
			UnlockListObject(oiList);
			break;

		case AOBJ_READ:
		//Setup the virtual table
			rVal = CheckListObjectVersion(oiList);

			if (rVal != A_ERROR)
				{
				list = LockListObject(oiList);
				I_VERIFY_POINTER(list, rVal=A_ERROR;break);

				new(list) LIST(list);
				theSystem->message3 = list->GetVersion();
				UnlockListObject(oiList);
				}
      break;

		case AOBJ_EXPORTOBJECT:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			rVal = list->Export();
			UnlockListObject(oiList);
			break;

		case AOBJ_IMPORTOBJECT:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			rVal = list->Import();
			UnlockListObject(oiList);
			break;

		case AOBJ_EDITOBJECT:
			rVal = EditListObject(oiList);
			break;

		case AOBJ_VALIDATEOBJECTIDS:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			rVal = list->ValidateObjectIDs(theSystem);
			UnlockListObject(oiList);
			break;

		case AOBJ_CHECKOBJECT:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			rVal = list->CheckObject();
			UnlockListObject(oiList);
			break;

		case AOBJ_BUILDSIGNALS:
			list = LockListObject(oiList);
			I_VERIFY_POINTER(list, break);
			rVal = list->BuildSignals(theSystem);
			UnlockListObject(oiList);
			break;

		case AOBJ_QUERYPROTOCOL:
			ApQueryInfo* pQuery = (ApQueryInfo*)theSystem->message4;
			if (pQuery->protocol == APROT_WND)
				{
				pQuery->pfnCreate = (APPCFN) CreateListVI ;
				rVal = AP_SUPPORTED;
				}
			else
				rVal = A_NOTHANDLED;
			break;
		}

	return rVal;
	}
