//-----------------------------------------------------------------
// NWDynLib.CPP
//
//    C++ class to support dynamic binding to NetWare C Library
//    funtions.
//
//  (c) 1994 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//-----------------------------------------------------------------

#include <windows.h>

// place the various NO... #defines here (before include NWDynLib.H) to omit certain
// categories of NetWare API calls
//#define NOREDIR_INC
//#define NOMISC_INC
//#define NOCONNECT_INC
//#define NODPATH_INC
//#define NOFILES_INC
//#define NOAFP_INC
//#define NOBINDRY_INC
//#define NODEL_INC
//#define NODENTRY_INC
//#define NODIRECTORY_INC
//#define NOEA_INC
//#define NOERROR_INC
//#define NOMESSAGES_INC
//#define NONAMSPC_INC
//#define NOPRINT_INC
//#define NOQUEUE_INC
//#define NOSERVER_INC
//#define NOSYNC_INC
//#define NONTTS_INC
//#define NOVOL_INC
//#define NOACCT_INC
//#define NOFSE_INC
//#define NOMIGRATE_INC

#include "NWDynLib.h"

// define the NetWare API class
NetWareLib   dynNWLib;

//-----------------------------------------------------------------------
// define variables to contain the function ptrs for the functions used
//-----------------------------------------------------------------------
// NWCallsInit( )
NWCCODE NWAPI (*nwfpNWCallsInit)( void NWFAR *, void NWFAR * );

// __NWGetNWCallsState( )
WORD NWAPI (*nwfp__NWGetNWCallsState)( void );

#ifndef NOREDIR_INC
//#include <nwredir.h>
// NWParseUNCPath( )
NWCCODE NWAPI (*nwfpNWParseUNCPath)(  char NWFAR *, NWCONN_HANDLE NWFAR *,
                                      char NWFAR *, char NWFAR *,
                                      char NWFAR *, char NWFAR *  );
#endif

//**************************************
//* from Chapter 16 - Misc Services
//**************************************
#ifndef NOMISC_INC
//#include <nwmisc.h>

// NWGetRequesterVersion( )
NWCCODE NWAPI (*nwfpNWGetRequesterVersion)( NWREQUESTER_VERSION NWFAR *,
                                            NWREQUESTER_VERSION NWFAR *,
                                            NWREQUESTER_VERSION NWFAR * );
// NWWordSwap( )
WORD NWAPI (*nwfpNWWordSwap)( WORD );
#endif

//***********************************************
//* Chapter 6 - Connection Services (nwconnec.h)
//***********************************************
#ifndef NOCONNECT_INC
//#include <nwconnec.h>
// NWGetConnectionHandle( )
NWCCODE NWAPI (*nwfpNWGetConnectionHandle)( BYTE NWFAR *, NWLOCAL_MODE,
                                                   NWCONN_HANDLE NWFAR *, NWLOCAL_SCOPE NWFAR * );
// NWGetConnectionInformation( )
NWCCODE NWAPI (*nwfpNWGetConnectionInformation)( NWCONN_HANDLE, NWCONN_NUM,
                                                        char NWFAR *, NWOBJ_TYPE NWFAR *,
                                                        NWOBJ_ID NWFAR *, BYTE NWFAR *     );
// NWGetConnectionList( )
NWCCODE NWAPI (*nwfpNWGetConnectionList)( NWLOCAL_MODE, NWCONN_HANDLE NWFAR *,
                                                 NWSTRUCT_SIZE, NWNUMBER NWFAR * );
// NWGetConnectionStatus( )
NWCCODE NWAPI (*nwfpNWGetConnectionStatus)( NWCONN_HANDLE, CONNECT_INFO NWFAR *,
                                                   NWSTRUCT_SIZE );
// NWGetDefaultConnectionID( )
NWCCODE NWAPI (*nwfpNWGetDefaultConnectionID)( NWCONN_HANDLE NWFAR * );

// NWGetMaximumConnections( )
NWCCODE NWAPI (*nwfpNWGetMaximumConnections)( NWNUMBER NWFAR * );
#endif


//********************************************
//* from Chapter 19 - Path and Drive Services
//********************************************
#ifndef NODPATH_INC
//#include <nwdpath.h>
// NWGetDriveStatus( )
NWCCODE NWAPI (*nwfpNWGetDriveStatus)( WORD, WORD, WORD NWFAR *, NWCONN_HANDLE NWFAR *,
                                        char NWFAR *, char NWFAR *, char NWFAR * );
// NWGetFirstDrive( )
NWCCODE NWAPI (*nwfpNWGetFirstDrive)( WORD NWFAR * );

// NWParseNetWarePath( )
NWCCODE NWAPI (*nwfpNWParseNetWarePath)( char NWFAR *, NWCONN_HANDLE NWFAR *,
                                          NWDIR_HANDLE NWFAR *, char NWFAR * );
