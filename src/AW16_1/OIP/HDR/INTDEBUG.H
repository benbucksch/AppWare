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
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#ifndef INTDEBUG_H
#define INTDEBUG_H

/*
 * Define macro to get rid of 'parameter x is unused' warnings.
 */

#define I_UNUSED(x)								((void) (x))

/*
 * Define internal ASSERT macro to bring up message box if assert fails.
 * Define internal Print macro to bring up a message box containing the
 * given string.
 * I_AlWAYSASSERT will always perform the test and execute the return
 * statement if the test fails.  However, if AW_I_DEBUG is defined,
 * a message will also be displayed.
 */

#define I_ENABLE_ASSERT           //define it to no-op

#ifdef AW_I_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

int ALMAPI									ADbgInternalPrint(LPSTR, LPSTR, LONG);

#ifdef __cplusplus
}
#endif

#ifdef ENV_MAKE

#undef  I_ENABLE_ASSERT
#define I_ENABLE_ASSERT static char _APPWARE_FILE_ [] = __FILE__
#define I_ASSERT(e)			((e) ? 0 : ADbgInternalPrint(NULL, _APPWARE_FILE_, __LINE__))
#define I_ALWAYSASSERT(e,r) if(!(e)){ADbgInternalPrint(NULL, _APPWARE_FILE_, __LINE__);r;}

#else

#define I_ASSERT(e)			((e) ? 0 : ADbgInternalPrint(NULL, __FILE__, __LINE__))
#define I_ALWAYSASSERT(e,r) if(!(e)){ADbgInternalPrint(NULL, __FILE__, __LINE__);r;}

#endif

//ASSERT for header files
#define I_ASSERT_H(e)			((e) ? 0 : ADbgInternalPrint(NULL, __FILE__, __LINE__))	 

#define I_PRINT(s)			(ADbgInternalPrint(s, __FILE__, __LINE__))
#define I_VERIFY(e)			I_ASSERT(e)

#else
#define I_ASSERT(e)
#define I_ASSERT_H(e)
#define I_PRINT(e)
#define I_ALWAYSASSERT(e,r) if(!(e)) r
#define I_VERIFY(e)			e
#endif


#ifdef ADBG_DEBUG

#define ADBG_VALIDATE(e,r,s)				if (!(e)) {ADbgPrint(ADBG_ERROR, 0, "%s", (s));r;}
#define ADBG_VALIDATE_RES(e,r,h,i)  if (!(e)) {ADbgPrintRes(ADBG_ERROR, 0, h, i);r;}

#else

#define ADBG_VALIDATE(e,r,s)				if (!(e)) r
#define ADBG_VALIDATE_RES(e,r,h,i)  if (!(e)) r

#endif

#endif	/* !INTDEBUG_H */
