//--------------------------------------------------------------------------
// File Name:  BindMeth.CPP
// Description.:
//	The Event Handlers (Methods) for the Bindery object.
//--------------------------------------------------------------------------
#include <windows.h>
#include <a_alm.h>
#include "bindery.h"

#ifdef __cplusplus
extern "C" {
#endif
void FAR PASCAL StartupMeth( pAEvtInfo, pAObjMessage );
#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------------
// FUNCTION:  StartupMeth
// DESCRIPTION: Responds to the AEVENT_STARTUP by posting a Startup signal
//--------------------------------------------------------------------------
void FAR PASCAL _export StartupMeth(pAEvtInfo, pAObjMessage)
{
   OBJECTID           oiBindery;
   BinderyData far*   pBinderyData;
   long               key;

   key = AOBJ_GETFIRSTKEY;
   while ((oiBindery = AObjGetNextObject(OTYPE_BINDERY, &key)) != 0)
	{
		pBinderyData = (BinderyData far*)AObjLockData( oiBindery, 0 );
      if ( pBinderyData != NULL )
      {  // register the Bindery object as an owner of the BinderyValue Text object
         AObjRegisterOwner( oiBindery, pBinderyData->ServerName,    0 );
         AObjRegisterOwner( oiBindery, pBinderyData->ObjNameFilter, 0 );
         AObjRegisterOwner( oiBindery, pBinderyData->ObjTypeFilter, 0 );
         AObjRegisterOwner( oiBindery, pBinderyData->ObjNameData,   0 );
         AObjRegisterOwner( oiBindery, pBinderyData->ObjTypeData,   0 );
         AObjRegisterOwner( oiBindery, pBinderyData->ObjIDData,     0 );

         AEvtPostSignalAtTail( oiBindery, 1 );
         AObjUnlockData( oiBindery, 0 );
      }
   }
}
