/* ****************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef A_ALM_H
#define A_ALM_H

/* ****************************************************************
 * AppWare Kernel Version 2.0
 * **************************************************************** */

#define A_KERNELVER           0x0200

#ifndef _WINDOWS_       /*!! at least for borland !! */
#include <windows.h>
#ifndef _WINDOWS_
#define _WINDOWS_
#endif /* !WINDOWS_H */
#endif /* !WINDOWS_H */

#define ALMAPI      APIENTRY
#define ALMCAPI     CDECL
#define ALMCALLBACK APIENTRY

//#include <ap_macro.h>

/* ****************************************************************
 * Type definitions
 * **************************************************************** */

#ifdef ASTRICT
#define ADECL_TYPE(t)       struct t##__ {short astrict;};     \
                            typedef const struct t##__ * t
#else
#define ADECL_TYPE(t)       typedef long t
#endif  /* ASTRICT */

ADECL_TYPE(ATYPEID);
ADECL_TYPE(AOBJECTID);
ADECL_TYPE(AEVENTID);
ADECL_TYPE(AMEMBLOCKID);
ADECL_TYPE(APROTOCOLID);

typedef ATYPEID*      LPTYPEID;
typedef AOBJECTID*    LPOBJECTID;

/* ****************************************************************
 * ID Creation macros
 * **************************************************************** */

#define AMAKEID(a,b,c,d)           (   \
            ((long) (a) << 24) +      \
            ((long) (b) << 16) +      \
            ((long) (c) <<  8) +      \
            ((long) (d)      )        \
          )

#define ATYPE_MAKEID(a,b,c,d)       ((ATYPEID)     AMAKEID(a,b,c,d))
#define AEVENT_MAKEID(a,b,c,d)      ((AEVENTID)    AMAKEID(a,b,c,d))
#define APROTOCOL_MAKEID(a,b,c,d)   ((APROTOCOLID) AMAKEID(a,b,c,d))

/* ****************************************************************
 * UPSF Types
 * **************************************************************** */

/* Universal (UPSF) Types */
#define AUPSF_UNIV_NUMBER         AMAKEID('U', 'N', 'u', 'm')
#define AUPSF_UNIV_INTEGER        AMAKEID('U', 'I', 'n', 't')
#define AUPSF_UNIV_BOOL           AUPSF_UNIV_INTEGER
#define AUPSF_UNIV_TEXT           AMAKEID('U', 'T', 'x', 't')
#define AUPSF_UNIV_STRING         AUPSF_UNIV_TEXT
#define AUPSF_UNIV_BITMAP         AMAKEID('U', 'B', 'i', 't')
#define AUPSF_UNIV_OBJECTREF      AMAKEID('U', 'R', 'e', 'f')
#define AUPSF_UNIV_RECT           AMAKEID('U', 'R', 'c', 't')
#define AUPSF_UNIV_POINT          AMAKEID('U', 'P', 'n', 't')
#define AUPSF_UNIV_FONT           AMAKEID('U', 'F', 'o', 'n')
#define AUPSF_UNIV_COLOR          AMAKEID('U', 'C', 'l', 'r')

/* Native Types */
#define AUPSF_NATV_BYTE           AMAKEID('B', 'y', 't', 'e')
#define AUPSF_NATV_SBYTE          AMAKEID('C', 'h', 'a', 'r')
#define AUPSF_NATV_CHAR           AUPSF_NATV_SBYTE
#define AUPSF_NATV_UCHAR          AUPSF_NATV_BYTE

#define AUPSF_NATV_WORD           AMAKEID('W', 'o', 'r', 'd')
#define AUPSF_NATV_SWORD          AMAKEID('S', 'h', 'r', 't')
#define AUPSF_NATV_SHORT          AUPSF_NATV_SWORD
#define AUPSF_NATV_USHORT         AUPSF_NATV_WORD

#define AUPSF_NATV_BOOL           AMAKEID('B', 'o', 'o', 'l')

#define AUPSF_NATV_DWORD          AMAKEID('D', 'w', 'r', 'd')
#define AUPSF_NATV_SDWORD         AMAKEID('L', 'o', 'n', 'g')
#define AUPSF_NATV_LONG           AUPSF_NATV_SDWORD
#define AUPSF_NATV_ULONG          AUPSF_NATV_DWORD
#define AUPSF_NATV_INT            AUPSF_NATV_SDWORD
#define AUPSF_NATV_UINT           AUPSF_NATV_DWORD

