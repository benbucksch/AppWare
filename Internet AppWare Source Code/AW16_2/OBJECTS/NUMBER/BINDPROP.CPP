//--------------------------------------------------------------------------
// BindProp.CPP
//
// DESCRIPTION:
//	   The Bindery ALM's object property manipulation functions.
//
// VAB FUNCTIONS IMPLEMENTED: (x = currently implemented)
//    Change Property Security   --> ChangeBindPropSecurity( )
//    Clear Property             --> ClearBinderyProperty( )
//    Get Property Info          --> GetBinderyPropertyInfo( )
//    Get Date / Time Property   --> GetDateTimeBindProp( )
//    Get Net Addr Property      --> GetNetAddrBindProp( )
//    Get Time Property          --> GetTimeBindProp( )
//    Set Date / Time Property   --> SetDateTimeBindProp( )
//    Set Net Addr Property      --> SetNetAddrBindProp( )
//    Set Time Property          --> SetTimeBindProp( )
//  x Property Supports Type ?   --> DoesBindPropSupportDataType( )
//  x List Properties            --> ListProperties( )
//  x Create Property            --> CreateProperty( )
//  x Delete Property            --> DeleteProperty( )
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


//=======================================================================================
//
// Prototype Info for Property Look-Up / Hash Table
//
// Data Structure For Each Property
//      char *  bindPropName;
//      char *  localPropName;
//      BYTE    dataTypeSupported;         bit for each data type possible
//                                         also one for SET vs ITEM
//                  #define OBIND_VABDT_TEXT
//                  #define OBIND_VABDT_TABLE
//                  #define OBIND_VABDT_NUMBER
//                  #define OBIND_VABDT_DATE_TIME
//                  #define OBIND_VABDT_TIME
//                  #define OBIND_VABDT_NET_ADDR
//                  #define OBIND_VABDT_IS_FIELD
//      BYTE    defaultRdSecurity;
//      BYTE    defaultWrtSecurity;
//      BYTE    nativeDataType;            need to contain info about byte ordering
//                  #define OBIND_NATDT_TEXT
//                  #define OBIND_NATDT_BYTE
//                  #define OBIND_NATDT_WORD_LO
//                  #define OBIND_NATDT_WORD_HI
//                  #define OBIND_NATDT_LONG_LO
//                  #define OBIND_NATDT_LONG_HI
//                  #define OBIND_NATDT_OBJ_REF
//      BYTE    segmentNumber;
//      BYTE    startOffsetInSegment;
//      BYTE    endOffsetInSegment;
//
//=======================================================================================

