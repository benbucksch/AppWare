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
 | FILE		: SQLLIB.H                                                |
 | DESCRIPTION	: SQL Libraries Include File.		                  |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	01-Nov-1993  Initial Edit.                        |
 |		  mw	28-Apr-1994  Use QELIB 2.0 Include File to        |
 |				     Support Database View.		  |
\*------------------------------------------------------------------------*/
#include "sqllib.h"


SqlLib		sqlLib;			// SQLLIB class


BOOL SqlLib::FreeLib		/* ---- Free Library -------------------- */
( void
)
/* Frees the library. */
{
  FreeLibrary(handle);
  return FALSE;
} // FreeLib


SqlLib::SqlLib			/* ---- SQL Library Constructor --------- */
( void
)
/* Constructor. */
{
  isHandle = FALSE;
} // SqlLib	

SqlLib::~SqlLib			/* ---- SQL Library Destructor ---------- */
( void
)
/* Destructor. */
{
  if (isHandle)
    FreeLibrary(handle);
} // ~SqlLib	


int SqlLib::Version		/* ---- Version ------------------------- */
( void
)
/* Returns the version of the library. */
{
  return version;
} // Version

BOOL SqlLib::IsLoadLib		/* ---- Is Load Library ----------------- */
( void
)
/* Returns whether the library is currently loaded. */
{
  return isHandle;
} // IsLoadLib

BOOL SqlLib::LoadLib		/* ---- Load Library -------------------- */
( void				     
)
/* Loads the library, and the indirect function-addresses. */
{
  if (isHandle)
    return TRUE;			// it's already loaded
  return isHandle = LoadQelib();
} // LoadLib


		/*-----------------------------+
		|           QELIB.H            |
		+------------------------------+
		|  Q+E Library Include File    |
		+-----------------------------*/

//--------------------------
// Data dictionary routines
//--------------------------

//--------------------------------------------------------------------------
qeAPI(qeHANDLE) (*qe_Tables) (
	qeHANDLE        hdbc,           // Connection handle.
	qeLPSTR         qualifer_pattern,// Database (or directory) pattern.
	qeLPSTR         user_pattern,   // Pattern for selecting users.  If
					// the pattern is "", the current
					// user is assumed.  If the pattern
					// is "*", all users are selected.
					// Ignored for file-based databases,
					// for which the current working
					// directory is assumed.
	qeLPSTR         table_pattern,  // Pattern for selecting tables (or
					// files).  If the pattern is "",
					// all tables are selected.
	short           flags);         // Options that determine which
					// types of items are to be
					// returned.  See OPTIONS defined
					// below.

// Creates a statement execution (hstmt) that will return the set of
// database tables (or files) that can be accessed by the hdbc.
// RETURNS: the statement execution (hstmt), or 0 if the function fails.
// The records can be subsequently read using the qeFetch functions, and
// column values can be retrieved using either the qeVal or qeBindCol
// functions.
// When you have finished retrieving tables (when you receive EOF from
// qeFetchNext), you must call qeEndSQL to terminate the hstmt.

// The resulting records contain the following columns:
//      Table Qualifier Char(128)- Table qualifier.
//      Table User      Char(128)- Table owner.
//      Table Name      Char(128)- Table name.
//      Type            Short    - The type of item returned.  For
//                                 table-based sources, this can be used to
//                                 distinguish tables, views, stored
//                                 procedures, and synonym names, all of
//                                 which may be returned by this query.  For
//                                 file-based sources, the type is always
//                                 file.
//      Remarks         Char(256)- Remarks field for table.
//--------------------------------------------------------------------------

//--------------------------
// Error Handling functions
//--------------------------

//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_Err) (void);
// RETURNS: the result code from the most recent call to QELIB.  The values
// that this function can return are defined at the top of this file, under
// the heading "Result Code Values."
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_ErrMsgBuf) (
	qeLPSTR         msg_buf);       // Buffer for error message
// RETURNS: a pointer to the error messages from the most recent call to
// QELIB, either as the return value (qeErrMsg) or in a provided buffer
// (qeErrMsgBuf).  Multiple errors will be concatenated up to
// qeMAX_ERR_MSG_LEN bytes.  msg_buf must be at least this large, or an
// error may result.
//--------------------------------------------------------------------------

//-------------------------------
// Task functions
//-------------------------------

//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_LibInit) (void);
// Performs task initialization.  Each task that calls QELIB should call
// this first, if possible.  RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_LibTerm) (void);
// Performs task termination.  Each task that calls QELIB should call
// this when it is finished using QELIB, if possible.  RETURNS: a result code.
//--------------------------------------------------------------------------

