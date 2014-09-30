/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

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
 | FILE		: SQLD_FNC.CPP                                            |
 | DESCRIPTION	: SequeLink SQL Functions.                                |
 | COPYRIGHT	: Novell, Inc.                                            |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Mar-1994  Initial Edit.                        |
\*------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "sqld_fnc.h"
#include "sqldpc.h"
#include "slex.h"
#include "sqlnkutl.h"
#include "queue.h"
#include "memblock.h"
#include "safemem.h"


BOOL SQLD_FNC::Cancel		/* ---- Cancel -------------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SSPRefNum	cursorRef		// cursor reference number
)
/* Returns true or false as the result of cancelling the cursor. */
{
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQCancel(cursorRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // Cancel

BOOL SQLD_FNC::Close		/* ---- Close --------------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SSPRefNum	cursorRef		// cursor reference number
)
/* Returns true or false as the result of closing the cursor. */
{
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQClose(cursorRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // Close

BOOL SQLD_FNC::Commit		/* ---- Commit -------------------------- */
( SqldCore	*sqldCore		// sqld core info
)
/* Returns true or false as the result of committing the changes. */
{
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode =
    SQCommit(sqldCore->logonRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // Commit

BOOL SQLD_FNC::Disconnect	/* ---- Disconnect ---------------------- */
( AOBJECTID	sqldObj,		// sqld object
  SqldCore	*sqldCore		// sqld core info
)
/* Returns true or false as the result of disconnecting the database. */
{
  if (sqldCore->isLogonRef)
    if (!Logoff(sqldObj, sqldCore))
      return FALSE;

  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQDisconnect(sqldCore->serverRef);
  if (sqldCore->clientCode)
    return FALSE;
  sqldCore->isServerRef = FALSE;
  return TRUE;
} // Disconnect

BOOL SQLD_FNC::Logoff		/* ---- Logoff -------------------------- */
( AOBJECTID	sqldObj,		// sqld object
  SqldCore	*sqldCore		// sqld core info
)
/* Returns true or false as the result of Logoffs from the database. */
{
  switch (sqldCore->mrcType)
  { case AUTOCOMMIT:
      if (!Commit(sqldCore))
	return FALSE;
      break;
    case AUTOROLLBACK:
      if (!Rollback(sqldCore))
	return FALSE;
      break;
  } // switch

  DisposeCursors(sqldObj, sqldCore);

  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode =
    SQLogoff(sqldCore->logonRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  sqldCore->isLogonRef = FALSE;
  return TRUE;
} // Logoff

BOOL SQLD_FNC::Rollback		/* ---- Rollback ------------------------ */
( SqldCore	*sqldCore		// sqld core info
)
/* Returns true or false as the result of calling SQRollback. */
{
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode =
    SQRollback(sqldCore->logonRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // Rollback


void SQLD_FNC::DisposeBindBuff	/* ---- Dispose Bind Buffer ------------- */
( SqldCursor	 *cursor		// cursor
)
/* Disposes bind buffers. */
{
  if (!cursor->isBindMB)		
    return;				// nothing to dispose

  AMemFree(cursor->bindMB);
  cursor->isBindMB = FALSE;
} // DisposeBindBuff

void SQLD_FNC::DisposeSelectBuff /* --- Dispose Select Buffer ----------- */
( SqldCursor	 *cursor		// cursor
)
/* Disposes select buffers. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of columns
  SqldCol	*col;			// an array of columns
  SqldCol	 *colPtr;		// column pointer

  int		i;			// counter

  if (!cursor->isSelectMB)
    return;				// nothing to dispose

  colCount = AMemGetSize(cursor->selectMB) / sizeof(SqldCursor);
  if (colCount)
  { col = (SqldCol *) mb.GetPointer(cursor->selectMB);

    /* ---- disposes data buffers ---- */
    for (colPtr = col, i = 0; i < colCount; i++, colPtr++)
      if (colPtr->nameMB)
      { AMemFree(colPtr->nameMB);
        colPtr->nameMB = 0;
      } // if
  } // if
  AMemFree(cursor->selectMB);
  cursor->isSelectMB = FALSE;  
} // DisposeSelectBuff

void SQLD_FNC::Dispose		/* ---- Dispose ------------------------- */
( SqldCursor	 *cursor		// cursor
)
/* Disposes all memory blocks. */
{
  DisposeBindBuff(cursor);
  DisposeSelectBuff(cursor);
} // Dispose
					

BOOL SQLD_FNC::DefineBind	/* ---- Define Bind --------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	cursor			// sqld cursor
)
/* Defines bind columns buffers. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of columns
  SqldCol	*col;			// an array of columns
  SqldCol	*colPtr;		// column pointer

  SSPDefList	*defList;		// define list
  SSPDefList	*defListPtr;		// define list pointer

  int		i;			// counter

  if ((colCount = AMemGetSize(cursor.bindMB) / sizeof(SqldCol)) != 0)
  { col = (SqldCol *) mb.GetPointer(cursor.bindMB);

    defList = (SSPDefList *) mb.Alloc(colCount * sizeof(SSPDefList));

    for (colPtr = col, defListPtr = defList,
         i = 0; i < colCount; i++,
         colPtr++, defListPtr++)
    { defListPtr->Kind = kSSPBindKind;
      defListPtr->Type = colPtr->type;
      defListPtr->Size = colPtr->size;

      defListPtr->Buffer = NULL;	// will be supplied by PutVarDta

      defListPtr->VarName = NULL;
      defListPtr->Indicator = NULL;
      defListPtr->IndName = NULL;
      defListPtr->RelCursor = 0;
    } // for

    sqldCore->reasonCode = 0;
    sqldCore->serverCode = 0;
    sqldCore->clientCode = SQnDefine(cursor.cursorRef, colCount, defList,
		                     &sqldCore->serverCode);
    if (sqldCore->clientCode || sqldCore->serverCode)
      return FALSE;
  } // if
  return TRUE;
} // DefineBind

BOOL SQLD_FNC::DefineSelect	/* ---- Define Select ------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	cursor			// sqld cursor
)
/* Defines select columns buffers. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of columns
  SqldCol	*col;			// an array of columns
  SqldCol	 *colPtr;		// column pointer

  SSPCount	itemCount;		// number of items
  char		*name;			// variable name
  SSPDescList	(*descList)[];		// describe list
  SSPDefList	*defList;		// define list

  int		i;			// counter

  colCount = AMemGetSize(cursor.selectMB) / sizeof(SqldCol);

  /* ---- describes table ---- */
  itemCount = SSP_AllVars;
  descList = NULL;
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQnDescribe(cursor.cursorRef, kSSPSelectKind,
			   &itemCount, &descList, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;

  descList = (SSPDescList (*) []) mb.Alloc(itemCount * sizeof(SSPDescList));
  sqldCore->clientCode = SQnDescribe(cursor.cursorRef, kSSPSelectKind,
			   &itemCount, &descList, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;

  /* ---- defines table ---- */ 
  defList = (SSPDefList *) mb.Alloc(itemCount * sizeof(SSPDefList));

  if (colCount != itemCount)
    MemResize(cursor.selectMB, itemCount * sizeof(SqldCol));
  col = (SqldCol *) mb.GetPointer(cursor.selectMB);
  if (itemCount > colCount)
    for (colPtr = col + colCount,
         i = colCount; i < itemCount; i++,
	 colPtr++)
    { colPtr->obj = 0;
      colPtr->objType = 0;

      colPtr->nameMB = 0;
      colPtr->type = (*descList)[i].SqlnkType;
      switch ((*descList)[i].SqlnkType)
      { case kSSPFloat:
	  colPtr->size = sizeof(double);
        case kSSPLong:
	  colPtr->size = sizeof(long);
	  break;
        default:
	  colPtr->size = (*descList)[i].Size;
	  break;
      } // switch
    } // for

  for (colPtr = col, i = 0; i < itemCount; i ++, colPtr++)
  { /* ---- variable name ---- */
    colPtr->nameMB = MemAllocate(lstrlen((*descList)[i].VarName) + 1);
    name = (char *) mb.GetPointer(colPtr->nameMB);  
    lstrcpy(name, (*descList)[i].VarName);

    defList[i].Kind = kSSPSelectKind;
    defList[i].Type = colPtr->type;
    defList[i].Size = colPtr->size;
    defList[i].Buffer = NULL;
    defList[i].VarName = NULL;
    defList[i].Indicator = NULL;
    defList[i].IndName = NULL;
    defList[i].RelCursor = 0;
  } // for

  sqldCore->clientCode = SQnDefine(cursor.cursorRef, itemCount, defList,
				   &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // DefineSelect


BOOL SQLD_FNC::BindObjects	/* ---- Bind Objects -------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	 *cursor		// sqld cursor
)
/* Binds the objects. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of bind columns
  SqldCol	*col;			// an array of columns
  SqldCol	 *colPtr;		// column pointer

  SSPDefList	defList;		// definition list
  long double	realVal;		// real value
  AMEMBLOCKID	textMB;			// text memory block
  long		textSize;		// text size			
  char		*text;			// text

  int		i;			// counter

  if (!cursor->isBindMB)
    return TRUE;			// doesn't need to bind

  colCount = AMemGetSize(cursor->bindMB) / sizeof(SqldCol);
  if (colCount)
    col = (SqldCol *) mb.GetPointer(cursor->bindMB);
  else
    return TRUE;			// doesn't need to bind

  for (colPtr = col, i = 0; i < colCount; i++, colPtr++)
  { defList.Kind = kSSPBindKind;
    defList.Indicator = NULL;
    defList.VarName = NULL;

    switch (colPtr->type)
    { case kSSPFloat:
        defList.Buffer = (char *) mb.Alloc(colPtr->size); 
	if (colPtr->objType == OTYPE_NUMBER)
	  if (ONmbrGetType(colPtr->obj) == ONMBR_INTEGER)
            *(double *) defList.Buffer = ONmbrGetInteger(colPtr->obj);
	  else
	  { ONmbrGetReal(colPtr->obj, &realVal);
            *(double *) defList.Buffer = realVal;
	  } // else 
	else
	{ textMB = mb.Allocate();
	  if (AObjConvertToText(colPtr->obj, textMB))
	  { text = (char *) mb.GetPointer(textMB);
	    *(double *) defList.Buffer = atof(text);
	  } // if
	  else
	    *(double *) defList.Buffer = 0;
        } // else
	break;
      case kSSPLong:
	defList.Buffer = (char *) mb.Alloc(colPtr->size);
	if (colPtr->objType == OTYPE_NUMBER)
	  if (ONmbrGetType(colPtr->obj) == ONMBR_INTEGER)
	    *(long *) defList.Buffer = ONmbrGetInteger(colPtr->obj);
	  else
	  { ONmbrGetReal(colPtr->obj, &realVal);
	    *(long *) defList.Buffer = realVal;
          } // else
	else
	{ textMB = mb.Allocate();
	  if (AObjConvertToText(colPtr->obj, textMB))
	  { text = (char *) mb.GetPointer(textMB);
	    *(long *) defList.Buffer = atol(text);
	  } // if
	  else
	    *(long *) defList.Buffer = 0;
        } // else
        break;
      case kSSPDecimal:
      case kSSPChar:
	defList.Buffer = (char *) mb.Alloc(colPtr->size);
	textMB = mb.Allocate();
	if (AObjConvertToText(colPtr->obj, textMB))
	{ text = (char *) mb.GetPointer(textMB);
	  textSize = lstrlen(text) + 1;
	  if (textSize >= colPtr->size)
	  { CopyMemory(defList.Buffer, text, colPtr->size);
	    *((char  *) defList.Buffer + colPtr->size) = '\0';
	  } // if
	  else
	    lstrcpy(defList.Buffer, text);
	} // if
	else
	  *defList.Buffer = '\0';
	break;
    } // switch

    sqldCore->reasonCode = 0;
    sqldCore->serverCode = 0;
    sqldCore->clientCode = PutVarDta(cursor->cursorRef, &defList, i);
    if (sqldCore->clientCode)
      return FALSE;
  } // for

  sqldCore->clientCode = SQBind(cursor->cursorRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
    return FALSE;
  return TRUE;
} // BindObjects

BOOL SQLD_FNC::FetchToObjects	/* ---- Fetch To Objects ---------------- */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	cursor,			// sqld cursor
  long		rowCount		// number of rows
)
/* Fetches rows to the objects. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of select columns
  SqldCol	*col;			// an array of select columns   
  SqldCol	 *colPtr;		// column pointer

  SSPDefList	varDef;			// variable definition
  char		 *dataPtr;		// data pointer

  AMEMBLOCKID	textMB;			// text memory block
  long		textSize;		// text size
  char		*text;			// text
  char		 *textPtr;		// text pointer

  char		str[MXSTRSIZE];		// string

  long		l;			// counter
  int		i;			// counter

  if ((colCount = AMemGetSize(cursor.selectMB) / sizeof(SqldCol)) != 0)
    col = (SqldCol *) mb.GetPointer(cursor.selectMB);

  varDef.Kind = kSSPSelectKind;
  varDef.Buffer = NULL;
  for (i = 0, colPtr = col; i < colCount; colPtr++, i++)
    if (colPtr->obj)
    { MemBlock	mb;			// memblock class

      sqldCore->clientCode = GetVarDef(cursor.cursorRef, &varDef, i + 1);
      if (sqldCore->clientCode)
        return FALSE;

      switch (varDef.Type)
      { case kSSPFloat:
	  if (colPtr->objType == OTYPE_NUMBER)
	    ONmbrSetReal(colPtr->obj, *(double *) varDef.Buffer, FALSE);
          else			// does data conversion	
	  { /* ---- calculates the buffer size ---- */
            /* ---- row separators ---- */
	    if (rowCount > 1)
	      textSize = 2 * (rowCount - 1) + 1;
	    else
              textSize = 1;		// 0 terminated string

	    for (dataPtr = varDef.Buffer,
		 l = 0; l < rowCount; l++,
		 dataPtr += sizeof(double))
	      textSize += sprintf(str, "%lf", *(double *) dataPtr);

	    /* ---- allocates the buffer ---- */
	    textMB = mb.Allocate(textSize);
	    text = (char *) mb.GetPointer(textMB);
            textPtr = text;

	    /* ---- copies data into the buffer ---- */
	    for (dataPtr = varDef.Buffer,
		 l = 0; l < rowCount; l++,
		 dataPtr += sizeof(double))
	    { textPtr += sprintf(text, "%lf", *(double *) dataPtr);

	      /* ---- row separator ---- */
	      if (l < rowCount - 1)
	      { *textPtr++ = '\r';
                *textPtr++ = '\n';
              } // if
	    } // for
	    *textPtr = '\0';		// 0 terminated string

	    AObjConvertFromText(colPtr->obj, textMB);
	  } // else  			
	  break;
	case kSSPLong:
	  if (colPtr->objType == OTYPE_NUMBER)
	    ONmbrSetInteger(colPtr->obj, *(long *) varDef.Buffer, FALSE);
          else			// does data conversion	
	  { /* ---- calculates the buffer size ---- */
            /* ---- row separators ---- */
  	    if (rowCount > 1)
	      textSize = 2 * (rowCount - 1) + 1;
	    else
              textSize = 1;		// 0 terminated string

	    for (dataPtr = varDef.Buffer,
	         l = 0; l < rowCount; l++,
		 dataPtr += sizeof(long))
	      textSize += sprintf(str, "%ld", *(long *) dataPtr);

	    /* ---- allocates the buffer ---- */
	    textMB = mb.Allocate(textSize);
	    text = (char *) mb.GetPointer(textMB);
            textPtr = text;

	    /* ---- copies data into the buffer ---- */
	    for (dataPtr = varDef.Buffer,
		 l = 0; l < rowCount; l++,
		 dataPtr += sizeof(long))
	    { textPtr += sprintf(text, "%ld", *(long *) dataPtr);

	      /* ---- row separator ---- */
	      if (l < rowCount - 1)
	      { *textPtr++ = '\r';
                *textPtr++ = '\n';
              } // if
	    } // for
	    *textPtr = '\0';		// 0 terminated string

	    AObjConvertFromText(colPtr->obj, textMB);
	  } // else  			
	  break;
        case kSSPChar:
        case kSSPDecimal:
	  /* ---- calculates the buffer size ---- */
          /* ---- row separators ---- */
  	  if (rowCount > 1)
	    textSize = 2 * (rowCount - 1) + 1;
	  else
            textSize = 1;		// 0 terminated string

	  for (dataPtr = varDef.Buffer,
	       l = 0; l < rowCount; l++,
	       dataPtr += varDef.Size + 1)
            textSize += lstrlen(dataPtr);

	  /* ---- allocates the buffer ---- */
	  textMB = mb.Allocate(textSize);
	  text = (char *) mb.GetPointer(textMB);
          textPtr = text;

	  /* ---- copies data into the buffer ---- */
	  for (dataPtr = varDef.Buffer,
	       l = 0; l < rowCount; l++,
	       dataPtr += varDef.Size + 1)
	  { textSize = lstrlen(dataPtr);
	    CopyMemory(textPtr, dataPtr, textSize);
            textPtr += textSize;

	    /* ---- row separator ---- */
	    if (l < rowCount - 1)
	    { *textPtr++ = '\r';
              *textPtr++ = '\n';
            } // if
	  } // for
	  *textPtr = '\0';		// 0 terminated string

	  AObjConvertFromText(colPtr->obj, textMB);
	  break;
      } // switch
    } // if
  return TRUE;
} // FetchToObjects

BOOL SQLD_FNC::FetchToTable	/* ---- Fetch To Table ------------------ */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	cursor,			// sqld cursor
  long		rowCount,		// number of rows
  AOBJECTID	tableObj		// table object				
)
/* Fetches rows to the table object. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of select columns
  SqldCol	*col;			// an array of select columns   
  SqldCol	 *colPtr;		// column pointer

  SSPDefList	varDef;			// variable definition
  char		 *dataPtr;		// data pointer

  AMEMBLOCKID	textMB;			// text memory block
  long		textSize;		// text size
  char		*text;			// text
  char		 *textPtr;		// text pointer

  char		str[MXSTRSIZE];		// string

  long		l;			// counter
  int		i;			// counter

  if ((colCount = AMemGetSize(cursor.selectMB) / sizeof(SqldCol)) != 0)
    col = (SqldCol *) mb.GetPointer(cursor.selectMB);

  /* ---- sets number of columns and number of rows to the table object - */
  tblSetColumnCount(tableObj, colCount);
  tblSetRowCount(tableObj, rowCount);

  for (i = 0, colPtr = col; i < colCount; colPtr++, i++)
  { MemBlock	mb;			// memblock class

    /* ---- sets column title to the table object ---- */
    tblColumnTitle(tableObj, (char *) mb.GetPointer(colPtr->nameMB), i);

    varDef.Kind = kSSPSelectKind;
    varDef.Buffer = NULL;
    sqldCore->clientCode = GetVarDef(cursor.cursorRef, &varDef, i + 1);
    if (sqldCore->clientCode)
      return FALSE;

    switch (varDef.Type)
    { case kSSPFloat:
        /* ---- calculates the buffer size ---- */
        /* ---- row separators ---- */
	textSize = rowCount + 1;	// 0 terminated string

	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += sizeof(double))
	  textSize += sprintf(str, "%lf", *(double *) dataPtr);

	/* ---- allocates the buffer ---- */
	textMB = mb.Allocate(textSize);
	text = (char *) mb.GetPointer(textMB);
        textPtr = text;

	/* ---- copies data into the buffer ---- */
	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += sizeof(double))
	{ textSize = sprintf(str, "%lf", *(double *) dataPtr);
	  CopyMemory(textPtr, str, textSize);
	  textPtr += textSize;

	  /* ---- row separator ---- */
	  *textPtr++ = '\t';
	} // for
	*textPtr = '\0';		// 0 terminated string
	break;
      case kSSPLong:
        /* ---- calculates the buffer size ---- */
        /* ---- row separators ---- */
	textSize = rowCount + 1;	// 0 terminated string

	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += sizeof(long))
	  textSize += sprintf(str, "%ld", *(long *) dataPtr);

	/* ---- allocates the buffer ---- */
	textMB = mb.Allocate(textSize);
	text = (char *) mb.GetPointer(textMB);
        textPtr = text;

	/* ---- copies data into the buffer ---- */
	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += sizeof(long))
	{ textSize = sprintf(str, "%ld", *(long *) dataPtr);
	  CopyMemory(textPtr, str, textSize);
	  textPtr += textSize;

	  /* ---- row separator ---- */
	  *textPtr++ = '\t';
	} // for
	*textPtr = '\0';		// 0 terminated string
	break;
      case kSSPChar:
      case kSSPDecimal:
        /* ---- calculates the buffer size ---- */
	/* ---- row separators ---- */
	textSize = rowCount + 1;	// 0 terminated string

	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += varDef.Size + 1)
          textSize += lstrlen(dataPtr);

	/* ---- allocates the buffer ---- */
	text = (char *) mb.Alloc(textSize);
        textPtr = text;

	/* ---- copies data into the buffer ---- */
	for (dataPtr = varDef.Buffer,
	     l = 0; l < rowCount; l++,
	     dataPtr += varDef.Size + 1)
	{ textSize = lstrlen(dataPtr);
	  CopyMemory(textPtr, dataPtr, textSize);
          textPtr += textSize;

	  /* ---- row separator ---- */
	  *textPtr++ = '\t';
	} // for
	*textPtr = '\0';		// 0 terminated string
        break;
      default:
        text = NULL;
    } // switch

    /* ---- transfers text to the table object ---- */
    tblSetColumn(tableObj, text, i);
  } // for
  return TRUE;
} // FetchToTable

