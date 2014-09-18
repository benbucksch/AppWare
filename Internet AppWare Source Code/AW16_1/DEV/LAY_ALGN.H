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
// FILE:          "lay_algn.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_ALGN_H
#define LAY_ALGN_H

// required includes ///////////////////////////////////////////////////////

#ifndef PALETTE_H
#include "palette.h"
#endif

// function prototypes for using alignment palette /////////////////////////

void RegisterPaletteWindow();
void UnregisterPaletteWindow();
HWND InitAlignmentPalette(HWND hwndOwner, Layout* pLayout, BOOL fPrstPos);
void DestroyAlignmentPalette(BOOL fPrstPos);

// defines for palette sizes ///////////////////////////////////////////////

#define ALIGNMENT_WIDTH           49
#define ALIGNMENT_HEIGHT          193
#define TOOL_WIDTH                73
#define TOOL_HEIGHT               193


// Help IDs
#define ALIGNLEFT_HELPID		5000
#define ALIGNRIGHT_HELPID 		5001
#define ALIGNTOP_HELPID			5002
#define ALIGNBOTTOM_HELPID		5003
#define CENTERGROUPHORZ_HELPID		5004
#define CENTERGROUPVERT_HELPID		5005
#define CENTERHORZ_HELPID		5006
#define CENTERVERT_HELPID		5007
#define GROWWIDE_HELPID			5008
#define GROWTALL_HELPID			5010
#define SHRINKTHIN_HELPID		5009
#define SHRINKSHORT_HELPID		5011
#define SPACEVERT_HELPID		5012
#define SPACEHORZ_HELPID		5013
//#define GRID_HELPID			5014
//#define POSITIONSIZE_HELPID		5015

// AlignmentPalette ////////////////////////////////////////////////////////

class AlignmentPalette : public PaletteWindow
  {
  private:
    Layout*             m_pLayout;

    void                AddButton(UINT, char*, DWORD);
    void                Closing(void);
    void                ItemChosen(PaletteItem*);

  public:
                        AlignmentPalette(Layout*, HWND);
                       ~AlignmentPalette(void);

    void                InitPalette(void);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifdef LAY_ALGN_H
