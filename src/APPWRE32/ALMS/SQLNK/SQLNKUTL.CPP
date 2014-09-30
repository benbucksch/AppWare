/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: SqlnkUtl.CPP                                            |
 | DESCRIPTION	: SequeLink SQL Utilities.                                |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "sqld.h"
#include "sqlnkutl.h"


int SqlnkUtl::GetDatabaseType	/* ---- Get Database Type --------------- */
( char		*database		// database
)
/* Returns the database type. */
{
  if (lstrcmpi(database, "AS/400") == 0)
    return DB_AS400;
  if (lstrcmpi(database, "DB2") == 0)
    return DB_DB2;
  if (lstrcmpi(database, "Informix") == 0)
    return DB_INFORMIX;
  if (lstrcmpi(database, "Ingres") == 0)
    return DB_INGRES;
  if (lstrcmpi(database, "Interbase") == 0)
    return DB_INTERBASE;
  if (lstrcmpi(database, "Oracle") == 0)
    return DB_ORACLE;
  if (lstrcmpi(database, "OS/2 DBM") == 0)
    return DB_OS2DBM;
  if (lstrcmpi(database, "RDB") == 0)
    return DB_RDB;
  if (lstrcmpi(database, "Sybase") == 0)
    return DB_SYBASE;
  return 0;
} // GetDatabaseType

int SqlnkUtl::GetProtocolType	/* ---- Get Protocol Type --------------- */
( char		*protocol		// protocol
)
/* Returns the protocol type. */
{
  if (lstrcmpi(protocol, "APPC") == 0)
    return NP_APPC;
  if (lstrcmpi(protocol, "DECnet") == 0)
    return NP_DECNET;
  if (lstrcmpi(protocol, "NetBIOS") == 0)
    return NP_NETBIOS;
  if (lstrcmpi(protocol, "TCP") == 0 || lstrcmpi(protocol, "TCP/IP") == 0)
    return NP_TCPIP;
  return 0;
} // GetProtocolType

int SqlnkUtl::GetServerType	/* ---- Get Server Type ----------------- */
( char		*server			// server
)
/* Returns the server type. */
{
  if (lstrcmpi(server, "AS/400") == 0)
    return SV_AS400;
  if (lstrcmpi(server, "MVS") == 0)
    return SV_MVS;
  if (lstrcmpi(server, "OS/2 Router") == 0)
    return SV_OS2ROUTER;
  if (lstrcmpi(server, "OS/2") == 0)
    return SV_OS2;
  if (lstrcmpi(server, "VAX") == 0)
    return SV_VAX;
  if (lstrcmpi(server, "UNIX") == 0)
    return SV_UNIX;
  return 0;
} // GetServerType


char *SqlnkUtl::GetNetworkModuleName /* Get Network Module Name --------- */
( int		networkType		// network type
)
/* Returns pointer to the network module name. */
{
  switch (networkType)
  { case NP_APPC:
      return "SSP APPC";
    case NP_DECNET:
      return "SSP DECNET";
    case NP_NETBIOS:
      return "SSP NETBIOS";
    case NP_TCPIP:
      return "SSP TCP";
  } // switch
  return "";
} // GetNetworkModuleName
