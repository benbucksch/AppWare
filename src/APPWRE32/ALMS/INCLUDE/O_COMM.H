/* ***********************************************************************
 *
 * Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.
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

#ifndef O_COMM_H
#define O_COMM_H

/* ***********************************************************************
 * ATYPEID for Communication Object
 * *********************************************************************** */

#define OTYPE_CONN          ATYPE_MAKEID ('C','o','n','n')
#define OTYPE_TERMINAL      ATYPE_MAKEID ('T','e','r','m')
#define OTYPE_FILETRANSFER  ATYPE_MAKEID ('F','t','r','n')

#ifdef __cplusplus
extern "C" {
#endif

/* ***********************************************************************
 * Getting and setting from normal representations
 * *********************************************************************** */

BOOL  ALMAPI CommIsPortOpen(AOBJECTID oiConn) ;
int   ALMAPI CommRxCount (AOBJECTID oiConn) ;
int   ALMAPI CommTxCount (AOBJECTID oiConn) ;
int   ALMAPI CommFlushQueue (AOBJECTID oiConn, int fnQueue) ;
int   ALMAPI CommGetStopBits (AOBJECTID oiConn) ;
int   ALMAPI CommGetParity (AOBJECTID oiConn) ;
int   ALMAPI CommGetHandShake(AOBJECTID oiConn) ;

int   ALMAPI CommSetBaudRate (AOBJECTID oiConn, int Baudrate) ;
int   ALMAPI CommSetStopBits (AOBJECTID oiConn, int StopBits) ;
int   ALMAPI CommSetParity (AOBJECTID oiConn, int Parity) ;
int   ALMAPI CommSetHandShake(AOBJECTID oiConn, int handshake) ;
int   ALMAPI CommSetXONXOFF (AOBJECTID oiConn, int XonXoff) ;

int   ALMAPI CommGetPauseStatus(AOBJECTID) ;
int   ALMAPI CommGetBuffer(AOBJECTID oiConn, BYTE far* buffer, int char_count ) ;
int   ALMAPI CommGetString(AOBJECTID oiConn, BYTE far* string, int n) ;
int   ALMAPI CommGetChar(AOBJECTID oiConn ) ;
int   ALMAPI CommWaitChar(AOBJECTID oiConn, long mesc) ;
int   ALMAPI CommPutBuffer(AOBJECTID oiConn, const BYTE far* buf, int n ) ;
int   ALMAPI CommPutChar(AOBJECTID oiConn, BYTE b ) ;
int   ALMAPI CommPutString(AOBJECTID oiConn, const BYTE far* string ) ;
LPSTR ALMAPI CommGetMem(AOBJECTID oiConn) ;
long  ALMAPI CommGetMemLen(AOBJECTID oiConn) ;

#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* !O_COMM_H */
