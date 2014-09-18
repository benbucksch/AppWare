/* ****************************************************************
 *
 * Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.
 *
 * THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
 * TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
 * CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 * TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
 * NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
 * AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
 * LIABILITY.
 *
 * **************************************************************** */

#ifndef A_ALM_H
#define A_ALM_H

/* ****************************************************************
 * AppWare Kernel Version 1.0
 * **************************************************************** */

#define A_KERNELVER           0x0100

#ifndef WINDOWS_H
#include <windows.h>
#ifndef WINDOWS_H
#define WINDOWS_H
#endif /* !WINDOWS_H */
#endif /* !WINDOWS_H */

#define ALMAPI      FAR PASCAL _export
#define ALMCAPI     FAR CDECL  _export
#define ALMCALLBACK FAR PASCAL _export

#include <ap_macro.h>

/* ****************************************************************
 * Type definitions
 * **************************************************************** */

#ifdef ASTRICT
#define ADECL_TYPE(t)       struct t##__ {short astrict;};     \
                            typedef const struct t##__ FAR* t
#else
#define ADECL_TYPE(t)       typedef long t
#endif  /* ASTRICT */

ADECL_TYPE(TYPEID);
ADECL_TYPE(OBJECTID);
ADECL_TYPE(EVENTID);
ADECL_TYPE(MEMBLOCKID);
ADECL_TYPE(PROTOCOLID);

typedef TYPEID   FAR*   LPTYPEID;
typedef OBJECTID FAR*   LPOBJECTID;

/* ****************************************************************
 * ID Creation macros
 * **************************************************************** */

#define MAKEID(a,b,c,d)           (   \
            ((long) (a) << 24) +      \
            ((long) (b) << 16) +      \
            ((long) (c) <<  8) +      \
            ((long) (d)      )        \
          )

#define MAKETYPEID(a,b,c,d)       ((TYPEID)     MAKEID(a,b,c,d))
#define MAKEEVENTID(a,b,c,d)      ((EVENTID)    MAKEID(a,b,c,d))
#define MAKEPROTOCOLID(a,b,c,d)   ((PROTOCOLID) MAKEID(a,b,c,d))

/* ****************************************************************
 * UPSF Types
 * **************************************************************** */

/* Universal (UPSF) Types */
#define AUPSF_UNIV_NUMBER         MAKEID('U', 'N', 'u', 'm')
#define AUPSF_UNIV_INTEGER        MAKEID('U', 'I', 'n', 't')
#define AUPSF_UNIV_BOOL           AUPSF_UNIV_INTEGER
#define AUPSF_UNIV_TEXT           MAKEID('U', 'T', 'x', 't')
#define AUPSF_UNIV_STRING         AUPSF_UNIV_TEXT
#define AUPSF_UNIV_BITMAP         MAKEID('U', 'B', 'i', 't')
#define AUPSF_UNIV_OBJECTREF      MAKEID('U', 'R', 'e', 'f')
#define AUPSF_UNIV_RECT           MAKEID('U', 'R', 'c', 't')
#define AUPSF_UNIV_POINT          MAKEID('U', 'P', 'n', 't')
#define AUPSF_UNIV_FONT           MAKEID('U', 'F', 'o', 'n')
#define AUPSF_UNIV_COLOR          MAKEID('U', 'C', 'l', 'r')

/* Native Types */
#define AUPSF_NATV_BYTE           MAKEID('B', 'y', 't', 'e')
#define AUPSF_NATV_SBYTE          MAKEID('C', 'h', 'a', 'r')
#define AUPSF_NATV_CHAR           AUPSF_NATV_SBYTE
#define AUPSF_NATV_UCHAR          AUPSF_NATV_BYTE

#define AUPSF_NATV_WORD           MAKEID('W', 'o', 'r', 'd')
#define AUPSF_NATV_SWORD          MAKEID('S', 'h', 'r', 't')
#define AUPSF_NATV_SHORT          AUPSF_NATV_SWORD
#define AUPSF_NATV_USHORT         AUPSF_NATV_WORD
#define AUPSF_NATV_INT            AUPSF_NATV_SWORD
#define AUPSF_NATV_UINT           AUPSF_NATV_WORD

#define AUPSF_NATV_BOOL           MAKEID('B', 'o', 'o', 'l')

