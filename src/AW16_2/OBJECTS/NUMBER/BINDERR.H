/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** 

=============================================================================
   File Name:  BindErr.h

   Description.:
      Header file for the AppWare 'NW Bindery' object and functions.
=============================================================================
*/
#ifndef NWBIND_ERR_H
#define NWBIND_ERR_H   1

/* return code values for the NW Bindery ALM internal functions */
#define OBIND_ERR_SUCCESS                      0
#define OBIND_ERR_BAD_MHS_OID                -51  // bad MHS object ID
#define OBIND_ERR_BAD_TEXT_OID               -52  // bad Text object ID
#define OBIND_ERR_FAILED_RESIZE              -53  // AObjResizeData( ) failed
#define OBIND_ERR_FAILED_LOCK                -54  // AObjLockData( ) failed
#define OBIND_ERR_INDEX_OUT_OF_RANGE         -55  // specified index is out of range
#define OBIND_ERR_FAILED_MEM_ALLOC           -56  // failed internal memory allocation
#define OBIND_ERR_CANT_FIND_MHS_ROOT         -57  // cannot determine root MHS directory
#define OBIND_ERR_CANT_FIND_MHS_USER         -58  // cannot determine user's MHS name
#define OBIND_ERR_CANT_FIND_MHS_MBOX         -59  // cannot determine user's MHS mail box location
#define OBIND_ERR_CANT_FIND_MHS_APP          -60  // cannot determine user's MHS preferred app name
#define OBIND_ERR_CANT_FIND_IN_MSG_DIR       -61  // cannot determine user's incoming message directory
#define OBIND_ERR_CANT_FIND_STORE_MSG_DIR    -62  // cannot determine user's stored message directory
#define OBIND_ERR_CANT_FIND_IN_ATT_DIR       -63  // cannot determine user's incoming attachment directory
#define OBIND_ERR_CANT_FIND_STORE_ATT_DIR    -64  // cannot determine user's stored attachment directory
#define OBIND_ERR_BAD_PATH_TYPE              -65  // invalid path type
#define OBIND_ERR_BAD_REPLY_TYPE             -66  // invalid msg reply type
#define OBIND_ERR_BAD_FORWARD_TYPE           -67  // invalid msg forward type
#define OBIND_ERR_SRC_MSG_NOT_RECVD          -68  // source Message object was not received
#define OBIND_ERR_BAD_SRC_MSG_OBJ            -69  // bad source Message object ID
#define OBIND_ERR_BAD_DEST_MSG_OBJ           -70  // bad destination Message object ID
#define OBIND_ERR_DEL_NO_FILE                -71  // can't delete file - not found
#define OBIND_ERR_DEL_FILE_ACCESS            -72  // can't delete file - permission denied
#define OBIND_ERR_DEL_UNKNOWN_CAUSE          -73  // can't delete file - unknown cause
#define OBIND_ERR_BAD_DEST_BUFFER            -74  // invalid destination buffer
#define OBIND_ERR_BAD_DEST_BUFFER_LEN        -75  // invalid destination buffer length
#define OBIND_ERR_NO_RECIPIENTS_SPECIFIED    -76  // cannot send message - no recipients specified
#define OBIND_ERR_NO_SENDER_SPECIFIED        -77  // cannot send message - no sender specified
#define OBIND_ERR_BAD_STORE_MSG_DIR          -78  // invalid stored message directory spec
#define OBIND_ERR_BAD_STORE_ATTACH_DIR       -79  // invalid stored attachment directory spec
#define OBIND_ERR_BAD_IN_MSG_DIR             -80  // invalid incoming message directory spec
#define OBIND_ERR_BAD_IN_ATTACH_DIR          -81  // invalid incoming attachment directory spec
#define OBIND_ERR_SRC_FILE_OPEN              -82  // error opening source file (copy or move)
#define OBIND_ERR_SRC_FILE_READ              -83  // error reading from source file (copy or move)
#define OBIND_ERR_SRC_FILE_DELETE            -84  // error deleting source file (move)
#define OBIND_ERR_DEST_FILE_OPEN             -85  // error opening destination file (copy or move)
#define OBIND_ERR_DEST_FILE_WRITE            -86  // error writing destination file (copy or move)
#define OBIND_ERR_DEST_FILE_SPACE            -87  // destination file out of disk space (copy or move)
#define OBIND_ERR_BAD_SRC_FILE_SPEC          -88  // invalid source file specification (copy or move)
#define OBIND_ERR_BAD_DEST_FILE_SPEC         -89  // invalid destination file specification (copy or move)
#define OBIND_ERR_CANT_FIND_OUT_MSG_DIR      -90  // cannot determine outgoing message directory
#define OBIND_ERR_SEND_MSG_FILE_OPEN         -91  // error opening message file (send msg)
#define OBIND_ERR_SEND_MSG_FILE_WRITE        -92  // error writing to message file (send msg)
#define OBIND_ERR_BAD_SMF_SIGNATURE          -93  // invalid SMF signature header (parse msg)
#define OBIND_ERR_PARSE_OPEN_ERR             -94  // error opening the SMF msg file (parse msg)
#define OBIND_ERR_PARSE_READ_ERR             -95  // error reading from SMF msg file (parse msg)
#define OBIND_ERR_PARSE_BAD_SMF_KEYWORD      -96  // invalid SMF keyword (parse msg)
#define OBIND_ERR_PARSE_UNEXPECTED_INPUT     -97  // error parsing SMF msg file (parse msg)
#define OBIND_ERR_CANT_FIND_MHS_WRKGRP       -98  // cannot determine MHS workgroup name
#define OBIND_ERR_FAILED_DIR_CREATE          -99  // directory creation failed
#define OBIND_ERR_INCORRECT_PATH_FORMAT     -100  // path not of specified type
#define OBIND_ERR_NETWARE_DLLS_NOT_AVAIL    -101  // NetWare DLLs not available
#define OBIND_ERR_BAD_PATH_SPECIFIED        -102  // input path is invalid
#define OBIND_ERR_CANT_CONVERT_TO_DOS_PATH  -103  // unable to determine DOS path

#endif  /* ! NWBIND_ERR_H */