//-------------------------------
// Database Connection functions
//-------------------------------

//--------------------------------------------------------------------------
qeAPI(qeHANDLE) (*qe_Connect) (
	qeLPSTR         con_string);    // Connection string
// Opens a connection to a database system and creates a handle to the
// connection (hdbc).  RETURNS: the connection handle, or 0 if connect fails.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_Disconnect) (
	qeHANDLE        hdbc);          // Connection handle
// Closes a connection to a database system.  RETURNS: a result code.
//--------------------------------------------------------------------------

//-------------------------
// SQL execution functions
//-------------------------

//--------------------------------------------------------------------------
qeAPI(qeHANDLE) (*qe_ExecSQL) (
	qeHANDLE        hdbc,           // Connection handle
	qeLPSTR         sql_stmt);      // SQL statement to execute
// Executes an SQL statement.  If sql_stmt is null, the statement is retrieved
// from the hdbc's SQL statement buffer, which is filled by calls to
// qeSetSQL and qeAppendSQL.
// RETURNS: a handle to statement execution (hstmt), or 0 if the function
// fails.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_EndSQL) (
	qeHANDLE        hstmt);         // Statement handle
// Ends a SQL statement's execution.  RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_NumModRecs) (
	qeHANDLE        hstmt);         // Statement handle
// RETURNS: the number of records affected by Insert, Update, or Delete
// SQL statements, or 0 if the statement is a Select statement.
//--------------------------------------------------------------------------

//-----------------------------
// Column definition functions
//-----------------------------

//--------------------------------------------------------------------------
qeAPI(short) (*qe_NumCols) (
	qeHANDLE        hstmt);         // Statement handle
// RETURNS: the number of columns in a Select statement, or 0 for
// non-SELECT statements.
//--------------------------------------------------------------------------
qeAPI(qeLPSTR) (*qe_ColName) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
//--------------------------------------------------------------------------
qeAPI(short) (*qe_ColType) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the QELIB data type of a column in a Select statement.
// See the top of this file for the list of QELIB data types.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_ColWidth) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the width (in bytes) of a column in a Select statement.
// Includes the zero terminator for Character or Date data types.
//--------------------------------------------------------------------------
qeAPI(short) (*qe_ColPrecision) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the number of significant digits for Decimal columns in a Select
// statement.
//--------------------------------------------------------------------------
qeAPI(short) (*qe_ColScale) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the number of digits to the right of decimal point for Decimal
// columns in a Select statement.
//--------------------------------------------------------------------------
qeAPI(short) (*qe_ColDateStart) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the offset of the first significant character (0-origin) for
// Date/Time columns in a Select statement.
//--------------------------------------------------------------------------
qeAPI(short) (*qe_ColDateEnd) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num);       // Column number (1-origin)
// RETURNS: the offset of the last significant character (0-origin) for
// Date/Time columns in a Select statement.
//--------------------------------------------------------------------------

//-------------------------
// Data fetching functions
//-------------------------

//-------------------------------------------------------------------------- 
qeAPI(qeSTATUS) (*qe_BindCol) (
	qeHANDLE        hstmt,          // Statement handle
	short           col_num,        // Column number (1-origin)
	qeLPVOID        value_ptr,      // Buffer to receive column values
	qeLPLONG        len_ptr);       // Buffer to receive column value
					// length
// Specifies the buffers to receive the value and lengths for one column
// from a Select statement.  Subsequent calls to qeFetch functions will get
// a record from the database and put the column values and their lengths
// in the buffers. When called, len_ptr must specify the size of value_ptr
// in bytes.  RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_SetSelectOptions) (
	qeHANDLE        hdbc,           // Connection handle
	long            flags);         // Option flags
// Sets options that determine which fetch commands and cursor behaviors
// are allowed.
// RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_FetchNext) (
	qeHANDLE        hstmt);         // Statement handle
// Gets the next record from the database system.
// RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_FetchRandom) (
	qeHANDLE        hstmt,          // Statement handle
	long            rec_num);       // Record number (1-origin)
// Gets a record from the database system given its record number.
// RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_FetchNumRecs) (
	qeHANDLE        hstmt);         // Statement handle
// RETURNS: the number of records resulting from the Select statement
// represented by hstmt.  Returns 0 otherwise. Can only be used if
// qeFetchSetOptions or qeSetSelectOptions is called to enable random
// fetching.
//--------------------------------------------------------------------------

//-----------------------
// Transaction functions
//-----------------------

//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_BeginTran) (
	qeHANDLE        hdbc);          // Connection handle
