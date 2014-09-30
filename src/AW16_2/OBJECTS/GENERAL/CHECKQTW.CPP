////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "checkqtw.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <qtw.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_number.h>

////////////////////////////////////////////////////////////////////////////

extern "C" void ALMAPI QuickTimePresent
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  static int  s_nMajorVer       = 0;
  static int  s_nMinorVer       = 0;
  static int  s_nRevision       = 0;
  static BOOL s_fQTInitialized  = FALSE;
  static BOOL s_fCalled         = FALSE;

  // setup static variables the first time this function is called
  if(!s_fCalled)
    {
    long  lVer;
    s_fCalled = TRUE;

    if((UINT)QTInitialize (&lVer) == QTI_OK)
      {
      // initialize static variables
      s_fQTInitialized = TRUE;
      s_nMajorVer = (int)((0x0000F000L & lVer) >> 12);  // Major version   -- bits 12-15
      s_nMinorVer = (int)((0x00000F00L & lVer) >>  8);  // Major version   -- bits  8-11
      s_nRevision = (int)((0x000000FFL & lVer));        // Revision number -- bits  0- 7

      QTTerminate();
      }
    }

  if(s_fQTInitialized) 
    {
    // set the output variables if QuickTime is present
    OBJECTID  oiMajorVer = AObjCreate(OTYPE_NUMBER);
    OBJECTID  oiMinorVer = AObjCreate(OTYPE_NUMBER);
    OBJECTID  oiRevision = AObjCreate(OTYPE_NUMBER);

    ONmbrSetInteger(oiMajorVer, (long)s_nMajorVer, FALSE);
    ONmbrSetInteger(oiMinorVer, (long)s_nMinorVer, FALSE);
    ONmbrSetInteger(oiRevision, (long)s_nRevision, FALSE);

    AFuncSetParameter(1L, oiMajorVer);
    AFuncSetParameter(2L, oiMinorVer);
    AFuncSetParameter(3L, oiRevision);

    AFuncReportResult(pMsg, 1L, NULL);
    }
  else
    AFuncReportResult(pMsg, 2L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
