/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
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
// FILE:					INTALM.H
//
// AUTHOR:				Scott McCart
//
// DESCRIPTION:		Contains ALM-internal information
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef INTALM_H
#define INTALM_H


/*----- general posting masks -------------------------------------*/

  #define  howPostBitsMask   0x000000FFL
  #define  howKillBitsMask   0x00FFFF00L
  #define  howWhoMask        0x0F000000L

  #define  validPostBitsMask \
                             \
     ( AEVT_ATTAIL           \
     | AEVT_ATMARK           \
     | AEVT_ATONCE )

  #define  validKillBitsMask \
                             \
     ( AEVT_KILLWHAT         \
     | AEVT_KILLTARGET       \
     | AEVT_KILLDETAILS      \
     | AEVT_KILLSOURCE       \
     | AEVT_KILLMODIFIERS    \
     | AEVT_KILLWHEN         \
     | AEVT_KILLVARIATION    \
     | AEVT_KILLSELF         \
     | AEVT_KILLALLQUEUES )

  #define  validWhoBitsMask  \
                             \
     ( AEVT_SELF             \
     | AEVT_DIRECT )

/*----- bits that may be on in 'how' ------------------------------*/

  #define  validHowBits      \
     ( validPostBitsMask     \
     | validKillBitsMask     \
     | validWhoBitsMask )


/*-----bits that should never be on in 'how' ----------------------*/

  #define  bogusHowBits        (~(validHowBits))


/*----------------------------------------------------------------*
  * AOBJ_VALIDATEOBJECTIDS flags -- indicate whether objects IDs are
  * being validated for design time or compile time.
  * (passed in 'message4' of System record)
  *----------------------------------------------------------------*/

  #define validateCompileTime   0L
  #define validateDesignTime    1L
  #define validateSubject       2L
  #define validateProject       4L


/*-----------------------------------------------------------------*
 * METHODPROC: interface for a serius method
 *-----------------------------------------------------------------*/
  typedef void (ALMCALLBACK *METHODPROC) (pAEvtInfo theEvent,
                                       pAObjMessage theSystem);

/*-----------------------------------------------------------------*
 * ADMNPROC: interface for the object administration routine
 *-----------------------------------------------------------------*/
  typedef LONG (ALMCALLBACK *ADMNPROC)   (OBJECTID theObject,
																				pAObjMessage theSystem);

/*-----------------------------------------------------------------*
 * Private Event IDs
 *-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*
 * Highword return values for AObjReportError
 *-----------------------------------------------------------------*/
	#define AHIGH_CONTINUE 		0
	#define AHIGH_CANCEL   		1

//!! Taken out until we can change the database object
//#define AEVENT_CALLFUNCTION							MAKEEVENTID('c','a','l','F')
#define AEVENT_PAUSEPROGRAM							MAKEEVENTID('p','s','e','P')
#define AEVENT_IMMEDIATETAIL						MAKEEVENTID('i','m','m','T')

#define OTYPE_NUMBER										MAKETYPEID('N','m','b','r')
#define OTYPE_TEXT											MAKETYPEID('T','e','x','t')
#define OTYPE_WINDOW										MAKETYPEID('W','i','n','o')

/*-----------------------------------------------------------------*
 * Stuff for AObjAddImplicitLink
 *-----------------------------------------------------------------*/

#define AOBJ_INT_REPORTREQUIREDFILES	 1L

typedef struct
	{
	char fileName[13];
	} Implicit_Link, *pImplicit_Link;

#endif	/* INTALM_H */
