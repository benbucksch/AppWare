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
// FILE:					OIPTIME.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Interface to some timing debug routines.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OIPTIME_H
#define OIPTIME_H

#ifdef AW_I_DEBUG
// Comment this out for no time.  This is the single-point control on
// this definition.
//#define AW_I_TIME
#endif

#ifdef AW_I_TIME
void		StartHiResTimer(void);
void		EndHiResTimer(void);

long		HiResTime(void);
void		AddHandlerTime(AEVENTID, long);
void		PrintHandlerTimes(void);
#endif

#endif