// Begins a transaction. RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_Commit) (
	qeHANDLE        hdbc);          // Connection handle
// Commits all changes since qeBeginTran and ends the transaction.
// RETURNS: a result code.
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_Rollback) (
	qeHANDLE        hdbc);          // Connection handle
// Rolls back all changes since qeBeginTran and ends the transaction.
// RETURNS: a result code.
//--------------------------------------------------------------------------

//---------------------------
// Data Conversion Functions
//---------------------------

//--------------------------------------------------------------------------

	//---------------------
	// To Character String
	//---------------------

//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_DateToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	qeLPSTR         date_val,       // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a standard date value to a character string formatted using
// the format string.
// RETURNS: the converted value (qeDateToChar), or a result code
// (qeDateToCharBuf).
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_IntToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	short           int_val,        // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a 2-byte integer value to a character string formatted using
// the format string.
// RETURNS: the converted value (qeIntToChar), or a result code
// (qeIntToCharBuf).
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_LongToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	long            long_val,       // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a 4-byte integer value to a character string formatted using
// the format string.
// RETURNS: the converted value (qeLongToChar), or a result code
// (qeLongToCharBuf).
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_FloatToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	float           flt_val,        // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a 4-byte floating value to a character string formatted using
// the format string.
// RETURNS: the converted value (qeFloatToChar), or a result code
// (qeFloatToCharBuf).
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_DoubleToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	double          dbl_val,        // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts an 8-byte floating value to a character string formatted using
// the format string.
// RETURNS: the converted value (qeDoubleToChar), or a result code
// (qeDoubleToCharBuf).
//--------------------------------------------------------------------------
qeAPI(qeSTATUS) (*qe_DecimalToCharBuf) (
	qeLPSTR         char_val,       // Buffer for character string result
	qeLPDEC         dec_val,        // Value to convert
	short           precision,      // Number of significant digits in
					// value
	short           scale,          // Number of digits right of decimal
					// in value
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a decimal value to a character string formatted using the format
// string.
// RETURNS: the converted value (qeDecimalToChar), or a result code
// (qeDecimalToCharBuf).
//-------------------------------------------------------------------------

	//-----------------
	// To Long Integer
	//-----------------

//--------------------------------------------------------------------------
qeAPI(long) (*qe_CharToLong) (
	qeLPSTR         char_val,       // Value to convert
	qeLPSTR         fmt_string);    // Format string for the conversion
// Converts a character string to a 4-byte integer.  The character string
// is formatted as specified by the format_string.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_DateToLong) (
	qeLPSTR         date_val);      // Value to convert
// Converts a standard date value to a Julian value returned as a 4-byte
// integer.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_IntToLong) (
	short           int_val);       // Value to convert
// Converts a 2-byte integer to a 4-byte integer.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_FloatToLong) (
	float           flt_val);       // Value to convert
// Converts a 4-byte float to a 4-byte integer.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_DoubleToLong) (
	double          dbl_val);       // Value to convert
// Converts a 8-byte float to a 4-byte integer.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------
qeAPI(long) (*qe_DecimalToLong) (
	qeLPDEC         dec_val,        // Value to convert
	short           precision,      // Number of significant digits in
					// value
	short           scale);         // Number of digits right of decimal
					// in value
// Converts a decimal number to a 4-byte integer.
// RETURNS: the converted integer.
//--------------------------------------------------------------------------

	//------------------------------------
	// To Double Precision Floating Point
	//------------------------------------

//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_CharToDouble) (
	qeLPSTR         char_val,       // Value to convert
	qeLPSTR         fmt_string      // Format string for the conversion
	qeHIDDENDBL);                   // See Borland notes
// Converts a character string to an 8-byte float.  The character string
// is formatted as specified by the format_string.
// RETURNS: the converted double.
//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_DateToDouble) (
	qeLPSTR         date_val        // Value to convert
	qeHIDDENDBL);                   // See Borland notes
// Converts a standard date value to a Julian value returned as an 8-byte
// float.
// RETURNS: the converted double.
//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_IntToDouble) (
	short           int_val         // Value to convert
	qeHIDDENDBL);                   // See Borland notes
// Converts a 2-byte integer to an 8-byte float.
// RETURNS: the converted double.
//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_LongToDouble) (
	long            long_val        // Value to convert
	qeHIDDENDBL);                   // See Borland notes
// Converts a 4-byte integer to an 8-byte float.
// RETURNS: the converted double.
//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_FloatToDouble) (
	float           flt_value       // Value to convert
	qeHIDDENDBL);                   // See Borland notes
