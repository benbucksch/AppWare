////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          SHAPEPAL.H
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Contains definitions for shape palette items.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SHAPEPAL_H
#define SHAPEPAL_H

#ifndef O_WNDSHP_H
#include <o_wndshp.h>
#endif

#ifndef PALETTE_H
#include "palette.h"
#endif

#ifndef SHP_ATTR_H
#include "shp_attr.h"
#endif




// Help IDs
#define SELECTION_HELPID		4000
#define FILLOVAL_HELPID			4010
#define UNFILLOVAL_HELPID		4010
#define REORDER_HELPID			4001
#define FILLBOX_HELPID			4011
#define UNFILLBOX_HELPID		4011
#define TABSTOP_HELPID			4002
#define FILLROUNDEDBOX_HELPID		4012
#define UNFILLROUNDEDBOX_HELPID		4012
#define GROUPSTOP_HELPID		4003
#define LINE_HELPID			4013
#define TEXT_HELPID			4014
#define VISIBILITY_HELPID		4004
#define GROUPBOX_HELPID			4007
#define SELECTALL_HELPID		4008
#define LOCK_HELPID			4005
#define ATTACH_HELPID			4006
#define BACKGROUND_HELPID		4009
#define LINESELECT_HELPID		4015
#define PATTERNCOLORSELECT_HELPID	4016
#define FONTSELECT_HELPID		4017


//////////////// Library-defined tools

// Because of the RC compiler's stupidity, we have to list the
// IDs explicitly.  We also define a useful ID() macro.

#ifndef ID
#ifndef RCINVOKED
#define ID(x)       MAKEINTRESOURCE(x)
#else
#define ID(x)       x
#endif
#endif

////////////////////////////////////////////////////////////////
//
// CLASS:   LineButton
//
// DESCR:   Declares class for the line attributes button
//
////////////////////////////////////////////////////////////////
#ifndef RCINVOKED

class LineButton : public PaletteItem
  {
  private:
    LineAttrib            m_LineAttributes;

    void                  DrawItem(HDC, BOOL);  // virtual method

                          LineButton(void);

  public:
                          LineButton(PaletteWindow*, UINT);
                         ~LineButton(void);

    void                  Edit(short);
    LineAttrib*           LineAttributes(void)      {return &m_LineAttributes;}
  };


////////////////////////////////////////////////////////////////
//
// CLASS:   FillButton
//
// DESCR:   Declares class for the fill attributes button
//
////////////////////////////////////////////////////////////////

class FillButton : public PaletteItem
  {
  private:
    FillAttrib            m_FillAttributes;

    void                  DrawItem(HDC, BOOL);  // virtual method

                          FillButton(void);

  public:
                          FillButton(PaletteWindow*, UINT);
                         ~FillButton(void);

    void                  Edit(void);
    FillAttrib*           FillAttributes(void)      {return &m_FillAttributes;}
  };


////////////////////////////////////////////////////////////////
//
// CLASS:   FontButton
//
// DESCR:   Declares class for the font attributes button
//
////////////////////////////////////////////////////////////////

class FontButton : public PaletteItem
  {
  private:
    FontAttrib            m_FontAttributes;

    void                  DrawItem(HDC, BOOL);  // virtual method
    void                  DrawFont(HDC);

                          FontButton(void);

  public:
                          FontButton(PaletteWindow*, UINT);
                         ~FontButton(void);

    void                  Edit(void);
    FontAttrib*           FontAttributes(void)      {return &m_FontAttributes;}
  };


////////////////////////////////////////////////////////////////
//
// CLASS:   ShapePalette
//
// DESCR:   Declares class for displaying shape palette
//
////////////////////////////////////////////////////////////////

class ShapePalette : public PaletteWindow
  {
  private:
    short                 m_nCurrentTool;
    LineButton*           m_pLineButton;
    FillButton*           m_pFillButton;
    FontButton*           m_pFontButton;

    void                  Closing(void);
    void                  ItemChosen(PaletteItem*);

    void                  CreateBitmap();
    void                  SetActiveTool(ATYPEID, short);
    void                  EnablePalettes(BOOL);

    void                  AddStdButton(UINT, char*, DWORD);
    void                  AddButton(UINT, char*, DWORD);
    void                  AddAttrButton(UINT, char*, DWORD);

  public:

    static OpWndItemD* ALMAPI   ShapeCreate(AOBJECTID, AOBJECTID, RECT*, long, AMEMBLOCKID);
    static OpWndItemD*          CreateVIFromPalette(AOBJECTID, AOBJECTID, RECT*, long);
    static OpWndItemD*          CreateVIFromMemBlock(AOBJECTID, AOBJECTID, RECT*, long, AMEMBLOCKID);
    static short                GetCurrentUnits(void);
    static ShapePalette*        s_pCurrentPalette;

                          ShapePalette(HWND);
                         ~ShapePalette(void);

    void                  InitPalette(void);

    void                  ChangeToolState(ATYPEID, short, LONG);
    void                  ShapeCreated(ShpItem*);

    short                 CurrentTool(void)     {return m_nCurrentTool;}
    short                 GetShapeID(void);

    LineAttrib*           LineAttributes(void);
    FillAttrib*           FillAttributes(void);
    FontAttrib*           FontAttributes(void);
  };
#endif

////////////////////////////////////////////////////////////////
//
// Tool defines for the resources
//
////////////////////////////////////////////////////////////////

#if (OSHP_STDTOOL_USERTOOL != 100)
#error "OSHP_STDTOOL_USERTOOL does NOT have the value 100!!"
#endif

#define SHP_TOOL_BASE                   100

#define SHP_TOOL_BITMAP                 100
#define SHP_TOOL_FILLCIRCLE             101
#define SHP_TOOL_FILLRRECT              102
#define SHP_TOOL_FILLSQUARE             103
#define SHP_TOOL_GROUPBOX               104
#define SHP_TOOL_LINE                   105
#define SHP_TOOL_TEXT                   106
#define SHP_TOOL_UNFILLCIRCLE           107
#define SHP_TOOL_UNFILLRRECT            108
#define SHP_TOOL_UNFILLSQUARE           109

#define SHP_TOOL_LASTSTDTOOL            SHP_TOOL_UNFILLSQUARE

#define SHP_TOOL_LINEATTRIBS            110
#define SHP_TOOL_FILLATTRIBS            111
#define SHP_TOOL_FONTATTRIBS            112

#define SHP_TOOL_LASTTOOL               SHP_TOOL_FONTATTRIBS

#define SHP_STDTOOLCOUNT                (SHP_TOOL_LASTSTDTOOL - SHP_TOOL_BASE + 1)
#define SHP_TOOLCOUNT                   (SHP_TOOL_LASTTOOL - SHP_TOOL_BASE + 1)

#endif // !SHAPEPAL_H
