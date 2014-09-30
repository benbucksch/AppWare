//======================================================================
// exefunc.cpp -- runtime Function instance
//
// jon 19-Aug-92 modified GetParameter to do optional type-coercion
// jon  2-Sep-92 SetParameter deletes temp objects bound to real ones
// jon 16-Sep-92 reworked SetParameter so that temp objects will always
//               be deleted, even if they are not connected.
//======================================================================

	 #include "almsys.h"

   #include "exefunc.h"
   #include "param.h"
   #include "funcmask.h"
   #include "sereclib.h"
   #include "rectypes.h"
   #include "exeobjs.h"
   #include "compuid.h"
   #include "tempobj.h"
	 #include "exeparam.h"
   #include "exrecblk.h"
   #include "rtreclib.h"
	 #include "intdebug.h"

////////////////////////////////////////////////////////////////////////
//    EXEFUNCINST::EXEFUNCINST (constructor)
//
// Allocate parameter blocks
////////////////////////////////////////////////////////////////////////
EXEFUNCINST::EXEFUNCINST(
	WORD newStatus):status(newStatus)
	{
	if (status & FN_INPUTS)
    inputParams =
     (new EXERECORDBLOCK(sizeof(EXEPARAM), RT_EXEPARAM))->GetUID();

  if (status & FN_OUTPUTS)
    outputParams =
     (new EXERECORDBLOCK(sizeof(EXEPARAM), RT_EXEPARAM))->GetUID();
  }


////////////////////////////////////////////////////////////////////////
//    EXEFUNCINST::~EXEFUNCINST (destructor)
//
// Free parameter blocks
////////////////////////////////////////////////////////////////////////
EXEFUNCINST::~EXEFUNCINST()
  {
  if (status & FN_INPUTS)
    {
    ((LPEXERECORDBLOCK)GetObject(inputParams))->PurgeBlock();
    delete (LPEXERECORDBLOCK)GetObject(inputParams);
    }

  if (status & FN_OUTPUTS)
    {
    ((LPEXERECORDBLOCK)GetObject(outputParams))->PurgeBlock();
    delete (LPEXERECORDBLOCK)GetObject(outputParams);
    }
  }

