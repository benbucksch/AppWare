/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** 

==========================================================================
 File Name:  O_NWMSG.h

 Description.:
       Header file for the AppWare 'NW Message' object and functions.
==========================================================================
*/
#ifndef O_NWMSG_H
#define O_NWMSG_H

#define OTYPE_NWMSG               ATYPE_MAKEID('m','s','g','o')

/* message type */
#define OMSG_MHS_SMF_MESSAGE      0x02
#define OMSG_MAPI_MESSAGE         0x04
#define OMSG_VIM_MESSAGE          0x06

/* recipient type IDs */
#define OMSG_ALL_RECIPIENTS       99
#define OMSG_TO_RECIPIENTS        1
#define OMSG_CC_RECIPIENTS        2
#define OMSG_BCC_RECIPIENTS       3


/****
 Bit mask values for the 'fMsgFormatType' field in the 'MsgData' data structure.
 The various bit flags should only be checked if the value of
 the "OMSG_FMT_SPECIFIED" bit is set (not zero).
****/
#define OMSG_FMT_SPECIFIED        0x01
#define OMSG_FMT_MHS_SMF          0x02
#define OMSG_FMT_MAPI             0x04
#define OMSG_FMT_VIM              0x06
#define OMSG_FMT_WHICH_FMT_MASK   0x06
#define OMSG_FMT_SENT             0x08
#define OMSG_FMT_RECEIVED         0x10
#define OMSG_FMT_GEN_REPLY        0x20
#define OMSG_FMT_GEN_FORWARD      0x40
#define OMSG_FMT_GEN_RECEIPT      0x80
//#define OMSG_FMT_RETURN_RCPT      0x40
//#define OMSG_FMT_NONDELIV_NOT     0x80

/****
 Bit mask values for the 'fDeliveryFlags' field in the 'MsgData' data structure
 The various bit flags should only be checked if the value of
 the "OMSG_DF_SPECIFIED" bit is set (not zero).
   the current defines are from the "MCB-options" SMF header
***/
#define OMSG_DF_SPECIFIED                 0x0001
#define OMSG_DF_RETURN_CONTENTS           0x0002
#define OMSG_DF_RECEIPT_NOTIFY            0x0004
#define OMSG_DF_NON_DELIVERY_NOTIFY       0x0008
#define OMSG_DF_DELIVERY_GRADE_MASK       0x0030
#define OMSG_DF_DELIVERY_GRADE_NORMAL     0x0010
#define OMSG_DF_DELIVERY_GRADE_URGENT     0x0020
#define OMSG_DF_DELIVERY_GRADE_OFFPEAK    0x0030
#define OMSG_DF_PROHIBIT_ALT_APP_DELIV    0x0080
#define OMSG_DF_DELIVERY_NOTIFY           0x0100
#define OMSG_DF_PROHIBIT_DESIGNATE_DELIV  0x0200
#define OMSG_DF_NON_RECEIPT_NOTIFY        0x0400
#define OMSG_DF_PROHIBIT_DOC_TRANSLATE    0x0800


/***
------------------------------------------------------------------------------
 a copy of the following data structure is stored for each attachment
 in data block 'OMSG_ATTACHMENTS_DATA_BLK' of the 'NW Message' object
------------------------------------------------------------------------------
***/
typedef struct
{
   char fOrigFileName[ 40 ];             /* original file name */
   char fCurrentFilePath[ 256 ];         /* the full path of the file (w/o the file name portion) */
                                         /*    Before Send = path of original file */
                                         /*    After Receive = stored attachment directory */
   char fCurrentTempFileName[ 10 ];      /* temporary file name */
   char fFileType[ 40 ];                 /* file type */
   char fFileEncodingType[ 40 ];         /* file encoding type */
   char fFileModDateAndTime[ 40 ];       /* file modification date and time */
   unsigned long fFileSizeInBytes;       /* file size */
   unsigned char fAttachmentType;        /* flag for hidden attachments, ... */
} MsgAttachData;

// define flags for the 'fAttachmentType' field
#define OMSG_ATTACH_DISPLAY_AS_BODY    0x01
#define OMSG_ATTACH_OEM_HIDDEN         0x02


