//--------------------------------------------------------------------------
// BindFunc.CPP
//
// DESCRIPTION:
//    The 'NW Bindery' ALM's general function implementations.
//
// VAB FUNCTIONS IMPLEMENTED:
//    Who Am I ?
//    Get Primary Server
//    Is Bindery Emulation ?
//
// POTENTIAL OBJECT APIs IMPLEMENTED:
//    OBindGetConnHandle( )
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

//--------------------------------------------------------------------------
// FUNCTION:  Who Am I ?
//
// DESCRIPTION:  Gets the name of the current user and other information
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//
// OUTPUT PARAMETERS
//    1: Text object to receive the user name
//    2: Flag object which, if set, means the user is supervisor or equiv.
//    3: Text object to receive the user's read access level
//    4: Text object to receive the user's write access level
//
// FLOWS
//    1: Retrieved
//	   2: No connection
//	   3: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetBinderyUserInfo( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID           oiBindery;    // the ID of the Bindery input object
   OBJECTID           oiOutText;    // the ID of the 'user name' output object
   OBJECTID           oiOutFlag;    // the ID of the 'is Supervisor' output object
   TYPEID             outputType;   // the type of the current output object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;
   NWCONN_HANDLE      cHandle;
   char               serverName[ 50 ];
   char               userName[ 50 ];
   int                retFlow = 3; // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   if ( oiBindery != 0 )
   {
      cHandle = OBindGetConnHandle( oiBindery, TRUE );
      if ( cHandle != 0 )
      {  NWSERVICE_VERSION   majorVersion, minorVersion, revision;

         serverName[ 0 ] = '\0';
         ccode = nwfpNWGetFileServerInformation( cHandle, serverName, &majorVersion, &minorVersion, &revision,
                                             NULL, NULL, NULL, NULL, NULL, NULL );
         if ( (ccode == SUCCESSFUL) && (serverName[0] != '\0') )
         {  // get the user name for the specified connection handle
            CONNECT_INFO    connInfo;

            // dynamic call to NetWare API NWGetConnectionStatus( )
            ccode = nwfpNWGetConnectionStatus( cHandle, (CONNECT_INFO NWFAR *)&connInfo,
                                               sizeof(CONNECT_INFO) );
            if ( ccode == SUCCESSFUL )
            {  // copy the user name to the appropriate buffer
               strncpy( userName, connInfo.clientName, sizeof(userName) );
            }

            // ??? check if output parameter 1 is even hooked up ???
            oiOutText = AObjCreate( OTYPE_TEXT );
            if ( oiOutText != 0 )
            {
               OTextSetValueFromBuffer( oiOutText, userName );
               AFuncSetParameter( 1, oiOutText ); // Output this text object
               retFlow = 1;
            }
            else
            {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
            }

            // check if 'Is Supervisor' output parameter flag object is connected
            oiOutFlag = AFuncGetOutput( 2, &outputType );
            if ( (oiOutFlag != 0) && (outputType == OTYPE_FLAG) )
            {  // we need to supply the 'Is Supervisor' output value - Flag object

               // ??? need to perform a real check ???
               ccode = nwfpNWIsManager( cHandle );
               if ( ccode == 0 )  // current user is supervisor or listed as a mgr of supervisor
//             if ( strcmpi( "SUPERVISOR", userName ) == 0 )
                  OFlagSet( oiOutFlag, TRUE );
               else
                  OFlagSet( oiOutFlag, FALSE );
            }

            // ??? test if 'Rd Access' and/or 'Wrt Access' output is hooked up ???
         }
      }
   }

	theSystem->message1 = retFlow; // 'Success' flow

	return 0;

} // end of GetBinderyUserInfo( )

