//---------------------------------------------------------------------------
// Bindery.h
//
// DESCRIPTION:
//    Header file for the 'NW Bindery' ALM.
//---------------------------------------------------------------------------

#ifndef NW_BIND_ALM_H

#define NW_BIND_ALM_H     1

#include <stdio.h>

#include <nwcalls.h>

#define OTYPE_BINDERY MAKETYPEID('B','i','n','D') // define the new type ID


#define BIND_SEGMENT_SIZE  128


extern NWCONN_HANDLE OBindGetConnHandle( OBJECTID oiBindObj, BOOL useDefaultServer );


// Exported functions
#ifdef __cplusplus
extern "C" {
#endif
LONG FAR PASCAL Admin(OBJECTID, pAObjMessage) ;
BOOL FAR PASCAL EditObjectDialogProc (HWND, UINT, UINT, LONG lParam) ;
LONG FAR PASCAL VerifyPassword (pAEvtInfo, pAObjMessage);
LONG FAR PASCAL ChangePassword (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL ListBinderyObjects (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL ListProperties (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL CreateObject (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL CreateProperty (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL MyDeleteObject (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL DeleteProperty (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL RenameObject (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL OpenBindery (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL CloseBindery (pAEvtInfo, pAObjMessage theSystem);

// **** new functions
LONG FAR PASCAL IsBinderyEmulation( pAEvtInfo, pAObjMessage theSystem );

// **** even newer functions
LONG FAR PASCAL ClearBinderyProperty( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetBinderyObjectInfo( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetBinderyPropertyInfo( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetPrimaryServer( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL ListBinderyObjectTypes( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL DoesBinderyObjectExist( pAEvtInfo, pAObjMessage theSystem );

// **** even newer yet functions
LONG FAR PASCAL DoesBindPropSupportDataType( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetDateTimeBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetNetAddrBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetNumberBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetTableBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetTextBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetTimeBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetDateTimeBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetNetAddrBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetNumberBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetTableBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetTextBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL SetTimeBindProp( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL GetBinderyUserInfo( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL ChangeBindObjSecurity( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL ChangeBindPropSecurity( pAEvtInfo, pAObjMessage theSystem );

// **** new functions added after Networld + Interop
LONG FAR PASCAL ArchiveBinderyFiles( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL RestoreBinderyFiles( pAEvtInfo, pAObjMessage theSystem );


#ifdef ZZZ_USE_OLD_CODE
// old functions no longer required
LONG FAR PASCAL GetObjectID (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL GetObjectName (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL GetCHandle (pAEvtInfo, pAObjMessage theSystem);
LONG FAR PASCAL ReadPropertyValue( pAEvtInfo, pAObjMessage theSystem );
LONG FAR PASCAL WritePropertyValue( pAEvtInfo, pAObjMessage theSystem );
#endif

#ifdef __cplusplus
}
#endif

LONG EditObject( OBJECTID oiBindery );
int  ConvertBindObjTypeTextToID( OBJECTID oiObjType );
int  ConvertBindObjTypeStrToID( char *objTypeStr );

// functions implemented in the source file BINDMISC.CPP
int  ConvertSecurityTextToNum( OBJECTID oiSecText, int rdOrWrtFlag );
int  ConvertSecurityStrToNum( char *secText, int rdOrWrtFlag );
BOOL IsObjectNameValid( OBJECTID oiObjName, BOOL allowWildCards );
BOOL IsPropertyNameValid( OBJECTID oiPropName );
int  ConvertObjNameToValid( char *objectName, char *dest, int destLen );
int  ConvertPropNameToValid( char *objectName, char *dest, int destLen );

// current major and minor version values
#define BIND_ALM_CURR_MAJ_VER   0
#define BIND_ALM_CURR_MIN_VER   1

typedef struct
{
   BYTE     majorVersion;
   BYTE     minorVersion;
   OBJECTID ServerName;
   OBJECTID ObjNameFilter;
   OBJECTID ObjTypeFilter;
   OBJECTID ObjNameData;
   OBJECTID ObjTypeData;
   OBJECTID ObjIDData;

} BinderyData;

extern HINSTANCE hInstance;

#define TRUSTEE_DIR_STR   "Trustee Directory Assignments"


#endif