// NWParsePath( )
NWCCODE NWAPI (*nwfpNWParsePath)( char NWFAR *, char NWFAR *, NWCONN_HANDLE NWFAR *,
                                 char NWFAR *, char NWFAR * );
// NWStripServerOffPath( )
NWCCODE NWAPI (*nwfpNWStripServerOffPath)( char NWFAR *, char NWFAR * );
#endif


//******************************************
//* from Chapter 13 - File Systems Services
//******************************************
#ifndef NOFILES_INC
//#include <nwfile.h>
// NWFileServerFileCopy( )
NWCCODE NWAPI (*nwfpNWFileServerFileCopy)( NWLOCAL_FILE_HANDLE, NWLOCAL_FILE_HANDLE, NWOFFSET,
                                           NWOFFSET, NWNUM, NWNUM NWFAR * );
// NWRenameFile( )
NWCCODE NWAPI (*nwfpNWRenameFile)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                   NWSEARCH_ATTR, NWDIR_HANDLE, char NWFAR * );
// NWEraseFiles( )
NWCCODE NWAPI (*nwfpNWEraseFiles)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                   NWSEARCH_ATTR );
// NWFileSearchInitialize( )
NWCCODE NWAPI (*nwfpNWFileSearchInitialize)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                             NWPATH_VOL NWFAR *, NWDIR_NUM NWFAR *,
                                             NWSEQUENCE_NUM NWFAR *, NWACCESS_RIGHTS NWFAR * );
// NWFileSearchContinue( )
NWCCODE NWAPI (*nwfpNWFileSearchContinue)( NWCONN_HANDLE, NWPATH_VOL, NWDIR_NUM,
                                           NWSEARCH_CONTEXT, NWSEARCH_ATTR,
                                           char NWFAR *, char NWFAR * );
// NWCreateDirectory( )
NWCCODE NWAPI (*nwfpNWCreateDirectory)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                        NWACCESS_RIGHTS );
#endif

// *** other conditional sections from NWCALLS.H

//#ifndef NOAFP_INC
//#include <nwafp.h>
//#endif

#ifndef NOBINDRY_INC
#include <nwbindry.h>
NWCCODE NWAPI (*nwfpNWVerifyObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR password);

NWCCODE NWAPI (*nwfpNWDisallowObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR disallowedPassword);

NWCCODE NWAPI (*nwfpNWChangeObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR oldPassword,
  char NWPTR newPassword);

NWCCODE NWAPI (*nwfpNWReadPropertyValue)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE segmentNumber,
  BYTE NWPTR segmentData,
  BYTE NWPTR moreSegments,
  BYTE NWPTR flags);

NWCCODE NWAPI (*nwfpNWWritePropertyValue)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE segmentNumber,
  BYTE NWPTR segmentData,
  BYTE moreSegments);

NWCCODE NWAPI (*nwfpNWAddObjectToSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

NWCCODE NWAPI (*nwfpNWDeleteObjectFromSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

NWCCODE NWAPI (*nwfpNWIsObjectInSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

NWCCODE NWAPI (*nwfpNWScanProperty)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR searchPropertyName,
  DWORD NWPTR sequenceNumber,
  char NWPTR propertyName,
  BYTE NWPTR propertyFlags,
  BYTE NWPTR propertySecurity,
  BYTE NWPTR hasValueFlag,
  BYTE NWPTR moreFlag);

NWCCODE NWAPI (*nwfpNWGetObjectID)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  DWORD NWPTR objectID);

NWCCODE NWAPI (*nwfpNWGetObjectDiskSpaceLeft)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  DWORD NWPTR systemElapsedTime,
  DWORD NWPTR unusedDiskBlocks,
  BYTE NWPTR restrictionEnforced);

NWCCODE NWAPI (*nwfpNWGetObjectName)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  char NWPTR objectName,
  WORD NWPTR objectType);

NWCCODE NWAPI (*nwfpNWScanObject)(
  NWCONN_HANDLE conn,
  char NWPTR searchName,
  WORD searchType,
  DWORD NWPTR objectID,
  char NWPTR objectName,
  WORD NWPTR objectType,
  BYTE NWPTR hasPropertiesFlag,
  BYTE NWPTR objectFlags,
  BYTE NWPTR objectSecurity);

NWCCODE NWAPI (*nwfpNWGetBinderyAccessLevel)(
  NWCONN_HANDLE conn,
  BYTE NWPTR accessLevel,
  DWORD NWPTR objectID);

NWCCODE NWAPI (*nwfpNWCreateProperty)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE propertyFlags,
  BYTE propertySecurity);

NWCCODE NWAPI (*nwfpNWDeleteProperty)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName);

NWCCODE NWAPI (*nwfpNWChangePropertySecurity)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE newPropertySecurity);

NWCCODE NWAPI (*nwfpNWCreateObject)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  BYTE objectFlags,
  BYTE objectSecurity);

