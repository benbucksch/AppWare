//--------------------------------------------------------------------------
// BindObj.CPP
//
// DESCRIPTION:
//    The Bindery ALM's object manipulation functions.
//
// VAB FUNCTIONS IMPLEMENTED:
//    Change Object Security  --> ChangeBindObjSecurity( )
//    Get Object Info         --> GetBinderyObjectInfo( )
//    Does Object Exist ?     --> DoesBinderyObjectExist( )
//    List Object Types       --> ListBinderyObjectTypes( )
//    List Objects            --> ListBinderyObjects( )
//    Create Object           --> CreateObject( )
//    Delete Object           --> MyDeleteObject( )
//    Rename Object           --> RenameObject( )
//
// POTENTIAL OBJECT APIs IMPLEMENTED:
//
// UTILITY ROUTINES IMPLEMENTED:
//    ConvertBindObjTypeTextToID( )
//    ConvertBindObjTypeStrToID( )
//    ConvertBindObjTypeIDToStr( )
//    InitNewUserObject( )
//    InitNewPrtQueueObject( )
//    InitNewPrtServerObject( )
//
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <o_text.h>   // defines OTYPE_TEXT
#include <o_number.h> // defines OTYPE_NUMBER
#include <o_flag.h>   // defines OTYPE_FLAG

#include <nwcalls.h>

#include "Bindery.h"
#include "nwdynlib.h"

int InitNewUserObject(      NWCONN_HANDLE connHandle, char *objectName,
                            BOOL createHomeDir );
int InitNewPrtQueueObject(  NWCONN_HANDLE connHandle, char *objectName );
int InitNewPrtServerObject( NWCONN_HANDLE connHandle, char *objectName );



typedef struct
{
   unsigned int   fObjTypeID;        // Bindery object type ID
   unsigned char  fDefaultSecurity;
   unsigned char  fIsExtendedType;   // if = 1 then type is extended
   char far *     fObjTypeStr;       // local name
} BindObjType;

const BindObjType objTypeData[] = {
   { OT_WILD,                        0x00, 0, "All" },
// { OT_UNKNOWN,                     0x00, 0, "Unknown" },
   { OT_USER,                        0x00, 0, "User" },
   { OT_USER_GROUP,                  0x00, 0, "User Group" },
   { OT_PRINT_QUEUE,                 0x00, 0, "Print Queue" },
   { OT_FILE_SERVER,                 0x00, 0, "File Server" },
   { OT_JOB_SERVER,                  0x00, 0, "Job Server" },
   { OT_GATEWAY,                     0x00, 0, "Gateway" },
   { OT_PRINT_SERVER,                0x00, 0, "Print Server" },
   { OT_ARCHIVE_QUEUE,               0x00, 0, "Archive Queue" },
   { OT_ARCHIVE_SERVER,              0x00, 0, "Archive Server" },
   { OT_JOB_QUEUE,                   0x00, 0, "Job Queue" },
   { OT_ADMINISTRATION,              0x00, 0, "Administration" },
   { OT_NAS_SNA_GATEWAY,             0x00, 0, "SNA Gateway" },
   { OT_REMOTE_BRIDGE_SERVER,        0x00, 0, "Remote Bridge" },
   { OT_TIME_SYNCHRONIZATION_SERVER, 0x00, 1, "Time Synch Server" },
   { OT_ARCHIVE_SERVER_DYNAMIC_SAP,  0x00, 1, "Archive Dynamic SAP" },
   { OT_ADVERTISING_PRINT_SERVER,    0x00, 1, "Advertising Print Server" },
   { OT_BTRIEVE_VAP,                 0x00, 1, "Btrieve VAP" },
   { OT_PRINT_QUEUE_USER,            0x00, 1, "Print Queue User" },
   { 0,                              0x00, 0, NULL }
};

//--------------------------------------------------------------------------
int ConvertBindObjTypeTextToID( OBJECTID oiObjType )
//--------------------------------------------------------------------------
{
   char    typeNameStr[ 50 ];
   int     retCode = 0;

   if ( oiObjType != 0 )
   {  // call Text object API to extract value from object
      typeNameStr[0] = '\0';
      OTextGetToBuffer( oiObjType, typeNameStr, sizeof(typeNameStr) );
      if ( typeNameStr[0] != '\0' )
         retCode = ConvertBindObjTypeStrToID( typeNameStr );
   }
   return retCode;

} // end of ConvertBindObjTypeTextToID( )

