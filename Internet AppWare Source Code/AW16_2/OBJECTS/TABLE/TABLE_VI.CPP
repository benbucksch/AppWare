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
// FILE:  "table_vi.cpp"
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
#include  <stdlib.h>

#include  <a_alm.h>
#include  <op_wnd.h>
#include  <o_wnd.h>

#include  "tabl_hdr.h"  // for hInstance
#include  "table.h"
#include  "table_vi.h"

// operator new for the TABLE object ///////////////////////////////////////

void* TABLE::operator new(
  size_t    size,
  OBJECTID  objID)

  {
  TABLE_VI  vi_table(objID);

  return vi_table.NewVisualInstance(size);
  }

// operator delete for the TABLE object ////////////////////////////////////

void  TABLE::operator delete(
  void* table)

  {
  TABLE_VI  vi_table(((TABLE*)table)->theObject);

  vi_table.DeleteVisualInstance((TABLE*)table);
  }

// TABLE_VI::TABLE_VI //////////////////////////////////////////////////////

TABLE_VI::TABLE_VI(
  OBJECTID  objID)

  {
  theObject = objID;

  long  count = AObjGetDataCount(theObject);
  if(count <= NUM_TABLEDATA)
    numVisInst = 0;
  else
    numVisInst = (int)(AObjGetDataSize(theObject, DATA_VIMGR)/sizeof(VIMGR));
  }

// TABLE_VI::NewVisualInstance /////////////////////////////////////////////

TABLE* TABLE_VI::NewVisualInstance(
  size_t  size)

  {
  TABLE*  pTable;
  VIMGR*  viList;

  if(numVisInst==0)
    AObjSetDataCount(theObject, NUM_TABLEDATA+1L);

  AObjResizeData(theObject, DATA_VIMGR, (long)(numVisInst+1L)*sizeof(VIMGR));
  viList = (VIMGR*)AObjLockData(theObject, DATA_VIMGR);

  (viList+numVisInst)->theBlock = AMemAllocate((long)size);
  (viList+numVisInst)->table = (TABLE*)AMemLock((viList+numVisInst)->theBlock);
  numVisInst++;
  pTable = (viList+(numVisInst-1))->table;

  AObjUnlockData(theObject, DATA_VIMGR);

  return pTable;
  }

// TABLE_VI::DeleteVisualInstance //////////////////////////////////////////

void TABLE_VI::DeleteVisualInstance(
  TABLE*  table)

  {
  int     i=0;
  BOOL    found = FALSE;
  VIMGR*  viList = (VIMGR*)AObjLockData(theObject, DATA_VIMGR);
  while(i<numVisInst)
    {
    // search for the given pointer
    if (viList && ((viList+i)->table == table))
      {
      found = TRUE;

      // free the memory
      AMemFree((viList+i)->theBlock);

      // move the remaining blocks down....
      while(i<(numVisInst-1))
        {
        *(viList+i) = *(viList+(i+1));
        i++;
        }
      }

    i++;
    }

  AObjUnlockData(theObject, DATA_VIMGR);
  if(found)
    {
    numVisInst--;
    if(numVisInst==0)
      AObjSetDataCount(theObject, NUM_TABLEDATA);
    else
      AObjResizeData(theObject, DATA_VIMGR, (long)numVisInst*sizeof(VIMGR));
    }
  }

// TABLE_VI::GetVisualInstance /////////////////////////////////////////////

TABLE* TABLE_VI::GetVisualInstance(
  int vi_index)

  {
  TABLE*  pTable = NULL;
  VIMGR*  viList = (VIMGR*)AObjLockData(theObject, DATA_VIMGR);

  if(vi_index<numVisInst)
    pTable = (viList+vi_index)->table;
  AObjUnlockData(theObject, DATA_VIMGR);

  return pTable;
  }

////////////////////////////////////////////////////////////////////////////
