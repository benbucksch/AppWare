#include "l_includ.h"


extern "C" void ALMAPI ListEvent_ObjectToText(
	pAEvtInfo 							theEvent, 
	pAObjMessage						)
	{
	OBJECTID 								oiList = (OBJECTID)theEvent->targetObject;
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return);

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return);

	list->ObjectToText((MEMBLOCKID)theEvent->details);

	UnlockListObject(oiList);
	}

///////////////////////////

extern "C" void ALMAPI ListEvent_TextToObject(
	pAEvtInfo 							theEvent, 
	pAObjMessage						)
	{
	OBJECTID 								oiList = (OBJECTID)theEvent->targetObject;
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return);

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return);

	list->TextToObject((MEMBLOCKID)theEvent->details);

	UnlockListObject(oiList);
	}