//--------------------------------------------------------------------------
int ConvertBindObjTypeStrToID( char *objTypeStr )
//--------------------------------------------------------------------------
{
   int   currIndex = 0;
   int   retCode   = 0;

   if ( objTypeStr != NULL )
   {
      while ( (objTypeData[currIndex].fObjTypeStr != NULL) && (retCode == 0) )
      {  // perform a case insensitive string compare with the current object type
         if ( strcmpi( objTypeStr, objTypeData[currIndex].fObjTypeStr ) == 0 )
         {  // we found a matching object type string - return the value
            retCode = objTypeData[currIndex].fObjTypeID;
         }
         else
            ++currIndex;
      }
   }
   return retCode;

} // end of ConvertBindObjTypeStrToID( )

//--------------------------------------------------------------------------
const char far * ConvertBindObjTypeIDToStr( int objectID )
//--------------------------------------------------------------------------
{
   int                 currIndex = 0;
   const char far *    retValue = NULL;

   if ( objectID != 0 )
   {
      while ( (objTypeData[currIndex].fObjTypeStr != NULL) && (retValue == NULL) )
      {  // see if the current ID is the same as the ID we are looking for
         if ( objTypeData[currIndex].fObjTypeID == objectID )
         {  // we found a matching object type ID - return a ptr to the
            // corresponding string
            retValue = objTypeData[currIndex].fObjTypeStr;
         }
         else
            ++currIndex;
      }
   }
   return retValue;

} // end of ConvertBindObjTypeIDToStr(  )

//--------------------------------------------------------------------------
LONG FAR PASCAL _export ChangeBindObjSecurity( pAEvtInfo, pAObjMessage theSystem )
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
}
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetBinderyObjectInfo( pAEvtInfo, pAObjMessage theSystem )
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
}
//--------------------------------------------------------------------------
LONG FAR PASCAL _export DoesBinderyObjectExist( pAEvtInfo, pAObjMessage theSystem )
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
}

#pragma argsused
//--------------------------------------------------------------------------
// FUNCTION:  ListBinderyObjectTypes
//
// DESCRIPTION:  Outputs names of possible Bindery object types.
//
// INPUT 1:   Bindery object holding input values  (NOT REALLY USED !!!)
//
//   ??? could possibly use a second input - basic/extended flag ???
//
//   ??? should VAP related items only be returned if server is v2.x ???
//
// OUTPUT 1:  Text object
//
// FLOW 1: Listed
// FLOW 2: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export ListBinderyObjectTypes( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID         oiBindery;    // the ID of the Bindery Object
   OBJECTID         oiOutText;
   char *           pObjTypeList;
   int              currIndex = 0;
   int              itemCount = 0;
   int              retFlow = 1;  // assume 'Listed' flow

   pObjTypeList = (char *) malloc( 1 );
   if ( pObjTypeList != NULL )
   {
      pObjTypeList[ 0 ] = '\0';

      while ( (objTypeData[currIndex].fObjTypeStr != NULL) && (retFlow == 1) )
      {
         pObjTypeList = (char *)realloc( pObjTypeList, lstrlen(pObjTypeList) +
                             lstrlen(objTypeData[currIndex].fObjTypeStr) + 4 );
         if ( pObjTypeList == NULL )
         {
            retFlow = 2;
         }
         else
         {
            if ( ++itemCount > 1 )
               lstrcat( pObjTypeList, "\n" );
            lstrcat( pObjTypeList, objTypeData[currIndex].fObjTypeStr );
            ++currIndex;
         }
      }
   }

   if ( retFlow == 1 )
   {
      oiOutText = AObjCreate( OTYPE_TEXT );
      if ( oiOutText != 0 )
      {  // use Ex version of Text object API so CR/LF are handled properly
         OTextSetValueFromBufferEx( oiOutText, pObjTypeList );
         AFuncSetParameter( 1, oiOutText ); // Output this text object
      }
      else
      {  // creation of temporary object failed !!
         retFlow = 2;
#ifdef ADBG_DEBUG
#endif
      }
   }

   if ( pObjTypeList != NULL )
      free( pObjTypeList );

   theSystem->message1 = retFlow;
   return 0;

} // end of ListBinderyObjectTypes( )

