#define STRICT
#define ASTRICT

#include <a_alm.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_number.h>

#define OTYPE_SPED ATYPE_MAKEID('S', 'P', 'E', 'D')

#ifdef __BORLANDC__
extern int              _WinAllocFlag;
#endif

#define SPEED_DEFAULTITERATIONS 10000L

//parameters
#define SPEED_INPUTINDEX        1
#define SPEED_OUTPUTINDEX 1

//signals
#define SPEED_KEEPITERATING             1
#define SPEED_AFTERITERATIONS   2

//globals
DWORD speed_startTime;
DWORD speed_currentIteration = 0;
DWORD 	speed_iterations = SPEED_DEFAULTITERATIONS;


extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE		, //	hInstDLL,
	DWORD				dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}



/////////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_StartTime(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
  {
	I_UNUSED(theEvent);

  theSystem->message1 = 1;
  
  speed_startTime = GetTickCount();
  }

///////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_Iteration(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	if(speed_currentIteration < speed_iterations)
		{
		speed_currentIteration++;
		theSystem->message1 = SPEED_KEEPITERATING;
		}
	else
		theSystem->message1 = SPEED_AFTERITERATIONS;

	}


///////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_StopTime(
  pAEvtInfo                             theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	DWORD currentTime = GetTickCount();
	char buffer[256];

	wsprintf(buffer, "# milliseconds for %lu iterations	= %lu",
		(long)speed_iterations, (long)(currentTime - speed_startTime));
	
	MessageBox(NULL, buffer, "SPEED TEST", MB_ICONINFORMATION|MB_TASKMODAL|
		MB_OK);
	
	speed_currentIteration = 0;
	
	theSystem->message1 = 1;
	}


///////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_Input(
  pAEvtInfo       theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	AFuncGetParameter(SPEED_INPUTINDEX);

	theSystem->message1 = 1;
	}

///////////////////////////////////////////////////////
//the input must be hooked to a valid object
extern "C" void ALMCALLBACK speed_InputOutput(
  pAEvtInfo             theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	AOBJECTID id = AFuncGetParameter(SPEED_INPUTINDEX);

	AFuncSetParameter(SPEED_OUTPUTINDEX, id);

	theSystem->message1 = 1;
	}

///////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_OutputTemporary(
  pAEvtInfo       theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	AOBJECTID id = AObjCreate(OTYPE_TEXT);

	AFuncSetParameter(SPEED_OUTPUTINDEX, id);

	theSystem->message1 = 1;
	}



///////////////////////////////////////////////////////
extern "C" void ALMCALLBACK speed_CreateDestroyTemp(
  pAEvtInfo       theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);

	AOBJECTID id = AObjCreate(OTYPE_SPED);

	AObjDestroy(id);

	theSystem->message1 = 1;
	}

///////////////////////////////////////////////////////
//the input must be hooked to a valid object
extern "C" void ALMCALLBACK speed_SetIterations(
  pAEvtInfo             theEvent,
  pAObjMessage    theSystem)
	{
	I_UNUSED(theEvent);
	DWORD newIterations;

	AOBJECTID id = AFuncGetTypedParameter(1, OTYPE_NUMBER);

	newIterations = ONmbrGetInteger(id);

	if(newIterations > 0)
		speed_iterations = newIterations;

	theSystem->message1 = 1;
	}


////////////////////////////////////////////////////////////////
//object stuff
////////////////////////////////////////////////////////////////
extern "C" LONG ALMCALLBACK speed_ADMN(
	AOBJECTID                        theObject,
	pAObjMessage    theSystem)
	{
	switch(theSystem->message1)
		{
		case AOBJ_CREATED:

			AObjSetDataCount(theObject, 2);
			break;
		}
	return A_OK;
	}



