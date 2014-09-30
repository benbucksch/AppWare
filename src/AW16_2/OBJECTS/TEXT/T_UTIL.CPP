#include "t_includ.h"

//////////////////////////////////////////////////////////
// This function returns the length of a string not
// counting <LF>s
UINT ParameterStringLength(
	LPSTR										szText)
	{
	UINT										rVal=0;

	while(*szText)
		{
		if (*szText != LF)
			rVal++;
		szText++;
		}
	return rVal;
	}

//////////////////////////////////////////////////////////
// This function takes a one-based position and count
// and figures out the actual position and count.
// The incomming position and count do not account for 
// <CR><LF>, and the outgoing position and count do.
// Both position and count must be >=1
////////////////////////////////////////////////////////
BOOL ParameterPosAndCountToActual(
	LPSTR										szText, 
	LPUINT									ptrPos,  
	LPUINT									ptrCount) //Can be NULL!!
	{
	LPSTR										cp;
	UINT										i, pos, count=0;
	BOOL										rVal;	 //whether or not pos is within the string

	pos = (*ptrPos);		 //pos = (*ptrPos)-1;
	if (pos >=1) 	
		pos--;
	
	for(cp=szText,i=0; *cp && i<pos; i++,cp++)
		{
		if (*cp==CR)
			pos++;
		}
	if (*cp)
		{
		*ptrPos = pos;
		rVal = TRUE;
		}
	else
		{
		*ptrPos = i;  //if we got to the end of the string first, pos gets set to the end of the string
		rVal = FALSE;
		}

	if (ptrCount)
		{
		count = *ptrCount;
		for(i=0; *cp && i<count; i++,cp++)
			{
			if (*cp==CR)
				count++;
			}
		if (*cp)
			*ptrCount = count;
		else
			*ptrCount = i; //if we got to the end of the string first, count gets the max char posible
		
		I_ASSERT(*ptrPos + *ptrCount <= lstrlen(szText));
		}

	I_ASSERT(*ptrPos <= lstrlen(szText) );

	return rVal;
	}			 

//////////////////////////////////////////////////////////
// This function takes an actual position and count and
// figures out the one-based position and count.  
// The incomming position and count account for 
// <CR><LF>, and the outgoing position and count do NOT.
////////////////////////////////////////////////////////
void ActualPosAndCountToParameter(
	LPSTR										szText, 
	LPUINT									ptrPos,  
	LPUINT									ptrCount) //Can be NULL!!
	{
	LPSTR										cp;
	UINT										i, pos, posOrig, count;

	posOrig = pos = (*ptrPos)+1;

	for(cp=szText,i=0; *cp && i<posOrig; i++,cp++)
		{
		if (*cp==LF)
			pos--;
		}
	*ptrPos = pos;

	if (ptrCount)
		{
		I_ASSERT( *ptrPos >= 1);

		count = *ptrCount;
		for(i=0; *cp && i<count; i++,cp++)
			{
			if (*cp==LF)
				count--;
			}
		*ptrCount = count;
		}

	}

/////////////////////////////////////////////
// This function takes a string and checks to
// see if the word 'NO' is in it.  It is used
// by the functions 'Find/Replace' and 'Compare Text'
BOOL IsCaseSensitive(
	LPSTR									szText)
	{
	BOOL									rVal=TRUE;

	while (*szText && rVal)
		{
		if ( tolower(*szText) == 'n'  &&  tolower(*(szText+1)) == 'o')
			rVal = FALSE;
		szText++;
		}
	return rVal;
	}

//////////////////////////////////////////////////////////
LONG PostObjectChanged(
	OBJECTID								oiText, 
	LONG 										lModifiers, 
	BOOL_POSTEDITED					postEdited,
	BOOL_POSTWITHDELAY			postWithDelay)
	{
	AEvtInfo   							event;

	if (postEdited ==  POSTEDITED)
		AEvtPostSignalAtMark(oiText, I_SIG_EDITED); 	//Post the edited signal

	event.sourceObject = event.targetObject = oiText;
	event.when         = postWithDelay==DELAY ? AAppGetTime() + 60 : 0;
	event.variation		 = 0;

	event.what         = AEVENT_OBJECTCHANGED;
	event.details      = 0;
	event.modifiers    = lModifiers;
	event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE);
	AEvtPost(&event);		//Post ObjectChanged...

	event.what         = AEVENT_SIGNAL;
	event.details      = I_SIG_CHANGED;
	event.modifiers    = 0L;
	event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE | AEVT_KILLDETAILS);
	AEvtPost(&event);		//Posts the changed signal

	return event.when;
	}
