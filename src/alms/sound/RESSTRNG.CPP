////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "resstrng.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the Resource String class methods.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

// include files ///////////////////////////////////////////////////////////

#include <string.h>
#include <windows.h>

#include "resstrng.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ResString::ResString(UINT)  ---  constructor
//
//  DESCRIPTION:  This will load the string and create a copy of it.
//
////////////////////////////////////////////////////////////////////////////

ResString::ResString
  (
  UINT  uID
  ) : m_uID(uID),
      m_szString(NULL)

  {
  int   nLen;
  char  szStr[255];

  // load the string from the DLL's resource
  nLen = LoadString(g_hinstDLL, m_uID, szStr, sizeof(szStr));

  if(nLen)
    {
    // make sure the string is NULL terminated
    if(nLen == sizeof(szStr))
      szStr[sizeof(szStr)-1] = '\0';

    // allocate space for a local string
    m_szString = new char[nLen+1];

    // copy the resource string to the local resource
    if(m_szString)
      lstrcpy(m_szString, szStr);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       ResString::~ResString()  ---  destructor
//
//  DESCRIPTION:  This will free the memory associated with the string
//                resource.
//
////////////////////////////////////////////////////////////////////////////

ResString::~ResString
  (
  )

  {
  // NULL pointers will work correctly
  delete [] m_szString;
  }

////////////////////////////////////////////////////////////////////////////
