/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** 

=============================================================================
   File Name:  O_NWMHS.h

   Description.:
      Header file for the AppWare 'NW MHS' object and functions.
=============================================================================
*/
#ifndef O_NWMHS_H
#define O_NWMHS_H

#define OTYPE_NWMHS ATYPE_MAKEID('m','h','s','o')

/* defines related to the SMF keyword parsing */
#define SMFKW_NUM_KEYWORDS           66
#define SMFKW_LAST_KEYWORD_TO_CHK    63
#define SMFID_UNASSIGNED              0
#define SMFID_TO                      1
#define SMFID_COPIES_TO               2
#define SMFID_BCC                     3
#define SMFID_FROM                    4
#define SMFID_SUBJECT                 5
#define SMFID_DATE                    6
#define SMFID_ATTACHMENT              7
#define SMFID_ATTACHMENT_NAME         8
#define SMFID_ATTACHMENT_TYPE         9
#define SMFID_ATTACHMENT_ENCODING    10
#define SMFID_ATTACHMENT_DATE        11
#define SMFID_MESSAGE_ID             12
#define SMFID_REPLY_TO               13
#define SMFID_REPLY_COPIES           14
#define SMFID_MCB_OPTIONS            15
#define SMFID_MCB_TYPE               16
#define SMFID_MHS_ID                 17
#define SMFID_SESSION_ID             18
#define SMFID_CONVERSATION_ID        19
#define SMFID_DEST_APPL_NAME         20
#define SMFID_ERROR_REPORT           21
#define SMFID_MESSAGE_TYPE           22
#define SMFID_IN_REPLY_TO            23
#define SMFID_RE_SENT_BY             24
#define SMFID_SUMMARY                25
#define SMFID_DATE_RECEIVED          26
#define SMFID_FORM_TYPE              27
#define SMFID_SMF_VERSION            28
#define SMFID_DATE_DELIVERED         29
#define SMFID_SEND_TO                30
#define SMFID_SENDER                 31
#define SMFID_ADDR_REFERRED_TO       32
#define SMFID_REJECTED_FOR           33
#define SMFID_RECEIVED_BY            34
#define SMFID_REFUSED_BY             35
#define SMFID_DATE_POSTED            36
#define SMFID_DATE_TRANSFERRED       37
#define SMFID_DELIVERED_TO           38
#define SMFID_DL_EXPANDED            39
#define SMFID_EXPIRATION_DATE        40
#define SMFID_EXPORTED_TO            41
#define SMFID_HEADER_ENCODING        42
#define SMFID_HOP_COUNT              43
#define SMFID_MESSAGE_ENCODING       44
#define SMFID_NOTIFY_CORRELATOR      45
#define SMFID_VIA_HOST               46
#define SMFID_APPLICATION_NAME       47
#define SMFID_AUTHORS                48
#define SMFID_COMMENTS               49
#define SMFID_IMPORTANCE             50
#define SMFID_KEYWORDS               51
#define SMFID_ORIG_COPIES_TO         52
#define SMFID_ORIGINAL_TO            53
#define SMFID_ORIGINATOR             54
#define SMFID_RESPOND_BY             55
#define SMFID_RET_MESSAGE            56
#define SMFID_SENSITIVITY            57
#define SMFID_OBS_MAC_FCREATOR       58
#define SMFID_OBS_MAC_FTYPE          59
#define SMFID_OBS_MESSAGE_SEQUENCE   60
#define SMFID_OBS_POSTING_STATUS     61
#define SMFID_OBS_TOTAL_COPIES_TO    62
#define SMFID_OBS_TOTAL_TO           63
#define SMFID_X_KEYWORD              64
#define SMFID_O_OEM_KEYWORD          65

/* define start and end of range of keyword IDs who's values are SMF address lists */
#define SMFID_RECIP_LIST_START     1
#define SMFID_RECIP_LIST_END       3

/* define start and end of attachment related IDs */
#define SMFID_ATTACH_START         7
#define SMFID_ATTACH_END          11
 
/* defines related to reserved characters in certain MHS headers */
#define OMHS_SMF_ADDR_RSVD_CHARS      " \t,;"
#define OMHS_ATTACH_NAME_RSVD_CHARS   " ,"

