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

#ifndef O_UPSF_H
#define O_UPSF_H

/* *********************************************************************** */

#ifndef __STRING_H
#include <string.h>
  #ifndef __STRING_H
  #define __STRING_H
  #endif
#endif

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef O_WND_H
#include "o_wnd.h"
#endif

/* *********************************************************************** */

inline long ExportBool
  (
  LPCSTR  szName,
  LONG    idx,
  BOOL    fValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &fValue, sizeof(BOOL));
  }

/* *********************************************************************** */

inline long ImportBool
  (
  LPCSTR    szName,
  LONG      idx,
  BOOL * fValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_BOOL, fValue, sizeof(BOOL));
  }

/* *********************************************************************** */

inline long ExportShort
  (
  LPCSTR  szName,
  LONG    idx,
  short   sValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER, &sValue, sizeof(short));
  }

/* *********************************************************************** */

inline long ImportShort
  (
  LPCSTR      szName,
  LONG        idx,
  short *  sValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_SHORT, sValue, sizeof(short));
  }

/* *********************************************************************** */

inline long ExportUShort
  (
  LPCSTR  					szName,
  LONG    					idx,
  unsigned short   	usValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_USHORT, AUPSF_UNIV_INTEGER, &usValue, sizeof(unsigned short));
  }

/* *********************************************************************** */

inline long ImportUShort
  (
  LPCSTR      					szName,
  LONG        					idx,
  unsigned short *  	usValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_USHORT, usValue, sizeof(unsigned short));
  }

/* *********************************************************************** */

inline long ExportInt
  (
  LPCSTR  szName,
  LONG    idx,
  int   	iValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &iValue, sizeof(int));
  }

/* *********************************************************************** */

inline long ImportInt
  (
  LPCSTR      szName,
  LONG        idx,
  int *  			iValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_INT, iValue, sizeof(int));
  }

/* *********************************************************************** */

inline long ExportUInt
  (
  LPCSTR  				szName,
  LONG    				idx,
  unsigned int   	uValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_UINT, AUPSF_UNIV_INTEGER, &uValue, sizeof(unsigned int));
  }

/* *********************************************************************** */

inline long ImportUInt
  (
  LPCSTR      			szName,
  LONG        			idx,
  unsigned int *  	uValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_UINT, uValue, sizeof(unsigned int));
  }

/* *********************************************************************** */

inline long ExportLong
  (
  LPCSTR  szName,
  LONG    idx,
  long    lValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &lValue, sizeof(long));
  }

/* *********************************************************************** */

inline long ImportLong
  (
  LPCSTR    szName,
  LONG      idx,
  long * lValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_LONG, lValue, sizeof(long));
  }

/* *********************************************************************** */

inline long ExportULong
  (
  LPCSTR  				szName,
  LONG    				idx,
  unsigned long		ulValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_ULONG, AUPSF_UNIV_INTEGER, &ulValue, sizeof(unsigned long));
  }

/* *********************************************************************** */

inline long ImportULong
  (
  LPCSTR    						szName,
  LONG      						idx,
  unsigned long * 		ulValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_ULONG, ulValue, sizeof(unsigned long));
  }

/* *********************************************************************** */

inline long ExportColor
  (
  LPCSTR    szName,
  LONG      idx,
  COLORREF  clrValue
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &clrValue, sizeof(COLORREF));
  }

/* *********************************************************************** */

inline long ImportColor
  (
  LPCSTR        szName,
  LONG          idx,
  COLORREF * clrValue
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_RGBCOLOR, clrValue, sizeof(COLORREF));
  }

/* *********************************************************************** */

inline long ExportFont
  (
  LPCSTR    szName,
  LONG      idx,
  LPSTR     szFaceName
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_FONT, AUPSF_UNIV_FONT, szFaceName, LF_FACESIZE);
  }

/* *********************************************************************** */

inline long ImportFont
  (
  LPCSTR    szName,
  LONG      idx,
  LPSTR     szFaceName
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_FONT, szFaceName, LF_FACESIZE);
  }

/* *********************************************************************** */

inline long ExportBrush
  (
  LPCSTR    szName,
  LONG      idx,
  HBRUSH    hbr
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_HBRUSH, AUPSF_UNIV_BITMAP, &hbr, sizeof(HBRUSH));
  }

/* *********************************************************************** */

