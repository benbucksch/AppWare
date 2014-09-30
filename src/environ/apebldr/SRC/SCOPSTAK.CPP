#include "almsys.h"

#include "scopstak.h"
#include "upsf_env.h"
#include "compstat.h"

SCOPESTACK::SCOPESTACK(
	HWND owner,
	LPSTR operation,
	LPSTR fileName,
	LPSTR title,
	double prePercent,
	double postPercent,
	long typeStatus):WINDOWLIST()
	{
	long size;

	size = upsfScopeLength();
	totalSize = (long)((double)size * (1.0 + prePercent + postPercent));

	compStatPtr = new COMPILESTATUS(owner, operation, fileName,
		totalSize, (long)(prePercent * (double)totalSize), title, typeStatus);
	PushScope(ST_FULL);

	}

SCOPESTACK::~SCOPESTACK()
	{
	BOOL noError = !IsListEmpty();

	while(PopScope(TRUE));

	if(noError)
		{
		compStatPtr->FillToEnd();
		compStatPtr->ShowFinish();
		}
	delete compStatPtr;
	}

void SCOPESTACK::PushScope(
	double percentage)
	{
	LPSCOPERECORD srPtr;
	long size;
	HWND dummy;

	size = upsfScopeLength();

	//get parent percent and adjust the new scopes percentage by that amount
	srPtr = (LPSCOPERECORD)GetTailObject((HWND *)&dummy);
	if(srPtr)
		percentage*=srPtr->percent;

	size = (long)((double)size * percentage);
	//now go through list and add the scope length to the amount drawn of
	//all scopes down the chain - this is so when the parent scopes close,
	//they will know how much of their total length has already been drawn.

	srPtr= NULL;
	while(NULL != (srPtr = (LPSCOPERECORD)GetNextObject(srPtr)))
		srPtr->amountDrawn+=size;

	//add new scope
	srPtr = new SCOPERECORD(size, 0, percentage);
	Add(NULL, srPtr);

	}

BOOL SCOPESTACK::PopScope(
	BOOL drawToEnd)
	{
	LPSCOPERECORD srPtr;
	HWND dummy;

	srPtr = (LPSCOPERECORD)GetTailObject((HWND *)&dummy);
	if(srPtr)
		{
		if(drawToEnd)
			{
			if(srPtr->size > srPtr->amountDrawn)
				compStatPtr->UpdateStatus(srPtr->size - srPtr->amountDrawn);
			}
		delete srPtr;
		RemoveTail();
		return TRUE;
		}
	return FALSE;
	}

void SCOPESTACK::ErrorCleanup()
	{
	while(PopScope(FALSE));
	}
