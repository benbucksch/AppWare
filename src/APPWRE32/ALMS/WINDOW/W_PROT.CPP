#define OWND_DEFAULTIMPLEMENTATION

#include "w_includ.h"

#define I_DEFAULT_HEIGHT   100
#define I_DEFAULT_WIDTH    300
#define I_MIN_WIDTH        50
#define I_MIN_HEIGHT       50

#define I_DOT_SIZE         16 //1.6mm
#define I_DOT_SIZEH        8


////////////////////////////////////////////////////
// The init functions
////////////////////////////////////////////////////

void localInit(
  OpWndItemD*             wi,
  AOBJECTID                oiItem,
  AOBJECTID                oiParent,
  RECT*                   rcItem,
  long                    status)
  {
  if(rcItem)
    CopyRect(&wi->m_rcItem, rcItem);
  else
    SetRectEmpty(&wi->m_rcItem);

  wi->m_oiObject = oiItem;
  wi->m_oiParent = oiParent;
  wi->m_status = status;

  wi->m_grow = 0;//will be set by the window after protocolCreate
  wi->m_uidItem = 0;//will be set by the window after protocolCreate
  wi->m_accelVKey = wi->m_accelModifier = 0;

  wi->m_hwndParent = 0; //The parent window object isn't yet created
  }

void ALMAPI OpLwiInit(
  OpLiteWndItem*          lwi,
  AOBJECTID                oiItem,
  AOBJECTID                oiParent,
  RECT*                   rcItem,
  long                    status)
  {
  localInit(lwi, oiItem, oiParent, rcItem, status);
  WINDOWAPI::SetLiteItem(lwi);
  }

void ALMAPI OpHwiInit(
  OpHwndWndItem*          hwi,
  AOBJECTID                oiItem,
  AOBJECTID                oiParent,
  RECT*                   rcItem,
  long                    status)
  {
  localInit(hwi, oiItem, oiParent, rcItem, status);
	hwi->m_hwndItem = 0;
  WINDOWAPI::SetHwndItem(hwi);
  }

///////////////////////////////////////////////////////////////
///   The Destroy Functions
///////////////////////////////////////////////////////////////

void localDestroy(
  OpWndItemD*             wi)
  {
  char                    tmpStr[100];
  long                    type = (long)wi->Type();

  wsprintf(tmpStr, "The object type %c%c%c%c has not provided a Destroy method",
    HIBYTE(HIWORD(type)), LOBYTE(HIWORD(type)),
    HIBYTE(LOWORD(type)), LOBYTE(LOWORD(type)) );

  MessageBox(NULL, tmpStr, "ERROR", MB_ICONEXCLAMATION|MB_OK|MB_TASKMODAL);
  }

void ALMAPI OpLwiDestroy(
  OpLiteWndItem*              lwi)
  {
  lwi->OpLiteWndItem::Destroy();
  }

void ALMAPI OpHwiDestroy(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Destroy();
  }

void ALMAPI OpLiteWndItem::Destroy()
  {
  localDestroy(this);
  }

void ALMAPI OpHwndWndItem::Destroy()
  {
  localDestroy(this);
  }

////////////////////////////////////////////////////
//  The QueryProtocol Functions
///////////////////////////////////////////////////

long localQueryProtocol(
  OpWndItemD*             wi,
  ApQueryInfo*            qpi,
  APROTOCOLID              pid)
  {
  return AObjQueryProtocol(wi->Object(), qpi, pid);
  } 

long ALMAPI OpLwiQueryProtocol(
  OpLiteWndItem*          lwi,
  ApQueryInfo*            qpi,
  APROTOCOLID              pid)
  {
  return lwi->OpLiteWndItem::QueryProtocol(qpi, pid);
  }

long ALMAPI OpHwiQueryProtocol(
  OpHwndWndItem*          hwi,
  ApQueryInfo*            qpi,
  APROTOCOLID              pid)
  {
  return hwi->OpHwndWndItem::QueryProtocol(qpi, pid);
  }

long ALMAPI OpLiteWndItem::QueryProtocol(
  ApQueryInfo*            qpi,
  APROTOCOLID              pid)
  {
  return localQueryProtocol(this, qpi, pid);
  }

long ALMAPI OpHwndWndItem::QueryProtocol(
  ApQueryInfo*            qpi,
  APROTOCOLID              pid)
  {
  return localQueryProtocol(this, qpi, pid);
  }


////////////////////////////////////////////////////
//  The Object Functions
///////////////////////////////////////////////////

AOBJECTID localObject(
  OpWndItemD*             wi)
  {
  return wi->m_oiObject;
  } 

AOBJECTID ALMAPI OpLwiObject(
  OpLiteWndItem*          lwi)
  {
  return lwi->OpLiteWndItem::Object();
  }

AOBJECTID ALMAPI OpHwiObject(
  OpHwndWndItem*          hwi)
  {
  return hwi->OpHwndWndItem::Object();
  }

AOBJECTID ALMAPI OpLiteWndItem::Object()
  {
  return localObject(this);
  }

AOBJECTID ALMAPI OpHwndWndItem::Object()
  {
  return localObject(this);
  }

////////////////////////////////////////////////////
//  The Type Functions
///////////////////////////////////////////////////

ATYPEID localType(
  OpWndItemD*             wi)
  {
  return AObjGetType(wi->Object() );
  } 

ATYPEID ALMAPI OpLwiType(
  OpLiteWndItem*          lwi)
  {
  return lwi->OpLiteWndItem::Type();
  }

ATYPEID ALMAPI OpHwiType(
  OpHwndWndItem*          hwi)
  {
  return hwi->OpHwndWndItem::Type();
  }

ATYPEID ALMAPI OpLiteWndItem::Type()
  {
  return localType(this);
  }

ATYPEID ALMAPI OpHwndWndItem::Type()
  {
  return localType(this);
  }



//////////////////////////////////////////////////////////////
// The Edit Functions
//////////////////////////////////////////////////////////////
long localEdit(
  OpWndItemD*             wi,
  BOOL                    bEditExtra)
  {
  I_UNUSED(bEditExtra);

  //I_METHOD_NOT_PROVIDED(Type(), "Edit");
  return AObjEdit( wi->Object() );
  }

long ALMAPI OpLwiEdit(
  OpLiteWndItem*          lwi,
  BOOL                    bEditExtra)
  {
  return lwi->OpLiteWndItem::Edit(bEditExtra);
  }