#define AUPSF_NATV_FLOAT          AMAKEID('F', 'l', 'o', 't')
#define AUPSF_NATV_DOUBLE         AMAKEID('D', 'b', 'l', 'e')
#define AUPSF_NATV_LONGDOUBLE     AMAKEID('L', 'd', 'b', 'l')
#define AUPSF_NATV_STRING         AMAKEID('S', 't', 'r', 'g')
#define AUPSF_NATV_BITMAPINFO     AMAKEID('B', 'i', 'n', 'f')
#define AUPSF_NATV_BITMAPCORE     AMAKEID('B', 'c', 'o', 'r')
#define AUPSF_NATV_BITMAP         AMAKEID('B', 't', 'm', 'p')
#define AUPSF_NATV_METAFILE       AMAKEID('M', 'e', 't', 'F')
#define AUPSF_NATV_HBITMAP        AMAKEID('H', 'b', 'm', 'p')
#define AUPSF_NATV_HBRUSH         AMAKEID('H', 'b', 'r', 's')

#define AUPSF_NATV_OBJECTREF      AMAKEID('O', 'b', 'j', 'R')

#define AUPSF_NATV_FONT           AMAKEID('F', 'o', 'n', 't')
#define AUPSF_NATV_RECT           AMAKEID('R', 'e', 'c', 't')
#define AUPSF_NATV_POINT          AMAKEID('P', 'o', 'n', 't')
#define AUPSF_NATV_RGBCOLOR       AMAKEID('R', 'G', 'B', 'C')

/* ****************************************************************
 * Structure definitions
 * **************************************************************** */

/* AppWare Event */
typedef struct tagAEvtInfo
  {
  AOBJECTID    sourceObject;
  AOBJECTID    targetObject;
  AEVENTID     what;
  LONG        details;
  LONG        modifiers;
  LONG        when;
  LONG        how;
  LONG        variation;
  }  AEvtInfo, *pAEvtInfo;

/* Object Message */
typedef struct tagAObjMessage
  {
  LONG    message1;
  LONG    message2;
  LONG    message3;
  LONG    message4;
  char    errorMessage[256];
  }  AObjMessage, *pAObjMessage;

/* Signal Record */
typedef struct tagAObjSignalRecord
  {
  LONG    theID;
  char    theName[256];
  }  AObjSignalRecord, *pAObjSignalRecord;

/* Object Information Record */
typedef struct tagAObjInfo
  {
  char            errorMessage[256];
  LONG            typeOverhead;
  LONG            objectOverhead;
  } AObjInfo, *pAObjInfo;

/* Object Ownership Record */
typedef struct tagAObjOwnerInfo
  {
  AOBJECTID  ownerID;
  LONG      ownerData;
  LONG      key;
  LONG      reserved1;
  } AObjOwnerInfo, *pAObjOwnerInfo;

/* Protocol */
typedef struct tagApQueryInfo       ApQueryInfo;
typedef LPVOID (ALMAPI* APPCFN)(ApQueryInfo*);

typedef struct tagApQueryInfo
  {
  LONG        size;
  AOBJECTID    object;
  ATYPEID      type;
  APROTOCOLID  protocol;
  APPCFN      pfnCreate;
  } ApQueryInfo, *pApQueryInfo;


/* ****************************************************************
 * Constant definitions
 * **************************************************************** */

/* Miscellaneous Constants    */
#define AOBJ_GETFIRSTKEY            (-1L)

/* Execution State Constants  */
#define AAPP_EXECUTABLETIME         1
#define AAPP_PROJECTRUNTIME         2
#define AAPP_DESIGNTIME             3

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
#define AEVENT_OBJECTTOTEXT         AEVENT_MAKEID('o','t','o','T')
#define AEVENT_TEXTTOOBJECT         AEVENT_MAKEID('t','t','o','O')

/* AppWare System Events */
#define AEVENT_STARTUP              AEVENT_MAKEID('s','t','r','T')
#define AEVENT_SHUTDOWN             AEVENT_MAKEID('s','t','o','P')
#define AEVENT_SIGNAL               AEVENT_MAKEID('s','g','n','L')
#define AEVENT_OBJECTCHANGED        AEVENT_MAKEID('o','b','j','C')
#define AEVENT_CALLFUNCTION         AEVENT_MAKEID('c','a','l','F')

