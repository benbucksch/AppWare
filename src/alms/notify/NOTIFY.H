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
// FILE:          "notify.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the general header file containing the NOTIFY 
//                class definition and function prototypes.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

// Notify class definition /////////////////////////////////////////////////

class Notify
  {
  private:
    AOBJECTID    m_oiSelf;     // object id of the notify object
    short       m_sVer;       // current version
    BOOL        m_fNotify;    // TRUE if a window is currently flashing
    HWND        m_hwndFlash;  // handle of flashing window (or NULL for active)

  public:
    Notify(AOBJECTID oiNtfy);

    void*       operator new(size_t s, AOBJECTID oiNotify);
    void        operator delete(void* p);

    short       Version()             {return m_sVer;}
    BOOL        DoNotify()            {return m_fNotify;}
    BOOL        SetNotify(BOOL fSet)  {return m_fNotify = fSet;}
    HWND        GetWindow()           {return (m_hwndFlash==NULL) ? GetActiveWindow() : m_hwndFlash;}
    HWND        SetWindow(HWND hwnd)  {return m_hwndFlash = hwnd;}
  };

// Constants ///////////////////////////////////////////////////////////////

#define OTYPE_NOTIFY          ATYPE_MAKEID ('N', 't', 'f', 'y')
#define OEVT_NOTIFY_FLASH     AEVENT_MAKEID('n', 'O', 'f', 'l')

#define VER_OBJECT            0x0100

////////////////////////////////////////////////////////////////////////////