//--------------------------------------------------------------------------
// FUNCTION:  ListBinderyObjects
//
// DESCRIPTION:  Outputs names of objects matching search criteria to
//		output 1.
//
// INPUT 1: Bindery object holding input values
//
//	OUTPUT 1:	Text object
//
// FLOW 1: Listed
//	FLOW 2: No such object  
// FLOW 3: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export ListBinderyObjects( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID         oiBindery;    // the ID of the Bindery Object
   OBJECTID         oiOutText;
   BinderyData far* pBinderyData; // pointer to a structure with default values
   WORD             Type;
   NWCCODE          ccode;
   DWORD            ID;
   char             SearchName[50], resName[50];
   NWCONN_HANDLE    cHandle;
   int              flag;
   char *           Name = NULL;
   int              retFlow = 3; // assume 'Error' flow
   int              objectCount = 0;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   flag = 0;
   Name = (char *) malloc( 1 );
   if ( Name != NULL )
   {
      Name[ 0 ] = '\0';

      oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
      if ( oiBindery != 0 )
      {
         pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            // Get values and make call
            SearchName[ 0 ] = '\0';
            OTextGetToBuffer( pBinderyData->ObjNameFilter, SearchName, 50 );
            if ( SearchName[ 0 ] == '\0' )
              strcpy( SearchName, "*" );   // use wildcard if search name not specified

            cHandle = OBindGetConnHandle( oiBindery, TRUE );
            if ( cHandle != 0 )
            {
               if ( pBinderyData->ObjTypeFilter == 0 )
               {  // no 'Object Type' specified
                  Type = 0xFFFF; // look for all object types
               }
               else
               {  // object type was specified - see if valid
                  Type = 0;
                  Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeFilter );
                  if ( Type == 0 )
                     Type = 0xFFFF; // look for all object types
               }

               ID = -1;

               AObjUnlockData( oiBindery, 0 );

               ccode = SUCCESSFUL;

               while ( ccode == SUCCESSFUL )
               {
                  NWOBJ_TYPE    thisObjType;
                  NWFLAGS       hasPropsFlag;
                  NWFLAGS       objectFlags;
                  NWFLAGS       objSecurityFlags;

                  ccode = nwfpNWScanObject ( cHandle, SearchName, Type, &ID, resName, &thisObjType,
                                             &hasPropsFlag, &objectFlags, &objSecurityFlags);
                  if ( ccode == SUCCESSFUL )
                  {
                     char   tempStr[ 256 ];

                     // copy current object name to temp string
                     lstrcpy( tempStr, resName );

                     // concatenate object type string
                     lstrcat( tempStr, "\t" );
                     // !!! call ID to Str conversion routine insted !!!
                     switch ( thisObjType )
                     {
                        case OT_USER:
                           lstrcat( tempStr, "User" );
                           break;
                        case OT_USER_GROUP:
                           lstrcat( tempStr, "User Group" );
                           break;
                        case OT_PRINT_QUEUE:
                           lstrcat( tempStr, "Print Queue" );
                           break;
                        case OT_FILE_SERVER:
                           lstrcat( tempStr, "File Server" );
                           break;
                        case OT_PRINT_SERVER:
                           lstrcat( tempStr, "Print Server" );
                           break;
                        case OT_JOB_QUEUE:
									lstrcat( tempStr, "Job Queue" );
									break;
								case OT_ADVERTISING_PRINT_SERVER:
                           lstrcat( tempStr, "Advertising Print Server" );
									break;
								default:
                           {
                              char  objTypeStr[ 10 ];
                              wsprintf( objTypeStr, "0x%08x", thisObjType );
                              lstrcat( tempStr, objTypeStr );
                           }
                           break;
                     }

                     // concatenate object flags string
                     lstrcat( tempStr, "\t" );
                     if ( objectFlags & BF_DYNAMIC )
                        lstrcat( tempStr, "Dynamic" );
                     else
                        lstrcat( tempStr, "Static" );

                     // concatenate 'has properties' string
                     if ( hasPropsFlag != 0 )
                        lstrcat( tempStr, "; Has Properties" );

                     // ??? concatenate object security string ???
//                   lstrcat( tempStr, "\t" );

                     // resize destination memory
                     Name = (char *)realloc( Name, lstrlen(Name) + lstrlen(tempStr) + 4 );
                     if ( Name == NULL )
                     {
                        theSystem->message1 = 3;
                        return 0;
                     }
                     if ( ++objectCount > 1 )
                        lstrcat( Name, "\n" );
                     lstrcat( Name, tempStr );

                  }
                  flag = 1;

               } // end of WHILE ccode == SUCCESSFUL

               oiOutText = AObjCreate( OTYPE_TEXT );
               if ( oiOutText != 0 )
               {  // use Ex version of Text object API so CR/LF are handled properly
                  OTextSetValueFromBufferEx( oiOutText, Name );
                  AFuncSetParameter( 1, oiOutText ); // Output this text object
               }
               else
               {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
               }

               if ( flag == 0 )
               {
                  theSystem->message1 = 2;
                  return 0;
               }
               retFlow = 1; // follow the 'Listed' flow

            } // end of IF connection handle is valid (non-zero)

         } // end of IF AObjLockData( ) was successful

      } // end of IF Bindery object ID is valid

   }  // end of IF memory alloc successful

   if ( Name != NULL )
      free( Name );
   theSystem->message1 = retFlow;
   return 0;

} // end of ListBinderyObjects( )

