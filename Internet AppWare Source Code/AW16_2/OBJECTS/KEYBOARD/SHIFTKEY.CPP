// shiftkey.cpp

#define STRICT
#define ASTRICT

#include <a_alm.h>
#include "keyboard.h"

//----------------------------------------------------------------------
//   ShiftKey
//
// Returns via flow 1 if the SHIFT key is down right now, else flow 2.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK		shiftKey(
  pAEvtInfo  /*-- theEvent --*/,
  pAObjMessage       theSystem)
	{
	if (GetAsyncKeyState(VK_SHIFT) & 0x80000000L)
		theSystem->message1 = 1;
  else
    theSystem->message1 = 2;
	}