////////////////////////////////////////////////////////////////////////
//    EXEFUNCINST::GetParameter
//
//  Gets the specified input parameter for this function.  If theType
//  is specified as non-zero, the parameter will be coerced to that
//  type, or the returned AOBJECTID will be zero.
////////////////////////////////////////////////////////////////////////
AOBJECTID  EXEFUNCINST::GetParameter(
  LONG   paramIndex,
  ATYPEID theTypeID)
  {
  LPEXEPARAM  paramPtr;
  LPEXEPARAM  linkedParamPtr=NULL;
  AOBJECTID    linkedObject;
  AMEMBLOCKID  blockID;
  int         idx;
  int         byteCount;
  ATYPEID      paramType;
  ATYPEID      tempType;
  AOBJECTID    tempObject;
  BOOL        isConstant;

  // Ensure this function has inputs
  if (!(status & FN_INPUTS))
    {

    #ifdef DEBUG
      MessageBox(NULL,
       "[EXEFUNC::GetParameter]\n\n"
       "Parameter requested for a function that has no inputs.",
       "Internal Serius Error", MB_OK|MB_TASKMODAL);
    #endif

    return 0;
    }

  // Ensure the specified parameter exists
  idx = (int)(paramIndex - 1);
  paramPtr = (LPEXEPARAM)GetObject(inputParams, idx);
  if (!paramPtr)
    {

    #ifdef DEBUG
      MessageBox(NULL,
        "[EXEFUNC::GetParameter]\n\n"
        "Parameter index out of range in EXEFUNC::GetParameter.",
        "Internal Serius Error", MB_OK|MB_TASKMODAL);
    #endif

    return 0;
    }

  // Ensure the parameter has been filled in
  if (!paramPtr->TestLinkFlag())
    return 0;

  //--------------------------------------------------------------------
  // Derive the object ID for this parameter.  There are three possible 
  // sources for an input parameter:
  //
  //   (1) object list
  //   (2) output parameter of some other function
  //   (3) constant 
  //--------------------------------------------------------------------

  linkedObject = 0;

  if (paramPtr->TestObjConnectFlag())
    {
    //------------------------------------------------------------------
    // case 1 -- an object on the object list
    //------------------------------------------------------------------
    isConstant = FALSE;

    linkedObject = paramPtr->GetDataFromUID().id;
    if (!linkedObject)
			return 0;

    if (AObjCheckType(linkedObject, theTypeID))
      return linkedObject;
    else
      tempObject = paramPtr->GetTempObjectUID().id;
    }

  else if (paramPtr->TestParamConnectFlag())
    {
    //------------------------------------------------------------------
    // case 2 -- an output param of some other function
    //------------------------------------------------------------------
    isConstant = FALSE;

    linkedParamPtr = (LPEXEPARAM)GetObject(paramPtr->GetDataFromUID());

		if(!linkedParamPtr)
			{
			//we weren't connected to anything
			return 0;
			}

    if(!linkedParamPtr->TestSlotFilledFlag())
      {
			tempObject = NULL; //will cause a new temp to be created
			}
		else
			{

    	linkedObject = linkedParamPtr->GetDataFromUID().id;
			
			if(!linkedObject)
				return 0;

    	if (AObjCheckType(linkedObject, theTypeID))
      	return linkedObject;
    	else
      	tempObject = paramPtr->GetTempObjectUID().id;
			}

		//if we leave this else clause, tempObject must have been initialized.
    }
  else
    {
    //------------------------------------------------------------------
    // case 3 -- a constant parameter
    //------------------------------------------------------------------
		isConstant = TRUE;

    tempObject = paramPtr->GetTempObjectUID().id;

    if (AObjCheckType(tempObject, theTypeID))
      return tempObject;
    }

//----------------------------------------------------------------------
// Being here means we are going to pass back a temporary object
//----------------------------------------------------------------------

  // Determine the type for the temp object.  If the parameter is not
  // typed then we will use the type specified by our caller.  If
  // the caller did not specify a type [ie, AFuncGetParameter() instead
  // of AFuncGetTypedParameter()] then we will create a temporary Serius
  // Text object.
  paramType = paramPtr->GetTypeID();
 
 	//figure the type we'll need to create
 	if(theTypeID && (theTypeID != COMPUID_GENERAL))
		tempType = theTypeID;	//create requested type
	else if(paramType == COMPUID_GENERAL)
		tempType = OTYPE_TEXT;	//default to text object
	else
		tempType = paramType;	 //create param type
 
  if (tempObject)
    {
    // This parameter had a temp object attached from the previous flow
    // through this function .. do the types match?
    if (tempType == AObjGetType(tempObject))
      {
      // The previous temporary object is the type we need,
      // fill it in and go home 
      if (!isConstant)
        AObjAssign(linkedObject, tempObject);

      return tempObject;
      }
    else
      {
      // Bye bye, previous temp object
      AObjDestroy(tempObject);
      paramPtr->SetTempObjectUID(0);
      }
    }

//----------------------------------------------------------------------
// Being here means this parameter has never had a temp object in it
// (because it's the first time we've gotten it) *OR* we can't re-use
// the temp object that was here before (probably because the type
// is different)
//----------------------------------------------------------------------

  tempObject = AObjCreate(tempType);
  if (!tempObject)
    return 0;

  TEMPOBJECT::GetObject(tempObject)->IncrementUseCount();

  
  if(linkedParamPtr && !linkedParamPtr->TestSlotFilledFlag())
		{
		//return an uninitialized parameter - a result of incorrect
		//VAB user logic.
		linkedParamPtr->SetSlotFilledFlag();
		linkedParamPtr->SetDataFromUID(tempObject);
		}
	else
		{
	  // Fill in a serius memblock with the text value of the parameter
	  if (isConstant)
	    {
			LPCSTR       paramText		= paramPtr->GetName();

      byteCount = lstrlen(paramText)+1;
	    blockID = AMemAllocate(byteCount);
	    lstrcpy((LPSTR)AMemLock(blockID), paramText);
			AMemUnlock(blockID);
	    }
	  else
	    {
	    blockID = AMemAllocate(0);
	    AObjConvertToText(linkedObject, blockID);
	    }

	  // Fill in the temp object to be output
	  if (!AObjConvertFromText(tempObject, blockID))
	    {
	    AObjDestroy(tempObject);
	    return 0;
	    }

	  // Clean up and go home
	  AMemFree(blockID);

	  // Remember the object we will hand back (for the next time around...)
	  paramPtr->SetTempObjectUID(tempObject);
		}

  return tempObject;
  }

