////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:  "tabl_lib.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#define   STRICT
#define   ASTRICT

#include  <windows.h>
#include  <string.h>
#include  "a_alm.h"
#include  "tabl_hdr.h"
#include  "o_number.h"

// hmemcpy -- for pre-Windows 3.1 //////////////////////////////////////////
#if (WINVER < 0x030a) // use Windows API functions for later versions

void  hmemcpy(
  void _huge*       dest,
  const void _huge* source,
  long              size)

  {
  BYTE huge*  srcPtr = (BYTE huge*)source;
  BYTE huge*  dstPtr = (BYTE huge*)dest;

  WORD  srcOffset;
  WORD  dstOffset;
  DWORD copycount;

  while(size > 0L)
    {
    srcOffset = LOWORD((long)srcPtr);
    dstOffset = LOWORD((long)dstPtr);

    copycount = 0x00010000L - (srcOffset>dstOffset ? srcOffset : dstOffset);
    if(copycount > size)
      copycount = size;
    if(copycount > 0x0000FFFFL)
      copycount = 0x0000FFFFL;

    _fmemcpy((BYTE*)dstPtr, (BYTE*)srcPtr, (size_t)copycount);
    srcPtr  += copycount;
    dstPtr  += copycount;
    size    -= copycount;
    }
  }

#endif  // WINVER < 0x030a
// hmemmove -- huge pointer move ///////////////////////////////////////////

void  hmemmove(
  void _huge*       dest,
  const void _huge* source,
  long              size)

  {
  BYTE huge*  srcPtr = (BYTE huge*)source;
  BYTE huge*  dstPtr = (BYTE huge*)dest;

  WORD  srcOffset;
  WORD  dstOffset;
  DWORD movecount;

  while(size > 0L) {
    srcOffset = LOWORD((long)srcPtr);
    dstOffset = LOWORD((long)dstPtr);

    movecount = 0x00010000L - (srcOffset>dstOffset ? srcOffset : dstOffset);
    if(movecount > size)
      movecount = size;
    if(movecount > 0x0000FFFFL)
      movecount = 0x0000FFFFL;

    memmove((BYTE*)dstPtr, (BYTE*)srcPtr, (size_t)movecount);
    srcPtr  += movecount;
    dstPtr  += movecount;
    size    -= movecount;
    }
  }

// hstrlen /////////////////////////////////////////////////////////////////

long hstrlen(
  const char _huge* string)

  {
  long  len = 0L;
  
  if(string)
    {
    while(*(string+len))
      len++;
    }

  return len;
  }

// hstricmp ////////////////////////////////////////////////////////////////

int hstricmp(
  const char _huge* str1,
  const char _huge* str2)

  {
  while(*str1 && *str2)
    {
    if(LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))==LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2)))))
      {
      str1++;
      str2++;
      }
    else
      return LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))-LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2))));
    }

  return LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))-LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2))));
  }

// hstrcmp /////////////////////////////////////////////////////////////////

int hstrcmp(
  const char _huge* str1,
  const char _huge* str2)

  {
  while(*str1 && *str2)
    {
    if(*str1==*str2)
      {
      str1++;
      str2++;
      }
    else
      return *str1 - *str2;
    }

  return *str1 - *str2;
  }

// lfcmp ///////////////////////////////////////////////////////////////////

BOOL lfcmp(
  const LOGFONT*  lf1,
  const LOGFONT*  lf2)

  {
  if(lf1->lfHeight != lf2->lfHeight)
    return TRUE;

  if(lf1->lfWidth != lf2->lfWidth)
    return TRUE;

  if(lf1->lfEscapement != lf2->lfEscapement)
    return TRUE;

  if(lf1->lfOrientation != lf2->lfOrientation)
    return TRUE;

  if(lf1->lfWeight != lf2->lfWeight)
    return TRUE;

  if((lf1->lfItalic && !lf2->lfItalic)
      ||
     (!lf1->lfItalic && lf2->lfItalic))
    return TRUE;

  if((lf1->lfUnderline && !lf2->lfUnderline)
      ||
     (!lf1->lfUnderline && lf2->lfUnderline))
    return TRUE;

  if((lf1->lfStrikeOut && !lf2->lfStrikeOut)
      ||
     (!lf1->lfStrikeOut && lf2->lfStrikeOut))
    return TRUE;

  if(lf1->lfCharSet != lf2->lfCharSet)
    return TRUE;

  if(lf1->lfOutPrecision != lf2->lfOutPrecision)
    return TRUE;

  if(lf1->lfClipPrecision != lf2->lfClipPrecision)
    return TRUE;

  if(lf1->lfQuality != lf2->lfQuality)
    return TRUE;

  if(lf1->lfPitchAndFamily != lf2->lfPitchAndFamily)
    return TRUE;

  if(strcmp(lf1->lfFaceName, lf2->lfFaceName))
    return TRUE;

  return FALSE;
  }

