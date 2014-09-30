//--------------------------------------------------------------------------
// ArchRest.CPP
//
// DESCRIPTION:
//	   The Bindery ALM's functions relating to archiving and restoring
//    the Bindery files.
//
// VAB FUNCTIONS IMPLEMENTED:
//    Archive Bindery
//    Restore Bindery
//    Close Bindery
//    Open Bindery
//
// STATUS:
//   * currently requires that for NetWare or UNC path specs, that
//     a drive mapping must already exist to the specified server volume
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <utime.h>
#include <dir.h>
#include <dos.h>
#include <ctype.h>

#include <a_alm.h>
#include <o_text.h>    // defines OTYPE_TEXT
#include <o_number.h>  // defines OTYPE_NUMBER
#include <o_flag.h>    // defines OTYPE_FLAG
#include <o_table.h>   // defines OTYPE_TABLE
#include <o_nwerr.h>   // defines OTYPE_NWERROR

#include <nwcalls.h>

#include "Bindery.h"
#include "BindErr.h"
#include "nwdynlib.h"

int   BindCopyFile( char *srcfile, char *destfile );
BOOL  BindDoesDirectoryExist( char far *dirName );
int   BindCreateDirectory( char far *dirName );
BOOL  BindIsNetWarePath( char far *pathSpec );
BOOL  BindIsDOSPath( char far *pathSpec );
BOOL  BindIsUNCPath( char far *pathSpec );
BOOL  BindIsRelativePath( char far *pathSpec );
int   BindConvertNetWarePathToDOS( char far *currPathSpec, char far *newPathSpec, int newBufLen );
int   BindConvertUNCPathToDOS( char far *currPathSpec, char far *newPathSpec, int newBufLen );

