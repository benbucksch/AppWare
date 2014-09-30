//--------------------------------------------------------------------------
// File Name:  BindMisc.CPP
// Description.:
//	   The Bindery ALM's internal functions.
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <o_text.h>    // defines OTYPE_TEXT
#include <o_number.h>  // defines OTYPE_NUMBER

#include <nwcalls.h>

#include "Bindery.h"
#include "nwdynlib.h"

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int ConvertSecurityTextToNum( OBJECTID oiSecText, int rdOrWrtFlag )
{
   char securityText[ 100 ];

   securityText[ 0 ] = '\0';

   // verify that passed object ID is valid and a Text object

   // OTextGetToBuffer( oiSecText, securityText, ???len??? );

   return ConvertSecurityStrToNum( securityText, rdOrWrtFlag );
   
}

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int ConvertSecurityStrToNum( char *secText, int rdOrWrtFlag )
{
   return 0;
}

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL IsObjectNameValid( OBJECTID oiObjName, BOOL allowWildCards )
{
   // are there any wildcard characters

   // are there any spaces

   // are there any other reserved characters

   // are there any non-printing characters

   return TRUE;
}

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL IsPropertyNameValid( OBJECTID oiPropName )
{
   // are there any wildcard characters

   // are there any spaces

   // are there any other reserved characters

   // are there any non-printing characters

   return TRUE;
}

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int ConvertObjNameToValid( char *objectName, char *dest, int destLen )
{
   // convert to upper case

   // convert spaces to underscores

   return 0;
}

#pragma argsused
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int ConvertPropNameToValid( char *objectName, char *dest, int destLen )
{
   // convert to upper case

   // convert spaces to underscores

   return 0;
}




