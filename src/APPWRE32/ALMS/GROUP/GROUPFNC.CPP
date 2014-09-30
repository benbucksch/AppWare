////////////////////////////////////////////////////////////////////////
//File Name : GroupFnc.cpp
//			  Copyright 1989-92 Serius Corporation
//Author :	  Sibai Li
//Date :      June 1, 1992
//Description:
//			  Contains all serius function associated with the Serius Group Object
//        CountFunc
//  		  FindFunc
//			  GetFunc
//			  InsertFunc
//			  RemoveFunc
///////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <windows.h>
#include <memory.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_number.h>

#include "groupobj.h"

///////////////////////////////////////////////////////////////////////
//Function :  CountFunc
//Description:Count the total number of objects in the group object
//Input 1: 	  a group object
//Output 1:   a number Object
//Flow 1:     After count the objects
//Flow 2:     if an error occurs
///////////////////////////////////////////////////////////////////////
void ALMCALLBACK CountFunc(pAEvtInfo theEvent, pAObjMessage theSystem)
	{
	AOBJECTID	gObject ; 		// Object ID of Group Object.
	AOBJECTID  nObject ;     // Object ID of Number Object.
  BOOL      goodParams = TRUE ;       

	I_UNUSED(theEvent);

	
	if ((gObject=AFuncGetTypedParameter(1, OTYPE_GROUP)) == 0)
		{
		ADBG_PRINT_E("Count Object function: Error accessing Group parameter") ;
    goodParams = FALSE ;
		}

	if (goodParams)
  	{
		nObject = AObjCreate(OTYPE_NUMBER);
		if (nObject)
			{
			ONmbrSetInteger(nObject, AObjGetDataSize(gObject,0)/sizeof(AOBJECTID), FALSE) ;
			AFuncSetParameter (1, nObject) ;
			}
		else
			{
			ADBG_PRINT_E("Unable to create output parameter.") ;
      }
		AFuncReportResult(theSystem, 1, NULL) ;
		}
	}

///////////////////////////////////////////////////////////////////////
//Function:   FindFunc
//Description:find out a given object in the Group Object
//Input 1:    A Group Object.
//Input 2:    A selected object.
//OutPut 1:   A found object.
//Flow 1:     After found out a selected object.
//Flow 2:     if an error occurs.
///////////////////////////////////////////////////////////////////////
void ALMCALLBACK FindFunc(pAEvtInfo theEvent, pAObjMessage theSystem)
	{
	AOBJECTID   gObject ;        // Group Object ID
	AOBJECTID   tObject ;		    // A Selected Object ID
	AOBJECTID   nObject ;        // A Found Object ID
	LPOBJECTID gObjPtr ;        // A pointer to object IDs for objects participating
	BOOL       goodParams = TRUE ;     // Check Parameter
  BOOL       findOut = FALSE ;
	int        count ;  
	int        i ;              // A simple loop counter

	I_UNUSED(theEvent);

	gObject=AFuncGetTypedParameter(1,OTYPE_GROUP) ;
	if (!gObject)
		{				
		ADBG_PRINT_E("Find Object function: Cannot access Group parameter.") ;
    goodParams = FALSE ;
	 	}

	tObject=AFuncGetParameter(2);
	if (!tObject)
		{
		ADBG_PRINT_E("Find Object function: Cannot access Object parameter.") ;
		goodParams = FALSE ;
		}

	if (goodParams)
  	{
		gObjPtr = (LPOBJECTID ) AObjLockData(gObject, 0) ;
		if (gObjPtr)
    	{
	   	count = (int)AObjGetDataSize(gObject,0)/sizeof(AOBJECTID) ;
	 		for	(i=0; i<count; i++)
	 			{
		 		if (gObjPtr[i]==tObject)
		 			{
					nObject = AObjCreate(OTYPE_NUMBER) ;
					if (nObject != 0)
			  		{
						ONmbrSetInteger(nObject, i + 1, FALSE);
						AFuncSetParameter (1, nObject) ;
            }
					findOut = TRUE ;
          break ;
          }
				}
			AObjUnlockData(gObject, 0) ;
			if (findOut)
				AFuncReportResult (theSystem, 1, NULL) ;
			else
				AFuncReportResult (theSystem, 2, NULL) ;
			}
		else
      AFuncReportResult (theSystem, 2, NULL) ;
	  }
	}

///////////////////////////////////////////////////////////////////////
//Function:   GetFunc
//Description:Throught a given index to find out a object in the group object.
//Input 1:	  A Group Object
//Input 2:    A given Index
//Output 1:   A found object
//Flow 1:     After find out a object
//Flow 2:     if a error occurs
//////////////////////////////////////////////////////////////////////
void ALMCALLBACK GetFunc(pAEvtInfo theEvent, pAObjMessage theSystem)
	{
	AOBJECTID          gObject ;		//A Group Object ID
	AOBJECTID          nObject ;    //A Number Object ID that pass the given index
	LPOBJECTID        gObjPtr ;    //A pointer to object IDs for objects participating
	int               value ;      //The given index
  BOOL              goodParams = TRUE ;

	I_UNUSED(theEvent);

	gObject = AFuncGetTypedParameter (1,OTYPE_GROUP) ;
	nObject = AFuncGetTypedParameter (2,OTYPE_NUMBER) ;
	if (gObject == 0 || nObject == 0)
		{
		ADBG_PRINT_E ("Get Object function: Error accessing Group input parameter.") ;
		goodParams = FALSE ;
		}
	if (AObjGetType(nObject) != OTYPE_NUMBER)
		{
		ADBG_PRINT_E ("Get Object function: Index parameter must be a whole number ") ;
    goodParams = FALSE ;
		}

	if (goodParams)
  	{
		gObjPtr = (LPOBJECTID) AObjLockData(gObject,0) ;
		if (gObjPtr)
			{
			value = (int) ONmbrGetInteger(nObject) ;
			if (value > 0 && value <= AObjGetDataSize(gObject, 0)/sizeof(AOBJECTID))
      	{
				AFuncSetParameter(1, gObjPtr[value-1]) ;
				AFuncReportResult(theSystem, 1, NULL) ;
        }
	 		else
				AFuncReportResult(theSystem, 2, NULL) ;
			AObjUnlockData(gObject, 0);
			}
		else
			AFuncReportResult(theSystem, 2, NULL) ;
    }
	}