//--------------------------------------------------------------------------
// FUNCTION:  Get Primary Server
//
// DESCRIPTION:  Gets the name of the NetWare server
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//
// OUTPUT PARAMETERS
//    1: Text object to receive the server name
//
// FLOWS
//    1: Retrieved
//	   2: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export GetPrimaryServer( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   OBJECTID           oiOutText;    // the ID of the Bindery Object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;
   NWCONN_HANDLE      cHandle;
   char               serverName[ 50 ];
   int                retFlow = 3; // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   if ( oiBindery != 0 )
   {
      // determine if the specific server or current server is using
      // Bindery Emulation versus a true Bindery
      cHandle = OBindGetConnHandle( oiBindery, TRUE );
      if ( cHandle != 0 )
      {  NWSERVICE_VERSION   majorVersion, minorVersion, revision;

         serverName[ 0 ] = '\0';
         ccode = nwfpNWGetFileServerInformation( cHandle, serverName, &majorVersion, &minorVersion, &revision,
                                             NULL, NULL, NULL, NULL, NULL, NULL );
         if ( (ccode == SUCCESSFUL) && (serverName[0] != '\0') )
         {
            oiOutText = AObjCreate( OTYPE_TEXT );
            if ( oiOutText != 0 )
            {
               OTextSetValueFromBuffer( oiOutText, serverName );
               AFuncSetParameter( 1, oiOutText ); // Output this text object
               retFlow = 1;
            }
            else
            {  // creation of temporary object failed !!
#ifdef ADBG_DEBUG
#endif
            }
         }
      }
   }

	theSystem->message1 = retFlow; // 'Success' flow

	return 0;

} // end of GetPrimaryServer( )

//--------------------------------------------------------------------------
// FUNCTION:  IsBinderyEmulation
//
// DESCRIPTION:  Determines if the server specified by the NW Bindery object
//               is supporting the NetWare Bindery in emulation mode
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Not Emulation
//	   2: Is Emulation
//	   3: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export IsBinderyEmulation( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;
   NWCONN_HANDLE      cHandle;
   int                retFlow = 3; // assume 'Error' flow

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      theSystem->message1 = retFlow;
      return 0;
   }

	oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
   if ( oiBindery != 0 )
   {
      // determine if the specific server or current server is using
      // Bindery Emulation versus a true Bindery
      cHandle = OBindGetConnHandle( oiBindery, TRUE );
      if ( cHandle != 0 )
      {  NWSERVICE_VERSION   majorVersion, minorVersion, revision;

         ccode = nwfpNWGetFileServerInformation( cHandle, NULL, &majorVersion, &minorVersion, &revision,
                                             NULL, NULL, NULL, NULL, NULL, NULL );
         if ( ccode == SUCCESSFUL )
         {
            if ( majorVersion >= 4 )
               retFlow = 2;  // server is running Bindery emulation
            else
               retFlow = 1;  // server is NOT running Bindery emulation
         }
         else
            retFlow = 3; // error determining server version
      }
      else
         retFlow = 3; // error determining connection handle
   }

	theSystem->message1 = retFlow; // 'Success' flow

	return 0;

} // end of IsBinderyEmulation( )

//-------------------------------------------------------
NWCONN_HANDLE OBindGetConnHandle( OBJECTID oiBindObj, BOOL useDefaultServer  )
//-------------------------------------------------------
{
   NWCONN_HANDLE      connHandle = 0;
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   NWCCODE            ccode;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      return 0;
   }

   if ( AObjGetType(oiBindObj) == OTYPE_BINDERY )
   {  // get connection handle, based on server name
      char serverName[ 50 ];

      // upper case the server name (probably only works for ENGLISH !)
      pBinderyData = (BinderyData far*)AObjLockData( oiBindObj, 0 );
      if ( pBinderyData != NULL )
      {
         serverName[0] = '\0';
         OTextGetToBuffer( pBinderyData->ServerName, serverName, 48 );
         AObjUnlockData( oiBindObj, 0 );

         if ( serverName[0] != '\0' )
         {  // if server name is specified, get connection handle for that server
            strupr( serverName );

            // call NWGetConnectionHandle( ) API to get the desired value
            connHandle = 0;
            ccode = nwfpNWGetConnectionHandle( serverName, 0, &connHandle, NULL );
            if ( (ccode != SUCCESSFUL) && (connHandle != 0) )
            {
               connHandle = 0;
            }
         }
         else if ( useDefaultServer == TRUE )
         {  // no server name specified, get default connection handle
            ccode = nwfpNWGetDefaultConnectionID( &connHandle );
            if ( ccode != SUCCESSFUL )
            {  // error obtaining default connection handle
               connHandle = 0;
            }
         }
         else
            connHandle = 0; // could not determine connection handle value
      }
   }

   return connHandle;

} // end of OBindGetConnHandle( )
