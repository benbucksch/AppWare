//======================================================================
// exerespl.h -- runtime response list
//
// jon 12-Jun-92 created
// jon 18-Sep-92 explicitly state that responses are sortable!
// jon 28-Sep-92 moved dtor out of .h to lose "can't inline for stmts"
//======================================================================

#ifndef __EXERESPL_H
#define __EXERESPL_H

#include "exeresp.h"
#include "exelistb.h"
#include "exehead.h"

class EXERESPONSELIST : public EXELISTBASE
  {

private:

  LONG GetNodeKey(
    LONG theNodeIdx)
    {
    return ((LPEXERESPONSE)dataPtr + (int)theNodeIdx)->typeID;
    }

  LONG GetNodeKey(
    LPVOID theNodePtr)
    {
    return ((LPEXERESPONSE)theNodePtr)->typeID;
    }

public:

  void  * operator new(
	  size_t  size)
	  {

	  return ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
     ->GetExeResponseListBlock()->AddRecord(size);
	  }

  void  * operator new(
	  size_t    /*-- size --*/,
    void  *objPtr)
	  {
	  return objPtr;
	  }

  void operator delete(
    void *  objPtr,
	  size_t     size)
	  {
	  ((LPEXEHEAD)GetObject(pTheSystem->GetFileObject()->GetHeadExeRec()))
     ->GetExeResponseListBlock()->DeleteRecord(objPtr, size);
	  }

  EXERESPONSELIST()

    : EXELISTBASE(sizeof(EXERESPONSE))

    {
    isSortable = TRUE;
    }

  EXERESPONSELIST(
    void  *objPtr)

    : EXELISTBASE(objPtr)

    {
    isSortable = TRUE;
    }

  ~EXERESPONSELIST();

  BOOL AddResponseFor(
    ATYPEID theType)
    {
    EXERESPONSE  resp(theType, (METHODPROC)0);

    return this->AddNode(&resp);
    }

  LPEXERESPONSE FindNode(
    ATYPEID theType)
    {
    return (LPEXERESPONSE)EXELISTBASE::FindNode(theType);
    }

  LPEXERESPONSE IdxGetNode(
    LONG theIdx)
    {
    return (LPEXERESPONSE)EXELISTBASE::IdxGetNode(theIdx);
    }

  };

#endif  // __EXERESPL_H
