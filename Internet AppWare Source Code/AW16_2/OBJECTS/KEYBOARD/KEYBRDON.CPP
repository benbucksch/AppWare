#define STRICT
#define ASTRICT

#include <a_alm.h>
#include "keyboard.h"

extern "C"
void ALMCALLBACK		keybrdOn(
  pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
  {
  OBJECTID				targetObject;
  KBRECORD FAR*		kbRec;

  targetObject = AFuncGetParameter(1);
  if (AObjCheckType(targetObject, OTYPE_KEYBOARD))
    {
    kbRec = (KBRECORD *)AObjLockData(targetObject, DATA0);
    kbRec->active = 1;
    AEvtPostStandard(targetObject, AEVENT_OBJECTCHANGED, 0, AEVT_KILLDUPLICATES);
    }

  theSystem->message1 = 1;
  }