long ALMAPI OpHwiEdit(
  OpHwndWndItem*          hwi,
  BOOL                    bEditExtra)
  {
  return hwi->OpHwndWndItem::Edit(bEditExtra);
  }

long ALMAPI OpLiteWndItem::Edit(
  BOOL                    bEditExtra)
  {
  return localEdit(this, bEditExtra);
  }

long ALMAPI OpHwndWndItem::Edit(
  BOOL                    bEditExtra)
  {
  return localEdit(this, bEditExtra);
  }


/////////////////////////////////////////////////////////////
// The Cut Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiCut(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Cut();
  }

void ALMAPI OpHwiCut(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Cut();
  }

void ALMAPI OpLiteWndItem::Cut()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Cut");
  }

void ALMAPI OpHwndWndItem::Cut()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Cut");
  }


/////////////////////////////////////////////////////////////
// The Copy Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiCopy(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Copy();
  }

void ALMAPI OpHwiCopy(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Copy();
  }

void ALMAPI OpLiteWndItem::Copy()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Copy");
  }

void ALMAPI OpHwndWndItem::Copy()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Copy");
  }


/////////////////////////////////////////////////////////////
// The Paste Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiPaste(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Paste();
  }

void ALMAPI OpHwiPaste(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Paste();
  }

void ALMAPI OpLiteWndItem::Paste()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Paste");
  }

void ALMAPI OpHwndWndItem::Paste()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Paste");
  }


/////////////////////////////////////////////////////////////
// The Undo Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiUndo(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Undo();
  }

void ALMAPI OpHwiUndo(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Undo();
  }

void ALMAPI OpLiteWndItem::Undo()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Undo");
  }

void ALMAPI OpHwndWndItem::Undo()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Undo");
  }


//////////////////////////////////////////////////////////////
// The DataChanged Functions
//////////////////////////////////////////////////////////////

void ALMAPI OpLwiDataChanged(
  OpLiteWndItem*          lwi,
  long                    details,
  long                    modifiers,
  long                    variation)
  {
  lwi->OpLiteWndItem::DataChanged(details, modifiers, variation);
  }

void ALMAPI OpHwiDataChanged(
  OpHwndWndItem*          hwi,
  long                    details,
  long                    modifiers,
  long                    variation)
  {
  hwi->OpHwndWndItem::DataChanged(details, modifiers, variation);
  }

void ALMAPI OpLiteWndItem::DataChanged(
  long                    details,
  long                    modifiers,
  long                    variation)
  {
  I_UNUSED(details);
  I_UNUSED(modifiers);
  I_UNUSED(variation);

  }

void ALMAPI OpHwndWndItem::DataChanged(
  long                    details,
  long                    modifiers,
  long                    variation)
  {
  I_UNUSED(details);
  I_UNUSED(modifiers);
  I_UNUSED(variation);

  }


/////////////////////////////////////////////////////////////
// The ParentOpened Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiParentOpened(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::ParentOpened();
  }

void ALMAPI OpHwiParentOpened(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::ParentOpened();
  }

void ALMAPI OpLiteWndItem::ParentOpened()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ParentOpened");
  }

void ALMAPI OpHwndWndItem::ParentOpened()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ParentOpened");
  }



/////////////////////////////////////////////////////////////
// The ParentClosing Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiParentClosing(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::ParentClosing();
  }

void ALMAPI OpHwiParentClosing(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::ParentClosing();
  }

void ALMAPI OpLiteWndItem::ParentClosing()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ParentClosing");
  }

void ALMAPI OpHwndWndItem::ParentClosing()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ParentClosing");
  }



/////////////////////////////////////////////////////////////
// The Print Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiPrint(
  OpLiteWndItem*          lwi,
  HDC                     hdc,
  RECT*                   rect, 
  int 										pclPrinter)
  {
  lwi->OpLiteWndItem::Print(hdc, rect, pclPrinter);
  }

void ALMAPI OpHwiPrint(
  OpHwndWndItem*          hwi,
  HDC                     hdc,
  RECT*                   rect, 
  int 										pclPrinter)
  {
  hwi->OpHwndWndItem::Print(hdc, rect, pclPrinter);
  }

void ALMAPI OpLiteWndItem::Print(
  HDC                     hdc,
  RECT*                   rect, 
  int 										pclPrinter)
  {
  I_UNUSED(hdc);
  I_UNUSED(rect);
  I_UNUSED(pclPrinter);

  //I_METHOD_NOT_PROVIDED(Type(), "Print");
  }

void ALMAPI OpHwndWndItem::Print(
  HDC                     hdc,
  RECT*                   rect, 
  int 										pclPrinter)
  {
  I_UNUSED(hdc);
  I_UNUSED(rect);
  I_UNUSED(pclPrinter);

  //I_METHOD_NOT_PROVIDED(Type(), "Print");
  }


/////////////////////////////////////////////////////////////
// The AccelKeyHit Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiAccelKeyHit(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::AccelKeyHit();
  }

void ALMAPI OpHwiAccelKeyHit(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::AccelKeyHit();
  }

void ALMAPI OpLiteWndItem::AccelKeyHit()
  {
  }

void ALMAPI OpHwndWndItem::AccelKeyHit()
  {
  }



/////////////////////////////////////////////////////////////
// The GetSelection Functions
/////////////////////////////////////////////////////////////

BOOL localGetSelection(
  OpWndItemD*             wi,
  int*                   	start,
  int*                   	count)
  {
  I_UNUSED(wi);

  *start = *count = 0;                                                

  //I_METHOD_NOT_PROVIDED(wi->Type(), "GetSelection");
  return FALSE;
  }

BOOL ALMAPI OpLwiGetSelection(
  OpLiteWndItem*          lwi,
  int*                   	start,
  int*                   	count)
  {
  return lwi->OpLiteWndItem::GetSelection(start, count);
  }

BOOL ALMAPI OpHwiGetSelection(
  OpHwndWndItem*          hwi,
  int*                   	start,
  int*                   	count)
  {
  return hwi->OpHwndWndItem::GetSelection(start, count);
  }

BOOL ALMAPI OpLiteWndItem::GetSelection(
  int*                   	start,
  int*                   	count)
  {
  return localGetSelection(this, start, count);
  }

BOOL ALMAPI OpHwndWndItem::GetSelection(
  int*                   	start,
  int*                   	count)
  {
  return localGetSelection(this, start, count);
  }


