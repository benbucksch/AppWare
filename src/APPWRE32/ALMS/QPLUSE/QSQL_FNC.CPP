/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: QSQL_FNC.CPP                                            |
 | DESCRIPTION	: Q+E ODBC SQL Object Functions.	                  |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                  |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Add Database View.	                  |
 |		  mw	08-Jun-1995  Win32 Convertions.                   |
\*------------------------------------------------------------------------*/
#include "qsql_fnc.h"
#include "qcnc_fnc.h"
#include "qsqlpc.h"
#include "qsqldc.h"
#include "queue.h"
#include "memblock.h"
#include "safemem.h"
#include "o_table.h"


#define QECHUNK 65280L

BOOL QSQL_FNC::AllocDataBuff	/* ---- Allocate Data Buffer ------------ */
( QsqlCore	*qsqlCore,      	// qsql core info
  long		rowCount,		// number of rows
  int		selColCount,		// number of columns
  Col		*selCol			// an array of columns
)
/* Allocates data buffers. */
{ MemBlock	mb;			// memblock class
  Col		*selColPtr;		// column pointer

  int		i;			// counter

  if (qsqlCore->isDataBuff)
    if (qsqlCore->dataBuffRowCount == rowCount)
      return TRUE;			// allocated
    else				// disposes
      DisposeDataBuff(qsqlCore, selColCount, selCol);  
  qsqlCore->isDataBuff = TRUE;
  qsqlCore->dataBuffRowCount = rowCount;

  /* ---- allocates buffers ---- */
  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { selColPtr->dataSizeHdl = GAlloc(rowCount * sizeof(long));
    selColPtr->dataHdl = GAlloc(rowCount * selColPtr->size);
  } // for
  return TRUE;
} // AllocDataBuff

void QSQL_FNC::DisposeDataBuff	/* ---- Dispose Data Buffer ------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  int		selColCount,		// number of columns
  Col		*selCol			// an array of columns			
)
/* Disposes data buffers. */
{ MemBlock	mb;			// memblock class
  Col		*selColPtr;		// column pointer

  int		i;			// counter

  if (!qsqlCore->isDataBuff)
    return;				// nothing to dispose

  /* ---- disposes buffers ---- */
  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { if (selColPtr->dataSizeHdl)		
    { GFree(selColPtr->dataSizeHdl);
      selColPtr->dataSizeHdl = 0;
    } // if
    if (selColPtr->dataHdl)
    { GFree(selColPtr->dataHdl);
      selColPtr->dataHdl = 0;
    } // if
  } // for
  qsqlCore->isDataBuff = FALSE;
} // DisposeDataBuff

void QSQL_FNC::Dispose		/* ---- Dispose ------------------------- */
( QsqlCore	*qsqlCore		// qsql core info
)
/* Disposes select memory block. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer

  int		i;			// counter

  if (!qsqlCore->isSelectMB)
    return;				// nothing to dispose

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  /* ---- disposes all the buffers ---- */
  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { if (selColPtr->name)
    { MBFree(selColPtr->name);
      selColPtr->name = 0;
    } // if
    if (selColPtr->bindData)
    { MBFree(selColPtr->bindData);
      selColPtr->bindData = 0;
    } // if

    if (selColPtr->dataSizeHdl)
    { GFree(selColPtr->dataSizeHdl);
      selColPtr->dataSizeHdl = 0;
    } // if
    if (selColPtr->dataHdl)
    { GFree(selColPtr->dataHdl);
      selColPtr->dataHdl = 0;
    } // if
  } // for
  qsqlCore->isDataBuff = FALSE;

  AMemFree(qsqlCore->selectMB);
  qsqlCore->isSelectMB = FALSE;
} // Dispose

long QSQL_FNC::Fetch		/* ---- Fetch --------------------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount,		// number of rows
  long		rowNmbr			// row number
)
/* Returns the number of rows fetched into the data buffers.
   Returns -1, if an error has occured.
*/
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer
  long		**dataSize;		// an array of data size pointer
  long		**dataSizePtr;		// bind data size pointer
  char		**data;			// an array of data pointer
  char		**dataPtr;		// data pointer
  qeSTATUS	errorCode;		// error code				

  long		l;			// counter
  int		i;			// counter

  if (!qsqlCore->isSelectMB)
    return 0;				// nothing to fetch

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  if (!selColCount)
    return 0;				// nothing to fetch
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  AllocDataBuff(qsqlCore, rowCount, selColCount, selCol);
  					// allocates enough space

  dataSize = (long **) mb.Alloc(selColCount * sizeof(long *));
  data = (char **) mb.Alloc(selColCount * sizeof(char *));

  /* ---- gets the buffers' pointers ---- */
  for (dataSizePtr = dataSize, dataPtr = data,
       selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
       dataSizePtr++, dataPtr++)
  { *dataSizePtr = (long *) mb.GLock(selColPtr->dataSizeHdl);
    *dataPtr = (char *) mb.GLock(selColPtr->dataHdl);
  } // for

  /* ---- fetches data into buffers ---- */
  for (l = 0; l < rowCount; l++)
  { /* ---- fetches data ---- */
    errorCode = rowNmbr? (*qe_FetchRandom)(qsqlCore->cursor, rowNmbr++) :
                         (*qe_FetchNext)(qsqlCore->cursor);
    if (errorCode != qeSUCCESS)
    { if (errorCode == qeEOF)
      { RecordError(qsqlCore, qeSUCCESS);
      	return l;			// # of record fecthed < rowCount
      } // if
      RecordError(qsqlCore, errorCode);
      return -1;			// an error has occured
    } // if

    /* ---- copies data into buffers ---- */
    for (dataSizePtr = dataSize, dataPtr = data,
	 selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
	 dataSizePtr++, dataPtr++)
    { if (selColPtr->bindDataSize == qeTRUNCATION ||
	  selColPtr->bindDataSize == QECHUNK)
      { **dataSizePtr = selColPtr->size;
  	CopyMemory(*dataPtr, selColPtr->bindData, selColPtr->size);

        *(*dataPtr + selColPtr->size - 1) = '\0';	// truncates with '\0'		
      } // if
      else
      { **dataSizePtr = selColPtr->bindDataSize;
	if (selColPtr->bindDataSize != qeNULL_DATA)
          if (selColPtr->bindDataSize == selColPtr->size)
	    CopyMemory(*dataPtr, selColPtr->bindData, selColPtr->size);
	  else
	  { CopyMemory(*dataPtr, selColPtr->bindData, selColPtr->bindDataSize);

            *(*dataPtr + selColPtr->bindDataSize) = '\0';// terms with '\0'
	  } // else
      } // else	  

      (*dataSizePtr)++;
      *dataPtr +=selColPtr->size;
    } // for
  } // for
  return rowCount;
} // Fetch