NWCCODE NWAPI (*nwfpNWDeleteObject)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType);

NWCCODE NWAPI (*nwfpNWRenameObject)(
  NWCONN_HANDLE conn,
  char NWPTR oldObjectName,
  char NWPTR newObjectName,
  WORD objectType);

NWCCODE NWAPI (*nwfpNWChangeObjectSecurity)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  BYTE newObjectSecurity);

NWCCODE NWAPI (*nwfpNWOpenBindery)( NWCONN_HANDLE conn );
NWCCODE NWAPI (*nwfpNWCloseBindery)( NWCONN_HANDLE conn );

NWCCODE NWAPI (*nwfpNWScanObjectTrusteePaths)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  WORD volNumber,
  WORD NWPTR sequenceNumber,
  BYTE NWPTR accessRights,
  char NWPTR dirPath );

NWCCODE NWAPI (*nwfpNWGetObjectEffectiveRights)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  NWDIR_HANDLE dirHandle,
  char NWPTR path,
  WORD NWPTR rightsMask );
#endif

//#ifndef NODEL_INC
//#include <nwdel.h>
//#endif

//#ifndef NODENTRY_INC
//#include <nwdentry.h>
//#endif

//#ifndef NODIRECTORY_INC
//#include <nwdirect.h>
//#endif

//#ifndef NOEA_INC
//#include <nwea.h>
//#endif

//#ifndef NOERROR_INC
//#include <nwerror.h>
//#endif

//#ifndef NOMESSAGES_INC
//#include <nwmsg.h>
//#endif

//#ifndef NONAMSPC_INC
//#include <nwnamspc.h>
//#endif

//#ifndef NOPRINT_INC
//#include <nwprint.h>
//#endif

//#ifndef NOQUEUE_INC
//#include <nwqms.h>
//#endif

#ifndef NOSERVER_INC
#include <nwserver.h>
NWCCODE NWAPI (*nwfpNWGetFileServerInformation)(
  NWCONN_HANDLE conn,
  char NWPTR serverName,
  BYTE NWPTR majorVersion,
  BYTE NWPTR minorVersion,
  BYTE NWPTR revision,
  WORD NWPTR maxConnections,
  WORD NWPTR maxConnectionsUsed,
  WORD NWPTR connectionsInUse,
  WORD NWPTR numVolumes,
  BYTE NWPTR SFTLevel,
  BYTE NWPTR TTSLevel);

NWCCODE NWAPI (*nwfpNWIsManager)( NWCONN_HANDLE conn );
#endif

//#ifndef NOSYNC_INC
//#include <nwsync.h>
//#endif

//#ifndef NONTTS_INC
//#include <nwtts.h>
//#endif

#ifndef NOVOL_INC
#include <nwvol.h>
NWCCODE NWAPI (*nwfpNWGetDiskUtilization)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  BYTE volumeNumber,
  WORD NWPTR usedDirectories,
  WORD NWPTR usedFiles,
  WORD NWPTR usedBlocks);

NWCCODE NWAPI (*nwfpNWGetObjDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE  volNumber,
  DWORD objectID,
  DWORD NWPTR restriction,
  DWORD NWPTR inUse);

NWCCODE NWAPI (*nwfpNWScanVolDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE volNumber,
  DWORD NWPTR sequence,
  NWVolumeRestrictions NWPTR volInfo);

NWCCODE NWAPI (*nwfpNWScanVolDiskRestrictions2)(
  NWCONN_HANDLE conn,
  BYTE volNum,
  DWORD NWPTR seq,
  NWVOL_RESTRICTIONS NWPTR volInfo);

NWCCODE NWAPI (*nwfpNWRemoveObjectDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE volNumber,
  DWORD objectID);

NWCCODE NWAPI (*nwfpNWSetObjectVolSpaceLimit)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  DWORD objectID,
  DWORD restriction);

NWCCODE NWAPI (*nwfpNWGetVolumeInfoWithHandle)(
  NWCONN_HANDLE conn,
  NWDIR_HANDLE dirHandle,
  char NWPTR volName,
  WORD NWPTR totalBlocks,
  WORD NWPTR sectorsPerBlock,
  WORD NWPTR availableBlocks,
  WORD NWPTR totalDirEntries,
  WORD NWPTR availableDirEntries,
  WORD NWPTR volIsRemovableFlag);

NWCCODE NWAPI (*nwfpNWGetVolumeInfoWithNumber)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  char NWPTR volName,
  WORD NWPTR totalBlocks,
  WORD NWPTR sectorsPerBlock,
  WORD NWPTR availableBlocks,
  WORD NWPTR totalDirEntries,
  WORD NWPTR availableDirEntries,
  WORD NWPTR volIsRemovableFlag);

NWCCODE NWAPI (*nwfpNWGetVolumeName)(
  NWCONN_HANDLE conn,
  WORD volNum,
  char NWPTR volName);

