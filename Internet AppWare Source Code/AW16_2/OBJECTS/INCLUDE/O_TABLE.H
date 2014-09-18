/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_TABLE_H   /* prevent multiple includes */
#define O_TABLE_H

/* ***********************************************************************
 * TYPEID for Table Object
 * *********************************************************************** */

#define OTYPE_TABLE                   MAKETYPEID('T','a','b','l')

/* ***********************************************************************
 * EVENTIDs for Table Object
 * *********************************************************************** */

#define autoTableGetFieldObjects      MAKEEVENTID('a', 't', 'f', 'O')
#define autoTableGetIndexableFields   MAKEEVENTID('a', 't', 'i', 'F')
#define autoTableReadRecords          MAKEEVENTID('a', 't', 'r', 'R')
#define autoTableRecordCount          MAKEEVENTID('a', 't', 'r', 'C')

#define SERIOUSERROR                  (-1L)
#define UNEXPECTEDERROR               (-2L)

/* ***********************************************************************
 * Constants for Table Object
 * *********************************************************************** */

/* SEEK_xxxx constants */
#define SEEK_CUR                      1
#define SEEK_END                      2
#define SEEK_SET                      0

/* ***********************************************************************
 * Structure for event communication with Table Object
 * *********************************************************************** */

/* TablREADRECORDS */
typedef struct
  {
  MEMBLOCKID  hFields;    /* the field objects requested                  */
  OBJECTID    idxObj;     /* objectID for the indexed field               */
  long        relIdx;     /* relative index for reading                   */
  int         readType;   /* SEEK_SET, SEEK_CUR, or SEEK_END              */
  long        recCount;   /* number of records requested                  */
  MEMBLOCKID  hData;      /* memory block for '\t' an '\0' delimited text */
  MEMBLOCKID  hRecIDs;    /* memory block for the record IDs              */
  long        retCode;    /* # of rows copied or error code               */
  long        currRecIdx; /* current record index                         */
  } TablREADRECORDS;

/* ***********************************************************************
 * Table Object APIs
 * *********************************************************************** */

/* GENERAL NOTE:
 *  BOOL  return values:
 *    TRUE means the function was successful.
 *    FALSE means an error occurred (e.g. the row or column index
 *          is out of range).
 *
 *  INDEX inputs:
 *    ALL INDICIES are zero based (i.e. the first row is row 0 and
 *          the first column is column 0). 
 */