#define AEVENT_OS_ACTIVATEAPP       AEVENT_MAKEID('a', 'c', 't', 'A')  /* from WM_ACTIVATEAPP */
#define AEVENT_OS_CANCELMODE       	AEVENT_MAKEID('c', 'c', 'l', 'M')  /* from WM_CANCELMODE  */

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

class ApObjectD
	{
	public:
		AOBJECTID    m_oiObject;

	public:
  	virtual void ALMAPI Destroy(void) = 0;
  	virtual LONG ALMAPI QueryProtocol(ApQueryInfo *, APROTOCOLID) = 0;
	  virtual AOBJECTID ALMAPI Object() = 0;
  	virtual ATYPEID ALMAPI Type() = 0;
		virtual short ALMAPI Version() = 0;
	};


#define APTYPE_OBJECT               APROTOCOL_MAKEID('P','o','b','j')
typedef ApObjectD*                  (ALMAPI* ApObjCreateFN)(AOBJECTID);
typedef LONG                        (ALMAPI* ApObjInitFN)(AOBJECTID);

/* ****************************************************************
 * ****************************************************************
 * Kernel APIs
 * ****************************************************************
 * **************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ****************************************************************
 * Version support
 * **************************************************************** */

LONG	ALMAPI	ASysGetKernelVersion(void);

/* ****************************************************************
 * Debug support
 * **************************************************************** */

#ifdef ADBG_DEBUG
/* Passed in 'nType' parameter */
#define ADBG_INFO                   0
#define ADBG_WARNING                1
#define ADBG_ERROR                  2
#define ADBG_UNKNOWN                3

short       ALMCAPI ADbgPrint(short nType, ATYPEID t, LPCSTR pszFmt, ...);
short       ALMCAPI ADbgPrintRes(short nType, ATYPEID t, HINSTANCE hInst, WORD idRes, ...);

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

BOOL        ALMAPI  AEvtPostStandard(AOBJECTID theObject, AEVENTID what,
                     LONG details, LONG how);

BOOL        ALMAPI  AEvtPostSignalAtTail(AOBJECTID theObject, LONG signalID);

BOOL        ALMAPI  AEvtPostSignalAtMark(AOBJECTID theObject, LONG signalID);

/* Miscellaneous Event Functions */
LONG        ALMAPI  AEvtGetObjectChangedPriority(LONG preferred);

/* Inter-object Messages */
BOOL        ALMAPI  AEvtCallMethod(AOBJECTID sourceObject,
                     AOBJECTID targetObject, ATYPEID targetType, AEVENTID selector,
                     LONG methodParam);

BOOL        ALMAPI  AEvtCheckForMethod(ATYPEID theType, AEVENTID selector);

/* ****************************************************************
 * Object Types & Objects
 * **************************************************************** */

/* General Object Management */
AOBJECTID    ALMAPI  AObjCreate(ATYPEID theType);

AOBJECTID    ALMAPI  AObjCreateWithProtocol(ATYPEID theType,
                     APROTOCOLID theProtocol);

BOOL        ALMAPI  AObjDestroy(AOBJECTID theObject);

BOOL        ALMAPI  AObjAssign(AOBJECTID fromObject, AOBJECTID toObject);

BOOL        ALMAPI  AObjValidateID(LPOBJECTID theObject,
                     pAObjMessage theSystem);

BOOL        ALMAPI  AObjSetName(AOBJECTID theObject, LPCSTR newName);

/* Protocol Support */
LONG        ALMAPI  AObjQueryProtocol(AOBJECTID, pApQueryInfo, APROTOCOLID);

LONG        ALMAPI  ATypeQueryProtocol(ATYPEID, APROTOCOLID);

/* Object Type Enumeration */
LONG        ALMAPI  ATypeGetCount(void);

LONG        ALMAPI  ATypeGetCountCfg(void);

ATYPEID      ALMAPI  ATypeGetNext(LPLONG theGetKey);

ATYPEID      ALMAPI  ATypeGetNextCfg(LPLONG theGetKey);

/* Object Enumeration */
LONG        ALMAPI  AObjGetObjectCount(ATYPEID theType);

AOBJECTID    ALMAPI  AObjGetNextObject(ATYPEID theType, LPLONG theGetKey);

/* Object Type Information */
LONG        ALMAPI  ATypeGetName(ATYPEID theType, LPSTR theName,
                LONG nameSize);

LONG        ALMAPI  ATypeGetRuntimeInfo(ATYPEID theType);

LONG        ALMAPI  ATypeSetRuntimeInfo(ATYPEID theType, LONG theData);

