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
// FILE:          "lay_shap.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_SHAP_H
#define LAY_SHAP_H

// make sure the correct things are included ///////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

#ifndef O_WNDSHP_H
#include <o_wndshp.h>
#endif

// forward class definition ////////////////////////////////////////////////

class Layout;
class Window;

// defines for Shapes //////////////////////////////////////////////////////

#define LEN_DLLNAME             13
#define LEN_SHAPELIBNAME        25

// SHAPELIB struct /////////////////////////////////////////////////////////

struct SHAPELIB
  {
  HINSTANCE   hinstShapeLib;            // DLL hinst for shape library
  OShapeAdmin lpfnShapeAdmin;           // address for shape lib's admin.
  TYPEID      type;                     // shape lib's TYPEID
  PWICreateFN lpfnCreate;               // address for shape lib's create fn
  char        szDLL[LEN_DLLNAME];       // DLL name for shape library
  char        szName[LEN_SHAPELIBNAME]; // Shape lib's name -- for menu
  long        lCaps;                    // Library capabilities
  long        hPalette;                 // handle to the shape palette
  };

// Shapes class definition /////////////////////////////////////////////////

class Shapes

  {
  private:
    UINT          m_nShapeLib;        // number of shape libraries
    MEMBLOCKID    m_miShapeLib;       // memory handle for the shape library information
    SHAPELIB*     m_pShapeLib;        // pointer the the list of shape library structures

    short         m_nTool;            // current tool
    long          m_fToolProp;        // properties of the current tool (for shapes only)
    int           m_nToolLib;         // current tool library index
    HCURSOR       m_hcursor;          // cursor for the current tool

    long          m_fTrack;           // tracking flags for shape tools
    OpWndItemD*   m_wiShape;          // new shape that is being tracked
    RECT          m_rcLast;           // last tracking rectangle
    POINT         m_ptStart;          // starting point for shape

    Layout*       m_layout;           // pointer to the layout

    int           FindShapeLibIndex(TYPEID tiShape);
    void          BroadcastToolChange();
    OpWndItemD*   CreateShape(Window* pWnd, LPPOINT lppt, BOOL fLButtonDown);

    friend LONG ALMAPI LayoutShapeAdmin(UINT uMsg, AObjMessage* pMsg);

  public:
                  Shapes(Layout* layout);
                 ~Shapes();

    inline short  CurrentTool()       {return m_nTool;}
    inline TYPEID CurrentLib()        {return (m_nToolLib>=0) ? m_pShapeLib[m_nToolLib].type : NULL;}
    inline BOOL   IsTracking()        {return (m_fTrack & (TRACK_NEWSHAPE | TRACK_SELECT)) ? TRUE : FALSE;}

    void          LoadShapeLibs();
    void          LoadShapeLib(char* szDLL);
    void          ValidateShapeLib(TYPEID tiShape);
    void          FreeShapeLibs();
    long          GetCurrentUnits();
    void          ChangeTool(TYPEID tiShape, short nTool);
    void          InitPalettes();
    void          EnablePalettes(BOOL fEnable);
    void          DestroyPalettes(BOOL fPrstPos);
    void          ShowAllPalettes(BOOL fShow);
    void          InitPaletteMenu(HMENU hmenuPopup);
    void          MenuPicked(WPARAM wParam);
    BOOL          FullInfoTool();
    void          GetShapeName(OpWndItemD* wi, LPSTR szName, int nLen);
    void          TrackShape(Window* pWnd, HWND hwnd, LPPOINT lppt, BOOL fDraw, BOOL fToggle);

    // window message processing
    //  -- the tool needs to process mouse and keyboard commands
    void          MouseMove(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseLButtonDown(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseLButtonDblClk(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseLButtonUp(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseMButtonDown(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseMButtonDblClk(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseMButtonUp(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseRButtonDown(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseRButtonDblClk(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          MouseRButtonUp(Window* pwnd, HWND hwnd, int fwKeys, short xPos, short yPos);
    void          KeyDown(Window* pwnd, HWND hwnd, int wVKey, DWORD dwKeyData);
    void          KeyUp(Window* pwnd, HWND hwnd, int wVKey, DWORD dwKeyData);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAY_SHAP_H
