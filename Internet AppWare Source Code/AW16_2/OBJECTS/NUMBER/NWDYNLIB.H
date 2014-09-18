//-----------------------------------------------------------------
// NWDynLib.H
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
/****************************************************************************
  This header will include ALL the NWCALLS include files. To
  exclude one or more specific headers, use the following defines:

        #define NOAFP_INC
        #define NOBINDRY_INC
        #define NOCONNECT_INC
        #define NODEL_INC
        #define NODENTRY_INC
        #define NODIRECTORY_INC
        #define NODPATH_INC
        #define NOEA_INC
        #define NOERROR_INC
        #define NOFILES_INC
        #define NOMESSAGES_INC
        #define NOMISC_INC
        #define NONAMSPC_INC
        #define NONTTS_INC
        #define NOPRINT_INC
        #define NOREDIR_INC        NOTE: this is not present in NWCALLS.H
        #define NOQUEUE_INC
        #define NOSERVER_INC
        #define NOSYNC_INC
        #define NOVOL_INC
        #define NOFSE_INC
        #define NOMIGRATE_INC
****************************************************************************/

#ifndef INC_NWDYNLIB_H

#define INC_NWDYNLIB_H   1

// include the NetWare type definition related header files
#include <nwcaldef.h>
#include <nwalias.h>
#include <nwmisc.h>     // for NWCallsInit( ) and __NWGetNWCallsState( )

class NetWareLib
{
   BOOL         isHandle;        // is 'handle' valid ?
   HINSTANCE    handle;          // Windows instance handle of library (DLL)
   short        version;         // version of library
   BOOL         allFuncPtrsSet;  // did we get all function ptrs

   BOOL FreeLib( void );

public:
   NetWareLib( void );          // constructor
   ~NetWareLib( void );         // destructor

   short Version ( void );      // obtain the module version

   BOOL IsNWLibLoaded( void );  // check if NetWare libraries are available for use

   BOOL LoadNWLib( void );

};


extern NetWareLib   dynNWLib;

//------------------------------------------------------------------------
// define extern vars to contain the function ptrs for the functions used
//------------------------------------------------------------------------

//***************************************************************************
//* from Chapter 16 - Misc Services - but we always should get function ptrs
//***************************************************************************
// NWCallsInit( )
extern NWCCODE NWAPI (*nwfpNWCallsInit)( void NWFAR *in, void NWFAR *out );

// __NWGetNWCallsState( )
extern WORD NWAPI (*nwfp__NWGetNWCallsState)( void );


//**************************************************************************
//* from NWREDIR.H - Need routine to parse UNC paths (\\server\volume\path)
//**************************************************************************
#ifndef NOREDIR_INC
#include <nwredir.h>
// NWParseUNCPath( )
extern NWCCODE NWAPI (*nwfpNWParseUNCPath)( char NWFAR *, NWCONN_HANDLE NWFAR *,
                                            char NWFAR *, char NWFAR *,
                                            char NWFAR *, char NWFAR *  );
#endif

//**************************************
//* from Chapter 16 - Misc Services
//**************************************
#ifndef NOMISC_INC
//#include <nwmisc.h>  -- already included above
// NWGetRequesterVersion( )
extern NWCCODE NWAPI (*nwfpNWGetRequesterVersion)( NWREQUESTER_VERSION NWFAR *majorVer,
                                                   NWREQUESTER_VERSION NWFAR *minorVer,
                                                   NWREQUESTER_VERSION NWFAR *revision );
// NWWordSwap( )
extern WORD NWAPI (*nwfpNWWordSwap)( WORD );
#endif

//***********************************************
//* Chapter 6 - Connection Services (nwconnec.h)
//***********************************************
#ifndef NOCONNECT_INC
#include <nwconnec.h>
// NWGetConnectionHandle( )
extern NWCCODE NWAPI (*nwfpNWGetConnectionHandle)( BYTE NWFAR *, NWLOCAL_MODE,
                                                   NWCONN_HANDLE NWFAR *, NWLOCAL_SCOPE NWFAR * );
