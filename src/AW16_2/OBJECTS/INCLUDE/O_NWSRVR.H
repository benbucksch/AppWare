/***
-----------------------------------------------------------------------------
 File Name:  O_NWSrvr.h

 Description.:
	  Header file for the AppWare NetWare Server object APIs.

  (c) 1994-1995 Novell, Inc.  All rights reserved.

  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
  LIABILITY.
-----------------------------------------------------------------------------
***/
#ifndef OTYPE_NWSRVR

#include <nwcalls.h>

/* define the new type ID */
#define OTYPE_NWSRVR MAKETYPEID('N','W','S','0')

/* Error return codes for functions returning an 'int' */
#define ONWSRVR_SUCCESS              0
#define ONWSRVR_ERR_SUCCESS          0
#define ONWSRVR_ERR_LOCK_FAILED     -1
#define ONWSRVR_ERR_UNLOCK_FAILED   -2
#define ONWSRVR_ERR_NO_CONNECTION   -3
#define ONWSRVR_ERR_BAD_SRVR_OBJ    -4
#define ONWSRVR_ERR_RESIZE_FAILED   -5
#define ONWSRVR_ERR_MEM_ALLOC       -6
#define ONWSRVR_ERR_MUST_BE_VER4    -7
#define ONWSRVR_ERR_NO_NW_DLLS      -9

/* Exported functions */
#ifdef __cplusplus
extern "C" {
#endif

NWCONN_HANDLE ALMAPI ONWSrvrGetConnHandle( OBJECTID oiNWServer, BOOL noPrimaryOverride );
/***
 'noPrimaryOverride' = TRUE   overrides the setting stored in the NW Server object data blks
                              and disallows obtaining the connection handle of the primary
                              connection if a server name is not specified
 'noPrimaryOverride' = FALSE  uses the setting stored in the NW Server object data blks
                              to determine if the connection handle of the primary
                              connection can be used if a server name is not specified
***/

int ALMAPI ONWSrvrSetConnHandle( OBJECTID oiNWServer, NWCONN_HANDLE connHandle );
/***
   setting the value of 'connHandle' to zero effectively clears the cached value
***/

int ALMAPI ONWSrvrSetName( OBJECTID oiNWServer, LPSTR serverName );

int ALMAPI ONWSrvrGetName( OBJECTID oiNWServer, LPSTR serverName, int bufferSize );

int ALMAPI ONWSrvrGetVersion( OBJECTID oiNWServer, LPSTR versionStr, int *majorVersion,
                              int *minorVersion, char *revLetter );

int ALMAPI ONWSrvrClearCachedValues( OBJECTID oiNWServer );

#ifdef __cplusplus
}
#endif

#endif