/* Object Runtime Information */
LONG        ALMAPI  AObjGetRuntimeInfo(AOBJECTID theObject);

LONG        ALMAPI  AObjSetRuntimeInfo(AOBJECTID theObject, LONG theData);

/* Object Information */
LONG        ALMAPI  AObjGetName(AOBJECTID theObject, LPSTR theName,
                LONG nameSize);

ATYPEID      ALMAPI  AObjGetType(AOBJECTID theObject);

BOOL        ALMAPI  AObjCheckType(AOBJECTID theObject, ATYPEID theType);

/* Object-Text Interchange */
BOOL        ALMAPI  AObjConvertToText(AOBJECTID theObject,
                AMEMBLOCKID theText);

BOOL        ALMAPI  AObjConvertFromText(AOBJECTID theObject,
                AMEMBLOCKID theText);

/* Object Data Block Management */
BOOL        ALMAPI  AObjAddData(AOBJECTID  theObject,
                LPVOID    theData,
                LONG      dataSize);

#ifdef ADBG_DEBUG
LPVOID      ALMAPI  AObjLockDataDbg(AOBJECTID theObject, LONG dataIndex,
                     LPCSTR lpszFile, LONG lLine);
BOOL        ALMAPI  AObjUnlockDataDbg(AOBJECTID theObject, LONG dataIndex,
                     LPCSTR lpszFile, LONG lLine);

#define AObjLockData(o,i)      AObjLockDataDbg(o,i,__FILE__,__LINE__)
#define AObjUnlockData(o,i)    AObjUnlockDataDbg(o,i,__FILE__,__LINE__)