//---------------------------------------------------------------------------------
LONG FAR PASCAL _export ArchiveBinderyFiles( pAEvtInfo, pAObjMessage theSystem )
//
// Archives the NetWare Bindery files to the specified directory.
//
// INPUT PARAMETERS
//    1. An 'NW Bindery' object specifying the NetWare Bindery to manipulate.
//       This input is MANDATORY.
//    2. A 'Text' object specifying the directory specification of where to
//       store the NetWare Bindery system files. This input is MANDATORY.
//
//       ??? path specification types (DOS, NetWare, UNC) supported ???
//
//    3. A 'Flag' object which, if set, will result in a confirmation dialog
//       to be displayed to the user before the archive operation occurs.
//       This input is OPTIONAL.
//
// OUTPUT PARAMETERS
//    None
//
// RETURN FLOWS
//   1. Archived
//   2. Locked         The specified NetWare Bindery is currently locked and cannot be archived.
//   3. No rights      The current user must be Supervisor or equivalent for this operation to
//                     succeed.
//   4. Not supported  Archiving and restoring the NetWare Bindery system files are not supported
//                     for NetWare servers which use Bindery emulation.
//   5. No server      No specific NetWare server name was associated in the 'NW Bindery' ALM's
//                     object editing dialog.  This function will NOT default to use the
//                     current primary server.
//   6. Error
//   7. Canceled
//---------------------------------------------------------------------------------
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiDestPath;   // object ID of the Text object containing the destination path
   int               retFlow = 6;  // assume 'Error' flow
   NWCCODE           ccode;
   NWCONN_HANDLE     cHandle;
   NWSERVICE_VERSION majorVersion, minorVersion, revision;

   char              serverName[    50 ];
   char              tempPath[     500 ];
   char              srcDOSPath[   500 ];
   char              destDOSPath[  500 ];
   char              currSrcFile[  500 ];
   char              currDestFile[ 500 ];

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow; // return 'Error' flow if no network
      return 0;
   }

   oiBindery  = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiDestPath = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiDestPath != 0) )
   {
      BOOL      confirmOperation = FALSE;
      OBJECTID  oiConfirmFlag;

      // get the value of the Flag input object (Confirmation Dlg)
      oiConfirmFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
      if ( oiConfirmFlag != 0 )
      {
         confirmOperation = OFlagGet( oiConfirmFlag );
      }

      if ( confirmOperation == TRUE )
      {  // display a YES / NO message box before proceeding
         int   msgRet;

         msgRet = MessageBox( GetActiveWindow( ), "Proceed With Archive of NetWare Bindery",
                              "Archive NetWare Bindery Files",
                              MB_ICONQUESTION | MB_YESNO );
         if ( msgRet != IDYES )
         {
            theSystem->message1 = 7; // return 'Canceled' flow
            return 0;
         }
      }

      // go ahead and perform the ARCHIVE operation

      retFlow = 1; // assume we will return the 'Archived' flow

      // get connection handle to server - CANNOT BE VIA DEFAULT SERVER CONNECTION
      cHandle = OBindGetConnHandle( oiBindery, FALSE );
      if ( cHandle == 0 )
      {  // return the flow 'Error'
         theSystem->message1 = 6;
         return 0;
      }

      // determine if NetWare user has SUPERVISOR equivalence
      ccode = nwfpNWIsManager( cHandle );
      if ( ccode != 0 )
      {  // user does NOT have supervisor equivalence - return the 'No rights' flow
         theSystem->message1 = 3;
         return 0;
      }

      // determine the version of NetWare running
      serverName[ 0 ] = '\0';
      ccode = nwfpNWGetFileServerInformation( cHandle, serverName, &majorVersion, &minorVersion, &revision,
                                              NULL, NULL, NULL, NULL, NULL, NULL );
      if ( ccode != SUCCESSFUL )
      {  // return the flow 'Error'
         theSystem->message1 = 6;
         return 0;
      }

      // build source path from server name - will be a NetWare path spec
      srcDOSPath[ 0 ] = '\0';
      wsprintf( tempPath, "%s\\SYS:\\SYSTEM\\", serverName );
      // convert Bindery file path from NetWare to DOS
      BindConvertNetWarePathToDOS ( tempPath, srcDOSPath, sizeof(srcDOSPath) - 1 );
      // append the trailing '\\' if it is missing
      if ( strlen(srcDOSPath) >= 1 )
      {
         if ( srcDOSPath[ strlen(srcDOSPath) - 1 ] != '\\' )
            strcat( srcDOSPath, "\\" );
      }

      // obtain destination path from Text object
      tempPath[ 0 ] = destDOSPath[ 0 ] = '\0';
      OTextGetToBuffer( oiDestPath, tempPath, sizeof(tempPath) - 1 );
      // convert destination path to DOS, if required
      if ( BindIsNetWarePath( tempPath ) == TRUE )
      {  // convert NetWare path to DOS
         BindConvertNetWarePathToDOS( tempPath, destDOSPath, sizeof( destDOSPath) - 1 );
      }
      else if ( BindIsUNCPath( tempPath ) == TRUE )
      {  // convert UNC path to DOS
         BindConvertUNCPathToDOS( tempPath, destDOSPath, sizeof( destDOSPath) - 1 );
      }
      else
      {  // if path is a full DOS path or a relative DOS path
         strcpy( destDOSPath, tempPath );
      }
      // append the trailing '\\' if it is missing
      if ( strlen(destDOSPath) >= 1 )
      {
         if ( destDOSPath[ strlen(destDOSPath) - 1 ] != '\\' )
            strcat( destDOSPath, "\\" );
      }

      // copy the files, based on the version of NetWare running
      if ( majorVersion == 3 )
      {  // if NetWare v3.x - need to save the three Bindery files
         // close the Bindery
         ccode = nwfpNWCloseBindery( cHandle );
         if ( ccode == SUCCESSFUL )
         {  // copy the required three files to the dest path
            int copyReturn;

            strcpy( currSrcFile, srcDOSPath );
            strcat( currSrcFile, "NET$OBJ.SYS" );
            strcpy( currDestFile, destDOSPath );
            strcat( currDestFile, "NET$OBJ.SYS" );
            copyReturn = BindCopyFile( currSrcFile, currDestFile );
            if ( copyReturn == OBIND_ERR_SUCCESS )
            {
               strcpy( currSrcFile, srcDOSPath );
               strcat( currSrcFile, "NET$PROP.SYS" );
               strcpy( currDestFile, destDOSPath );
               strcat( currDestFile, "NET$PROP.SYS" );
               copyReturn = BindCopyFile( currSrcFile, currDestFile );
               if ( copyReturn == OBIND_ERR_SUCCESS )
               {
                  strcpy( currSrcFile, srcDOSPath );
                  strcat( currSrcFile, "NET$VAL.SYS" );
                  strcpy( currDestFile, destDOSPath );
                  strcat( currDestFile, "NET$VAL.SYS" );
                  copyReturn = BindCopyFile( currSrcFile, currDestFile );
                  if ( copyReturn != OBIND_ERR_SUCCESS )
                     retFlow = 6; // return the 'Error' flow
               }
               else
                  retFlow = 6; // return the 'Error' flow
            }
            else
               retFlow = 6; // return the 'Error' flow


            // open the Bindery
            ccode = nwfpNWOpenBindery( cHandle );
         }
         else if ( ccode == BINDERY_LOCKED )
         {
            retFlow = 2; // return the 'Locked' flow 
         }
         else
         {
            retFlow = 6; // return the 'Error' flow
         }
      }
      else if ( majorVersion == 2 )
      {  // if NetWare v2.x - need to save the two Bindery files
         // close the Bindery
         ccode = nwfpNWCloseBindery( cHandle );
         if ( ccode == SUCCESSFUL )
         {  // copy the required two files to the dest path
            int copyReturn;

            strcpy( currSrcFile, srcDOSPath );
            strcat( currSrcFile, "NET$BIND.SYS" );
            strcpy( currDestFile, destDOSPath );
            strcat( currDestFile, "NET$BIND.SYS" );
            copyReturn = BindCopyFile( currSrcFile, currDestFile );
            if ( copyReturn == OBIND_ERR_SUCCESS )
            {
               strcpy( currSrcFile, srcDOSPath );
               strcat( currSrcFile, "NET$BVAL.SYS" );
               strcpy( currDestFile, destDOSPath );
               strcat( currDestFile, "NET$BVAL.SYS" );
               copyReturn = BindCopyFile( currSrcFile, currDestFile );
               if ( copyReturn != OBIND_ERR_SUCCESS )
                  retFlow = 6; // return the 'Error' flow
            }
            else
               retFlow = 6; // return the 'Error' flow

            // open the Bindery
            ccode = nwfpNWOpenBindery( cHandle );
         }
         else if ( ccode == BINDERY_LOCKED )
         {
            retFlow = 2; // return the 'Locked' flow 
         }
         else
         {
            retFlow = 6; // return the 'Error' flow
         }
      }
      else if ( majorVersion > 3 )
      {  // if server is Bindery emulation
         retFlow = 4; // return the 'Not supported' flow
      }
   }

   theSystem->message1 = retFlow;
   return 0;

}  // end of ArchiveBinderyFiles( )

