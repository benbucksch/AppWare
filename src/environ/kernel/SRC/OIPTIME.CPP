/*//////////////////////////////////////////////////////////////
//
//					Copyright 1994, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					OIPTIME.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Debug timing routines.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include "almsys.h"
#include <mmsystem.h>

#include "oiptime.h"
#include "sassoc2.h"

#ifdef AW_I_TIME

static SAssoc2*		s_pHandlerTimes;

////////////////
void		StartHiResTimer(
	void
	)
	{
	s_pHandlerTimes = new SAssoc2();
	timeBeginPeriod(1);
	}

////////////////
void		EndHiResTimer(
	void
	)
	{
	timeEndPeriod(1);
	delete s_pHandlerTimes;
	s_pHandlerTimes = NULL;
	}

////////////////
long		HiResTime(
	void
	)
	{
	return timeGetTime();
	}

////////////////
void		AddHandlerTime(
	AEVENTID		eEvent,
	long			nDeltaTime
	)
	{
	if (s_pHandlerTimes != NULL)
		{
		long*		pLong	= (long*) s_pHandlerTimes->Get(0, (long) eEvent);

		if (pLong == NULL)
			{
			pLong = new long;
			*pLong = nDeltaTime;
			s_pHandlerTimes->Add(0, (long) eEvent, pLong);
			}
		else
			*pLong = (*pLong) + nDeltaTime;
		}
	}

////////////////
void		PrintHandlerTimes(
	void
	)
	{
	if (s_pHandlerTimes != NULL)
		{
		long		nIdx	= 0;
		long*		pLong;
		char		szMsg[80];
		AEVENTID	eEvent;

		for (;;)
			{
			pLong = (long*) s_pHandlerTimes->Get(0, (long*) &eEvent, &nIdx);

			if (pLong == NULL)
				break;

			wsprintf(szMsg, "Event %c%c%c%c:  %ld\r\n",
								(char) ((((long) (void*) eEvent) >> 24) & 0xff),
								(char) ((((long) (void*) eEvent) >> 16) & 0xff),
								(char) ((((long) (void*) eEvent) >>  8) & 0xff),
								(char) ((((long) (void*) eEvent)      ) & 0xff),
								*pLong);

			OutputDebugString(szMsg);
			}
		}
	}


#endif	// AW_I_TIME

