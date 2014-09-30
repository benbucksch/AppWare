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
// FILE:          "w_upsf.cpp"
//
// AUTHOR:        Stephen Jones
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define OWND_DEFAULTIMPLEMENTATION
#include "w_includ.h"
#include <o_upsf.h>

/////////////////////////////////
// This generic window item is for storing the basic window item
// information (for UPSF import)
/////////////////////////////////
class GenericWndItem :
  public OpLiteWndItem
  
  {
  private:
  
  public:
                       GenericWndItem(AOBJECTID   oiSelf, 
                                      AOBJECTID   oiParent, 
                                      LPRECT     lprcItem, 
                                      long       lStatus, 
                                      AMEMBLOCKID miExtra);
                      ~GenericWndItem();
  
    /* overridden ApObjectD methods */
    virtual void       ALMAPI Destroy();
  };

/////////////////////////////////
OpWndItemD* ALMAPI CreateGenericItem
  (
  AOBJECTID    oiItem,
  AOBJECTID    oiParent,
  RECT*       rcItem,
  long        lStatus,
  AMEMBLOCKID  miExtraData
  )

  {
  return new GenericWndItem(oiItem, oiParent, rcItem, lStatus, miExtraData);
  }

/////////////////////////////////
GenericWndItem::GenericWndItem
  (
  AOBJECTID   oiSelf, 
  AOBJECTID   oiParent, 
  LPRECT     lprcItem, 
  long       lStatus, 
  AMEMBLOCKID miExtra
  ) : OpLiteWndItem(oiSelf, oiParent, lprcItem, lStatus)

  {
  I_UNUSED(miExtra);
  }

/////////////////////////////////
GenericWndItem::~GenericWndItem
  (
  )

  {
  }

/////////////////////////////////
void ALMAPI GenericWndItem::Destroy
  (
  )

  {
  delete this;
  }

/////////////////////////////////
void LogError(
  LPSTR                   szMember)
  {
  static char             szComment[256];

  wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
  AUpsfLogComment(szComment);
  }

/////////////////////////////////
void LogInfo(
  LPSTR                   szMember)
  {
  static char             szComment[256];

  wsprintf(szComment, "INFO -- \"%s\" field not present -- using default.", szMember);
  AUpsfLogComment(szComment);
  }

/////////////////////////////////