// NWGetConnectionInformation( )
extern NWCCODE NWAPI (*nwfpNWGetConnectionInformation)( NWCONN_HANDLE, NWCONN_NUM,
                                                        char NWFAR *, NWOBJ_TYPE NWFAR *,
                                                        NWOBJ_ID NWFAR *, BYTE NWFAR *     );
// NWGetConnectionList( )
extern NWCCODE NWAPI (*nwfpNWGetConnectionList)( NWLOCAL_MODE, NWCONN_HANDLE NWFAR *,
                                                 NWSTRUCT_SIZE, NWNUMBER NWFAR * );
// NWGetConnectionStatus( )
extern NWCCODE NWAPI (*nwfpNWGetConnectionStatus)( NWCONN_HANDLE, CONNECT_INFO NWFAR *,
                                                   NWSTRUCT_SIZE );
// NWGetDefaultConnectionID( )
extern NWCCODE NWAPI (*nwfpNWGetDefaultConnectionID)( NWCONN_HANDLE NWFAR * );

// NWGetMaximumConnections( )
extern NWCCODE NWAPI (*nwfpNWGetMaximumConnections)( NWNUMBER NWFAR * );
#endif


//********************************************
//* from Chapter 19 - Path and Drive Services
//********************************************
#ifndef NODPATH_INC
#include <nwdpath.h>
// NWGetDriveStatus( )
extern NWCCODE NWAPI (*nwfpNWGetDriveStatus)( WORD, WORD, WORD NWFAR *, NWCONN_HANDLE NWFAR *,
                                              char NWFAR *, char NWFAR *, char NWFAR * );
// NWGetFirstDrive( )
extern NWCCODE NWAPI (*nwfpNWGetFirstDrive)( WORD NWFAR * );

// NWParseNetWarePath( )
extern NWCCODE NWAPI (*nwfpNWParseNetWarePath)( char NWFAR *, NWCONN_HANDLE NWFAR *,
                                                NWDIR_HANDLE NWFAR *, char NWFAR * );
// NWParsePath( )
extern NWCCODE NWAPI (*nwfpNWParsePath)( char NWFAR *, char NWFAR *, NWCONN_HANDLE NWFAR *,
                                         char NWFAR *, char NWFAR * );
// NWStripServerOffPath( )
extern NWCCODE NWAPI (*nwfpNWStripServerOffPath)( char NWFAR *, char NWFAR * );
#endif


//******************************************
//* from Chapter 13 - File Systems Services
//******************************************
#ifndef NOFILES_INC
#include <nwfile.h>
// NWFileServerFileCopy( )
extern NWCCODE NWAPI (*nwfpNWFileServerFileCopy)( NWLOCAL_FILE_HANDLE, NWLOCAL_FILE_HANDLE, NWOFFSET,
                                                  NWOFFSET, NWNUM, NWNUM NWFAR * );
// NWRenameFile( )
extern NWCCODE NWAPI (*nwfpNWRenameFile)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                          NWSEARCH_ATTR, NWDIR_HANDLE, char NWFAR * );
// NWEraseFiles( )
extern NWCCODE NWAPI (*nwfpNWEraseFiles)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                          NWSEARCH_ATTR );
// NWFileSearchInitialize( )
extern NWCCODE NWAPI (*nwfpNWFileSearchInitialize)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                                    NWPATH_VOL NWFAR *, NWDIR_NUM NWFAR *,
                                                    NWSEQUENCE_NUM NWFAR *, NWACCESS_RIGHTS NWFAR * );
// NWFileSearchContinue( )
extern NWCCODE NWAPI (*nwfpNWFileSearchContinue)( NWCONN_HANDLE, NWPATH_VOL, NWDIR_NUM,
                                                  NWSEARCH_CONTEXT, NWSEARCH_ATTR,
                                                  char NWFAR *, char NWFAR * );
// NWCreateDirectory( )
extern NWCCODE NWAPI (*nwfpNWCreateDirectory)( NWCONN_HANDLE, NWDIR_HANDLE, char NWFAR *,
                                               NWACCESS_RIGHTS );
#endif

// *** other conditional sections from NWCALLS.H

#ifndef NOAFP_INC
#include <nwafp.h>
#endif