//--------------------------------------------------------------------------
// FUNCTION:  CreateObject
//
// DESCRIPTION:  Creates a bindery object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values (REQ'D)
//    2: Flag object which, if set, means object is dynamic (OPTIONAL)
//    3: Text object containing Read security value (OPTIONAL)
//    4: Text object containing Write security value (OPTIONAL)
//    5: Flag object which, if set, means a HOME DIRECTORY should be created (OPTIONAL)
//    6: Flag object which, if set, will result in a YES / NO confirmation dlg to appear (OPTIONAL)
//
// OUTPUT PARAMETERS
//    None (could return new Bindery object ID)
//
// FLOWS
//    1: Created
//    2: Object exists
//	   3: No (object create) rights   (must be SUPERVISOR or equiv.)
//	   4: Invalid obj name
//	   5: Invalid obj type
//	   6: Error
//    7: Canceled
//
// STATUS
//    * parameter 5 is currently being ignored
//--------------------------------------------------------------------------
LONG FAR PASCAL _export CreateObject( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far *  pBinderyData; // pointer to a structure with default values
   WORD               Type;
   NWCCODE            ccode;
   char               Name[50];
   NWCONN_HANDLE      cHandle;
   NWFLAGS            oflags, sflags;
   int                retFlow = 6; // assume 'Error' flow
   OBJECTID           oiIsDynamicFlag;
// BOOL               isDynamicBool = FALSE;
   BOOL               confirmOperation = FALSE;
   OBJECTID           oiConfirmFlag;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

   oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
	if ( pBinderyData == NULL )
	{
		theSystem->message1 = 6; // follow 'Error' and return
		return 0;
	}

// Get values and make call

   Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
   if ( Type == 0 )
   {  // need to return error - object type not known
      AObjUnlockData( oiBindery, 0 );
		theSystem->message1 = 5; // follow 'Invalid obj type' and return
		return 0;
   }

   Name[ 0 ] = '\0';
	OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );
   if ( (pBinderyData->ObjNameData == 0) || (Name[0] == '\0') )
   {  // need to return error - object name not specified or bad fmt
      AObjUnlockData( oiBindery, 0 );
		theSystem->message1 = 4; // follow 'Invalid obj name' and return
		return 0;
   }
   AObjUnlockData( oiBindery, 0 );

   // get the value of the Flag input object (Confirmation Dlg)
   oiConfirmFlag = AFuncGetTypedParameter( 6, OTYPE_FLAG );
   if ( oiConfirmFlag != 0 )
   {
      confirmOperation = OFlagGet( oiConfirmFlag );
   }

   if ( confirmOperation == TRUE )
   {  // display a YES / NO message box before proceeding
      int   msgRet;

      msgRet = MessageBox( GetActiveWindow( ), "Proceed With Creation of New Bindery Object",
                           "Create NetWare Bindery Object",
                           MB_ICONQUESTION | MB_YESNO );
      if ( msgRet != IDYES )
      {
         theSystem->message1 = 7; // return 'Canceled' flow
         return 0;
      }
   }

   // get connection handle based on server name
   cHandle = OBindGetConnHandle( oiBindery, FALSE );

   // ??? need to check completion code ???

   // check if 'Is Dynamic' flag is specified and is set
   oflags = BF_STATIC;
   oiIsDynamicFlag = AFuncGetTypedParameter( 2, OTYPE_FLAG );
   if ( oiIsDynamicFlag != 0 )
   {
      if ( OFlagGet(oiIsDynamicFlag) == TRUE )
      {
         oflags = BF_DYNAMIC;
//       isDynamicBool = TRUE;
      }
   }

   // ??? need to get read and write security from parameters

  	sflags = BS_OBJECT_READ | BS_SUPER_WRITE;

	ccode = nwfpNWCreateObject( cHandle, Name, Type, oflags, sflags );
   switch( ccode )
   {
      case SUCCESSFUL:
         switch ( Type )
         {
            case OT_USER:
               {
                  OBJECTID        oiCreatHomeDirFlag;
                  BOOL            bCreateHomeDir = FALSE;

                  oiCreatHomeDirFlag = AFuncGetTypedParameter( 5, OTYPE_FLAG );
                  if ( oiCreatHomeDirFlag != 0 )
                  {
                     if ( OFlagGet(oiCreatHomeDirFlag) == TRUE )
                        bCreateHomeDir = TRUE;
                  }
                  if ( InitNewUserObject( cHandle, Name, bCreateHomeDir ) == 0 )
                     retFlow = 1;
                  else
                     retFlow = 6; // flow 'Error' flow
               }
               break;

            case OT_PRINT_QUEUE:
               if ( InitNewPrtQueueObject( cHandle, Name ) == 0 )
                  retFlow = 1;
               else
                  retFlow = 6; // 'Error' flow
               break;

            case OT_PRINT_SERVER:
               if ( InitNewPrtServerObject( cHandle, Name ) == 0 )
                  retFlow = 1;
               else
                  retFlow = 6; // 'Error' flow
               break;

            default:
               retFlow = 1;
               break;
         } // end of SWITCH on object type
         break;

      case OBJECT_ALREADY_EXISTS:
         retFlow = 2; // follow the flow 'Object exists'
         break;

      case NO_OBJECT_CREATE_PRIVILEGE:
         retFlow = 3; // follow the flow 'No rights'
         break;

//   case INVALID_CONNECTION:
      default:
         retFlow = 6; // follow the flow 'Error'
         break;

   } // end of SWITCH on completion code from NWCreateObject( )

	theSystem->message1 = retFlow;
	return 0;

} // end of CreateObject( )