#ifdef __cplusplus
extern "C"            /* Assume C declarations for C++ */
  {
#endif  /* __cplusplus */

  /*  OTableAddRow will add a row(s) to the specified table.  The rowText
   *  is a tab and return delimited text for the new row.  The tabs break
   *  the columns and the return denotes the row break.
   */
  BOOL ALMAPI OTableAddRow
    (
    OBJECTID  tableObj,
    LPSTR     rowText,
    long      startRow
    );


  /*  OTableColumnTitle will the the title of the specified column.  If the
   *  colIndex equals the number of columns a new column with the
   *  specified title is added.
   */
  BOOL ALMAPI OTableColumnTitle
    (
    OBJECTID  tableObj,
    LPSTR     colName,
    int       colIndex
    );


  /*  OTableGetCell will get the specified cell text from the specified table
   *  object.  The text will be returned in the cellBlock MEMBLOCKID.
   */
  BOOL ALMAPI OTableGetCell
    (
    OBJECTID    tableObj,
    MEMBLOCKID  cellBlock,
    int         colIndex,
    long        rowIndex
    );


  /*  OTableGetColumn will get the specified column from the table object.
   *  the text will be returned in the colBlock MEMBLOCKID.  The text will
   *  be tab delimited.
   */
  BOOL ALMAPI OTableGetColumn
    (
    OBJECTID    tableObj,
    MEMBLOCKID  colBlock,
    int         colIndex
    );


  /*  OTableGetColumnCount will return the number of columns in the specified
   *  table object.
   */
  int  ALMAPI OTableGetColumnCount
    (
    OBJECTID  tableObj
    );


  /*  OTableGetRow will get the specified row from the table object.  The
   *  text will be returned in the rowBlock MEMBLOCKID.  Mulitple rows can
   *  be retrieved by having numRows greater than 1.  The text is tab and
   *  return delimited.
   */
  BOOL ALMAPI OTableGetRow
    (
    OBJECTID    tableObj,
    MEMBLOCKID  rowBlock,
    long        startRow,
    int         numRows
    );


  /*  OTableGetRowCount will return the number of rows in the specified table
   *  object.
   */
  long ALMAPI OTableGetRowCount
    (
    OBJECTID  tableObj
    );


  /*  OTableSetCell will set the specified cell to the cellText.
   */
  BOOL ALMAPI OTableSetCell
    (
    OBJECTID  tableObj,
    LPSTR     cellText,
    int       colIndex,
    long      rowIndex
    );


  /*  OTableSetColumn will set the specified column.  The text in colText is
   *  tab-delimited.
   */
  BOOL ALMAPI OTableSetColumn
    (
    OBJECTID    tableObj,
    char huge*  colText,
    int         colIndex
    );


  /*  OTableSetColumnCount will set the number of columns to be colCount.
   *  This function will add or delete columns.  If columns are added the
   *  attributes from the last column will be copied to the new column.
   */
  BOOL ALMAPI OTableSetColumnCount
    (
    OBJECTID  tableObj,
    int       colCount
    );


  /* OTableSetColumnWidth will set the width of a column.  If doAll is set TRUE,
   * width of all columns will be set.
   * width : in Pixel
   */
  BOOL ALMAPI OTableSetColumnWidth
    (
    OBJECTID  tableObj,
    BOOL      doAll,        /* All columns */
    int       colIndex,     /* specific col */
    int       width
    );

  /* OTableMakeColumnEditable will set  a column of a manual table
   * editable if bCanEdit is TRUE.  Else it makes it uneditable.  
   *
   */
  BOOL ALMAPI OTableSetColEditAttrib
    (
    OBJECTID  tableObj,
    int       colIndex,       /* specific col */
    BOOL      bCanEdit
    );


  /*  OTableSetRow will overwrite the specified row with rowText.  Multiple
   *  rows will be set (overwritten) if there are returns in the rowText
   *  parameter.  The fields are tab delimted.  The lines are return
   *  delimted. 
   */
  BOOL ALMAPI OTableSetRow
    (
    OBJECTID  tableObj,
    LPSTR     rowText,
    long      startRow
    );


  /*  OTableSetRowCount will set the number of rows to rowCount.  This will
   *  delete excess rows or add new rows depending on the number of
   *  existing rows.
   */
  BOOL ALMAPI OTableSetRowCount
    (
    OBJECTID  tableObj,
    long      rowCount
    );


  /*  OTableFindCell will return the cell row and column if the point is
   *  in the table.  This returns TRUE if the point is in a cell of the
   *  table and FALSE if the point is not in the table.  If oiTable is NULL,
   *  the last visual instance used is used.  If you make one call with the
   *  oiTable field set, and then call the rest of the times with oiTable
   *  == NULL, this will work a little faster.
   */
  BOOL ALMAPI OTableFindCell
    (
    OBJECTID  oiTable,    /* Table object */
    LPPOINT   lppt,       /* Point (in screen coordinates) to check */
    LPLONG    plRow,      /* Row the point is in (can be NULL) */
    LPINT     pnCol       /* Column the point is in (can be NULL) */
    );

#ifdef __cplusplus
  }
#endif  /* __cplusplus */

/*  Old API names for backward compatibility
 */
#define tblAddRow               OTableAddRow
#define tblColumnTitle          OTableColumnTitle
#define tblGetCell              OTableGetCell
#define tblGetColumn            OTableGetColumn
#define tblGetColumnCount       OTableGetColumnCount
#define tblGetRow               OTableGetRow
#define tblGetRowCount          OTableGetRowCount
#define tblSetCell              OTableSetCell
#define tblSetColumn            OTableSetColumn
#define tblSetColumnCount       OTableSetColumnCount
#define tblSetColumnWidth       OTableSetColumnWidth
#define tblSetColEditAttrib     OTableSetColEditAttrib
#define tblSetRow               OTableSetRow
#define tblSetRowCount          OTableSetRowCount
#define tblFindCell             OTableFindCell

/* *********************************************************************** */

#endif  /* #ifndef O_TABLE_H */