/////////////////////////////////////////////////////////////
// The SetSelection Functions
/////////////////////////////////////////////////////////////

BOOL localSetSelection(
  OpWndItemD*             wi,
  int                    start,
  int                    count)
  {
  I_UNUSED(wi);
  I_UNUSED(start);
  I_UNUSED(count);

  //I_METHOD_NOT_PROVIDED(wi->Type(), "SetSelection");
  return FALSE;
  }

BOOL ALMAPI OpLwiSetSelection(
  OpLiteWndItem*          lwi,
  int                    	start,
  int                    	count)
  {
  return lwi->OpLiteWndItem::SetSelection(start, count);
  }

BOOL ALMAPI OpHwiSetSelection(
  OpHwndWndItem*          hwi,
  int                    	start,
  int                    	count)
  {
  return hwi->OpHwndWndItem::SetSelection(start, count);
  }

BOOL ALMAPI OpLiteWndItem::SetSelection(
  int                    	start,
  int                    	count)
  {
  return localSetSelection(this, start, count);
  }

BOOL ALMAPI OpHwndWndItem::SetSelection(
  int                    	start,
  int                    	count)
  {
  return localSetSelection(this, start, count);
  }


/////////////////////////////////////////////////////////////
// The SetDefaultItem Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiSetDefaultItem(
  OpLiteWndItem*          lwi,
  BOOL                    set)
  {
  lwi->OpLiteWndItem::SetDefaultItem(set);
  }

void ALMAPI OpHwiSetDefaultItem(
  OpHwndWndItem*          hwi,
  BOOL                    set)
  {
  hwi->OpHwndWndItem::SetDefaultItem(set);
  }

void ALMAPI OpLiteWndItem::SetDefaultItem(
  BOOL                    set)
  {
  I_UNUSED(set);
  }

void ALMAPI OpHwndWndItem::SetDefaultItem(
  BOOL                    set)
  {
  I_UNUSED(set);
  }


/////////////////////////////////////////////////////////////
// The MouseEntry Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiMouseEntry(
  OpLiteWndItem*          lwi,
  BOOL                    inside)
  {
  lwi->OpLiteWndItem::MouseEntry(inside);
  }

void ALMAPI OpHwiMouseEntry(
  OpHwndWndItem*          hwi,
  BOOL                    inside)
  {
  hwi->OpHwndWndItem::MouseEntry(inside);
  }

void ALMAPI OpLiteWndItem::MouseEntry(
  BOOL                    inside)
  {
  I_UNUSED(inside);
  //I_METHOD_NOT_PROVIDED(Type(), "MouseEntry");
  }

void ALMAPI OpHwndWndItem::MouseEntry(
  BOOL                    inside)
  {
  I_UNUSED(inside);
  //I_METHOD_NOT_PROVIDED(Type(), "MouseEntry");
  }


/////////////////////////////////////////////////////////////
// The Activate Functions
/////////////////////////////////////////////////////////////

long ALMAPI OpLwiActivate(
  OpLiteWndItem*          lwi,
  int                     causedBy,
  BOOL                    mustKeep)
  {
  return lwi->OpLiteWndItem::Activate(causedBy, mustKeep);
  }

long ALMAPI OpHwiActivate(
  OpHwndWndItem*          hwi,
  int                     causedBy,
  BOOL                    mustKeep)
  {
  return hwi->OpHwndWndItem::Activate(causedBy, mustKeep);
  }

long ALMAPI OpLiteWndItem::Activate(
  int                     causedBy,
  BOOL                    mustKeep)
  {
  I_UNUSED(causedBy);
  I_UNUSED(mustKeep);

  InvalidateItem(TRUE);

  //I_METHOD_NOT_PROVIDED(Type(), "Activate");
  return OWND_ACTIVATION_KEPT;
  }

long ALMAPI OpHwndWndItem::Activate(
  int                     causedBy,
  BOOL                    mustKeep)
  {
  I_UNUSED(causedBy);
  I_UNUSED(mustKeep);

	I_ASSERT(IsWindow(m_hwndItem));

  SetFocus(m_hwndItem);

  //I_METHOD_NOT_PROVIDED(Type(), "Activate");
  return OWND_ACTIVATION_KEPT;
  }



/////////////////////////////////////////////////////////////
// The Deactivated Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiDeactivated(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Deactivated();
  }

void ALMAPI OpHwiDeactivated(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Deactivated();
  }

void ALMAPI OpLiteWndItem::Deactivated()
  {
  InvalidateItem(OWndIsItemTransparent(this));

  //I_METHOD_NOT_PROVIDED(Type(), "Deactivated");
  }

void ALMAPI OpHwndWndItem::Deactivated()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "Deactivated");
  }


/////////////////////////////////////////////////////////////
// The Show Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiShow(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Show();
  }

void ALMAPI OpHwiShow(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Show();
  }

void ALMAPI OpLiteWndItem::Show()
  {
  InvalidateItem(TRUE);

  //I_METHOD_NOT_PROVIDED(Type(), "Show");
  }

void ALMAPI OpHwndWndItem::Show()
  {
	if (IsWindow(m_hwndItem))
	  ShowWindow(m_hwndItem, SW_SHOWNOACTIVATE);

  //I_METHOD_NOT_PROVIDED(Type(), "Show");
  }


/////////////////////////////////////////////////////////////
// The Hide Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiHide(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::Hide();
  }

void ALMAPI OpHwiHide(
  OpHwndWndItem*          hwi)
  {
  hwi->OpHwndWndItem::Hide();
  }

void ALMAPI OpLiteWndItem::Hide()
  {
  InvalidateItem(TRUE);

  //I_METHOD_NOT_PROVIDED(Type(), " Hide");
  }

void ALMAPI OpHwndWndItem::Hide()
  {
	if (IsWindow(m_hwndItem))
	  ShowWindow(m_hwndItem, SW_HIDE);

  //I_METHOD_NOT_PROVIDED(Type(), " Hide");
  }

/////////////////////////////////////////////////////////////
// The Move Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiMove(
  OpLiteWndItem*          lwi,
  RECT*                   newRect)
  {
  lwi->OpLiteWndItem::Move(newRect);
  }

void ALMAPI OpHwiMove(
  OpHwndWndItem*          hwi,
  RECT*                   newRect)
  {
  hwi->OpHwndWndItem::Move(newRect);
  }

