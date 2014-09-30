#include "almsys.h"
#include "ownerlst.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~OWNERLIST
//
// CLASS.......: OWNERLIST destructor
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
OWNERLIST::~OWNERLIST()
	{
  LPOWNERTYPE  pOwnerType;
	int i;

	//clean up the list of owners
  for (i=0; i<typeList.GetCount(); i++)
    {
    pOwnerType = (LPOWNERTYPE)typeList.GetElement((short)i);
    delete pOwnerType;
    }
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: AddOwner
//
// CLASS.......: OWNERLIST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL OWNERLIST::AddOwner(
  AOBJECTID ownerID,
  long     ownerData)
  {
  ATYPEID       ownerType;
  short        i;
  LPOWNERTYPE  pOwnerType;
  LPOWNER      pOwner;
  BOOL         result = FALSE;

  ownerType = AObjGetType(ownerID);

  if (ownerType != 0)
    {
    for (i=0; i<typeList.GetCount(); i++)
      {
      pOwnerType = (LPOWNERTYPE)typeList.GetElement(i);
      if (ownerType == pOwnerType->type)
        break;
      }

    if (i >= typeList.GetCount())
      { //did not find type in array, add to end of the array
      pOwnerType = (LPOWNERTYPE)typeList.Insert(32767);
      new(pOwnerType) OWNERTYPE(ownerType);    //initialize the owner type
      }

    if (pOwnerType != 0)
      {
      //Add owner to end of owner list and fill it in
      pOwner = (LPOWNER)pOwnerType->ownerList.Insert(32767);
      pOwner->ownerID = ownerID;
      pOwner->ownerData = ownerData;
      result = TRUE;
      }

    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RemoveOwner
//
// CLASS.......: OWNERLIST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL OWNERLIST::RemoveOwner(
  AOBJECTID  ownerID,
  long      ownerData)
  {
  ATYPEID       ownerType;
  short        typeIdx;
  short        ownerIdx;
  LPOWNERTYPE  pOwnerType;
  LPOWNER      pOwner;
  BOOL         result = FALSE;

  ownerType = AObjGetType(ownerID);

  if (ownerType != 0)
    {
    for (typeIdx=0; typeIdx<typeList.GetCount(); typeIdx++)
      {
      pOwnerType = (LPOWNERTYPE)typeList.GetElement(typeIdx);
      if (ownerType == pOwnerType->type)
        break;
      }

    if (typeIdx < typeList.GetCount())           //found the type
      {
      for (ownerIdx=0; ownerIdx<pOwnerType->ownerList.GetCount(); ownerIdx++)
        {
        pOwner = (LPOWNER)pOwnerType->ownerList.GetElement(ownerIdx);
        if (pOwner->ownerID == ownerID && pOwner->ownerData == ownerData)
          {
          pOwnerType->ownerList.Delete(ownerIdx);
          // if no more owners of this type, delete the type
          if (pOwnerType->ownerList.GetCount() == 0)
            {
            delete pOwnerType;
            this->typeList.Delete(typeIdx);
            }
          result = TRUE;
          break;
          }
        }
      }
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetNextOwner
//
// CLASS.......: OWNERLIST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL OWNERLIST::GetNextOwner(
  ATYPEID            ownerType,
  pAObjOwnerInfo  pInfo)
  {
  short        i;
  LPOWNERTYPE  pOwnerType;
  LPOWNER      pOwner;
  BOOL         result = FALSE;

  if (ownerType != 0)
    //  get the first one
    {
    if (pInfo->key == AOBJ_GETFIRSTKEY)
      {
      for (i=0; i<typeList.GetCount(); i++)
        {
        pOwnerType = (LPOWNERTYPE)typeList.GetElement(i);
        if (ownerType == pOwnerType->type)
          break;
        }
      if (i < typeList.GetCount())           //found the type
        {
        pOwner = (LPOWNER)pOwnerType->ownerList.GetElement(0);
        if (pOwner != 0)
          {
          pInfo->ownerID   = pOwner->ownerID;
          pInfo->ownerData = pOwner->ownerData;
          pInfo->key       = 1;
          pInfo->reserved1 = i;
          result = TRUE;
          }
        }
      }
    else
      // Get the next one
      {
      pOwnerType = (LPOWNERTYPE)typeList.GetElement((short)pInfo->reserved1);
      if ((pOwnerType != 0) && (ownerType == pOwnerType->type))
        {
        pOwner = (LPOWNER)pOwnerType->ownerList.GetElement((short)pInfo->key);
        if (pOwner != 0)
          {
          pInfo->ownerID   = pOwner->ownerID;
          pInfo->ownerData = pOwner->ownerData;
          pInfo->key++;
          result = TRUE;
          }
        else
          {
          pInfo->ownerID   = 0;
          pInfo->ownerData = 0;
          }
        }
      }
    }

  return result;

  }
