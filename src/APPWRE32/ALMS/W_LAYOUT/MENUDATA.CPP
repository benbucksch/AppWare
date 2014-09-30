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
// FILE:          "menudata.cpp"
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

#include <windows.h>
#include <string.h>

#include <a_alm.h>
#include <intdebug.h>

#include <o_menus.h>

#include "menudata.h"

////////////////////////////////////////////////////////////////////////////

int MenuData::sm_nSpacing = 0;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MenuData Constructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

MenuData::MenuData
  (
  AOBJECTID  oiMenuBar
  )
  
  {
  if(sm_nSpacing<=0)
    sm_nSpacing = GetSystemMetrics(SM_CXVSCROLL)/2;
    
  m_nMenus  = 0;
  m_miMenus = AMemAllocate(0L);
  m_pMenus  = NULL;
  
  Update(oiMenuBar);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MenuData Destructor
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

MenuData::~MenuData
  (
  )
  
  {
  if(m_pMenus)
    AMemUnlock(m_miMenus);
  AMemFree(m_miMenus);
  }
     
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MenuData::Edit
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void MenuData::Edit
  (
  )
  
  {
  AObjEdit(m_oiMenuBar);  
  Update(m_oiMenuBar);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MenuData::Update
//
//  DESCRIPTION:
//
////////////////////////////////////////////////////////////////////////////

void MenuData::Update
  (
  AOBJECTID  oiMenuBar
  )
  
  {
  HDC               hdc       = GetDC(NULL);
  HFONT             hfont     = (HFONT)GetStockObject(SYSTEM_FONT); // no need to delete stock objects
  HFONT             hfontOld  = (HFONT)SelectObject(hdc, hfont);
  MENUSPARAMBLOCK   mpb       = {oiMenuBar, NULL, 0};
  int               i;
  RECT              rc        = {0, 0, 0, 0};
  
  m_oiMenuBar = oiMenuBar;
  if(m_pMenus)
    AMemUnlock(m_miMenus);    

  AEvtCallMethod(NULL, mpb.objID, NULL, OEVENT_GETMENUHANDLE, (long)&mpb);

  m_nMenus = mpb.hMenu ? GetMenuItemCount(mpb.hMenu) : 0;
  AMemSetSize(m_miMenus, m_nMenus*sizeof(MenuString));
  m_pMenus = (MenuString*)AMemLock(m_miMenus);

  // store the strings and their widths  
  for(i=0; i<m_nMenus; i++)
    {
    GetMenuString(mpb.hMenu, i, m_pMenus[i].szTitle, MENU_MAXLENGTH, MF_BYPOSITION);

    DrawText(hdc, m_pMenus[i].szTitle, -1, &rc, DT_CALCRECT | DT_SINGLELINE | DT_VCENTER);

    m_pMenus[i].nTitleWidth = rc.right;
    }
  
  AEvtCallMethod(NULL, mpb.objID, NULL, OEVENT_RELEASEMENUHANDLE, (long)&mpb);

  SelectObject(hdc, hfontOld);
  ReleaseDC(NULL, hdc);
  }
  
////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       MenuData::Height
//
//  DESCRIPTION:  This method will return the height of the menu bar based
//                on the window width.
//
////////////////////////////////////////////////////////////////////////////

int MenuData::Height
  (
  int nWidth
  )

  {
  int   nMenu   = GetSystemMetrics(SM_CYMENU)+1;
  int   nLines  = 0;
  int   i       = 0;
  int   nPos    = sm_nSpacing;
  BOOL  fFirst  = TRUE;

  while(i < m_nMenus)
    {
    if(fFirst)
      {
      // always have at least one menu on each line
      nPos += TitleWidth(i);
      fFirst = FALSE;
      nLines++;
      i++;
      }
    else if((nPos + TitleWidth(i)) <= nWidth)
      {
      nPos += TitleWidth(i);
      i++;
      }
    else
      {
      nPos   = sm_nSpacing;
      fFirst = TRUE;
      }
    }

  return ((nLines * nMenu) - 1);
  }

////////////////////////////////////////////////////////////////////////////
