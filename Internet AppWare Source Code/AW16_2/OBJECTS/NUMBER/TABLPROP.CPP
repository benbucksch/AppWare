//--------------------------------------------------------------------------
// TablProp.CPP
//
// DESCRIPTION:
//	   The Bindery ALM's object table property manipulation functions.
//
// VAB FUNCTIONS IMPLEMENTED:
//    Get Table Property         ---> GetTableBindProp( )
//    Set Table Property         ---> SetTableBindProp( )
//
// POTENTIAL OBJECT APIs IMPLEMENTED:
//    OBindConvertRightsToText( )
//
// UTILITY FUNCTIONS IMPLEMENTED:
//    WriteLoginControlToTable( )
//    WriteUserDefaultsToTable( )
//    WriteDirectoryTrusteesToTable( )
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

void WriteLoginControlToTable( OBJECTID oiTable, Restrictions *loginRestrictions );
void WriteUserDefaultsToTable( OBJECTID oiTable, DefaultRestrictions *defaultRestrictions );
void WriteDirectoryTrusteesToTable( NWCONN_HANDLE connHandle, NWOBJ_ID bindObjID, OBJECTID oiOutTable );
void OBindConvertRightsToText( NWFLAGS accessRights, char NWFAR *rightsText,
                               NWSERVICE_VERSION majorVersion, NWSERVICE_VERSION minorVersion );

//--------------------------------------------------------------------------
LONG FAR PASCAL _export SetTableBindProp( pAEvtInfo, pAObjMessage theSystem )
//
// INPUT PARAMETERS
//
// OUTPUT PARAMETERS
//
// FLOWS
//    1: Set
//--------------------------------------------------------------------------
{
   theSystem->message1 = 1;
   return 0;
}


//--------------------------------------------------------------------------
// FUNCTION:    GetTableBindProp
//
// DESCRIPTION: Implements the 'Get Text Property' function
//    Reads the value(s) of a property from a Bindery object and outputs the values
//    to a Text object
//
// INPUT PARAMETERS
//    1: Bindery object holding input values (MANDATORY)
//    2: Text object holding property name (MANDATORY)
//    3: Flag object - if set means we can adjust # columns in tbl (OPTIONAL)
//
// OUTPUT PARAMETERS
//    1: Table object to receive property value (MANDATORY)
//
// FLOWS
//    1: Retrieved
//    2: No such object
//    3: No such property
//    4: Wrong data type
//    5: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetTableBindProp( pAEvtInfo, pAObjMessage theSystem )
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

               if ( strcmpi( PName, TRUSTEE_DIR_STR ) == 0 )
               {  // special case to handle the 'Trustee Directory Assignments' property
                  // which is not actually stored as a Bindery property
                  OBJECTID     oiOutTable;
                  TYPEID       tiOutTable;
                  OBJECTID     oiModTableFlag;
                  BOOL         canChangeTable = FALSE;

                  oiModTableFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
                  if ( oiModTableFlag != 0 )
                  {
                     if ( OFlagGet(oiModTableFlag) == TRUE )
                        canChangeTable = TRUE;
                  }

                  oiOutTable = AFuncGetOutput( 1, &tiOutTable );
                  if ( (oiOutTable != 0) && (tiOutTable == OTYPE_TABLE) )
                  {  // output TABLE object is hooked up and is of the correct type
                     NWOBJ_ID    bindObjID = -1;

                     // clear the table object
                     tblSetRowCount( oiOutTable, 0 );
                      
                     if ( canChangeTable == TRUE )
                     {  // change column titles and number of columns if allowed
                        // set number of columns to three
                        tblSetColumnCount( oiOutTable, 2 );
                        // set column titles (index is zero-based)
                        tblColumnTitle( oiOutTable, "NetWare Path Specification", 0 );
                        tblColumnTitle( oiOutTable, "Rights", 1 );
                     }

                     // need to pass in the User of Group Bindery object ID
                     ccode = nwfpNWScanObject( cHandle, OName, Type, &bindObjID,
                                               NULL, NULL, NULL, NULL, NULL );
                     if ( ccode == SUCCESSFUL )
                     {
                        WriteDirectoryTrusteesToTable( cHandle, bindObjID, oiOutTable );
                        retFlow = 1; // return 'Retrieved' flow
                     }
                     else
                        retFlow = 2; // return 'No such object' flow
                  }
                  else
                     retFlow = 5; // return 'Error' flow
               }
               else
               {
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
                  OBJECTID     oiOutTable;
                  TYPEID       tiOutTable;

                  oiOutTable = AFuncGetOutput( 1, &tiOutTable );
                  if ( (oiOutTable != 0) && (tiOutTable == OTYPE_TABLE) )
                  {  // output TABLE object is hooked up and is of the correct type

                     // clear the table object
                     tblSetRowCount( oiOutTable, 0 );

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
                                       lstrcat( pObjectNames, "\r\n" );
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
                           {  // converted the list of Bindery Object IDs to object names
                              int   currNumCols = 0;

                              currNumCols = tblGetColumnCount( oiOutTable );
                              if ( (currNumCols != 2) )
                              {  // ??? create the desired number of columns
                                 // ??? set the column titles to the default
                                 tblSetColumnCount( oiOutTable, 2 );
                              }
                              // add new rows to the table
                              tblAddRow( oiOutTable, pObjectNames, 0 );
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
								if ( strcmpi(PName, "LOGIN_CONTROL") == 0 )
								{
									Restrictions *loginRestrictions; // data structure defined in NWSysCon.H

									loginRestrictions = (Restrictions *)pcPropertyValue;
									WriteLoginControlToTable( oiOutTable, loginRestrictions );
									retFlow = 1; // return 'Retrieved' flow
								}
								else if ( strcmpi(PName, "USER_DEFAULTS") == 0 )
								{
									DefaultRestrictions *defaultRestrictions; // data structure defined in NWSysCon.H

									defaultRestrictions = (DefaultRestrictions *)pcPropertyValue;
									WriteUserDefaultsToTable( oiOutTable, defaultRestrictions );
									retFlow = 1; // return 'Retrieved' flow
								}
								else if ( strcmpi(PName, "MISC_LOGIN_INFO") == 0 )
								{  // ??? print message stating we don't know how to display
									tblAddRow( oiOutTable, "GET of this property not yet implemented !!!", 0 );
									retFlow = 1; // return 'Retrieved' flow
								}
								else if ( strcmpi(PName, "NET_ADDRESS") == 0 )
								{  // ??? print message stating we don't know how to display
									tblAddRow( oiOutTable, "GET of this property not yet implemented !!!", 0 );
									retFlow = 1; // return 'Retrieved' flow
								}
								else
                        {  // add new rows to the table
                           tblAddRow( oiOutTable, pcPropertyValue, 0 );
                           retFlow = 1; // return 'Retrieved' flow
                        }
                     }
                  }
                  else
                  {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
                  }
               } // end of IF return code was successful
               } // end of ELSE current property if not 'Trustee Directory Assignments'
            }
         }
         if ( pcPropertyValue != NULL )
            free( pcPropertyValue );
      }
   }

   theSystem->message1 = retFlow;
   return 0;

} // end of GetTableBindProp( )