#else
LPVOID      ALMAPI  AObjLockData(AOBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjUnlockData(AOBJECTID theObject, LONG dataIndex);
#endif /* ADBG_DEBUG */

LONG        ALMAPI  AObjGetDataCount(AOBJECTID theObject);

LONG        ALMAPI  AObjGetDataSize(AOBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjInsertData(AOBJECTID theObject, LONG dataIndex,
                     LPVOID theData, LONG dataSize);

BOOL        ALMAPI  AObjDeleteData(AOBJECTID theObject, LONG dataIndex);

BOOL        ALMAPI  AObjResizeData(AOBJECTID theObject, LONG dataIndex,
                     LONG dataSize);

BOOL        ALMAPI  AObjSetData(AOBJECTID theObject, LONG dataIndex,
                     LPVOID theData, LONG dataSize);

BOOL        ALMAPI  AObjSetDataCount(AOBJECTID theObject, LONG numDatas);

LONG        ALMAPI  AObjGetDataLockCount(AOBJECTID theObject, LONG theData);

BOOL        ALMAPI  AObjWriteDatas(AOBJECTID theObject, HANDLE hFile);

BOOL        ALMAPI  AObjReadDatas(AOBJECTID theObject, HANDLE hFile);

BOOL        ALMAPI  AObjCopyDatas(AOBJECTID theObject, AMEMBLOCKID memBlck);

BOOL        ALMAPI  AObjRestoreDatas(AOBJECTID theObject, LPVOID memBlockPtr);

/* Object Type Data Block Management */
BOOL        ALMAPI  ATypeAddData(ATYPEID theType, LPVOID theData,
                     LONG dataSize);

LPVOID      ALMAPI  ATypeLockData(ATYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeUnlockData(ATYPEID theType, LONG dataIndex);

LONG        ALMAPI  ATypeGetDataCount(ATYPEID theType);

LONG        ALMAPI  ATypeGetDataSize(ATYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeInsertData(ATYPEID theType, LONG dataIndex,
                     LPVOID theData, LONG dataSize);

BOOL        ALMAPI  ATypeDeleteData(ATYPEID theType, LONG dataIndex);

BOOL        ALMAPI  ATypeResizeData(ATYPEID theType, LONG dataIndex,
                     LONG dataSize);

BOOL        ALMAPI  ATypeSetData(ATYPEID theType, LONG dataIndex,
                     LPVOID theData, LONG dataSize);

BOOL        ALMAPI  ATypeSetDataCount(ATYPEID theType, LONG numDatas);

HICON       ALMAPI  ATypeGetIcon(ATYPEID theType);

/* ****************************************************************
 * Object Ownership
 * **************************************************************** */

BOOL        ALMAPI  AObjRegisterOwner(AOBJECTID ownerObject,
                     AOBJECTID  ownedObject, LONG ownerData);

BOOL        ALMAPI  AObjUnregisterOwner(AOBJECTID ownerObject,
                     AOBJECTID ownedObject, LONG ownerData);

BOOL        ALMAPI  AObjGetNextOwner(ATYPEID ownerType, AOBJECTID ownedObject,
                     pAObjOwnerInfo pNextOwnerInfo);


/* ****************************************************************
 * Function Parameters
 * **************************************************************** */

AOBJECTID    ALMAPI  AFuncGetParameter(LONG parameterIndex);

AOBJECTID    ALMAPI  AFuncGetTypedParameter(LONG parameterIndex,
                     ATYPEID theType);

BOOL        ALMAPI  AFuncSetParameter(LONG parameterIndex,
                     AOBJECTID theObject);

AOBJECTID    ALMAPI  AFuncGetOutput(LONG parameterIndex, LPTYPEID  theType);

/* ****************************************************************
 * Miscellaneous AppWare functions
 * **************************************************************** */

LONG        ALMAPI  AObjReportError(AOBJECTID theObject, LONG lMessage,
                     LONG lStatus, LPCSTR pszMessage, LONG lInfo);

BOOL        ALMAPI  AFuncReportResult(pAObjMessage theSystem,
                     LONG flowIndex, LPCSTR theError);

LONG        ALMAPI  AObjEdit(AOBJECTID theObject);

BOOL        ALMAPI  AObjExport(AOBJECTID theObject);

BOOL        ALMAPI  AObjImport(AOBJECTID theObject);

BOOL        ALMAPI  AObjAddRequiredFile(pAObjMessage theSystem,
                     LPCSTR fileName);

LONG        ALMAPI  AAppConvertOSTime(LONG osTime);

VOID        ALMAPI  AAppDefWndProc(HWND, UINT, WPARAM, LPARAM);

LONG        ALMAPI  AAppGetTime(void);

LONG        ALMAPI  AAppGetPriorityMode(void);

LONG        ALMAPI  AAppSetPriorityMode(LONG theMode);

BOOL        ALMAPI  AAppGetName(AMEMBLOCKID theBlock);

BOOL        ALMAPI  AAppSetName(AMEMBLOCKID theBlock);

HICON       ALMAPI  AAppGetIcon(void);

BOOL        ALMAPI  AAppSave(void);

void        ALMAPI  AAppPostShutdown(BOOL);

void        ALMAPI  AAppGetCmdLine(LPSTR, short);

BOOL        ALMAPI  AAppShowAppBuilder(BOOL show);

BOOL        ALMAPI  AAppIsDesignTime(void);

WORD        ALMAPI AAppGetExecutionState(void);

/* ****************************************************************
 * AppWare Memory Management
 * **************************************************************** */

AMEMBLOCKID  ALMAPI  AMemAllocate(LONG numBytes);

BOOL        ALMAPI  AMemFree(AMEMBLOCKID theBlock);

LPVOID      ALMAPI  AMemLock(AMEMBLOCKID theBlock);

BOOL        ALMAPI  AMemUnlock(AMEMBLOCKID theBlock);

LONG        ALMAPI  AMemGetSize(AMEMBLOCKID theBlock);

BOOL        ALMAPI  AMemSetSize(AMEMBLOCKID theBlock, LONG numBytes);

/* ****************************************************************
 * UPSF Support
 * **************************************************************** */

LONG        ALMAPI  AUpsfCreateScope(LPCSTR scopeName, LONG index);
LONG        ALMAPI  AUpsfOpenScope(LPCSTR scopeName, LONG index);
LONG        ALMAPI  AUpsfCloseScope(void);

LONG        ALMAPI  AUpsfLogComment(LPCSTR comment);

LONG        ALMAPI  AUpsfExportField(LPCSTR fieldName, LONG fieldIndex,
                     LONG nativeType, LONG universalType,
                     LPCVOID sourceAddress, LONG sourceSize);

LONG        ALMAPI  AUpsfImportField(LPCSTR fieldName, LONG fieldIndex,
                     LONG nativeType, LPVOID destinationAddress,
                     LONG destinationSize);

LONG        ALMAPI  AUpsfFieldCount(LPCSTR fieldName);

LONG        ALMAPI  AUpsfFieldInfo(LPCSTR fieldName, LONG fieldIndex,
                     LONG nativeType, LPLONG universalType,
                     LPLONG convertedSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !A_ALM_H */