#define AUPSF_NATV_DWORD          MAKEID('D', 'w', 'r', 'd')
#define AUPSF_NATV_SDWORD         MAKEID('L', 'o', 'n', 'g')
#define AUPSF_NATV_LONG           AUPSF_NATV_SDWORD
#define AUPSF_NATV_ULONG          AUPSF_NATV_DWORD

#define AUPSF_NATV_FLOAT          MAKEID('F', 'l', 'o', 't')
#define AUPSF_NATV_DOUBLE         MAKEID('D', 'b', 'l', 'e')
#define AUPSF_NATV_LONGDOUBLE     MAKEID('L', 'd', 'b', 'l')
#define AUPSF_NATV_STRING         MAKEID('S', 't', 'r', 'g')
#define AUPSF_NATV_BITMAPINFO     MAKEID('B', 'i', 'n', 'f')
#define AUPSF_NATV_BITMAPCORE     MAKEID('B', 'c', 'o', 'r')
#define AUPSF_NATV_BITMAP         MAKEID('B', 't', 'm', 'p')
#define AUPSF_NATV_METAFILE       MAKEID('M', 'e', 't', 'F')
#define AUPSF_NATV_HBITMAP        MAKEID('H', 'b', 'm', 'p')
#define AUPSF_NATV_HBRUSH         MAKEID('H', 'b', 'r', 's')

#define AUPSF_NATV_OBJECTREF      MAKEID('O', 'b', 'j', 'R')

#define AUPSF_NATV_FONT           MAKEID('F', 'o', 'n', 't')
#define AUPSF_NATV_RECT           MAKEID('R', 'e', 'c', 't')
#define AUPSF_NATV_POINT          MAKEID('P', 'o', 'n', 't')
#define AUPSF_NATV_RGBCOLOR       MAKEID('R', 'G', 'B', 'C')

/* ****************************************************************
 * Structure definitions
 * **************************************************************** */

/* AppWare Event */
typedef struct tagAEvtInfo
  {
  OBJECTID    sourceObject;
  OBJECTID    targetObject;
  EVENTID     what;
  LONG        details;
  LONG        modifiers;
  LONG        when;
  LONG        how;
  LONG        variation;
  }  AEvtInfo, FAR* pAEvtInfo;

/* Object Message */
typedef struct tagAObjMessage
  {
  LONG    message1;
  LONG    message2;
  LONG    message3;
  LONG    message4;
  char    errorMessage[256];
  }  AObjMessage, FAR* pAObjMessage;

/* Signal Record */
typedef struct tagAObjSignalRecord
  {
  LONG    theID;
  char    theName[256];
  }  AObjSignalRecord, FAR* pAObjSignalRecord;

/* Object Information Record */
typedef struct tagAObjInfo
  {
  char            errorMessage[256];
  LONG            typeOverhead;
  LONG            objectOverhead;
  } AObjInfo, FAR* pAObjInfo;

/* Object Ownership Record */
typedef struct tagAObjOwnerInfo
  {
  OBJECTID  ownerID;
  LONG      ownerData;
  LONG      key;
  LONG      reserved1;
  } AObjOwnerInfo, FAR* pAObjOwnerInfo;

/* Protocol */
typedef struct tagApQueryInfo       ApQueryInfo;
typedef LPVOID (ALMAPI* APPCFN)(ApQueryInfo FAR*);

typedef struct tagApQueryInfo
  {
  LONG        size;
  OBJECTID    object;
  TYPEID      type;
  PROTOCOLID  protocol;
  APPCFN      pfnCreate;
  } ApQueryInfo, FAR* pApQueryInfo;


/* ****************************************************************
 * Constant definitions
 * **************************************************************** */

/* Miscellaneous Constants */
#define AOBJ_GETFIRSTKEY            (-1L)

/* Application priority modes */
#define AAPP_OBJECTPRIORITY         101L
#define AAPP_VISUALPRIORITY         102L
#define AAPP_FUNCTIONPRIORITY       103L


/* AEvtInfo.how -- event posting descriptors */
#define AEVT_ATTAIL                 0x00000001L
#define AEVT_ATMARK                 0x00000002L
#define AEVT_ATONCE                 0x00000004L