//--------------------------------------------------------------------------
// FUNCTION:  MyDeleteObject
//
// DESCRIPTION:  Deletes a bindery object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//    2: Flag object - if set, a confirmation dialog will be displayed
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Deleted
//	   2: No wildcards
//    3: No rights                (must be supervisor or equivalent)
//	   4: No such object
//	   5: Object active            (logged in user)
//	   6: Error
//    7: Canceled
//--------------------------------------------------------------------------
LONG FAR PASCAL _export MyDeleteObject( pAEvtInfo, pAObjMessage theSystem )
{
	OBJECTID            oiBindery;     // the ID of the Bindery Object
	BinderyData far *   pBinderyData;  // pointer to a structure with default values
	WORD                Type;
	NWCCODE             ccode;
	char                Name[50];
	NWCONN_HANDLE       cHandle;
   int                 retFlow = 6;   // assume 'Error' flow
   BOOL                confirmOperation = FALSE;
   OBJECTID            oiConfirmFlag;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
	pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
	if ( pBinderyData == NULL )
	{
		theSystem->message1 = 6; // follow flow #5 (ERROR) and return
		return 0;
	}

// Get values and make call

   Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
   // if ( Type == 0 )
      // need to return error - object type not known

	OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );
   AObjUnlockData( oiBindery, 0 );

   // get the value of the Flag input object (Confirmation Dlg)
   oiConfirmFlag = AFuncGetTypedParameter( 2, OTYPE_FLAG );
   if ( oiConfirmFlag != 0 )
   {
      confirmOperation = OFlagGet( oiConfirmFlag );
   }

   if ( confirmOperation == TRUE )
   {  // display a YES / NO message box before proceeding
      int   msgRet;

      msgRet = MessageBox( GetActiveWindow( ), "Proceed With Deletion of Bindery Object",
                           "Delete NetWare Bindery Object",
                           MB_ICONQUESTION | MB_YESNO );
      if ( msgRet != IDYES )
      {
         theSystem->message1 = 7; // return 'Canceled' flow
         return 0;
      }
   }

   // get connection handle based on server name
   cHandle = OBindGetConnHandle( oiBindery, FALSE );

   // ??? check the completion code ???

	ccode = nwfpNWDeleteObject( cHandle, Name, Type );
   switch ( ccode )
   {
      case SUCCESSFUL:
         retFlow = 1;
         break;
      case WILD_CARD_NOT_ALLOWED:
         retFlow = 2;  // follow the 'No wilcards' flow
         break;
      case NO_OBJECT_DELETE_PRIVILEGE:
         retFlow = 3;  // follow the 'No privilege' flow
         break;
      case NO_SUCH_OBJECT:
         retFlow = 4; // follow the 'No such object' flow
         break;
//    case INVALID_CONNECTION:
      default:
         retFlow = 6; // follow the 'Error' flow
         break;
   }

	theSystem->message1 = retFlow;
	return 0;

} // end of MyDeleteObject( )