void ALMAPI OpLiteWndItem::Move(
  RECT*                   newRect)
  {
  if (!EqualRect(&m_rcItem, newRect))
    {
    InvalidateItem(TRUE);         //Invalidate the old
    CopyRect(&m_rcItem, newRect); //Copy the new
    InvalidateItem(TRUE);         //Invalidate the new
    }

  //I_METHOD_NOT_PROVIDED(Type(), " Move");
  }

void ALMAPI OpHwndWndItem::Move(
  RECT*                   newRect)
  {

  if (!EqualRect(&m_rcItem, newRect))
    {
    OWndMoveWindow(this, newRect, TRUE);
    InvalidateItem(TRUE);
    //OWndInvalidateLogicalRect(m_oiParent, newRect, TRUE); //Invalidate the new
    }

  //I_METHOD_NOT_PROVIDED(Type(), " Move");
  }




/////////////////////////////////////////////////////////////
// The Enable Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiEnable(
  OpLiteWndItem*          lwi,
  BOOL                    bEnable)
  {
  lwi->OpLiteWndItem::Enable(bEnable);
  }

void ALMAPI OpHwiEnable(
  OpHwndWndItem*          hwi,
  BOOL                    bEnable)
  {
  hwi->OpHwndWndItem::Enable(bEnable);
  }

void ALMAPI OpLiteWndItem::Enable(
  BOOL                    bEnable)
  {
	I_UNUSED(bEnable);

  InvalidateItem(TRUE);
  }

void ALMAPI OpHwndWndItem::Enable(
  BOOL                    bEnable)
  {
	if (IsWindow(m_hwndItem))
	  EnableWindow(m_hwndItem, bEnable);
  }


/////////////////////////////////////////////////////////////
// The ScreenLocationChanged Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiScreenLocationChanged(
  OpLiteWndItem*          lwi,
  int                     causedBy)
  {
  lwi->OpLiteWndItem::ScreenLocationChanged(causedBy);
  }

void ALMAPI OpHwiScreenLocationChanged(
  OpHwndWndItem*          hwi,
  int                     causedBy)
  {
  hwi->OpHwndWndItem::ScreenLocationChanged(causedBy);
  }

void ALMAPI OpLiteWndItem::ScreenLocationChanged(
  int                     causedBy)
  {
  I_UNUSED(causedBy);
  }

void ALMAPI OpHwndWndItem::ScreenLocationChanged(
  int                     causedBy)
  {
  I_UNUSED(causedBy);
  }


/////////////////////////////////////////////////////////////
// The AdjustNewRect Functions
/////////////////////////////////////////////////////////////
long localAdjustNewRect(
  OpWndItemD*             wi,
  RECT*                   newRect,
  long                    adjustFlag)
  {
  long                    rVal = 0L;
  int                     tmp;

  I_UNUSED(wi);

  if (adjustFlag & OWND_ADJRECT_DEFAULT_H )
    rVal |= OWND_ADJRECT_NODEFAULT_H;

  if (newRect->bottom < newRect->top)
    {
    tmp = newRect->bottom;
    newRect->bottom = newRect->top;
    newRect->top = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_TB;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
      adjustFlag |= OWND_ADJRECT_ANCHOR_T;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
      adjustFlag |= OWND_ADJRECT_ANCHOR_B;
      }
    }

  if (adjustFlag & OWND_ADJRECT_DEFAULT_W)
    rVal |= OWND_ADJRECT_NODEFAULT_W;

  if (newRect->right < newRect->left)
    {
    tmp = newRect->right;
    newRect->right = newRect->left;
    newRect->left = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_LR;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
      adjustFlag |= OWND_ADJRECT_ANCHOR_L;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
      adjustFlag |= OWND_ADJRECT_ANCHOR_R;
      }
    }

  //At this point, the rectangle is not flipped inside-out

  // Make sure it is a minimum height and width
  if (newRect->bottom - newRect->top < I_MIN_HEIGHT)
    {
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      newRect->top = newRect->bottom - I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      newRect->bottom = newRect->top + I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
    }

  if (newRect->right - newRect->left < I_MIN_WIDTH)
    { 
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      newRect->left = newRect->right - I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      newRect->right = newRect->left + I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
    }

  //I_METHOD_NOT_PROVIDED(wi->Type(), "AdjustNewRect");
  return rVal;
  }

long ALMAPI OpLwiAdjustNewRect(
  OpLiteWndItem*          lwi,
  RECT*                   newRect,
  long                    adjustFlag)
  {
  return lwi->OpLiteWndItem::AdjustNewRect(newRect, adjustFlag);
  }

long ALMAPI OpHwiAdjustNewRect(
  OpHwndWndItem*          hwi,
  RECT*                   newRect,
  long                    adjustFlag)
  {
  return hwi->OpHwndWndItem::AdjustNewRect(newRect, adjustFlag);
  }

long ALMAPI OpLiteWndItem::AdjustNewRect(
  RECT*                   newRect,
  long                    adjustFlag)
  {
  return localAdjustNewRect(this, newRect, adjustFlag);
  }

long ALMAPI OpHwndWndItem::AdjustNewRect(
  RECT*                   newRect,
  long                    adjustFlag)
  {
  return localAdjustNewRect(this, newRect, adjustFlag);
  }


/////////////////////////////////////////////////////////////
// The InvalidateItem Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiInvalidateItem(
  OpLiteWndItem*          lwi,
  BOOL                    bErase)
  {
  lwi->OpLiteWndItem::InvalidateItem(bErase);
  }

void ALMAPI OpHwiInvalidateItem(
  OpHwndWndItem*          hwi,
  BOOL                    bErase)
  {
  hwi->OpHwndWndItem::InvalidateItem(bErase);
  }

void ALMAPI OpLiteWndItem::InvalidateItem(
  BOOL                    bErase)
  {
  OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, bErase);
  return;
  }

void ALMAPI OpHwndWndItem::InvalidateItem(
  BOOL                    bErase)
  {
  OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, bErase);
  return;
  }

/////////////////////////////////////////////////////////////
// The PointInItem Functions
/////////////////////////////////////////////////////////////

BOOL localPointInItem(
  OpWndItemD*             wi,
  POINT*                  pt)
  {
  //I_METHOD_NOT_PROVIDED(wi->Type(), "PointInItem");
  return PtInRect(&wi->m_rcItem, *pt);
  }

BOOL ALMAPI OpLwiPointInItem(
  OpLiteWndItem*          lwi,
  POINT*                  pt)
  {
  return lwi->OpLiteWndItem::PointInItem(pt);
  }

