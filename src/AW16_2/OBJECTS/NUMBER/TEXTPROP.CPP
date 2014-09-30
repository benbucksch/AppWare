//--------------------------------------------------------------------------
// TextProp.CPP
//
// DESCRIPTION:
//	   The Bindery ALM's object TEXT property manipulation functions.
//
// VAB FUNCTIONS IMPLEMENTED:
//    Get Text Property         ---> GetTextBindProp( )
//    Set Text Property         ---> SetTextBindProp( )
//
// POTENTIAL OBJECT APIs IMPLEMENTED:
//
// UTILITY FUNCTIONS IMPLEMENTED:
//
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <o_text.h>    // defines OTYPE_TEXT
#include <o_number.h>  // defines OTYPE_NUMBER
#include <o_flag.h>    // defines OTYPE_FLAG
#include <o_table.h>   // defines OTYPE_TABLE
#include <o_nwerr.h>   // defines OTYPE_NWERROR

#include <nwcalls.h>

#include "Bindery.h"
#include "nwdynlib.h"
#include "nwsyscon.h"

//--------------------------------------------------------------------------
// FUNCTION:    GetTextBindProp
//
// DESCRIPTION: Implements the 'Get Text Property' function
//    Reads the value(s) of a property from a Bindery object and outputs the values
//    to a Text object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values (MANDATORY)
//    2: Text object holding property name (MANDATORY)
//
// OUTPUT PARAMETERS
//    1: Text object to receive property value (MANDATORY)
//
// FLOWS
//    1: Retrieved
//    2: No such object
//    3: No such property
//    4: Wrong data type
//    5: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetTextBindProp( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiPropName;   // object ID of the Text object containing the property name
   int               retFlow = 5;  // assume 'Error' flow
   BinderyData far * pBinderyData; // pointer to a structure with default values
   NWCCODE           ccode;

   WORD              Type;
   DWORD             ID;
   char              OName[50], PName[50], resName[50];
   NWCONN_HANDLE     cHandle;
   unsigned char *   pcPropertyValue = NULL;

   int               currSegNum = 0;
   NWFLAGS           moreSeg, propFlag;
   BOOL              isSetValue = FALSE;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow; // return 'Error' flow if no network
      return 0;
   }

   oiBindery  = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiPropName = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiPropName != 0) )
   {
      pcPropertyValue = (unsigned char *)malloc( BIND_SEGMENT_SIZE );
      if ( pcPropertyValue != NULL )
      {
         pcPropertyValue[ 0 ] = OName[ 0 ] = PName[ 0 ] = '\0';

         pBinderyData = (BinderyData far *)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            OTextGetToBuffer( pBinderyData->ObjNameData, OName, 50 );
            OTextGetToBuffer( oiPropName, PName, 50 );
            cHandle = OBindGetConnHandle( oiBindery, TRUE );
            if ( cHandle != 0 )
            {
               BOOL  done; // are we done reading all segments

               if ( pBinderyData->ObjTypeData == 0 )
               {  // no 'Object Type' specified
                  Type = 0xFFFF; // look for all object types
               }
               else
               {  // object type was specified - see if valid
                  Type = 0;
                  Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
                  if ( Type == 0 )
                     Type = 0xFFFF; // look for all object types
               }
               AObjUnlockData( oiBindery, 0 );

               if ( Type == 0xFFFF )
               {  // we need a "real" object type (not a wildcard)
                  theSystem->message1 = 2; // flow = 'No such object'
                  return 0;
               }

               ccode = SUCCESSFUL;
               done  = FALSE;

               while ( (ccode == SUCCESSFUL) && (! done) )
               {
                  char *    currSegPtr;

                  if ( ++currSegNum > 1 )
                  {  // realloc storage area for data
                     pcPropertyValue = (char *)realloc( pcPropertyValue, BIND_SEGMENT_SIZE * currSegNum );
                     if ( pcPropertyValue == NULL )
                     {  // memory reallocation failed
                        theSystem->message1 = 5;
                        return 0;
                     }
                     currSegPtr = &(pcPropertyValue[ BIND_SEGMENT_SIZE * (currSegNum - 1) ]);
                  }
                  else
                     currSegPtr = pcPropertyValue;

                  ccode = nwfpNWReadPropertyValue( cHandle, OName, Type, PName, currSegNum, currSegPtr,
                                               &moreSeg, &propFlag );
                  if ( ccode != SUCCESSFUL )
                  {
                     if ( ccode == NO_SUCH_SEGMENT )
                     {  ccode = SUCCESSFUL;
                        done  = TRUE;
                     }

                     // ??? check if NO_SUCH_PROPERTY
                  }
                  else
                  {
                     if ( (currSegNum == 1) && (propFlag == BF_SET) )
                        isSetValue = TRUE;

                     if ( moreSeg == 0 )
                        done = TRUE;
                  }
               }

               if ( ccode == SUCCESSFUL )
               {
                  OBJECTID     oiOutText;

                  oiOutText = AObjCreate( OTYPE_TEXT );
                  if ( oiOutText != 0 )
                  {
                     if ( isSetValue == TRUE )
                     {  // property value is of SET type and contains a list of
                        // Bindery object IDs which need to be converted to Bindery
                        // object names
                        char         currObjectName[ 50 ];
                        char *       pObjectNames = NULL;
                        NWOBJ_ID *   pObjectIDs;
                        int          maxObjNum  = 0;
                        int          currObjNum = 0;

                        pObjectIDs = (NWOBJ_ID *)pcPropertyValue;

                        pObjectNames = (char *)malloc( 1 );
                        if ( pObjectNames != NULL )
                        {
                           // ??? if SET property, need to convert list of Bindery Object IDs
                           //     to Bindery object names by calling the API NWGetObjName( )

                           ccode = SUCCESSFUL;
                           pObjectNames[ 0 ] = '\0';
                           maxObjNum = (BIND_SEGMENT_SIZE * currSegNum) / sizeof(NWOBJ_ID);

                           while ( (currObjNum < maxObjNum) && (ccode == SUCCESSFUL) && (pObjectNames != NULL) )
                           {
                              ccode = nwfpNWGetObjectName( cHandle, pObjectIDs[currObjNum], currObjectName,
                                                       NULL );
                              if ( ccode == SUCCESSFUL )
                              {  // realloc storage for Bindery object names
                                 pObjectNames = (char *)realloc( pObjectNames, lstrlen(pObjectNames) + lstrlen(currObjectName) + 4 );
                                 if ( pObjectNames != NULL )
                                 {  // append newline if not first in list
                                    if ( ++currObjNum > 1 )
                                       lstrcat( pObjectNames, "\n" );
                                    // copy curr name to storage
                                    lstrcat( pObjectNames, currObjectName );
                                 }
                                 else
                                    ccode = INVALID_CONNECTION;
                              }
                              else if ( pObjectIDs[currObjNum] == 0 )
                              {  // assume that the remaining Bindery Object IDs are not valid
                                 currObjNum = maxObjNum;
                                 ccode = SUCCESSFUL;
                              }
                           }

                           if ( ccode == SUCCESSFUL )
                           {  // have the object name for the current item
                              OTextSetValueFromBufferEx( oiOutText, pObjectNames );
                              AFuncSetParameter( 1, oiOutText ); // Output this text object
                              retFlow = 1; // return 'Retrieved' flow
                           }

                           if ( pObjectNames != NULL )
                              free( pObjectNames );
                        }
                        // else - report 'Error' flow
                     }
                     else
                     {  // use Ex version of Text object API so CR/LF are handled properly

                        // ??? what about properties who's data is known not to be just ASCII
                        //     such as LOGIN_CONTROL and MISC_LOGIN_INFO
                        if ( (strcmpi(PName, "LOGIN_CONTROL") == 0) ||
                             (strcmpi(PName, "MISC_LOGIN_INFO") == 0) ||
									  (strcmpi(PName, "NET_ADDRESS") == 0) ||
									  (strcmpi(PName, "ACCOUNT_BALANCE") == 0) ||
                             (strcmpi(PName, TRUSTEE_DIR_STR ) == 0) ||
									  (strcmpi(PName, "USER_DEFAULTS") == 0)
                           )
                        {
                           OTextSetValueFromBufferEx( oiOutText,
                               "GET of this property not yet implemented !!!\n" );
                           AFuncSetParameter( 1, oiOutText ); // Output this text object
                           retFlow = 1; // return 'Retrieved' flow
                        }
                        else
                        {
                           OTextSetValueFromBufferEx( oiOutText, pcPropertyValue );
                           AFuncSetParameter( 1, oiOutText ); // Output this text object
                           retFlow = 1; // return 'Retrieved' flow
                        }
                     }
                  }
                  else
                  {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
                  }
               }
            }
         }
         if ( pcPropertyValue != NULL )
            free( pcPropertyValue );
      }
   }

   theSystem->message1 = retFlow;
   return 0;

} // end of GetTextBindProp( )

