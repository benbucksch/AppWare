#define STRICT
#define ASTRICT
#include <a_alm.h>
#include "keyboard.h"

extern "C"
void ALMCALLBACK		KeybrdOff(
  pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
  {
  AOBJECTID				targetObject;
  KBRECORD *		kbRec;

  targetObject = AFuncGetParameter(1);
  if (AObjCheckType(targetObject, OTYPE_KEYBOARD))
    {
    kbRec = (KBRECORD  *)AObjLockData(targetObject, DATA0);
    kbRec->active = 0;
    AEvtPostStandard(targetObject, AEVENT_OBJECTCHANGED, 0, AEVT_KILLDUPLICATES);
    }

  theSystem->message1 = 1;
  }