//---------------------------------------------------------------------------------
LONG FAR PASCAL _export RestoreBinderyFiles( pAEvtInfo, pAObjMessage theSystem )
//
// Restores the NetWare Bindery files from the specified directory to the NetWare server.
//
// INPUT PARAMETERS
//   1. An 'NW Bindery' object specifying the NetWare Bindery to manipulate.
//      This input is MANDATORY.
//   2. A 'Text' object specifying the directory specification of where the
//      NetWare Bindery system files can be found. This input is MANDATORY.
//   3. A 'Flag' object which, if set, will result in a confirmation dialog
//      to be displayed to the user before the restore operation occurs.
//      This input is OPTIONAL.
//
// OUTPUT PARAMETERS
//   None
//
// RETURN FLOWS
//   1. Restored
//   2. Locked         The specified NetWare Bindery is currently locked and cannot be restored.
//   3. No rights      The current user must be Supervisor or equivalent for this operation to
//                     succeed.
//   4. Not supported  Archiving and restoring the NetWare Bindery system files are not supported
//                     for NetWare servers which use Bindery emulation.
//   5. No server      No specific NetWare server name was associated in the 'NW Bindery' ALM's
//                     object editing dialog.  This function will not default to use the current
//                     primary server.
//   6. Error
//   7. Canceled
//---------------------------------------------------------------------------------
{
   OBJECTID          oiBindery;    // the ID of the Bindery Object
   OBJECTID          oiSrcPath;    // object ID of the Text object containing the source path
   int               retFlow = 6;  // assume 'Error' flow
   NWCCODE           ccode;
   NWCONN_HANDLE     cHandle;
   NWSERVICE_VERSION majorVersion, minorVersion, revision;

   char              serverName[    50 ];
   char              tempPath[     500 ];
   char              srcDOSPath[   500 ];
   char              destDOSPath[  500 ];
   char              currSrcFile[  500 ];
   char              currDestFile[ 500 ];

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow; // return 'Error' flow if no network
      return 0;
   }

   oiBindery  = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   oiSrcPath  = AFuncGetTypedParameter( 2, OTYPE_TEXT );
   if ( (oiBindery != 0) && (oiSrcPath != 0) )
   {
      BOOL      confirmOperation = FALSE;
      OBJECTID  oiConfirmFlag;

      // get the value of the Flag input object (Confirmation Dlg)
      oiConfirmFlag = AFuncGetTypedParameter( 3, OTYPE_FLAG );
      if ( oiConfirmFlag != 0 )
      {
         confirmOperation = OFlagGet( oiConfirmFlag );
      }

      if ( confirmOperation == TRUE )
      {  // display a YES / NO message box before proceeding
         int   msgRet;

         msgRet = MessageBox( GetActiveWindow( ), "Proceed With Restore of NetWare Bindery",
                              "Restore NetWare Bindery Files",
                              MB_ICONQUESTION | MB_YESNO );
         if ( msgRet != IDYES )
         {
            theSystem->message1 = 7; // return 'Canceled' flow
            return 0;
         }
      }

      // go ahead and perform the RESTORE operation

      retFlow = 1; // assume we will return the 'Restored' flow

      // get connection handle to server - CANNOT BE VIA DEFAULT SERVER CONNECTION
      cHandle = OBindGetConnHandle( oiBindery, FALSE );
      if ( cHandle == 0 )
      {  // return the flow 'Error'
         theSystem->message1 = 6;
         return 0;
      }

      // determine if NetWare user has SUPERVISOR equivalence
      ccode = nwfpNWIsManager( cHandle );
      if ( ccode != 0 )
      {  // user does NOT have supervisor equivalence - return the 'No rights' flow
         theSystem->message1 = 3;
         return 0;
      }

      // determine the version of NetWare running
      serverName[ 0 ] = '\0';
      ccode = nwfpNWGetFileServerInformation( cHandle, serverName, &majorVersion, &minorVersion, &revision,
                                              NULL, NULL, NULL, NULL, NULL, NULL );
      if ( ccode != SUCCESSFUL )
      {  // return the flow 'Error'
         theSystem->message1 = 6;
         return 0;
      }

      // builde destination path as a NetWare path spec
      destDOSPath[ 0 ] = '\0';
      wsprintf( tempPath, "%s\\SYS:\\SYSTEM\\", serverName );
      // convert Bindery file path from NetWare to DOS
      BindConvertNetWarePathToDOS( tempPath, destDOSPath, sizeof(destDOSPath) - 1 );
      // append the trailing '\\' if it is missing
      if ( strlen(destDOSPath) >= 1 )
      {
         if ( destDOSPath[ strlen(destDOSPath) - 1 ] != '\\' )
            strcat( destDOSPath, "\\" );
      }

      // obtain destination path from Text object
      tempPath[ 0 ] = srcDOSPath[ 0 ] = '\0';
      OTextGetToBuffer( oiSrcPath, tempPath, sizeof(tempPath) - 1 );
      // convert source path to DOS, if required
      if ( BindIsNetWarePath( tempPath ) == TRUE )
      {  // convert NetWare path to DOS
         BindConvertNetWarePathToDOS( tempPath, srcDOSPath, sizeof(srcDOSPath) - 1 );
      }
      else if ( BindIsUNCPath( tempPath ) == TRUE )
      {  // convert UNC path to DOS
         BindConvertUNCPathToDOS( tempPath, srcDOSPath, sizeof(srcDOSPath) - 1 );
      }
      else
      {  // if path is a full DOS path or a relative DOS path
         strcpy( srcDOSPath, tempPath );
      }
      // append the trailing '\\' if it is missing
      if ( strlen(srcDOSPath) >= 1 )
      {
         if ( srcDOSPath[ strlen(srcDOSPath) - 1 ] != '\\' )
            strcat( srcDOSPath, "\\" );
      }

      // copy the files, based on the version of NetWare running
      if ( majorVersion == 3 )
      {  // if NetWare v3.x - need to RESTORE the three Bindery files
         // close the Bindery
         ccode = nwfpNWCloseBindery( cHandle );
         if ( ccode == SUCCESSFUL )
         {  // copy the required three files to the dest path
            int copyReturn;

            // ??? we should probably check to make sure that all the Bindery
            //     files exist in the specified source path before copying any
            //     of the archived files

            // ??? is there any way we can check the header of the file to see
            //     if it is actually the expected file, not just a file of
            //     the same name

            strcpy( currSrcFile, srcDOSPath );
            strcat( currSrcFile, "NET$OBJ.SYS" );
            strcpy( currDestFile, destDOSPath );
            strcat( currDestFile, "NET$OBJ.SYS" );
            copyReturn = BindCopyFile( currSrcFile, currDestFile );
            if ( copyReturn == OBIND_ERR_SUCCESS )
            {
               strcpy( currSrcFile, srcDOSPath );
               strcat( currSrcFile, "NET$PROP.SYS" );
               strcpy( currDestFile, destDOSPath );
               strcat( currDestFile, "NET$PROP.SYS" );
               copyReturn = BindCopyFile( currSrcFile, currDestFile );
               if ( copyReturn == OBIND_ERR_SUCCESS )
               {
                  strcpy( currSrcFile, srcDOSPath );
                  strcat( currSrcFile, "NET$VAL.SYS" );
                  strcpy( currDestFile, destDOSPath );
                  strcat( currDestFile, "NET$VAL.SYS" );
                  copyReturn = BindCopyFile( currSrcFile, currDestFile );
                  if ( copyReturn != OBIND_ERR_SUCCESS )
                     retFlow = 6; // return the 'Error' flow
               }
               else
                  retFlow = 6; // return the 'Error' flow
            }
            else
               retFlow = 6; // return the 'Error' flow

            // open the Bindery
            ccode = nwfpNWOpenBindery( cHandle );
         }
         else if ( ccode == BINDERY_LOCKED )
         {
            retFlow = 2; // return the 'Locked' flow 
         }
         else
         {
            retFlow = 6; // return the 'Error' flow
         }
      }
      else if ( majorVersion == 2 )
      {  // if NetWare v2.x - need to RESTORE the two Bindery files
         // close the Bindery
         ccode = nwfpNWCloseBindery( cHandle );
         if ( ccode == SUCCESSFUL )
         {  // copy the required two files to the dest path
            int copyReturn;

            strcpy( currSrcFile, srcDOSPath );
            strcat( currSrcFile, "NET$BIND.SYS" );
            strcpy( currDestFile, destDOSPath );
            strcat( currDestFile, "NET$BIND.SYS" );
            copyReturn = BindCopyFile( currSrcFile, currDestFile );
            if ( copyReturn == OBIND_ERR_SUCCESS )
            {
               strcpy( currSrcFile, srcDOSPath );
               strcat( currSrcFile, "NET$BVAL.SYS" );
               strcpy( currDestFile, destDOSPath );
               strcat( currDestFile, "NET$BVAL.SYS" );
               copyReturn = BindCopyFile( currSrcFile, currDestFile );
               if ( copyReturn != OBIND_ERR_SUCCESS )
                  retFlow = 6; // return the 'Error' flow
            }
            else
               retFlow = 6; // return the 'Error' flow

            // open the Bindery
            ccode = nwfpNWOpenBindery( cHandle );
         }
         else if ( ccode == BINDERY_LOCKED )
         {
            retFlow = 2; // return the 'Locked' flow 
         }
         else
         {
            retFlow = 6; // return the 'Error' flow
         }
      }
      else if ( majorVersion > 3 )
      {  // if server is Bindery emulation
         retFlow = 4; // return the 'Not supported' flow
      }
   }

   theSystem->message1 = retFlow;
   return 0;

}  // end of RestoreBinderyFiles( )