//--------------------------------------------------------------------------
// FUNCTION:    SetTextBindProp
//
// DESCRIPTION: Implements the 'Set Text Property' function
//    Write(s) the specified value(s) to the specified property to Bindery object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//    2: Text object holding property name
//    3: Text object holding the property value(s)
//    4: Flag object which, if set, causes any current values for the specified
//       property to be replaced by the new values.  If not set, the new values
//       are appended to the current values.
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Set
//    2: No such object
//    3: No such property
//    4: Wrong data type
//    5: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetTextBindProp( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiProp;
   int               retFlow = 5;  // assume 'Error' flow

   BinderyData far * pBinderyData; // pointer to a structure with default values
   WORD              Type;
   NWCCODE           ccode;
   DWORD             ID;
   char              OName[50], SName[50], resName[50];
   NWCONN_HANDLE     cHandle;
// int               flag;
   char *            Name    = NULL;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiProp    = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiProp != 0) )
   {
//    flag = 0;
      Name = (char *)malloc( 1 );
      if ( Name != NULL )
      {
         Name[ 0 ] = '\0';

         pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            // ??? rest of the implementation

            AObjUnlockData( oiBindery, 0 );
         }
      }
   }
   theSystem->message1 = retFlow;
   return 0;

} // end of SetBindTextProp( )