// return codes for the Object APIs who's prototypes are listed below
#define OMSG_SUCCESS                     0
#define OMSG_ERR_SUCCESS                 0
#define OMSG_ERR_BAD_MSG_OID            -1  // bad Message object ID
#define OMSG_ERR_BAD_TEXT_OID           -2  // bad Text object ID
#define OMSG_ERR_FAILED_RESIZE          -3  // AObjResizeData( ) failed
#define OMSG_ERR_FAILED_LOCK            -4  // AObjLockData( ) failed
#define OMSG_ERR_INDEX_OUT_OF_RANGE     -5  // specified index is out of range
#define OMSG_ERR_FAILED_MEM_ALLOC       -6  // failed internal memory allocation
#define OMSG_ERR_BAD_RECIP_TYPE        -10  // invalid recipient type id or string
#define OMSG_ERR_BAD_DEST_OBJ_ID       -11  // invalid destination object ID
#define OMSG_ERR_BAD_DEST_OBJ_TYPE     -12  // unsupported destination object type
#define OMSG_ERR_BAD_DEST_BUFFER       -13  // invalid (NULL) destination buffer
#define OMSG_ERR_KEYWD_INST_NOT_FOUND  -20
#define OMSG_ERR_VALUE_INDEX_NOT_FOUND -21
#define OMSG_ERR_NO_VALUE_TO_GET       -22
#define OMSG_ERR_DIALOG_CANCELED       -30
#define OMSG_ERR_DEL_NO_FILE           -40  // can't delete file - not found
#define OMSG_ERR_DEL_FILE_ACCESS       -41  // can't delete file - permission denied
#define OMSG_ERR_DEL_UNKNOWN_CAUSE     -42  // can't delete file - unknown cause


