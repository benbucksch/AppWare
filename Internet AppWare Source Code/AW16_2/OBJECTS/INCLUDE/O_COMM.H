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
 * TYPEID for Communication Object
 * *********************************************************************** */

#define OTYPE_CONN          MAKETYPEID ('C','o','n','n')
#define OTYPE_TERMINAL      MAKETYPEID ('T','e','r','m')
#define OTYPE_FILETRANSFER  MAKETYPEID ('F','t','r','n')

#ifdef __cplusplus
extern "C" {
#endif

/* ***********************************************************************
 * Getting and setting from normal representations
 * *********************************************************************** */

BOOL  ALMAPI CommIsPortOpen(OBJECTID oiConn) ;
int   ALMAPI CommRxCount (OBJECTID oiConn) ;
int   ALMAPI CommTxCount (OBJECTID oiConn) ;
int   ALMAPI CommFlushQueue (OBJECTID oiConn, int fnQueue) ;
int   ALMAPI CommGetStopBits (OBJECTID oiConn) ;
int   ALMAPI CommGetParity (OBJECTID oiConn) ;
int   ALMAPI CommGetHandShake(OBJECTID oiConn) ;

int   ALMAPI CommSetBaudRate (OBJECTID oiConn, int Baudrate) ;
int   ALMAPI CommSetStopBits (OBJECTID oiConn, int StopBits) ;
int   ALMAPI CommSetParity (OBJECTID oiConn, int Parity) ;
int   ALMAPI CommSetHandShake(OBJECTID oiConn, int handshake) ;
int   ALMAPI CommSetXONXOFF (OBJECTID oiConn, int XonXoff) ;

int   ALMAPI CommGetPauseStatus(OBJECTID) ;
int   ALMAPI CommGetBuffer(OBJECTID oiConn, BYTE far* buffer, int char_count ) ;
int   ALMAPI CommGetString(OBJECTID oiConn, BYTE far* string, int n) ;
int   ALMAPI CommGetChar(OBJECTID oiConn ) ;
int   ALMAPI CommWaitChar(OBJECTID oiConn, long mesc) ;
int   ALMAPI CommPutBuffer(OBJECTID oiConn, const BYTE far* buf, int n ) ;
int   ALMAPI CommPutChar(OBJECTID oiConn, BYTE b ) ;
int   ALMAPI CommPutString(OBJECTID oiConn, const BYTE far* string ) ;
LPSTR ALMAPI CommGetMem(OBJECTID oiConn) ;
long  ALMAPI CommGetMemLen(OBJECTID oiConn) ;

#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* !O_COMM_H */