//--------------------------------------------------------------------------
LONG FAR PASCAL _export ChangeBindPropSecurity( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Changed
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of ChangeBindPropSecurity( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export ClearBinderyProperty( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Cleared
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of ClearBinderyProperty( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetBinderyPropertyInfo( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Retrieved
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of GetBinderyPropertyInfo( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetDateTimeBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Retrieved
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of GetDateTimeBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetNetAddrBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Retrieved
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of GetNetAddrBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetNumberBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Retrieved
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of GetNumberBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetTimeBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Retrieved
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of GetTimeBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetDateTimeBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Set
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of SetDateTimeBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetNetAddrBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Set
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of SetNetAddrBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetNumberBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Set
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of SetNumberBindProp( )
//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetTimeBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Set
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
} // end of SetTimeBindProp( )

//--------------------------------------------------------------------------
// FUNCTION:    DoesBindPropSupportDataType
//
// DESCRIPTION: Implements the 'Property Supports Type' function
//
//    Determines if the specified property of a Bindery object can be output to the
//    specified data type
//
// INPUT PARAMETERS
//    1: Bindery object holding input values (MANDATORY)
//    2: Text object holding property name (MANDATORY)
//    3: Text object holding property type (MANDATORY)
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Supported
//    2: Not supported
//    3: Unknown
//    4: Invalid property
//    5: No such object
//    6: No such property
//    7: Error
//
// DEVELOPMENT STATUS
//    * hard-coded knowledge should be removed
//    * SET properties should have support TEXT and TABLE
//--------------------------------------------------------------------------
LONG FAR PASCAL _export DoesBindPropSupportDataType( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiPropName;   // object ID of the Text object containing the property name
   OBJECTID          oiPropType;   // object ID of the Text object containing the property type
   int               retFlow = 7;  // assume 'Error' flow

   oiBindery  = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiPropName = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   oiPropType = AFuncGetTypedParameter( 3, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiPropName != 0) && (oiPropType != 0) )
   {
      char               propName[ 50 ];
      char               propType[ 81 ];
      BinderyData far *  pBinderyData;

      propName[ 0 ] = '\0';
      OTextGetToBuffer( oiPropName, propName, sizeof(propName)-1 );
      propType[ 0 ] = '\0';
      OTextGetToBuffer( oiPropType, propType, sizeof(propType)-1 );

      if ( (propName[ 0 ] != '\0') && (propType[ 0 ] != '\0') )
      {
         int    objType;

         pBinderyData = (BinderyData far *)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            if ( pBinderyData->ObjTypeData == 0 )
            {  // no 'Object Type' specified
               objType = 0xFFFF; // look for all object types
            }
            else
            {  // object type was specified - see if valid
               objType = 0;
               objType = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
               if ( objType == 0 )
                  objType = 0xFFFF; // look for all object types
            }
            AObjUnlockData( oiBindery, 0 );
         }

         if ( strcmpi( propType, "Table" ) == 0 )
         {
            // ??? if a SET property - return 'Supported' flow

            if ( strcmpi( propName, "NET_ADDRESS" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, "LOGIN_CONTROL" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, "USER_DEFAULTS" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, "GROUPS_I'M_IN" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, "SECURITY_EQUALS" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, "GROUP_MEMBERS" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else if ( strcmpi( propName, TRUSTEE_DIR_STR ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else
               retFlow = 2; // flow = 'Not supported'
         }
         else if ( strcmpi( propType, "Text" ) == 0 )
         {
            retFlow = 1; // flow = 'Supported'
         }
         else if ( (strcmpi( propType, "Date / Time" ) == 0) ||
                   (strcmpi( propType, "Date and Time" ) == 0) )
         {
            // ??? if a SET property - return 'Not supported' flow

//          if ( strcmpi( propName, "NET_ADDRESS" ) == 0 )
//             retFlow = 1; // flow = 'Supported'
//          else
               retFlow = 2; // flow = 'Not supported'
         }
         else if ( strcmpi( propType, "Time" ) == 0 )
         {
            // ??? if a SET property - return 'Not supported' flow

//          if ( strcmpi( propName, "NET_ADDRESS" ) == 0 )
//             retFlow = 1; // flow = 'Supported'
//          else
               retFlow = 2; // flow = 'Not supported'
         }
         else if ( (strcmpi( propType, "Net Addr" ) == 0) ||
                   (strcmpi( propType, "Network Address" ) == 0) )
         {
            // ??? if a SET property - return 'Not supported' flow

            if ( strcmpi( propName, "NET_ADDRESS" ) == 0 )
               retFlow = 1; // flow = 'Supported'
            else
               retFlow = 2; // flow = 'Not supported'
         }
      }
      else
         retFlow = 4; // flow = 'Invalid property'

   } // end of IF input parameters are valid

   theSystem->message1 = retFlow;
   return 0;

} // end of DoesBindPropSupportDataType( )

//--------------------------------------------------------------------------
// FUNCTION:  ListProperties
//
// DESCRIPTION:  Outputs names of properties matching search criteria to
//		output 1.
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//	   2: Text object containing the name of the property to search for.
//	   3: Flag object which, if set, means that this function may change
//       the number of columns in the output Table object 
//
// OUTPUT PARAMETERS
//	   1: Text object to receive list of matching properties associated with the
//       object specified via the NW Bindery object data (MANDATORY)
//
//       NOTE: this needs to be changed to a TABLE object
//
//    2: Number object which will receive the count of the number of properties
//       listed (OPTIONAL)
//
// FLOWS
//    1: Listed
//    2: Invalid object name
//    3: Invalid object type
//	   4: No such object
//    5: Error
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
LONG FAR PASCAL _export ListProperties (pAEvtInfo, pAObjMessage theSystem)
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiOutText;
   OBJECTID          oiProp;
   BinderyData far * pBinderyData; // pointer to a structure with default values
   WORD              Type;
   NWCCODE           ccode;
   DWORD             ID;
   char              OName[50], SName[50], resName[50];
   NWCONN_HANDLE     cHandle;
   int               flag;
   char *            Name    = NULL;
   int               retFlow = 5; // assume 'Error' flow
   int               propertyCount = 0;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   flag = 0;
   Name = (char *)malloc( 1 );
   if ( Name != NULL )
   {
      Name[ 0 ] = '\0';

      oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
      oiProp    = AFuncGetTypedParameter( 2, OTYPE_TEXT );
      if ( (oiBindery != 0) && (oiProp != 0) )
      {
         pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            // Get values and make call

            OTextGetToBuffer( pBinderyData->ObjNameData, OName, 50 );
            OTextGetToBuffer( oiProp, SName, 50 );
            // get connection handle based on server name
            cHandle = OBindGetConnHandle( oiBindery, TRUE );

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

            if ( Type == 0xFFFF )
            {  // we need a "real" object type (not a wildcard)
               theSystem->message1 = 3; // flow = 'Invalid object type'
               return 0;
            }

            ID = -1;

            AObjUnlockData( oiBindery, 0 );

            ccode = SUCCESSFUL;

            while ( ccode == SUCCESSFUL )
            {
               NWFLAGS     propertyFlags;
               NWFLAGS     propertySecurity;
               NWFLAGS     hasValueFlags;

               ccode = nwfpNWScanProperty ( cHandle, OName, Type, SName, &ID, resName,
                                            &propertyFlags, &propertySecurity,
                                            &hasValueFlags, NULL );
               if ( ccode == SUCCESSFUL )
               {
                  char tempStr[ 256 ];

//                tempStr[0] = '\0';
                  lstrcpy( tempStr, resName );

                  // display the static / dynamic string
                  lstrcat( tempStr, "\t" );
                  if ( propertyFlags & BF_DYNAMIC )
                     lstrcat( tempStr, "Dynamic" );
                  else
                     lstrcat( tempStr, "Static" );

                  // display the SET / ITEM string
                  lstrcat( tempStr, "; " );
                  if ( propertyFlags & BF_SET )
                     lstrcat( tempStr, "Set" );
                  else
                     lstrcat( tempStr, "Item" );

                  // display the static / dynamic string
                  if ( hasValueFlags != 0 )
                     lstrcat( tempStr, "; Has Value(s)" );

                  Name = (char *) realloc( Name, lstrlen(Name) + lstrlen(tempStr) + 4 );
                  if (Name == NULL)
                  {  // memory reallocation failed
                     theSystem->message1 = 3;
                     return 0;
                  }
                  if ( ++propertyCount > 1 )
                     lstrcat( Name, "\n" );
                  lstrcat( Name, tempStr );
               }
               flag = 1; // return 'Listed' flow
            }

            
            if ( (Type == OT_USER) || (Type == OT_USER_GROUP) )
            {  // ??? if type is OT_USER or OT_GROUP then add 'Trustee Directory Assignments'
               char *newPropStr = TRUSTEE_DIR_STR;
               Name = (char *) realloc( Name, lstrlen(Name) + lstrlen(newPropStr) + 2 );
               if (Name == NULL)
               {  // memory reallocation failed
                  theSystem->message1 = 3;
                  return 0;
               }
               if ( ++propertyCount > 1 )
                  lstrcat( Name, "\n" );
               lstrcat( Name, newPropStr );
            }

            oiOutText = AObjCreate( OTYPE_TEXT );
            if ( oiOutText != 0 )
            {  // use Ex version of Text object API so CR/LF are handled properly
               OTextSetValueFromBufferEx( oiOutText, Name );
               AFuncSetParameter( 1, oiOutText ); // Output this text object

               if ( flag == 0 )
                  retFlow = 4; // return 'No such object' flow
               else
                  retFlow = 1;
            }
            else
            {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
            }

            // ??? output the count

         } // end of IF AobjLockData was successful

      } // end of IF input parameters are valid

   } // end of IF memory allocation were successful

   if ( Name != NULL )
      free( Name );

   theSystem->message1 = retFlow;
   return 0;

} // end of ListProperties( )

//--------------------------------------------------------------------------
// FUNCTION:  CreateProperty
//
// DESCRIPTION:  Creates a property
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//	   2: Text object holding property name
//    3: Dynamic Flag   - Flag object, if set means dynamic property  (OPTIONAL)
//    4: Read Security  - Text object (OPTIONAL)
//    5: Write Security - Text object (OPTIONAL)
//    6: Set Flag - Flag object, if set means SET (list of Bindery Obj IDS) property (OPTIONAL)
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Property created
//    2: Property already exists
//	   3: No Property Create Privilege
//    4: No such object
//	   5: Error
//
// DEVELOPMENT STATUS
//    * the 'Read Security' and 'Write Security' input parameters are not yet
//      implemented
//--------------------------------------------------------------------------
LONG FAR PASCAL _export CreateProperty( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID            oiBindery;     // the ID of the Bindery Object
   BinderyData far *   pBinderyData;  // pointer to a structure with default values
   OBJECTID            oiProp;
   WORD                Type;
   NWCCODE             ccode;
   char                Name[50], PName[50];
   NWCONN_HANDLE       cHandle;
   NWFLAGS             oflags, sflags;
   int                 retFlow = 5;   // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiProp    = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiProp != 0) )
   {  // the input parameter Object IDs are valid
      pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      if (pBinderyData != NULL)
      {  // we were able to access the data in data block zero
         OTextGetToBuffer( oiProp, PName, 50 );
         // get the Bindery object type by converting the ASCII name
         // contained in the NW Bindery object
         Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
         if ( Type != 0 )
         {  // the determined Bindery object type is valid
            OBJECTID    oiIsDynamicFlag;
//          BOOL        isDynamicBool = FALSE;
            OBJECTID    oiIsSetPropFlag;
//          BOOL        isSetPropBool = FALSE;

            OTextGetToBuffer (pBinderyData->ObjNameData, Name, 48);
            // get connection handle based on server name
            cHandle = OBindGetConnHandle( oiBindery, TRUE );
            // check if 'Is Dynamic' flag is specified and is set
            oflags = BF_STATIC;
            oiIsDynamicFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
            if ( oiIsDynamicFlag != 0 )
            {
               if ( OFlagGet(oiIsDynamicFlag) == TRUE )
               {
                  oflags = BF_DYNAMIC;
//                isDynamicBool = TRUE;
               }
            }
            // check if 'Is Set Property' flag is specified and is set
            oiIsSetPropFlag = AFuncGetTypedParameter( 6, OTYPE_FLAG );
            if ( oiIsSetPropFlag != 0 )
            {
               if ( OFlagGet(oiIsSetPropFlag) == TRUE )
               {
                  oflags |= BF_SET;
//                isSetPropBool = TRUE;
               }
               else
                  oflags |= BF_ITEM;
            }
            else
               oflags |= BF_ITEM;

            // ??? need to get Read and Write security from parameters ???

            sflags = BS_OBJECT_READ | BS_SUPER_WRITE;

            ccode = nwfpNWCreateProperty( cHandle, Name, Type, PName, oflags, sflags );
            switch( ccode )
            {
         	   case SUCCESSFUL:
                  retFlow = 1;
                  break;
               case OBJECT_ALREADY_EXISTS:
                  retFlow = 2;
                  break;
               case NO_OBJECT_CREATE_PRIVILEGE:
                  retFlow = 3;
                  break;
//             case INVALID_CONNECTION:
               default:
                  retFlow = 5; // error
                  break;
            }

            // ??? if creating a new property of type SET we should write
            //     a 128 byte segment to the Bindery which is all zeros

         } // end of IF object type is determined
         AObjUnlockData( oiBindery, 0 );
      }
      else
      {  // call to AObjLockData( ) failed
         theSystem->message1 = 5 ; // follow flow #5 (ERROR) and return
         return 0;
      }
   }  // end of IF input parameter object IDs are valid

   theSystem->message1 = retFlow; // return desired flow

   return 0;

} // end of CreateProperty( )

//--------------------------------------------------------------------------
// FUNCTION:  DeleteProperty
//
// DESCRIPTION:  Deletes a property
//
// INPUT PARAMETERS
//    1: Bindery object holding input values (MANDATORY)
//	   2: Text object holding property name (MANDATORY)
//    3: Flag object, it set means display confirm delete dialog (OPTIONAL)
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Deleted
//    2: No such property
//	   3: No such object
//    4: No rights
//	   5: Error
//    6: Canceled
//
// DEVELOPMENT STATUS
//
//--------------------------------------------------------------------------
LONG FAR PASCAL _export DeleteProperty( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID            oiBindery;     // the ID of the Bindery Object
   BinderyData far *   pBinderyData;  // pointer to a structure with default values
   OBJECTID            oiProp;
   WORD                Type;
   NWCCODE             ccode;
   char                Name[50], PName[50];
   NWCONN_HANDLE       cHandle;
   int                 retFlow = 5; // assume 'Error' flow
   BOOL                confirmOperation = FALSE;
   OBJECTID            oiConfirmFlag;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiProp    = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiProp != 0) )
   {
      // get the value of the Flag input object (Confirmation Dlg)
      oiConfirmFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
      if ( oiConfirmFlag != 0 )
      {
         confirmOperation = OFlagGet( oiConfirmFlag );
      }

      if ( confirmOperation == TRUE )
      {  // display a YES / NO message box before proceeding
         int   msgRet;
   
         msgRet = MessageBox( GetActiveWindow( ), "Proceed With Deletion Of Bindery Object Property",
                              "Delete NetWare Bindery Object Property",
                              MB_ICONQUESTION | MB_YESNO );
         if ( msgRet != IDYES )
         {
            theSystem->message1 = 6; // return 'Canceled' flow
            return 0;
         }
      }

      pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      if ( pBinderyData != NULL )
      {
         OTextGetToBuffer( oiProp, PName, 50 );

         Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
         if ( Type != 0 )
         {
            OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );

            // get connection handle, based on info in object datas
            cHandle = OBindGetConnHandle( oiBindery, TRUE );

            ccode = nwfpNWDeleteProperty( cHandle, Name, Type, PName );
            switch( ccode )
            {
               case SUCCESSFUL:
                  retFlow = 1; // flow = 'Deleted'
                  break;
               case NO_SUCH_PROPERTY:
                  retFlow = 2; // flow = 'No such property'
                  break;
               case NO_SUCH_OBJECT:
                  retFlow = 3; // flow = 'No such object'
                  break;
//             case INVALID_CONNECTION:
               default:
                  retFlow = 5; // flow = 'Error'
                  break;
            } // end of SWITCH completion code
         } // end of IF object type was able to be determined
         AObjUnlockData( oiBindery, 0 );

      } // end of IF able to access NW Bindery object's data block zero
   } // end IF input parameter object IDs are valid

	theSystem->message1 = retFlow;
	return 0;

} // end of DeleteProperty( )
