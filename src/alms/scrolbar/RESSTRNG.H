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
// FILE:          "resstrng.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   ResString class definition.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef RESSTRNG_H
#define RESSTRNG_H

// resource string constants ///////////////////////////////////////////////

#define STR_SIGNAL_VLINEUP             10
#define STR_SIGNAL_VLINEDOWN           11
#define STR_SIGNAL_VPAGEUP             12
#define STR_SIGNAL_VPAGEDOWN           13
#define STR_SIGNAL_VTOP                14
#define STR_SIGNAL_VBOTTOM             15
#define STR_SIGNAL_HLINEUP             16
#define STR_SIGNAL_HLINEDOWN           17
#define STR_SIGNAL_HPAGEUP             18
#define STR_SIGNAL_HPAGEDOWN           19
#define STR_SIGNAL_HTOP                20
#define STR_SIGNAL_HBOTTOM             21
#define STR_SIGNAL_THUMBTRACK          22
#define STR_SIGNAL_THUMBPOSITION       23
#define STR_SIGNAL_ACTIVATED           24
#define STR_SIGNAL_DEACTIVATED         25
#define STR_SIGNAL_CHANGED             26

// class ResString /////////////////////////////////////////////////////////

class ResString

  {
  public:
                ResString(UINT uID);
               ~ResString();

    LPCSTR      String() const        {return m_szString;}

  private:
    UINT        m_uID;
    char*       m_szString;

    // unused methods for protection
                ResString(const ResString&);
    ResString&  operator=(const ResString&);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef RESSTRNG_H