/***
------------------------------------------------------------------------------
 Exported functions
------------------------------------------------------------------------------
***/
#ifdef __cplusplus
extern "C" {
#endif

/*------ names of exported functions should go here (APIs for other ALMs) ------*/

/***
-------------------------------------------------------------------------------------------
 The following function removes all data elements stored in the 'NW Message' object.  The
 object will be left in its 'newly initialized' state, with only version information.
-------------------------------------------------------------------------------------------
***/
int ALMAPI OMsgClearMessage(   AOBJECTID oiNWMsgObj );


/***
-------------------------------------------------------------------------------------------
 The following functions provide access to the various major fields of the message
 which are not stored using the keyword / value pair mechanism.
-------------------------------------------------------------------------------------------
***/
int  ALMAPI OMsgGetSenderToBuffer(         AOBJECTID oiNWMsgObj, char far *senderBuffer,  int bufferLen );
int  ALMAPI OMsgSetSenderFromBuffer(       AOBJECTID oiNWMsgObj, char far *senderBuffer );
int  ALMAPI OMsgGetSubjectToBuffer(        AOBJECTID oiNWMsgObj, char far *subjectBuffer, int bufferLen );
int  ALMAPI OMsgSetSubjectFromBuffer(      AOBJECTID oiNWMsgObj, char far *subjectBuffer );
int  ALMAPI OMsgGetDateComposedToBuffer(   AOBJECTID oiNWMsgObj, char far *dateBuffer,    int bufferLen );
int  ALMAPI OMsgSetDateComposedFromBuffer( AOBJECTID oiNWMsgObj, char far *dateBuffer );
long ALMAPI OMsgGetMsgBodySize(            AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgGetMsgBodyToBuffer(        AOBJECTID oiNWMsgObj, char far *msgBodyBuffer, int bufferLen );
int  ALMAPI OMsgSetMsgBodyFromBuffer(      AOBJECTID oiNWMsgObj, char far *msgBodyBuffer );
int  ALMAPI OMsgAppendMsgBodyFromBuffer(   AOBJECTID oiNWMsgObj, char far *msgBodyBuffer );


/***
-------------------------------------------------------------------------------------------
 the following functions provide access to the attachment related information stored
 in the 'NW Message' object
-------------------------------------------------------------------------------------------
***/
int ALMAPI OMsgCountAttachments( AOBJECTID oiNWMsgObj );
int ALMAPI OMsgAddAttachment( AOBJECTID oiNWMsgObj, AOBJECTID oiTextObj );
int ALMAPI OMsgAddAttachmentFromBuffer( AOBJECTID oiNWMsgObj, MsgAttachData far *pAttachDataBuffer );
int ALMAPI OMsgGetAttachmentToBuffer( AOBJECTID oiNWMsgObj, int attachIndex, MsgAttachData far *pAttachDataBuffer );
int ALMAPI OMsgSetAttachmentFromBuffer( AOBJECTID oiNWMsgObj, int attachIndex, MsgAttachData far *pAttachDataBuffer );
int ALMAPI OMsgDeleteAttachment( AOBJECTID oiNWMsgObj, int attachmentIndex, BOOL deleteFile );


/***
-------------------------------------------------------------------------------------------
 the following functions provide access to the destination recipients
 information stored in the 'NW Message' object
-------------------------------------------------------------------------------------------
***/
int ALMAPI OMsgCountRecipients(  AOBJECTID oiNWMsgObj, int recipTypeID );
int ALMAPI OMsgClearRecipients(  AOBJECTID oiNWMsgObj, int recipTypeID );
int ALMAPI OMsgAddRecipient(     AOBJECTID oiNWMsgObj, int recipTypeID, char far *recipName );
int ALMAPI OMsgDeleteRecipient(  AOBJECTID oiNWMsgObj, int recipTypeID, int index );
int ALMAPI OMsgGetRecipToText(   AOBJECTID oiNWMsgObj, int recipTypeID, int index,
                                 AOBJECTID oiTextObj );
int ALMAPI OMsgGetRecipToBuffer( AOBJECTID oiNWMsgObj, int recipTypeID, int index,
                                 char far *recipNameBuf, int bufferLen );
int ALMAPI OMsgGetRecipientList( AOBJECTID oiNWMsgObj, int recipTypeID, AOBJECTID oiDestObj );


/***
-------------------------------------------------------------------------------------------
 the following functions provide access to the keyword / value pairs stored in the
 'NW Message' object.  These keyword / value pairs are normally stored only for
 received messages.  The 'NW MHS' function 'Get Message' will usually be the source
 of these various pairs.
-------------------------------------------------------------------------------------------
***/
long ALMAPI OMsgCountNumValueHdrs( AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgCountKeywordInstances( AOBJECTID oiNWMsgObj, int keywordID );
int  ALMAPI OMsgCountKeywordValues( AOBJECTID oiNWMsgObj, int keywordID, int instanceNum );
int  ALMAPI OMsgCountValues( AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgClearValues( AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgAddKeywordInstance( AOBJECTID oiNWMsgObj, int keywordID, char far *keywordText,
                                    char far *valueText, int valueDataTypeID, char far *optionSpec );
int  ALMAPI OMsgAddKeywordValue( AOBJECTID oiNWMsgObj, int keywordID, int instanceNum,
                                 char far *valueText, int valueDataTypeID );
int  ALMAPI OMsgDeleteValue( AOBJECTID oiNWMsgObj, int keywordID, int instanceNum,
                             int valueIndex );
int  ALMAPI OMsgGetValueToText( AOBJECTID oiNWMsgObj, int keywordID, int instanceNum,
                                int valueIndex, AOBJECTID oiTextObj );
int  ALMAPI OMsgGetValueToBuffer( AOBJECTID oiNWMsgObj, int keywordID, int instanceNum,
                                  int valueIndex, char far *valueBuffer, int bufferLen );
int  ALMAPI OMsgChangeKeywordID(  AOBJECTID oiNWMsgObj, int oldKeywordID, int instanceNum,
                                  int newKeywordID );
int  ALMAPI OMsgGetValueInfoToBuffer( AOBJECTID oiNWMsgObj,       int indexNum,
                                      int  far *fpKeywordID,     int far *fpInstanceNum,
                                      char far *fpOptSpecBuffer, int optSpecLen,
                                      char far *fpValueBuffer,   int valueLen,
                                      char far *fpKeywordBuffer, int keywordLen );

/***
-------------------------------------------------------------------------------------------
 the following functions provide access to the information stored in the
 'NW Message' object relating to general status and state information about the
 message object.
-------------------------------------------------------------------------------------------
***/
int  ALMAPI OMsgMarkMessageAsSent(     AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgMarkMessageAsReceived( AOBJECTID oiNWMsgObj );
BOOL ALMAPI OMsgHasMsgBeenSent(        AOBJECTID oiNWMsgObj );
BOOL ALMAPI OMsgHasMsgBeenReceived(    AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgSetMsgTypeAndVer(      AOBJECTID oiNWMsgObj, int messageType, int version );
int  ALMAPI OMsgGetMsgType(            AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgGetMsgVersion(         AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgGetDelivOptions(       AOBJECTID oiNWMsgObj, unsigned long far *delivOptions );
int  ALMAPI OMsgSetDelivOptions(       AOBJECTID oiNWMsgObj, unsigned long      delivOptions );
int  ALMAPI OMsgSelectDelivOpt(        AOBJECTID oiNWMsgObj );
int  ALMAPI OMsgGetMsgTypeFlags(       AOBJECTID oiNWMsgObj );

/***
-------------------------------------------------------------------------------------------
 the following functions provide access to the information stored in the
 'NW Message' object relating to the current temporary name of the source SMF
 text file from which the 'NW Message' object data was obtained.
-------------------------------------------------------------------------------------------
***/
int ALMAPI OMsgSetMsgFileName( AOBJECTID oiNWMsgObj, char far *fileName );
int ALMAPI OMsgGetMsgFileName( AOBJECTID oiNWMsgObj, char far *fileName, int maxLen );

#ifdef __cplusplus
}
#endif

#endif  /* !O_NWMSG_H */
