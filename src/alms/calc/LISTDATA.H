///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"listdata.h"
//
// Author:	Kevin Smart
//   Start:	July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	LISTDATA class header for object management
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

class LISTDATA {
  private:
///    char*			string;
		AOBJECTID	theObject;
    char*			alias;

  public:

    LISTDATA()								{theObject=0L; alias=NULL;}
		LISTDATA(AOBJECTID);
		~LISTDATA()								{if(alias!=NULL) delete[] alias;}

		char*			Alias()					{return alias;}
    AOBJECTID	TheObject()			{return theObject;}
    void			SetAlias(char*);
    void			Init(AOBJECTID, char*);
  };

////////////////////////////////////////////////////////////////////////////
