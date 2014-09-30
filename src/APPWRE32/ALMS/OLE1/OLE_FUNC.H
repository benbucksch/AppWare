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
// FILE:					OLE_FUNC.H
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Appware function definition file.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef OLE_FUNC_H
#define OLE_FUNC_H

////////////////////////////////////////////////////////////////
//
// Function parameter and flow "declarations."  The syntax of the
// macros is as follows:
//
//		OLEF_<func>_<type>_<id>
//
//	where:
//		<func> is a three letter abbreviation of the function;
//		<type> is one of IN, OUT, FLOW; and
//		<id>   is a unique, meaningful name for the parameter or flow.
//
////////////////////////////////////////////////////////////////

////////////////// Function description for CheckOleObject

#define OLEF_CHK_IN_OLEOBJECT			1

#define OLEF_CHK_FLOW_ACTIVE			1
#define OLEF_CHK_FLOW_INACTIVE		2
#define OLEF_CHK_FLOW_ERROR				3

////////////////// Function description for CreateOleObject

#define OLEF_CRE_IN_OLEOBJECT			1
#define OLEF_CRE_IN_TYPE					2
#define OLEF_CRE_IN_LINKFILE			3
#define OLEF_CRE_IN_LINKITEM			4

#define OLEF_CRE_FLOW_AFTER				1
#define OLEF_CRE_FLOW_ERROR				2

////////////////// Function description for DeleteOleObject

#define OLEF_DEL_IN_OLEOBJECT			1

#define OLEF_DEL_FLOW_AFTER				1
#define OLEF_DEL_FLOW_NOOBJECT		2
#define OLEF_DEL_FLOW_ERROR				3

////////////////// Function description for ActivateOleObject

#define OLEF_ACT_IN_OLEOBJECT			1
#define OLEF_ACT_IN_VERB					2

#define OLEF_ACT_FLOW_AFTER				1
#define OLEF_ACT_FLOW_NOOBJECT		2
#define OLEF_ACT_FLOW_ERROR				3

////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

extern "C" {

VOID ALMCALLBACK		CheckOleObject(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	);

VOID ALMCALLBACK		CreateOleObject(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	);

VOID ALMCALLBACK		DeleteOleObject(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	);

VOID ALMCALLBACK		ActivateOleObject(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	);

}

#endif	// !OLE_FUNC_H