#define AEVT_KILLWHAT               0x00000100L
#define AEVT_KILLTARGET             0x00000200L
#define AEVT_KILLDETAILS            0x00000400L
#define AEVT_KILLSOURCE             0x00000800L
#define AEVT_KILLMODIFIERS          0x00001000L
#define AEVT_KILLWHEN               0x00002000L
#define AEVT_KILLVARIATION          0x00004000L

#define AEVT_KILLSELF               0x00800000L
#define AEVT_KILLALLQUEUES          0x00400000L

#define AEVT_KILLSIMILARS           (AEVT_KILLWHAT)
#define AEVT_KILLDUPLICATES         (AEVT_KILLWHAT|AEVT_KILLTARGET)
#define AEVT_KILLPARTICULARS        (AEVT_KILLWHAT|AEVT_KILLTARGET|AEVT_KILLDETAILS)

#define AEVT_DIRECT                 0x01000000L
#define AEVT_SELF                   0x02000000L

/*** Administration routine messages ****************/

/* State changes */
#define AOBJ_CREATED                1L      /* request                     */
#define AOBJ_AWAKENED               2L      /* notification                */
#define AOBJ_ASLEEP                 3L      /* notification                */
#define AOBJ_DESTROYED              4L      /* notification                */

/* I/O */
#define AOBJ_READ                   6L      /* request                     */
#define AOBJ_PREWRITE               8L      /* notification                */
#define AOBJ_POSTWRITE              10L     /* notification                */

/* Miscellaneous */
#define AOBJ_ASSIGNOBJECT           11L     /* request                     */
#define AOBJ_IMPORTOBJECT           12L     /* request                     */
#define AOBJ_EXPORTOBJECT           13L     /* request                     */
#define AOBJ_QUERYPROTOCOL          14L     /* request                     */

/* Definition time only */
#define AOBJ_BUILDSIGNALS           15L     /* request                     */
#define AOBJ_CHECKOBJECT            16L     /* request                     */
#define AOBJ_EDITOBJECT             17L     /* request                     */
#define AOBJ_VALIDATEOBJECTIDS      18L     /* request                     */
#define AOBJ_REPORTREQUIREDFILES    19L     /* request                     */

/* message4 values */
#define AOBJ_TEMPORARY              1L      /* in AOBJ_CREATED             */
#define AOBJ_PROJECTSAVE            2L      /* in AOBJ_{PRE,POST}WRITE     */

/*** Valid return values from the Admin procedure ****************/
/* Common values passed in lower 8 bits */
#define A_NOTHANDLED                0L      /* for unprocessed msgs        */
#define A_OK                        1L      /* for requests & notifications*/
#define A_WARNING                   2L      /* for requests                */
#define A_ERROR                     3L      /* for requests                */
#define A_MODIFIED                  4L      /* for AOBJ_EDITOBJECT only    */
#define A_CONTINUE                  5L      /* for notifications only      */

/* Bit values next 8 bits */
#define AP_NOTSUPPORTED             0x0100  /* not supported by obj or type*/
#define AP_DYNAMIC                  0x0200  /* supported some of the time  */
#define AP_SUPPORTED                0x0400  /* supported                   */

/**** AppWare Event IDs  (passed in 'what' of Event record) ****************/

/* Text-Interchange Events */
#define AEVENT_OBJECTTOTEXT         MAKEEVENTID('o','t','o','T')
#define AEVENT_TEXTTOOBJECT         MAKEEVENTID('t','t','o','O')

/* AppWare System Events */
#define AEVENT_STARTUP              MAKEEVENTID('s','t','r','T')
#define AEVENT_SHUTDOWN             MAKEEVENTID('s','t','o','P')
#define AEVENT_SIGNAL               MAKEEVENTID('s','g','n','L')
#define AEVENT_OBJECTCHANGED        MAKEEVENTID('o','b','j','C')
#define AEVENT_CALLFUNCTION         MAKEEVENTID('c','a','l','F')

#define AEVENT_OS_ACTIVATEAPP       MAKEEVENTID('a', 'c', 't', 'A')  /* from WM_ACTIVATEAPP */


/*** UPSF Constants ****************/

/* General Constants for UPSF routines */
#define AUPSF_IDLENGTH              8