NWCCODE NWAPI (*nwfpNWGetVolumeNumber)(
  NWCONN_HANDLE conn,
  char NWPTR volName,
  WORD NWPTR volNumber);

NWCCODE NWAPI (*nwfpNWGetVolumeStats)(
  NWCONN_HANDLE conn,
  BYTE volumeNumber,
  VOL_STATS NWPTR volInfo );

NWCCODE NWAPI (*nwfpNWGetExtendedVolumeInfo)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  NWVolExtendedInfo NWPTR volInfo);
#endif

//#ifndef NOACCT_INC
//#include <nwacct.h>
//#endif

//#ifndef NOFSE_INC
//#include <nwfse.h>
//#endif

//#ifndef NOMIGRATE_INC
//#include <nwmigrat.h>
//#endif

#ifdef ZZZ_FUTURE_SECTIONS
   // NWGetFileServerName( )
   // NWGetFileServerVersion( )
   // NWGetFileServerVersionInfo( )

   // Unicode services

   // Internationalization services
#endif


//----------------------------------
BOOL NetWareLib::FreeLib( void )
//----------------------------------
{
   if ( isHandle )
   {  isHandle = FALSE;
      FreeLibrary( handle );
   }
   allFuncPtrsSet = FALSE;
   return FALSE;
}

//----------------------------------
NetWareLib::NetWareLib( void )
//----------------------------------
{
   isHandle = FALSE;
   version = 0;
   allFuncPtrsSet = FALSE;
}

//----------------------------------
NetWareLib::~NetWareLib( void )
//----------------------------------
{
   FreeLib( );
}

//----------------------------------
short NetWareLib::Version( void )
//----------------------------------
{
   return version;
}

//----------------------------------
BOOL NetWareLib::IsNWLibLoaded( void )
//----------------------------------
{
   if ( (isHandle == TRUE) && (allFuncPtrsSet == TRUE) )
      return TRUE;
   else
      return FALSE;
}