////////////////////////////////////////////////////////////////////////
//    EXEFUNCINST::SetParameter
//
//  Sets the specified output parameter for this function.
////////////////////////////////////////////////////////////////////////
BOOL EXEFUNCINST::SetParameter(
  LONG       parameterIndex,
  AOBJECTID   theObject)
  {
  LPEXEPARAM     paramPtr;
  int            myIdx = (int)parameterIndex - 1;
  LPOBJECTRECORD objPtr;

  if (theObject > 0)
    objPtr = (LPEXEOBJINST)GetSeriusObject(theObject);
  else if (theObject < 0)
		{
    objPtr = TEMPOBJECT::GetObject(theObject);
		if(objPtr)
			objPtr->IncrementUseCount();
		}
  else
    objPtr = 0;

  if (theObject && !objPtr)
    {

    #ifdef DEBUG
      MessageBox(NULL,
       "[EXEFUNC::SetParameter]\n\n"
       "Bad objectID for parameter.",
       "Internal Serius Error", MB_OK|MB_TASKMODAL);
    #endif

    return FALSE;
    }

  if (!status & FN_OUTPUTS)
    {

    #ifdef DEBUG
     MessageBox(NULL,
       "[EXEFUNC::SetParameter]\n\n"
       "This primitive was called for a Serius Function"
			 " that has no output parameters.",
       "Internal Serius Error", MB_OK|MB_TASKMODAL);
    #endif

    return FALSE;
    }

  paramPtr = (LPEXEPARAM)GetObject(outputParams, myIdx);
  if (!paramPtr)
    {

    #ifdef DEBUG
     MessageBox(NULL,
       "[EXEFUNC::SetParameter]\n\n"
       "Output parameter index is out of bounds.",
       "Internal Serius Error", MB_OK|MB_TASKMODAL);
    #endif

    return FALSE;
    }

  //--------------------------------------------------------------------
  // There are three possible cases for output parameter connection
  //
  //   (1) not connected
  //   (2) connected to object on object list
  //   (3) connected as input parameter to one or more other functions
  //--------------------------------------------------------------------

  if (!paramPtr->TestLinkFlag())
    {
    //------------------------------------------------------------------
    // case 1 -- it's not hooked to anything
    //------------------------------------------------------------------

    if(objPtr && objPtr->TestTemporaryFlag())
    	AObjDestroy(FASTRECMGR::GetUID(objPtr));

		return TRUE;
    }
  else if (paramPtr->TestObjConnectFlag())
    {
    //------------------------------------------------------------------
    // case 2 -- it's connected to a "real" object
    //------------------------------------------------------------------
    BOOL result;

		I_ASSERT(objPtr);

    result = AObjAssign(theObject, paramPtr->GetDataFromUID().id);
    
    if(objPtr && objPtr->TestTemporaryFlag())
    	AObjDestroy(FASTRECMGR::GetUID(objPtr));

    return result;
    }
  else
    {
    //------------------------------------------------------------------
    // case 3 -- hold its ID in an "output slot"
    //------------------------------------------------------------------

    // Decrement use count of any temporary object already in the slot
    if (paramPtr->TestSlotFilledFlag() &&
   		(paramPtr->GetDataFromUID().id < 0))
	  	AObjDestroy(paramPtr->GetDataFromUID().id);

    paramPtr->SetSlotFilledFlag();
    paramPtr->SetDataFromUID(theObject);

    return TRUE;
    }
	}


////////////////////////////////////////////////////////////////////////
//    EXEFUNCINST::GetOutputParameter
//
//  Gets the specified output parameter object. If the output is not
//  connected, zero is returned.  If the output is connected to a real
//  object, the ID of that object is returned.  If the output is
//  connected to one or more inputs the id of a temporary object is
//  returned.
////////////////////////////////////////////////////////////////////////
AOBJECTID EXEFUNCINST::GetOutputParameter(
  LONG      parameterIndex,
  LPTYPEID  lpTypeID)
  {
  AOBJECTID       theObject = 0;
  LPEXEPARAM     paramPtr;
  int            myIdx = (int)parameterIndex - 1;

  *lpTypeID = 0;

  if (status & FN_OUTPUTS)
    {
    paramPtr = (LPEXEPARAM)GetObject(outputParams, myIdx);
    if (paramPtr)
      {
      if (paramPtr->TestObjConnectFlag())
        {
        theObject = paramPtr->GetDataFromUID().id;
        *lpTypeID = AObjGetType(theObject);
        }
      else if (paramPtr->TestLinkFlag() == TRUE)
        {
				BOOL needNewTemp = TRUE;

        if (paramPtr->TestSlotFilledFlag())
          {
          theObject = paramPtr->GetDataFromUID().id;
         
					I_ASSERT(TEMPOBJECT::GetObject(theObject));

          if(((LPOBJECTRECORD)TEMPOBJECT::GetObject(theObject))->GetUseCount() > 1)
						{	
						//Calling AObjDestroy could be a problem if we end up not creating
						//a temp and AFuncSetParameter is immediately called after
						
						//If the typeID is 0, SetParameter will have to be called which will
						//then call AObjDestroy
						if(paramPtr->GetTypeID() != 0)
							AObjDestroy(theObject);
						}
					else
						{
          	*lpTypeID = AObjGetType(theObject);
						needNewTemp = FALSE;
						}
          }

				if(needNewTemp)
          {
          if (paramPtr->GetTypeID() != 0)
            {
            theObject = AObjCreate(paramPtr->GetTypeID());
            if (theObject != 0)
              {
              *lpTypeID = paramPtr->GetTypeID();
              paramPtr->SetSlotFilledFlag();
              paramPtr->SetDataFromUID(theObject);
							((LPOBJECTRECORD)TEMPOBJECT::GetObject(theObject))->IncrementUseCount();
              }
						else
							ADBG_PRINT_E("GetOutputParameter: could not create a temporary object.");
            }
          }
        }
      }
    else
      {
      ADBG_PRINT_E("GetOutputParameter: parameters index is out of range.");
      }
    }
  else
    {
    ADBG_PRINT_E("GetOutputParameter: Function has no output parameters.");
    return FALSE;
    }

  return theObject;

	}

