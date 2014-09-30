
#include "n_includ.h"


/////////////////////////////////////

extern "C" void ALMAPI NmbrEvent_ObjectToText(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
  LPNUMBER								num;
	AOBJECTID								oiNumber;
	LPCSTR										szSource;
	LPSTR										szDest;
	AMEMBLOCKID							miDest;
  int											len;

	I_UNUSED(theSystem);

	oiNumber = (AOBJECTID)theEvent->targetObject;
	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "NmbrEvent_ObjectToText", return);

	szSource = num->GetStringPtr();
	UnlockNumberObject(oiNumber);

	len = lstrlen(szSource)+1;

	miDest = (AMEMBLOCKID) theEvent->details;
	AMemSetSize(miDest, len);
	szDest = (LPSTR)AMemLock(miDest);
	lstrcpy(szDest, szSource);
  AMemUnlock(miDest);
	}

///////////////////////////

extern "C" void ALMAPI NmbrEvent_TextToObject(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	LPNUMBER								num;
	AOBJECTID								oiNumber;
	LPCSTR									szSource;
	AMEMBLOCKID							miSource;

	I_UNUSED(theSystem);

	oiNumber = (AOBJECTID)theEvent->targetObject;
	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "NmbrEvent_TextToObject", return);

	miSource = (AMEMBLOCKID) theEvent->details;
	szSource = (LPCSTR)AMemLock(miSource);
	num->SetValue(szSource, POST_SIGNALS, CHANGE_IF_PSEUDO, BLANK_IF_EMPTY_STRING);

	UnlockNumberObject(oiNumber);
  AMemUnlock(miSource);
	}

///////////////////////////////////////////////////