//----------------------------------
BOOL NetWareLib::LoadNWLib( void )
//----------------------------------
{
   NWCCODE   nwReturnCode;
   WORD      nwCallsState;

   if ( isHandle )
      return TRUE;

   // ??? what about NWNET.DLL and the other potential NW DLLs req'd

   if ( (handle = LoadLibrary( "NWCALLS.DLL" ) ) < (HINSTANCE)32 )
   {  // ??? try to determine if file not found or no network loaded
      MessageBox( GetActiveWindow(), "Unable to Load NWCalls.DLL",
                  NULL, MB_OK );
      return FALSE;
   }
   else
      version = 1;  // ??? how to get the version information from the
                    // DLL - NOTE: do not use date information

   isHandle = TRUE;

   //---------------------------------------------------
   // call GetProcAddress( ) for each desired function
   //---------------------------------------------------
      // if any fail - call FreeLib( ) member function

   // NWCallsInit( )
   if ( NULL == (nwfpNWCallsInit = (NWCCODE NWAPI (*)( void NWFAR *, void NWFAR * ))
                  GetProcAddress( handle, "NWCallsInit" )
          )
      )
      return FreeLib( );


   // NWCallsInit( )
   if ( nwfpNWCallsInit != NULL )
   {
      nwReturnCode = nwfpNWCallsInit( (void NWFAR *)NULL, (void NWFAR *)NULL );
      if ( nwReturnCode != SUCCESSFUL )
      {  // error - don't bother doing any more
         return FreeLib( );
      }
   }
   else
   {  // error - don't bother doing any more
      return FreeLib( );
   }


   // __NWCallsState( )
   if ( NULL == (nwfp__NWGetNWCallsState = (WORD NWAPI (*)( void ))
                  GetProcAddress( handle, "__NWGetNWCallsState" )
          )
      )
      return FreeLib( );

   // call __NWGetNWCallsState( ) now and check return code - if neither VLM or SHELL is loaded
   // don't bother to acquire function ptrs
   if ( nwfp__NWGetNWCallsState != NULL )
   {
      nwCallsState = nwfp__NWGetNWCallsState( );
      if ( (!(nwCallsState & _NETX_COM)) && (!(nwCallsState & _VLM_LOADED)) )
      {  // error - don't bother doing any more
         // neither the NETX shell nor the VLMs are loaded !
         return FreeLib( );
      }
   }
   else
   {  // error - don't bother doing any more
      return FreeLib( );
   }


#ifndef NOREDIR_INC
   if ( NULL == (nwfpNWParseUNCPath = (NWCCODE NWAPI (*)( char NWFAR *, NWCONN_HANDLE NWFAR *,
                                                   char NWFAR *, char NWFAR *,
                                                   char NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWParseUNCPath" )
          )
      )
      return FreeLib( );
#endif


#ifndef NOMISC_INC
   // NWGetRequesterVersion( )
   if ( NULL == (nwfpNWGetRequesterVersion = (NWCCODE NWAPI (*)( NWREQUESTER_VERSION NWFAR *,
                                                    NWREQUESTER_VERSION NWFAR *,
                                                    NWREQUESTER_VERSION NWFAR * ))
                  GetProcAddress( handle, "NWGetRequesterVersion" )
          )
      )
      return FreeLib( );

   // NWWordSwap( )
   if ( NULL == (nwfpNWWordSwap = (WORD NWAPI (*)( WORD ))
                  GetProcAddress( handle, "NWWordSwap" )
          )
      )
      return FreeLib( );

#endif

#ifndef NOCONNECT_INC
   //***********************************************
   //* Chapter 6 - Connection Services (nwconnec.h)
   //***********************************************
   // NWGetConnectionHandle( )
   if ( NULL == (nwfpNWGetConnectionHandle = (NWCCODE NWAPI (*)( BYTE NWFAR *, NWLOCAL_MODE,
                                               NWCONN_HANDLE NWFAR *, NWLOCAL_SCOPE NWFAR * ))
                  GetProcAddress( handle, "NWGetConnectionHandle" )
          )
      )
      return FreeLib( );

   // NWGetConnectionInformation( )
   if ( NULL == (nwfpNWGetConnectionInformation = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWCONN_NUM,
                                                    char NWFAR *, NWOBJ_TYPE NWFAR *,
                                                    NWOBJ_ID NWFAR *, BYTE NWFAR * ))
                  GetProcAddress( handle, "NWGetConnectionInformation" )
          )
      )
      return FreeLib( );

   // NWGetConnectionList( )
   if ( NULL == (nwfpNWGetConnectionList = (NWCCODE NWAPI (*)( NWLOCAL_MODE, NWCONN_HANDLE NWFAR *,
                                             NWSTRUCT_SIZE, NWNUMBER NWFAR * ))
                  GetProcAddress( handle, "NWGetConnectionList" )
          )
      )
      return FreeLib( );

   // NWGetConnectionStatus( )
   if ( NULL == (nwfpNWGetConnectionStatus = (NWCCODE NWAPI (*)( NWCONN_HANDLE, CONNECT_INFO NWFAR *,
                                               NWSTRUCT_SIZE ))
                  GetProcAddress( handle, "NWGetConnectionStatus" )
          )
      )
      return FreeLib( );

   // NWGetDefaultConnectionID( )
   if ( NULL == (nwfpNWGetDefaultConnectionID = (NWCCODE NWAPI (*)( NWCONN_HANDLE NWFAR * ))
                  GetProcAddress( handle, "NWGetDefaultConnectionID" )
          )
      )
      return FreeLib( );

   // NWGetMaximumConnections( )
   if ( NULL == (nwfpNWGetMaximumConnections = (NWCCODE NWAPI (*)( NWNUMBER NWFAR * ))
                  GetProcAddress( handle, "NWGetMaximumConnections" )
          )
      )
      return FreeLib( );
#endif

#ifndef NODPATH_INC
   // NWGetDriveStatus( )
   if ( NULL == (nwfpNWGetDriveStatus = (NWCCODE NWAPI (*)( WORD, WORD, WORD NWFAR *, NWCONN_HANDLE NWFAR *,
                                              char NWFAR *, char NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWGetDriveStatus" )
          )
      )
      return FreeLib( );

   // NWGetFirstDrive( )
   if ( NULL == (nwfpNWGetFirstDrive = (NWCCODE NWAPI (*)( WORD NWFAR * ))
                  GetProcAddress( handle, "NWGetFirstDrive" )
          )
      )
      return FreeLib( );

   // NWParseNetWarePath( )
   if ( NULL == (nwfpNWParseNetWarePath = (NWCCODE NWAPI (*)( char NWFAR *, NWCONN_HANDLE NWFAR *,
                                                NWDIR_HANDLE NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWParseNetWarePath" )
          )
      )
      return FreeLib( );

   // NWParsePath( )
   if ( NULL == (nwfpNWParsePath = (NWCCODE NWAPI (*)( char NWFAR *, char NWFAR *, NWCONN_HANDLE NWFAR *,
                                         char NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWParsePath" )
          )
      )
      return FreeLib( );

   // NWStripServerOffPath( )
   if ( NULL == (nwfpNWStripServerOffPath = (NWCCODE NWAPI (*)( char NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWStripServerOffPath" )
          )
      )
      return FreeLib( );
#endif


#ifndef NOFILES_INC
   // NWRenameFile( )
   if ( NULL == (nwfpNWRenameFile = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                           NWSEARCH_ATTR, NWDIR_HANDLE, char NWFAR * ))
                  GetProcAddress( handle, "NWRenameFile" )
          )
      )
      return FreeLib( );

   // NWEraseFiles( )
   if ( NULL == (nwfpNWEraseFiles = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                           NWSEARCH_ATTR ))
                  GetProcAddress( handle, "NWEraseFiles" )
          )
      )
      return FreeLib( );

   // NWFileServerFileCopy( )
   if ( NULL == (nwfpNWFileServerFileCopy = (NWCCODE NWAPI (*)( NWLOCAL_FILE_HANDLE, NWLOCAL_FILE_HANDLE,
                                                   NWOFFSET, NWOFFSET,
                                                   NWNUM, NWNUM NWFAR * ))
                  GetProcAddress( handle, "NWFileServerFileCopy" )
          )
      )
      return FreeLib( );

   // NWFileSearchInitialize( )
   if ( NULL == (nwfpNWFileSearchInitialize = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                                     NWPATH_VOL NWFAR *, NWDIR_NUM NWFAR *,
                                                     NWSEQUENCE_NUM NWFAR *, NWACCESS_RIGHTS NWFAR * ))
                  GetProcAddress( handle, "NWFileSearchInitialize" )
          )
      )
      return FreeLib( );

   // NWFileSearchContinue( )
   if ( NULL == (nwfpNWFileSearchContinue = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWPATH_VOL, NWDIR_NUM,
                                                   NWSEARCH_CONTEXT, NWSEARCH_ATTR,
                                                   char NWFAR *, char NWFAR * ))
                  GetProcAddress( handle, "NWFileSearchContinue" )
          )
      )
      return FreeLib( );

   // NWCreateDirectory( )
   if ( NULL == (nwfpNWCreateDirectory = (NWCCODE NWAPI (*)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                           NWACCESS_RIGHTS ))
                  GetProcAddress( handle, "NWCreateDirectory" )
          )
      )
      return FreeLib( );
