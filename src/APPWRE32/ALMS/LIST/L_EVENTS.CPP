#include "l_includ.h"


extern "C" void ALMAPI ListEvent_ObjectToText(
	pAEvtInfo 							theEvent, 
	pAObjMessage						)
	{
	AOBJECTID 								oiList = (AOBJECTID)theEvent->targetObject;
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return);

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return);

	list->ObjectToText((AMEMBLOCKID)theEvent->details);

	UnlockListObject(oiList);
	}

///////////////////////////

extern "C" void ALMAPI ListEvent_TextToObject(
	pAEvtInfo 							theEvent, 
	pAObjMessage						)
	{
	AOBJECTID 								oiList = (AOBJECTID)theEvent->targetObject;
	LPLIST 									list;

	I_VERIFY_TYPEID(oiList, OTYPE_LIST, return);

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return);

	list->TextToObject((AMEMBLOCKID)theEvent->details);

	UnlockListObject(oiList);
	}

