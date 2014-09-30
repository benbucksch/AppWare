/* ***********************************************************************
 *
 * Copyright (c) 1994 Novell, Inc.  All rights reserved.
 *
 * THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
 * TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
 * CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 * TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
 * NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
 * AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
 * LIABILITY.
 *
 * *********************************************************************** */

#ifndef O_NWERR_H
#define O_NWERR_H

/* ***********************************************************************
 * ATYPEID for NetWare Error Object
 * *********************************************************************** */

#define OTYPE_NWERROR           ATYPE_MAKEID('N','W','E','0')

/* ***********************************************************************
 * Function Prototypes for NetWare Error Object
 * *********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif
BOOL ALMAPI ReportNWError(LPSTR lpszText) ;
BOOL ALMAPI IsReportingOn() ;
#ifdef __cplusplus
}
#endif

#endif  /* #ifdef O_NWERR_H */
