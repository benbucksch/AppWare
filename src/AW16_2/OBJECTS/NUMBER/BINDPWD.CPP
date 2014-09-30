//--------------------------------------------------------------------------
// File Name:  BindPwd.CPP
// Description.:
//    The Bindery object's password related functions.
//--------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <o_text.h>   // defines OTYPE_TEXT
#include <o_number.h> // defines OTYPE_NUMBER

#include <nwcalls.h>

#include "Bindery.h"
#include "nwdynlib.h"


//--------------------------------------------------------------------------
// FUNCTION:  VerifyPassword
//
// DESCRIPTION:  Verifies object password
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//	   2: Text object holding password to verify
//
// INPUT PARAMETERS
//    None
//
// FLOWS
//    1: Verified
//    2: Not verified
//	   3: No such object
//    4: Bad pwd fmt
//    5: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export VerifyPassword( pAEvtInfo, pAObjMessage theSystem )
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far*   pBinderyData; // pointer to a structure with default values
   OBJECTID           oiText;       // ID of password text
   WORD               Type;
   NWCCODE            ccode;
   char               Name[50];
   char               Password[150];
   NWCONN_HANDLE      cHandle;
   int                retFlow = 5; // assume 'Error' flow

   if ( dynNWLib.IsNWLibLoaded( ) )
   {
      oiBindery = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
      if ( oiBindery != 0 )
      {
         pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {
            oiText = AFuncGetTypedParameter ( 2, OTYPE_TEXT );
            if ( oiText != 0 )
            {  // Get values and make call
               Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
               if ( Type != 0 )
               {
                  OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );
                  OTextGetToBuffer( oiText, Password, 128 );
                  strupr( Password );
 
                  // get connection handle based on server name
                  cHandle = OBindGetConnHandle( oiBindery, TRUE );
  
                  // ??? check return value from previous function ???

                  ccode = nwfpNWVerifyObjectPassword( cHandle, Name, Type, Password );
                  if ( ccode == SUCCESSFUL )
                  {
                     retFlow = 1; // flow = 'Verified'
                  }
//                else if ( ccode == NO_SUCH_OBJECT_OR_BAD_PASSWORD )
                  else
                  {
                     retFlow = 2; // flow = 'Not verified'
                  }
               }
               else
                  retFlow = 5; // flow = 'Error'
            }
#ifdef ADBG_DEBUG
            else
            {  // input Text object ID is invalid
               ADBG_PRINT_W( "Bindery VerifyPwd: invalid input Text object ID" );
            }
#endif
            AObjUnlockData( oiBindery, 0 );
         }
#ifdef ADBG_DEBUG
         else
         {  // a call to AObjLock( ) failed for data block zero
            ADBG_PRINT_W( "Bindery VerifyPwd: data LOCK failed" );
         }
#endif
      } // end of if Bindery object ID is valid
#ifdef ADBG_DEBUG
      else
      {  // Bindery object ID is invalid
         ADBG_PRINT_W( "Bindery VerifyPwd: invalid input Bindery object ID" );
      }
#endif
   }

   theSystem->message1 = retFlow;
	return 0;

} // end of VerifyPassword( )

//--------------------------------------------------------------------------
// FUNCTION:  ChangePassword
//
// DESCRIPTION:  Changes object password
//
// INPUT PARAMETERS
//    1: Bindery object holding input values
//	   2: Text object holding OLD password
//	   3: Text object holding NEW password
//
// OUTPUT PARAMETERS
//    None
//
// FLOWS
//    1: Changed
//	   2: No such object
//    3: Invalid pwd fmt
//    4: Wrong current pwd
//    5: No rights
//    6: Error
//--------------------------------------------------------------------------
LONG FAR PASCAL _export ChangePassword (pAEvtInfo, pAObjMessage theSystem)
{
   OBJECTID           oiBindery;    // the ID of the Bindery Object
   BinderyData far *  pBinderyData; // pointer to a structure with default values
   OBJECTID           oiOldPwd, oiNewPwd; // IDs of password text
   WORD               Type;
   NWCCODE            ccode;
   char               Name[50];
   char               NewPassword[150], OldPassword[150];
   NWCONN_HANDLE      cHandle;
   int                retFlow = 3;  // assume 'Error' flow

   if ( dynNWLib.IsNWLibLoaded( ) )
   {
      oiBindery  = AFuncGetTypedParameter( 1, OTYPE_BINDERY );
      oiOldPwd   = AFuncGetTypedParameter( 2, OTYPE_TEXT );
      oiNewPwd   = AFuncGetTypedParameter( 3, OTYPE_TEXT );
      if ( (oiBindery != 0) && (oiNewPwd != 0) && (oiOldPwd != 0) )
      {
         pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
         if ( pBinderyData != NULL )
         {  // Get values and make call
            Type = ConvertBindObjTypeTextToID( pBinderyData->ObjTypeData );
            if ( Type != 0 )
            {
               OTextGetToBuffer( pBinderyData->ObjNameData, Name, 48 );
               OTextGetToBuffer( oiOldPwd, OldPassword, 128 );
               OTextGetToBuffer( oiNewPwd, NewPassword, 128 );
               strupr( OldPassword );
               strupr( NewPassword );

               // get connection handle based on server name
               cHandle = OBindGetConnHandle( oiBindery, TRUE );

               // ??? check return value from previous function ???

               ccode = nwfpNWChangeObjectPassword( cHandle, Name, Type, OldPassword, NewPassword );

               if ( ccode == SUCCESSFUL )
               {
                  retFlow = 1;
               }
               else if ( ccode == NO_SUCH_OBJECT_OR_BAD_PASSWORD )
               {
                  retFlow = 2;
               }
            }
            AObjUnlockData( oiBindery, 0 );
         } // end of IF AObjLock( ) was successful
#ifdef ADBG_DEBUG
         else
         {  // a call to AObjLock( ) failed for data block zero
            ADBG_PRINT_W( "Bindery ChangePwd: data LOCK failed" );
         }
#endif
      } // end of IF able to obtain all input parameter IDs successfully
#ifdef ADBG_DEBUG
      else
      {  // Bindery, old pwd, or new pwd object ID is invalid
         ADBG_PRINT_W( "Bindery ChangePwd: invalid input object ID(s)" );
      }
#endif
   } // end of IF NetWare DLLs are present

   theSystem->message1 = retFlow; // return the desire flow
   return 0;

} // end of ChangePassword( )
