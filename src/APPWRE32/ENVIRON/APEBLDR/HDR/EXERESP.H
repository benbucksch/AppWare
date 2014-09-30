// exeresp.h --> EXERESPONSE

#ifndef __EXERESP_H
#define __EXERESP_H

class EXERESPONSE
	{

friend class EXERESPONSELIST;

private:

  ATYPEID        typeID;
  METHODPROC    method;

public:

  EXERESPONSE(ATYPEID theType, METHODPROC  theMeth)
    {
    typeID = theType;
    method = theMeth;
    }

  ~EXERESPONSE()
    {
    }

  // A dummy delete to override default ..
  // .. since we never 'new' these things
  void operator delete(
    void *  /*theExeEvent*/,
	  size_t     /*theSize*/)
	  {
	  }

  BOOL SetTypeID(
    LONG theTypeID)
    {
    typeID = theTypeID;
    return TRUE;
    }

  BOOL SetMethodAddr(
    METHODPROC theMethod)
    {
    method = theMethod;
    return TRUE;
    }

  LONG GetTypeID()
    {
    return typeID;
    }

  METHODPROC GetMethod()
    {
    return method;
    }
  };

#endif  // __EXERESP_H
