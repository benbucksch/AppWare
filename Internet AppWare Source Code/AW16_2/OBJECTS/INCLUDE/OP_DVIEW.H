/*                                                                        *\
   Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.

   THIS WORK IS SUBJECT TO U.S.  AND  INTERNATIONAL  COPYRIGHT  LAWS  AND
   TREATIES.  NO PART OF THIS WORK MAY  BE  USED,  PRACTICED,  PERFORMED,
   COPIED,   DISTRIBUTED,   REVISED,   MODIFIED,   TRANSLATED,  ABRIDGED,
   CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,  TRANSFORMED
   OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY  USE
   OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD  SUBJECT  THE
   PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE         : OP_DVIEW.H                                              |
 | DESCRIPTION  : Object Protocol Database View Include File.             |
 | COPYRIGHT    : 1993-1994 Novell, Inc.                                  |
\*------------------------------------------------------------------------*/
#ifndef OP_DVIEW_H
#define OP_DVIEW_H

#ifdef A_ALM_H                          /* AppWare Loadable Module */

/* ---- Object Protocol Database View Events ---- */
#define AEVENT_DVIEWCHANGED     MAKEEVENTID('d', 'b', 'v', 'C')
#define AEVENT_DVIEWGETDATA     MAKEEVENTID('d', 'b', 'v', 'D')
#define AEVENT_DVIEWGETROWCOUNT MAKEEVENTID('d', 'b', 'v', 'R')
#define AEVENT_DVIEWSET         MAKEEVENTID('d', 'b', 'v', 'S')

/* ---- error codes ---- */
#define DVIEWERROR              -1      /* q+e error */
#define DVIEWNOSTMT             -2      /* no statement */

typedef struct coldesc          ODViewColDesc;
struct coldesc                  /* ---- Column Description -------------- */
{ long          rowCount;               /* O: number of rows */
  WORD          colCount;               /* O: number of columns */
  char          huge *colNames;         /* O: '\0' separated column names */
};

typedef struct coldata          ODViewColData;
struct coldata                  /* ---- Column Data --------------------- */
{ long          recNmbr;                /* I: record number */
  long          recCount;               /* I: number of records */
  MEMBLOCKID    colDataMB;              /* IO: '\0' separated column data
                                           memory block
                                        */
  long          retCode;                /* O: # of records retrieved,
                                              -1 if an error occured,
                                              -2 if no stmt executed
                                        */
};

#endif /* A_ALM_H */

#endif /* OP_DVIEW_H */