/* valid value defines for the 'pathTypeID' parameter to the API OMHSGetPathToBuffer( ) */
#define OMHS_SYSTEM_ROOT_PATH          1
#define OMHS_SYSTEM_OUT_MESSAGE_PATH   2
#define OMHS_SYSTEM_OUT_ATTACH_PATH    3
#define OMHS_USER_ROOT_PATH            4
#define OMHS_USER_IN_MESSAGE_PATH      5
#define OMHS_USER_IN_ATTACH_PATH       6
#define OMHS_STORED_MESSAGE_PATH       7
#define OMHS_STORED_ATTACH_PATH        8

/* defines relating to the type of Reply to be created.  One of these values should */
/* be specified for the 'replyType' parameter to the API OMHSCreateReplyMsg( )      */
#define OMHS_REPLY_TO_ALL              1
#define OMHS_REPLY_TO_SENDER           2

/* defines relating to the type of Forwarded Msg to be created.  One of these values */
/* should be specified for the 'fwdType' parameter to the API OMHSCreateFwdMsg( )    */
#define OMHS_FORWARD_UNMODIFIED        3
#define OMHS_FORWARD_MODIFIED          4

/* defines for which type of path format to use */
#define OMHS_UNC_PATH_TYPE             1
#define OMHS_DOS_PATH_TYPE             2
#define OMHS_NETWARE_PATH_TYPE         3
#define OMHS_MACINTOSH_PATH_TYPE       4


/* return code values for the NW MHS object APIs */
#define OMHS_ERR_SUCCESS                      0
#define OMHS_ERR_BAD_MHS_OID                -51  // bad MHS object ID
#define OMHS_ERR_BAD_TEXT_OID               -52  // bad Text object ID
#define OMHS_ERR_FAILED_RESIZE              -53  // AObjResizeData( ) failed
#define OMHS_ERR_FAILED_LOCK                -54  // AObjLockData( ) failed
#define OMHS_ERR_INDEX_OUT_OF_RANGE         -55  // specified index is out of range
#define OMHS_ERR_FAILED_MEM_ALLOC           -56  // failed internal memory allocation
#define OMHS_ERR_CANT_FIND_MHS_ROOT         -57  // cannot determine root MHS directory
#define OMHS_ERR_CANT_FIND_MHS_USER         -58  // cannot determine user's MHS name
#define OMHS_ERR_CANT_FIND_MHS_MBOX         -59  // cannot determine user's MHS mail box location
#define OMHS_ERR_CANT_FIND_MHS_APP          -60  // cannot determine user's MHS preferred app name
#define OMHS_ERR_CANT_FIND_IN_MSG_DIR       -61  // cannot determine user's incoming message directory
#define OMHS_ERR_CANT_FIND_STORE_MSG_DIR    -62  // cannot determine user's stored message directory
#define OMHS_ERR_CANT_FIND_IN_ATT_DIR       -63  // cannot determine user's incoming attachment directory
#define OMHS_ERR_CANT_FIND_STORE_ATT_DIR    -64  // cannot determine user's stored attachment directory
#define OMHS_ERR_BAD_PATH_TYPE              -65  // invalid path type
#define OMHS_ERR_BAD_REPLY_TYPE             -66  // invalid msg reply type
#define OMHS_ERR_BAD_FORWARD_TYPE           -67  // invalid msg forward type
#define OMHS_ERR_SRC_MSG_NOT_RECVD          -68  // source Message object was not received
#define OMHS_ERR_BAD_SRC_MSG_OBJ            -69  // bad source Message object ID
#define OMHS_ERR_BAD_DEST_MSG_OBJ           -70  // bad destination Message object ID
#define OMHS_ERR_DEL_NO_FILE                -71  // can't delete file - not found
#define OMHS_ERR_DEL_FILE_ACCESS            -72  // can't delete file - permission denied
#define OMHS_ERR_DEL_UNKNOWN_CAUSE          -73  // can't delete file - unknown cause
#define OMHS_ERR_BAD_DEST_BUFFER            -74  // invalid destination buffer
#define OMHS_ERR_BAD_DEST_BUFFER_LEN        -75  // invalid destination buffer length
#define OMHS_ERR_NO_RECIPIENTS_SPECIFIED    -76  // cannot send message - no recipients specified
#define OMHS_ERR_NO_SENDER_SPECIFIED        -77  // cannot send message - no sender specified
#define OMHS_ERR_BAD_STORE_MSG_DIR          -78  // invalid stored message directory spec
#define OMHS_ERR_BAD_STORE_ATTACH_DIR       -79  // invalid stored attachment directory spec
#define OMHS_ERR_BAD_IN_MSG_DIR             -80  // invalid incoming message directory spec
#define OMHS_ERR_BAD_IN_ATTACH_DIR          -81  // invalid incoming attachment directory spec
#define OMHS_ERR_SRC_FILE_OPEN              -82  // error opening source file (copy or move)
#define OMHS_ERR_SRC_FILE_READ              -83  // error reading from source file (copy or move)
#define OMHS_ERR_SRC_FILE_DELETE            -84  // error deleting source file (move)
#define OMHS_ERR_DEST_FILE_OPEN             -85  // error opening destination file (copy or move)
#define OMHS_ERR_DEST_FILE_WRITE            -86  // error writing destination file (copy or move)
#define OMHS_ERR_DEST_FILE_SPACE            -87  // destination file out of disk space (copy or move)
#define OMHS_ERR_BAD_SRC_FILE_SPEC          -88  // invalid source file specification (copy or move)
#define OMHS_ERR_BAD_DEST_FILE_SPEC         -89  // invalid destination file specification (copy or move)
#define OMHS_ERR_CANT_FIND_OUT_MSG_DIR      -90  // cannot determine outgoing message directory
#define OMHS_ERR_SEND_MSG_FILE_OPEN         -91  // error opening message file (send msg)
#define OMHS_ERR_SEND_MSG_FILE_WRITE        -92  // error writing to message file (send msg)
#define OMHS_ERR_BAD_SMF_SIGNATURE          -93  // invalid SMF signature header (parse msg)
#define OMHS_ERR_PARSE_OPEN_ERR             -94  // error opening the SMF msg file (parse msg)
#define OMHS_ERR_PARSE_READ_ERR             -95  // error reading from SMF msg file (parse msg)
#define OMHS_ERR_PARSE_BAD_SMF_KEYWORD      -96  // invalid SMF keyword (parse msg)
#define OMHS_ERR_PARSE_UNEXPECTED_INPUT     -97  // error parsing SMF msg file (parse msg)
#define OMHS_ERR_CANT_FIND_MHS_WRKGRP       -98  // cannot determine MHS workgroup name
#define OMHS_ERR_FAILED_DIR_CREATE          -99  // directory creation failed
#define OMHS_ERR_INCORRECT_PATH_FORMAT     -100  // path not of specified type
#define OMHS_ERR_NETWARE_DLLS_NOT_AVAIL    -101  // NetWare DLLs not available
#define OMHS_ERR_BAD_PATH_SPECIFIED        -102  // input path is invalid
#define OMHS_ERR_CANT_CONVERT_TO_DOS_PATH  -103  // unable to determine DOS path