//--------------------------------------------------------------------------
// FUNCTION:  Open Bindery
//
// DESCRIPTION:  Opens the bindery
//
// INPUT PARAMETERS
//    1: Bindery object holding input values including old object name
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Opened
//	   2: Invalid connection
//	   3: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export OpenBindery (pAEvtInfo, pAObjMessage theSystem)
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;
   NWCONN_HANDLE      cHandle;
   int                retFlow = 3;  // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   if ( oiBindery != 0 )
   {
      pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      if ( pBinderyData == NULL )
      {
         // ??? get server name and determine connection handle ???

         // Get connection handle value from Bindery object
         cHandle = OBindGetConnHandle( oiBindery, FALSE );

         ccode = nwfpNWCloseBindery( cHandle );
         switch( ccode )
         {
            case SUCCESSFUL:
               retFlow = 1; // 'Opened' flow
               break;
            case INVALID_CONNECTION:
               retFlow = 2; // 'Invalid connection' flow
               break;
            default:
               retFlow = 3; // 'Error' flow
               break;
         }
      }
   }  // end of IF Bindery object ID is valid

	theSystem->message1 = retFlow; // error

	return 0;

} // end of OpenBindery( )

//--------------------------------------------------------------------------
// FUNCTION:  Close Bindery
//
// DESCRIPTION:  Closes the bindery
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Closed
//	   2: Invalid connection
//	   3: Bindery locked
//	   4: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export CloseBindery (pAEvtInfo, pAObjMessage theSystem)
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;
   NWCONN_HANDLE      cHandle;
   int                retFlow = 4;  // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   if ( oiBindery != 0 )
   {
      pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      if ( pBinderyData == NULL )
      {
         // Get connection handle value from Bindery object
         cHandle = OBindGetConnHandle( oiBindery, FALSE );

         ccode = nwfpNWCloseBindery( cHandle );
         switch( ccode )
         {
            case SUCCESSFUL:
               retFlow = 1; // 'Closed' flow
               break;
            case INVALID_CONNECTION:
               retFlow = 2; // 'Invalid connection' flow
               break;
            case BINDERY_LOCKED:
               retFlow = 3; // 'Bindery locked' flow
               break;
            default:
               retFlow = 4; // 'Error' flow
               break;
         }
      }
   }  // end of IF Bindery object ID is valid

	theSystem->message1 = retFlow; // error

	return 0;

} // end of CloseBindery( )