BOOL QSQL_FNC::FetchToColData	/* ---- Fetch To Column Data ------------ */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount,		// number of rows
  AMEMBLOCKID	tableMB			// table memory block  
)
/* Fetches rows to the column data. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer
  long		**dataSize;		// an array of datasize pointers
  long		**dataSizePtr;		// datasize pointer
  char		**data;			// an array of data pointers
  char		**dataPtr;		// data pointer
  long		**dataSize2;		// an array of data size 2 pointers
  long		**dataSize2Ptr;		// data size 2 pointer
  char		**data2;		// an array of data 2 pointers
  char		**data2Ptr;		// data 2 pointer

  QSqlDatC	dataCon;		// qsqldatc class
  long		textSize;		// text size
  char		*textPtr;		// text pointer
  long		strLen;			// string length
  char		str[MXSTRSIZE];		// string
  char		*strPtr;		// string pointer
  long		wholeVal;		// whole number value
  long double	realVal;		// real number value
  long		dateVal;		// date value
  long		timeVal;		// time value

  long		l;			// counter
  int		i;			// counter

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  dataSize = (long **) mb.Alloc(selColCount * sizeof(long *));
  data = (char **) mb.Alloc(selColCount * sizeof(char *));
  dataSize2 = (long **) mb.Alloc(selColCount * sizeof(long *));
  data2 = (char **) mb.Alloc(selColCount * sizeof(char *));
  for (dataSizePtr = dataSize, dataPtr = data,
       dataSize2Ptr = dataSize2, data2Ptr = data2,
       selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
       dataSizePtr++, dataPtr++, dataSize2Ptr++, data2Ptr++)
  { *dataSizePtr = *dataSize2Ptr = (long *) mb.GLock(selColPtr->dataSizeHdl);
    *dataPtr = *data2Ptr = (char *) mb.GLock(selColPtr->dataHdl);
  } // for

  /* ---- figures out the size ---- */
  textSize = selColCount * rowCount + 1;

  for (l = 0; l < rowCount; l++)
    for (dataSizePtr = dataSize, dataPtr = data,
	 selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
	 dataSizePtr++, dataPtr++)
    { switch ((long) selColPtr->varType)
      { case OTYPE_TEXT:
	case 0:
	  if (**dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += lstrlen(strPtr);
	  } // if
	  break;
	case OTYPE_NUMBER:
	  if (**dataSizePtr != qeNULL_DATA)
	    if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      textSize += ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                     str, sizeof(str));
	    } // if
	    else			// real number
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      textSize += ONmbrFormatReal(selColPtr->varObj, realVal,
	                                  str, sizeof(str));
	    } // else
	  break;
	case OTYPE_DATE:
	  if (**dataSizePtr != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += ODateFormatDate(selColPtr->varObj, dateVal,
	                                str, sizeof(str));
	  } // if
	  break;
	case OTYPE_TIME:
	  if (**dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += OTimeFormatTime(selColPtr->varObj, timeVal,
	                                str, sizeof(str));
	  } // if
	  break;
      } // switch

      (*dataSizePtr)++;
      *dataPtr += selColPtr->size;
    } // for

  /* ---- allocates the space ---- */
  MemResize(tableMB, textSize);
  textPtr = (char *) mb.GetPointer(tableMB);

  for (dataSizePtr = dataSize, dataPtr = data,
       dataSize2Ptr = dataSize2, data2Ptr = data2,
       i = 0; i < selColCount; i++,
       dataSizePtr++, dataSize2Ptr++, dataPtr++, data2Ptr++)
  { *dataSizePtr = *dataSize2Ptr;
    *dataPtr = *data2Ptr;
  } // for

  /* ---- copies the data ---- */
  for (l = 0; l < rowCount; l++)
  { for (dataSizePtr = dataSize, dataPtr = data,
	 selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
	 dataSizePtr++, dataPtr++)
    { switch ((long) selColPtr->varType)
      { case OTYPE_TEXT:
        case 0:
	  if (**dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = lstrlen(strPtr);
	    if (qsqlCore->isOEM)
	      OemToAnsi(strPtr, textPtr);
	    else
	      CopyMemory(textPtr, strPtr, strLen);
	    textPtr += strLen;
	  } // if
	  break;
	case OTYPE_NUMBER:
	  if (**dataSizePtr != qeNULL_DATA)
	    if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      strLen = ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                  str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	    } // if
	    else			// real number
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      strLen = ONmbrFormatReal(selColPtr->varObj, realVal,
	                               str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	    } // else
	    textPtr += strLen;
	  break;
	case OTYPE_DATE:
	  if (**dataSize != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = ODateFormatDate(selColPtr->varObj, dateVal,
	                             str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += lstrlen(str);
	  } // if
	  break;
	case OTYPE_TIME:
	  if (**dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = OTimeFormatTime(selColPtr->varObj, timeVal,
				     str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += lstrlen(str);
	  } // if
	  break;
      } // switch
      (*dataSizePtr)++;
      *dataPtr += selColPtr->size;

      /* ---- column separator ---- */
      *textPtr++ = '\0';
    } // for
  } // for
  *textPtr = '\0';			// 0 terminated string
  return TRUE;
} // FetchToColData

BOOL QSQL_FNC::FetchToObject	/* ---- Fetch To Object ----------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount		// number of rows
)
/* Fetches rows to the objects. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of select columns
  Col		*selCol;		// an array of select columns
  Col		*selColPtr;		// select column pointer
  long		*dataSize;		// data size
  long		*dataSizePtr;		// data size pointer
  char		*data;			// data
  char		*dataPtr;		// data pointer

  StrUtl	s;			// strutl class
  QSqlDatC	dataCon;		// qsqldatc class
  long		textSize;		// text size
  char		*text;			// text
  char		*textPtr;		// text pointer
  long		strLen;			// string length
  char		*strPtr;		// string
  long		wholeVal;		// whole number value
  long double	realVal;		// real number value
  long		dateVal;		// date value
  long		timeVal;		// time value

  long		l;			// counter
  int		i;			// counter

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  if (selColCount)
    selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { dataSize = (long *) mb.GLock(selColPtr->dataSizeHdl);
    data = (char *) mb.GLock(selColPtr->dataHdl);

    switch ((long) selColPtr->varType)
    { case OTYPE_TEXT:
	/* ---- calculates the size ---- */

        /* ---- row separators ---- */
	if (rowCount > 1)		// "\r\n"
	  textSize = 2 * (rowCount - 1) + 1;
	else
          textSize = 1;			// 0 terminated string

	for (dataSizePtr = dataSize, dataPtr = data,
             l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += lstrlen(strPtr) + s.CRCount(strPtr);
	  } // if

	/* ---- allocates the space ---- */
	text = (char *) mb.GAlloc(textSize);
        textPtr = text;

	/* ---- copies data into buffer ---- */
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	{ if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
		       qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    strLen = lstrlen(strPtr);
	    if (qsqlCore->isOEM)
	      OemToAnsi(strPtr, textPtr);
	    else
	      CopyMemory(textPtr, strPtr, strLen + 1);
            textPtr += strLen + s.CRToNewLine(textPtr);
	  } // if

	  /* ---- row separator ---- */
	  if (l < rowCount - 1)	     
	  { *textPtr++ = '\r';
	    *textPtr++ = '\n';
	  } // if
	} // for
	*textPtr = '\0';		// 0 terminated string

	/* ---- transfers text to the text object ---- */
	OTextSetValueFromBuffer(selColPtr->varObj, text);
	break;
      case OTYPE_NUMBER:
	if (*dataSize != qeNULL_DATA)
	  if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	  { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                 qsqlCore->cursor, i + 1, *selColPtr, data);
	    ONmbrSetInteger(selColPtr->varObj, wholeVal, FALSE);
	  } // if
	  else				// real number
	  { realVal = *(long double *) dataCon.QEDataToRealVal(
	                qsqlCore->cursor, i + 1, *selColPtr, data);
	    ONmbrSetReal(selColPtr->varObj, realVal, FALSE);
	  } // else
	else
	  ONmbrConvertFromString(selColPtr->varObj, "", FALSE);
	break;
      case OTYPE_DATE:
	if (*dataSize != qeNULL_DATA)
	{ dateVal = *(long *) dataCon.QEDataToDateVal(
	              qsqlCore->cursor, i + 1, *selColPtr, data);
	  ODateSetValue(selColPtr->varObj, dateVal);
	} // if
	else
	  ODateConvertFromString(selColPtr->varObj, "");
	break;
      case OTYPE_TIME:
	if (*dataSize != qeNULL_DATA)
	{ timeVal = *(long *) dataCon.QEDataToTimeVal(
	              qsqlCore->cursor, i + 1, *selColPtr, data);
	  OTimeSetValue(selColPtr->varObj, timeVal);
	} // if
	else
	  OTimeConvertFromString(selColPtr->varObj, "");
	break;
    } // switch
  } // for
  return TRUE;
} // FetchToObject

BOOL QSQL_FNC::AppendToTable	/* ---- Append To Table ----------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount,		// number of rows
  AOBJECTID	tableObj		// table object
)
/* Appends rows to the table object. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer
  long		*dataSize;		// data size
  long		*dataSizePtr;		// data size pointer
  char		*data;			// data
  char		*dataPtr;		// data pointer

  QSqlDatC	dataCon;		// qsqldatc class
  long		textSize;		// text size
  char		*text;			// text
  char		*textPtr;		// text pointer
  long		strLen;			// string length
  char		str[MXSTRSIZE];		// string
  char		*strPtr;		// string pointer
  long		wholeVal;		// whole number value
  long double	realVal;		// real number value
  long		dateVal;		// date value
  long		timeVal;		// time value

  long		l;			// counter
  int		i;			// counter

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  /* ---- set number of columns and number of rows to the table object -- */
//  tblSetColumnCount(tableObj, selColCount);
//  tblSetRowCount(tableObj, rowCount);

  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { MemBlock	mb;			// memblock class

    /* ---- sets column title to the table object ---- */
//    tblColumnTitle(tableObj, selColPtr->name, i);

    dataSize = (long *) mb.GLock(selColPtr->dataSizeHdl);
    data = (char *) mb.GLock(selColPtr->dataHdl);

    /* ---- calculates the size ---- */
    textSize = 1;			// 0 terminated string

    /* ---- row separators ---- */
    textSize += rowCount;		// "\t"	      

    switch ((long) selColPtr->varType)
    { case OTYPE_TEXT:
      case 0: 
	for (dataSizePtr = dataSize, dataPtr = data,
             l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += lstrlen(strPtr);
	  } // if
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	{ for (dataSizePtr = dataSize, dataPtr = data,
               l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	    if (*dataSizePtr != qeNULL_DATA)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      textSize += ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                     str, sizeof(str));
	    } // if
        } // if
	else				// real number
	{ for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	    if (*dataSizePtr != qeNULL_DATA)
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      textSize += ONmbrFormatReal(selColPtr->varObj, realVal,
	                                  str, sizeof(str));
	    } // if
        } // else
	break;
      case OTYPE_DATE:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += ODateFormatDate(selColPtr->varObj, dateVal,
	                                str, sizeof(str));
	  } // if
	break;
      case OTYPE_TIME:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
			qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += OTimeFormatTime(selColPtr->varObj, timeVal, 
				        str, sizeof(str));
	  } // if
	break;
    } // switch

    /* ---- allocates the space ---- */
    text = (char *) mb.GAlloc(textSize);
    textPtr = text;

    /* ---- copies the data ---- */
    switch ((long) selColPtr->varType)
    { case OTYPE_TEXT:
      case 0:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	{ if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    strLen = lstrlen(strPtr);
	    if (qsqlCore->isOEM)
	      OemToAnsi(strPtr, textPtr);
	    else
	      CopyMemory(textPtr, strPtr, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';		
	} // for
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	  for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	  { if (*dataSizePtr != qeNULL_DATA)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      strLen = ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                  str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	      textPtr += strLen;
	     } // if

             /* ---- row separator ---- */
	     *textPtr++ = '\t';	      
	  } // for
	else				// real number
	  for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	  { if (*dataSizePtr != qeNULL_DATA)
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      strLen = ONmbrFormatReal(selColPtr->varObj, realVal,
	                               str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	      textPtr += strLen;
	    } // if

	    /* ---- row separator ---- */
	    *textPtr++ = '\t';	       
	  } // for
	break;
      case OTYPE_DATE:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSize++, dataPtr += selColPtr->size)
	{ if (*dataSize != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, data);
	    strLen = ODateFormatDate(selColPtr->varObj, dateVal,
	                             str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';		
	} // for
	break;
      case OTYPE_TIME:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	{ if (*dataSize != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    strLen += OTimeFormatTime(selColPtr->varObj, timeVal,
	                              str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';	
	} // for
	break;
    } // switch
    *textPtr = '\0';			// 0 terminated string

    /* ---- transfers text to the table object ---- */
//    tblSetColumn(tableObj, text, i);
  } // for
  return TRUE;
} // AppendToTable