inline long ImportBrush
  (
  LPCSTR    szName,
  LONG      idx,
  WORD      bits[8]
  )

  {
  long  nLen = 0;
  long  lRet = AUpsfFieldInfo(szName, idx, AUPSF_NATV_BITMAP, NULL, &nLen);

  if(lRet == AUPSF_NOERROR)
    {
    BITMAP* bmp = (BITMAP*)new char[(int)nLen];

    if(bmp)
      {
      lRet = AUpsfImportField(szName, idx, AUPSF_NATV_BITMAP, bmp, nLen);

      if(lRet == AUPSF_NOERROR)
        {
        WORD *  pBits = (WORD*)(bmp + 1);

        // only the low-order byte has significant information
        bits[0] = pBits[0] & (WORD)0x00FF;
        bits[1] = pBits[1] & (WORD)0x00FF;
        bits[2] = pBits[2] & (WORD)0x00FF;
        bits[3] = pBits[3] & (WORD)0x00FF;
        bits[4] = pBits[4] & (WORD)0x00FF;
        bits[5] = pBits[5] & (WORD)0x00FF;
        bits[6] = pBits[6] & (WORD)0x00FF;
        bits[7] = pBits[7] & (WORD)0x00FF;
        }

      delete bmp;
      }
    else
      lRet = AUPSF_MEMORYERROR;
    }

  if(lRet != AUPSF_NOERROR)
    FillMemory(bits, 0xFF, 8*sizeof(WORD));

  return lRet;
  }

/* *********************************************************************** */

inline long ExportText
  (
  LPCSTR    szName,
  LONG      idx,
  LPCSTR    szText,
  long      nLen
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_STRING, AUPSF_UNIV_STRING, (void*)szText, nLen);
  }

/* *********************************************************************** */

inline long ImportText
  (
  LPCSTR      szName,
  LONG        idx,
  AMEMBLOCKID  miText
  )

  {
  long      lRet;
  long      nLen  = 0;
  char * szText;

  lRet = AUpsfFieldInfo(szName, idx, AUPSF_NATV_STRING, NULL, &nLen);

  if(lRet == AUPSF_NOERROR)
    {
    AMemSetSize(miText, nLen);
    szText = (char*)AMemLock(miText);
    lRet = AUpsfImportField(szName, idx, AUPSF_NATV_STRING, szText, nLen);
    AMemUnlock(miText);
    }

  return lRet;
  }

/* *********************************************************************** */

inline long ExportPoint
  (
  LPCSTR        szName,
  LONG          idx,
  const LPPOINT lppt
  )

  {
  return AUpsfExportField(szName, idx, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, lppt, sizeof(POINT));
  }

/* *********************************************************************** */

inline long ImportPoint
  (
  LPCSTR    szName,
  LONG      idx,
  LPPOINT   lppt
  )

  {
  return AUpsfImportField(szName, idx, AUPSF_NATV_POINT, lppt, sizeof(POINT));
  }

/* *********************************************************************** */

inline long ExportLogicalPoint
  (
  AOBJECTID      oiWnd,
  LPCSTR        szName,
  LONG          idx,
  const LPPOINT lppt
  )

  {
  POINT pt;

  pt.x = lppt->x;
  pt.y = lppt->y;

  OWndLPtoDP(oiWnd, &pt, 1);

  return AUpsfExportField(szName, idx, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, &pt, sizeof(POINT));
  }

/* *********************************************************************** */

inline long ImportLogicalPoint
  (
  AOBJECTID  oiWnd,
  LPCSTR    szName,
  LONG      idx,
  LPPOINT   lppt
  )

  {
  long  rVal;

  rVal = AUpsfImportField(szName, idx, AUPSF_NATV_POINT, lppt, sizeof(POINT));
  if (rVal == AUPSF_NOERROR)
    OWndDPtoLP(oiWnd, lppt, 1);

  return rVal;
  }

/* *********************************************************************** */

inline long ExportLogicalRect
  (
  AOBJECTID      oiWnd,
  LPCSTR        szName,
  LONG          idx,
  const LPRECT  lprc
  )

  {
  RECT  rc;

  CopyRect(&rc, lprc);
  OWndLPtoDP(oiWnd, (LPPOINT)&rc, 2);
  return AUpsfExportField(szName, idx, AUPSF_NATV_RECT, AUPSF_UNIV_RECT, &rc, sizeof(RECT));
  }

/* *********************************************************************** */

inline long ImportLogicalRect
  (
  AOBJECTID  oiWnd,
  LPCSTR    szName,
  LONG      idx,
  LPRECT    lprc
  )

  {
  long  rVal;

  rVal = AUpsfImportField(szName, idx, AUPSF_NATV_RECT, lprc, sizeof(RECT));
  if (rVal == AUPSF_NOERROR)
    OWndDPtoLP(oiWnd, (LPPOINT)lprc, 2);

  return rVal;
  }

/* *********************************************************************** */

#endif  /* #ifndef O_UPSF_H */
