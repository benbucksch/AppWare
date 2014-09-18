/***
-----------------------------------------------------------------------------
 File Name:  O_Stream.h

 Description.:
   Header file for the AppWare Data Stream object APIs.

-----------------------------------------------------------------------------
  (c) 1995 Novell, Inc.  All rights reserved.

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
#define OTYPE_DATASTRM  MAKETYPEID('N','W','s','0')

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
#define OSTRM_BAD_TEXT_TYPE              -16
#define OSTRM_BAD_NUM_TYPE               -17
#define OSTRM_END_OF_STREAM              -18
#define OSTRM_READ_ONLY                  -19
#define OSTRM_NUM_OUT_OF_RANGE           -20

/*----- Numeric Values Types -----*/ 
// general masks
#define OSTRM_NUM_TYPE_UNKNOWN          0x00
#define OSTRM_NUM_TYPE_SIGNED           0x80 /* value if is signed */
#define OSTRM_NUM_TYPE_LO_TO_HI         0x40 /* low to high byte ordering */
#define OSTRM_NUM_TYPE_BYTES_MASK       0x0F
/* 8-bit numbers */
#define OSTRM_NUM_TYPE_NUM8             0x81
#define OSTRM_NUM_TYPE_UNUM8            0x01
/* 16-bit numbers */
#define OSTRM_NUM_TYPE_NUM16H           0x82
#define OSTRM_NUM_TYPE_NUM16L           0xC2
#define OSTRM_NUM_TYPE_UNUM16H          0x02
#define OSTRM_NUM_TYPE_UNUM16L          0x42
/* 32-bit numbers */
#define OSTRM_NUM_TYPE_NUM32H           0x84
#define OSTRM_NUM_TYPE_NUM32L           0xC4
#define OSTRM_NUM_TYPE_UNUM32H          0x04
#define OSTRM_NUM_TYPE_UNUM32L          0x44


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
LONG ALMAPI OStreamGetSize( OBJECTID oiDataStream );
int  ALMAPI OStreamSetSize( OBJECTID oiDataStream, LONG newSize );
int  ALMAPI OStreamIsReadOnly(  OBJECTID oiDataStream, BOOL *isReadOnly  );
int  ALMAPI OStreamMarkReadOnly(  OBJECTID oiDataStream  );
int  ALMAPI OStreamMarkReadWrite(  OBJECTID oiDataStream  );
int  ALMAPI OStreamSaveToFile(  OBJECTID oiDataStream, LPSTR fileName  );
int  ALMAPI OStreamLoadFromFile(  OBJECTID oiDataStream, LPSTR fileName  );
int  ALMAPI OStreamSetFromBuffer(  OBJECTID oiDataStream, LPVOID buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamGetToBuffer(  OBJECTID oiDataStream, LPVOID buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamSetFromHugeBuffer(  OBJECTID oiDataStream, void huge *buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamGetToHugeBuffer(  OBJECTID oiDataStream, void huge *buffer, LONG size, LONG startOffset  );
int  ALMAPI OStreamSetString(  OBJECTID oiDataStream, LONG offset,
                               LPSTR value, BYTE stringType, LONG fixedSize  );
int  ALMAPI OStreamGetString(  OBJECTID oiDataStream, LONG offset, BYTE stringType,
                               LPSTR buffer, LONG bufferSize, LONG fixedSize );
int  ALMAPI OStreamGetNumber(  OBJECTID oiDataStream, LONG offset, BYTE numType, long *value );
int  ALMAPI OStreamSetNumber(  OBJECTID oiDataStream, LONG offset, LONG value, BYTE numType  );

/* coming very soon */
int  ALMAPI OStreamFillWithBytes(  OBJECTID oiDataStream );

#ifdef __cplusplus
}
#endif

#endif