// *** the following are routines from the MHS ALM files MHS-UTIL.CPP and MHS-FILE.CPP ***

//---------------------------------------------------------------------------
int BindCopyFile( char *srcfile, char *destfile )
//
// NOTE:  this function will overwite any existing destination file and
//        truncates its data.  If you want to prompt for whether the overwrt
//        should occur, do so before calling this routine !!!
//
// RETURN VALUES
// -------------
//   OBIND_ERR_SUCCESS            = file successful copied
//   OBIND_ERR_FAILED_MEM_ALLOC   = memory allocation error
//   OBIND_ERR_SRC_FILE_OPEN      = error opening source file (copy or move)
//   OBIND_ERR_SRC_FILE_READ      = error reading from source file (copy or move)
//   OBIND_ERR_DEST_FILE_OPEN     = error opening destination file (copy or move)
//   OBIND_ERR_DEST_FILE_WRITE    = error writing destination file (copy or move)
//   OBIND_ERR_BAD_SRC_FILE_SPEC  = invalid source file specification (copy or move)
//   OBIND_ERR_BAD_DEST_FILE_SPEC = invalid destination file specification (copy or move)
//---------------------------------------------------------------------------
{
  int              sourceFileHandle;
  int              targetFileHandle;
  char	          *buffer;
  int              bufferSize,
                   bytesRead,
                   bytesWritten;
  struct stat      statbuf;
  struct utimbuf   timebuf;
  int              returnCode = OBIND_ERR_SUCCESS;     // assume success

  if ( srcfile == NULL )
     return OBIND_ERR_BAD_SRC_FILE_SPEC;
  if ( strlen( srcfile ) == 0 )
     return OBIND_ERR_BAD_SRC_FILE_SPEC;
  if ( destfile == NULL )
     return OBIND_ERR_BAD_DEST_FILE_SPEC;
  if ( strlen( destfile ) == 0 )
     return OBIND_ERR_BAD_DEST_FILE_SPEC;

  sourceFileHandle = open( srcfile, O_RDONLY | O_BINARY );
  if ( sourceFileHandle < 0 )
    return OBIND_ERR_SRC_FILE_OPEN; // error opening source file

  // NOTE:  this function will overwite any existing destination file and
  //        truncates its data.  If you want to prompt for whether the overwrt
  //        should occur, do so before calling this routine !!!
  //
  // O_TRUNC effectively deletes existing file
  targetFileHandle = open( destfile, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, S_IWRITE );
  if ( targetFileHandle < 0 )
  {
    close( sourceFileHandle );
    return OBIND_ERR_DEST_FILE_OPEN; // error opening destination file
  }

	// Allocate the transfer buffer.
   bufferSize = (32 * 1024) - 1; // stay just under signed versus unsigned problem area
   do 
   {  // Try allocate the maximum size first (32K - 1 bytes)
      buffer = (char far *)malloc( (WORD)bufferSize );
      if ( buffer == NULL )               // if not enough mem
      {
         if ( bufferSize > (7*1024) )     // if buffer size is greater than 7K
            bufferSize -= 4096;           // decrease by 4K
			else                             // if unable to allocate at least 4K
         {  returnCode = OBIND_ERR_FAILED_MEM_ALLOC;  // unable to allocate memory
            goto Error0;
         }
      }
   } while ( buffer == NULL );

	// Transfer the contents of the source file to the destination file
   do
   {  // read the current buffer of data from the source file
      bytesRead = read( sourceFileHandle, buffer, bufferSize );
      if ( bytesRead == -1 )
      {  returnCode = OBIND_ERR_SRC_FILE_READ;         // error reading from source file
         goto Error0;
      }
      // Check for Zero length files, and let them get copied
 		if ( bytesRead == 0 )
		{
         if ( eof(sourceFileHandle) != 1 )
         {  // if we have Zero length file, we should be at the end of file
            returnCode = OBIND_ERR_SRC_FILE_READ;      // error reading from source file
            goto Error0;
         }
         else // we're at the end of the file
            goto DONE;
      }
      // write data to destination file
 		bytesWritten = write( targetFileHandle, buffer, bytesRead );
 		if ( bytesWritten != bytesRead )
      {  returnCode = OBIND_ERR_DEST_FILE_WRITE;       // error writing to destination file
         goto Error0;
      }
   } while ( bytesRead != 0 );

DONE:
   free( buffer );
   buffer = NULL;
   // mark destination file with the same time and date as the source file
   stat( srcfile, &statbuf );
   timebuf.modtime = statbuf.st_mtime;
   utime( destfile, &timebuf );
   close( sourceFileHandle   );
   close( targetFileHandle   );
   returnCode = OBIND_ERR_SUCCESS; // return success

Error0:
   if ( buffer )
      free( buffer );

   return returnCode; // return success

} // end of BindCopyFile( )