long WINDOW::Export()
  {
  OpWndItemD*             wi;
  int                     i;
  WNDITEM_DATA*           wid;
  WINDOWRUNTIME*          wr;
  HBRUSH                  hbr;
  HBITMAP                 hbmp;
  char                  	szTitle[I_TITLE_LENGTH+1];

  wr = new WINDOWRUNTIME(1, TRUE);  //This class needs to be created so the shape library name stuff will work

  AUpsfExportField(g_szUpsf_version, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &m_version, sizeof(int));
	GetTitle(szTitle, I_TITLE_LENGTH);
  AUpsfExportField(g_szUpsf_title, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, szTitle, (LONG)lstrlen(szTitle) );

  // The pieces of the window style --------
  ExportBool(g_szUpsf_titlebar,    0L, HasTitleBar() );
  ExportBool(g_szUpsf_sysmenu,     0L, HasSysMenu() );
  ExportBool(g_szUpsf_maxbutton,   0L, HasMaxButton() );
  ExportBool(g_szUpsf_minbutton,   0L, HasMinButton() );
  ExportBool(g_szUpsf_horzscroll,  0L, HasHorzScroll() );
  ExportBool(g_szUpsf_vertscroll,  0L, HasVertScroll() );
  ExportBool(g_szUpsf_resize,      0L, HasResizeFrame() );
  ExportBool(g_szUpsf_dialogframe, 0L, HasDialogFrame() );
  ExportBool(g_szUpsf_modal,       0L, IsModal() );
  ExportBool(g_szUpsf_ontop,       0L, IsPopup() );
  ExportBool(g_szUpsf_initvisible, 0L, IsInitVisible() );
  ExportBool(g_szUpsf_drawsmooth,  0L, DrawsSmooth() );
  ExportBool(g_szUpsf_closeonexit, 0L, QuitsOnClose() );

  ExportLogicalRect(m_oiSelf, g_szUpsf_initrect, 0L, &m_rclInitialClient);

  ExportBool(g_szUpsf_limitsize,  0L, m_bLimitMaxSize);
  ExportLogicalPoint(m_oiSelf, g_szUpsf_maxsize, 0L, &m_ptlMaxSize);
  ExportLogicalPoint(m_oiSelf, g_szUpsf_minsize, 0L, &m_ptlMinSize);
  ExportLogicalPoint(m_oiSelf, g_szUpsf_scrollsize, 0L, &m_ptlScrollSize);

  AUpsfExportField(g_szUpsf_menuid, 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &m_oiMenu, sizeof(AOBJECTID) );
  AUpsfExportField(g_szUpsf_nextsignal, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_NUMBER, &m_uidNext, sizeof(long) );

  // export the window background information
  hbmp = CreateBitmap(8, 8, 1, 1, m_awPattern);
  hbr  = CreatePatternBrush(hbmp);
  ExportBrush(g_szUpsf_bg_pattern, 0, hbr);
  DeleteObject(hbmp);
  DeleteObject(hbr);
  ExportColor(g_szUpsf_bg_fgcolor, 0, (m_clrForeground == I_WINDOW_FOREGROUND) ? GetSysColor(COLOR_WINDOWTEXT) : m_clrForeground);
  ExportColor(g_szUpsf_bg_bgcolor, 0, (m_clrBackground == I_WINDOW_BACKGROUND) ? GetSysColor(COLOR_WINDOW) : m_clrBackground);
  ExportBool(g_szUpsf_bg_fgdefault, 0, m_clrForeground == I_WINDOW_FOREGROUND);
  ExportBool(g_szUpsf_bg_bgdefault, 0, m_clrBackground == I_WINDOW_BACKGROUND);

  //Now, export the window items..
  m_iNumItems = (int)( AObjGetDataCount(m_oiSelf) - I_DATA_ITEMS_OFFSET);

  for (i=0; i<m_iNumItems; i++)
    {
    wid = LockItemData(m_oiSelf, i);
    I_VERIFY_POINTER(wid, continue);

    wi = wid->CreateVI(m_oiSelf, GetItemDataSize(m_oiSelf, i), i);
    I_VERIFY_POINTER(wi, continue);

    if(AUpsfCreateScope(g_szUpsf_item_scope, i)==AUPSF_NOERROR)
      {
      if(WINDOWAPI::IsItemShape(wi) || WINDOWAPI::IsItemGroup(wi))
        {
        AUpsfExportField(g_szUpsf_item_id, 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &m_oiSelf, sizeof(AOBJECTID) );
        ExportBool(g_szUpsf_item_shape, 0L, WINDOWAPI::IsItemShape(wi));
        ExportBool(g_szUpsf_item_group, 0L, WINDOWAPI::IsItemGroup(wi));
        ExportLong(g_szUpsf_item_typeid, 0L, (long)wi->Type());
        }
      else
        AUpsfExportField(g_szUpsf_item_id, 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &wi->m_oiObject, sizeof(AOBJECTID) );

      ExportLogicalRect(m_oiSelf, g_szUpsf_item_rect, 0L, &wi->m_rcItem);
      ExportBool(g_szUpsf_item_visible, 0L, WINDOWAPI::IsItemVisible(wi) );
      ExportBool(g_szUpsf_item_tabstop, 0L, WINDOWAPI::IsItemTabstop(wi) );
      ExportBool(g_szUpsf_item_groupstop, 0L, WINDOWAPI::IsItemGroupstop(wi) );
      ExportBool(g_szUpsf_item_locked, 0L, WINDOWAPI::IsItemLocked(wi) );
      ExportShort(g_szUpsf_item_growsflag, 0L, wi->m_grow);
      AUpsfExportField(g_szUpsf_item_signalid, 0L, AUPSF_NATV_LONG, AUPSF_UNIV_NUMBER, &wi->m_uidItem, sizeof(long) );

      if(AUpsfCreateScope(g_szUpsf_item_extrascope, 0)==AUPSF_NOERROR)
        {
        if(WINDOWAPI::IsItemShape(wi) || WINDOWAPI::IsItemGroup(wi))
          {
          AObjMessage msg = {(long)wi, 0L, 0L, 0L, ""};
          if((wr->GetShapeAdmin(wi->Type()))(OSHP_QUERYVERSIONANDID, &msg) == A_OK)
            {
            ExportShort(g_szUpsf_shape_version, 0, (short)msg.message2);
            ExportShort(g_szUpsf_shape_type, 0, (short)msg.message3);
            }
          }
        AUpsfExportField(g_szUpsf_shape_dll, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_STRING, wid->m_szShapeDll, lstrlen(wid->m_szShapeDll));
        wi->ExportExtraData();
        AUpsfCloseScope(); //extra data scope
        }

      AUpsfCloseScope(); //Item scope
      }

    wi->Destroy();
    UnlockItemData(m_oiSelf, i);
    }

  delete wr;
  return A_OK;
  }