BOOL ALMAPI OpHwiPointInItem(
  OpHwndWndItem*          hwi,
  POINT*                  pt)
  {
  return hwi->OpHwndWndItem::PointInItem(pt);
  }

BOOL ALMAPI OpLiteWndItem::PointInItem(
  POINT*                  pt)
  {
  return localPointInItem(this, pt);
  }

BOOL ALMAPI OpHwndWndItem::PointInItem(
  POINT*                  pt)
  {
  return localPointInItem(this, pt);
  }


/////////////////////////////////////////////////////////////
// The ItemInRect Functions
/////////////////////////////////////////////////////////////

BOOL localItemInRect(
  OpWndItemD*             wi,
  RECT*                   rect,
  BOOL                    fReqFullEnclosure)
//  RECT*                   intersectRect)
  {
  BOOL                    fRet;

  //I_METHOD_NOT_PROVIDED(wi->Type(), "ItemInRect");

  if(fReqFullEnclosure)
    {
    if((rect->left   <= wi->m_rcItem.left)
          &&
       (rect->top    <= wi->m_rcItem.top)
          &&
       (rect->right  >= wi->m_rcItem.right)
          &&
       (rect->bottom >= wi->m_rcItem.bottom))
      {
      fRet = TRUE;
      }
    else
      fRet = FALSE;
    }
  else
    {
    RECT                    tmpRect;
    RECT                    intersectRect;

    CopyRect(&tmpRect, rect);
    if (tmpRect.left == tmpRect.right) tmpRect.right++;
    if (tmpRect.top == tmpRect.bottom) tmpRect.bottom++;

    fRet = IntersectRect(&intersectRect, &tmpRect, &wi->m_rcItem);
    }

  return fRet;
  }

BOOL ALMAPI OpLwiItemInRect(
  OpLiteWndItem*          lwi,
  RECT*                   rect,
  BOOL                    fReqFullEnclosure)
//  RECT*                   intersectRect)
  {
  return lwi->OpLiteWndItem::ItemInRect(rect, fReqFullEnclosure);
  }

BOOL ALMAPI OpHwiItemInRect(
  OpHwndWndItem*          hwi,
  RECT*                   rect,
  BOOL                    fReqFullEnclosure)
//  RECT*                   intersectRect)
  {
  return hwi->OpHwndWndItem::ItemInRect(rect, fReqFullEnclosure);
  }

BOOL ALMAPI OpLiteWndItem::ItemInRect(
  RECT*                   rect,
  BOOL                    fReqFullEnclosure)
//  RECT*                   intersectRect)
  {
  return localItemInRect(this, rect, fReqFullEnclosure);
  }

BOOL ALMAPI OpHwndWndItem::ItemInRect(
  RECT*                   rect,
  BOOL                    fReqFullEnclosure)
//  RECT*                   intersectRect)
  {
  return localItemInRect(this, rect, fReqFullEnclosure);
  }


/////////////////////////////////////////////////////////////
// The ItemInRegion Functions
/////////////////////////////////////////////////////////////

BOOL localItemInRegion(
  OpWndItemD*             wi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  RECT                    tmpRect;

  //I_METHOD_NOT_PROVIDED(wi->Type(), "ItemInRegion");

  CopyRect(&tmpRect, &wi->m_rcItem);

  if (bDeviceUnits)
    OWndLPtoDP(wi->m_oiParent, (LPPOINT)&tmpRect, 2);

  return RectInRegion(hRegion, &tmpRect);
  }

BOOL ALMAPI OpLwiItemInRegion(
  OpLiteWndItem*          lwi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  return lwi->OpLiteWndItem::ItemInRegion(hRegion, bDeviceUnits);
  }

BOOL ALMAPI OpHwiItemInRegion(
  OpHwndWndItem*          hwi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  return hwi->OpHwndWndItem::ItemInRegion(hRegion, bDeviceUnits);
  }

BOOL ALMAPI OpLiteWndItem::ItemInRegion(
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  return localItemInRegion(this, hRegion, bDeviceUnits);
  }

BOOL ALMAPI OpHwndWndItem::ItemInRegion(
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  return localItemInRegion(this, hRegion, bDeviceUnits);
  }

/////////////////////////////////////////////////////////////
// The RemoveItemFromRegion Functions
/////////////////////////////////////////////////////////////

void localRemoveItemFromRegion(
  OpWndItemD*             wi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  HRGN                    itemRegion, tmpRegion;
  RECT                    tmpRect;


  //I_METHOD_NOT_PROVIDED(wi->Type(), "RemoveItemFromRegion");

  CopyRect(&tmpRect, &wi->m_rcItem);
  if (bDeviceUnits)
    OWndLPtoDP(wi->m_oiParent, (LPPOINT)&tmpRect, 2);

  tmpRegion = CreateRectRgn(0,0,0,0);
  itemRegion = CreateRectRgn(tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom);
    
  if(CombineRgn(tmpRegion, hRegion, itemRegion, RGN_DIFF)!=ERROR)
    CombineRgn(hRegion, tmpRegion, 0, RGN_COPY);

  DeleteObject(tmpRegion);
  DeleteObject(itemRegion);
  }

void ALMAPI OpLwiRemoveItemFromRegion(
  OpLiteWndItem*          lwi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  lwi->OpLiteWndItem::RemoveItemFromRegion(hRegion, bDeviceUnits);
  }

void ALMAPI OpHwiRemoveItemFromRegion(
  OpHwndWndItem*          hwi,
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  hwi->OpHwndWndItem::RemoveItemFromRegion(hRegion, bDeviceUnits);
  }

void ALMAPI OpLiteWndItem::RemoveItemFromRegion(
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  localRemoveItemFromRegion(this, hRegion, bDeviceUnits);
  }

void ALMAPI OpHwndWndItem::RemoveItemFromRegion(
  HRGN                    hRegion,
  BOOL                    bDeviceUnits)
  {
  localRemoveItemFromRegion(this, hRegion, bDeviceUnits);
  }



/////////////////////////////////////////////////////////////
// The DrawTrackingRect Functions
/////////////////////////////////////////////////////////////

