// cntrlkey.cpp

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include "keyboard.h"

//----------------------------------------------------------------------
//   CntrlKey
//
// Returns via flow 1 if the CTRL key is down right now, else flow 2.
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	CntrlKey(
  pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
	{
	if (GetAsyncKeyState(VK_CONTROL) & 0x80000000L)
		theSystem->message1 = 1;
  else
    theSystem->message1 = 2;
	}



