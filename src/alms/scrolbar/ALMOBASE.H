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
// FILE:          "almobase.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is a base class for ALM objects.  This will handle
//                the basic Administration function messages.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef ALMOBASE_H
#define ALMOBASE_H

// includes ////////////////////////////////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

// ALMOBase class definition ///////////////////////////////////////////////

class ALMOBase

  {
  public:
                        ALMOBase(AOBJECTID oi, BOOL fTemp);
    virtual            ~ALMOBase();

    AOBJECTID            ObjectID()                {return m_oiSelf;}

    // utility to validate correct construction of the object
    virtual BOOL        Valid() const             = 0L;

    // version checking function to upgrade object
    static long         CheckVersion(AOBJECTID oi);

    // initialize the object from Object Data
    virtual void        InitializeFromData()      = 0L;

    // AppWare administration function message handlers
    virtual long        Assign(const ALMOBase* pSrc);
    virtual long        BuildSignals(AMEMBLOCKID mi) const;
    virtual long        Check() const;
    virtual long        Edit();
    virtual long        Export() const;
    virtual long        Import();
    virtual long        PostWrite();
    virtual long        PreWrite();
    static long         QueryProtocol(AOBJECTID oi, pApQueryInfo pQI);
    static long         ReportRequiredFiles(pAObjMessage pMsg);
    virtual long        ValidateIDs(pAObjMessage pMsg);

  private:
    AOBJECTID            m_oiSelf;       // ALM object id
  };


////////////////////////////////////////////////////////////////////////////

#endif // #ifndef ALMOBASE_H
