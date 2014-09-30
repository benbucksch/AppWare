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
// Filename:	"listdata.cpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	LISTDATA class functions for object management
//
//
////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <a_alm.h>
#include <a_almutl.h>

#include <helpids.h>
#include <o_number.h>
#include "listdata.h"

#define 	MAX_EDIT	255
#define		DATACOUNT	4

////////////////////Function Prototypes for this File///////////////////////


////////////////////////////////////////////////////////////////////////////

LISTDATA::LISTDATA(AOBJECTID oid)

{

  char* string;
  
  if(oid!=NULL) {
    string = new char[MAX_EDIT+1];
    AObjGetName((theObject=oid), string, MAX_EDIT);
    alias = new char[lstrlen(string)+1];
    lstrcpy(alias, string);

    // insert '_' for ' ' for parsing and make sure no invalid chars
    if(isdigit(alias[0])) alias[0] = 'A'; // must start with an alpha char
    for(int i=0; alias[i]!=0; i++) {
      if(isspace(alias[i])) alias[i] = '_';
      if(!(isalnum(alias[i]) || alias[i]=='_')) alias[i]=0;
      }
    delete[] string;
    }
  else {
    theObject = 0L;
    alias = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////

void	LISTDATA::SetAlias(char* name)

{
  if(alias!=NULL) delete alias;

  alias = new char[lstrlen(name)+1];
  lstrcpy(alias, name);

  for(int i=0; alias[i]!=0; i++) {
    if(isspace(alias[i])) alias[i] = '_';
    }  
}

////////////////////////////////////////////////////////////////////////////

void	LISTDATA::Init(AOBJECTID oid, char* name)

{
  theObject = oid;

  if(alias!=NULL) delete alias;

  alias = new char[lstrlen(name)+1];
  lstrcpy(alias, name);
}
////////////////////////////////////////////////////////////////////////////
