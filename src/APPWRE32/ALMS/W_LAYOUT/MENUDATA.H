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
// FILE:          "menudata.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef MENUDATA_H  // prevent multiple includes
#define MENUDATA_H

////////////////////////////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

////////////////////////////////////////////////////////////////////////////

#define MENU_MAXLENGTH    81

////////////////////////////////////////////////////////////////////////////

struct MenuString
  {
  int   iTitleWidth;
  char  szTitle[MENU_MAXLENGTH];
  };
  
////////////////////////////////////////////////////////////////////////////

class MenuData

  {
  private:
    static int    sm_nSpacing;
    
    AOBJECTID      m_oiMenuBar;
    
    int		        m_iNumMenus;
    AMEMBLOCKID    m_miMenus;
    MenuString*   m_pMenus;
  
  public:
                  MenuData(AOBJECTID oiMenuBar);
                 ~MenuData();

    inline int    NumMenus()        {return m_iNumMenus;}
    inline int    LeftOffset()      {return sm_nSpacing;}
    inline LPCSTR Title(int i)      {I_ASSERT(i>=0 && i<m_iNumMenus); return m_pMenus[i].szTitle;}
    inline int    TitleWidth(int i) {I_ASSERT(i>=0 && i<m_iNumMenus); return (m_pMenus[i].iTitleWidth + sm_nSpacing * 2);}

    void          Edit();
    void          Update(AOBJECTID oiMenuBar);
    int           Height(int nWidth);
  };

////////////////////////////////////////////////////////////////////////////

#endif    // MENUDATA_H