BOOL SQLD_FNC::FetchToText	/* ---- Fetch To Text ------------------- */
( SqldCore	*sqldCore,		// sqld core info
  SqldCursor	cursor,			// sqld cursor
  long		rowCount,		// number of rows
  AOBJECTID	textObj			// text object	
)
/* Fetches rows to the text object. */
{ MemBlock	mb;			// memblock class
  int		colCount;		// number of select columns
  SqldCol	*col;			// an array of select columns   
  SqldCol	 *colPtr;		// column pointer

  SSPDefList	varDef;			// variable definition
  SSPDefList	*varDefine;		// an array variable definition
  SSPDefList	 *varDefinePtr;	// variable definition pointer
  char		**data;			// data 
  char		 **dataPtr;		// data pointer

  AMEMBLOCKID	textMB;			// text memory block
  long		textSize;		// text size
  char		*text;			// text
  char		 *textPtr;		// text pointer

  char		str[MXSTRSIZE];		// string

  long		l;			// counter
  int		i;			// counter

  if ((colCount = AMemGetSize(cursor.selectMB) / sizeof(SqldCol)) != 0)
    col = (SqldCol *) mb.GetPointer(cursor.selectMB);

  varDefine = (SSPDefList *) mb.Alloc(colCount * sizeof(SSPDefList));
  data = (char **) mb.Alloc(colCount * sizeof(char *));
  for (varDefinePtr = varDefine, dataPtr = (char  **) data,
       i = 0; i < colCount; i++,
       varDefinePtr++, dataPtr++)
  { varDef.Kind = kSSPSelectKind;
    varDef.Buffer = NULL;
    sqldCore->clientCode = GetVarDef(cursor.cursorRef, &varDef, i + 1);
    if (sqldCore->clientCode)
      return FALSE;

    *varDefinePtr = varDef;
    *dataPtr = varDef.Buffer;
  } // for

  /* ---- calculates the buffer size ---- */
  /* ---- column separators, and row separators ---- */
  if (colCount > 1)
    textSize = (colCount + 1) * rowCount + 1;
  else
    textSize = 1;			// 0 terminated string

  for (l = 0; l < rowCount; l++)
    for (varDefinePtr = varDefine, dataPtr = (char  **) data,
	 colPtr = col, i = 0; i < colCount; i++, colPtr++,
	 varDefinePtr++, dataPtr++)
      switch (varDefinePtr->Type)
      { case kSSPFloat:
	  textSize += sprintf(str, "%lf", *(double *) *dataPtr);

	  *dataPtr += sizeof(double);
          break;
	case kSSPLong:
	  textSize += sprintf(str, "%ld", *(long *) *dataPtr);

	  *dataPtr += sizeof(long);
          break;
        case kSSPChar:
        case kSSPDecimal:
          textSize += lstrlen(*dataPtr);

	  *dataPtr += varDefinePtr->Size + 1;
          break;
      } // switch

  /* ---- allocates the buffer ---- */
  textMB = mb.Allocate(textSize);
  text = (char *) mb.GetPointer(textMB);
  textPtr = text;

  for (varDefinePtr = varDefine, dataPtr = (char  **) data,
       i = 0; i < colCount; i++,
       varDefinePtr++, dataPtr++)
    *dataPtr = varDefinePtr->Buffer;

  /* ---- copies data into the buffer ---- */
  for (l = 0; l < rowCount; l++)
  { for (varDefinePtr = varDefine, dataPtr = (char  **) data,
         colPtr = col, i = 0; i < colCount; i++, colPtr++,
	 varDefinePtr++, dataPtr++)
    { switch (varDefinePtr->Type)
      { case kSSPFloat:
	  textSize = sprintf(str, "%lf", *(double *) *dataPtr);
	  CopyMemory(textPtr, str, textSize);
          textPtr += textSize;

	  *dataPtr += sizeof(double);
          break;
	case kSSPLong:
	  textSize = sprintf(str, "%ld", *(long *) *dataPtr);
	  CopyMemory(textPtr, str, textSize);
          textPtr += textSize;

	  *dataPtr += sizeof(long);
          break;
        case kSSPChar:
        case kSSPDecimal:
	  textSize = lstrlen(*dataPtr);
	  CopyMemory(textPtr, *dataPtr, textSize);
          textPtr += textSize;

	  *dataPtr += varDefinePtr->Size + 1;
          break;
      } // switch

      /* ---- column separator ---- */
      if (i < colCount - 1)
	*textPtr++ = sqldCore->colSprtr;
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


void SQLD_FNC::DisposeCursors	/* ---- Dispose Cursors ----------------- */
( AOBJECTID	sqldObj,		// sqld object
  SqldCore	*sqldCore		// sqld core info
)
/* Disposes cursors. */
{ MemBlock	mb;			// memblock class
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  SqldCursor	 *cursorPtr;	// cursor pointer

  int		i;			// counter

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount)
  { cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS);

    for (cursorPtr = cursor, i = 0; i < cursorCount; i++, cursorPtr++)
    { if (!Close(sqldCore, cursorPtr->cursorRef))
	AEvtPostSignalAtMark(sqldObj, SQLDIFERROR);

      /* ---- releases resources ---- */
      Dispose(cursorPtr);
    } // for
    AObjResizeData(sqldObj, SQLDCURSORS, 0);
  } // if
} // DisposeCursors

SqldCursor *SQLD_FNC::GetCursor	/* ---- Get Cursor ---------------------- */
( int		cursorCount,		// number of cursors
  SqldCursor	*cursor,		// an array of cursors
  SSPRefNum	cursorRef		// cursor reference number
)
/* Returns pointer to the cursor. */
{ SqldCursor	 *cursorPtr;	// cursor pointer

  int		i;			// counter

  for (cursorPtr = cursor, i = 0; i < cursorCount; i++, cursorPtr++)
    if (cursorRef == cursorPtr->cursorRef)
      return (SqldCursor *) cursorPtr;
  return NULL;
} // GetCursor


/*------------------------------------------------------------------------*\
 | METHOD	: OSqldStartupMeth                                        |
 | DESCRIPTION	: SequeLink SQL object's constructor.                     |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldStartupMeth
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  AOBJECTID	sqldObj;		// sqld object
  SqldCore	*sqldCore;		// sqld core info

  long		objIdx;			// object index

  objIdx = AOBJ_GETFIRSTKEY;
  while((sqldObj = AObjGetNextObject(OTYPE_SQLD, &objIdx)) != 0)
  { AObjSetDataCount(sqldObj, SQLDCOUNTEXTEND);
    AObjResizeData(sqldObj, SQLDCURSORS, 0); 

    MemResizeObjectData(sqldObj, SQLDCORE, sizeof(SqldCore));
    sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE);
    sqldCore->clientCode = 0;
    sqldCore->serverCode = 0;
    sqldCore->reasonCode = 0;

    sqldCore->isServerRef = FALSE;
    sqldCore->isLogonRef = FALSE;
    sqldCore->dbOverride = 0;
  } // while

  if (!sqlnkLib.LoadLib())
    lstrcpy(theSystem->errorMessage,
	    "SequeLink SQL: Couldn't find the SequeLink DLL.");
} // OSqldStartupMeth

/*------------------------------------------------------------------------*\
 | METHOD       : OSqldShutdownMeth                                       |
 | DESCRIPTION  : SequeLink SQL object's destructor.                      |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldShutdownMeth
( pAEvtInfo,				// event pointer
  pAObjMessage				// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object
  SqldCore	*sqldCore;		// sqld core info

  SQLD_FNC	fnc;			// sqld_fnc class
  long		objIdx;			// object index

  objIdx = AOBJ_GETFIRSTKEY;
  while((sqldObj = AObjGetNextObject(OTYPE_SQLD, &objIdx)) != 0)
  { /* ---- releases resource ---- */
    sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE);
    if (sqldCore->isServerRef)
      fnc.Disconnect(sqldObj, sqldCore);
  } // while
} // OSqldShutdownMeth