//--------------------------------------------------------------------------
// FUNCTION:  RenameObject
//
// DESCRIPTION:  Renames an object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values including old object name
//	   2: Text object holding new object name name
//    3: Flag object - if set, a confirmation dialog will be displayed
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Renamed
//	   2: No such object
//    3: Invalid object name
//    4: No rights
//	   5: Error
//    6: Canceled
//--------------------------------------------------------------------------
LONG FAR PASCAL _export RenameObject( pAEvtInfo, pAObjMessage theSystem )
{
	OBJECTID            oiBindery;    // the ID of the Bindery Object
	BinderyData far *   pBinderyData; // pointer to a structure with default values
	OBJECTID            oiNewName;
	WORD                Type;
	NWCCODE             ccode;
	char                Name[50], NewName[50];
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
	pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
	if (pBinderyData == NULL)
	{
		theSystem->message1 = 5; // follow flow #5 (ERROR) and return
		return 0;
	}

	oiNewName = AFuncGetTypedParameter( 2, OTYPE_TEXT );
	if (oiNewName == 0)
	{
		theSystem->message1 = 5;  // follow flow #5 (ERROR) and return
		return 0;
	}

	OTextGetToBuffer( oiNewName, NewName, 50 );

// Get values and make call

   Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
   // if ( Type == 0 )
      // need to return error - object type not known

	OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );

   AObjUnlockData( oiBindery, 0 );

   // get the value of the Flag input object (Confirmation Dlg)
   oiConfirmFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
   if ( oiConfirmFlag != 0 )
   {
      confirmOperation = OFlagGet( oiConfirmFlag );
   }

   if ( confirmOperation == TRUE )
   {  // display a YES / NO message box before proceeding
      int   msgRet;

      msgRet = MessageBox( GetActiveWindow( ), "Proceed With Renaming of Bindery Object",
                           "Rename NetWare Bindery Object",
                           MB_ICONQUESTION | MB_YESNO );
      if ( msgRet != IDYES )
      {
         theSystem->message1 = 6; // return 'Canceled' flow
         return 0;
      }
   }

   // get connection handle based on server name
   cHandle = OBindGetConnHandle( oiBindery, FALSE );
   if ( cHandle != 0 )
   {
      // ??? check if user is SUPERVISOR or equivalent ???

      // ??? check validity of new name (no wildcards, ...) ???

      ccode = nwfpNWRenameObject( cHandle, Name, NewName, Type );
      switch( ccode )
      {
         case SUCCESSFUL:
            retFlow = 1; // follow the 'Renamed' flow
            break;
         case NO_SUCH_OBJECT:
            retFlow = 2; // follow the 'No such object' flow
            break;
//       case INVALID_CONNECTION:
         default:
            retFlow = 5; // follow the 'Error' flow
            break;
      }
   }

	theSystem->message1 = retFlow;
	return 0;

} // end of RenameObject( )

