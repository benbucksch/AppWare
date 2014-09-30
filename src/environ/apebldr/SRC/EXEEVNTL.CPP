//======================================================================
// exeevntl.cpp -- runtime event list
//
// jon 12-Jun-92 created
//======================================================================

	#include "almsys.h"
	#include "intalm.h"

  #include "exeevntl.h"

EXEEVENTLIST::~EXEEVENTLIST()
  {
  LPEXEEVENT      exeEvent;
  long            i;

  for (i = 0 ; 0 != (exeEvent = (LPEXEEVENT)this->IdxGetNode(i)) ; i++)
    {
    delete exeEvent;
    }

  }

BOOL EXEEVENTLIST::SetMethodAddr(PROC procAddr,
                                 ATYPEID  theTypeID,
                                 AEVENTID theEventID)
  {
  LPEXERESPONSELIST   responseList;
  LPEXERESPONSE       response;

  responseList = this->GetResponseList(theEventID);
  if (!responseList)
    return FALSE;

  response = responseList->FindNode(theTypeID);
  if (!response)
    return FALSE;

  return response->SetMethodAddr((METHODPROC)procAddr);
  }



// -- end exeevntl.cpp
