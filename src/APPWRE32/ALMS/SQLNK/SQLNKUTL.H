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
 | FILE		: SQLNKUTL.H                                              |
 | DESCRIPTION	: SequeLink SQL Utilities Include File.                   |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef SQLNKUTL_H
#define SQLNKUTL_H

#include "sqlnkutl.h"


class SqlnkUtl			/* ---- SequeLink SQL Utilities --------- */
{
public:
  int GetDatabaseType(char *database);
  /* Returns the database type. */

  int GetProtocolType(char *protocol);
  /* Returns the protocol type. */

  int GetServerType(char *server);
  /* Returns the server type. */


  char *GetNetworkModuleName(int networkType);
  /* Returns pointer to the network module name. */
};

#endif // SQLNKUTL_H