#endif

#ifndef NOBINDRY_INC
   // NWVerifyObjectPassword( )
   if ( NULL == (nwfpNWVerifyObjectPassword = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                                char NWPTR, WORD, char NWPTR ))
                  GetProcAddress( handle, "NWVerifyObjectPassword" )
          )
      )
      return FreeLib( );

   // NWDisallowObjectPassword( )
   if ( NULL == (nwfpNWDisallowObjectPassword = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                              char NWPTR, WORD, char NWPTR ))
                  GetProcAddress( handle, "NWDisallowObjectPassword" )
          )
      )
      return FreeLib( );

   // NWChangeObjectPassword( )
   if ( NULL == (nwfpNWChangeObjectPassword = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, WORD, char NWPTR, char NWPTR ))
                  GetProcAddress( handle, "NWChangeObjectPassword" )
          )
      )
      return FreeLib( );

   // NWReadPropertyValue( )
   if ( NULL == (nwfpNWReadPropertyValue = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                                char NWPTR, WORD, char NWPTR, BYTE,
                                BYTE NWPTR, BYTE NWPTR, BYTE NWPTR ))
                  GetProcAddress( handle, "NWReadPropertyValue" )
          )
      )
      return FreeLib( );

   // NNWWritePropertyValue( )
   if ( NULL == (nwfpNWWritePropertyValue = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, WORD, char NWPTR, BYTE, BYTE NWPTR, BYTE ))
                  GetProcAddress( handle, "NWWritePropertyValue" )
          )
      )
      return FreeLib( );

   // NWAddObjectToSet( )
   if ( NULL == (nwfpNWAddObjectToSet = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                          char NWPTR, WORD, char NWPTR, char NWPTR, WORD ))
                  GetProcAddress( handle, "NWAddObjectToSet" )
          )
      )
      return FreeLib( );

   // NWDeleteObjectFromSet( )
   if ( NULL == (nwfpNWDeleteObjectFromSet = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                      char NWPTR, WORD, char NWPTR, char NWPTR, WORD ))
                  GetProcAddress( handle, "NWDeleteObjectFromSet" )
          )
      )
      return FreeLib( );

   // NWIsObjectInSet( )
   if ( NULL == (nwfpNWIsObjectInSet = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                        char NWPTR, WORD, char NWPTR, char NWPTR, WORD ))
                  GetProcAddress( handle, "NWIsObjectInSet" )
          )
      )
      return FreeLib( );

   // NWScanProperty( )
   if ( NULL == (nwfpNWScanProperty = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, WORD, char NWPTR, DWORD NWPTR, char NWPTR,
                         BYTE NWPTR, BYTE NWPTR, BYTE NWPTR, BYTE NWPTR ))
                  GetProcAddress( handle, "NWScanProperty" )
          )
      )
      return FreeLib( );

   // NWGetObjectID( )
   if ( NULL == (nwfpNWGetObjectID = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                           char NWPTR, WORD, DWORD NWPTR ))
                  GetProcAddress( handle, "NWGetObjectID" )
          )
      )
      return FreeLib( );

   // NWGetObjectDiskSpaceLeft( )
   if ( NULL == (nwfpNWGetObjectDiskSpaceLeft = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                        DWORD, DWORD NWPTR, DWORD NWPTR, BYTE NWPTR ))
                  GetProcAddress( handle, "NWGetObjectDiskSpaceLeft" )
          )
      )
      return FreeLib( );

   // NWGetObjectName( )
   if ( NULL == (nwfpNWGetObjectName = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         DWORD, char NWPTR, WORD NWPTR ))
                  GetProcAddress( handle, "NWGetObjectName" )
          )
      )
      return FreeLib( );

   // NWScanObject( )
   if ( NULL == (nwfpNWScanObject = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, WORD, DWORD NWPTR, char NWPTR,
                         WORD NWPTR, BYTE NWPTR, BYTE NWPTR, BYTE NWPTR ))
                  GetProcAddress( handle, "NWScanObject" )
          )
      )
      return FreeLib( );

   // NWGetBinderyAccessLevel( )
   if ( NULL == (nwfpNWGetBinderyAccessLevel = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                          BYTE NWPTR, DWORD NWPTR ))
                  GetProcAddress( handle, "NWGetBinderyAccessLevel" )
          )
      )
      return FreeLib( );

   // NWCreateProperty( )
   if ( NULL == (nwfpNWCreateProperty = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                      char NWPTR, WORD, char NWPTR, BYTE, BYTE ))
                  GetProcAddress( handle, "NWCreateProperty" )
          )
      )
      return FreeLib( );

   // NWDeleteProperty( )
   if ( NULL == (nwfpNWDeleteProperty = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                          char NWPTR, WORD, char NWPTR ))
                  GetProcAddress( handle, "NWDeleteProperty" )
          )
      )
      return FreeLib( );

   // NWChangePropertySecurity( )
   if ( NULL == (nwfpNWChangePropertySecurity = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                      char NWPTR, WORD, char NWPTR, BYTE ))
                  GetProcAddress( handle, "NWChangePropertySecurity" )
          )
      )
      return FreeLib( );

   // NWCreateObject( )
   if ( NULL == (nwfpNWCreateObject = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, WORD, BYTE, BYTE ))
                  GetProcAddress( handle, "NWCreateObject" )
          )
      )
      return FreeLib( );

   // NWDeleteObject( )
   if ( NULL == (nwfpNWDeleteObject = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                        char NWPTR, WORD ))
                  GetProcAddress( handle, "NWDeleteObject" )
          )
      )
      return FreeLib( );

   // NWRenameObject( )
   if ( NULL == (nwfpNWRenameObject = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                          char NWPTR, char NWPTR, WORD ))
                  GetProcAddress( handle, "NWRenameObject" )
          )
      )
      return FreeLib( );

   // NWChangeObjectSecurity( )
   if ( NULL == (nwfpNWChangeObjectSecurity = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                              char NWPTR, WORD, BYTE ))
                  GetProcAddress( handle, "NWChangeObjectSecurity" )
          )
      )
      return FreeLib( );

   // NWOpenBindery( )
   if ( NULL == (nwfpNWOpenBindery = (NWCCODE NWAPI (*)( NWCONN_HANDLE ))
                  GetProcAddress( handle, "NWOpenBindery" )
          )
      )
      return FreeLib( );

   // NWCloseBindery( )
   if ( NULL == (nwfpNWCloseBindery = (NWCCODE NWAPI (*)( NWCONN_HANDLE ))
                  GetProcAddress( handle, "NWCloseBindery" )
          )
      )
      return FreeLib( );

   // NWScanObjectTrusteePaths( )
   if ( NULL == (nwfpNWScanObjectTrusteePaths = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         DWORD, WORD, WORD NWPTR, BYTE NWPTR, char NWPTR ))
                  GetProcAddress( handle, "NWScanObjectTrusteePaths" )
          )
      )
      return FreeLib( );

   // NWGetObjectEffectiveRights( )
   if ( NULL == (nwfpNWGetObjectEffectiveRights = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         DWORD, NWDIR_HANDLE, char NWPTR, WORD NWPTR ))
                  GetProcAddress( handle, "NWGetObjectEffectiveRights" )
          )
      )
      return FreeLib( );
