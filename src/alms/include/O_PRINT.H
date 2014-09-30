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

#ifndef   O_PRINT_H
#define   O_PRINT_H

#define OTYPE_PRINTER                    ATYPE_MAKEID('P','r','n','t')
#define OPRINT_EVT_PRINTITEMDRAWROUTINE  AEVENT_MAKEID('p', 'i', 'd', 'r')


typedef struct tagPIDR_BLOCK
{    
  WORD    fromPage ;    /* starting page to print                     */
  WORD    toPage ;      /* ending page to print                       */
  BOOL    numPages ;    /* TRUE when asking for number of pages       */
  HDC     hDC ;         /* Printer Device Context Handle              */
  HWND    hMsgWnd ;     /* Write print message via SetWindowText      */
  RECT    rect ;        /* Print Rectangle Physical Dots              */
  POINT   DPI ;         /* Dots per inch in x and y directions        */
  char    error[256] ;  /* Error message string                       */

  /* See Windows API, vol. II, Chapter 12 for more information for
     supporting PCL printers.  Using the DRAWPATTERNRECT Escape will
     significantly reduce print time and memory requirements.         */
  int     pclPrinter ;  /* 0 if not pcl, 2 if White rule, 1 otherwise */
} PIDR_BLOCK ;

typedef PIDR_BLOCK far*    LPPIDR_BLOCK ;

typedef struct tagPRECT_STRUCT
{
  POINT   prPosition ;  /* Upper Left corner of the rectangle         */
  POINT   prSize ;      /* Width (x) and Height (y) of the rectangle  */
  WORD    prStyle ;     /* type of pattern (see WINDOWS API, vol. II) */
  WORD    prPattern ;   /* pattern (see WINDOWS API, vol. II)         */
} PRECT_STRUCT ;

typedef PRECT_STRUCT far*  LPPRECT_STRUCT ;

#endif  /* #ifndef O_PRINT_H */