void localDrawTrackingRect(
  OpWndItemD*             wi,
  HDC                     hdc,
  RECT*                   rect)
  {
  RECT                    tmpRect;
  POINT                   ptLineWidths = {1,1};

  OWndDStoLS(wi->m_oiParent, &ptLineWidths, 1);


  I_UNUSED(wi);

  tmpRect.top     = min(rect->top, rect->bottom);
  tmpRect.bottom  = max(rect->top, rect->bottom);
  tmpRect.left    = min(rect->left, rect->right);
  tmpRect.right   = max(rect->left, rect->right);

  if (tmpRect.top == tmpRect.bottom)
    {
    tmpRect.top--;
    tmpRect.bottom++;
    }
  if (tmpRect.left == tmpRect.right)
    {
    tmpRect.left--;
    tmpRect.right++;
    }

  MoveToEx(hdc, tmpRect.left, tmpRect.top, NULL);

  LineTo(hdc, tmpRect.left, tmpRect.bottom-ptLineWidths.y);
  LineTo(hdc, tmpRect.right-ptLineWidths.x, tmpRect.bottom-ptLineWidths.y);
  LineTo(hdc, tmpRect.right-ptLineWidths.x, tmpRect.top);
  LineTo(hdc, tmpRect.left, tmpRect.top);

  //I_METHOD_NOT_PROVIDED(wi->Type(), "DrawTrackingRect");
  }

void ALMAPI OpLwiDrawTrackingRect(
  OpLiteWndItem*              lwi,
  HDC                     hdc,
  RECT*                   rect)
  {
  lwi->OpLiteWndItem::DrawTrackingRect(hdc, rect);
  }

void ALMAPI OpHwiDrawTrackingRect(
  OpHwndWndItem*          hwi,
  HDC                     hdc,
  RECT*                   rect)
  {
  hwi->OpHwndWndItem::DrawTrackingRect(hdc, rect);
  }

void ALMAPI OpLiteWndItem::DrawTrackingRect(
  HDC                     hdc,
  RECT*                   rect)
  {
  localDrawTrackingRect(this, hdc, rect);
  }

void ALMAPI OpHwndWndItem::DrawTrackingRect(
  HDC                     hdc,
  RECT*                   rect)
  {
  localDrawTrackingRect(this, hdc, rect);
  }


/////////////////////////////////////////////////////////////
// The DrawGrabHandles Functions
/////////////////////////////////////////////////////////////

void localCreateGrabHandle(
  AOBJECTID                oiWindow,
  RECT*                   targetRect,
  RECT*                   sourceRect,
  int                     grabHandle) //grabHandle is the POINT_IN_?? constants
  {
  int                     i;
  POINT                   ptLineWidths = {1,1};

  OWndDStoLS(oiWindow, &ptLineWidths, 1);
  CopyRect(targetRect, sourceRect);

  //targetRect->bottom -= ptLineWidths.y;
  //targetRect->right -= ptLineWidths.x;

  switch(grabHandle)
    {
    case OWND_POINT_IN_TL:
      SetRect(targetRect, targetRect->left, targetRect->top, targetRect->left+I_DOT_SIZE, targetRect->top+I_DOT_SIZE);
      break;
    case OWND_POINT_IN_TR:
      SetRect(targetRect, targetRect->right-I_DOT_SIZE, targetRect->top, targetRect->right, targetRect->top+I_DOT_SIZE);
      break;
    case OWND_POINT_IN_BL:
      SetRect(targetRect, targetRect->left, targetRect->bottom-I_DOT_SIZE, targetRect->left+I_DOT_SIZE, targetRect->bottom);
      break;
    case OWND_POINT_IN_BR:
      SetRect(targetRect, targetRect->right-I_DOT_SIZE, targetRect->bottom-I_DOT_SIZE, targetRect->right, targetRect->bottom);
      break;
    case OWND_POINT_IN_T:
      i = (targetRect->right + targetRect->left - I_DOT_SIZE)/2; //horz start for top & bottom
      SetRect(targetRect, i, targetRect->top, i+I_DOT_SIZE, targetRect->top+I_DOT_SIZE);
      break;
    case OWND_POINT_IN_L:
      i = (targetRect->bottom + targetRect->top - I_DOT_SIZE)/2; //vert start for left & right
      SetRect(targetRect, targetRect->left, i, targetRect->left+I_DOT_SIZE, i+I_DOT_SIZE);
      break;
    case OWND_POINT_IN_B:
      i = (targetRect->right + targetRect->left - I_DOT_SIZE)/2; //horz start for top & bottom
      SetRect(targetRect, i, targetRect->bottom-I_DOT_SIZE, i+I_DOT_SIZE, targetRect->bottom);
      break;
    case OWND_POINT_IN_R:
      i = (targetRect->bottom + targetRect->top - I_DOT_SIZE)/2; //vert start for left & right
      SetRect(targetRect, targetRect->right-I_DOT_SIZE, i, targetRect->right, i + I_DOT_SIZE);
      break;
    }
  }

void localDrawGrabHandles(
  OpWndItemD*             wi,
  HDC                     hdc)
  {
  RECT                    grabHandleRect;
  int                     i;
  POINT                   ptLineWidths = {1,1};

  OWndDStoLS(wi->m_oiParent, &ptLineWidths, 1);

  for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(wi->m_oiParent, &grabHandleRect, &wi->m_rcItem, i);
    Rectangle(hdc, grabHandleRect.left, grabHandleRect.top, grabHandleRect.right, grabHandleRect.bottom);
    }

  MoveToEx(hdc, wi->m_rcItem.left, wi->m_rcItem.top, NULL);
  LineTo(hdc, wi->m_rcItem.left, wi->m_rcItem.bottom-ptLineWidths.y);
  LineTo(hdc, wi->m_rcItem.right-ptLineWidths.x, wi->m_rcItem.bottom-ptLineWidths.y);
  LineTo(hdc, wi->m_rcItem.right-ptLineWidths.x, wi->m_rcItem.top);
  LineTo(hdc, wi->m_rcItem.left, wi->m_rcItem.top);

  //I_METHOD_NOT_PROVIDED(wi->Type(), "DrawGrabHandles");
  }

void ALMAPI OpLwiDrawGrabHandles(
  OpLiteWndItem*          lwi,
  HDC                     hdc)
  {
  lwi->OpLiteWndItem::DrawGrabHandles(hdc);
  }

void ALMAPI OpHwiDrawGrabHandles(
  OpHwndWndItem*          hwi,
  HDC                     hdc)
  {
  hwi->OpHwndWndItem::DrawGrabHandles(hdc);
  }

