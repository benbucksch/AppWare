#define   STRICT
#define   ASTRICT

#include "t_includ.h"

//////////////////////////////////////////////////

extern "C" void ALMAPI TxtEvent_Startup(
  pAEvtInfo               theEvent,
  pAObjMessage            theSystem)
  {
  TEXTRUNTIME*            tr = new TEXTRUNTIME();


  I_ASSERT_TR(tr, "TxtEvent_Startup", ;);

  I_UNUSED(tr); I_UNUSED(theEvent); I_UNUSED(theSystem);

  return;
  }

//////////////////////////////////////

extern "C" void ALMAPI TxtEvent_Shutdown(
  pAEvtInfo               theEvent,
  pAObjMessage            theSystem)
  {
  TEXTRUNTIME*            tr;

  I_UNUSED(theEvent); I_UNUSED(theSystem);

  tr = GetTextRuntime() ;
  I_ASSERT_TR(tr, "TxtEvent_Shutdown", return);

  delete tr;

  }

//////////////////////////////////////////////////////////////

#if 0
extern "C" void ALMAPI TxtEvent_SendKey(
  pAEvtInfo     theEvent,
  pAObjMessage            theSystem)
  {
  KeyStruct far       *lpKey = (KeyStruct far*)theEvent->details;
  LPSTR               lpText;
  LONG                length;

  switch (lpKey->keyFlags)
    {
    case KF_QUERY:
      lpKey->keyFlags = KF_TEXT;
      break;
    case KF_TEXT:
      lpText = (LPSTR) AObjLockData(theEvent->targetObject, CHARS_INFO);
      length = AObjGetDataSize(theEvent->targetObject, CHARS_INFO);
      length = min((LONG)MAXKEYLENGTH, length);
      _fmemcpy(lpKey->key, lpText, length);
      lpKey->key[length] = '\0';
      AObjUnlockData(theEvent->targetInfo, CHARS_INFO);
      break;
    }
  return;
  }
#endif

//////////////////////////////////////////////////////////////


extern "C" void ALMAPI TxtEvent_ObjectToText(
  pAEvtInfo               theEvent,
  pAObjMessage            theSystem)
  {
  I_UNUSED(theSystem);
  OTextGetToMemblock(theEvent->targetObject, (MEMBLOCKID) theEvent->details);
  }

///////////////////////////

extern "C" void ALMAPI TxtEvent_TextToObject(
  pAEvtInfo               theEvent,
  pAObjMessage            theSystem)
  {
  I_UNUSED(theSystem);
  OTextSetValueFromMemblock(theEvent->targetObject, (MEMBLOCKID)theEvent->details);
  }

///////////////////////////

extern "C" void ALMAPI TxtEvent_PIDR(
  pAEvtInfo               theEvent,
  pAObjMessage            theSystem)
  {
  TEXT*                   txt;

  I_UNUSED(theSystem);

  txt = LockTextStyle(theEvent->targetObject);
	I_ASSERT_TXT(txt, "TxtEvent_PIDR", return);

	txt->Print((PIDR_BLOCK*)theEvent->details);

	UnlockTextStyle(theEvent->targetObject);
  }

///////////////////////////////////////////////////