/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldBuildQueryFunc                                     |
 | DESCRIPTION	: Builds a query from an SQL script and send it to the    |
 |		  database for processing.                                |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The object to build a      |
 |                query for.                                              |
 | INPUT 2      : Query Script: TEXT -- The SQL script to build the query |
 |                for.                                                    |
 | OUTPUT       : Query ID: NUMBER -- The ID of the built query.  This    |
 |                parameter is used in later operations to identify the   |
 |                query.                                                  |
 | FLOW 1       : After building.                                         |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldBuildQueryFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	cursor;			// cursor
  SqldCursor	 *cursorPtr;	// an array of cursors
  AOBJECTID	queryObj;		// text object		      (I: 2)
  AMEMBLOCKID	queryMB;		// query memory block
  char		*query;			// query
  AOBJECTID	queryIDObj;		// number object	      (O: 1)
  SSPRefNum     queryID;		// query id

  SQLD_FNC	fnc;			// sqld_fnc class
  SqldPC	pc;			// sqldpc class
  AMEMBLOCKID	sqlStmtMB;		// sql statement memory block
  char		*sqlStmt;		// sql statement
  char          errMssg[sizeof(theSystem->errorMessage)];
  					// error message
  SSPOffset	parseOffset;		// parse offset
  SSPCount	itemCount;		// number of items
  SSPSrvRtn	serverCode;		// server code

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Build Query: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Build Query: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryObj = AFuncGetTypedParameter(2, OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Build Query: Couldn't get the Query Script input parameter.");
    return;
  } // if
  queryMB = mb.Allocate();
  OTextGetToMemblock(queryObj, queryMB);
  if (!(query = (char *) mb.GetPointer(queryMB)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Build Query: Couldn't get the Query Script input parameter.");
    return;
  } // if

  if (!sqldCore->isLogonRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- precompiles statement ---- */
  sqlStmtMB = mb.Allocate();
  if (!pc.Precompile(sqldObj, &cursor, query, errMssg, sizeof(errMssg),
		     sqlStmtMB))
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, errMssg);
    return;
  } // if
  sqlStmt = (char *) mb.GetPointer(sqlStmtMB);

  /* ---- creates cursor (query id) ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQOpen(sqldCore->logonRef, NULL, &queryID,
                                &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  cursor.cursorRef = queryID;

  /* ---- associates sql statement with the cursor (query id) ---- */
  sqldCore->clientCode = SQAssociate(queryID, sqlStmt, &parseOffset,
                                     &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { SQClose(queryID, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- special case ---- */
  if (sqldCore->dbOverride == DB_SYBASE)
  { sqldCore->clientCode = SQExec(queryID, &itemCount,
				  &sqldCore->serverCode);
    if (sqldCore->clientCode || sqldCore->serverCode)
    { SQClose(queryID, &serverCode);
      AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if				   
  } // if

  /* ---- defines select columns buffers ---- */
  if (!fnc.DefineSelect(sqldCore, cursor))
  { SQClose(queryID, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- defines bind columns buffers ---- */
  if (!fnc.DefineBind(sqldCore, cursor))
  { SQClose(queryID, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- adds the new cursor ---- */
  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  MemResizeObjectData(sqldObj, SQLDCURSORS, (cursorCount + 1) *
					      sizeof(SqldCursor));
  cursorPtr = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS) +
			       cursorCount;
  *cursorPtr = cursor;
  cursorPtr->cursorRef = queryID;

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(queryIDObj = AObjCreate(OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Build Query: Couldn't create the Query ID output parameter.");
    return;
  } // if
  ONmbrSetInteger(queryIDObj, queryID, TRUE);
  AFuncSetParameter(1, queryIDObj);

  /* ---- FLOW 1 (After building) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldBuildQueryFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldCommitFunc                                         |
 | DESCRIPTION	: Commits changes made to the database.	                  | 
 | INPUT 1	: SequeLink SQL: SEQUELINK SQL -- The SequeLink SQL       | 
 |		  object to commit the changes for.                       |
 | OUTPUT	: None.                                                   |
 | FLOW 1	: After commiting.                                        |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldCommitFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info

  SQLD_FNC	fnc;			// sqld_fnc class		

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Commit: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Commit: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  if (!sqldCore->isLogonRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.Commit(sqldCore))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FLOW 1 (After commiting) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldCommitFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldConnectFunc		                          |
 | DESCRIPTION	: Establishes a connection to a database.                 |
 | INPUT 1      : SequeLink SQL: SEQUELINK SQL -- The SequeLink SQL       |
 |		  object to use to make a connection.                     |
 | INPUT 2(Opt) : Networking: TEXT -- The network protocol to use to      |
 |                connect.  This parameter is only necessary if the       |
 |                programmer wants to override the network parameter      |
 |                setting in the SQL object.  Acceptable parameters       |
 |		  include: "APPC", "DECnet", "NetBIOS", and "TCP/IP".     |
 | INPUT 3(Opt) : Parameters: TEXT -- The link parameters for the server  |
 |                to connect to.  This parameter is only necessary if the |
 |                programmer wants to override the link parameter         |
 |		  settings in the SQL object.  Link parameters for        |
 |		  specific servers are described in the SequeLink         |
 |		  manuals.                                                |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : After connecting.                                       |
 | FLOW 2       : Failed.                                                 |
 | Novell, March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldConnectFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class			
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  AOBJECTID	protocolObj;		// text object		      [I: 2]
  AMEMBLOCKID	protocolMB;		// protocol memory block
  char		*protocol;		// protocol
  int		protocolType;		// protocol type
  AOBJECTID	linkParamObj;		// text object		      [I: 3]
  AMEMBLOCKID	linkParamMB;		// link parameter memory block	
  char		*linkParam;		// link parameter

  SQLD_FNC	fnc;			// sqld_fnc class
  SqlnkUtl	utl;			// sqlnkutl class
  Slex		slex;			// slex class
  int		tokenSize;		// token size
  char		*token;			// token
  BOOL		isParam[MXPARAMCOUNT];	// is parameter?
  char		*param[MXPARAMCOUNT];	// parameter
  int		connectParamSize;	// connect parameter size		
  char		*connectParam;		// connect parameter

  int		i;			// counter

  if (!sqlnkLib.IsLoadLib())		// no library			
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Connect: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Connect: Couldn't get the SequeLink SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(protocolObj = AFuncGetTypedParameter(2, OTYPE_TEXT)))
    protocolType = sqldCore->protocolType;	// default setting
  else   
  { protocolMB = mb.Allocate();
    OTextGetToMemblock(protocolObj, protocolMB);
    if (!(protocol = (char *) mb.GetPointer(protocolMB)))
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Connect: Couldn't get the Networking input parameter.");
      return;
    } // if

    /* ---- gets protocol type ---- */
    if (!(protocolType = utl.GetProtocolType(protocol)))
    { AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	"Connect: Invalid network protocol name.  Valid protocol names are "
	"APPC, DECnet, NetBIOS, and TCP/IP.");
      return;
    } // if
  } // else

  /* ---- FUNCTION INPUT 3 ---- */
  if (!(linkParamObj = AFuncGetTypedParameter(3, OTYPE_TEXT)))
    linkParam = sqldCore->linkParam;
  else
  { linkParamMB = mb.Allocate();
    OTextGetToMemblock(linkParamObj, linkParamMB);
    if (!(linkParam = (char *) mb.GetPointer(linkParamMB)))
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
        "Connect: Couldn't get Parameters input parameter.");
      return;
    } // if
  } // else

  /* ---- disconnects the current connection ---- */
  if (sqldCore->isServerRef)
    if (!fnc.Disconnect(sqldObj, sqldCore))
    { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if;

  /* ---- gets link parameters ---- */
  slex.InitSlex(linkParam);
  connectParamSize = 1;			// 0 terminated size
  for (i = 0; i < MXPARAMCOUNT; i++)
  { isParam[i] = slex.Colon() == 0? FALSE : TRUE;
    if (isParam[i])
    { token = slex.Text();
      tokenSize = lstrlen(token) + 1;
      param[i] = (char *) mb.Alloc(tokenSize);
      lstrcpy(param[i], token);

      connectParamSize += tokenSize;
    } // if
    else
      break;    
  } // for

  /* ---- makes connection parameter ---- */
  switch (protocolType)
  { case NP_DECNET:
      if (!(isParam[0] && isParam[1] && isParam[2] && isParam[3] &&
	    !isParam[4]))
      { AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	  "Connect: DECnet link parameters syntax is "
	  "':node_name:service:host_logon:host_password'.");
	return;
      } // if
      connectParam = (char *) mb.Alloc(connectParamSize);
      SSPMakeConnectParamDECnet(param[0], param[1], param[2], param[3],
				connectParam);
      break;
    case NP_APPC:
      if (!(isParam[0] && isParam[1] && isParam[2] && isParam[3] &&
	    isParam[4] && isParam[5] && !isParam[6]))
      { AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	  "Connect: APPC link parameters syntax is "
	  "':local_LU:partner_LU:host_TP:host_mode"
	  ":host_username:host_password'.");
	return;
      } // if
      connectParam = (char *) mb.Alloc(connectParamSize);
      SSPMakeConnectParamAPPC(param[0], param[1], param[2], param[3],
			      param[4], param[5], connectParam);
      break;
    case NP_TCPIP:
      if (!(isParam[0] && isParam[1] && isParam[2] && isParam[3] &&
	    !isParam[4]))
      { AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	  "Connect: TCP/IP link parameters syntax is "
	  "':host_name:service:host_logon:host_password'.");
	return;
      } // if
      connectParam = (char *) mb.Alloc(connectParamSize);
      SSPMakeConnectParamTCP(param[0], param[1], param[2], param[3],
                             connectParam);
      break;
    case NP_NETBIOS:
      if (!(isParam[0] && isParam[1] && isParam[2] && isParam[3] &&
	    !isParam[4]))
      { AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	  "Connect: NetBIOS link parameters syntax is "
	  "':node_name:client_name:sqlnk_service:adapter_num"
	  ":host_username:host_password'.");
        return;
      } // if
      connectParam = (char *) mb.Alloc(connectParamSize);
      SSPMakeConnectParamNetBIOS(param[0], param[1], param[2], param[3],
                                 param[4], param[5], connectParam);
      break;
    default:
      AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
        "Connect: Invalid network protocol.");
      return;
  } // switch

  /* ---- connects ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode =
    SQConnect(utl.GetNetworkModuleName(protocolType), connectParam,
	      (SSPName) NULL, &sqldCore->serverRef);
  if (sqldCore->clientCode)
  { sqldCore->reasonCode = sqldCore->serverRef;
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  sqldCore->isServerRef = TRUE;

  /* ---- FLOW 1 (After connecting) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldConnectFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: DescribeTableFunc                                       |
 | DESCRIPTION  : Returns a list of column names and types for a given    |
 |		  database table.                                         |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The object to use to get   |
 |		  table information from the database.                    |
 | INPUT 2      : Table Name: TEXT -- The name of the database table.     |
 | OUTPUT 1(Opt): Column Names: TEXT -- a "\r\n" separated list of the    |
 |                table's column names.                                   |
 | OUTPUT 2(Opt): Column Types: TEXT -- a "\r\n" seperated list of the    |
 |                table's column types.  These will be one of the         |
 |		  following: "Characters", "Long Integer",                |
 |		  "Floating Point", and "Decimal".                        |
 | FLOW 1	: After describing.                                       |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldDescribeTableFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqldk core info
  AOBJECTID	tabNameObj;		// text object		      (I: 2)
  AMEMBLOCKID	tabNameMB;		// table name memory block
  char		*tabName;		// table name
  AOBJECTID	namesObj;		// text object		      [O: 1]
  long		namesSize;		// names size
  char		*names;			// "\r\n" separated names
  char		 *namesPtr;		// names pointer			
  Queue		nameQueue;		// queue class
  int		namePtrLen;		// name pointer length
  char		*namePtr;		// name pointer
  AOBJECTID	typesObj;		// text object		      [O: 2]
  long		typesSize;		// types size
  char		*types;			// column types
  char		 *typesPtr;		// types pointer
  Queue		typeQueue;		// queue class

  char		*stmt;			// statement
  char		*stmtPtr;		// statement pointer
  SSPRefNum	cursorRef;		// cursor reference number
  SSPParse	parseOffset;		// statement's parse offset
  SSPCount	itemCount;		// number of items
  SSPDescList	(*descList)[];		// describe list
  SSPSrvRtn	serverCode;		// server error code

  int		i;			// counter

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Describe Table: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Describe Table: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  tabNameObj = AFuncGetTypedParameter(2, OTYPE_TEXT);
  tabNameMB = mb.Allocate();
  OTextGetToMemblock(tabNameObj, tabNameMB);
  if (!(tabName = (char *) mb.GetPointer(tabNameMB)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Describe Table: Couldn't get the Table Name input parameter.");
    return;
  } // if

  if (!sqldCore->isLogonRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- builds statement ---- */
  stmt = "SELECT * FROM ";
  stmtPtr = (char *) mb.Alloc(lstrlen(stmt) + lstrlen(tabName) + 1);
  lstrcpy(stmtPtr, stmt);
  lstrcat(stmtPtr, tabName);

  /* ---- creates cursor ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode =
    SQOpen(sqldCore->logonRef, NULL, &cursorRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- associates sql statement with the cursor ---- */
  sqldCore->clientCode =
    SQAssociate(cursorRef, stmtPtr, &parseOffset, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { SQClose(cursorRef, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- special case ---- */
  if (sqldCore->dbOverride == DB_SYBASE)
  { sqldCore->clientCode = SQExec(cursorRef, &itemCount,
                                  &sqldCore->serverCode);
    if (sqldCore->clientCode || sqldCore->serverCode)
    { SQClose(cursorRef, &serverCode);
      AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if
  } // if

  /* ---- describes table ---- */
  itemCount = SSP_AllVars;
  descList = NULL;
  sqldCore->clientCode = SQnDescribe(cursorRef, kSSPSelectKind, &itemCount,
				     &descList, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { SQClose(cursorRef, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  descList = (SSPDescList (*) []) mb.Alloc(itemCount * sizeof(SSPDescList));
  sqldCore->clientCode = SQnDescribe(cursorRef, kSSPSelectKind, &itemCount,
				     &descList, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { SQClose(cursorRef, &serverCode);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- calculates name, and type sizes ---- */
  /* ---- row separators ---- */
  if (itemCount > 1)
    namesSize = typesSize = 2 * (itemCount - 1) + 1;
  else
    namesSize = typesSize = 1;		// 0 terminated string

  for (i = 0; i < itemCount; i++)
  { /* ---- table name ---- */
    namePtrLen = lstrlen((*descList)[i].VarName);
    namePtr = (char *) mb.Alloc(namePtrLen + 1);
    lstrcpy(namePtr, (*descList)[i].VarName);
    nameQueue.Enque(namePtr);

    namesSize += namePtrLen;

    /* ---- table type ---- */
    switch ((*descList)[i].SqlnkType)
    { case kSSPChar:
	typeQueue.Enque((void *) kSSPChar);

	typesSize += 10;		// "Characters"
        break;
      case kSSPLong:
        typeQueue.Enque((void *) kSSPLong);
	   
	typesSize += 12;		// "Long Integer"
        break;
      case kSSPFloat:
	typeQueue.Enque((void *) kSSPFloat);

        typesSize += 14;		// "Floating Point"
        break;
      case kSSPDecimal:
        typeQueue.Enque((void *) kSSPDecimal);

        typesSize += 7;			// "Decimal"
        break;
    } // switch
  } // for

  /* ---- writes names and types ---- */
  names = (char *) mb.Alloc(namesSize);
  namesPtr = names;
  types = (char *) mb.Alloc(typesSize);
  typesPtr = types;

  for (i = 0; i < itemCount; i++)
  { /* ---- table name ---- */
    namePtr = (char *) nameQueue.Deque();
    namePtrLen = lstrlen(namePtr);
    CopyMemory(namesPtr, namePtr, namePtrLen);
    namesPtr += namePtrLen;

    /* ---- table type ---- */
    switch ((int) typeQueue.Deque())
    { case kSSPChar:
        CopyMemory(typesPtr, "Characters", 10);
	typesPtr += 10;
	break;
      case kSSPLong:
	CopyMemory(typesPtr, "Long Integer", 12);
	typesPtr += 12;
	break;
      case kSSPFloat:
        CopyMemory(typesPtr, "Floating Point", 14);
	typesPtr += 14;
	break;
      case kSSPDecimal:
	CopyMemory(types, "Decimal", 7);
	typesPtr += 7;
	break;
    } // switch

    /* ---- row separator --- */
    if (i < itemCount - 1)		// "\r\n"			
    { *namesPtr++ = *typesPtr++ = '\r';
      *namesPtr++ = *typesPtr++ = '\n';
    } // if
  } // for
  *namesPtr = *typesPtr = '\0';		// 0 terminated string

  SQClose(cursorRef, &serverCode);

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(namesObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Describe Table: Couldn't create the Names output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(namesObj, names);
  AFuncSetParameter(1, namesObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if (!(typesObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Describe Table: Couldn't create the Types output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(typesObj, types);
  AFuncSetParameter(2, typesObj);

  /* ---- FLOW 1 (After describing) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldDescribeTableFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldDisconnectFunc                                     |
 | DESCRIPTION	: Breaks the connection with a database server and        |
 |		  releases its resources.                                 |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The SequeLink SQL object   |
 |		  to disconnect.                                          |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : After disconnecting.                                    |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldDisconnectFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object
  SqldCore	*sqldCore;		// sqld core info

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Disconnect: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Disconnect: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  if (!sqldCore->isServerRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.Disconnect(sqldObj, sqldCore))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FLOW 1 (After disconnecting) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldDisconnectFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldDisposeQueryFunc                                   |
 | DESCRIPTION	: Releases resources used by an SQL query.                |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The SequeLink SQL object   |
 |		  that owns the query to dispose.                         |
 | INPUT 2	: Query ID: NUMBER -- The ID of the query to dispose.     |
 | OUTPUT	: None.                                                   |
 | FLOW 1	: After disposing.                                        |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldDisposeQueryFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class			
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  SqldCursor	 *cursorPtr;	// cursor pointer
  AOBJECTID	queryIDObj;		// number object	      (I: 2)
  SSPRefNum	queryID;		// query id

  SQLD_FNC	fnc;			// sqld_fnc class

  int		i;			// counter

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Dispose Query: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Dispose Query: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Dispose Query: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  { MemBlock	mb;			// memblock class		

    if ((cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) /
		       sizeof(SqldCursor)) != 0)
      if (!(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
      { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
          "Dispose Query: Couldn't get the SQL Object input parameter.");
        return;
      } // if

    for (cursorPtr = cursor, i = 0; i < cursorCount; i++, cursorPtr++)
      if (cursorPtr->cursorRef == queryID)	// found
      { if (!fnc.Close(sqldCore, cursorPtr->cursorRef))
	{ AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

	  /* ---- FLOW 2 (Failed) ---- */
  	  AFuncReportResult(theSystem, 2, "");
	  return;
	} // if

	/* ---- releases resources ---- */
        fnc.Dispose(cursorPtr);

        /* ---- removes the cursor ---- */
        CopyMemory(cursorPtr, cursorPtr + 1,
		(cursorCount - i - 1) * sizeof(SqldCursor));
        break; // for 
      } // if

    if (i == cursorCount)
    { /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "SequeLink SQL\r\n"
	"Dispose Query: Invalid Query ID.");
      return;
    } // if
  } // scope level-1

  AObjResizeData(sqldObj, SQLDCURSORS, (cursorCount - 1) *
                 sizeof(SqldCursor));

  /* ---- FLOW 1 (After disposing) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldDisposeQueryFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: ExecuteQueryFunc                                        |
 | DESCRIPTION  : Execute a SQL query.                                    |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The SequeLink SQL object   |
 |		  that owns the query to execute.                         |
 | INPUT 2      : Query ID: NUMBER -- The ID of the query to execute.     |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : After executing.                                        |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldExecuteQueryFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  AOBJECTID	queryIDObj;		// number object	      (I: 2)
  SSPRefNum	queryID;		// query id

  SQLD_FNC	fnc;			// sqld_fnc class
  SSPCount	rowCount;		// number of rows

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Execute Query: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Execute Query: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount &&
      !(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Execute Query: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Execute Query: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  if ((cursor = fnc.GetCursor(cursorCount, cursor, queryID)) == NULL)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.BindObjects(sqldCore, cursor))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return; 
  } // if

  sqldCore->clientCode = SQExec(queryID, &rowCount, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FLOW 1 (After executing) ---- */
  AFuncReportResult(theSystem, 1, "");
} // ExecuteQueryFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldFetchFunc                                          |
 | DESCRIPTION	: Retrieves data from an SQL query.  Place results into   |
 |		  the objects defined in the script.                      |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The object that owns the   |   
 |		  query to fetch data from.                               |
 | INPUT 2      : NUMBER: Query ID -- The ID of the query to fetch data   |
 |		  from.                                                   |
 | INPUT 3(Opt)	: Row Count: NUMBER -- The number of rows to fetch.  This |
 |                parameter is only necessary if the programmer wants to  |
 |                override the row count setting in the SQL object.       |
 | OUTPUT 1(Opt): Count: NUMBER -- The number of rows were actually       |
 |		  fetched from the database.                              |
 | FLOW 1       : After fetching.                                         |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldFetchFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  AOBJECTID	queryIDObj;		// number object	      (I: 2)
  SSPRefNum	queryID;		// query id
  AOBJECTID	rowCountObj;		// number object	      (I: 3)
  SSPCount	rowCount;		// number of rows
  AOBJECTID	countObj;		// number object	      (O: 1)

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Fetch: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount &&
      !(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch: Couldn't get the SequeLink SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  /* ---- FUNCTION INPUT 3 ---- */
  if (!(rowCountObj = AFuncGetTypedParameter(3, OTYPE_NUMBER)))
    rowCount = sqldCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount < 0)
    rowCount = -rowCount;

  if ((cursor = fnc.GetCursor(cursorCount, cursor, queryID)) == NULL)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- fetchs rows ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQFetch(cursor->cursorRef, &rowCount,
                                 &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.FetchToObjects(sqldCore, *cursor, rowCount))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(countObj = AObjCreate(OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch: Couldn't create the Count output parameter.");
    return;
  } // if
  ONmbrSetInteger(countObj, rowCount, TRUE);
  AFuncSetParameter(1, countObj);

  /* ---- FLOW 1 (After fetching) ---- */
  AFuncReportResult(theSystem, 1, "");
} // FetchFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldFetchToTableFunc                                   |
 | DESCRIPTION	: Retrieves data from a query into a Table object.        |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The object that owns the   |
 |                query to fetch data from.                               |
 | INPUT 2      : Query ID: NUMBER -- The ID of the query to fetch data   |
 |                from.                                                   |
 | INPUT 3      : Table: TABLE -- The results of the query formatted into |
 |                a Table object.                                         |
 | INPUT 4(Opt) : Row Count: NUMBER -- The number of rows to fetch.  This |
 |                parameter is only necessary if the programmer wants to  |
 |                override the row count setting in the SQL object.  A    |
 |                row count of zero signifies that the object should      |
 |                fetch as many rows as memory will allow.                |
 | OUTPUT 1(Opt): Count: NUMBER -- The number of actual rows fetch.       |
 | FLOW 1       : After fetching.                                         |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |                 
\*------------------------------------------------------------------------*/
void ALMAPI OSqldFetchToTableFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  AOBJECTID	queryIDObj;     	// number object	      (I: 2)
  SSPRefNum	queryID;		// query id
  AOBJECTID	tableObj;		// table object		      (I: 3)
  AOBJECTID	rowCountObj;		// number object	      (I: 4)
  SSPCount	rowCount;		// number of rows
  AOBJECTID	countObj;		// number object	      (O: 1)

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Fetch To Table: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Table: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount &&
      !(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Table: Couldn't get the SequeLink SQL input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Table: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  /* ---- FUNCTION INPUT 3 ---- */
  if (!(tableObj = AFuncGetTypedParameter(3, OTYPE_TABLE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Table: Couldn't get the Table input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 4 ---- */
  if (!(rowCountObj = AFuncGetTypedParameter(4, OTYPE_NUMBER)))
    rowCount = sqldCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount < 0)
    rowCount = -rowCount;

  if ((cursor = fnc.GetCursor(cursorCount, cursor, queryID)) == NULL)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- fetchs rows ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQFetch(cursor->cursorRef, &rowCount,
                                 &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.FetchToTable(sqldCore, *cursor, rowCount, tableObj))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(countObj = AObjCreate(OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Table: Couldn't create the Count output parameter.");
    return;
  } // if
  ONmbrSetInteger(countObj, rowCount, TRUE);
  AFuncSetParameter(1, countObj);

  /* ---- FLOW 1 (After fetching) ---- */
  AFuncReportResult(theSystem, 1, "");
} // FetchToTableFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldFetchToTextFunc                                    |
 | DESCRIPTION	: Retrieves data from a query into a Text object.         |
 |                Seperators are user-defineable.                         |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The object that owns the   |
 |                query to fetch data from.                               |
 | INPUT 2      : Query ID: NUMBER -- The ID of the query to fetch data   |
 |                from.                                                   |
 | INPUT 3(Opt) : Row Count: NUMBER -- The number of rows to fetch.  This |
 |                parameter is only necessary if the programmer wants to  |
 |                override the row count setting in the SQL object.  A    |
 |                row count of zero signifies that the object should      |
 |                fetch as many rows as memory will allow.                |           
 | OUTPUT 1     : Text: TEXT -- The results of the query concatenated     |
 |                into a Text object.                                     |
 | OUTPUT 2(Opt): Count: NUMBER -- The number of actual rows fetch.       |
 | FLOW 1       : After fetching.                                         |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |                      
\*------------------------------------------------------------------------*/
void ALMAPI OSqldFetchToTextFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors
  AOBJECTID	queryIDObj;     	// number object	      (I: 2)
  SSPRefNum	queryID;		// query id
  AOBJECTID	rowCountObj;		// number object	      (I: 3)
  SSPCount	rowCount;		// number of rows
  AOBJECTID	textObj;		// text object		      (O: 1)
  AOBJECTID	countObj;		// number object	      (O: 2)

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Fetch To Text: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Text: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount &&
      !(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Text: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Text: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  /* ---- FUNCTION INPUT 3 ---- */
  if (!(rowCountObj = AFuncGetTypedParameter(3, OTYPE_NUMBER)))
    rowCount = sqldCore->rowCount;	// default setting
  else
    rowCount = ONmbrGetInteger(rowCountObj);
  if (rowCount < 0)
    rowCount = -rowCount;

  if ((cursor = fnc.GetCursor(cursorCount, cursor, queryID)) == NULL)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- fetchs rows ---- */
  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQFetch(cursor->cursorRef, &rowCount,
                                 &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(textObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Text: Couldn't create the Count output parameter.");
    return;
  } // if
  if (!fnc.FetchToText(sqldCore, *cursor, rowCount, textObj))
  { AObjDestroy(textObj);
    AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  AFuncSetParameter(1, textObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if (!(countObj = AObjCreate(OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Fetch To Text: Couldn't create the Count output parameter.");
    return;
  } // if
  ONmbrSetInteger(countObj, rowCount, TRUE);
  AFuncSetParameter(2, countObj);

  /* ---- FLOW 1 (After fetching) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldFetchToTextFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldGetErrorFunc                                       |
 | DESCRIPTION	: Returns the most recent error code, and the associated  |
 |		  error message.                                          |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The object to retrieve the |
 |		  the error from.                                         |
 | OUTPUT 1(OPT): Error Number: NUMBER -- The returned error code.        |
 | OUTPUT 2(OPT): Error Message: TEXT -- The returned error message.      |
 | FLOW 1	: After getting.                                          |  
 | FLOW 2	: No error.                                               |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldGetErrorFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  AOBJECTID	errorNumberObj;		// number object	      [O: 1]
  AOBJECTID	errorMessageObj;	// text object		      [O: 2]
  char		errorMessage[2 * MXSTRSIZE];
  					// error message
  char		str[MXSTRSIZE];		// string
  SSPSrvRtn	serverCode;		// server error code

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Get Error: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Error: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  if (!(sqldCore->clientCode || sqldCore->serverCode ||
	sqldCore->reasonCode))
  { /* ---- FLOW 2 (No error) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- client error ---- */
  lstrcpy(errorMessage,           "SequeLink Error: ");
  ltoa(sqldCore->clientCode, str, 10);
  lstrcat(errorMessage, str);

  /* ---- server error ---- */
  if (sqldCore->serverCode)
  { lstrcat(errorMessage,     "\r\nServer Error   : ");
    ltoa(sqldCore->serverCode, str, 10);
    lstrcat(errorMessage, str);
    if (sqldCore->isServerRef)
      if (SQErrorMessage(sqldCore->serverRef, sqldCore->serverCode, str,
		         &serverCode) == SSP_OK)
      { /* ---- server error message ---- */
        
        lstrcat(errorMessage, "\r\nServer Message : ");
        lstrcat(errorMessage, str);
      } // if
  } // if  		     

  /* ---- reason code ---- */
  if (sqldCore->reasonCode)
  { lstrcat(errorMessage,     "\r\nReason Code    : ");
    ltoa(sqldCore->reasonCode, str, 10);
    lstrcat(errorMessage, str);
  } // if

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(errorNumberObj = AObjCreate(OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Error: Couldn't create the Error Number output parameter.");
    return;
  } // if
  ONmbrSetInteger(errorNumberObj, sqldCore->clientCode, TRUE);
  AFuncSetParameter(1, errorNumberObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if (!(errorMessageObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Error: Couldn't create the Error Message output parameter.");
    return;
  } // if
  OTextSetValueFromBuffer(errorMessageObj, errorMessage);
  AFuncSetParameter(2, errorMessageObj);

  /* ---- FLOW 1 (After getting) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldGetErrorFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OSqldGetStatusFunc                                      |
 | DESCRIPTION  : Returns the status of the SQL connection.               |
 | INPUT 1      : SequeLink SQL: SEQUELINK SQL -- The SequeLink SQL       |
 |		  object to get the status from.                          |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : Idle.                                                   |
 | FLOW 2       : Connected.                                              |
 | FLOW 3       : Logged on.                                              |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldGetStatusFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Status: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FLOW 1 (IDLE), FLOW 2 (CONNECTED), FLOW 3 (LOGGED ON) ---- */
  AFuncReportResult(theSystem, 1 + (sqldCore->isServerRef == TRUE) +
		    (sqldCore->isLogonRef == TRUE), "");
} // OSqldGetStatusFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : OSqldGetVariablesFunc                                   |
 | DESCRIPTION  : Returns a list of variable names, and the associated    |
 |		  variable types from a given SequeLink SQL object.       |
 | INPUT 1      : SequeLink SQL: SEQUELINK SQL -- The SequeLink SQL       |
 |		  object used to get the variables from.                  |
 | OUTPUT 1(OPT): Names: TEXT -- A "\r\n" seperated list of variable      |
 |		  names.                                                  |
 | OUTPUT 2(OPT): Types: TEXT -- A "\r\n" seperated list of variable      |
 |		  types.  Variable types are "Characters",                |
 |		  "Long Integer", "Floating Point", and "Decimal".        |
 | FLOW 1       : After getting.                                          |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldGetVariablesFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  int		varCount;		// number of variables
  Var		*var;			// an array of variables
  Var		*varPtr;		// variable pointer
  AOBJECTID	namesObj;		// text object		      [O: 1]
  long		namesSize;		// names size
  char		*names;			// "\r\n" separated names
  char		*namesPtr;		// names pointer
  AOBJECTID	typesObj;		// text object		      [O: 2]
  long		typesSize;		// types size
  char		*types;			// "\r\n" separated types
  char		*typesPtr;		// types pointer

  int		i;			// counter

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!sqldObj)
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Variables: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  varCount = AObjGetDataSize(sqldObj, SQLDVARS) / sizeof(Var);
  if (varCount && !(var = (Var *) mb.GetPointer(sqldObj, SQLDVARS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Variables: Couldn't get SQL Object input parameter.");
    return;
  } // if

  /* ---- calculates names, and types sizes ---- */
  /* ---- row separators ---- */
  if (varCount > 1)
    namesSize = typesSize = 2 * (varCount - 1) + 1;
  else
    namesSize = typesSize = 1;		// 0 terminated string

  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { /* ---- column name ---- */
    namesSize += lstrlen(varPtr->name);

    /* ---- column type ---- */
    switch (varPtr->type)
    { case kSSPChar:
        typesSize += 10;		// "Characters"
        break;
      case kSSPLong:
        typesSize += 12;		// "Long Integer"
	break;
      case kSSPFloat:
	typesSize += 14;		// "Floating Point"
        break;
      case kSSPDecimal:
        typesSize += 7;			// "Decimal"
        break;
    } // switch
  } // for

  /* ---- writes names, and types ---- */
  names = (char *) mb.Alloc(namesSize);
  namesPtr = names;
  types = (char *) mb.Alloc(typesSize);
  typesPtr = types;

  for (varPtr = var, i = 0; i < varCount; i++, varPtr++)
  { /* ---- column name ---- */
    namesSize = lstrlen(varPtr->name);
    CopyMemory(namesPtr, varPtr->name, namesSize);
    namesPtr += namesSize;

    /* ---- column type ---- */
    switch (varPtr->type)
    { case kSSPChar:
	CopyMemory(typesPtr, "Characters", 10);
	typesPtr += 10;			// "Characters"
	break;
      case kSSPLong:
	CopyMemory(typesPtr, "Long Integer", 12);
	typesPtr += 12;			// "Long Integer"
	break;
      case kSSPFloat:
	CopyMemory(typesPtr, "Floating Point", 14);
	typesPtr += 14;			// "Floating Point"
	break;
      case kSSPDecimal:
	CopyMemory(typesPtr, "Decimal", 7);
	typesPtr += 7;			// "Decimal"
	break;
    } // switch

    /* ---- row separator --- */
    if (i < varCount - 1)		// "\r\n"			
    { *namesPtr++ = *typesPtr++ = '\r';
      *namesPtr++ = *typesPtr++ = '\n';
    } // if
  } // for
  *namesPtr = *typesPtr = '\0';		// 0 terminated string

  /* ---- FUNCTION OUTPUT 1 ---- */
  if (!(namesObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Variables: Couldn't create the Names output parameter.");
    return;    
  } // if
  OTextSetValueFromBuffer(namesObj, names); 
  AFuncSetParameter(1, namesObj);

  /* ---- FUNCTION OUTPUT 2 ---- */
  if (!(typesObj = AObjCreate(OTYPE_TEXT)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Get Variables: Couldn't create the Types output parameter.");
    return;    
  } // if
  OTextSetValueFromBuffer(typesObj, types); 
  AFuncSetParameter(2, typesObj);

  /* ---- FLOW 1 (After Getting) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldGetVariablesFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldLogoffFunc                                         |
 | DESCRIPTION  : Logoffs from a database.                                |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The SequeLink SQL object   |
 |		  to logoff.                                              |
 | OUTPUT       : None.                                                   | 
 | FLOW 1       : After logoff.                                           |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldLogoffFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Log Off: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Log Off: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  if (!sqldCore->isLogonRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.Logoff(sqldObj, sqldCore))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FLOW 1 (After logoff) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldLogoffFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldLogonFunc                                          |
 | DESCRIPTION  : Logons to a database.                                   |
 | INPUT 1      : SQL Object: SEQUELINK SQL -- The object to use to       | 
 |                logon.                                                  |
 | INPUT 2(Opt) : Database: TEXT -- The database type to logon to.  This  |
 |		  parameter is only necessary if the programmer wants to  |
 |                override the database parameter setting in the SQL      |
 |		  object.  Acceptable parameters include: "AS/400",       |
 |		  "DB2", "Informix", "Ingres", "Interbase", "Oracle",     |
 |		  "OS/2 DBM", "RDB", "Sybase".                            |
 | INPUT 3(Opt)	: 1st Parameter: TEXT -- The database 1st logon           |
 |		  parameter.  This parameter is only necessary if the     |
 |		  programmer wants to override the first logon parameter  |
 |		  in the SQL object.  Parameter for specific databases    |
 |		  are described in the SequeLink manuals.                 |
 | INPUT 4(Opt) : 2nd Parameter: TEXT -- The database 2nd logon           |
 |		  parameter.  (see the description of 1st Parameter.)     |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : After logon.                                            |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldLogonFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  AOBJECTID	databaseObj;		// text object		      [I: 2]
  AMEMBLOCKID	databaseMB;		// database memory block
  char		*database;		// database name
  AOBJECTID	param1Obj;		// text object		      [I: 3]
  AMEMBLOCKID	param1MB;		// 1st logon parameter memory block
  char		*param1;		// 1st logon parameter
  AOBJECTID	param2Obj;		// text object		      [I: 4]
  AMEMBLOCKID	param2MB;		// 2nd logon parameter memory block
  char		*param2;		// 2nd logon parameter

  SQLD_FNC	fnc;			// sqld_fnc class
  SqlnkUtl	utl;			// sqlnkutl class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Log On: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Log On: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(databaseObj = AFuncGetTypedParameter(2, OTYPE_TEXT)))
    /* Do Nothing */;
  else
  { databaseMB = mb.Allocate();
    OTextGetToMemblock(databaseObj, databaseMB);
    database = (char *) mb.GetPointer(databaseMB);
    if (!(sqldCore->dbOverride = utl.GetDatabaseType(database)))
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Log On: Invalid database name.  Valid database names are "
	"AS/400, DB2, Informix, Ingres, Interbase, Oracle, OS/2 DBM, RDB, "
	"and Sybase." );
      return;
    } // if
  } // else

  /* ---- FUNCTION INPUT 3 ---- */
  if (!(param1Obj = AFuncGetTypedParameter(3, OTYPE_TEXT)))
    param1 = sqldCore->logon1;
  else
  { param1MB = mb.Allocate();
    OTextGetToMemblock(param1Obj, param1MB);
    if (!(param1 = (char *) mb.GetPointer(param1MB)))
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Log On: Couldn't get the 1st Parameter input parameter.");
      return;
    } // if
  } // else

  /* ---- FUNCTION INPUT 4 -*/
  if (!(param2Obj = AFuncGetTypedParameter(4, OTYPE_TEXT)))
    param2 = sqldCore->logon2;
  else
  { param2MB = mb.Allocate();
    OTextGetToMemblock(param2Obj, param2MB);
    if (!(param2 = (char *) mb.GetPointer(param2MB)))
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Log On: Couldn't get the 2nd Parameter input parameter.");
      return;
    } // if  
  } // else

  if (!sqldCore->isServerRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (sqldCore->isLogonRef)
    if (!fnc.Logoff(sqldObj, sqldCore))
    { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

      /* ---- FLOW 2 (Failed) ---- */
      AFuncReportResult(theSystem, 2, "");
      return;
    } // if

  sqldCore->reasonCode = 0;
  sqldCore->serverCode = 0;
  sqldCore->clientCode = SQLogon(sqldCore->serverRef, param1, param2,
    (SSPName) NULL, &sqldCore->logonRef, &sqldCore->serverCode);
  if (sqldCore->clientCode || sqldCore->serverCode)
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if
  sqldCore->isLogonRef = TRUE;

  /* ---- FLOW 1 (After logon) ---- */
  AFuncReportResult(theSystem, 1, "");
} // LogonFunc

/*------------------------------------------------------------------------*\
 | FUNCTION     : RollbackFunc                                            |
 | DESCRIPTION  : Rollbacks changes made to the database.                 |
 | INPUT 1      : SequeLink SQL: SEQUELINK SQL -- The SequeLink SQL       |
 |		  object to rollback the changes for.                     |
 | OUTPUT       : None.                                                   |
 | FLOW 1       : After rollback.                                         |
 | FLOW 2       : Failed.                                                 |
 | Novell, Inc., Marach 4, 1994.				       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldRollbackFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class		
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Rollback: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Rollback: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  if (!sqldCore->isLogonRef)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.Rollback(sqldCore))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- FLOW 1 (After rollback) ---- */
  AFuncReportResult(theSystem, 1, "");
} // RollbackFunc

/*------------------------------------------------------------------------*\
 | FUNCTION	: OSqldStopFetchFunc                                      |
 | DESCRIPTION	: Releases resources allocated for a fetch operation.     |
 | INPUT 1	: SQL Object: SEQUELINK SQL -- The object that owns the   |
 |		  query for a stop fetch from.                            |
 | INPUT 2	: Query ID: NUMBER -- The ID of the query to stop         |
 |		  fetching from.                                          |
 | OUTPUT	: None.                                                   |
 | FLOW 1	: After stopping.                                         |
 | FLOW 2	: Failed.                                                 |
 | Novell, Inc., March 4, 1994.					       mw |
\*------------------------------------------------------------------------*/
void ALMAPI OSqldStopFetchFunc
( pAEvtInfo,				// event pointer
  pAObjMessage	theSystem		// system pointer
)
{ MemBlock	mb;			// memblock class
  AOBJECTID	sqldObj;		// sqld object		      (I: 1)
  SqldCore	*sqldCore;		// sqld core info
  int		cursorCount;		// number of cursors
  SqldCursor	*cursor;		// an array of cursors	
  AOBJECTID	queryIDObj;		// number object	      (I: 2)
  SSPRefNum	queryID;		// cursor reference number

  SQLD_FNC	fnc;			// sqld_fnc class

  if (!sqlnkLib.IsLoadLib())		// no library
    if (!sqlnkLib.LoadLib())
    { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
	"Stop Fetch: Couldn't find the SequeLink DLL.");
      return;
    } // if

  /* ---- FUNCTION INPUT 1 ---- */
  sqldObj = AFuncGetTypedParameter(1, OTYPE_SQLD);
  if (!(sqldCore = (SqldCore *) mb.GetPointer(sqldObj, SQLDCORE)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Stop Fetch: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  cursorCount = AObjGetDataSize(sqldObj, SQLDCURSORS) / sizeof(SqldCursor);
  if (cursorCount &&
      !(cursor = (SqldCursor *) mb.GetPointer(sqldObj, SQLDCURSORS)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Stop Fetch: Couldn't get the SQL Object input parameter.");
    return;
  } // if

  /* ---- FUNCTION INPUT 2 ---- */
  if (!(queryIDObj = AFuncGetTypedParameter(2, OTYPE_NUMBER)))
  { AFuncReportResult(theSystem, 0, "SequeLink SQL\r\n"
      "Stop Fetch: Couldn't get the Query ID input parameter.");
    return;
  } // if
  queryID = ONmbrGetInteger(queryIDObj);

  if ((cursor = fnc.GetCursor(cursorCount, cursor, queryID)) == NULL)
  { /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  if (!fnc.Cancel(sqldCore, queryID))
  { AEvtPostSignalAtTail(sqldObj, SQLDIFERROR);

    /* ---- FLOW 2 (Failed) ---- */
    AFuncReportResult(theSystem, 2, "");
    return;
  } // if

  /* ---- release resources ---- */
  fnc.Dispose(cursor);

  /* ---- FLOW 1 (After stopping) ---- */
  AFuncReportResult(theSystem, 1, "");
} // OSqldStopFetchFunc