// hmemset /////////////////////////////////////////////////////////////////

void hmemset(
  void _huge* s,
  int         c,
  long        n)

  {
  HPSTR str = (HPSTR)s;
  for(long i=0; i<n; i++)
    *(str+i) = c;
  }

// hmemicmp ////////////////////////////////////////////////////////////////

int hmemicmp(
  const char _huge* str1,
  const char _huge* str2,
  long              count)

  {
  long  i = 0;
  while(i<(count-1))
    {
    if(LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))==LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2)))))
      {
      str1++;
      str2++;
      }
    else
      return LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))-LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2))));

    i++;
    }

  return LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str1))))-LOBYTE(LOWORD(AnsiUpper((LPSTR)MAKELP(0, *str2))));
  }

// myStrtok ////////////////////////////////////////////////////////////////

char huge* myStrtok(
  char huge*  source,
  BOOL        tabDelimited)

  {
  static char huge* str;
  char huge*        ret;
  char huge*        tmp;

  if(source)
    str = source;
  tmp = str;
  ret = str;

  if(!*tmp)
    return NULL;

  if(!tabDelimited)
    {
    while(*tmp && (*tmp!='\r') && (*(tmp+1)!='\n'))
      tmp++;

    if((*tmp=='\r') && (*(tmp+1)=='\n'))
      {
      *tmp = 0;
      *(tmp+1)=0;
      str = tmp+2;
      }
    else if(!*tmp)
      str = tmp;
    }
  else
    {
    while(*tmp && (*tmp!='\t'))
      tmp++;

    if(*tmp=='\t')
      {
      *tmp = 0;
      str = tmp+1;
      }
    else if(!*tmp)
      str = tmp;
    }

  return ret;
  }

////////////////////////////////////////////////////////////////////////////
// Function to set the contents of a number object to a long or a float
// used in Tbl_func.cpp
////////////////////////////////////////////////////////////////////////////
BOOL Tbl_SetNmbrObj(OBJECTID n_oid, long double ldVal)
  {
  if (ONmbrGetType(n_oid) == ONMBR_REAL)
    ONmbrSetReal(n_oid, (long double)ldVal, TRUE);
  else
    ONmbrSetInteger(n_oid, (long)ldVal, FALSE);
  return TRUE;
  }

//////////////////////////////////////////////////////////////////////////
// function to get the contents of a nmb object in long format
// Used in tabl_func.cpp
//////////////////////////////////////////////////////////////////////////
long double Tbl_GetNmbrObj(OBJECTID n_oid)
  {
  long double ldVal = 0;
  if (ONmbrGetType(n_oid) == ONMBR_REAL)
    {
    ONmbrGetReal(n_oid, &ldVal);
    ldVal += 0.5;
    }
  else
    ldVal = (long double)ONmbrGetInteger(n_oid);

  return ldVal;
  }


void MultiUnlockData(OBJECTID oid, int dataIndex, int *lockCount)
  {
  while (*lockCount)
    {
    AObjUnlockData(oid, dataIndex);
    (*lockCount)--;
    }
  }


/*************************NUMBER UTILITY FUNC ***********/
long ONmbrConvertFromTextInteger
  (OBJECTID oid,
   LPSTR txt)
  {/* Nmbr_TextToLong */

  long rVal;
  OBJECTID  indexObj;
  if (oid)
    indexObj = oid;
  else
    indexObj = AObjCreate(OTYPE_NUMBER);

  /*returns the type the text represented*/
  ONmbrConvertFromString(indexObj,txt,FALSE);
  /* returns the integer value*/
  rVal = ONmbrGetInteger(indexObj);

  if (oid == NULL)
    AObjDestroy(indexObj);
  return rVal;
  }

long double ONmbrConvertFromTextReal
  (OBJECTID oid,
   LPSTR txt)
  {/* Nmbr_TextToLong */
  long double rVal;
  OBJECTID  indexObj;
  if (oid)
    indexObj = oid;
  else
    indexObj = AObjCreate(OTYPE_NUMBER);

  /*returns the type the text represented*/
  ONmbrConvertFromString(indexObj,txt,TRUE);
  /* returns the integer value*/
  ONmbrGetReal(indexObj, &rVal);

  if (oid == NULL)
    AObjDestroy(indexObj);
  return rVal;
  }
