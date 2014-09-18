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
// FILE:  "table_vi.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef   __TABLE_VI_H  // prevent multiple includes
#define   __TABLE_VI_H

#ifndef   __TABLE_H
#include  "table.h"
#endif

// VIMGR ///////////////////////////////////////////////////////////////////

struct VIMGR

  {
  MEMBLOCKID  theBlock;
  TABLE*      table;
  };

// TABLE_VI ////////////////////////////////////////////////////////////////

class TABLE_VI

  {
  private:
    OBJECTID  theObject;
    int       numVisInst;

  public:
    TABLE_VI(OBJECTID objID);

    int       NumVisualInstances()        {return numVisInst;}

    TABLE*    NewVisualInstance(size_t size);
    void      DeleteVisualInstance(TABLE* table);
    TABLE*    GetVisualInstance(int vi_index);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __TABLE_VI_H
