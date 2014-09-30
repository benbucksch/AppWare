// altkey.cpp

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include "keyboard.h"

//----------------------------------------------------------------------
//   AltKey
//
// Returns via flow 1 if the ALT key is down right now, else flow 2.
//----------------------------------------------------------------------

extern "C"
void ALMCALLBACK AltKey(
  pAEvtInfo  /*-- theEvent --*/,
  pAObjMessage       theSystem)
	{
	if (GetAsyncKeyState(VK_MENU) & 0x80000000L)
		theSystem->message1 = 1;
  else
    theSystem->message1 = 2;
	}