void ALMAPI OpLiteWndItem::DrawGrabHandles(
  HDC                     hdc)
  {
  localDrawGrabHandles(this, hdc);
  return;
  }

void ALMAPI OpHwndWndItem::DrawGrabHandles(
  HDC                     hdc)
  {
  localDrawGrabHandles(this, hdc);
  return;
  }


/////////////////////////////////////////////////////////////
// The InvalidateGrabHandles Functions
/////////////////////////////////////////////////////////////

void localInvalidateGrabHandles(
  OpWndItemD*             wi,
  BOOL                    bErase)
  {
  RECT                    tmpRect;
  int                     i;
  POINT                   ptLineWidths = {1,1};

  for (i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(wi->m_oiParent, &tmpRect, &wi->m_rcItem, i);
    OWndInvalidateLogicalRect(wi->m_oiParent, &tmpRect, bErase);
    }

  OWndDStoLS(wi->m_oiParent, &ptLineWidths, 1);

  SetRect(&tmpRect, wi->m_rcItem.left, wi->m_rcItem.top, wi->m_rcItem.right, wi->m_rcItem.top+ptLineWidths.y);
  OWndInvalidateLogicalRect(wi->m_oiParent, &tmpRect, bErase); //top line

  SetRect(&tmpRect, wi->m_rcItem.left, wi->m_rcItem.top, wi->m_rcItem.left+ptLineWidths.x, wi->m_rcItem.bottom);
  OWndInvalidateLogicalRect(wi->m_oiParent, &tmpRect, bErase); //left line

  SetRect(&tmpRect, wi->m_rcItem.left, wi->m_rcItem.bottom-ptLineWidths.y, wi->m_rcItem.right, wi->m_rcItem.bottom);
  OWndInvalidateLogicalRect(wi->m_oiParent, &tmpRect, bErase); //bottom line

  SetRect(&tmpRect, wi->m_rcItem.right-ptLineWidths.x, wi->m_rcItem.top, wi->m_rcItem.right, wi->m_rcItem.bottom);
  OWndInvalidateLogicalRect(wi->m_oiParent, &tmpRect, bErase); //right line


  //I_METHOD_NOT_PROVIDED(wi->Type(), "InvalidateGrabHandles");
  }

void ALMAPI OpLwiInvalidateGrabHandles(
  OpLiteWndItem*          lwi,
  BOOL                    bErase)
  {
  lwi->OpLiteWndItem::InvalidateGrabHandles(bErase);
  }

void ALMAPI OpHwiInvalidateGrabHandles(
  OpHwndWndItem*          hwi,
  BOOL                    bErase)
  {
  hwi->OpHwndWndItem::InvalidateGrabHandles(bErase);
  }

void ALMAPI OpLiteWndItem::InvalidateGrabHandles(
  BOOL                    bErase)
  {
  localInvalidateGrabHandles(this, bErase);
  return;
  }

void ALMAPI OpHwndWndItem::InvalidateGrabHandles(
  BOOL                    bErase)
  {
  localInvalidateGrabHandles(this, bErase);
  return;
  }


/////////////////////////////////////////////////////////////
// The PointInGrabHandle Functions
/////////////////////////////////////////////////////////////
long localPointInGrabHandle(
  OpWndItemD*             wi,
  POINT*                  pt,
  HCURSOR*                phCursor)
  {
  RECT                    grabHandleRect;
  int                     i;

  I_UNUSED(phCursor);


  for(i=OWND_POINT_IN_TL; i<=OWND_POINT_IN_R; i++)
    {
    localCreateGrabHandle(wi->m_oiParent, &grabHandleRect, &wi->m_rcItem, i);
    if (PtInRect(&grabHandleRect, *pt) )
      return i;
    }

  //I_METHOD_NOT_PROVIDED(wi->Type(), "PointInGrabHandle");
  return OWND_POINT_IN_NONE;
  }

long ALMAPI OpLwiPointInGrabHandle(
  OpLiteWndItem*          lwi,
  POINT*                  pt,
  HCURSOR*                phCursor)
  {
  return lwi->OpLiteWndItem::PointInGrabHandle(pt, phCursor);
  }

long ALMAPI OpHwiPointInGrabHandle(
  OpHwndWndItem*          hwi,
  POINT*                  pt,
  HCURSOR*                phCursor)
  {
  return hwi->OpHwndWndItem::PointInGrabHandle(pt, phCursor);
  }

long ALMAPI OpLiteWndItem::PointInGrabHandle(
  POINT*                  pt,
  HCURSOR*                phCursor)
  {
  return localPointInGrabHandle(this, pt, phCursor);
  }

long ALMAPI OpHwndWndItem::PointInGrabHandle(
  POINT*                  pt,
  HCURSOR*                phCursor)
  {
  return localPointInGrabHandle(this, pt, phCursor);
  }

////////////////////////////////////////////////////////////
// The GetExtraData Functions
/////////////////////////////////////////////////////////////

AMEMBLOCKID ALMAPI OpLwiGetExtraData(
  OpLiteWndItem*          lwi)
  {
  return lwi->OpLiteWndItem::GetExtraData();
  }

AMEMBLOCKID ALMAPI OpHwiGetExtraData(
  OpHwndWndItem*          hwi)
  {
  return hwi->OpHwndWndItem::GetExtraData();
  }

AMEMBLOCKID ALMAPI OpLiteWndItem::GetExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "GetExtraData");
  return 0L;
  }

AMEMBLOCKID ALMAPI OpHwndWndItem::GetExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "GetExtraData");
  return 0L;
  }

////////////////////////////////////////////////////////////
// The ExportExtraData Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiExportExtraData(
  OpLiteWndItem*          lwi)
  {
   lwi->OpLiteWndItem::ExportExtraData();
  }

void ALMAPI OpHwiExportExtraData(
  OpHwndWndItem*          hwi)
  {
   hwi->OpHwndWndItem::ExportExtraData();
  }

void ALMAPI OpLiteWndItem::ExportExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ExportExtraData");
   0L;
  }

void ALMAPI OpHwndWndItem::ExportExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ExportExtraData");
   0L;
  }

////////////////////////////////////////////////////////////
// The ImportExtraData Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiImportExtraData(
  OpLiteWndItem*          lwi)
  {
   lwi->OpLiteWndItem::ImportExtraData();
  }

void ALMAPI OpHwiImportExtraData(
  OpHwndWndItem*          hwi)
  {
   hwi->OpHwndWndItem::ImportExtraData();
  }