//------------------------------------------------------
BOOL BindDoesDirectoryExist( char far *dirName )
//------------------------------------------------------
{
   char             tempDirName[ 256 ];
   struct ffblk     findFirstData;
   int              retCode;

   strncpy( tempDirName, dirName, sizeof(tempDirName) );
   if ( tempDirName[ strlen(tempDirName) - 1 ] == '\\' )
      tempDirName[ strlen(tempDirName) - 1 ] = '\0';

   // use find first to see if directory exists
   retCode = findfirst( tempDirName, &findFirstData, FA_DIREC );
   if ( retCode == 0 )
      return TRUE;
   else
   {
      retCode = errno;
      return FALSE;
   }
} // end of BindDoesDirectoryExist( )

//------------------------------------------------------
int BindCreateDirectory( char far *dirName )
//
// RETURN VALUES
// -------------
//   OBIND_ERR_SUCCESS              = date successfully determined
//   OBIND_ERR_FAILED_DIR_CREATE    = directory creation failed
//------------------------------------------------------
{
   char    tempDirName[ 256 ];
   int     retCode;

   strncpy( tempDirName, dirName, sizeof(tempDirName) );
   if ( tempDirName[ strlen(tempDirName) - 1 ] == '\\' )
      tempDirName[ strlen(tempDirName) - 1 ] = '\0';

   retCode = mkdir( tempDirName );
   if ( retCode != 0 )
   {  // ??? it may be the case that the entire directory structure needs to be created ???
      retCode = OBIND_ERR_FAILED_DIR_CREATE;
   }
   else
      retCode = OBIND_ERR_SUCCESS;

   return retCode;

} // end of BindCreateDirectory( )


//------------------------------------------------------
BOOL BindIsNetWarePath( char far *pathSpec )
//------------------------------------------------------
{
   BOOL retCode = FALSE;

   if ( (pathSpec != NULL) && (strlen(pathSpec) > 2) )
   {  
      if ( BindIsDOSPath(pathSpec) == TRUE )
         retCode = FALSE;  // path is probably a DOS path
      else if ( pathSpec[0] == '\\' )
         retCode = FALSE;  // probably a UNC or relative path spec
      else if ( strchr(pathSpec, ':') == NULL )
         retCode = FALSE;  // probably a relative path spec
      else if ( (strchr(pathSpec, ':') != NULL) && (pathSpec[0] != ':') )
      {  // ':' is present in other than second character - path is probably NetWare fmt
         retCode = TRUE;
      }
      else if ( dynNWLib.IsNWLibLoaded( ) )
      {
         NWCCODE   nwRetCode = SUCCESSFUL;
         char      serverName[  48 ];
         char      volumeName[  17 ];
         char      dirPath[    256 ];

         serverName[0] = volumeName[0] = dirPath[0] = '\0';
         nwRetCode = nwfpNWParsePath( pathSpec,
                                      (char far *)serverName,
                                      (NWCONN_HANDLE NWFAR *)NULL,
                                      (char far *)volumeName,
                                      (char far *)dirPath  );

         if ( ((nwRetCode == SUCCESSFUL) && (serverName[0] != '\0')) ||
              ((serverName[0] != '\0') && (volumeName[0] != '\0') && (dirPath[0] != '\0')) )
         {
            retCode = TRUE;
         }
      }
   }
   else
      retCode = FALSE;

   return retCode;

} // end of BindIsNetWarePath(  )

//------------------------------------------------------
BOOL BindIsUNCPath( char far *pathSpec )
//------------------------------------------------------
{
   BOOL retCode =  FALSE;

   if ( (pathSpec != NULL) && (strlen(pathSpec) > 2) )
   {
      if ( (pathSpec[0] == '\\') && (pathSpec[1] == '\\') &&
           (strchr(pathSpec, ':') == NULL) )
      {  // we have a UNC path (first two characters are '\\' and no ':' chars)
         retCode = TRUE;
      }
   }
   return retCode;

} // end of BindIsUNCPath(  )

//------------------------------------------------------
BOOL BindIsDOSPath( char far *pathSpec )
//------------------------------------------------------
{
   BOOL retCode =  FALSE;

   if ( (isalpha(pathSpec[0])) && (pathSpec[1] == ':') )
      retCode = TRUE;

   return retCode;

} // end of BindIsDOSPath(  )

//------------------------------------------------------
BOOL BindIsRelativePath( char far *pathSpec )
//------------------------------------------------------
{
   BOOL retCode =  TRUE;

   if ( BindIsDOSPath( pathSpec ) )
     retCode = FALSE;
   else if ( BindIsUNCPath( pathSpec ) )
     retCode = FALSE;
   else if ( BindIsNetWarePath( pathSpec ) )
     retCode = FALSE;

   return retCode;

} // end of BindIsRelativePath(  )