BOOL QSQL_FNC::FetchToTable	/* ---- Fetch To Table ------------------ */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount,		// number of rows
  AOBJECTID	tableObj		// table object
)
/* Fetches rows to the table object. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer
  long		*dataSize;		// data size
  long		*dataSizePtr;		// data size pointer
  char		*data;			// data
  char		*dataPtr;		// data pointer

  QSqlDatC	dataCon;		// qsqldatc class
  long		textSize;		// text size
  char		*text;			// text
  char		*textPtr;		// text pointer
  long		strLen;			// string length
  char		str[MXSTRSIZE];		// string
  char		*strPtr;		// string pointer
  long		wholeVal;		// whole number value
  long double	realVal;		// real number value
  long		dateVal;		// date value
  long		timeVal;		// time value

  long		l;			// counter
  int		i;			// counter

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  /* ---- set number of columns and number of rows to the table object -- */
//  tblSetColumnCount(tableObj, selColCount);
//  tblSetRowCount(tableObj, rowCount);

  for (selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++)
  { MemBlock	mb;			// memblock class

    /* ---- sets column title to the table object ---- */
//    tblColumnTitle(tableObj, selColPtr->name, i);

    dataSize = (long *) mb.GLock(selColPtr->dataSizeHdl);
    data = (char *) mb.GLock(selColPtr->dataHdl);

    /* ---- calculates the size ---- */
    textSize = 1;			// 0 terminated string

    /* ---- row separators ---- */
    textSize += rowCount;		// "\t"	      

    switch ((long) selColPtr->varType)
    { case OTYPE_TEXT:
      case 0: 
	for (dataSizePtr = dataSize, dataPtr = data,
             l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += lstrlen(strPtr);
	  } // if
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	{ for (dataSizePtr = dataSize, dataPtr = data,
               l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	    if (*dataSizePtr != qeNULL_DATA)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      textSize += ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                     str, sizeof(str));
	    } // if
        } // if
	else				// real number
	{ for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	    if (*dataSizePtr != qeNULL_DATA)
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      textSize += ONmbrFormatReal(selColPtr->varObj, realVal,
	                                  str, sizeof(str));
	    } // if
        } // else
	break;
      case OTYPE_DATE:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += ODateFormatDate(selColPtr->varObj, dateVal,
	                                str, sizeof(str));
	  } // if
	break;
      case OTYPE_TIME:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	  if (*dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
			qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    textSize += OTimeFormatTime(selColPtr->varObj, timeVal, 
				        str, sizeof(str));
	  } // if
	break;
    } // switch

    /* ---- allocates the space ---- */
    text = (char *) mb.GAlloc(textSize);
    textPtr = text;

    /* ---- copies the data ---- */
    switch ((long) selColPtr->varType)
    { case OTYPE_TEXT:
      case 0:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	{ if (*dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    strLen = lstrlen(strPtr);
	    if (qsqlCore->isOEM)
	      OemToAnsi(strPtr, textPtr);
	    else
	      CopyMemory(textPtr, strPtr, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';
	} // for
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	  for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	  { if (*dataSizePtr != qeNULL_DATA)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      strLen = ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                  str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	      textPtr += strLen;
	     } // if

             /* ---- row separator ---- */
	     *textPtr++ = '\t';	      
	  } // for
	else				// real number
	  for (dataSizePtr = dataSize, dataPtr = data,
	       l = 0; l < rowCount; l++,
	       dataSizePtr++, dataPtr += selColPtr->size)
	  { if (*dataSizePtr != qeNULL_DATA)
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	      strLen = ONmbrFormatReal(selColPtr->varObj, realVal,
	                               str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
	      textPtr += strLen;
	    } // if

	    /* ---- row separator ---- */
	    *textPtr++ = '\t';	       
	  } // for
	break;
      case OTYPE_DATE:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSize++, dataPtr += selColPtr->size)
	{ if (*dataSize != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, data);
	    strLen = ODateFormatDate(selColPtr->varObj, dateVal,
	                             str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';		
	} // for
	break;
      case OTYPE_TIME:
	for (dataSizePtr = dataSize, dataPtr = data,
	     l = 0; l < rowCount; l++,
	     dataSizePtr++, dataPtr += selColPtr->size)
	{ if (*dataSize != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, dataPtr);
	    strLen += OTimeFormatTime(selColPtr->varObj, timeVal,
	                              str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if

          /* ---- row separator ---- */
	  *textPtr++ = '\t';	
	} // for
	break;
    } // switch
    *textPtr = '\0';			// 0 terminated string

    /* ---- transfers text to the table object ---- */
//    tblSetColumn(tableObj, text, i);
  } // for
  return TRUE;
} // FetchToTable