/* Error Constants for UPSF routines */
#define AUPSF_NOERROR               (  0L)
#define AUPSF_FILEERROR             ( -1L)
#define AUPSF_MEMORYERROR           ( -2L)
#define AUPSF_FILECLOSED            ( -3L)

#define AUPSF_CANTINITEXPORT        ( -4L)
#define AUPSF_EXPORTERROR           ( -5L)
#define AUPSF_NOTEXPORTING          ( -6L)

#define AUPSF_CANTCONVERT           ( -7L)
#define AUPSF_SIZEERROR             ( -8L)
#define AUPSF_INVALIDOBJECT         ( -9L)

#define AUPSF_CANTINITIMPORT        (-10L)
#define AUPSF_IMPORTERROR           (-11L)
#define AUPSF_NOTIMPORTING          (-12L)

#define AUPSF_SCOPEERROR            (-13L)
#define AUPSF_SCOPENOTEXIST         (-14L)
#define AUPSF_FIELDNOTEXIST         (-15L)
#define AUPSF_LOGERROR              (-16L)

#define AUPSF_OUTOFDISKSPACE        (-17L)

/* ****************************************************************
 * Base Protocol definition
 * **************************************************************** */

APROT_BEGINPROTOCOL_(ApObjectD)

  APROT_BEGINSERVICES_(ApObjectD)
  APROT_SERVICE(void, Destroy,          (APROT_VOID(ApObjectD)))
  APROT_SERVICE(LONG, QueryProtocol,    (APROT_THIS(ApObjectD) pApQueryInfo, PROTOCOLID))
  APROT_SERVICE(OBJECTID, Object,       (APROT_VOID(ApObjectD)))
  APROT_SERVICE(TYPEID, Type,           (APROT_VOID(ApObjectD)))
  APROT_ENDSERVICES_(ApObjectD)

  OBJECTID    m_oiObject;

APROT_ENDPROTOCOL_(ApObjectD);

#ifndef __cplusplus
/* Field access for 'C' */
#define APOBJ_OBJ(p)                (((ApObjectD FAR*)(p))->m_Obj)
#define APOBJ_VPTR(p)               (((ApObjectD FAR*)(p))->m_vptr)

/* Method access for 'C' */
#define APOBJ_CALLMETHOD(m,p)       (*((APOBJ_VPTR(p))->m))

#endif  /* !__cplusplus */

#define APTYPE_OBJECT               MAKEPROTOCOLID('P','o','b','j')
typedef ApObjectD FAR*              (ALMAPI* ApObjCreateFN)(OBJECTID);
typedef LONG                        (ALMAPI* ApObjInitFN)(OBJECTID);

/* ****************************************************************
 * ****************************************************************
 * Kernel APIs
 * ****************************************************************
 * **************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ****************************************************************
 * Debug support
 * **************************************************************** */

#ifdef ADBG_DEBUG
/* Passed in 'nType' parameter */
#define ADBG_INFO                   0
#define ADBG_WARNING                1
#define ADBG_ERROR                  2
#define ADBG_UNKNOWN                3

short       ALMCAPI ADbgPrint(short nType, TYPEID t, LPSTR pszFmt, ...);
short       ALMCAPI ADbgPrintRes(short nType, TYPEID t, HINSTANCE hInst, WORD idRes, ...);

#define ADBG_ASSERT_I(e,r,s)        if (!(e)) {ADbgPrint(ADBG_INFO,    0, "%s", (s));r;}
#define ADBG_ASSERT_W(e,r,s)        if (!(e)) {ADbgPrint(ADBG_WARNING, 0, "%s", (s));r;}
#define ADBG_ASSERT_E(e,r,s)        if (!(e)) {ADbgPrint(ADBG_ERROR,   0, "%s", (s));r;}

#define ADBG_ASSERT_RES_I(e,r,h,i)  if (!(e)) {ADbgPrintRes(ADBG_INFO,    0, h, i);r;}
#define ADBG_ASSERT_RES_W(e,r,h,i)  if (!(e)) {ADbgPrintRes(ADBG_WARNING, 0, h, i);r;}
#define ADBG_ASSERT_RES_E(e,r,h,i)  if (!(e)) {ADbgPrintRes(ADBG_ERROR,   0, h, i);r;}

