// comkey.cpp

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include "keyboard.h"

//----------------------------------------------------------------------
//   CommandKey
//
// Windows doesn't support a Command key so we always flow out
// on 'Not Down signal'
//----------------------------------------------------------------------
extern "C"
void ALMCALLBACK	CommandKey(
  pAEvtInfo /*-- theEvent --*/,
  pAObjMessage      theSystem)
	{
	theSystem->message1 = 2;
	}
