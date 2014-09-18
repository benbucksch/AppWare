/*//////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
//  "tabl_obj.h"
//
// Header file for other objects to interact with the table object
//
//////////////////////////////////////////////////////////////*/


#ifndef __TABL_OBJ_H	// prevent multiple includes
#define	__TABL_OBJ_H

#ifndef __OIP_H
// #include	<oip.h>
#endif

// SEEK_xxxx constants /////////////////////////////////////////////////////

#define SEEK_CUR    									1
#define SEEK_END   										2
#define SEEK_SET   										0

// event constants /////////////////////////////////////////////////////////

#define	autoTableGetFieldObjects			MAKEID('a', 't', 'f', 'O')
#define	autoTableGetIndexableFields		MAKEID('a', 't', 'i', 'F')
#define	autoTableReadRecords					MAKEID('a', 't', 'r', 'R')
#define	autoTableRecordCount					MAKEID('a', 't', 'r', 'C')

#define	SERIOUSERROR									(-1L)
#define	UNEXPECTEDERROR								(-2L)

// TablREADRECORDS /////////////////////////////////////////////////////////

typedef struct
	{
	MEMBLOCKID	hFields;		// the field objects requested
	OBJECTID		idxObj;			// objectID for the indexed field
	long				relIdx;			// relative index for reading
	int					readType;		// SEEK_SET, SEEK_CUR, or SEEK_END
	long				recCount;		// number of records requested
	MEMBLOCKID	hData;			// memory block for '\t' an '\0' delimited text
	MEMBLOCKID	hRecIDs;		// memory block for the record IDs
	long				retCode;		// # of rows copied or error code
	long				currRecIdx;	// current record index (for Serius database object)
	} TablREADRECORDS;

// function prototypes /////////////////////////////////////////////////////


/* GENERAL NOTE:
 *	BOOL	return values:
 *		TRUE means the function was successful.
 *		FALSE means an error occurred (e.g. the row or column index is out of
 *					range).
 *
 *	INDEX inputs:
 *		ALL INDICIES are zero based (i.e. the first row is row 0 and the first
 *					column is column 0). 
 */


#ifdef __cplusplus
extern "C"            /* Assume C declarations for C++ */
	{
#endif  /* __cplusplus */

	/*	tblAddRow will add a row(s) to the specified table.  The rowText
	 *	is a tab and return delimited text for the new row.  The tabs break
	 *	the columns and the return denotes the row break.
	 */
	BOOL far pascal _export tblAddRow(
		OBJECTID	tableObj,
		LPSTR			rowText,
		long			startRow);


	/*	tblColumnTitle will the the title of the specified column.  If the
	 *	colIndex equals the number of columns a new column with the
	 *	specified title is added.
	 */
	BOOL far pascal _export tblColumnTitle(
		OBJECTID	tableObj,
		LPSTR			colName,
		int				colIndex);


	/*	tblGetCell will get the specified cell text from the specified table
	 *	object.  The text will be returned in the cellBlock MEMBLOCKID.
	 */
	BOOL far pascal _export tblGetCell(
		OBJECTID		tableObj,
		MEMBLOCKID	cellBlock,
		int					colIndex,
		long				rowIndex);


	/*	tblGetColumn will get the specified column from the table object.
	 *	the text will be returned in the colBlock MEMBLOCKID.  The text will
	 *	be tab delimited.
	 */
	BOOL far pascal _export tblGetColumn(
		OBJECTID		tableObj,
		MEMBLOCKID	colBlock,
		int					colIndex);


	/*	tblGetColumnCount will return the number of columns in the specified
	 *	table object.
	 */
	int  far pascal _export tblGetColumnCount(
		OBJECTID	tableObj);


	/*	tblGetRow will get the specified row from the table object.  The
	 *	text will be returned in the rowBlock MEMBLOCKID.  Mulitple rows can
	 *	be retrieved by having numRows greater than 1.  The text is tab and
	 *	return delimited.
	 */
	BOOL far pascal _export tblGetRow(
		OBJECTID		tableObj,
		MEMBLOCKID	rowBlock,
		long				startRow,
		int					numRows);


	/*	tblGetRowCount will return the number of rows in the specified table
   *	object.
	 */
	long far pascal _export tblGetRowCount(
		OBJECTID	tableObj);


	/*	tblSetCell will set the specified cell to the cellText.
	 */
	BOOL far pascal _export tblSetCell(
		OBJECTID	tableObj,
		LPSTR			cellText,
		int				colIndex,
		long			rowIndex);


	/*	tblSetColumn will set the specified column.  The text in colText is
   *	tab-delimited.
	 */
	BOOL far pascal _export tblSetColumn(
		OBJECTID	tableObj,
		LPSTR			colText,
		int				colIndex);


	/*	tblSetColumnCount will set the number of columns to be colCount.
	 *	This function will add or delete columns.  If columns are added the
   *	attributes from the last column will be copied to the new column.
	 */
	BOOL far pascal _export tblSetColumnCount(
		OBJECTID	tableObj,
		int				colCount);


	/*	tblSetRow will overwrite the specified row with rowText.  Multiple
	 *	rows will be set (overwritten) if there are returns in the rowText
	 *	parameter.  The fields are tab delimted.  The lines are return
	 *	delimted. 
	 */
	BOOL far pascal _export tblSetRow(
		OBJECTID	tableObj,
		LPSTR			rowText,
		long			startRow);


	/*	tblSetRowCount will set the number of rows to rowCount.  This will
	 *	delete excess rows or add new rows depending on the number of
	 *	existing rows.
   */
	BOOL far pascal _export tblSetRowCount(
		OBJECTID	tableObj,
		long			rowCount);

#ifdef __cplusplus
	}
#endif  /* __cplusplus */

////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __TABL_OBJ_H