#define ADBG_PRINT_I(s)             (ADbgPrint(ADBG_INFO,    0, "%s", (s)))
#define ADBG_PRINT_W(s)             (ADbgPrint(ADBG_WARNING, 0, "%s", (s)))
#define ADBG_PRINT_E(s)             (ADbgPrint(ADBG_ERROR,   0, "%s", (s)))

#define ADBG_PRINT_RES_I(h,i)       (ADbgPrintRes(ADBG_INFO,    0, h, i))
#define ADBG_PRINT_RES_W(h,i)       (ADbgPrintRes(ADBG_WARNING, 0, h, i))
#define ADBG_PRINT_RES_E(h,i)       (ADbgPrintRes(ADBG_ERROR,   0, h, i))

#define ADBG_EVAL(x)                x
#else
#define ADBG_ASSERT_I(e,r,s)  
#define ADBG_ASSERT_W(e,r,s)  
#define ADBG_ASSERT_E(e,r,s)  
#define ADBG_ASSERT_RES_I(e,r,h,i)
#define ADBG_ASSERT_RES_W(e,r,h,i)
#define ADBG_ASSERT_RES_E(e,r,h,i)
#define ADBG_PRINT_I(s)
#define ADBG_PRINT_W(s)      
#define ADBG_PRINT_E(s)      
#define ADBG_PRINT_RES_I(h,i)
#define ADBG_PRINT_RES_W(h,i)
#define ADBG_PRINT_RES_E(h,i)
#define ADBG_EVAL(x)
#endif /* ADBG_DEBUG */

/* ****************************************************************
 * Events and Signals APIs
 * **************************************************************** */

/* General Purpose */
BOOL        ALMAPI  AEvtPost(pAEvtInfo theEvent);

BOOL        ALMAPI  AEvtPostStandard(OBJECTID theObject, EVENTID what,
                LONG details, LONG how);

BOOL        ALMAPI  AEvtPostSignalAtTail(OBJECTID theObject, LONG signalID);

BOOL        ALMAPI  AEvtPostSignalAtMark(OBJECTID theObject, LONG signalID);

/* Miscellaneous Event Functions */
LONG        ALMAPI  AEvtGetObjectChangedPriority(LONG preferred);

/* Inter-object Messages */
BOOL        ALMAPI  AEvtCallMethod(OBJECTID sourceObject,
                OBJECTID targetObject, TYPEID targetType, EVENTID selector,
                LONG methodParam);

BOOL        ALMAPI  AEvtCheckForMethod(TYPEID theType, EVENTID selector);

/* ****************************************************************
 * Object Types & Objects
 * **************************************************************** */

/* General Object Management */
OBJECTID    ALMAPI  AObjCreate(TYPEID theType);

OBJECTID    ALMAPI  AObjCreateWithProtocol(TYPEID theType,
                PROTOCOLID theProtocol);

BOOL        ALMAPI  AObjDestroy(OBJECTID theObject);

BOOL        ALMAPI  AObjAssign(OBJECTID fromObject, OBJECTID toObject);

BOOL        ALMAPI  AObjValidateID(LPOBJECTID theObject,
                pAObjMessage theSystem);

BOOL        ALMAPI  AObjSetName(OBJECTID theObject, LPSTR newName);

/* Protocol Support */
LONG        ALMAPI  AObjQueryProtocol(OBJECTID, pApQueryInfo, PROTOCOLID);

LONG        ALMAPI  ATypeQueryProtocol(TYPEID, PROTOCOLID);

/* Object Type Enumeration */
LONG        ALMAPI  ATypeGetCount(void);

LONG        ALMAPI  ATypeGetCountCfg(void);

TYPEID      ALMAPI  ATypeGetNext(LPLONG theGetKey);

TYPEID      ALMAPI  ATypeGetNextCfg(LPLONG theGetKey);

/* Object Enumeration */
LONG        ALMAPI  AObjGetObjectCount(TYPEID theType);

OBJECTID    ALMAPI  AObjGetNextObject(TYPEID theType, LPLONG theGetKey);

/* Object Type Information */
LONG        ALMAPI  ATypeGetName(TYPEID theType, LPSTR theName,
                LONG nameSize);

LONG        ALMAPI  ATypeGetRuntimeInfo(TYPEID theType);

LONG        ALMAPI  ATypeSetRuntimeInfo(TYPEID theType, LONG theData);