//------------------------------------------------------
int BindConvertNetWarePathToDOS( char far *currPathSpec, char far *newPathSpec, int newBufLen )
//
// RETURN VALUES
// -------------
//   OBIND_ERR_SUCCESS                  = NetWare path spec successfully converted to DOS format
//   OBIND_ERR_BAD_DEST_BUFFER          = invalid destination buffer
//   OBIND_ERR_BAD_DEST_BUFFER_LEN      = invalid destination buffer length
//   OBIND_ERR_INCORRECT_PATH_FORMAT    = path not of specified type
//   OBIND_ERR_NETWARE_DLLS_NOT_AVAIL   = NetWare DLLs not available
//   OBIND_ERR_BAD_PATH_SPECIFIED       = input path is invalid
//   OBIND_ERR_CANT_CONVERT_TO_DOS_PATH = unable to determine DOS path
//------------------------------------------------------
{
   int             retCode = OBIND_ERR_SUCCESS;
   char            serverName[  48 ];
   char            volumeName[  17 ];
   char            dirPath[    256 ];
   NWCONN_HANDLE   nwConnHandle;

   if ( currPathSpec == NULL )
   {  // input path spec is invalid (NULL)
      retCode = OBIND_ERR_BAD_PATH_SPECIFIED;
   }
   else if ( newPathSpec == NULL )
   {  // output buffer is invalid
      retCode = OBIND_ERR_BAD_DEST_BUFFER;
   }
   else if ( dynNWLib.IsNWLibLoaded( ) )
   {  // the NetWare DLLs are available so we can make NetWare API calls
      // via the function pointers defined in NWDynLib.H
      NWCCODE   nwRetCode;

      serverName[0] = volumeName[0] = dirPath[0] = '\0';
      nwRetCode = nwfpNWParsePath( currPathSpec, (char far *)serverName,
                                   (NWCONN_HANDLE NWFAR *)&nwConnHandle,
                                   (char far *)volumeName, (char far *)dirPath );
      if ( (nwRetCode == SUCCESSFUL) ||
           ((serverName[0] != '\0') && (volumeName[0] != '\0') && (dirPath[0] != '\0')) )
      {  // check current drive mapping table for a mapping to the server\volume
         WORD     drive;
         BOOL     found = FALSE;
         char     rootPath[ 256 ];
         char     relativePath[ 256 ];
         WORD     status;

         nwRetCode = nwfpNWGetFirstDrive( (WORD NWFAR *)&drive );
         if ( nwRetCode != SUCCESSFUL )
            drive = 1; // start with drive A if we can't get the first network drive

         while( (! found) && (drive <= 26) )
         {
            nwRetCode = nwfpNWGetDriveStatus( drive, NW_FORMAT_SERVER_VOLUME,
                                              (WORD NWFAR *)&status, NULL, (char NWFAR *)rootPath,
                                              (char NWFAR *)relativePath, (char NWFAR *)NULL );
            if ( nwRetCode != NW_INVALID_DRIVE )
            {
               if ( ( ! nwRetCode) && (status & NW_NETWARE_DRIVE) )
               {   // compare root path to desired NetWare server path
                  char   drvServerName[ 48 ];
                  char   drvVolName[    17 ];
                  char   drvPath[      256 ];
                  nwRetCode = nwfpNWParsePath( rootPath, (char far *)drvServerName,
                                               (NWCONN_HANDLE NWFAR *)NULL,
                                               (char far *)drvVolName, (char far *)drvPath );
                  if ( nwRetCode == SUCCESSFUL )
                  {  // compare server name
                     if ( stricmp( serverName, drvServerName ) == 0 )
                     {  // compare volume name
                        if ( stricmp( volumeName, drvVolName ) == 0 )
                        {  // if this drive was mapped with a MAP ROOT cmd - don't use it
                           if ( (strlen(drvPath)==0) ||
                                ((strlen(drvPath)==0) && (drvPath[0]=='\\')) )
                           {  // drive mapping is not a MAP ROOT
                              found = TRUE;
                              wsprintf( newPathSpec, "%c:", (drive - 1) + 'A' );
                              if ( newBufLen < (strlen(dirPath) + 3) )
                              {  // copy as much of the path as we can into the output buffer
                                 // also indicate that the path is not complete
                                 strncat( newPathSpec, dirPath, newBufLen - 2 );
                                 retCode = OBIND_ERR_BAD_DEST_BUFFER_LEN;
                              }
                              else
                              {  // copy all of the path into the output buffer and return success
                                 strcat( newPathSpec, dirPath );
                                 retCode = OBIND_ERR_SUCCESS;
                              }
                           } // end of IF not a MAP ROOT drive mapping
                        }
                     }
                  }
               } // end IF a NetWare drive
            } // end of IF a valid drive
            ++drive;
         } // end WHILE not found and more drive letters
         if ( ! found )
            retCode = OBIND_ERR_CANT_CONVERT_TO_DOS_PATH;
      } // end of IF NWParsePath( ) was successful
      else
         retCode = OBIND_ERR_CANT_CONVERT_TO_DOS_PATH;
   } // end of ELSE IF NetWare Library DLLs are available
   else
      retCode = OBIND_ERR_NETWARE_DLLS_NOT_AVAIL;

   return retCode;

} // end of BindConvertNetWarePathToDOS( )

