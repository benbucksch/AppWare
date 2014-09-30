#include "w_includ.h"
#include "o_wndshp.h"

/////////////////////////////////////

WNDITEM_DATA::WNDITEM_DATA()
  {
  m_size        = sizeof(WNDITEM_DATA);
  m_oiItem      = 0;
  m_uidItem     = 0;
  m_itemStatus  = 0;
  m_itemGrow    = 0;
  SetRect(&m_rcItem, 0, 0, 0, 0);
  *m_szShapeDll = 0;
  }

/////////////////////////////////////

WNDITEM_DATA::WNDITEM_DATA(
  OpWndItemD*             wi)
  {
  WINDOWRUNTIME*          wr = GetWindowRuntime();

  I_VERIFY_POINTER(wr, return);

  m_size        = sizeof(WNDITEM_DATA);
  m_oiItem      = wi->Object();
  m_uidItem     = wi->m_uidItem;
  m_itemStatus  = wi->m_status;
  m_itemGrow    = wi->m_grow;
  CopyRect(&m_rcItem, &wi->m_rcItem);

  if (WINDOWAPI::IsItemShape(wi) || WINDOWAPI::IsItemGroup(wi))
    {
    if ( !wr->GetShapeLibraryName((ATYPEID)m_oiItem, m_szShapeDll) )
      *m_szShapeDll = 0;
    }
  else
    *m_szShapeDll = 0;
  }

/////////////////////////////////////////

BOOL WNDITEM_DATA::Validate(
  pAObjMessage            theSystem)
  {
  BOOL                    rVal;

  if (m_szShapeDll[0])  //It the item a shape
    rVal = TRUE;
  else
    {
    AObjValidateID(&m_oiItem, theSystem);
    rVal = m_oiItem ? TRUE : FALSE;
    }
    
  return rVal;
  }

/////////////////////////////////////////

BOOL WNDITEM_DATA::Check()
  {  
  BOOL                    rVal;
  
  if (m_szShapeDll[0])  //It the item a shape
    rVal = TRUE;
  else
    rVal = AObjCheckType(m_oiItem, 0);

  return rVal;
  }

/////////////////////////////////////////

void WNDITEM_DATA::GetName(
  int		               		idxItem,
  LPSTR                   szName, 
  int		                  cbLength)
  {
  I_ASSERT(cbLength>10);

  if (m_itemStatus & I_SF_SHAPE)
    wsprintf(szName, "Shape #%d", idxItem+1);
  else if (m_itemStatus & I_SF_GROUP)
    wsprintf(szName, "Group #%d", idxItem+1);
  else
    AObjGetName(m_oiItem, szName, cbLength);
  }

/////////////////////////////////////////

OpWndItemD* WNDITEM_DATA::CreateVI(
  AOBJECTID                oiParent,
  long                    lDataBlockSize, 
  int                   	idxItem)
  {
  OpWndItemD*             wi = NULL;
  ApQueryInfo             qi;
  PWICreateFN             createFunction;
  AMEMBLOCKID              miExtraData;
  LPVOID                  extraDataSource, extraDataTarget;
  LONG                    lExtraDataSize;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  
  I_VERIFY_POINTER(wr, return NULL);
  
  createFunction = NULL;

  if (*m_szShapeDll)
    createFunction = wr->GetShapeCreateAndAddToList(m_szShapeDll, (ATYPEID)m_oiItem);
  else if ( AObjCheckType(m_oiItem, 0)  &&  AObjQueryProtocol(m_oiItem, &qi, APROT_WND) != AP_NOTSUPPORTED)
    createFunction = (PWICreateFN)qi.pfnCreate;

  if (createFunction)  //Did we sucessfully get the create function???
    {
    lExtraDataSize = lDataBlockSize - sizeof(WNDITEM_DATA);
    if (lExtraDataSize > 0L)
      {
      miExtraData = AMemAllocate(lExtraDataSize);
      extraDataTarget = AMemLock(miExtraData);
      extraDataSource  = (LPSTR)this + sizeof(WNDITEM_DATA);
      CopyMemory(extraDataTarget, extraDataSource, lExtraDataSize);
      AMemUnlock(miExtraData);
      }
    else
      miExtraData = 0L;

    wi = createFunction(m_oiItem, oiParent, &m_rcItem, m_itemStatus, miExtraData);
    if (wi)
      {
      wi->m_grow            = m_itemGrow;
      wi->m_uidItem         = m_uidItem;
      wi->m_index           = idxItem;
      }

    if (miExtraData)
      AMemFree(miExtraData);
    } //if(createFunction)

  return wi;
  }

/////////////////////////////////////////

OpWndItemD* WNDITEM_DATA::CreateShapeVI(
  AOBJECTID                oiParent,
  short                   sVer,
  short                   sType,
  int                   	nIdx)
  {
  OpWndItemD*             wi = NULL;
  PWICreateFN             lpfnCreate;
  AMEMBLOCKID              miExtraData;
  OShpBase*               pExtra;
  WINDOWRUNTIME*          wr = GetWindowRuntime();
  
  I_VERIFY_POINTER(wr, return NULL);
  I_ASSERT(*m_szShapeDll);
  
  lpfnCreate = wr->GetShapeCreateAndAddToList(m_szShapeDll, (ATYPEID)m_oiItem);

  if (lpfnCreate)  //Did we sucessfully get the create function???
    {
    miExtraData = AMemAllocate(sizeof(OShpBase));
    pExtra = (OShpBase*)AMemLock(miExtraData);
    pExtra->sVer  = sVer;
    pExtra->sType = sType;
    AMemUnlock(miExtraData);

    wi = lpfnCreate(m_oiItem, oiParent, &m_rcItem, m_itemStatus, miExtraData);
    if (wi)
      {
      wi->m_grow            = m_itemGrow;
      wi->m_uidItem         = m_uidItem;
      wi->m_index           = nIdx;
      }

    AMemFree(miExtraData);
    } //if(lpfnCreate)

  return wi;
  }

//////////////////////////////////////