#endif

#ifndef NOSERVER_INC
   // NWGetFileServerInformation( )
   if ( NULL == (nwfpNWGetFileServerInformation = (NWCCODE NWAPI (*)( NWCONN_HANDLE,
                         char NWPTR, BYTE NWPTR, BYTE NWPTR, BYTE NWPTR,
                         WORD NWPTR, WORD NWPTR, WORD NWPTR, WORD NWPTR,
                         BYTE NWPTR, BYTE NWPTR ))
                  GetProcAddress( handle, "NWGetFileServerInformation" )
          )
      )
      return FreeLib( );

   // NWIsManager( )
   if ( NULL == (nwfpNWIsManager = (NWCCODE NWAPI (*)( NWCONN_HANDLE ))
                  GetProcAddress( handle, "NWIsManager" )
          )
      )
      return FreeLib( );
#endif

#ifndef NOVOL_INC
   // NWGetDiskUtilization( )
   if ( NULL == (nwfpNWGetDiskUtilization = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                      DWORD objectID, BYTE volumeNumber, WORD NWPTR usedDirectories,
                      WORD NWPTR usedFiles, WORD NWPTR usedBlocks ))
                  GetProcAddress( handle, "NWGetDiskUtilization" )
          )
      )
      return FreeLib( );

   // NWGetObjDiskRestrictions( )
   if ( NULL == (nwfpNWGetObjDiskRestrictions = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                      BYTE  volNumber, DWORD objectID, DWORD NWPTR restriction,
                      DWORD NWPTR inUse ))
                  GetProcAddress( handle, "NWGetObjDiskRestrictions" )
          )
      )
      return FreeLib( );

   // NWScanVolDiskRestrictions( )
   if ( NULL == (nwfpNWScanVolDiskRestrictions = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                        BYTE volNumber, DWORD NWPTR sequence, NWVolumeRestrictions NWPTR volInfo ))
                  GetProcAddress( handle, "NWScanVolDiskRestrictions" )
          )
      )
      return FreeLib( );

   // NWScanVolDiskRestrictions2( )
   if ( NULL == (nwfpNWScanVolDiskRestrictions2 = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                      BYTE volNum, DWORD NWPTR seq, NWVOL_RESTRICTIONS NWPTR volInfo ))
                  GetProcAddress( handle, "NWScanVolDiskRestrictions2" )
          )
      )
      return FreeLib( );

   // NWRemoveObjectDiskRestrictions( )
   if ( NULL == (nwfpNWRemoveObjectDiskRestrictions = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                     BYTE volNumber, DWORD objectID ))
                  GetProcAddress( handle, "NWRemoveObjectDiskRestrictions" )
          )
      )
      return FreeLib( );

   // NWSetObjectVolSpaceLimit( )
   if ( NULL == (nwfpNWSetObjectVolSpaceLimit = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                       WORD volNumber, DWORD objectID, DWORD restriction ))
                  GetProcAddress( handle, "NWSetObjectVolSpaceLimit" )
          )
      )
      return FreeLib( );

   // NWGetVolumeInfoWithHandle( )
   if ( NULL == (nwfpNWGetVolumeInfoWithHandle = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                       NWDIR_HANDLE dirHandle, char NWPTR volName, WORD NWPTR totalBlocks,
                       WORD NWPTR sectorsPerBlock, WORD NWPTR availableBlocks,
                       WORD NWPTR totalDirEntries, WORD NWPTR availableDirEntries,
                       WORD NWPTR volIsRemovableFlag ))
                  GetProcAddress( handle, "NWGetVolumeInfoWithHandle" )
          )
      )
      return FreeLib( );

   // NWGetVolumeInfoWithNumber( )
   if ( NULL == (nwfpNWGetVolumeInfoWithNumber = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                       WORD volNumber, char NWPTR volName, WORD NWPTR totalBlocks,
                       WORD NWPTR sectorsPerBlock, WORD NWPTR availableBlocks,
                       WORD NWPTR totalDirEntries, WORD NWPTR availableDirEntries,
                       WORD NWPTR volIsRemovableFlag ))
                  GetProcAddress( handle, "NWGetVolumeInfoWithNumber" )
          )
      )
      return FreeLib( );

   // NWGetVolumeName( )
   if ( NULL == (nwfpNWGetVolumeName = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                      WORD volNum, char NWPTR volName ))
                  GetProcAddress( handle, "NWGetVolumeName" )
          )
      )
      return FreeLib( );

   // NWGetVolumeNumber( )
   if ( NULL == (nwfpNWGetVolumeNumber = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                       char NWPTR volName, WORD NWPTR volNumber ))
                  GetProcAddress( handle, "NWGetVolumeNumber" )
          )
      )
      return FreeLib( );

   // NWGetVolumeStats( )
   if ( NULL == (nwfpNWGetVolumeStats = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                      BYTE volumeNumber, VOL_STATS NWPTR volInfo ))
                  GetProcAddress( handle, "NWGetVolumeStats" )
          )
      )
      return FreeLib( );

   // NWGetExtendedVolumeInfo( )
   if ( NULL == (nwfpNWGetExtendedVolumeInfo = (NWCCODE NWAPI (*)( NWCONN_HANDLE conn,
                        WORD volNumber, NWVolExtendedInfo NWPTR volInfo ))
                  GetProcAddress( handle, "NWGetExtendedVolumeInfo" )
          )
      )
      return FreeLib( );

#endif

   allFuncPtrsSet = TRUE;

   return TRUE;

} // end of BOOL NetWareLib::LoadNWLib( void )

