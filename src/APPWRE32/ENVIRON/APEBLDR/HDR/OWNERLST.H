#ifndef __OWNERLST_H__
#define __OWNERLST_H__
#include <stdlib.h>
#include "array.h"

////////////////////////////////////////////////////////////////////////////
// struct:      OWNERTYPE
//
// DESCRIPTION: The structure of the entries in the ownersList array of
//              OWNERTYPE structure.
//
////////////////////////////////////////////////////////////////////////////
struct OWNER
  {
  AOBJECTID ownerID;
  long     ownerData;
  };

typedef OWNER  *  LPOWNER;

////////////////////////////////////////////////////////////////////////////
// struct:      OWNERTYPE
//
// DESCRIPTION: The structure of the entries in the typeList array of
//              OWNERLIST class.
//
////////////////////////////////////////////////////////////////////////////
struct OWNERTYPE
  {
  ATYPEID       type;
  SERIUSARRAY  ownerList;

  void  * operator new(
    size_t  /*size*/)
    {
    return 0;
    }

  void  * operator new(
    size_t           /*size*/,
    OWNERTYPE  *  objptr)
    {
    return objptr;
    }

  void operator delete(
    void *  /*objptr*/)
    {
    }

  OWNERTYPE(
    ATYPEID  typeID):
      ownerList(sizeof(OWNER))
    {
    type = typeID;
    }

  };

typedef OWNERTYPE *  LPOWNERTYPE;

////////////////////////////////////////////////////////////////////////////
// CLASS:       OWNERLIST
//
// DESCRIPTION: The owner list class manages a structure off an object that
//              contains a list of the object types that own the object.
//              Each owning type entry then keeps a list of the owning
//              objects of that type.  This class is designed to be included
//              as a data member in a class that inherits from SERIUSRECORD.
//
////////////////////////////////////////////////////////////////////////////
class OWNERLIST
  {

public:

  OWNERLIST(
    void):
      typeList(sizeof(OWNERTYPE))
    {
    flags = 0;
    }

  //conststructor called when the record is read from disk
  OWNERLIST(
    void  * objPtr):
      typeList(objPtr)
    {
    }

	~OWNERLIST();

  BOOL AddOwner(
    AOBJECTID ownerID,
    long     ownerData);

  BOOL RemoveOwner(
    AOBJECTID ownerID,
    long     ownerData);

  BOOL GetNextOwner(
		ATYPEID            ownerType,
    pAObjOwnerInfo  pInfo);

private:
  SERIUSARRAY   typeList;
  unsigned long flags;

  };

#endif