/* Exported functions */
#ifdef __cplusplus
extern "C" {
#endif

/* names of exported functions should go here (APIs for other ALMs) */
int ALMAPI OMHSGetPathToBuffer( AOBJECTID oiMHSObj, int pathTypeID, char far *buffer, int bufferLen );
int ALMAPI OMHSGetUserInfoToBuffer( AOBJECTID oiMHSObj, char far *userNameBuffer, int userNameLen,
                                    char far *mailBoxBuffer, int mailBoxLen,
                                    char far *prefAppBuffer, int prefAppLen );
int ALMAPI OMHSCountIncomingMsg( AOBJECTID oiMHSObj );
int ALMAPI OMHSCountStoredMsg( AOBJECTID oiMHSObj );
int ALMAPI OMHSSendMsg( AOBJECTID oiMHSObj, AOBJECTID oiSMFMsgObj );
int ALMAPI OMHSDeleteMsg( AOBJECTID oiMHSObj, AOBJECTID oiSMFMsgObj );
int ALMAPI OMHSParseSMFMessage( AOBJECTID oiNWMHSObj, int msgIndex, AOBJECTID oiNWMsgObj );
int ALMAPI OMHSCreateReplyMsg( AOBJECTID oiMHSObj, AOBJECTID oiSrcMsgObj, AOBJECTID oiDestMsgObj, int replyType );
int ALMAPI OMHSCreateFwdMsg( AOBJECTID oiMHSObj, AOBJECTID oiSrcMsgObj, AOBJECTID oiDestMsgObj, int fwdType );
int ALMAPI OMHSCreateReceiptMsg( AOBJECTID oiMHSObj, AOBJECTID oiSrcMsgObj, AOBJECTID oiDestMsgObj );
int ALMAPI OMHSPollForNewMessages( AOBJECTID oiMHSObj );

#ifdef __cplusplus
}
#endif

#endif  /* !O_NWMHS_H */