BOOL QSQL_FNC::FetchToText	/* ---- Fetch To Text ------------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  long		rowCount,		// number of rows
  AOBJECTID	textObj			// text object
)
/* Fetches rows to the text object. */
{ MemBlock	mb;			// memblock class
  int		selColCount;		// number of columns
  Col		*selCol;		// an array of columns
  Col		*selColPtr;		// column pointer
  long		**dataSize;		// an array of datasize pointers
  long		**dataSizePtr;		// datasize pointer
  char		**data;			// an array of data pointers
  char		**dataPtr;		// data pointer
  long		**dataSize2;		// an array of data size 2 pointers
  long		**dataSize2Ptr;		// data size 2 pointer
  char		**data2;		// an array of data 2 pointers
  char		**data2Ptr;		// data 2 pointer

  QSqlDatC	dataCon;		// qsqldatc class
  long		textSize;		// text size
  char		*text;			// text
  char		*textPtr;		// text pointer
  long		strLen;			// string length
  char		str[MXSTRSIZE];		// string
  char		*strPtr;		// string pointer
  long		wholeVal;		// whole number value
  long double	realVal;		// real number value
  long		dateVal;		// date value
  long		timeVal;		// time value

  long		l;			// counter
  int		i;			// counter

  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

  dataSize = (long **) mb.Alloc(selColCount * sizeof(long *));
  data = (char **) mb.Alloc(selColCount * sizeof(char *));
  dataSize2 = (long **) mb.Alloc(selColCount * sizeof(long *));
  data2 = (char **) mb.Alloc(selColCount * sizeof(char *));
  for (dataSizePtr = dataSize, dataPtr = data,
       dataSize2Ptr = dataSize2, data2Ptr = data2,
       i = 0, selColPtr = selCol; i < selColCount; i++, selColPtr++,
       dataSizePtr++, dataPtr++, dataSize2Ptr++, data2Ptr++)
  { *dataSizePtr = *dataSize2Ptr = (long *) mb.GLock(selColPtr->dataSizeHdl);
    *dataPtr = *data2Ptr = (char *) mb.GLock(selColPtr->dataHdl);
  } // for

  /* ---- figures out the size ---- */
  /* ---- row separators ---- */
  if (rowCount > 1)			// "\r\n"		      
    textSize = 2 * (rowCount - 1) + 1;
  else
    textSize = 1;			// 0 terminated string

  /* ---- column separators ---- */      
  if (selColCount > 1)			// qsqlCore->colSprtr
    textSize += rowCount * (selColCount - 1);

  for (l = 0; l < rowCount; l++)
    for (dataSizePtr = dataSize, dataPtr = data,
	 selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
	 dataSizePtr++, dataPtr++)
    { switch ((long) selColPtr->varType)
      { case 0:
        case OTYPE_TEXT:
	  if (**dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += lstrlen(strPtr);
	  } // if
	  break;
	case OTYPE_NUMBER:
	  if (**dataSizePtr != qeNULL_DATA)
	    if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      textSize += ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                     str, sizeof(str));
	    } // if
	    else			// real number
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      textSize += ONmbrFormatReal(selColPtr->varObj, realVal,
	                                  str, sizeof(str));
	    } // else
	  break;
	case OTYPE_DATE:
	  if (**dataSizePtr != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
			qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += ODateFormatDate(selColPtr->varObj, dateVal,
	                                str, sizeof(str));
	  } // if
	  break;
	case OTYPE_TIME:
	  if (**dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    textSize += OTimeFormatTime(selColPtr->varObj, timeVal,
	                                str, sizeof(str));
	  } // if
	  break;
      } // switch

      (*dataSizePtr)++;
      *dataPtr += selColPtr->size;
    } // for

  /* ---- allocates the space ---- */
  text = (char *) mb.GAlloc(textSize);
  textPtr = text;

  for (dataSizePtr = dataSize, dataPtr = data,
       dataSize2Ptr = dataSize2, data2Ptr = data2,
       i = 0; i < selColCount; i++,
       dataSizePtr++, dataSize2Ptr++, dataPtr++, data2Ptr++)
  { *dataSizePtr = *dataSize2Ptr;
    *dataPtr = *data2Ptr;
  } // for

  /* ---- copies the data ---- */
  for (l = 0; l < rowCount; l++)
  { for (dataSizePtr = dataSize, dataPtr = data,
	 selColPtr = selCol, i = 0; i < selColCount; i++, selColPtr++,
	 dataSizePtr++, dataPtr++)
    { switch ((long) selColPtr->varType)
      { case 0:
	case OTYPE_TEXT:
	  if (**dataSizePtr != qeNULL_DATA)
	  { strPtr = (char *) dataCon.QEDataToTextVal(
	               qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = lstrlen(strPtr);
	    if (qsqlCore->isOEM)
	      OemToAnsi(strPtr, textPtr);
	    else
	      CopyMemory(textPtr, strPtr, strLen);
	    textPtr += strLen;
	  } // if
	  break;
	case OTYPE_NUMBER:
	  if (**dataSizePtr != qeNULL_DATA)
	    if (ONmbrGetType(selColPtr->varObj) == ONMBR_INTEGER)
	    { wholeVal = *(long *) dataCon.QEDataToWholeVal(
	                   qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      strLen = ONmbrFormatInteger(selColPtr->varObj, wholeVal,
	                                  str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
              textPtr += strLen;
	    } // if
	    else			// real number
	    { realVal = *(long double *) dataCon.QEDataToRealVal(
	                  qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	      strLen = ONmbrFormatReal(selColPtr->varObj, realVal,
	                               str, sizeof(str));
	      CopyMemory(textPtr, str, strLen);
              textPtr += strLen;
	    } // else
	  break;
	case OTYPE_DATE:
	  if (**dataSize != qeNULL_DATA)
	  { dateVal = *(long *) dataCon.QEDataToDateVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = ODateFormatDate(selColPtr->varObj, dateVal,
	                             str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if
	  break;
	case OTYPE_TIME:
	  if (**dataSizePtr != qeNULL_DATA)
	  { timeVal = *(long *) dataCon.QEDataToTimeVal(
	                qsqlCore->cursor, i + 1, *selColPtr, *dataPtr);
	    strLen = OTimeFormatTime(selColPtr->varObj, timeVal,
				     str, sizeof(str));
	    CopyMemory(textPtr, str, strLen);
	    textPtr += strLen;
	  } // if
	  break;
      } // switch
      (*dataSizePtr)++;
      *dataPtr += selColPtr->size;

      /* ---- column separator ---- */
      if (i < selColCount - 1)
	*textPtr++ = (char) qsqlCore->colSprtr;
    } // for

    /* ---- row separator ---- */
    if (l < rowCount - 1)
    { *textPtr++ = '\r';
      *textPtr++ = '\n';
    } // if
  } // for
  *textPtr = '\0';			// 0 terminated string

  /* ---- transfers text to the text object ---- */
  OTextSetValueFromBuffer(textObj, text);
  return TRUE;
} // FetchToText

BOOL QSQL_FNC::GetHDBC		/* ---- Get Database Connection --------- */
( AOBJECTID	qcncObj,		// qcnc object
  qeHANDLE	*hdbc			// database connection handle
)
/* Returns true or false as the result of getting the hdbc. */
{ MemBlock	mb;			// memblock class
  QcncCore	*qcncCore;		// qcnc core info

  if (!qcncObj ||
      (qcncCore = (QcncCore *) mb.GetPointer(qcncObj, QCNCCORE)) == 0)
    return FALSE;
  if (!qcncCore->isConnected)
    return FALSE;			// not connected

  *hdbc = qcncCore->hdbc;
  return TRUE;			       
} // GetHDBC

BOOL QSQL_FNC::RecordError	/* ---- Record Error -------------------- */
( QsqlCore	*qsqlCore,		// qsql core info
  qeSTATUS	errorCode		// error code
)
/* Returns true or false as the result of recording error. */
{
  if (errorCode == qeSUCCESS)
  { /* ---- no error ---- */
    if (qsqlCore->errorCode != qeSUCCESS)
    { /* ---- erases the previous error message ---- */
      qsqlCore->errorCode = qeSUCCESS;
      MBFree(qsqlCore->errorMsg);
    } // if
    return FALSE;
  } // if

  if (qsqlCore->errorCode == qeSUCCESS)
    /* ---- creates buffer for the current error message ---- */
    qsqlCore->errorMsg = (char *) MBAlloc(qeMAX_ERR_MSG_LEN);
  (*qe_ErrMsgBuf)(qsqlCore->errorMsg);
  qsqlCore->errorCode = errorCode;
  return TRUE;
} // RecordError


/*------------------------------------------------------------------------*\
 | METHOD       : OQsqlStartupMeth                                        |
 | DESCRIPTION  : ODBC SQL object's constructor.                          |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlStartupMeth
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object
  QsqlCore	*qsqlCore;		// qsql core info

  long          objIdx;			// object index

  objIdx = AOBJ_GETFIRSTKEY;
  while((qsqlObj = AObjGetNextObject(OTYPE_QSQL, &objIdx)) != 0)
  { /* ---- inits resources ---- */
    MemResizeObjectData(qsqlObj, QSQLCORE, sizeof(QsqlCore));
    qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);

    qsqlCore->recCount = -1;		// special case for # of records

    qsqlCore->errorCode = qeSUCCESS;
    qsqlCore->isCursor = FALSE;		// no cursor
    qsqlCore->isSelectMB = FALSE;	// no select memory block
    qsqlCore->isDataBuff = FALSE;	// no select data buffers
  } // if

  if (!sqlLib.LoadLib())
    lstrcpy(theSystem->errorMessage,
	    "ODBC SQL object couldn't find the Q+E ODBC DLL.");
} // OQsqlStartupMeth

/*------------------------------------------------------------------------*\
 | METHOD       : OQsqlShutdownMeth                                       |
 | DESCRIPTION  : ODBC SQL object's destructor.                           |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlShutdownMeth
( pAEvtInfo,				// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object
  QsqlCore	*qsqlCore;		// qsql core info

  QSQL_FNC	fnc;			// qsql_fnc clas
  long          objIdx;			// object index
  qeSTATUS	errorCode;		// error code

  objIdx = AOBJ_GETFIRSTKEY;
  while((qsqlObj = AObjGetNextObject(OTYPE_QSQL, &objIdx)) != 0)
  { qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE);
    if (qsqlCore->isCursor)
    { /* ---- release resources ---- */
      errorCode = (*qe_EndSQL)(qsqlCore->cursor);
      if (fnc.RecordError(qsqlCore, errorCode))
	AEvtPostSignalAtMark(qsqlObj, QSQLIFERROR);
      else
      { qsqlCore->isCursor = FALSE;
	fnc.Dispose(qsqlCore);
      } // else

      if (qsqlCore->isDView)
      { qsqlCore->isDView = FALSE;
	if (qsqlCore->objList)
	{ delete qsqlCore->objList;
	  qsqlCore->objList = 0;
	} // if
      } // if
      qsqlCore->recCount = -1;		// special case for # of records
    } // if
    if (qsqlCore->errorCode != qeSUCCESS)
    { qsqlCore->errorCode = qeSUCCESS;
      MBFree(qsqlCore->errorMsg);
    } // if
  } // while
} // OQsqlShutdownMeth

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlAppendToTableFunc                                  |
 | DESCRIPTION  : Retrieves rows of SELECT results, and appends them into |
 |		  the specified Table object.                             |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The object that owns the query to |
 |		  fetch data from.                                        |
 | INPUT 2(OPT)	: Rows: NUMBER -- The number of rows to fetch.  This      |
 |		  parameter is only necessary if the programmer wants to  |
 |		  override the row count setting in the ODBC SQL object.  |
 | INPUT 3	: Table: TABLE -- The results of the query formatted into |
 |		  a Table object.                                         |
 | OUTPUT 1(OPT): Rows Fetched: NUMBER -- The number of rows were         |
 |		  actually fetched from the DBMS.                         |
 | FLOW 1       : Fetched.                                                |
 | FLOW 2       : No more data.                                           |
 | FLOW 3	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlAppendToTableFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID      rowCountObj;		// number object	      [I: 2]
  long		rowCount;		// number of rows
  AOBJECTID	tableObj;		// table object		      (I: 3)
  AOBJECTID	rowFetchedObj;		// number object	      [O: 1]

  QSQL_FNC	fnc;			// qsql_fnc class

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Append To Table: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Append To Table: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if ((rowCountObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)) == 0)
    rowCount = qsqlCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount <= 0)			// checks lower bound
    rowCount = rowCount? -rowCount : 1;

  /* ---- FUNCTION INPUT 3 ---- */
  tableObj = AFuncGetTypedParameter(3, OTYPE_TABLE);
  if (!tableObj)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Append To Table: Couldn't get the Table input parameter.");
    return;
  } // if

  if (!qsqlCore->isCursor)
  { /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "ODBC SQL\r\n"
      "Append To Table: The query wasn't executed.");
    return;
  } // if

  rowCount = fnc.Fetch(qsqlCore, rowCount);
  if (rowCount == -1)
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "");
    return;
  } // if
  if (!rowCount)
  { /* ---- FLOW 2 (No more data) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  fnc.AppendToTable(qsqlCore, rowCount, tableObj);

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((rowFetchedObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Append To Table: Couldn't create the Rows Fetched output parameter.");
    return;
  } // if
  ONmbrSetInteger(rowFetchedObj, rowCount, TRUE);
  AFuncSetParameter(1, rowFetchedObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlAppendToTableFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlDescribeTableFunc                                  |
 | DESCRIPTION  : Returns column names, the associated column types, and  |
 |		  the mapped variable types from a given database table.  |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object to used to    |
 |		  get the table information from.                         |
 | INPUT 2      : TEXT: Table Name -- The name of the database table.     |
 | OUTPUT 1(Opt): TEXT: Column Names -- A "\r\n" separated column names.  |
 | OUTPUT 2(Opt): TEXT: Column Types -- A "\r\n" separated column types.  |
 |		  These will be one of the following:                     |
 |		  "Fixed length character string", "Variable length       |
 |		  character string", "Decimal number", "Long integer",    |
 |		  "Integer", "Single precision floating point numbers",   |
 |		  "Double precision floating point numbers", "Date",      |
 |		  "Time", or "Date-Time".                                 |
 | FLOW 1       : Described.                                              |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlDescribeTableFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object                (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  char		*sqlStmt;		// sql statement
  AOBJECTID      tabNameObj;     	// text object		      (I: 2)
  int		tabNameSize;		// table name size
  char          *tabName;       	// table name

  AOBJECTID      colNamesObj;		// text object		      [O: 1]
  long          colNamesSize;		// column names size
  char		*colNames;		// "\r\n" separated column names
  char		*colNamesPtr;		// column names pointer
  AOBJECTID      colTypesObj;		// text object		      [O: 2]
  long          colTypesSize;		// column types size
  char		*colTypes;		// "\r\n" separated column types
  char          *colTypesPtr;		// column types pointer

  QSQL_FNC	fnc;			// qsql_fnc class
  qeHANDLE	hdbc;			// database connection handle
  qeHANDLE	cursor;			// cursor
  int		colCount;		// number of columns
  char		*colName;		// column name
  qeSTATUS	errorCode;		// error code

  int          i;              	// counter

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Describe Table: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Describe Table: Couldn't get the ODBC SQL input parameter,");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if ((tabNameObj = AFuncGetTypedParameter(2, OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Describe Table: Couldn't get the Table Name input parameter.");
    return;
  } // if
  tabNameSize = OTextGetLength(tabNameObj) + 1;
  tabName = (char *) mb.Alloc(tabNameSize);
  OTextGetToBuffer(tabNameObj, tabName, tabNameSize);

  /* ---- gets hdbc ---- */
  if (!fnc.GetHDBC(qsqlCore->connectObj, &hdbc))
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC SQL\r\n"
      "Describe Table: The database wasn't connected.");
    return;
  } // if

  /* ---- builds the sql statement ---- */
  sqlStmt = (char *) mb.Alloc(lstrlen("SELECT * FROM ") + tabNameSize);
  lstrcpy(sqlStmt, "SELECT * FROM ");
  lstrcat(sqlStmt, tabName);

  /* ---- executes q+e sql statement ---- */
  if (!(cursor = (*qe_ExecSQL)(hdbc, sqlStmt)))
  { errorCode = (*qe_Err)();
    if (fnc.RecordError(qsqlCore, errorCode))
      AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");	
    return;
  } // if

  /* ---- calculates column names, and column types sizes ---- */
  colCount = qe_NumCols(cursor);

  /* ---- row separators ---- */
  if (colCount > 1)
    colNamesSize = colTypesSize = 2 * (colCount - 1) + 1;
  else
    colNamesSize = colTypesSize = 1;	// 0 terminated string

  for (i = 1; i <= colCount; i++)
  { /* ---- column name ---- */
    colName = (*qe_ColName)(cursor, (short) i);
    colNamesSize += lstrlen(colName);

    /* ---- column type ---- */
    switch((*qe_ColType)(cursor, (short) i))
    { case qeCHAR:
	colTypesSize += 29;		// "Fixed length character string"
	break;
      case qeVARCHAR:
	colTypesSize += 32;		// "Variable length character string"
	break;
      case qeDECIMAL:
	colTypesSize += 14;		// "Decimal number"
	break;
      case qeINTEGER:
	colTypesSize += 12;		// "Long integer"
	break;
      case qeSMALLINT:
	colTypesSize += 7;		// "Integer"
	break;
      case qeFLOAT:
	colTypesSize += 5;		// "Float"
	break;
      case qeDOUBLEPRECISION:
	colTypesSize += 12;		// "Double Float"
	break;
      case qeDATETIME:
	if ((*qe_ColDateStart)(cursor, (short) i) == 0)
	  if ((*qe_ColDateEnd)(cursor, (short) i) == 9)
	    colTypesSize += 4;		// "Date"
	  else
	    colTypesSize += 9;		// "Date-Time"
	else
	  colTypesSize += 4;		// "Time"
	break;
    } // switch
  } // for

  /* ---- writes column names, and column types ---- */
  colNames = (char *) mb.GAlloc(colNamesSize);
  colNamesPtr = colNames;
  colTypes = (char *) mb.GAlloc(colTypesSize);
  colTypesPtr = colTypes;

  for (i = 1; i <= colCount; i++)
  { /* ---- column name ---- */
    colName = qe_ColName(cursor, (short) i);
    colNamesSize = lstrlen(colName);
    if (qsqlCore->isOEM)
      OemToAnsi(colName, colNamesPtr);
    else
      CopyMemory(colNamesPtr, colName, colNamesSize);
    colNamesPtr += colNamesSize;

    /* ---- column type ---- */
    switch (qe_ColType(cursor, (short) i))
    { case qeCHAR:
	CopyMemory(colTypesPtr, "Fixed length character string", 29);
	colTypesPtr += 29;		// "Fixed length character string"
	break;
      case qeVARCHAR:
	CopyMemory(colTypesPtr, "Variable length character string", 32);
	colTypesPtr += 32;		// "Variable length character string"
	break;
      case qeDECIMAL:
	CopyMemory(colTypesPtr, "Decimal number", 14);
	colTypesPtr += 14;		// "Decimal Number"
	break;
      case qeINTEGER:
	CopyMemory(colTypesPtr, "Long integer", 12);
	colTypesPtr += 12;		// "Long integer"
	break;
      case qeSMALLINT:
	CopyMemory(colTypesPtr, "Integer", 7);
	colTypesPtr += 7;		// "Integer"
	break;
      case qeFLOAT:
	CopyMemory(colTypesPtr, "Float", 5);
	colTypesPtr += 5;		// "Float"
	break;
      case qeDOUBLEPRECISION:
	CopyMemory(colTypesPtr, "Double float", 12);
	colTypesPtr += 12;		// "Double float"
	break;
      case qeDATETIME:
	if ((*qe_ColDateStart)(cursor, (short) i) == 0)
	  if ((*qe_ColDateEnd)(cursor, (short) i) == 9)
	  { CopyMemory(colTypesPtr, "Date", 4);
	    colTypesPtr += 4;		// "Date"
	  } // if
	  else
	  { CopyMemory(colTypesPtr, "Date-Time", 9);
	    colTypesPtr += 9;		// "Date-Time"
	  } // else
	else
	{ CopyMemory(colTypesPtr, "Time", 4);
	  colTypesPtr += 4;		// "Time"
	} // else
	break;
    } // switch

    /* ---- row separator ---- */
    if (i < colCount)
    { *colNamesPtr++ = *colTypesPtr++ = '\r';
      *colNamesPtr++ = *colTypesPtr++ = '\n';
    } // if
  } // for
  *colNamesPtr = *colTypesPtr = '\0';	// 0 terminated string

  errorCode = (*qe_EndSQL)(cursor);
  if (fnc.RecordError(qsqlCore, errorCode))
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((colNamesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Describe Table: Couldn't create the Column Names output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(colNamesObj, colNames);
  AFuncSetParameter(1, colNamesObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((colTypesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Describe Table: Couldn't create the Column Types output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(colTypesObj, colTypes);
  AFuncSetParameter(2, colTypesObj);

  /* ---- FLOW 1 (Describe) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlDescribeTableFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlExecuteFunc                                        |
 | DESCRIPTION  : Executes the SQL statement associated with the ODBC SQL |
 |		  object.                                                 |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object that owns the |
 |		  SQL statement to execute.                               |
 | OUTPUT 1(OPT): Rows Affected: NUMBER -- The number of rows affected    |
 |		  after successfully executing an UPDATE, DELETE, or      |
 |		  INSERT statement.                                       |
 | FLOW 1       : Executed.                                               |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlExecuteFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  int		selColCount;		// number of SELCOLs
  Col		*selCol;		// an array SELCOLs
  Col		*selColPtr;		// SELCOL pointer
  AOBJECTID	rowAffObj;		// number object	      [O: 1]

  QSQL_FNC	fnc;			// qsql_fnc class
  qeHANDLE	hdbc;			// database connection
  AMEMBLOCKID	sqlStmtMB;		// sql statement memory block
  char		*sqlStmt;		// sql statement
  QSqlPrec	pc;			// qsqlpc class
  AMEMBLOCKID	qeSqlStmtMB;		// q+e sql statement memory block
  char		*qeSqlStmt;		// q+e sql statement
  char		errMssg[sizeof(theSystem->errorMessage)];
					// error message
  long		colCount;		// number of columns
  int		colNameSize;		// column name size
  char		*colName;		// column name
  qeSTATUS	errorCode;		// error code

  ODViewColDesc	colDesc;		// column description
  long		colNamesSize;		// column names size
  char		*colNamesPtr;		// column names pointer

  int		i;			// counter

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Execute: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Execute: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  /* ---- gets hdbc ---- */
  if (!fnc.GetHDBC(qsqlCore->connectObj, &hdbc))
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC SQL\r\n"
      "Execute: The database wasn't connected.");
    return;
  } // if

  /* ---- gets query ---- */
  sqlStmtMB = mb.Allocate();
  OTextGetToMemblock(qsqlCore->sqlObj, sqlStmtMB);
  if ((sqlStmt = (char *) mb.GetPointer(sqlStmtMB)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Execute: Couldn't get the SQL Statement.");
    return;
  } // if

  /* ---- closes the current cursor ---- */
  if (qsqlCore->isCursor)		// an open cursor
  { errorCode = (*qe_EndSQL)(qsqlCore->cursor);
    if (fnc.RecordError(qsqlCore, errorCode))
    { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

      /* ---- FLOW 3 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if
    qsqlCore->isCursor = FALSE;

    fnc.Dispose(qsqlCore);
  } // if

  /* ---- precompiles the sql statement ---- */
  qeSqlStmtMB = mb.Allocate();
  if (!pc.Precompile(qsqlObj, sqlStmt, errMssg, sizeof(errMssg),
		     qeSqlStmtMB))	// creates selectMB too
  { /* ---- FLOW 2 (Bind Failed) ---- */
    AFuncReportResult(theSystem, 2, errMssg);
    return;
  } // if
  qeSqlStmt = (char *) mb.GetPointer(qeSqlStmtMB);

  /* ---- executes q+e sql statement ---- */
  if (!(qsqlCore->cursor = (*qe_ExecSQL)(hdbc, qeSqlStmt)))
  { errorCode = (*qe_Err)();
    if (fnc.RecordError(qsqlCore, errorCode))
      AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    fnc.Dispose(qsqlCore);

    /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- binds columns ---- */
  colCount = (*qe_NumCols)(qsqlCore->cursor);
  selColCount = AMemGetSize(qsqlCore->selectMB) / sizeof(Col);
  if (colCount < selColCount)
  { (*qe_EndSQL)(qsqlCore->cursor);
    fnc.Dispose(qsqlCore);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC SQL\r\n"
      "Execute: The number of variables is greater than "
      "the number of columns selected.");
    return;
  } // if
  if (colCount > 0)
  { if (colCount > selColCount)
    { MemResize(qsqlCore->selectMB, colCount * sizeof(Col));
      selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

      for (selColPtr = selCol + selColCount,
	   i = selColCount; i < colCount; i++,
	   selColPtr++)
      { selColPtr->varObj = 0;
        selColPtr->varType = 0;
      } // for
    } // if
    else
      selCol = (Col *) mb.GetPointer(qsqlCore->selectMB);

    colNamesSize = 0;
    for (selColPtr = selCol, i = 1; i <= colCount; i++, selColPtr++)
    { colName = (*qe_ColName)(qsqlCore->cursor, (short) i);
      colNameSize = lstrlen(colName) + 1;

      colNamesSize += colNameSize;

      selColPtr->name = (char *) MBAlloc(colNameSize);
      if (qsqlCore->isOEM)
	OemToAnsi(colName, selColPtr->name);
      else
	lstrcpy(selColPtr->name, colName);

      selColPtr->type = (*qe_ColType)(qsqlCore->cursor, (short) i);

      selColPtr->size = (*qe_ColWidth)(qsqlCore->cursor, (short) i);
      if (selColPtr->size > QECHUNK)
	selColPtr->size = QECHUNK;

      selColPtr->bindDataSize = selColPtr->size;
      selColPtr->bindData = (char *) MBAlloc(selColPtr->size);
      if ((*qe_BindCol)(qsqlCore->cursor, (short) i,
			selColPtr->bindData, &selColPtr->bindDataSize))
      { errorCode = (*qe_Err)();
	if (fnc.RecordError(qsqlCore, errorCode))
          AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

        (*qe_EndSQL)(qsqlCore->cursor);		
	fnc.Dispose(qsqlCore);

        /* ---- FLOW 3 (Failed) ---- */
        AFuncReportResult(theSystem, 2, "");
        return;
      } // if                                              

      selColPtr->dataSizeHdl = 0;
      selColPtr->dataHdl = 0;
    } // for

    if (qsqlCore->isDView)
    { /* ---- database view ---- */
      colDesc.rowCount = (int) (*qe_FetchNumRecs)(qsqlCore->cursor);
      colDesc.colCount = (short) colCount;

      colDesc.colNames = (char *) MBAlloc(colNamesSize);
      colNamesPtr = colDesc.colNames;
      for (selColPtr = selCol, i = 1; i <= colCount; i++, selColPtr++)
      { colNameSize = lstrlen(selColPtr->name) + 1;
	CopyMemory(colNamesPtr, selColPtr->name, colNameSize);
	colNamesPtr += colNameSize;
      } // for

      qsqlCore->recCount = colDesc.rowCount;// special case for # of records
      fnc.ChangeDView(qsqlObj, qsqlCore, colDesc);
    } // if 
  } // if
  qsqlCore->isCursor = TRUE;

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((rowAffObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Execute: Couldn't create the Rows Affected output parameter.");
    return;
  } // if
  ONmbrSetInteger(rowAffObj, (*qe_NumModRecs)(qsqlCore->cursor), TRUE);
  AFuncSetParameter(1, rowAffObj);

  /* ---- FLOW 1 (Executed) ---- */
  AFuncReportResult(theSystem, 1, "");   
} // OQsqlExecuteFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlFetchFunc 		                          |
 | DESCRIPTION  : Retrieves rows of SELECT results, and places them into  |
 |		  the objects listed in the sql statement.                |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The object that owns the query to |
 |		  fetch data from.                                        |
 | INPUT 2(OPT)	: Rows: NUMBER -- The number of rows to fetch.  This      |
 |		  parameter is only necessary if the programmer wants to  |
 |		  override the row count setting in the ODBC SQL object.  |
 | OUTPUT 1(OPT): Rows Fetched: NUMBER -- The number of rows were         |
 |		  actually fetched from the DBMS.                         |
 | FLOW 1       : Fetched.                                                |
 | FLOW 2       : No more data.                                           |
 | FLOW 3	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlFetchFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID      rowCountObj;		// number object	      [I: 2]
  long		rowCount;		// number of rows
  AOBJECTID	rowFetchedObj;		// number object	      [O: 1]

  QSQL_FNC	fnc;			// qsql_fnc class

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Fetch: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if ((rowCountObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)) == 0)
    rowCount = qsqlCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount <= 0)			// checks lower bound
    rowCount = rowCount? -rowCount : 1;

  if (!qsqlCore->isCursor)
  { /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "ODBC SQL\r\n"
      "Fetch: The query wasn't executed.");
    return;
  } // if

  rowCount = fnc.Fetch(qsqlCore, rowCount);
  if (rowCount == -1)
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "");
    return;
  } // if
  if (!rowCount)
  { /* ---- FLOW 2 (No more data) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  fnc.FetchToObject(qsqlCore, rowCount);

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((rowFetchedObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch: Couldn't create the Rows Fetched output parameter.");
    return;
  } // if
  ONmbrSetInteger(rowFetchedObj, rowCount, TRUE);
  AFuncSetParameter(1, rowFetchedObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlFetchFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlFetchToTableFunc                                   |
 | DESCRIPTION  : Retrieves rows of SELECT results, and places them into  |
 |		  the specified Table object.                             |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The object that owns the query to |
 |		  fetch data from.                                        |
 | INPUT 2(OPT)	: Rows: NUMBER -- The number of rows to fetch.  This      |
 |		  parameter is only necessary if the programmer wants to  |
 |		  override the row count setting in the ODBC SQL object.  |
 | INPUT 3	: Table: TABLE -- The results of the query formatted into |
 |		  a Table object.                                         |
 | OUTPUT 1(OPT): Rows Fetched: NUMBER -- The number of rows were         |
 |		  actually fetched from the DBMS.                         |
 | FLOW 1       : Fetched.                                                |
 | FLOW 2       : No more data.                                           |
 | FLOW 3	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlFetchToTableFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID      rowCountObj;		// number object	      [I: 2]
  long		rowCount;		// number of rows
  AOBJECTID	tableObj;		// table object		      (I: 3)
  AOBJECTID	rowFetchedObj;		// number object	      [O: 1]

  QSQL_FNC	fnc;			// qsql_fnc class

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Fetch To Table: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Table: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if ((rowCountObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)) == 0)
    rowCount = qsqlCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount <= 0)			// checks lower bound
    rowCount = rowCount? -rowCount : 1;

  /* ---- FUNCTION INPUT 3 ---- */
  tableObj = AFuncGetTypedParameter(3, OTYPE_TABLE);
  if (!tableObj)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Table: Couldn't get the Table input parameter.");
    return;
  } // if

  if (!qsqlCore->isCursor)
  { /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "ODBC SQL\r\n"
      "Fetch To Table: The query wasn't executed.");
    return;
  } // if

  rowCount = fnc.Fetch(qsqlCore, rowCount);
  if (rowCount == -1)
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "");
    return;
  } // if
  if (!rowCount)
  { /* ---- FLOW 2 (No more data) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  fnc.FetchToTable(qsqlCore, rowCount, tableObj);

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((rowFetchedObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Table: Couldn't create the Rows Fetched output parameter.");
    return;
  } // if
  ONmbrSetInteger(rowFetchedObj, rowCount, TRUE);
  AFuncSetParameter(1, rowFetchedObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlFetchToTableFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OQsqlFetchToTextFunc                                    |
 | DESCRIPTION  : Retrieves rows of SELECT results, and places them into  |
 |		  a Text object.                                          |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The object that owns the query to |
 |		  fetch data from.                                        |
 | INPUT 2(OPT)	: Rows: NUMBER -- The number of rows to fetch.  This      |
 |		  parameter is only necessary if the programmer wants to  |
 |		  override the row count setting in the ODBC SQL object.  |
 | OUTPUT 1	: Text: TEXT -- The results of the query concatenated     |
 |		  into a text object.                                     |
 | OUTPUT 2(OPT): Rows Fetched: NUMBER -- The number of rows were         |
 |		  actually fetched from the DBMS.                         |
 | FLOW 1       : Fetched.                                                |
 | FLOW 2       : No more data.                                           |
 | FLOW 3	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlFetchToTextFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID      rowCountObj;		// number object	      [I: 2]
  long		rowCount;		// number of rows
  AOBJECTID	textObj;		// text object		      (O: 1)
  AOBJECTID	rowFetchedObj;		// number object	      [O: 2]

  QSQL_FNC	fnc;			// qsql_fnc class

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Fetch To Text: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Text: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if ((rowCountObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)) == 0)
    rowCount = qsqlCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount <= 0)			// checks lower bound
    rowCount = rowCount? -rowCount : 1;

  if (!qsqlCore->isCursor)
  { /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "ODBC SQL\r\n"
      "Fetch To Text: The query wasn't executed.");
    return;
  } // if

  rowCount = fnc.Fetch(qsqlCore, rowCount);
  if (rowCount == -1)
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 3 (Failed) ---- */
    AFuncReportResult(theSystem, 3, "");
    return;
  } // if
  if (!rowCount)
  { /* ---- FLOW 2 (No more data) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((textObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Text: Couldn't create the Text output parameter.");
    return;
  } // if
  fnc.FetchToText(qsqlCore, rowCount, textObj);
  AFuncSetParameter(1, textObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((rowFetchedObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Fetch To Text: Couldn't create the Rows Fetched output parameter.");
    return;
  } // if
  ONmbrSetInteger(rowFetchedObj, rowCount, TRUE);
  AFuncSetParameter(2, rowFetchedObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlFetchToTextFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlFlushFunc                                          |
 | DESCRIPTION  : Flushes results used by a SQL statement.                |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object containing    |
 |		  the results.                                            |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Flushed.                                                |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlFlushFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object
  QsqlCore	*qsqlCore;		// qsql core info

  QSQL_FNC	fnc;			// qsql_fnc class
  qeSTATUS	errorCode;		// error code

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Flush: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Flush: Couldn't get the ODBC input parameter.");
    return;
  } // if

  if (!qsqlCore->isCursor)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC SQL\r\n"
      "Flush: The query wasn't executed.");
    return;
  } // if

  errorCode = (*qe_EndSQL)(qsqlCore->cursor);
  if (fnc.RecordError(qsqlCore, errorCode))
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  qsqlCore->isCursor = FALSE;

  fnc.Dispose(qsqlCore);

  qsqlCore->recCount = -1;		// special case for # of records

  /* ---- FLOW 1 (Flushed) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlFlushFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlGetErrorFunc                                       |
 | DESCRIPTION  : Returns the most recent ODBC error code, and the        |
 |		  associated error message.                               |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object to retrieve   |
 |		  the error from.                                         |
 | OUTPUT 1(OPT): Error Number: NUMBER -- The returned ODBC error code.   |
 | OUTPUT 2(OPT): Error Message: TEXT -- The returned ODBC error message. |
 | FLOW 1       : Retrieved.                                              |
 | FLOW 2       : No error.                                               |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlGetErrorFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	qsqlObj;		// qsql object		     (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID	errorNmbrObj;		// number object	     [O: 1]
  AOBJECTID	errorMsgObj;		// text object		     [O: 2]

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Get Error: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Error: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if

  if (qsqlCore->errorCode == qeSUCCESS)
  { /* ---- FLOW 1 (No error) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((errorNmbrObj = AObjCreate(OTYPE_NUMBER)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Error: Couldn't create the Error Code output parameter.");
    return;
  } // if
  ONmbrSetInteger(errorNmbrObj, qsqlCore->errorCode, TRUE);
  AFuncSetParameter(1, errorNmbrObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((errorMsgObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Error: Couldn't create the Error Message output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(errorMsgObj, qsqlCore->errorMsg);
  AFuncSetParameter(2, errorMsgObj);

  /* ---- FLOW 1 (Retrieved) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlGetErrorFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlGetVariablesFunc                                   |
 | DESCRIPTION  : Returns variable names, and the associated variable     |
 |		  types from a given ODBC SQL object.                     |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object used to get   |
 |		  the variables from.                                     |
 | OUTPUT 1(OPT): Names: TEXT -- A "\r\n" separated variable names.       |
 | OUTPUT 2(OPT): Types: TEXT -- A "\r\n" separated variable types.       |
 |                Variable types are "Text", "Number (whole)",            |
 |		  "Number (real)", "Date", and "Time".                    |
 | FLOW 1       : Done.                                                   |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlGetVariablesFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  int		varCount;		// number of variables
  Var		*var;			// an array of variables
  Var		*varPtr;		// variable pointer
  AOBJECTID      namesObj;		// text objet		      [O: 1]
  long          namesSize;		// names size
  char		*names;			// "\r\n" separated names
  char          *namesPtr;		// names pointer
  AOBJECTID      typesObj;		// text object		      [O: 2]
  long          typesSize;		// types size
  char		*types;			// "\r\n" separated types
  char          *typesPtr;		// types pointer

  int          i;              	// counter

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((varCount = AObjGetDataSize(qsqlObj, QSQLVARS) / sizeof(Var)) != 0)
    if ((var = (Var *) mb.GetPointer(qsqlObj, QSQLVARS)) == 0)
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Get Variables: Couldn't get the ODBC SQL input parameter.");
      return;
    } // if

  /* ---- calculates name, and type sizes ---- */
  /* ---- row separators ---- */
  if (varCount > 1)	       
    namesSize = typesSize = 2 * (varCount - 1) + 1;	
  else
    namesSize = typesSize = 1;		// 0 terminated string

  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { /* ---- column name ---- */
    namesSize += lstrlen(varPtr->name);

    /* ---- column type ---- */
    switch ((long) varPtr->type)
    { case OTYPE_TEXT:
	typesSize += 4;			// "Text"
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(varPtr->obj) == ONMBR_INTEGER)
	  typesSize += 14;		// "Number (whole)"
	else
	  typesSize += 13;		// "Number (real)"
	break;
      case OTYPE_DATE:			
	typesSize += 4;			// "Date"
	break;
      case OTYPE_TIME:
	typesSize += 4;			// "Time"
	break;
    } // switch
  } // for

  /* ---- writes names, and types ---- */
  names = (char *) mb.GAlloc(namesSize);
  namesPtr = names;
  types = (char *) mb.GAlloc(typesSize);
  typesPtr = types;

  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { /* ---- column name ---- */
    namesSize = lstrlen(varPtr->name);
    CopyMemory(namesPtr, varPtr->name, namesSize);
    namesPtr += namesSize;

    /* ---- column type ---- */
    switch ((long) varPtr->type)
    { case OTYPE_TEXT:
	CopyMemory(typesPtr, "Text", 4);
	typesPtr += 4;			// "Text"
	break;
      case OTYPE_NUMBER:
	if (ONmbrGetType(varPtr->obj) == ONMBR_INTEGER)
	{ CopyMemory(typesPtr, "Number (whole)", 14);
	  typesPtr += 14;		// "Number (whole)"
	} // if
	else
	{ CopyMemory(typesPtr, "Number (real)", 13);
	  typesPtr += 13;		// "Number (real)"
	} // else
	break;
      case OTYPE_DATE:
	CopyMemory(typesPtr, "Date", 4);
	typesPtr += 4;			// "Date"
	break;
      case OTYPE_TIME:
	CopyMemory(typesPtr, "Time", 4);
	typesPtr += 4;			// "Time"
	break;
    } // switch

    /* ---- row separator ---- */
    if (i < varCount - 1)
    { *namesPtr++ = *typesPtr++ = '\r';	// "\r\n"
      *namesPtr++ = *typesPtr++ = '\n';
    } // if
  } // for
  *namesPtr = *typesPtr = '\0';		// 0 terminated string

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((namesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Variables: Couldn't create the Names output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(namesObj, names);
  AFuncSetParameter(1, namesObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((typesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Variables: Couldn't create the Types output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(typesObj, types);
  AFuncSetParameter(2, typesObj);

  /* ---- FLOW 1 (Fetched) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlGetVariablesFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OQsqlGetTablesFunc                                      |
 | DESCRIPTION  : Returns table names, and the associated table types.    |
 | INPUT 1      : ODBC SQL: ODBC SQL -- The ODBC SQL object containing    |
 |		  the connection info.                                    |
 | OUTPUT 1(OPT): Names: TEXT -- A "\r\n" separated table names.          |
 | OUTPUT 2(OPT): Types: TEXT -- A "\r\n" separated table types.  Table   |
 |		  types are "TABLE", "VIEW", and "SYNONYM".               |
 | FLOW 1       : Retrieved.                                              |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., November 1, 1993.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OQsqlGetTablesFunc
( pAEvtInfo,				// event pointer
  pAObjMessage  theSystem       	// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID      qsqlObj;		// qsql object		      (I: 1)
  QsqlCore	*qsqlCore;		// qsql core info
  AOBJECTID      namesObj;		// text object		      [O: 1]
  long          namesSize;		// names size
  char		*names;			// "\r\n" separated names
  char          *namesPtr;		// names pointer
  Queue		nameQueue;		// name queue
  int		nameLen;		// name length		
  char		*namePtr;		// name pointer
  AOBJECTID	typesObj;		// text object		      [O: 2]
  long		typesSize;		// types size
  char		*types;			// "\r\n" separated types
  char		*typesPtr;		// types pointer
  Queue		typeQueue;		// type queue

  QSQL_FNC	fnc;			// qsql_fnc class
  qeHANDLE	hdbc;			// database connection handle
  qeHANDLE	cursor;			// cursor
  int		colCount;		// number of columns
  char		tblName[MXSTRSIZE];	// table name
  short		tblType;		// table type			
  char		dummy[MXSTRSIZE];	// dummy
  long		bindDataSize;		// bind data size
  long		rowFetchedCount;	// number of rows fetched		
  qeSTATUS	errorCode;		// error code

  int		i;			// counter

  if (!sqlLib.IsLoadLib())		// no library
    if (!sqlLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
	"Get Tables: Couldn't find the Q+E ODBC DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  qsqlObj = AFuncGetTypedParameter(1, OTYPE_QSQL);
  if ((qsqlCore = (QsqlCore *) mb.GetPointer(qsqlObj, QSQLCORE)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Tables: Couldn't get the ODBC SQL input parameter.");
    return;
  } // if 

  /* ---- gets hdbc ---- */
  if (!fnc.GetHDBC(qsqlCore->connectObj, &hdbc))
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "ODBC SQL\r\n"
      "Get Tables: The database wasn't connected.");
    return;
  } //

  /* ---- gets cursor ---- */
  if (!(cursor = (*qe_Tables)(hdbc, "%", "%", "%", qeTBL_TABLE |
			      qeTBL_VIEW | qeTBL_SYNONYM)))
  { errorCode = (*qe_Err)();
    if (fnc.RecordError(qsqlCore, errorCode))
      AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if				  

  /* ---- binds columns ---- */
  for (colCount = (*qe_NumCols)(cursor), i = 1; i <= colCount; i++)
  { switch (i)
    { case qeTBL_TABLE_NAME_COL:
	bindDataSize = sizeof(tblName);
	errorCode = (*qe_BindCol)(cursor, (short) i, tblName,
				  &bindDataSize);
	break;
      case qeTBL_TYPE_COL:
	bindDataSize = sizeof(tblType);
	errorCode = (*qe_BindCol)(cursor, (short) i, &tblType,
				  &bindDataSize);
	break;
      default:
	bindDataSize = sizeof(dummy);
	errorCode = (*qe_BindCol)(cursor, (short) i, dummy, &bindDataSize);
	break;
    } // switch
    if (fnc.RecordError(qsqlCore, errorCode))
    { (*qe_EndSQL)(cursor);
      AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if
  } // for

  /* ---- fetches table names, and table types ---- */
  namesSize = typesSize = 1;		// 0 terminated string

  rowFetchedCount = 0;
  while ((errorCode = (*qe_FetchNext)(cursor)) == qeSUCCESS)
  { /* ---- table name ---- */
    nameLen = lstrlen(tblName);
    namesSize += nameLen;

    namePtr = (char *) mb.Alloc(nameLen + 1);
    if (qsqlCore->isOEM)
      OemToAnsi(tblName, namePtr);
    else
      lstrcpy(namePtr, tblName);
    nameQueue.Enque(namePtr);

    /* ---- table type ---- */
    switch (tblType)
    { case qeTBL_TABLE:
	typesSize += 5;			// "TABLE"
        break;	
      case qeTBL_VIEW:
	typesSize += 4;			// "VIEW"
        break;
      case qeTBL_SYNONYM:
	typesSize += 7;			// "SYNONYM"
        break;
    } // switch
    typeQueue.Enque((void *) tblType);

    rowFetchedCount++; 
  } // while
  if (errorCode != qeEOF)
  { (*qe_EndSQL)(cursor);
    if (fnc.RecordError(qsqlCore, errorCode))
      AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if				  

  /* ---- row separators ---- */
  if (rowFetchedCount > 1)		// "\r\n"
  { namesSize += (rowFetchedCount - 1) * 2;
    typesSize += (rowFetchedCount - 1) * 2;
  } // if 

  /* ---- closes cursor ---- */
  errorCode = (*qe_EndSQL)(cursor);
  if (fnc.RecordError(qsqlCore, errorCode))
  { AEvtPostSignalAtTail(qsqlObj, QSQLIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");	
    return;
  } // if

  /* ---- writes table names, and table types ---- */
  names = (char *) mb.GAlloc(namesSize);
  namesPtr = names;
  types = (char *) mb.GAlloc(typesSize);
  typesPtr = types;
     
  while (rowFetchedCount)
  { /* ---- table name ---- */
    namePtr = (char *) nameQueue.Deque();
    nameLen = lstrlen(namePtr);

    CopyMemory(namesPtr, namePtr, nameLen);
    namesPtr += nameLen;

    /* ---- table type ---- */
    switch ((short) typeQueue.Deque())
    { case qeTBL_TABLE:
        CopyMemory(typesPtr, "TABLE", 5);	
	typesPtr += 5;			// "TABLE"							
        break;	
      case qeTBL_VIEW:
        CopyMemory(typesPtr, "VIEW", 4);	
	typesPtr += 4;			// "VIEW"		       
        break;
      case qeTBL_SYNONYM:
        CopyMemory(typesPtr, "SYNONYM", 7);
	typesPtr += 7;			// "SYNONYM"			
        break;
    } // switch

    /* ---- row separator ---- */
    if (rowFetchedCount > 1)		// "\r\n"
    { *namesPtr++ = *typesPtr++ = '\r';
      *namesPtr++ = *typesPtr++ = '\n';
    } // if  			

    rowFetchedCount--;
  } // while
  *namesPtr = *typesPtr = '\0';		// 0 terminated string

  /* ---- FUNCTION OUTPUT 1 ---- */
  if ((namesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Tables: Couldn't create the Names output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(namesObj, names);
  AFuncSetParameter(1, namesObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if ((typesObj = AObjCreate(OTYPE_TEXT)) == 0)
  { AFuncReportResult(theSystem, 0, "ODBC SQL\r\n"
      "Get Tables: Couldn't create the Types output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(typesObj, types);
  AFuncSetParameter(2, typesObj);

  /* ---- FLOW 1 (Retrieved) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OQsqlGetTablesFunc