#ifndef NOBINDRY_INC
#include <nwbindry.h>
extern NWCCODE NWAPI (*nwfpNWVerifyObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR password);

extern NWCCODE NWAPI (*nwfpNWDisallowObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR disallowedPassword);

extern NWCCODE NWAPI (*nwfpNWChangeObjectPassword)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR oldPassword,
  char NWPTR newPassword);

extern NWCCODE NWAPI (*nwfpNWReadPropertyValue)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE segmentNumber,
  BYTE NWPTR segmentData,
  BYTE NWPTR moreSegments,
  BYTE NWPTR flags);

extern NWCCODE NWAPI (*nwfpNWWritePropertyValue)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE segmentNumber,
  BYTE NWPTR segmentData,
  BYTE moreSegments);

extern NWCCODE NWAPI (*nwfpNWAddObjectToSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

extern NWCCODE NWAPI (*nwfpNWDeleteObjectFromSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

extern NWCCODE NWAPI (*nwfpNWIsObjectInSet)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  char NWPTR memberName,
  WORD memberType);

extern NWCCODE NWAPI (*nwfpNWScanProperty)(
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

extern NWCCODE NWAPI (*nwfpNWGetObjectID)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  DWORD NWPTR objectID);

extern NWCCODE NWAPI (*nwfpNWGetObjectDiskSpaceLeft)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  DWORD NWPTR systemElapsedTime,
  DWORD NWPTR unusedDiskBlocks,
  BYTE NWPTR restrictionEnforced);


extern NWCCODE NWAPI (*nwfpNWGetObjectName)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  char NWPTR objectName,
  WORD NWPTR objectType);

extern NWCCODE NWAPI (*nwfpNWScanObject)(
  NWCONN_HANDLE conn,
  char NWPTR searchName,
  WORD searchType,
  DWORD NWPTR objectID,
  char NWPTR objectName,
  WORD NWPTR objectType,
  BYTE NWPTR hasPropertiesFlag,
  BYTE NWPTR objectFlags,
  BYTE NWPTR objectSecurity);

extern NWCCODE NWAPI (*nwfpNWGetBinderyAccessLevel)(
  NWCONN_HANDLE conn,
  BYTE NWPTR accessLevel,
  DWORD NWPTR objectID);

extern NWCCODE NWAPI (*nwfpNWCreateProperty)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE propertyFlags,
  BYTE propertySecurity);

extern NWCCODE NWAPI (*nwfpNWDeleteProperty)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName);

extern NWCCODE NWAPI (*nwfpNWChangePropertySecurity)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  char NWPTR propertyName,
  BYTE newPropertySecurity);

extern NWCCODE NWAPI (*nwfpNWCreateObject)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  BYTE objectFlags,
  BYTE objectSecurity);

extern NWCCODE NWAPI (*nwfpNWDeleteObject)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType);

extern NWCCODE NWAPI (*nwfpNWRenameObject)(
  NWCONN_HANDLE conn,
  char NWPTR oldObjectName,
  char NWPTR newObjectName,
  WORD objectType);

extern NWCCODE NWAPI (*nwfpNWChangeObjectSecurity)(
  NWCONN_HANDLE conn,
  char NWPTR objectName,
  WORD objectType,
  BYTE newObjectSecurity);

extern NWCCODE NWAPI (*nwfpNWOpenBindery)( NWCONN_HANDLE conn );
extern NWCCODE NWAPI (*nwfpNWCloseBindery)( NWCONN_HANDLE conn );

extern NWCCODE NWAPI (*nwfpNWScanObjectTrusteePaths)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  WORD volNumber,
  WORD NWPTR sequenceNumber,
  BYTE NWPTR accessRights,
  char NWPTR dirPath );

extern NWCCODE NWAPI (*nwfpNWGetObjectEffectiveRights)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  NWDIR_HANDLE dirHandle,
  char NWPTR path,
  WORD NWPTR rightsMask );

#endif

#ifndef NODEL_INC
#include <nwdel.h>
#endif

#ifndef NODENTRY_INC
#include <nwdentry.h>
#endif

#ifndef NODIRECTORY_INC
#include <nwdirect.h>
#endif

#ifndef NOEA_INC
#include <nwea.h>
#endif

