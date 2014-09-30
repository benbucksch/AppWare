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
// FILE:					KEYBOARD.H
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Version 1:  
// Data Version 2 : May 28 , 1993 To Fix Signal problems
#define DLLVersion  1
#define DATAVersion 2

// Version which fixed Signal Problem May 28, 1993
#define CORRECTSIGNALVERSION		2 

#define ID_OBJECT								(-10)
#define ACTIVEID								201
#define IDHELP									998

#define DATA0										0

struct KBRECORD {
	long	active;
	int		version;
	};

#define primaryDownSignalID			0x1
#define secondaryDownSignalID		0x2
#define middleDownSignalID			0x4


#define OTYPE_KEYBOARD									ATYPE_MAKEID('K','e','y','b')

////////////////////////////////////////////////////////////////

// Defines for fStatus

typedef struct AppRuntimeInfo_
	{
	WORD			fStatus;
	HHOOK			hMouseHook;
	HHOOK			hKeyboardHook;
	}	KeyRuntimeInfo, * pKeyRuntimeInfo;

////////////////////////////////////////////////////////////////

// Function prototypes

BOOL	EditKey(AOBJECTID);
void	doBuildSignals(pAObjMessage);



#endif	// KEYBOARD_H