/* Object Runtime Information */
LONG        ALMAPI  AObjGetRuntimeInfo(OBJECTID theObject);

LONG        ALMAPI  AObjSetRuntimeInfo(OBJECTID theObject, LONG theData);

/* Object Information */
LONG        ALMAPI  AObjGetName(OBJECTID theObject, LPSTR theName,
                LONG nameSize);

TYPEID      ALMAPI  AObjGetType(OBJECTID theObject);

BOOL        ALMAPI  AObjCheckType(OBJECTID theObject, TYPEID theType);

/* Object-Text Interchange */
BOOL        ALMAPI  AObjConvertToText(OBJECTID theObject,
                MEMBLOCKID theText);

BOOL        ALMAPI  AObjConvertFromText(OBJECTID theObject,
                MEMBLOCKID theText);

/* Object Data Block Management */
BOOL        ALMAPI  AObjAddData(OBJECTID  theObject,
                LPVOID    theData,
                LONG      dataSize);

#ifdef ADBG_DEBUG
LPVOID      ALMAPI  AObjLockDataDbg(OBJECTID theObject, LONG dataIndex,
                LPSTR lpszFile, LONG lLine);
BOOL        ALMAPI  AObjUnlockDataDbg(OBJECTID theObject, LONG dataIndex,
                LPSTR lpszFile, LONG lLine);

#define AObjLockData(o,i)      AObjLockDataDbg(o,i,__FILE__,__LINE__)
#define AObjUnlockData(o,i)    AObjUnlockDataDbg(o,i,__FILE__,__LINE__)