//--------------------------------------------------------------------
// InitNewUserObject( )
//
// RETURN CODES
//    0 = sucessful
//   -1 = error changing password
//--------------------------------------------------------------------
int InitNewUserObject( NWCONN_HANDLE connHandle, char *objectName,
                       BOOL createHomeDir )
{
	NWCCODE    ccode;
   int        retCode = 0;

   // create required properties for USER objects

   // call NWChangeObjectPassword( ) to create pwd
   ccode = nwfpNWChangeObjectPassword( connHandle, objectName, OT_USER, "", "" );
   if ( ccode == SUCCESSFUL )
   {
      NWOBJ_ID     currObjID = -1;

      ccode = nwfpNWScanObject( connHandle, "EVERYONE", OT_USER_GROUP,
                                &currObjID, NULL, NULL, NULL, NULL, NULL );
      if ( ccode == SUCCESSFUL )
      {  // a group named EVERYONE exists
         unsigned char    zeroSegment[ 128 ];

         memset( zeroSegment, '\0', sizeof(zeroSegment) );

         // create property GROUPS_I'M_IN property
         ccode = nwfpNWCreateProperty( connHandle, objectName, OT_USER,
                                       "GROUPS_I'M_IN", (BF_SET | BF_STATIC),
                                       (BS_SUPER_WRITE | BS_LOGGED_READ) );

         // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_USER,
                                           "GROUPS_I'M_IN", 1, zeroSegment, 0 );

         // add EVERYONE group to GROUPS_I'M_IN property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_USER,
                                       "GROUPS_I'M_IN", "EVERYONE", OT_USER_GROUP );

         // create property SECURITY_EQUALS property
         ccode = nwfpNWCreateProperty( connHandle, objectName, OT_USER,
                                       "SECURITY_EQUALS", (BF_SET | BF_STATIC),
                                       (BS_SUPER_WRITE | BS_LOGGED_READ) );

         // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_USER,
                                           "SECURITY_EQUALS", 1, zeroSegment, 0 );

         // add EVERYONE group to SECURITY_EQUALS property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_USER,
                                       "SECURITY_EQUALS", "EVERYONE", OT_USER_GROUP );

         // add this user to the group EVERYONE object's GROUP_MEMBERS property
         ccode = nwfpNWAddObjectToSet( connHandle, "EVERYONE", OT_USER_GROUP,
                                       "GROUP_MEMBERS", objectName, OT_USER );
      }

      // ??? create LOGIN_CONTROL using defaults from SUPERVISOR
      //     user object's USER_DEFAULTS property or use defaults
      //     as defined in SYSCON source code

      if ( createHomeDir == TRUE )
      {  // create home directory

         // ??? use the HOMEDIRPATH property of the SUPERVISOR user object
            // it if exists - use it
            // if it does not exits
              // create HOMEDIRPATH property with ???\SYS:USERS\ as the value
              // use new default as the root path

         // ??? assign the user object rights to the created directory

         // ??? create the \MAIL\objectID directory where the login
         //     scripts are placed in NetWare v3.x (v2.x also ???)
      }
   }
   else
      retCode = -1;

   return retCode;

} // InitNewUserObject( )

//--------------------------------------------------------------------
// InitNewPrtQueueObject( )
//
// RETURN CODES
//    0 = sucessful
//   -1 = error changing password
//--------------------------------------------------------------------
int InitNewPrtQueueObject( NWCONN_HANDLE connHandle, char *objectName )
{
	NWCCODE    ccode;
   int        retCode = 0;

   // create required properties for PRINT QUEUE objects

   // call NWChangeObjectPassword( ) to create pwd
   ccode = nwfpNWChangeObjectPassword( connHandle, objectName, OT_PRINT_QUEUE, "", "" );
   if ( ccode == SUCCESSFUL )
   {
      NWOBJ_ID        currObjID = -1;
      unsigned char   zeroSegment[ 128 ];

      memset( zeroSegment, '\0', sizeof(zeroSegment) );

      // create property Q_USERS property (SET, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_QUEUE,
                                    "Q_USERS", (BF_SET | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_QUEUE,
                                           "Q_USERS", 1, zeroSegment, 0 );
      }

      // create property Q_SERVERS property (SET, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_QUEUE,
                                    "Q_SERVERS", (BF_SET | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_QUEUE,
                                           "Q_SERVERS", 1, zeroSegment, 0 );
      }

      // create property Q_OPERATORS property (SET, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_QUEUE,
                                    "Q_OPERATORS", (BF_SET | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_QUEUE,
                                           "Q_OPERATORS", 1, zeroSegment, 0 );
      }

      // create empty Q_DIRECTORY property (ITEM, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_QUEUE,
                                    "Q_DIRECTORY", (BF_ITEM | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created ITEM property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_QUEUE,
                                           "Q_DIRECTORY", 1, zeroSegment, 0 );
      }

      // see if the default GROUP named EVERYONE exists
      currObjID = -1;
      ccode = nwfpNWScanObject( connHandle, "EVERYONE", OT_USER_GROUP,
                                &currObjID, NULL, NULL, NULL, NULL, NULL );
      if ( ccode == SUCCESSFUL )   // ??? should we also check the value of 'currObjID'
      {  // a group named EVERYONE exists
         // add EVERYONE group to Q_USERS property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_PRINT_QUEUE,
                                       "Q_USERS", "EVERYONE", OT_USER_GROUP );
      }

      // see if the default user SUPERVISOR exists
      currObjID = -1;
      ccode = nwfpNWScanObject( connHandle, "SUPERVISOR", OT_USER,
                                &currObjID, NULL, NULL, NULL, NULL, NULL );
      if ( ccode == SUCCESSFUL )   // ??? should we also check the value of 'currObjID'
      {  // a user named SUPERVISOR exists
         // add SUPERVISOR group to Q_OPERATORS property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_PRINT_QUEUE,
                                       "Q_OPERATORS", "SUPERVISOR", OT_USER );
      }
   }
   else
      retCode = -1;

   return retCode;

} // InitNewPrtQueueObject( )

