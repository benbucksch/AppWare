/************************************************************************\
|* (c) Serius Corporation, 1989-1992                                    *|
|*                                                                      *|
|* Written by:  Kevin Smart                                             *|
|* Date:        July 2, 1992                                            *|
|*                                                                      *|
|* Header to interface with printer object                              *|
\************************************************************************/

#ifndef   __PRINTOBJ_H
#define   __PRINTOBJ_H

#ifndef   __WINDOWS_H
#include  <windows.h>
#endif  /* ifndef __WINDOWS_H */


/* millimeters per inch */
#define MM_PER_IN       25.4
/* inches per millimeter */
#define IN_PER_MM       0.03937007874016


#define printItemDrawRoutine    AMAKEID('p', 'i', 'd', 'r')

typedef struct tagPIDR_BLOCK {    
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

typedef PIDR_BLOCK *     LPPIDR_BLOCK ;

typedef struct {
  POINT   prPosition ;  /* Upper Left corner of the rectangle         */
  POINT   prSize ;      /* Width (x) and Height (y) of the rectangle  */
  WORD    prStyle ;     /* type of pattern (see WINDOWS API, vol. II) */
  WORD    prPattern ;   /* pattern (see WINDOWS API, vol. II)         */
  } PRECT_STRUCT ;

typedef PRECT_STRUCT *   LPPRECT_STRUCT ;

#endif  /* #ifndef __PRINTOBJ_H */
