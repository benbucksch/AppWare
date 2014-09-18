//--------------------------------------------------------------------------
// File Name:  BindAdmn.CPP
// Description.:
//	The Administration Routine for the Bindery object.
//--------------------------------------------------------------------------
#include <windows.h>
#include <a_alm.h>

#include "bindery.h"

#include "NWDynLib.h"


LONG CreateObject( OBJECTID oiMHS );
LONG CheckObject( OBJECTID oiMHS, pAObjMessage theSystem );
LONG ValidateObjectIDs( OBJECTID oiMHS, pAObjMessage theSystem );
LONG BuildSignals( pAObjMessage theSystem );
LONG CheckVersion( OBJECTID oiMHS );


HINSTANCE hInstance;

//--------------------------------------------------------------------------
// Function:  AdministrationRoutine
// Description:  Creates the and initializes the data structure for
//   a Bindery Object when the AOBJ_CREATED message is received
//--------------------------------------------------------------------------
LONG FAR PASCAL _export Admin( OBJECTID oiBindery, pAObjMessage theSystem )
{
	switch( theSystem->message1 )
		{
		case AOBJ_CREATED:
			return CreateObject( oiBindery );

		case AOBJ_EDITOBJECT:
			return EditObject( oiBindery );

		case AOBJ_CHECKOBJECT:
			return CheckObject( oiBindery, theSystem );

		case AOBJ_VALIDATEOBJECTIDS :
			return ValidateObjectIDs( oiBindery, theSystem );

		case AOBJ_READ :
			return CheckVersion( oiBindery );
		}
	return A_NOTHANDLED;
}

//--------------------------------------------------------------------------
//  Function:  CreateObject
//  Description:  Creates a bindery object 
//--------------------------------------------------------------------------
LONG CreateObject(OBJECTID oiBindery)
{
   BinderyData far *  pBinderyData;
   LONG               retValue = A_ERROR;

   AObjSetDataCount( oiBindery, 1 );
   AObjResizeData( oiBindery, 0, sizeof(BinderyData) );
   pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
   if ( pBinderyData != NULL )
   {  // initialize the data structure elements
      pBinderyData->majorVersion = BIND_ALM_CURR_MAJ_VER;
      pBinderyData->minorVersion = BIND_ALM_CURR_MIN_VER;

      // initialize all OBJECTIDs to zero
      pBinderyData->ServerName    = 0;
      pBinderyData->ObjNameFilter = 0;
      pBinderyData->ObjTypeFilter = 0;
      pBinderyData->ObjNameData   = 0;
      pBinderyData->ObjTypeData   = 0;
      pBinderyData->ObjIDData     = 0;

      AObjUnlockData( oiBindery, 0 );
      retValue = A_OK;
   }
   else
      MessageBox( NULL, "The Bindery object creation failed", NULL, MB_OK );

   return retValue;

} // end of CreateObject( )