/////////////////////////////////////////////////


long WINDOW::Import()
  {
  BOOL                    bool, isShape, isGroup;
  int                     i, numItems;
  WNDITEM_DATA*           wid;
  OpWndItemD**            array;
  AMEMBLOCKID              miArray;
  WINDOWRUNTIME*          wr;
	int											version=0;



  wr = new WINDOWRUNTIME(1, TRUE);

 //get the version of the export so we can tell if it was done by Workshop
	AUpsfImportField(g_szUpsf_version, 0L, AUPSF_NATV_INT, &version, sizeof(int));

  if(AUpsfImportField(g_szUpsf_title, 0L, AUPSF_NATV_STRING, m_szTitle, I_TITLE_LENGTH) != AUPSF_NOERROR)
    LogError(g_szUpsf_title);

  /// -- Reconstruct the window style ------------------
  if(AUpsfImportField(g_szUpsf_titlebar, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_titlebar);
  else
    SetHasTitleBar(bool);

  if(AUpsfImportField(g_szUpsf_sysmenu, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_sysmenu);
  else
    SetHasSysMenu(bool);

  if(AUpsfImportField(g_szUpsf_maxbutton, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_maxbutton);
  else
    SetHasMaxButton(bool);

  if(AUpsfImportField(g_szUpsf_minbutton, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_minbutton);
  else
    SetHasMinButton(bool);

  if(AUpsfImportField(g_szUpsf_horzscroll, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_horzscroll);
  else
    SetHasHorzScroll(bool);

  if(AUpsfImportField(g_szUpsf_vertscroll, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_vertscroll);
  else 
    SetHasVertScroll(bool);

  if(AUpsfImportField(g_szUpsf_resize, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_resize);
  else
    SetHasResizeFrame(bool);

  if(AUpsfImportField(g_szUpsf_dialogframe, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_dialogframe);
  else
    SetHasDialogFrame(bool);

  if(AUpsfImportField(g_szUpsf_modal, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_modal);
  else
    SetIsModal(bool);

  if(AUpsfImportField(g_szUpsf_ontop, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_ontop);
  else
    SetIsPopup(bool);

  if(AUpsfImportField(g_szUpsf_initvisible, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_initvisible);
  else
    SetIsInitVisible(bool);

  if(AUpsfImportField(g_szUpsf_drawsmooth, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_drawsmooth);
  else
    SetDrawsSmooth(bool);

  if(AUpsfImportField(g_szUpsf_closeonexit, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    {
    LogInfo(g_szUpsf_closeonexit);
    SetQuitsOnClose(FALSE);
    }
  else
    SetQuitsOnClose(bool);

  // Hopefully the style has been reconstructed properly -----

  if(AUpsfImportField(g_szUpsf_limitsize, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
    LogError(g_szUpsf_limitsize);
  else
    SetHasMaxSize(bool);

  if (ImportLogicalPoint(m_oiSelf, g_szUpsf_maxsize, 0L, &m_ptlMaxSize) != AUPSF_NOERROR)
    LogError(g_szUpsf_maxsize);

  if(ImportLogicalPoint(m_oiSelf, g_szUpsf_minsize, 0L, &m_ptlMinSize) != AUPSF_NOERROR)
    LogError(g_szUpsf_minsize);

  if(ImportLogicalPoint(m_oiSelf, g_szUpsf_scrollsize, 0L, &m_ptlScrollSize) != AUPSF_NOERROR)
    LogError(g_szUpsf_scrollsize);

  if(AUpsfImportField(g_szUpsf_menuid, 0L, AUPSF_NATV_OBJECTREF, &m_oiMenu, sizeof(AOBJECTID)) != AUPSF_NOERROR)
    LogError(g_szUpsf_menuid);

  if(AUpsfImportField(g_szUpsf_nextsignal, 0L, AUPSF_NATV_LONG, &m_uidNext, sizeof(long)) != AUPSF_NOERROR)
    {
    LogError(g_szUpsf_nextsignal);
    m_uidNext = 100;
    }

  // import the window background color and pattern information
  ImportBrush(g_szUpsf_bg_pattern, 0, m_awPattern);
  bool = FALSE;
  if((ImportBool(g_szUpsf_bg_fgdefault, 0, &bool)==AUPSF_NOERROR) && bool)
    m_clrForeground = I_WINDOW_FOREGROUND;
  else
    ImportColor(g_szUpsf_bg_fgcolor, 0, &m_clrForeground);

  bool = FALSE;
  if((ImportBool(g_szUpsf_bg_bgdefault, 0, &bool)==AUPSF_NOERROR) && bool)
    m_clrBackground = I_WINDOW_BACKGROUND;
  else
    ImportColor(g_szUpsf_bg_bgcolor, 0, &m_clrBackground);

	// and the window size
  if(ImportLogicalRect(m_oiSelf, g_szUpsf_initrect, 0L, &m_rclInitialClient) != AUPSF_NOERROR)
    LogError(g_szUpsf_initrect);
	else if (version < 200) //it was done by workshop
		{
		WndLStoDS((LPPOINT)&m_rclInitialClient, 2);
		if (HasVertScroll())
			m_rclInitialClient.right -= (GetSystemMetrics(SM_CXVSCROLL) - 1);
		if (HasHorzScroll())
			m_rclInitialClient.bottom -= (GetSystemMetrics(SM_CYHSCROLL) - 1);
		if (HasTitleBar())
			m_rclInitialClient.top += GetSystemMetrics(SM_CYCAPTION);
		if (HasResizeFrame())
			{
			m_rclInitialClient.top		+= GetSystemMetrics(SM_CYFRAME);
			m_rclInitialClient.bottom	-= GetSystemMetrics(SM_CYFRAME);
			m_rclInitialClient.left		+= GetSystemMetrics(SM_CXFRAME);
			m_rclInitialClient.right	-= GetSystemMetrics(SM_CXFRAME);
			}
		if (m_oiMenu)
			{
			HDC 						hdc;
			TEXTMETRIC			tm;
			hdc = GetDC(NULL);
			GetTextMetrics(hdc,	&tm);
			ReleaseDC(NULL, hdc);
			m_rclInitialClient.top += tm.tmHeight + 1;
			}
		WndDStoLS((LPPOINT)&m_rclInitialClient, 2);
		}
		


  // Now, the thrilling task of importing the items ------
  numItems = (int)AUpsfFieldCount(g_szUpsf_item_scope);
  miArray = AMemAllocate(numItems*sizeof(OpWndItemD*));
  array = (OpWndItemD**)AMemLock(miArray);
  _fmemset(array, 0, numItems*sizeof(OpWndItemD*));

  for(i=0; i<numItems; i++)
    {
    if ( AUpsfOpenScope(g_szUpsf_item_scope, i) != AUPSF_NOERROR)
      {
      LogError(g_szUpsf_item_scope);
      AMemUnlock(miArray);
      AMemFree(miArray);
      return A_ERROR;
      }

    wid = new WNDITEM_DATA();

    isShape = FALSE;
    ImportBool(g_szUpsf_item_shape, 0L, &isShape);
    isGroup = FALSE;
    ImportBool(g_szUpsf_item_group, 0L, &isGroup);

    if(isShape || isGroup)
      AUpsfImportField(g_szUpsf_item_typeid, 0L, AUPSF_NATV_LONG, &wid->m_oiItem, sizeof(AOBJECTID));
    else
      {
      if(AUpsfImportField(g_szUpsf_item_id, 0L, AUPSF_NATV_OBJECTREF, &wid->m_oiItem, sizeof(AOBJECTID)) != AUPSF_NOERROR)
        LogError(g_szUpsf_item_id);

      if(wid->m_oiItem == m_oiSelf)
        {
        isShape = TRUE;
        wid->m_oiItem = (AOBJECTID)ATYPE_MAKEID('S', 'h', 'p', 'S');
        }
      }

    if(ImportLogicalRect(m_oiSelf, g_szUpsf_item_rect, 0L, &wid->m_rcItem) != AUPSF_NOERROR)
      LogError(g_szUpsf_item_rect);

		//import the items status
    wid->m_itemStatus = 0;

    if(AUpsfImportField(g_szUpsf_item_visible, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
			{
   		wid->m_itemStatus |= I_SF_VISIBLE;	//default to visible
      LogError(g_szUpsf_item_visible);
			}
    else
      wid->m_itemStatus |= (bool ? I_SF_VISIBLE : 0);
    
    if(AUpsfImportField(g_szUpsf_item_tabstop, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
			{
   		wid->m_itemStatus |= I_SF_TABSTOP; //default to tabstop
      LogInfo(g_szUpsf_item_tabstop);
			}
    else
      wid->m_itemStatus |= (bool ? I_SF_TABSTOP : 0);

    if(AUpsfImportField(g_szUpsf_item_groupstop, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
      LogInfo(g_szUpsf_item_groupstop);
    else
      wid->m_itemStatus |= (bool ? I_SF_GROUPSTOP : 0);

    if(AUpsfImportField(g_szUpsf_item_locked, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
      LogInfo(g_szUpsf_item_locked);
    else
      wid->m_itemStatus |= (bool ? I_SF_LOCKED : 0);


    if(AUpsfImportField(g_szUpsf_item_growsflag, 0L, AUPSF_NATV_INT, &wid->m_itemGrow, sizeof(int)) != AUPSF_NOERROR)
      LogError(g_szUpsf_item_growsflag);

    if(AUpsfImportField(g_szUpsf_item_signalid, 0L, AUPSF_NATV_LONG, &wid->m_uidItem, sizeof(long)) != AUPSF_NOERROR)
      {
      LogError(g_szUpsf_item_signalid);
      wid->m_uidItem = m_uidNext; 
      m_uidNext += 10L;
      }

    array[i] = NULL;
    if ((AUpsfOpenScope(g_szUpsf_item_extrascope, 0) == AUPSF_NOERROR)
              ||
        (AUpsfOpenScope("SHAPSCOP", 0) == AUPSF_NOERROR))
      {
      if(isShape || isGroup)
        {
        short sVer  = 0;  // use 0 as the default
        short sType = 0;  // use 0 as the default

        // use standard shape library as default -- for Mac and Serius Workshop import/export
        lstrcpy(wid->m_szShapeDll, isShape ? "AW32SHAP.DLL" : "AW32WND.DLL");
        AUpsfImportField(g_szUpsf_shape_dll, 0L, AUPSF_NATV_STRING, wid->m_szShapeDll, 13);
				//Kludge, but so what
				if (strcmpi(wid->m_szShapeDll, "ALM_SHAP.DLL")==0)
	        lstrcpy(wid->m_szShapeDll, "AW32SHAP.DLL");

        ImportShort(g_szUpsf_shape_version, 0L, &sVer);
        ImportShort(g_szUpsf_shape_type, 0L, &sType);
        array[i] = wid->CreateShapeVI(m_oiSelf, sVer, sType, i);
        }
      else
        array[i] = wid->CreateVI(m_oiSelf, sizeof(WNDITEM_DATA), i);

      if(array[i])
        array[i]->ImportExtraData();
      AUpsfCloseScope(); //Extra data scope
      }
    else
      {
      array[i] = wid->CreateVI(m_oiSelf, sizeof(WNDITEM_DATA), i);
      LogError(g_szUpsf_item_scope);
      }

    if(!array[i])
      {
      array[i] = CreateGenericItem(wid->m_oiItem, m_oiSelf, &wid->m_rcItem, wid->m_itemStatus, NULL);

      I_ASSERT(array[i]);
      array[i]->m_grow    = wid->m_itemGrow;
      array[i]->m_uidItem = wid->m_uidItem;
      array[i]->m_index   = i;
      }

    AUpsfCloseScope(); //Item scope
    delete wid;
    }

  AMemUnlock(miArray);

  PutItemsIntoDatas(miArray, numItems);
  DestroyAllItems(miArray, numItems);

  AMemFree(miArray);
  
  delete wr;
  return A_OK;
  }