// Converts a 4-byte float to an 8-byte float.
// RETURNS: the converted double.
//--------------------------------------------------------------------------
qeAPI(qeRETDBL) (*qe_DecimalToDouble) (
	qeLPDEC         dec_val,        // Value to convert
	short           precision,      // Number of significant digits in
					// value
	short           scale           // Number of digits right of decimal
					// in value
	qeHIDDENDBL);                   // See Borland notes
// Converts a decimal number to an 8-byte float.
// RETURNS: the converted double.
//--------------------------------------------------------------------------


BOOL SqlLib::LoadQelib		/* ---- Load Q+E Database Library ------- */
( void
)
/* Loads Q+E Database Library. */
{
  if ((handle = LoadLibrary("QELIB.DLL")) < (HINSTANCE) 32)
    return FALSE;

  version = QELIB200;

  //--------------------------
  // Data dictionary routines
  //--------------------------

  if ((qe_Tables = (qeAPI(qeHANDLE) (*) (qeHANDLE, qeLPSTR, qeLPSTR,
					  qeLPSTR, short))
      GetProcAddress(handle, "qeTables")) == 0)
    return FreeLib();

  //--------------------------
  // Error Handling functions
  //--------------------------

  if ((qe_Err = (qeAPI(qeSTATUS) (*) (void))
      GetProcAddress(handle, "qeErr")) == 0)
    return FreeLib();
  if ((qe_ErrMsgBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR msg_buf))
      GetProcAddress(handle, "qeErrMsgBuf")) == 0)
    return FreeLib();

  //-------------------------------
  // Task functions
  //-------------------------------

  if ((qe_LibInit = (qeAPI(qeSTATUS) (*) (void))
      GetProcAddress(handle, "qeLibInit")) == 0)
    return FreeLib();
  if ((qe_LibTerm = (qeAPI(qeSTATUS) (*) (void))
      GetProcAddress(handle, "qeLibTerm")) == 0)
    return FreeLib();

  //-------------------------------
  // Database Connection functions
  //-------------------------------

  if ((qe_Connect = (qeAPI(qeHANDLE) (*) (qeLPSTR))
      GetProcAddress(handle, "qeConnect")) == 0)
    return FreeLib();
  if ((qe_Disconnect = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeDisconnect")) == 0)
    return FreeLib();

  //-------------------------
  // SQL execution functions
  //-------------------------

  if ((qe_ExecSQL = (qeAPI(qeHANDLE) (*) (qeHANDLE, qeLPSTR))
      GetProcAddress(handle, "qeExecSQL")) == 0)
    return FreeLib();
  if ((qe_EndSQL = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeEndSQL")) == 0)
    return FreeLib();
  if ((qe_NumModRecs = (qeAPI(long) (*) (qeHANDLE))
      GetProcAddress(handle, "qeNumModRecs")) == 0)
    return FreeLib();

  //-----------------------------
  // Column definition functions
  //-----------------------------

  if ((qe_NumCols = (qeAPI(short) (*) (qeHANDLE))
      GetProcAddress(handle, "qeNumCols")) == 0)
    return FreeLib();
  if ((qe_ColName = (qeAPI(qeLPSTR) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColName")) == 0)
    return FreeLib();
  if ((qe_ColType = (qeAPI(short) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColType")) == 0)
    return FreeLib();
  if ((qe_ColWidth = (qeAPI(long) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColWidth")) == 0)
    return FreeLib();
  if ((qe_ColPrecision = (qeAPI(short) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColPrecision")) == 0)
    return FreeLib();
  if ((qe_ColScale = (qeAPI(short) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColScale")) == 0)
    return FreeLib();
  if ((qe_ColDateStart = (qeAPI(short) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColDateStart")) == 0)
    return FreeLib();
  if ((qe_ColDateEnd = (qeAPI(short) (*) (qeHANDLE, short))
      GetProcAddress(handle, "qeColDateEnd")) == 0)
    return FreeLib();

  //-------------------------
  // Data fetching functions
  //-------------------------

  if ((qe_BindCol = (qeAPI(qeSTATUS) (*) (qeHANDLE, short, qeLPVOID,
					  qeLPLONG))
      GetProcAddress(handle, "qeBindCol")) == 0)
    return FreeLib();
  if ((qe_SetSelectOptions = (qeAPI(qeSTATUS) (*) (qeHANDLE, long))
      GetProcAddress(handle, "qeSetSelectOptions")) == 0)
    return FreeLib();
  if ((qe_FetchNext = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeFetchNext")) == 0)
    return FreeLib();
  if ((qe_FetchRandom = (qeAPI(qeSTATUS) (*) (qeHANDLE, long))
      GetProcAddress(handle, "qeFetchRandom")) == 0)
    return FreeLib();
  if ((qe_FetchNumRecs = (qeAPI(long) (*) (qeHANDLE))
      GetProcAddress(handle, "qeFetchNumRecs")) == 0)
    return FreeLib();

  //-----------------------
  // Transaction functions
  //-----------------------

  if ((qe_BeginTran = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeBeginTran")) == 0)
    return FreeLib();
  if ((qe_Commit = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeCommit")) == 0)
    return FreeLib();
  if ((qe_Rollback = (qeAPI(qeSTATUS) (*) (qeHANDLE))
      GetProcAddress(handle, "qeRollback")) == 0)
    return FreeLib();

  //---------------------------
  // Data Conversion Functions
  //---------------------------

	  //---------------------
	  // To Character String
	  //---------------------

  if ((qe_DateToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, qeLPSTR, qeLPSTR))
      GetProcAddress(handle, "qeDateToCharBuf")) == 0)
    return FreeLib();
  if ((qe_IntToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, short, qeLPSTR))
      GetProcAddress(handle, "qeIntToCharBuf")) == 0)
    return FreeLib();
  if ((qe_LongToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, long, qeLPSTR))
      GetProcAddress(handle, "qeLongToCharBuf")) == 0)
    return FreeLib();
  if ((qe_FloatToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, float, qeLPSTR))
      GetProcAddress(handle, "qeFloatToCharBuf")) == 0)
    return FreeLib();
  if ((qe_DoubleToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, double,
						  qeLPSTR))
      GetProcAddress(handle, "qeDoubleToCharBuf")) == 0)
    return FreeLib();
  if ((qe_DecimalToCharBuf = (qeAPI(qeSTATUS) (*) (qeLPSTR, qeLPDEC, short,
						   short, qeLPSTR))
      GetProcAddress(handle, "qeDecimalToCharBuf")) == 0)
    return FreeLib();

	  //-----------------
	  // To Long Integer
	  //-----------------

  if ((qe_CharToLong = (qeAPI(long) (*) (qeLPSTR, qeLPSTR))
      GetProcAddress(handle, "qeCharToLong")) == 0)
     return FreeLib();
  if ((qe_DateToLong = (qeAPI(long) (*) (qeLPSTR))
      GetProcAddress(handle, "qeDateToLong")) == 0)
    return FreeLib();
  if ((qe_IntToLong = (qeAPI(long) (*) (short))
      GetProcAddress(handle, "qeIntToLong")) == 0)
    return FreeLib();
  if ((qe_FloatToLong = (qeAPI(long) (*) (float))
      GetProcAddress(handle, "qeFloatToLong")) == 0)
    return FreeLib();
  if ((qe_DoubleToLong = (qeAPI(long) (*) (double))
      GetProcAddress(handle, "qeDoubleToLong")) == 0)
    return FreeLib();
  if ((qe_DecimalToLong = (qeAPI(long) (*) (qeLPDEC, short, short scale))
      GetProcAddress(handle, "qeDecimalToLong")) == 0)
    return FreeLib();

	  //------------------------------------
	  // To Double Precision Floating Point
	  //------------------------------------

  if ((qe_CharToDouble = (qeAPI(qeRETDBL) (*) (qeLPSTR,
					       qeLPSTR   qeHIDDENDBL))
      GetProcAddress(handle, "qeCharToDouble")) == 0)
    return FreeLib();
  if ((qe_DateToDouble = (qeAPI(qeRETDBL) (*) (qeLPSTR   qeHIDDENDBL))
      GetProcAddress(handle, "qeDateToDouble")) == 0)
    return FreeLib();
  if ((qe_IntToDouble = (qeAPI(qeRETDBL) (*) (short   qeHIDDENDBL))
      GetProcAddress(handle, "qeIntToDouble")) == 0)
    return FreeLib();
  if ((qe_LongToDouble = (qeAPI(qeRETDBL) (*) (long   qeHIDDENDBL))
      GetProcAddress(handle, "qeLongToDouble")) == 0)
    return FreeLib();
  if ((qe_FloatToDouble = (qeAPI(qeRETDBL) (*) (float   qeHIDDENDBL))
      GetProcAddress(handle, "qeFloatToDouble")) == 0)
    return FreeLib();
  if ((qe_DecimalToDouble = (qeAPI(qeRETDBL) (*) (qeLPDEC, short,
						  short   qeHIDDENDBL))
      GetProcAddress(handle, "qeDecimalToDouble")) == 0)
    return FreeLib();
  return TRUE;
} // LoadQelib