void ALMAPI OpLiteWndItem::ImportExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ImportExtraData");
   0L;
  }

void ALMAPI OpHwndWndItem::ImportExtraData()
  {
  //I_METHOD_NOT_PROVIDED(Type(), "ImportExtraData");
   0L;
  }

////////////////////////////////////////////////////////////
// The QueryDesiredPalette Functions
/////////////////////////////////////////////////////////////

BOOL ALMAPI OpLwiQueryDesiredPalette(
  OpLiteWndItem*          lwi,
  AMEMBLOCKID							miPalette)
  {
  return lwi->OpLiteWndItem::QueryDesiredPalette(miPalette);
  }

BOOL ALMAPI OpHwiQueryDesiredPalette(
  OpHwndWndItem*          hwi,
  AMEMBLOCKID							miPalette)
  {
  return hwi->OpHwndWndItem::QueryDesiredPalette(miPalette);
  }

BOOL ALMAPI OpLiteWndItem::QueryDesiredPalette(
	AMEMBLOCKID							)
  {
  return FALSE;
  }

BOOL ALMAPI OpHwndWndItem::QueryDesiredPalette(
	AMEMBLOCKID							)
  {
  return FALSE;
  }

/////////////////////////////////////////////////////////////
// The ParentNotification Functions
/////////////////////////////////////////////////////////////

LRESULT ALMAPI OpLwiParentNotification(
  OpLiteWndItem*          lwi,
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  return lwi->OpLiteWndItem::ParentNotification(hwnd, message, wParam, lParam);
  }

LRESULT ALMAPI OpHwiParentNotification(
  OpHwndWndItem*          hwi,
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
  return hwi->OpHwndWndItem::ParentNotification(hwnd, message, wParam, lParam);
  }

LRESULT ALMAPI OpLiteWndItem::ParentNotification(
  HWND                    ,
  UINT                    ,
  WPARAM                  ,
  LPARAM                  )
  {
  return 0L;
  }

LRESULT ALMAPI OpHwndWndItem::ParentNotification(
  HWND                    hwnd,
  UINT                    message,
  WPARAM                  wParam,
  LPARAM                  lParam)
  {
	switch (message)
		{
		case WM_WININICHANGE:
		case WM_SYSCOLORCHANGE:
		case WM_PALETTECHANGED:
		case WM_FONTCHANGE:
			return 0L;
		default:
		  return DefWindowProc(hwnd, message, wParam, lParam);
		}
  }

/////////////////////////////////////////////////////////////
// The Draw Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiDraw(
  OpLiteWndItem*          lwi,
  HDC                     hdc,
  RECT*                   invalidRect)
  {
  lwi->OpLiteWndItem::Draw(hdc, invalidRect);
  }

void ALMAPI OpLiteWndItem::Draw(
  HDC                     hdc,
  RECT*                   invalidRect)
  {
  I_UNUSED(hdc);
  I_UNUSED(invalidRect);

  return;
  }


/////////////////////////////////////////////////////////////
// The KeyDown Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiKeyDown(
  OpLiteWndItem*          lwi,
  int                     vKey,
  char                    ascii,
  long                    keyData)
  {
  lwi->OpLiteWndItem::KeyDown(vKey, ascii, keyData);
  }

void ALMAPI OpLiteWndItem::KeyDown(
  int                     vKey,
  char                    ascii,
  long                    keyData)
  {
  I_UNUSED(vKey);
  I_UNUSED(ascii);
  I_UNUSED(keyData);

  return;
  }


/////////////////////////////////////////////////////////////
// The KeyUp Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiKeyUp(
  OpLiteWndItem*          lwi,
  int                     vKey,
  char                    ascii,
  long                    keyData)
  {
  lwi->OpLiteWndItem::KeyUp(vKey, ascii, keyData);
  }

void ALMAPI OpLiteWndItem::KeyUp(
  int                     vKey,
  char                    ascii,
  long                    keyData)
  {
  I_UNUSED(vKey);
  I_UNUSED(ascii);
  I_UNUSED(keyData);

  //I_METHOD_NOT_PROVIDED(Type(), "KeyUp");
  return;
  }


/////////////////////////////////////////////////////////////
// The MouseMove Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiMouseMove(
  OpLiteWndItem*          lwi,
  POINT*                  where,
  int                     keysDown)
  {
  lwi->OpLiteWndItem::MouseMove(where, keysDown);
  }

void ALMAPI OpLiteWndItem::MouseMove(
  POINT*                  where,
  int                     keysDown)
  {
  I_UNUSED(where);
  I_UNUSED(keysDown);

  //I_METHOD_NOT_PROVIDED(Type(), "MouseMove");
  return;
  }

/////////////////////////////////////////////////////////////
// The MouseDown Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiMouseDown(
  OpLiteWndItem*          lwi,
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  lwi->OpLiteWndItem::MouseDown(where, keysDown, button);
  }

void ALMAPI OpLiteWndItem::MouseDown(
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  //I_METHOD_NOT_PROVIDED(Type(), "MouseDown");
  return;
  }



/////////////////////////////////////////////////////////////
// The DoubleClicked Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiDoubleClicked(
  OpLiteWndItem*          lwi,
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  lwi->OpLiteWndItem::DoubleClicked(where, keysDown, button);
  }

void ALMAPI OpLiteWndItem::DoubleClicked(
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  //I_METHOD_NOT_PROVIDED(Type(), "DoubleClicked");
  return;
  }


/////////////////////////////////////////////////////////////
// The MouseUp Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiMouseUp(
  OpLiteWndItem*          lwi,
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  lwi->OpLiteWndItem::MouseUp(where, keysDown, button);
  }

void ALMAPI OpLiteWndItem::MouseUp(
  POINT*                  where,
  int                     keysDown,
  int                     button)
  {
  I_UNUSED(where);
  I_UNUSED(keysDown);
  I_UNUSED(button);

  //I_METHOD_NOT_PROVIDED(Type(), "MouseUp");
  return;
  }


/////////////////////////////////////////////////////////////
// The CancelMode Functions
/////////////////////////////////////////////////////////////

void ALMAPI OpLwiCancelMode(
  OpLiteWndItem*          lwi)
  {
  lwi->OpLiteWndItem::CancelMode();
  }

void ALMAPI OpLiteWndItem::CancelMode()
  {

  //I_METHOD_NOT_PROVIDED(Type(), "CancelMode");
  return;
  }







