/***********************************************************/
/* Copyright (C) 1993 AT&T Bell Labs.  All rights reserved.*/
/***********************************************************/
/* abstract syntax: acs_asn_tbl */
/* last module in input: ACS-authentication-services */
/* created: Wed Dec  1 15:11:57 1993 */
/* associated control file: acsdefs.c */

#ifndef ACSDEFS_H
#define ACSDEFS_H

#ifndef ASN_TYPES_DEFINED

#define ASN_TYPES_DEFINED

typedef char Boolean;
typedef char Nulltype;

#if defined _MSDOS || defined __MSDOS__
#ifndef 
#define  __far
#endif
#else
#define  /* */
#endif

#endif


#define          ACS_OPEN_STREAM 1
#define          ACS_OPEN_STREAM_CONF 2
#define          ACS_CLOSE_STREAM 3
#define          ACS_CLOSE_STREAM_CONF 4
#define          ACS_ABORT_STREAM 5
#define          ACS_UNIVERSAL_FAILURE_CONF 6
#define          ACS_UNIVERSAL_FAILURE 7
#define          ACS_KEY_REQUEST 8
#define          ACS_KEY_REPLY 9

typedef enum StreamType_t {
    ST_CSTA = 1,
    ST_OAM = 2,
    ST_DIRECTORY = 3
} StreamType_t;

typedef char            ServerID_t[49];

typedef char            LoginID_t[49];

typedef struct CryptPasswd_t {
    short           length;
    unsigned char   value[47];
} CryptPasswd_t;

typedef char            AppName_t[21];

typedef enum Level_t {
    ACS_LEVEL1 = 1,
    ACS_LEVEL2 = 2,
    ACS_LEVEL3 = 3,
    ACS_LEVEL4 = 4
} Level_t;

typedef char            Version_t[21];

typedef struct ACSOpenStream_t {
    StreamType_t    streamType;
    ServerID_t      serverID;
    LoginID_t       loginID;
    CryptPasswd_t   cryptPass;
    AppName_t       applicationName;
    Level_t         level;
    Version_t       apiVer;
    Version_t       libVer;
    Version_t       tsrvVer;
} ACSOpenStream_t;

typedef struct ACSOpenStreamConfEvent_t {
    Version_t       apiVer;
    Version_t       libVer;
    Version_t       tsrvVer;
    Version_t       drvrVer;
} ACSOpenStreamConfEvent_t;

typedef struct ACSCloseStream_t {
    Nulltype        null;
} ACSCloseStream_t;

typedef struct ACSCloseStreamConfEvent_t {
    Nulltype        null;
} ACSCloseStreamConfEvent_t;

typedef struct ACSAbortStream_t {
    Nulltype        null;
} ACSAbortStream_t;

typedef enum ACSUniversalFailure_t {
    TSERVER_STREAM_FAILED = 0,
    TSERVER_NO_THREAD = 1,
    TSERVER_BAD_DRIVER_ID = 2,
    TSERVER_DEAD_DRIVER = 3,
    TSERVER_MESSAGE_HIGH_WATER_MARK = 4,
    TSERVER_FREE_BUFFER_FAILED = 5,
    TSERVER_SEND_TO_DRIVER = 6,
    TSERVER_RECEIVE_FROM_DRIVER = 7,
    TSERVER_REGISTRATION_FAILED = 8,
    TSERVER_SPX_FAILED = 9,
    TSERVER_TRACE = 10,
    TSERVER_NO_MEMORY = 11,
    TSERVER_ENCODE_FAILED = 12,
    TSERVER_DECODE_FAILED = 13,
    TSERVER_BAD_CONNECTION = 14,
    TSERVER_BAD_PDU = 15,
    TSERVER_NO_VERSION = 16,
    TSERVER_ECB_MAX_EXCEEDED = 17,
    TSERVER_NO_ECBS = 18,
    TSERVER_NO_SDB = 19,
    TSERVER_NO_SDB_CHECK_NEEDED = 20,
    TSERVER_SDB_CHECK_NEEDED = 21,
    TSERVER_BAD_SDB_LEVEL = 22,
    TSERVER_BAD_SERVERID = 23,
    TSERVER_BAD_STREAM_TYPE = 24,
    TSERVER_BAD_PASSWORD_OR_LOGIN = 25,
    TSERVER_NO_USER_RECORD = 26,
    TSERVER_NO_DEVICE_RECORD = 27,
    TSERVER_DEVICE_NOT_ON_LIST = 28,
    TSERVER_USERS_RESTRICTED_HOME = 30,
    TSERVER_NO_AWAYPERMISSION = 31,
    TSERVER_NO_HOMEPERMISSION = 32,
    TSERVER_NO_AWAY_WORKTOP = 33,
    TSERVER_BAD_DEVICE_RECORD = 34,
    TSERVER_DEVICE_NOT_SUPPORTED = 35,
    TSERVER_INSUFFICIENT_PERMISSION = 36,
    TSERVER_NO_RESOURCE_TAG = 37,
    TSERVER_INVALID_MESSAGE = 38,
    TSERVER_EXCEPTION_LIST = 39,
    TSERVER_NOT_ON_OAM_LIST = 40,
    TSERVER_PBX_ID_NOT_IN_SDB = 41,
    TSERVER_USER_LICENSES_EXCEEDED = 42,
    TSERVER_OAM_DROP_CONNECTION = 43,
    TSERVER_NO_VERSION_RECORD = 44,
    TSERVER_OLD_VERSION_RECORD = 45,
    TSERVER_BAD_PACKET = 46,
    TSERVER_OPEN_FAILED = 47,
    DRIVER_DUPLICATE_ACSHANDLE = 1000,
    DRIVER_INVALID_ACS_REQUEST = 1001,
    DRIVER_ACS_HANDLE_REJECTION = 1002,
    DRIVER_INVALID_CLASS_REJECTION = 1003,
    DRIVER_GENERIC_REJECTION = 1004,
    DRIVER_RESOURCE_LIMITATION = 1005,
    DRIVER_ACSHANDLE_TERMINATION = 1006,
    DRIVER_LINK_UNAVAILABLE = 1007
} ACSUniversalFailure_t;

typedef struct ACSUniversalFailureConfEvent_t {
    ACSUniversalFailure_t error;
} ACSUniversalFailureConfEvent_t;

typedef struct ACSUniversalFailureEvent_t {
    ACSUniversalFailure_t error;
} ACSUniversalFailureEvent_t;

typedef struct ChallengeKey_t {
    short           length;
    unsigned char   value[8];
} ChallengeKey_t;

typedef struct ACSKeyRequest_t {
    LoginID_t       loginID;
} ACSKeyRequest_t;

typedef struct ACSKeyReply_t {
    long            objectID;
    ChallengeKey_t  key;
} ACSKeyReply_t;


extern void  *acs_asn_tbl;    /* encoder-decoder control table */

#endif