#ifndef NOERROR_INC
#include <nwerror.h>
#endif

#ifndef NOMESSAGES_INC
#include <nwmsg.h>
#endif

#ifndef NONAMSPC_INC
#include <nwnamspc.h>
#endif

#ifndef NOPRINT_INC
#include <nwprint.h>
#endif

#ifndef NOQUEUE_INC
#include <nwqms.h>
#endif

#ifndef NOSERVER_INC
#include <nwserver.h>
extern NWCCODE NWAPI (*nwfpNWGetFileServerInformation)(
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

extern NWCCODE NWAPI (*nwfpNWIsManager)( NWCONN_HANDLE conn );
#endif

#ifndef NOSYNC_INC
#include <nwsync.h>
#endif

#ifndef NONTTS_INC
#include <nwtts.h>
#endif

#ifndef NOVOL_INC
#include <nwvol.h>
extern NWCCODE NWAPI (*nwfpNWGetDiskUtilization)(
  NWCONN_HANDLE conn,
  DWORD objectID,
  BYTE volumeNumber,
  WORD NWPTR usedDirectories,
  WORD NWPTR usedFiles,
  WORD NWPTR usedBlocks);

extern NWCCODE NWAPI (*nwfpNWGetObjDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE  volNumber,
  DWORD objectID,
  DWORD NWPTR restriction,
  DWORD NWPTR inUse);

extern NWCCODE NWAPI (*nwfpNWScanVolDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE volNumber,
  DWORD NWPTR sequence,
  NWVolumeRestrictions NWPTR volInfo);

extern NWCCODE NWAPI (*nwfpNWScanVolDiskRestrictions2)(
  NWCONN_HANDLE conn,
  BYTE volNum,
  DWORD NWPTR seq,
  NWVOL_RESTRICTIONS NWPTR volInfo);

extern NWCCODE NWAPI (*nwfpNWRemoveObjectDiskRestrictions)(
  NWCONN_HANDLE conn,
  BYTE volNumber,
  DWORD objectID);

extern NWCCODE NWAPI (*nwfpNWSetObjectVolSpaceLimit)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  DWORD objectID,
  DWORD restriction);

extern NWCCODE NWAPI (*nwfpNWGetVolumeInfoWithHandle)(
  NWCONN_HANDLE conn,
  NWDIR_HANDLE dirHandle,
  char NWPTR volName,
  WORD NWPTR totalBlocks,
  WORD NWPTR sectorsPerBlock,
  WORD NWPTR availableBlocks,
  WORD NWPTR totalDirEntries,
  WORD NWPTR availableDirEntries,
  WORD NWPTR volIsRemovableFlag);

extern NWCCODE NWAPI (*nwfpNWGetVolumeInfoWithNumber)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  char NWPTR volName,
  WORD NWPTR totalBlocks,
  WORD NWPTR sectorsPerBlock,
  WORD NWPTR availableBlocks,
  WORD NWPTR totalDirEntries,
  WORD NWPTR availableDirEntries,
  WORD NWPTR volIsRemovableFlag);

extern NWCCODE NWAPI (*nwfpNWGetVolumeName)(
  NWCONN_HANDLE conn,
  WORD volNum,
  char NWPTR volName);

extern NWCCODE NWAPI (*nwfpNWGetVolumeNumber)(
  NWCONN_HANDLE conn,
  char NWPTR volName,
  WORD NWPTR volNumber);

extern NWCCODE NWAPI (*nwfpNWGetVolumeStats)(
  NWCONN_HANDLE conn,
  BYTE volumeNumber,
  VOL_STATS NWPTR volInfo );

extern NWCCODE NWAPI (*nwfpNWGetExtendedVolumeInfo)(
  NWCONN_HANDLE conn,
  WORD volNumber,
  NWVolExtendedInfo NWPTR volInfo);

#endif

#ifndef NOACCT_INC
#include <nwacct.h>
#endif

#ifndef NOFSE_INC
#include <nwfse.h>
#endif

#ifndef NOMIGRATE_INC
#include <nwmigrat.h>
#endif

#endif  // end of #ifndef INC_NWDYNLIB_H

