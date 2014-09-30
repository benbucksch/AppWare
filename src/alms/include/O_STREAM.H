/***
-----------------------------------------------------------------------------
 File Name:  O_Stream.h

 Description.:
   Header file for the AppWare Data Stream object APIs.

  (c) 1994-1995 Novell, Inc.  All rights reserved.

  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
  LIABILITY.
-----------------------------------------------------------------------------
***/
#ifndef OTYPE_DATASTRM

/*----- the AppWare object type ID for the Data Stream ALM -----*/
#define OTYPE_DATASTRM  ATYPE_MAKEID('N','W','s','0')

/*----- error codes for the various API functions -----*/ 
#define OSTRM_SUCCESS                      0
#define OSTRM_ERROR                       -1
#define OSTRM_LOCK_FAILURE                -2
#define OSTRM_RESIZE_FAILURE              -3
#define OSTRM_BAD_OBJ_ID                  -4
#define OSTRM_FILE_NOT_FOUND              -5
#define OSTRM_FILE_WRITE_ERR              -6
#define OSTRM_FILE_READ_ERR               -7
#define OSTRM_FILE_OPEN_ERR               -8
#define OSTRM_BAD_SRC_BUFFER_PTR          -9
#define OSTRM_BAD_SRC_BUFFER_SIZE        -10
#define OSTRM_BAD_DEST_BUFFER_PTR        -11
#define OSTRM_BAD_DEST_BUFFER_SIZE       -12
#define OSTRM_EMPTY_STREAM               -13
#define OSTRM_SIZE_REQUIRES_HUGE         -14
#define OSTRM_BAD_START_OFFSET           -15

/*----- Numeric Values Types -----*/ 
#define OSTRM_NUM_TYPE_UNSIGNED         0x10
#define OSTRM_NUM_TYPE_LO_ORDER         0x20
#define OSTRM_NUM_TYPE_UNKNOWN          0x00
#define OSTRM_NUM_TYPE_BYTE             0x01
#define OSTRM_NUM_TYPE_UBYTE            0x11
#define OSTRM_NUM_TYPE_WORD_HI		    0x02
#define OSTRM_NUM_TYPE_UWORD_HI		    0x12
#define OSTRM_NUM_TYPE_WORD_LO		    0x22
#define OSTRM_NUM_TYPE_UWORD_LO		    0x32
#define OSTRM_NUM_TYPE_LONG_HI		    0x03
#define OSTRM_NUM_TYPE_ULONG_HI		    0x13
#define OSTRM_NUM_TYPE_LONG_LO		    0x23
#define OSTRM_NUM_TYPE_ULONG_LO		    0x33

/*----- Textual Value Types -----*/ 
#define OSTRM_TEXT_TYPE_UNKNOWN         0x00
#define OSTRM_TEXT_TYPE_NULL            0x01
#define OSTRM_TEXT_TYPE_BYTE_PRE        0x02
#define OSTRM_TEXT_TYPE_WORD_HI_PRE		 0x04
#define OSTRM_TEXT_TYPE_WORD_LO_PRE		 0x08
#define OSTRM_TEXT_TYPE_FIXED_LEN		 0x10

/*----- Exported functions -----*/
#ifdef __cplusplus
extern "C" {
#endif
/* currently implemented */
LONG ALMAPI OStreamGetSize( AOBJECTID oiDataStream );
int  ALMAPI OStreamSetSize( AOBJECTID oiDataStream, LONG newSize );
int  ALMAPI OStreamIsReadOnly(  AOBJECTID oiDataStream, BOOL *isReadOnly  );
int  ALMAPI OStreamMarkReadOnly(  AOBJECTID oiDataStream  );
int  ALMAPI OStreamMarkReadWrite(  AOBJECTID oiDataStream  );
int  ALMAPI OStreamSaveToFile(  AOBJECTID oiDataStream, LPSTR fileName  );
int  ALMAPI OStreamLoadFromFile(  AOBJECTID oiDataStream, LPSTR fileName  );
int  ALMAPI OStreamSetFromBuffer(  AOBJECTID oiDataStream, LPVOID buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamGetToBuffer(  AOBJECTID oiDataStream, LPVOID buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamSetFromHugeBuffer(  AOBJECTID oiDataStream, void huge *buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamGetToHugeBuffer(  AOBJECTID oiDataStream, void huge *buffer, LONG size, LONG startOffset  );

/* coming very soon */
int  ALMAPI OStreamFillWithBytes(  AOBJECTID oiDataStream );

/* coming not so soon */
int  ALMAPI OStreamSetString(  AOBJECTID oiDataStream, LONG offset,
                               LPSTR value, BYTE stringType  );
int  ALMAPI OStreamGetString(  AOBJECTID oiDataStream, LONG offset,
                               LPSTR buffer, LONG bufferSize, BYTE stringType  );
int  ALMAPI OStreamGetNumber(  AOBJECTID oiDataStream, LONG offset, int *value  );
int  ALMAPI OStreamSetNumber(  AOBJECTID oiDataStream, LONG offset,
                               LONG value, BYTE ordering  );
#ifdef __cplusplus
}
#endif

#endif