///////////////////////////////////////////////////////////////////////
//Function:   InsertFunc
//Description:Insert a object at the given index to the group object
//Input 1: 	  A Group Object
//Input 2:	  A inserted object
//Input 3:    The given index
//Flow 1:     After insert a object successfully
//Flow 2:     An error occurs
///////////////////////////////////////////////////////////////////////
void ALMCALLBACK InsertFunc(pAEvtInfo theEvent, pAObjMessage theSystem)
	{
	AOBJECTID          gObject ;		// A Group Object ID
	AOBJECTID          tObject ;        // A inserted Object ID
	AOBJECTID          nObject ;        // A Number Object passed the Index
	LPOBJECTID        gObjPtr ;        // A pointer to group object IDs
  BOOL              goodParams = TRUE ;
	int               value ;          // The given Index
	int               max ;            // The max number of group object
  int               i ;

	I_UNUSED(theEvent);

	 

	if ((gObject=AFuncGetTypedParameter (1,OTYPE_GROUP))==NULL)
		{
		ADBG_PRINT_E("Insert Object function: Error accessing Group input parameter.") ;
		goodParams = FALSE ;                                                                 
	  }

	if ((tObject=AFuncGetParameter (2))==NULL)
	  {
		ADBG_PRINT_E("Insert Object function: Error accessing Index input parameter.") ;
		goodParams = FALSE ;                                                               
	  }
	if (goodParams)
  	{
  	nObject = AFuncGetTypedParameter (3,OTYPE_NUMBER) ;
		if (nObject)
			value =(int) ONmbrGetInteger(nObject);
		else
	  	value = 32000; 
		max = (int) AObjGetDataSize(gObject,0)/sizeof(AOBJECTID) + 1 ;
		if(value > max) value = max ;
		if(value < 1)   value = max ; 
		if (AObjResizeData (gObject, 0, max*sizeof(AOBJECTID)) &&
				(gObjPtr = (LPOBJECTID) AObjLockData (gObject, 0))!= NULL)
  		{
	 		for (i=max-1; i>=value ; i--)
					gObjPtr[i] = gObjPtr[i-1] ;
	 		gObjPtr[value-1] = tObject ;
			AObjUnlockData(gObject, 0);
			AFuncReportResult(theSystem, 1, NULL) ;
			}
    else
			AFuncReportResult(theSystem, 2, NULL) ;
		}
	else
		AFuncReportResult(theSystem, 2, NULL) ;
    	
	}

///////////////////////////////////////////////////////////////////////
//Function:   RemoveFunc
//Description:Remove a object at the given index from the group object
//Input 1:    A Group Object
//Input 2:    A Number Object that passed the given index
//Flow 1:     After remove a object sucessufully
//Flow 2:     If an error occurs
///////////////////////////////////////////////////////////////////////
void ALMCALLBACK RemoveFunc(pAEvtInfo theEvent, pAObjMessage theSystem)
	{
	AOBJECTID          gObject ;		    // A Group Object ID
	AOBJECTID          tObject ;        // A Number Object ID
	LPOBJECTID        gObjPtr ;        //pointer to objetcIDs for objects participating
	int               value ;          //the given Index
  int               max ;            //the max number of objects in the group object
  int               i ;
  BOOL              goodParams = TRUE;

  I_UNUSED(theEvent);

  gObject = AFuncGetTypedParameter (1,OTYPE_GROUP) ;
  if(!gObject)
    {
    ADBG_PRINT_E("Remove Object function: Error accessing Group input parameter.");
	  goodParams = FALSE;
	  }

	tObject = AFuncGetTypedParameter (2,OTYPE_NUMBER) ;
	if (!tObject)
    {
    ADBG_PRINT_E("Remove Object function: Error accessing Index input parameter.") ;
	  goodParams = FALSE;
	  }

	if (goodParams)
    {
	  value =(int) ONmbrGetInteger(tObject);
	  max =(int) AObjGetDataSize(gObject,0)/sizeof(AOBJECTID) ;
	  if (value>max || value <1)
	 		{
			AFuncReportResult(theSystem, 2, NULL);
			}
		else
			{
			gObjPtr = (LPOBJECTID)AObjLockData(gObject,0) ;
			if (gObjPtr)
				{
		 	 	for (i=value; i<max ; i++)
				gObjPtr[i-1] = gObjPtr[i] ;
			 	if (AObjUnlockData(gObject, 0))
					AObjResizeData(gObject, 0, (max-1)*sizeof(AOBJECTID)) ;
				AFuncReportResult (theSystem, 1, NULL) ;
				}
	    }
		}
	}