#else
LPVOID      ALMAPI  AObjLockData(OBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjUnlockData(OBJECTID theObject, LONG dataIndex);
#endif /* ADBG_DEBUG */

LONG        ALMAPI  AObjGetDataCount(OBJECTID theObject);

LONG        ALMAPI  AObjGetDataSize(OBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjInsertData(OBJECTID theObject, LONG dataIndex,
                LPVOID theData, LONG dataSize);

BOOL        ALMAPI  AObjDeleteData(OBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjResizeData(OBJECTID theObject, LONG dataIndex,
                LONG dataSize);

BOOL        ALMAPI  AObjSetData(OBJECTID theObject, LONG dataIndex,
                LPVOID theData, LONG dataSize);

BOOL        ALMAPI  AObjSetDataCount(OBJECTID theObject, LONG numDatas);

LONG        ALMAPI  AObjGetDataLockCount(OBJECTID theObject, LONG theData);

BOOL        ALMAPI  AObjWriteDatas(OBJECTID theObject, int hFile);

BOOL        ALMAPI  AObjReadDatas(OBJECTID theObject, int hFile);

BOOL        ALMAPI  AObjCopyDatas(OBJECTID theObject, MEMBLOCKID memBlck);

BOOL        ALMAPI  AObjRestoreDatas(OBJECTID theObject, LPVOID memBlockPtr);

/* Object Type Data Block Management */
BOOL        ALMAPI  ATypeAddData(TYPEID theType, LPVOID theData,
                LONG dataSize);

LPVOID      ALMAPI  ATypeLockData(TYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeUnlockData(TYPEID theType, LONG dataIndex);

LONG        ALMAPI  ATypeGetDataCount(TYPEID theType);

LONG        ALMAPI  ATypeGetDataSize(TYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeInsertData(TYPEID theType, LONG dataIndex,
                LPVOID theData, LONG dataSize);

BOOL        ALMAPI  ATypeDeleteData(TYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeResizeData(TYPEID theType, LONG dataIndex,
                LONG dataSize);

BOOL        ALMAPI  ATypeSetData(TYPEID theType, LONG dataIndex,
                LPVOID theData, LONG dataSize);

BOOL        ALMAPI  ATypeSetDataCount(TYPEID theType, LONG numDatas);

HICON       ALMAPI  ATypeGetIcon(TYPEID theType);

/* ****************************************************************
 * Object Ownership
 * **************************************************************** */

BOOL        ALMAPI  AObjRegisterOwner(OBJECTID ownerObject,
                OBJECTID  ownedObject, LONG ownerData);

BOOL        ALMAPI  AObjUnregisterOwner(OBJECTID ownerObject,
                OBJECTID ownedObject, LONG ownerData);

BOOL        ALMAPI  AObjGetNextOwner(TYPEID ownerType, OBJECTID ownedObject,
                pAObjOwnerInfo pNextOwnerInfo);


/* ****************************************************************
 * Function Parameters
 * **************************************************************** */

OBJECTID    ALMAPI  AFuncGetParameter(LONG parameterIndex);

OBJECTID    ALMAPI  AFuncGetTypedParameter(LONG parameterIndex,
                TYPEID theType);

BOOL        ALMAPI  AFuncSetParameter(LONG parameterIndex,
                OBJECTID theObject);

OBJECTID    ALMAPI  AFuncGetOutput(LONG parameterIndex, LPTYPEID  theType);
/* ****************************************************************
 * Miscellaneous AppWare functions
 * **************************************************************** */

LONG        ALMAPI  AObjReportError(OBJECTID theObject, LONG lMessage,
                LONG lStatus, LPSTR pszMessage, LONG lInfo);

BOOL        ALMAPI  AFuncReportResult(pAObjMessage theSystem,
                LONG flowIndex, LPSTR theError);

LONG        ALMAPI  AObjEdit(OBJECTID theObject);

BOOL        ALMAPI  AObjExport(OBJECTID theObject);

BOOL        ALMAPI  AObjImport(OBJECTID theObject);

BOOL        ALMAPI  AObjAddRequiredFile(pAObjMessage theSystem,
                LPSTR fileName);

LONG        ALMAPI  AAppConvertOSTime(LONG osTime);

VOID        ALMAPI  AAppDefWndProc(HWND, UINT, WPARAM, LPARAM);

LONG        ALMAPI  AAppGetTime(void);

LONG        ALMAPI  AAppGetPriorityMode(void);

LONG        ALMAPI  AAppSetPriorityMode(LONG theMode);

BOOL        ALMAPI  AAppGetName(MEMBLOCKID theBlock);

BOOL        ALMAPI  AAppSetName(MEMBLOCKID theBlock);

HICON       ALMAPI  AAppGetIcon(void);

BOOL        ALMAPI  AAppSave(void);

void        ALMAPI  AAppPostShutdown(BOOL);

void        ALMAPI  AAppGetCmdLine(LPSTR, short);

BOOL        ALMAPI  AAppShowAppBuilder(BOOL show);

/*  Function because signals for some objects don't match
      between platforms - the code resides in oipobjs.cpp */
BOOL        ALMAPI  OKludgeReportChangedSignals(OBJECTID objID,
                long oldSignal, long newSignal);

BOOL        ALMAPI  AAppIsDesignTime(void);

/* ****************************************************************
 * AppWare Memory Management
 * **************************************************************** */

MEMBLOCKID  ALMAPI  AMemAllocate(LONG numBytes);

BOOL        ALMAPI  AMemFree(MEMBLOCKID theBlock);

LPVOID      ALMAPI  AMemLock(MEMBLOCKID theBlock);

BOOL        ALMAPI  AMemUnlock(MEMBLOCKID theBlock);

LONG        ALMAPI  AMemGetSize(MEMBLOCKID theBlock);

BOOL        ALMAPI  AMemSetSize(MEMBLOCKID theBlock, LONG numBytes);

/* ****************************************************************
 * UPSF Support
 * **************************************************************** */

long        ALMAPI  AUpsfCreateScope(LPCSTR scopeName, LONG index);
long        ALMAPI  AUpsfOpenScope(LPCSTR scopeName, LONG index);
long        ALMAPI  AUpsfCloseScope(void);

long        ALMAPI  AUpsfLogComment(LPCSTR comment);

long        ALMAPI  AUpsfExportField(LPCSTR fieldName, LONG fieldIndex,
                LONG nativeType, LONG universalType,
                LPVOID sourceAddress, LONG sourceSize);

long        ALMAPI  AUpsfImportField(LPCSTR fieldName, LONG fieldIndex,
                LONG nativeType, LPVOID destinationAddress,
                LONG destinationSize);

long        ALMAPI  AUpsfFieldCount(LPCSTR fieldName);

long        ALMAPI  AUpsfFieldInfo(LPCSTR fieldName, LONG fieldIndex,
                LONG nativeType, LPLONG universalType,
                LPLONG convertedSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !A_ALM_H */