//--------------------------------------------------------------------------
//  CheckObject
//  Verifies that the Bindery object is ready to be compiled.
//--------------------------------------------------------------------------
LONG CheckObject( OBJECTID oiBindery, pAObjMessage theSystem )
{
   BinderyData far*   lpBinderyData;
   pAObjInfo          lpObjectInfo;
   LONG               lreturn = A_OK;

   if ( ! dynNWLib.IsNWLibLoaded( ) )
   {
      lreturn = A_ERROR;
      lstrcpy( lpObjectInfo->errorMessage,
               "NetWare DLLs not present - can design, but not execute !" );
   }
   else
   {
      lpBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      lpObjectInfo  = (pAObjInfo)theSystem->message2;
#ifdef ZZZ_ALL_FIELDS_REQD
      if ( lpBinderyData != NULL )
      { 
         if ( lpBinderyData->ObjNameFilter == 0 )
         {
            lreturn = A_ERROR;
            lstrcpy( lpObjectInfo->errorMessage,
               "No Text object associated with the Object Name Filter" );
         }
         else if ( lpBinderyData->ObjTypeFilter == 0 )
         {
            lreturn = A_ERROR;
            lstrcpy( lpObjectInfo->errorMessage,
                     "No Number object associated with the Object Type Filter") ;
         }
         else if ( lpBinderyData->ObjIDData == 0 )
         {
            lreturn = A_ERROR;
            lstrcpy( lpObjectInfo->errorMessage,
                     "No Number object associated with the Object ID" );
         }
         else if ( lpBinderyData->ServerName == 0 )
         {
            lreturn = A_ERROR;
            lstrcpy( lpObjectInfo->errorMessage,
                     "No Text object associated with the Server Name" );
         }
         else if ( lpBinderyData->ObjNameData == 0 )
         {
            lreturn = A_ERROR;
            lstrcpy( lpObjectInfo->errorMessage,
                     "No Number object associated with the Connection Handle" );
         }
         else
            lreturn = A_OK;
      }
#endif

      AObjUnlockData( oiBindery, 0 );
   } // end of ELSE NetWare DLLs are initialized

   if ( lreturn == A_ERROR )
      AObjReportError( oiBindery, AOBJ_CHECKOBJECT, A_ERROR, lpObjectInfo->errorMessage, 0 );

	return lreturn;

} // end of CheckObject( )

//--------------------------------------------------------------------------
//  ValidateObjectIDs
//--------------------------------------------------------------------------
LONG ValidateObjectIDs( OBJECTID oiBindery, pAObjMessage theSystem )
{
   BinderyData far* lpBinderyData;

   lpBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
   if ( lpBinderyData != NULL )
   {  // exchange old object IDs for new ones
      AObjValidateID( &lpBinderyData->ServerName,    theSystem );
      AObjValidateID( &lpBinderyData->ObjNameFilter, theSystem );
      AObjValidateID( &lpBinderyData->ObjTypeFilter, theSystem );
      AObjValidateID( &lpBinderyData->ObjNameData,   theSystem );
      AObjValidateID( &lpBinderyData->ObjTypeData,   theSystem );
      AObjValidateID( &lpBinderyData->ObjIDData,     theSystem );
      AObjUnlockData( oiBindery, 0 );
   }
#ifdef ADBG_DEBUG
   else
   {  // report LOCK failure
   }
#endif
   return A_OK;

} // end of ValidateObjectIDs( )


//--------------------------------------------------------------------------
LONG CheckVersion( OBJECTID oiBindery )
//--------------------------------------------------------------------------
{
   char buffer[110];

   if ( AObjGetDataSize(oiBindery, 0) != sizeof(BinderyData) )
   {
      wsprintf( buffer, "BINDERY.DLL expects %d bytes to be allocated for Bindery object data, but %d are allocated.",
                (int)sizeof(BinderyData), (int)AObjGetDataSize(oiBindery, 0)) ;
      AObjReportError( oiBindery, AOBJ_READ, A_WARNING, buffer, 0 );
      return A_WARNING;
   }
   return A_OK ;
}

//--------------------------------------------------------------------------
// Function:  LibMain
// Description:
//	Marks global memory allocated by the DLL with the new operator sharable.
//  Initialized the hInstance global variable
//  Unlocks the local heap if there is one.
//--------------------------------------------------------------------------
int FAR PASCAL LibMain(HINSTANCE hInst, WORD, WORD wHeapSize, LPSTR )
{
#ifdef __BORLANDC__
   extern int _WinAllocFlag;

   _WinAllocFlag = GMEM_SHARE;  // Used by Borland compilers
#endif

   hInstance = hInst;
   if ( wHeapSize != 0 )
      UnlockData( 0 );

   // attempt to load the NetWare Library DLLs if not already loaded
   dynNWLib.LoadNWLib( );

   return 1;

} // end of LibMain( )