//------------------------------------------------------
int BindConvertUNCPathToDOS( char far *currPathSpec, char far *newPathSpec, int newBufLen )
//
// RETURN VALUES
// -------------
//   OBIND_ERR_SUCCESS                  = UNC path spec successfully converted to DOS format
//   OBIND_ERR_BAD_DEST_BUFFER          = invalid destination buffer
//   OBIND_ERR_BAD_DEST_BUFFER_LEN      = invalid destination buffer length
//   OBIND_ERR_INCORRECT_PATH_FORMAT    = path not of specified type
//   OBIND_ERR_NETWARE_DLLS_NOT_AVAIL   = NetWare DLLs not available
//   OBIND_ERR_BAD_PATH_SPECIFIED       = input path is invalid
//   OBIND_ERR_CANT_CONVERT_TO_DOS_PATH = unable to determine DOS path
//------------------------------------------------------
{
   int             retCode = OBIND_ERR_SUCCESS;
   char            serverName[  48 ];
   char            volumeName[  17 ];
   char            dirPath[    256 ];
   char            nwPath[     256 ];
   NWCONN_HANDLE   nwConnHandle;

   if ( currPathSpec == NULL )
   {  // input path spec is invalid (NULL)
      retCode = OBIND_ERR_BAD_PATH_SPECIFIED;
   }
   else if ( newPathSpec == NULL )
   {  // output buffer specified is invalid
      retCode = OBIND_ERR_BAD_DEST_BUFFER;
   }
   else if ( dynNWLib.IsNWLibLoaded( ) )
   {
      NWCCODE      nwRetCode = SUCCESSFUL;

      serverName[0] = volumeName[0] = dirPath[0] = nwPath[0] = '\0';
      nwRetCode = nwfpNWParseUNCPath( currPathSpec,
                                      (NWCONN_HANDLE NWFAR *)&nwConnHandle,
                                      (char far *)serverName,
                                      (char far *)volumeName,
                                      (char far *)dirPath,
                                      (char far *)nwPath  );

      if ( (nwRetCode == SUCCESSFUL) ||
           ((serverName[0] != '\0') && (volumeName[0] != '\0') && (dirPath[0] != '\0')) )
      {  // check current drive mapping table for a mapping to the server\volume
         WORD     drive;
         BOOL     found = FALSE;
         char     rootPath[ 256 ];
         char     relativePath[ 256 ];
         WORD     status;

         nwRetCode = nwfpNWGetFirstDrive( (WORD NWFAR *)&drive );
         if ( nwRetCode != SUCCESSFUL )
            drive = 1; // start with drive A if we can't get the first network drive

         while( (! found) && (drive <= 26) )
         {
            nwRetCode = nwfpNWGetDriveStatus( drive, NW_FORMAT_SERVER_VOLUME,
                                              (WORD NWFAR *)&status, NULL, (char NWFAR *)rootPath,
                                              (char NWFAR *)relativePath, (char NWFAR *)NULL );
            if ( nwRetCode != NW_INVALID_DRIVE )
            {
               if ( ( ! nwRetCode) && (status & NW_NETWARE_DRIVE) )
               {   // compare root path to desired NetWare server path
                  char   drvServerName[ 48 ];
                  char   drvVolName[    17 ];
                  char   drvPath[      256 ];
                  nwRetCode = nwfpNWParsePath( rootPath, (char far *)drvServerName,
                                               (NWCONN_HANDLE NWFAR *)NULL,
                                               (char far *)drvVolName, (char far *)drvPath );
                  if ( nwRetCode == SUCCESSFUL )
                  {  // compare server name
                     if ( stricmp( serverName, drvServerName ) == 0 )
                     {  // compare volume name
                        if ( stricmp( volumeName, drvVolName ) == 0 )
                        {  // if this drive was mapped with a MAP ROOT cmd - don't use it
                           if ( (strlen(drvPath) == 0) ||
                                ((strlen(drvPath) == 0) && (drvPath[0]=='\\')) )
                           {  // drive mapping is not a MAP ROOT
                              found = TRUE;
                              wsprintf( newPathSpec, "%c:", (drive - 1) + 'A' );
                              if ( newBufLen < (strlen(dirPath) + 3) )
                              {  // copy as much of the path as we can into the output buffer
                                 // also indicate that the path is not complete
                                 strncat( newPathSpec, dirPath, newBufLen - 2 );
                                 retCode = OBIND_ERR_BAD_DEST_BUFFER_LEN;
                              }
                              else
                              {  // copy all of the path into the output buffer and return success
                                 strcat( newPathSpec, dirPath );
                                 retCode = OBIND_ERR_SUCCESS;
                              }
                           } // end of IF not a MAP ROOT drive mapping
                        }
                     }
                  }
               } // end IF a NetWare drive
            } // end of IF a valid drive
            ++drive;
         } // end WHILE not found and more drive letters
         if ( ! found )
            retCode = OBIND_ERR_CANT_CONVERT_TO_DOS_PATH;
      } // end of IF NWParsePath( ) was successful
      else
         retCode = OBIND_ERR_CANT_CONVERT_TO_DOS_PATH;
   } // end of ELSE IF NetWare Library DLLs are available
   else
      retCode = OBIND_ERR_NETWARE_DLLS_NOT_AVAIL;

   return retCode;

} // end of BindConvertUNCPathToDOS( )
