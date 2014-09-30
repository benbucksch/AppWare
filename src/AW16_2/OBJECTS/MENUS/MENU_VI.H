/*//////////////////////////////////////////////////////////////
//
//					Copyright 1994, Novell, Inc.  All rights reserved
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
// FILE:					MENU_VI.H
//
// AUTHOR:				Russell Graves    28 Feb 94 - Initial entry
//
// DESCRIPTION:		Header file for the Popup menu visual instance
//                (when a menus is flagged as popup, it 
//                 becomes a lite window item.)
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#ifndef   MENU_VI_H
#define   MENU_VI_H

#include  <op_wnd.h>    // Window item protocol
#include  <o_wnd.h>     // Window object API
 
// Define the visual instance implementation class

class MenuPopupVI : public OpLiteWndItem
  {
  private:
                  ~MenuPopupVI(void);
  public:
                  MenuPopupVI(OBJECTID  oiItem,
                              OBJECTID  oiParent,
                              RECT*     rcItem,
                              long      status);
      
    void  ALMAPI  Destroy(void);
    void  ALMAPI  Draw(HDC hdc, RECT* invalidRect);
    void  ALMAPI  MouseDown(POINT* where, int keysDown, int button);
    void  ALMAPI  DataChanged(long details, long modifiers, long	variation);
    int   LUxtoDUx(int x);
    int   LUytoDUy(int y);
  private:
  
    void  PopupDraw(HDC hdc);
 
  };
  
extern "C"
OpWndItemD* ALMAPI MenuPopupCreate 
  (
  OBJECTID    oiItem,
  OBJECTID    oiParent,
  RECT*       rcItem,
  long        status,
  long        miExtra
  );
  

#endif  // !MENU_VI_H