//--------------------------------------------------------------------
// InitNewPrtServerObject( )
//
// RETURN CODES
//    0 = sucessful
//   -1 = error changing password
//--------------------------------------------------------------------
int InitNewPrtServerObject( NWCONN_HANDLE connHandle, char *objectName )
{
	NWCCODE    ccode;
   int        retCode = 0;

   // create required properties for PRINT SERVER objects

   // call NWChangeObjectPassword( ) to create pwd
   ccode = nwfpNWChangeObjectPassword( connHandle, objectName, OT_PRINT_SERVER, "", "" );
   if ( ccode == SUCCESSFUL )
   {
      NWOBJ_ID        currObjID = -1;
      unsigned char   zeroSegment[ 128 ];

      memset( zeroSegment, '\0', sizeof(zeroSegment) );

      // create property PS_USERS property (SET, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_SERVER,
                                    "PS_USERS", (BF_SET | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_SERVER,
                                           "PS_USERS", 1, zeroSegment, 0 );
      }

      // create property PS_OPERATORS property (SET, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_SERVER,
                                    "PS_OPERATORS", (BF_SET | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created SET property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_SERVER,
                                           "PS_OPERATORS", 1, zeroSegment, 0 );
      }

      // create empty ACCOUNT_BALANCE property (ITEM, STATIC)
      ccode = nwfpNWCreateProperty( connHandle, objectName, OT_PRINT_SERVER,
                                    "ACCOUNT_BALANCE", (BF_ITEM | BF_STATIC),
                                    (BS_SUPER_WRITE | BS_LOGGED_READ) );
      if ( ccode == SUCCESSFUL )
      {  // init newly created ITEM property to one segment of all zeros
         ccode = nwfpNWWritePropertyValue( connHandle, objectName, OT_PRINT_SERVER,
                                           "ACCOUNT_BALANCE", 1, zeroSegment, 0 );
      }

      // see if the default GROUP named EVERYONE exists
      currObjID = -1;
      ccode = nwfpNWScanObject( connHandle, "EVERYONE", OT_USER_GROUP,
                                &currObjID, NULL, NULL, NULL, NULL, NULL );
      if ( ccode == SUCCESSFUL )   // ??? should we also check the value of 'currObjID'
      {  // a group named EVERYONE exists
         // add EVERYONE group to Q_USERS property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_PRINT_SERVER,
                                       "PS_USERS", "EVERYONE", OT_USER_GROUP );
      }

      // see if the default user SUPERVISOR exists
      currObjID = -1;
      ccode = nwfpNWScanObject( connHandle, "SUPERVISOR", OT_USER,
                                &currObjID, NULL, NULL, NULL, NULL, NULL );
      if ( ccode == SUCCESSFUL )   // ??? should we also check the value of 'currObjID'
      {  // a user named SUPERVISOR exists
         // add SUPERVISOR group to Q_OPERATORS property
         ccode = nwfpNWAddObjectToSet( connHandle, objectName, OT_PRINT_SERVER,
                                       "PS_OPERATORS", "SUPERVISOR", OT_USER );
      }
   }
   else
      retCode = -1;

   return retCode;

} // InitNewPrtServerObject( )