//--------------------------------------------------------------------------
void WriteLoginControlToTable( OBJECTID oiOutTable, Restrictions *loginRestrictions )
//--------------------------------------------------------------------------
{
   char             tempBuffer[ 256 ];
   WORD             tempWord;

   if ( loginRestrictions->accountExpiresMonth == 0 )
   {
      tblAddRow( oiOutTable, "Account Expires (mm-dd-yy)\tno expiration date\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Account Expires (mm-dd-yy)\t%02d-%02d-%04d\r",
                loginRestrictions->accountExpiresMonth,
                loginRestrictions->accountExpiresDay,
                loginRestrictions->accountExpiresYear + 1900 );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   if ( loginRestrictions->passwordExpiresMonth == 0 )
   {
      tblAddRow( oiOutTable, "Password Expires (mm-dd-yy)\tno expiration date\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Password Expires (mm-dd-yy)\t%02d-%02d-%04d\r",
                            loginRestrictions->passwordExpiresMonth,
                            loginRestrictions->passwordExpiresDay,
                            loginRestrictions->passwordExpiresYear + 1900 );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   wsprintf( tempBuffer, "Minimum Password Length\t%d\r",
                         loginRestrictions->minimumPasswordLength );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   tempWord = nwfpNWWordSwap( loginRestrictions->maxConcurrentConnections );
   wsprintf( tempBuffer, "Maximum Concurrent Connections\t%d\r", tempWord );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   if ( (loginRestrictions->maxDiskBlocks == 0x7FFFFFFF) ||
        (loginRestrictions->maxDiskBlocks == 0xFFFFFF7F) )
   {
      tblAddRow( oiOutTable, "Maximum Disk Blocks\tno limit\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Maximum Disk Blocks\t0x%08lx\r",
                            loginRestrictions->maxDiskBlocks );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   tempWord = nwfpNWWordSwap( loginRestrictions->expirationInterval );
   if ( tempWord == 0 )
      tblAddRow( oiOutTable, "Password Expiration Interval (in days)\tnot specified\r", 0 );
   else
   {
      wsprintf( tempBuffer, "Password Expiration Interval (in days)\t%d\r", tempWord );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   tempWord = nwfpNWWordSwap( loginRestrictions->badLoginCount );
   wsprintf( tempBuffer, "Bad Login Count\t%d\r", tempWord );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   if ( loginRestrictions->lastLoginDate[0] == 0 )
   {
      tblAddRow( oiOutTable, "Last Login (mm-dd-yy)\tunknown\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Last Login (mm-dd-yy  hh:mm:ss)\t%02d-%02d-%04d  %02d:%02d:%02d\r",
                            loginRestrictions->lastLoginDate[1],
                            loginRestrictions->lastLoginDate[2],
                            loginRestrictions->lastLoginDate[0] + 1900,
                            loginRestrictions->lastLoginDate[3],
                            loginRestrictions->lastLoginDate[4],
                            loginRestrictions->lastLoginDate[5] );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   if ( loginRestrictions->accountExpired == 0 )
      tblAddRow( oiOutTable, "Account Status\tenabled\r", 0 );
   else
      tblAddRow( oiOutTable, "Account Status\tdisabled\r", 0 );

   wsprintf( tempBuffer, "Password Grace Logins\t%d\r",
                         loginRestrictions->passwordGraceLogins );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   wsprintf( tempBuffer, "Number Of Grace Logins Remaining\t%d\r",
                         loginRestrictions->graceReset );
   tblAddRow( oiOutTable, tempBuffer, 0 );


#ifdef ZZZ_BOGUS_COPY
typedef struct
{
	BYTE timeBitMap[42];
	BYTE restrictionFlags;
	LONG nextResetTime;
	BYTE badStationAddress[12];
} Restrictions;
#endif

} // end of WriteLoginControlToTable( )

//--------------------------------------------------------------------------
void WriteUserDefaultsToTable( OBJECTID oiOutTable, DefaultRestrictions *defaultRestrictions )
//--------------------------------------------------------------------------
{
   char             tempBuffer[ 256 ];
   WORD             tempWord;

   if ( defaultRestrictions->accountExpiresMonth == 0 )
   {
      tblAddRow( oiOutTable, "Account Expires (mm-dd-yy)\tno expiration date\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Account Expires (mm-dd-yy)\t%02d-%02d-%04d\r",
                (unsigned int)defaultRestrictions->accountExpiresMonth,
                (unsigned int)defaultRestrictions->accountExpiresDay,
                (unsigned int)defaultRestrictions->accountExpiresYear + 1900 );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   wsprintf( tempBuffer, "Minimum Password Length\t%d\r",
                         (unsigned int)defaultRestrictions->minimumPasswordLength );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   tempWord = nwfpNWWordSwap( defaultRestrictions->maxConcurrentConnections );
   wsprintf( tempBuffer, "Maximum Concurrent Connections\t%d\r", tempWord );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   if ( (defaultRestrictions->maxDiskBlocks == 0x7FFFFFFF) ||
        (defaultRestrictions->maxDiskBlocks == 0xFFFFFF7F) )
   {
      tblAddRow( oiOutTable, "Maximum Disk Blocks\tno limit\r", 0 );
   }
   else
   {
      wsprintf( tempBuffer, "Maximum Disk Blocks\t0x%08lx\r",
                            defaultRestrictions->maxDiskBlocks );
      tblAddRow( oiOutTable, tempBuffer, 0 );
   }

   tempWord = nwfpNWWordSwap( defaultRestrictions->expirationInterval );
   wsprintf( tempBuffer, "Password Expiration Interval (in days)\t%d\r", tempWord );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   wsprintf( tempBuffer, "Default Number Of Grace Logins Allowed\t%d\r",
                         defaultRestrictions->graceReset );
   tblAddRow( oiOutTable, tempBuffer, 0 );

   if ( defaultRestrictions->createHomeDir == 0 )
      tblAddRow( oiOutTable, "Create User Home Directories\tno\r", 0 );
   else
      tblAddRow( oiOutTable, "Create User Home Directories\tyes\r", 0 );

#ifdef ZZZ_BOGUS_COPY
typedef struct
{
	BYTE restrictionFlags;
	BYTE timeBitMap[42];
	long balance;
	long creditLimit;
	BYTE filler1;
} DefaultRestrictions;
#endif

} // end of WriteUserDefaultsToTable( )

//--------------------------------------------------------------------------
void WriteDirectoryTrusteesToTable( NWCONN_HANDLE connHandle, NWOBJ_ID bindObjID, OBJECTID oiOutTable )
//--------------------------------------------------------------------------
{
   WORD                      maxVolumes = 1;
   NWVOL_NUM                 currVolNum = 0;
   char                      currVolName[     80 ];
   char                      currPathText[   300 ];
   char                      currRightsText[  20 ];
   char                      currRowText[    512 ];
   NWCCODE                   ccode;
   BOOL                      done = FALSE;
   NWTRUSTEE_SEQUENCE_NUM    currSeqNum = -1;
   NWFLAGS                   accessRights;
   NWSERVICE_VERSION         nwMajorVersion;
   NWSERVICE_VERSION         nwMinorVersion;

   // determine maximum number of volumes
   ccode = nwfpNWGetFileServerInformation( connHandle, NULL, &nwMajorVersion, &nwMinorVersion,
                                           NULL, NULL, NULL, NULL, &maxVolumes, NULL, NULL );

   if ( (ccode == SUCCESSFUL) && (oiOutTable != 0) )
   {
      while ( (currVolNum < maxVolumes) && (! done) )
      {
         // get volume name for current volume number
         ccode = nwfpNWGetVolumeName( connHandle, currVolNum, currVolName );
         if ( ccode == NO_FILES_FOUND_ERROR )
         {  // probably means no more installed volumes
            currVolNum = maxVolumes + 1;
            done = TRUE;
         }
         else
         {
            BOOL    foundAll = FALSE;

            // scan for each trustee directory assignment on this volume
            while ( ! foundAll )
            {
               currPathText[ 0 ] = '\0';
               ccode = nwfpNWScanObjectTrusteePaths( connHandle, bindObjID, currVolNum, &currSeqNum,
                                                     &accessRights, currPathText );

               // path is NULL if all paths have been returned
               if ( (ccode == SUCCESSFUL) && (currPathText[0] != '\0') )
               {
                  currRowText[ 0 ] = '\0';
                  // column 1 = path name
                  lstrcat( currRowText, currPathText );
                  lstrcat( currRowText, "\t" );
  
                  // convert the rights flags into an ASCII string
                  currRightsText[ 0 ] = '\0';
                  OBindConvertRightsToText( accessRights, currRightsText, nwMajorVersion,
                                            nwMinorVersion );

                  // column 2 = rights string
                  lstrcat( currRowText, currRightsText );

                  // add the current buffer to the destination Table object
                  tblAddRow( oiOutTable, currRowText, 0 );
               }
               else
                  foundAll = TRUE;

            } // end of WHILE not all trustee directories on this volume found
         }
         ++currVolNum;
      } // end of WHILE more volumes to check
   }
   else
   {
      // somehow return the error code
   }

}  // end of WriteDirectoryTrusteesToTable( )

//--------------------------------------------------------------------------
void OBindConvertRightsToText( NWFLAGS accessRights, char NWFAR *rightsText,
                               NWSERVICE_VERSION majorVersion, NWSERVICE_VERSION minorVersion )
//--------------------------------------------------------------------------
{
   // the displaying of the rights is taken from SYSCON source code
   //   file SYSCON6.C in routine FormatTrusteeLine( ... )
   if ( rightsText != NULL )
   {
      rightsText[0] = '\0';

      lstrcat( rightsText, "[" );

      if ( majorVersion >= 3 )
      {
         if ( accessRights & 0x0100 )
            lstrcat( rightsText, "S" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_READ )
            lstrcat( rightsText, "R" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_WRITE )
            lstrcat( rightsText, "W" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_CREATE )
            lstrcat( rightsText, "C" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_DELETE )
            lstrcat( rightsText, "E" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_MODIFY )
            lstrcat( rightsText, "M" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_SEARCH )
            lstrcat( rightsText, "F" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_OWNERSHIP )
            lstrcat( rightsText, "A" );
         else
            lstrcat( rightsText, " " );
      }
      else if ( (majorVersion == 2) && (minorVersion > 15) )
      {
         if ( (accessRights & TA_READ) && (accessRights & TA_OPEN) )
            lstrcat( rightsText, "R" );
         else
            lstrcat( rightsText, " " );

         if ( (accessRights & TA_WRITE) && (accessRights & TA_OPEN) )
            lstrcat( rightsText, "W" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_CREATE )
            lstrcat( rightsText, "C" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_DELETE )
            lstrcat( rightsText, "E" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_MODIFY )
            lstrcat( rightsText, "M" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_SEARCH )
            lstrcat( rightsText, "F" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_OWNERSHIP )
            lstrcat( rightsText, "A" );
         else
            lstrcat( rightsText, " " );
      }
      else
      {
         if ( accessRights & TA_READ )
            lstrcat( rightsText, "R" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_WRITE )
            lstrcat( rightsText, "W" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_OPEN )
            lstrcat( rightsText, "O" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_CREATE )
            lstrcat( rightsText, "C" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_DELETE )
            lstrcat( rightsText, "D" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_OWNERSHIP )
            lstrcat( rightsText, "P" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_SEARCH )
            lstrcat( rightsText, "S" );
         else
            lstrcat( rightsText, " " );

         if ( accessRights & TA_MODIFY )
            lstrcat( rightsText, "M" );
         else
            lstrcat( rightsText, " " );
      }

      lstrcat( rightsText, "]" );
   }

} // end of OBindConvertRightsToText( )
