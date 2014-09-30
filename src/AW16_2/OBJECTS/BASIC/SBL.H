/* $Header:   S:/vcs/win/dev/sbl/include/sbl.h_v   1.194   12 Aug 1994 09:20:40   MADDOG  $ */
/*++ ***************************901022*********************************

NAME:		sbl.h

DESCRIPTION:	SBL - Public include file

HISTORY: $Log:   S:/vcs/win/dev/sbl/include/sbl.h_v  $
 * 
 *    Rev 1.194   12 Aug 1994 09:20:40   MADDOG
 * added get/set IP for SblContext and pack pragma
 * 
 *    Rev 1.188   13 Jul 1994 14:46:38   MSR
 * Added SBL_LI_UIOCRLF
 * 
 *    Rev 1.187   11 Jul 1994 10:35:48   sbrennan
 * added SBL_NOTIFY_FILESYS_REPLACED
 * 
 *    Rev 1.186   06 Jul 1994 17:58:34   MADDOG
 * added sblcntx_Gerr
 * 
 *    Rev 1.185   29 Jun 1994 17:55:36   MADDOG
 * added SBL_METHOD_TEMP
 * 
 *    Rev 1.184   23 Jun 1994 15:36:28   markt
 * WITH and CSTRINGS errors
 * 
 *    Rev 1.183   10 Jun 1994 14:23:20   sbrennan
 * added SblGetDialogFont
 * 
 *    Rev 1.182   10 Jun 1994 13:17:42   MADDOG
 * added VALIDATE_OBJS instance flag
 * 
 *    Rev 1.181   09 Jun 1994 16:34:54   MARKT
 * SblFreeAllGlobals
 * 
 *    Rev 1.180   27 May 1994 10:30:12   MADDOG
 * Cleaned up dll/virtual/static address call interface. ALso fixed a few
 * misc. bugs along the way.
 * 
 *    Rev 1.179   24 May 1994 15:29:48   MSR
 * Added DLLENTRY to SblSetDefaultVariable
 * 
 *    Rev 1.178   20 May 1994 19:58:28   markt
 * Added SblFreeGlobal
 * 
 *    Rev 1.177   16 May 1994 17:12:10   MADDOG
 * added class to sbl context
 * 
 *    Rev 1.176   16 May 1994 13:05:48   MADDOG
 * added default variable
 * 
 *    Rev 1.175   11 May 1994 16:09:20   MADDOG
 * added some errors and a class reg flag
 * 
 *    Rev 1.174   06 May 1994 11:33:12   MADDOG
 * added CPP calling conventions
 * 
 *    Rev 1.173   02 May 1994 17:54:40   markt
 * SBL_E_MISSING_PICTURE
 * 
 *    Rev 1.172   26 Apr 1994 12:49:00   MADDOG
 * change SBL_UNREG_NOTIFY to take SBL_WORD instead of int.
 * 
 *    Rev 1.171   26 Apr 1994 11:37:48   MADDOG
 * added unregister code and made are entry points be loadds
 * 
 *    Rev 1.170   22 Apr 1994 16:09:44   markt
 * Compiler erros on DropListBox and Picture controls
 * 
 *    Rev 1.169   15 Apr 1994 12:38:20   MADDOG
 * Changed prototype of SblFormat
 * 
 *    Rev 1.168   05 Apr 1994 07:59:22   MSR
 * Changed SBL_LI_DLGMSG_CALLBACK to SBL_LI_DLGMSG_SUSPEND
 * 
 *    Rev 1.167   02 Apr 1994 18:14:44   markt
 * added SBL_CTLID_ID
 * 
 *    Rev 1.166   01 Apr 1994 20:07:26   markt
 * SBL_E_DLGNESTING
 * 
 *    Rev 1.165   31 Mar 1994 15:03:08   MADDOG
 * added line number info to SBL_PROC_INFO
 * 
 *    Rev 1.164   30 Mar 1994 19:49:00   markt
 * Exposed Dialog Controls
 * 
 *    Rev 1.163   29 Mar 1994 22:03:18   MADDOG
 * added SblOleIsEnabled
 * 
 *    Rev 1.162   29 Mar 1994 11:08:12   MSR
 * Update THREADINFO structure some more
 * 
 *    Rev 1.161   29 Mar 1994 09:49:22   MADDOG
 * added TEMP_ADT flags
 * 
 *    Rev 1.160   28 Mar 1994 17:27:34   MSR
 * Added tidParent and hDlg to THREADINFO structure
 * 
 *    Rev 1.159   24 Mar 1994 19:39:08   MARKT
 * SBL_LI_DLGUNITS_WINWORD
 * 
 *    Rev 1.158   22 Mar 1994 17:05:58   MADDOG
 * Changed COOPCOMP routine to be void.
 * 
 *    Rev 1.157   18 Mar 1994 17:47:24   MSR
 * Phase 1 of suspending dialog boxes
 * 
 *    Rev 1.156   16 Mar 1994 14:14:58   MADDOG
 * Changed around the cooperative compile stuff
 * 
 *    Rev 1.155   14 Mar 1994 11:59:08   sbrennan
 * added notification mechanism
 * 
 *    Rev 1.154   11 Mar 1994 11:39:56   sbrennan
 * exported overloadable file system
 * 
 *    Rev 1.153   09 Mar 1994 18:47:52   markt
 * New error codes
 * 
 *    Rev 1.152   08 Mar 1994 10:48:56   MSR
 * Added sblstrlen macro
 * 
 *    Rev 1.151   08 Mar 1994 10:28:04   sbrennan
 * added long words to instance and thread
 * 
 *    Rev 1.150   02 Mar 1994 14:25:30   MADDOG
 * added SblRedim
 * 
 *    Rev 1.149   28 Feb 1994 14:11:36   sbrennan
 * first pass at stack swapping
 * 
 *    Rev 1.148   23 Feb 1994 13:40:28   MADDOG
 * Change SBL_DATATYPE_ILLEGAL to be 0xf instead of 0xffff, added
 * SBL_DATATYPE_SYSDIALOG and IDS_ASSERT_GPFAULT.
 * 
 *    Rev 1.147   23 Feb 1994 12:23:20   msr
 * Added QNAN support - started cooperative compilation
 * 
 *    Rev 1.146   09 Feb 1994 18:13:44   MADDOG
 * Fixed so all structure members are aligned on word boundaries.
 * 
 *    Rev 1.145   08 Feb 1994 16:41:10   MADDOG
 * Added SblInputBoxB, SblDissectDate and SblBuildDate.
 * 
 *    Rev 1.144   04 Feb 1994 10:33:16   MSR
 * Change SBL_DATATYPE_ANY to match DATATYPE_ANY
 * 
 *    Rev 1.143   03 Feb 1994 13:47:40   MADDOG
 * Added SBL_METHOD_DEFAULT and borland stuff.
 * 
 *    Rev 1.142   11 Jan 1994 13:13:48   MADDOG
 * Added SblGetPlainErrorText and IDS_ id's for ole errors.
 * 
 *    Rev 1.141   10 Jan 1994 19:07:42   MSR
 * Export SblFormat function
 * 
 *    Rev 1.140   30 Dec 1993 13:55:12   MADDOG
 * Added IDS defines for resrouce file.
 * 
 *    Rev 1.139   29 Dec 1993 16:52:58   MADDOG
 * Added an error.
 * 
 *    Rev 1.138   22 Dec 1993 14:42:06   MADDOG
 * early bound class copyback
 * 
 *    Rev 1.137   14 Dec 1993 17:00:38   markt
 * support for PRIVATE
 * 
 *    Rev 1.136   14 Dec 1993 14:25:48   MSR
 * Add PasswordBox functions and make SblInputBox public
 * 
 *    Rev 1.135   13 Dec 1993 10:59:22   MADDOG
 * Addeds blcntx_SerrOnly and three errors.
 * 
 *    Rev 1.134   10 Dec 1993 14:20:26   MADDOG
 * Added hard address binding. Fixed context error handling. Added the last
 * error message stuff.
 * 
 *    Rev 1.133   06 Dec 1993 18:33:36   markt
 * SBL_E_ACTIVE_THREAD
 * 
 *    Rev 1.132   29 Nov 1993 18:02:28   MADDOG
 * added STRONG_OVERLOAD for overloading builtins
 * 
 *    Rev 1.131   29 Nov 1993 18:00:16   sbrennan
 * changed EMITCALLBACK to return SBL_SHORT
 * 
 *    Rev 1.130   29 Nov 1993 11:25:46   MADDOG
 * removed some ints from the api

*/

// ---------------------------------------------------------------------
//                 Copyright (c) 1990-91 Softbridge Ltd.
// ---------------------------------------------------------------------

/*  If defined, the following flags inhibit definition
 *     of the indicated items.
 *
 *  SBL_NOERROR		Error numbers (SBL_E_*).
 *  SBL_NOFILESYSTEM	File System.
 *  SBL_NODEBUG		Debugging info.
 *
 */

#ifndef SBL_H
#define SBL_H

#ifdef _MSC_VER
#pragma pack(1)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//
	// platform specific API calling convention definitions
	//
#if defined(OS2)

#if !defined(SBL_16BIT)
	//
   // 32 bit OS/2
   //
   // calling conventions and nops for thunking API
   //
#define DLLENTRY		_System
#define DLLENTRYPTR		* DLLENTRY
#define SblThunk32(Ptr32)	(Ptr32)
#define SblThunk16(Ptr16)	(Ptr16)

#define SBL_FAR
#define SBL_NEAR
#define SBL_CDECL	__cdecl	// this is a guess - I probably screwed up
#else
	//
	// 16 bit OS/2
	//
#include "sbl16.h"	//** 16 bit function names
	//
	// calling convention of 16 bit API
	//
#define SBL_FAR			far
#define SBL_NEAR			near
#define SBL_CDECL	__cdecl	// this is a guess - I probably screwed up
#define DLLENTRY	far pascal
#define DLLENTRYPTR	DLLENTRY *
	//
	// routines to convert 16:16 to 0:32 and visa-versa
	//
SBL_PVOID DLLENTRY Sbl16FlatToSel(SBL_PVOID Ptr16);
SBL_PVOID DLLENTRY Sbl16SelToFlat(SBL_PVOID Ptr32);
	//
	// macros to ease upgrading 16:16 app to 0:32 app
	//
#define SblThunk32(Ptr32)	Sbl16FlatToSel(Ptr32)
#define SblThunk16(Ptr16)	Sbl16SelToFlat(Ptr16)
#endif
	//
	// get window handle from SBL_HWND
	//
#define SblGetHWnd(x)	x

	//
	//** end OS/2 definitions
	//
#elif defined(NT) || defined(_NTWIN)
	//
	//	NT stuff
	//
#define SBL_FAR
#define SBL_NEAR
#define SBL_CDECL	__cdecl	// this is a guess - I probably screwed up
#define DLLENTRY
#define DLLENTRYPTR		DLLENTRY *

#define SblGetHWnd(x)	((HWND)x)

#else
	//
	// MS Windows API calling conventions
	//
#if (_MSC_VER >= 700)
#define SBL_CDECL			__cdecl
#define SBL_LOADDS		__loadds
#else
#define SBL_CDECL			_cdecl
#define SBL_LOADDS		_loadds
#endif
#define SBL_FAR			far
#define SBL_NEAR			near
#define DLLENTRYSPROUT	far pascal _export
#define DLLENTRY			DLLENTRYSPROUT SBL_LOADDS
#define DLLENTRYPTR		far DLLENTRYSPROUT *
	//
	// get HWND from SBL_HWND
	//
#define SblGetHWnd(x)	LOWORD(x)
#endif			//** defined(OS2)
	//
	// SBL typedefs
	//
typedef short						SBL_SHORT;
typedef unsigned short			SBL_WORD;
typedef long						SBL_LONG;
typedef unsigned long			SBL_ULONG;
typedef void SBL_FAR *			SBL_PVOID;
typedef char SBL_FAR *			SBL_LPSTR;
typedef void SBL_NEAR *			SBL_HANDLE;
typedef unsigned char			SBL_BYTE;
typedef SBL_WORD					SBL_BOOL;
typedef SBL_BYTE SBL_FAR *		SBL_LPBYTE;
typedef SBL_SHORT SBL_FAR *	SBL_LPSHORT;
typedef SBL_WORD SBL_FAR *		SBL_LPWORD;
typedef SBL_ULONG SBL_FAR *	SBL_LPULONG;
typedef SBL_ULONG					SBL_HWND;
typedef SBL_ULONG					SBL_TID;
typedef SBL_TID SBL_FAR *		SBL_LPTID;
typedef SBL_ULONG					SBL_INSTANCE;
typedef SBL_ULONG					HSBL;
typedef HSBL SBL_FAR *			LPHSBL;
typedef SBL_SHORT					SBL_STATUS;
typedef SBL_STATUS SBL_FAR *	SBL_LPSTATUS;



	//
	// typedef for user handles
	//
typedef SBL_ULONG SBL_APPLHANDLE;

#define sblstrlen(lps) \
	((lps)? lstrlen(lps) : 0)
//-------------------------C O N T E X T--------------------------
/*
 * Sbl contexts are supplied to routines called by SBL. The context is passed
 * as a handle which can be used with the routine SblContext. SblContext accepts
 * the context handle, a request constant and a pointer to a value.
 *
 * If SblContext successfully completes the request it returns 0, otherwise
 * it return a SBL_E_ constant.
 */
typedef SBL_LONG	SBL_HCONTEXT;

// request codes for SblContext
enum context_req {
		sblcntx_Ghsbl,		// get the instance handle
		sblcntx_Gtid,		// get the thread id
		sblcntx_Serr,		// set the current error code
		sblcntx_SerrText,	// set the error text (must set code also)
		sblcntx_SerrOnly,	// set the error code w/o zeroing out the error text
#define MAX_SERRTEXT_LENGTH	127
		sblcntx_Ghmd,		// get module handle
		sblcntx_Gclass,	// get class type
		sblcntx_GclassName,// get class name
		sblcntx_Gerr,		// get error code stored in context
		sblcntx_Gip,		// get instruction pointer
		sblcntx_Sip,		// set instruction pointer
		sblcntx_Gme,		// get "me" pointer for calling module
};

SBL_STATUS DLLENTRY SblContext(
		SBL_HCONTEXT context,		// the context to use
		enum context_req request,	// request code
		void *resultOrValue);		// place to store value for get. Place to
											// read values from for set.



// --------------------- Logical Module File System ---------------------
#ifndef SBL_NOFILESYSTEM
typedef SBL_ULONG SBL_FILE;

#define SBL_E_OPEN_FILE 0xffffL     // This value is returned by functions
											// which return SBL_FILE values when
											// an error occurred, and
											// no file was opened or created.

#define SBL_CCH_MAX_NAME	 40		// ALL names <= 40 chars.
												// Used for functions, subprograms,
												// keywords, ...
												// 40 chars does NOT include trailing
												// NULL.

typedef struct sblFileSystem {
    int (DLLENTRYPTR SblSearchPath)(SBL_WORD wFlags,
												SBL_LPSTR pszPath,
												SBL_LPSTR pszFileName,
												SBL_WORD cExt,
												SBL_LPSTR SBL_FAR * ppszExt,
												SBL_LPSTR pszBuf,
												SBL_WORD cbBuf);

    SBL_FILE (DLLENTRYPTR Open)( SBL_LPSTR, SBL_SHORT );
    SBL_FILE (DLLENTRYPTR Creat)( SBL_LPSTR, SBL_SHORT, SBL_ULONG );

    int (DLLENTRYPTR Close)( SBL_FILE );

    SBL_LONG (DLLENTRYPTR Lseek)( SBL_FILE, SBL_LONG, SBL_SHORT );

    SBL_WORD (DLLENTRYPTR Read)( SBL_FILE, SBL_LPBYTE, SBL_WORD );
    SBL_WORD (DLLENTRYPTR Write)( SBL_FILE, SBL_LPBYTE, SBL_WORD );

    SBL_ULONG (DLLENTRYPTR FileDateTime) (SBL_LPSTR FileName);

} SBL_FILE_SYSTEM, SBL_FAR * SBL_LPFILE_SYSTEM;

typedef struct sblFileSystemEx {
    int (DLLENTRYPTR SblSearchPath)(HSBL hSbl,
	 											SBL_WORD wFlags,
												SBL_LPSTR pszPath,
												SBL_LPSTR pszFileName,
												SBL_WORD cExt,
												SBL_LPSTR SBL_FAR * ppszExt,
												SBL_LPSTR pszBuf,
												SBL_WORD cbBuf);

    SBL_FILE (DLLENTRYPTR Open)( HSBL hSbl, SBL_LPSTR, SBL_SHORT );
    SBL_FILE (DLLENTRYPTR Creat)( HSBL hSbl, SBL_LPSTR, SBL_SHORT, SBL_ULONG );

    int (DLLENTRYPTR Close)( HSBL hSbl, SBL_FILE );

    SBL_LONG (DLLENTRYPTR Lseek)( HSBL hSbl, SBL_FILE, SBL_LONG, SBL_SHORT );

    SBL_WORD (DLLENTRYPTR Read)( HSBL hSbl, SBL_FILE, SBL_LPBYTE, SBL_WORD );
    SBL_WORD (DLLENTRYPTR Write)( HSBL hSbl, SBL_FILE, SBL_LPBYTE, SBL_WORD );

    SBL_ULONG (DLLENTRYPTR FileDateTime) (HSBL hSbl, SBL_LPSTR FileName);

} SBL_FILE_SYSTEMEX, SBL_FAR * SBL_LPFILE_SYSTEMEX;

	// Flags for SblSearchPath
#define SBL_SF_CUR_DIRECTORY		1   // Search in current directory
#define SBL_SF_PATH				2   // Search on specified path string

extern SBL_STATUS DLLENTRY SblSearchPath(SBL_WORD wFlags,
											 SBL_LPSTR pszPath,
											 SBL_LPSTR pszFileName,
											 SBL_WORD cExt,
											 SBL_LPSTR SBL_FAR * ppszExt,
											 SBL_LPSTR pszBuf,
											 SBL_WORD cbBuf);
	// For now, the cbBuf arg to SblSearchPath is ignored - it BETTER be
	// big enough
extern SBL_FILE	DLLENTRY	SblOpenFile( SBL_LPSTR, SBL_SHORT );
extern SBL_FILE	DLLENTRY	SblCreatFile( SBL_LPSTR, SBL_SHORT, SBL_ULONG );
extern SBL_STATUS	DLLENTRY	SblCloseFile( SBL_FILE );
extern long			DLLENTRY SblLseekFile( SBL_FILE, long, SBL_SHORT );
extern SBL_WORD	DLLENTRY SblReadFile( SBL_FILE, SBL_LPBYTE, SBL_WORD );
extern SBL_WORD	DLLENTRY SblWriteFile( SBL_FILE, SBL_LPBYTE, SBL_WORD );
extern SBL_ULONG	DLLENTRY SblFileDateTime(SBL_LPSTR FileName );


extern SBL_STATUS DLLENTRY SblSearchPathEx(
											 HSBL hSbl,
											 SBL_WORD wFlags,
											 SBL_LPSTR pszPath,
											 SBL_LPSTR pszFileName,
											 SBL_WORD cExt,
											 SBL_LPSTR SBL_FAR * ppszExt,
											 SBL_LPSTR pszBuf,
											 SBL_WORD cbBuf);
	// For now, the cbBuf arg to SblSearchPath is ignored - it BETTER be
	// big enough
extern SBL_FILE	DLLENTRY	SblOpenFileEx( HSBL, SBL_LPSTR, SBL_SHORT );
extern SBL_FILE	DLLENTRY	SblCreatFileEx( HSBL, SBL_LPSTR, SBL_SHORT, SBL_ULONG );
extern SBL_STATUS	DLLENTRY	SblCloseFileEx( HSBL, SBL_FILE );
extern long			DLLENTRY SblLseekFileEx(HSBL, SBL_FILE, long, SBL_SHORT );
extern SBL_WORD	DLLENTRY SblReadFileEx( HSBL, SBL_FILE, SBL_LPBYTE, SBL_WORD );
extern SBL_WORD	DLLENTRY SblWriteFileEx( HSBL, SBL_FILE, SBL_LPBYTE, SBL_WORD );
extern SBL_ULONG	DLLENTRY SblFileDateTimeEx(HSBL, SBL_LPSTR FileName );

typedef SBL_ULONG SBL_HBFILE;

extern SBL_STATUS DLLENTRY SblFSSearchPath(HSBL hSbl,
											 SBL_WORD wFlags,
											 SBL_LPSTR pszPath,
											 SBL_LPSTR pszFileName,
											 SBL_WORD cExt,
											 SBL_LPSTR SBL_FAR * ppszExt,
											 SBL_LPSTR pszBuf,
											 SBL_WORD cbBuf);
extern SBL_HBFILE	DLLENTRY	SblFSOpenFile( HSBL hSbl, SBL_LPSTR szFile, SBL_SHORT fReadWrite);
extern SBL_HBFILE	DLLENTRY	SblFSCreatFile( HSBL hSbl, SBL_LPSTR szFile, SBL_SHORT Atts, SBL_ULONG dwLen);
extern SBL_STATUS	DLLENTRY	SblFSCloseFile(SBL_HBFILE hFile );
extern long			DLLENTRY SblFSLseekFile(SBL_HBFILE hFile, long Pos, SBL_SHORT Mode);
extern SBL_WORD	DLLENTRY SblFSReadFile( SBL_HBFILE hFile, SBL_LPBYTE Buf, SBL_WORD Len);
extern SBL_WORD	DLLENTRY SblFSWriteFile( SBL_HBFILE hFile, SBL_LPBYTE Buf, SBL_WORD Len);
extern SBL_ULONG	DLLENTRY SblFSFileDateTime( HSBL hSbl, SBL_LPSTR FileName );

extern SBL_STATUS DLLENTRY SblSetFileSystem(
	HSBL hSBL,
	SBL_FILE_SYSTEM SBL_FAR * pFileSystem,	// new filesystem to use
	SBL_WORD cbFileSystem					// size of filesystem structure
	);

extern SBL_STATUS DLLENTRY SblSetFileSystemEx(
	HSBL hSBL,
	SBL_FILE_SYSTEMEX SBL_FAR * pFileSystem,	// new filesystem to use
	SBL_WORD cbFileSystem					// size of filesystem structure
	);

SBL_STATUS DLLENTRY SblGetFileSystem (
	HSBL hSBL,
	SBL_FILE_SYSTEM SBL_FAR * pFileSystem,	// file system info goes here
	SBL_WORD cbFileSystem				// size of filesystem structure
	);

SBL_STATUS DLLENTRY SblGetFileSystemEx (
	HSBL hSBL,
	SBL_FILE_SYSTEMEX SBL_FAR * pFileSystem,	// file system info goes here
	SBL_WORD cbFileSystem				// size of filesystem structure
	);

#endif	// SBL_NOFILESYSTEM
// --------------------------- String Descriptor --------------------------

typedef struct stringDescr {
    SBL_WORD type;		// always 0 for dynamic strings.
    SBL_WORD cch;
    SBL_LPBYTE pch;	// This points to a buffer of size cch+1 !!!
						// pch[cch] == 0 !!!!!
						// This does NOT mean that we mind if there exists
						// an i such that 0 < i < cch and pch[cch] == 0.
						// We make this rule for convenience when passing
						// string descriptors to C functions, where we
						// obviously won'y have embedded nulls.
						// This character is NOT addressable in SBL,
						// and the length of the string is cch.
} SBL_STRING_DESCR;
typedef SBL_STRING_DESCR SBL_FAR * SBL_LPSTRING_DESCR;
typedef SBL_STRING_DESCR SBL_NEAR * SBL_NPSTRING_DESCR;

#if defined(OS2) && defined(SBL_16BIT)
	//
	// build these this way for compatibility with rest of
	// 16 bit api
	//
#define SblStrGetPointer(pStr32)	\
		SblStrGetPointer16((SBL_LPSTRING_DESCR)pStr32)
#define SblStrGetLen(pStr32)		\
		SblStrGetLen16((SBL_LPSTRING_DESCR)pStr32)
#define SblStrGetType(pStr32)		\
		SblStrGetType16((SBL_LPSTRING_DESCR)pStr32)

SBL_LPSTR	DLLENTRY SblStrGetPointer16(SBL_LPSTRING_DESCR pStr);
SBL_WORD	DLLENTRY SblStrGetLen16(SBL_LPSTRING_DESCR pStr);
SBL_WORD	DLLENTRY SblStrGetType16(SBL_LPSTRING_DESCR pStr);

#else
#define SblStrGetPointer(pStr)	(((SBL_LPSTRING_DESCR)pStr)->pch)
#define SblStrGetLen(pStr)			(((SBL_LPSTRING_DESCR)pStr)->cch)
#define SblStrGetType(pStr)		(((SBL_LPSTRING_DESCR)pStr)->type)
#endif
// ------------------------------------------------------------------------
typedef SBL_ULONG SBL_HMD, SBL_FAR * SBL_LPHMD;

typedef struct {
    SBL_HMD   hmd;			// Module handle
    SBL_WORD  ire;			// Index of Resource (procedure) where error occurred.
    SBL_WORD  erl;			// Error line number (within source module).
    SBL_SHORT err;			// Error code.
    SBL_LPSTR text;			// Filled in with a pointer to the null terminated
									// error text.
} SBL_ERROR_INFO;
typedef SBL_ERROR_INFO SBL_FAR *SBL_LPERROR_INFO;

#define SBL_STATE_READY		0
#define SBL_STATE_RUNNING	1
#define SBL_STATE_ERROR		2
#define SBL_STATE_SUSPEND	3

typedef struct {
    SBL_HMD hmd;				// Module handle
    SBL_WORD ire;				// Resource (procedure) index
    SBL_WORD line;			// Current line of code (within source module).
    SBL_WORD state;			// one of above SBL_STATE_???
    SBL_WORD nestlvl;		// Routine Nesting Level
// Note on following fields:  These fields were introduced in SBL version 3.2
// They are only tranferred if the application gives us a sufficiently large
// buffer (i.e., application is recompiled with new size).
// The fields are used to track dynamic dialogs.  The dynamic dialog function
// is run on its own thread.  If it is running, GetCurrentThread will return
// the internally created thread.  The original run (callproc) thread may be
// located by tracing tidParent.  Equivently, the currently executing dialog
// function may be found starting with the original run thread by tracing
// tidChild.  hDlg will be set for the thread that put up the dialog box, 
// hParentDlg will be set for the one running the dynamic dialog function.  
// For example, the application run procedure P on thread T1 which puts
// up dialog D defined as having a dialog function of F which runs on T2. If 
// execution is interupted while F is executing the current thread will be T2.
// T2.tidParent == T1, T2.tidChild == 0, T2.hDlg == 0, T2.hParentDlg = D;  
// T1.tidParent == 0, T1.tidChild = T2, T1.hDlg = D, and T1.hParentDlg = 0.

    SBL_TID  tidParent;		// thread id of parent (0 if none)
    SBL_TID  tidChild;		// thread id of chidl (0 if none)
    SBL_HWND hDlg;			// handle to active dialog (0 if none)
	 SBL_HWND hParentDlg;   // handle to dialog which put up this thread.
} SBL_THREAD_INFO;
typedef SBL_THREAD_INFO SBL_FAR *SBL_LPTHREAD_INFO;

	// callback functions for interpreter
typedef SBL_SHORT (DLLENTRYPTR SBL_LPFN_CALLBACK)( SBL_STATUS );
typedef SBL_SHORT (DLLENTRYPTR LPFN_CALLBACK)( SBL_STATUS );	   // for back compatibility
typedef SBL_SHORT (DLLENTRYPTR SBL_LPFN_ERRCALLBACK)( SBL_STATUS error, SBL_LPSTR pszErrorBuf,
			int cbErrorBuf);

typedef SBL_SHORT (DLLENTRYPTR SBL_LPFN_USERIOCALLBACK)(
			SBL_LPSTRING_DESCR psdOut,
			SBL_LPSTRING_DESCR psdIn );

typedef SBL_SHORT (DLLENTRYPTR SBL_LPFN_EMITCALLBACK)( 
			SBL_LPSTRING_DESCR psdOut,
			SBL_WORD flags); 

// These are flags that may be passed to EmitCallBack
#define SBL_EMIT_CRT	0x0001
#define SBL_EMIT_COMMA  0x0002
#define SBL_EMIT_SEMI	0x0004
#define SBL_EMIT_MASK   0x000F

#if !defined(OS2)
// Win: Dialog Box Creation Callback
#if defined(DLGPROC)
typedef SBL_HWND (DLLENTRYPTR SBL_LPFN_DLGINDIRECT_CALLBACK) (
								const void SBL_NEAR *hInst,
								const void SBL_FAR*,
								void SBL_NEAR *hWnd,
								DLGPROC,
								SBL_LONG);
#else
typedef SBL_HWND (DLLENTRYPTR SBL_LPFN_DLGINDIRECT_CALLBACK) (
								void SBL_NEAR *hInst,
								const void SBL_FAR *dum1,
								void SBL_NEAR *hWnd,
								SBL_PVOID dum2,
								SBL_LONG dum3);
#endif
#else
//OS2: Dialog Box Creation Callback
typedef SBL_HWND (DLLENTRYPTR SBL_LPFN_DLGINDIRECT_CALLBACK) (
								SBL_HWND hwndParent,
                        SBL_HWND hwndOwner,
                        SBL_PVOID pfnDlgProc,
                        SBL_PVOID pdlgt,
                        SBL_PVOID pCreateParams);
#endif

/* SBL memory management */
typedef SBL_ULONG SBL_HEAP;

// ---------------------- Function Declarations ------------------------

// SBL memory management
SBL_HEAP				DLLENTRY SblCreateHeap( SBL_WORD flags );
void					DLLENTRY SblDestroyHeap( SBL_HEAP heap);
SBL_PVOID			DLLENTRY SblAllocMem( SBL_HEAP heap, SBL_ULONG size);
SBL_ULONG				DLLENTRY SblMemSize( SBL_PVOID p);
SBL_PVOID			DLLENTRY SblReallocMem( SBL_PVOID p, SBL_ULONG newSize );
void					DLLENTRY SblFreeMem( SBL_PVOID p );
SBL_LPBYTE			DLLENTRY SblCopyString(SBL_HEAP heap, SBL_LPBYTE str);
void					DLLENTRY SblFreeDescr( SBL_PVOID p);
int					DLLENTRY SblQueryHeap( SBL_PVOID p, SBL_HEAP SBL_FAR *pRetHeap );

#ifndef __BORLANDC__
SBL_STRING_DESCR	DLLENTRY SblCreateStringDescr(
										SBL_TID tid,
										SBL_LPSTR pch,
										SBL_SHORT cch );
SBL_STRING_DESCR	DLLENTRY SblCreateSzDescr( SBL_TID tid, SBL_LPSTR psz );
#endif
SBL_STATUS					DLLENTRY SblSetStringDescr(
										SBL_TID tid,
										SBL_LPSTRING_DESCR psd,
										SBL_LPSTR pch,
										SBL_WORD cch );
SBL_PVOID			DLLENTRY SblThreadAllocMem( SBL_TID tid,  SBL_WORD cb );
SBL_HEAP				DLLENTRY SblGetInstanceHeap( HSBL hsbl );
SBL_STATUS DLLENTRY		SblCallProc(SBL_TID tid,
										SBL_HMD hmd,
										SBL_WORD ireProc,
										SBL_PVOID SBL_FAR * ppArgs );
SBL_STATUS DLLENTRY		SblCallProcObject(SBL_TID tid,
										SBL_HMD hmd,
										SBL_WORD ireProc,
										SBL_PVOID SBL_FAR * ppArgs,
										SBL_PVOID me );
SBL_STATUS					DLLENTRY SblGetCurrentThread( SBL_LPTID ptid);
SBL_STATUS					DLLENTRY SblGetClientThreadData(
										SBL_TID tid,
										SBL_ULONG SBL_FAR *pdw );
SBL_STATUS					DLLENTRY SblSetClientThreadData( SBL_TID tid, SBL_ULONG dw );
SBL_STATUS					DLLENTRY SblSetHwndParent (
										SBL_TID tid,
										SBL_HWND hwndParent );
SBL_STATUS					DLLENTRY SblGetHwndParent (
										SBL_TID tid,
										SBL_HWND SBL_FAR * phwndParent );
SBL_STATUS					DLLENTRY SblInitInternational( HSBL hsbl );
	//
	// The following macros are provided for backward compatibility with
	// earlier versions of the Get/SetClientThreadData APIs.
	//
#define SblGetClientData(_tid_,_pdw_)	SblGetClientThreadData (_tid_, _pdw_)
#define SblSetClientData(_tid_,_dw_)	SblSetClientThreadData (_tid_, _dw_)

SBL_STATUS			DLLENTRY SblSetCommandLine( SBL_TID tid, SBL_LPSTR pszCmd );
SBL_STATUS			DLLENTRY SblGetCommandLine(
										SBL_TID tid,
										SBL_LPSTR pszCmd,
										SBL_WORD cchCmd );
SBL_ULONG			DLLENTRY SblHelpContext( SBL_LPSTR pszContext );
void					DLLENTRY SblThreadError( SBL_TID tid, SBL_SHORT err );
void					DLLENTRY SblFloatSignalHandler( int sig, int subcode );
SBL_STATUS			DLLENTRY SblGetErrorInfo(
										SBL_TID tid,
										SBL_LPERROR_INFO pErrInfo,
										SBL_WORD cbErrInfo);
SBL_STATUS			DLLENTRY SblGetErrorText(
										SBL_STATUS errNumber,
										SBL_LPSTR pszErrBuf,
										SBL_WORD cbErrBuf );
SBL_STATUS			DLLENTRY SblGetPlainErrorText(
										SBL_STATUS errNumber,
										SBL_LPSTR pszErrBuf,
										SBL_WORD cbErrBuf );

SBL_STATUS			DLLENTRY SblGetThreadInfo(
										SBL_TID tid,
										SBL_LPTHREAD_INFO pThreadInfo,
										SBL_WORD cbThreadInfo);

SBL_SHORT			DLLENTRY SblVersion(void);
SBL_STATUS					DLLENTRY SblInitiate (HSBL SBL_FAR * phSb, SBL_HWND hWnd);
SBL_STATUS					DLLENTRY SblInitiateEx (HSBL SBL_FAR * phSb, SBL_HWND hWnd, SBL_SHORT cLong);
SBL_HWND				DLLENTRY	SblGetOwnerWnd(HSBL hSbl);
SBL_STATUS					DLLENTRY SblReset( HSBL hsbl);

SBL_STATUS					DLLENTRY SblTerminate (HSBL hSBL);

SBL_STATUS DLLENTRY SblSetInstanceLong(HSBL hSbl, SBL_SHORT Index, SBL_ULONG Long);
SBL_STATUS DLLENTRY SblGetInstanceLong(HSBL hSbl, SBL_SHORT Index, SBL_ULONG SBL_FAR *puLong);
#define SBL_GIL_DDEOBJECT		-3		// internal DDE object
#define SBL_GIL_CLIENTDATA		-2		// client data stuff
#define SBL_GIL_SBI				-1		// SBI handle for this instance
#define SBL_GIL_RESERVED 		 3		// number of reserved long words

	//
	// return the font used by dialogs in the specified instance
	//
SBL_STATUS DLLENTRY SblGetDialogFont(HSBL hSbl, SBL_LPSHORT pPoints, SBL_LPSTR pszFace, SBL_SHORT cbFace);
SBL_STATUS DLLENTRY SblGetDialogLong(SBL_HWND hDlg, SBL_ULONG SBL_FAR *puLong);
SBL_STATUS DLLENTRY SblSetDialogLong(SBL_HWND hDlg, SBL_ULONG Long);


SBL_STATUS DLLENTRY SblGetDlgContext( 
  SBL_HWND hDlg,
  SBL_HCONTEXT SBL_FAR * phContext);


SBL_BOOL DLLENTRY SblOleIsEnabled(HSBL hSbl);

// SblTerminate returns SBL_E_ACTIVE_THREAD if there is a 
// thread whose state is SBL_STATE_RUNNING.  The host application
// must try to close this instance of SBL later; for example, it may 
// post another WM_CLOSE message to itself.


#if defined(OS2)
SBL_SHORT			DLLENTRY SblSetEnvP( HSBL hSbl, char **pEnv);
#endif

SBL_STATUS	DLLENTRY SblGetCurrentInstance (HSBL SBL_FAR * phsbl);

SBL_STATUS	DLLENTRY SblGetClientInstanceData (
						HSBL hSBL,			// language instance whose
												//   instance data we get
						SBL_LPULONG pdw	// put instance data here
						);

SBL_STATUS	DLLENTRY SblSetClientInstanceData (
						HSBL hSBL,			// language instance to set data into
						SBL_ULONG dw		// use this instance data
						);

SBL_STATUS	DLLENTRY SblCreateThread (
						HSBL hSBL,
						SBL_LPTID pTID  /* return created thread ID here */
						);
SBL_STATUS	DLLENTRY SblCreateThreadEx (
						HSBL hSBL,
						SBL_LPTID pTID,  /* return created thread ID here */
						SBL_SHORT cLong
						);
#define SBL_GTL_HSBL				-1
#define SBL_GTL_SBI				-2
#define SBL_GTL_CLIENTDATA		-3
#define SBL_GTL_RESERVED	 	 3

SBL_STATUS	DLLENTRY SblGetThreadLong(
						SBL_TID tid,
						SBL_SHORT Index,
						SBL_ULONG SBL_FAR *puLong);
SBL_STATUS	DLLENTRY SblSetThreadLong(
						SBL_TID tid,
						SBL_SHORT Index,
						SBL_ULONG Long);
SBL_STATUS	DLLENTRY SblKillThread (SBL_TID tid);

SBL_STATUS	DLLENTRY SblSetSourcePath (
						HSBL hSBL,		// language instance whose path we set
						SBL_LPSTR pszPath		// path string to use
						);

SBL_STATUS	DLLENTRY SblGetSourcePath (
						HSBL hSBL,					// language instance whose path we get
						SBL_LPSTR pszPathBuf,	// copy into here
						SBL_SHORT cbPathBuf		// copy no more than this number of bytes, incl
														//   terminating NULL.
						);

SBL_STATUS	DLLENTRY SblSetBinaryPath (
						HSBL hSBL,				// language instance whose path we set
						SBL_LPSTR pszPath		// path string to use
						);

SBL_STATUS	DLLENTRY SblGetBinaryPath (
						HSBL hSBL,					// language instance whose path we get
						SBL_LPSTR pszPathBuf,	// copy into here
						SBL_SHORT cbPathBuf		// copy no more than this number of bytes, incl
														//   terminating NULL.
						);

SBL_STATUS	DLLENTRY SblSetCallBack (
						HSBL hSBL,								// language instance
						SBL_LPFN_CALLBACK pfnCallBack,	// callback fn
						SBL_SHORT cStmtCallBack
								// If 0 - don't call callback.
								// ElseIf 1 - call callback before each SBL statement
								// Else - call callback every cStmtCallback
								//        milliseconds.
						);

SBL_STATUS	DLLENTRY SblGetCallBack (
						HSBL hSBL,		// language instance
											// callback fn returned here
						SBL_LPFN_CALLBACK SBL_FAR * ppfnCallBack,
						SBL_SHORT SBL_FAR * pcStmtCallBack
								// If 0 - don't call callback.
								// ElseIf 1 - call callback before each SBL statement
								// Else - call callback every cStmtCallback
								//        milliseconds.
						);

SBL_STATUS	DLLENTRY SblSetErrorTextCallBack (
						HSBL hSBL,			// language instance
						SBL_LPFN_ERRCALLBACK pfnErrorTextCallBack
						);

SBL_STATUS	DLLENTRY SblGetErrorTextCallBack (
						HSBL hSBL,				// language instance
						SBL_LPFN_ERRCALLBACK SBL_FAR * ppfnErrorTextCallBack
						);

SBL_STATUS	DLLENTRY SblSetUserIOCallBack (
						HSBL hSBL,				// language instance
						SBL_LPFN_USERIOCALLBACK pfnUserIOCallBack
						);
//
// The following is useful for dealing with input from within the
// user IO callback routine.
//
#ifndef __BORLANDC__
SBL_STRING_DESCR DLLENTRY SblInputBox( 
	SBL_LPSTRING_DESCR psdPrompt, 			// prompt text
	SBL_LPSTRING_DESCR psdTitle,				// window title (if null use default)
	SBL_LPSTRING_DESCR psdDefault,			// default value (may be null)
	SBL_LPSHORT px, SBL_LPSHORT py,			// x, y position of box (may be null)
	SBL_WORD flags									// flags (see below)
	);
#endif
void DLLENTRY SblInputBoxB( 
	SBL_LPSTRING_DESCR psdResult,				// string descriptor to fill in 
	SBL_LPSTRING_DESCR psdPrompt, 			// prompt text
	SBL_LPSTRING_DESCR psdTitle,				// window title (if null use default)
	SBL_LPSTRING_DESCR psdDefault,			// default value (may be null)
	SBL_LPSHORT px, SBL_LPSHORT py,			// x, y position of box (may be null)
	SBL_WORD flags									// flags (see below)
	);
#define SBL_IBOX_NOECHO  0x01					// don't echo data

SBL_STATUS	DLLENTRY SblGetUserIOCallBack (
  HSBL hSBL,		// language instance whose callback fn info we get
  SBL_LPFN_USERIOCALLBACK SBL_FAR * ppfnUserIOCallBack
  );

SBL_STATUS DLLENTRY SblSetEmitCallBack ( 
  HSBL hSBL,		// language instance whose callback fn info we set
  SBL_LPFN_EMITCALLBACK pfnEmitCallBack
  );

SBL_STATUS DLLENTRY SblGetEmitCallBack ( 
  HSBL hSBL,		// language instance whose callback fn info we set
  SBL_LPFN_EMITCALLBACK SBL_FAR *pfnEmitCallBack
  );

SBL_STATUS	DLLENTRY SblSetDlgIndirectCallBack (
  HSBL hSBL,		// language instance whose callback fn info we set
  SBL_LPFN_DLGINDIRECT_CALLBACK pfnDlgIndirectCallBack
  );
SBL_STATUS	DLLENTRY SblGetDlgIndirectCallBack (
  HSBL hSBL,		// language instance whose callback fn info we set
  SBL_LPFN_DLGINDIRECT_CALLBACK SBL_FAR *ppfnDlgIndirectCallBack
  );

#define SBL_LI_NO_NULL_PTR_ARGS		 1
#define SBL_LI_TEMP_COOP_REGISTER	 2
#define SBL_LI_DLGMSG_SUSPEND			 4
#define SBL_LI_DLGFUN_CALLBACK		 8
#define SBL_LI_DLG_WINWORD		      16
#define SBL_LI_DLGUNITS_WINWORD		SBL_LI_DLG_WINWORD  // these two are the same
#define SBL_LI_TEMP_ADT					32
#define SBL_LI_VALIDATE_OBJS			64
#define SBL_LI_UIOCRLF				  128
#define SBL_LI_QUALSPACES			  256	// allow spaces in name qualification
#define SBL_LI_OLDBGRPNUMS			 1024 // use old style numbering of buttons in button group

SBL_STATUS	DLLENTRY SblSetInstanceFlags (  HSBL hSBL,  SBL_ULONG flags );
SBL_ULONG	DLLENTRY SblGetInstanceFlags (  HSBL hSBL );


SBL_STATUS	DLLENTRY SblLoad (
  HSBL hsbl,
  SBL_LPSTR pszFileName, /* name of module file */
  SBL_LPHMD phmd     /* returned module handle */
  );

SBL_STATUS	DLLENTRY SblRun (
  SBL_TID tid,       /* thread into which to load module */
  SBL_LPSTR pszFileName, /* name of module file */
  SBL_LPSTR pszCmdLine   /* commandline to pass to Main */
  );
SBL_STATUS	DLLENTRY SblRunObject (
  SBL_TID tid,       /* thread into which to load module */
  SBL_LPSTR pszFileName, /* name of module file */
  SBL_LPSTR pszCmdLine,  /* commandline to pass to Main */
  SBL_PVOID me
  );

// Resume codes
#define SBL_RESUME_CURRENT 1
#define SBL_RESUME_NEXT 2
		//
		// this resume code says execute at least one statement
		// before calling the call back
		//
#define SBL_RESUME_STATEMENT 3

SBL_STATUS	DLLENTRY SblResumeThread( SBL_TID tid, SBL_SHORT resumeCode );

SBL_STATUS	DLLENTRY SblUnload (
  HSBL hSbl,	    /* language instance from which to unload */
  SBL_HMD hmd       /* module handle */
);
SBL_STATUS	DLLENTRY SblUnloadByName ( HSBL hSbl, SBL_LPSTR pszFileName );

SBL_STATUS	DLLENTRY SblFindModule (
  HSBL hSbl,	        /* lg instance */
  SBL_LPSTR pszModuleName,	/* name of module (basename of file) */
  SBL_LPHMD phmd	/* returned module handle */
  );


SBL_STATUS	DLLENTRY SblGetModuleName (
  HSBL hSBL,					// language instance containing module
  SBL_HMD hmd,	// module handle
  SBL_LPSTR pszModuleName	// return module name here
  );

SBL_STATUS	DLLENTRY SblFindProc (
  HSBL hSbl,		    // language instance containing desired fn
  SBL_HMD hmd,	    // module handle
  SBL_LPSTR pszProcName,	// name of desired procedure
  SBL_LPWORD pireProc	   // index of procedure returned here
  );


SBL_STATUS	DLLENTRY SblGetProcLine (
  HSBL hSbl,					// language instance containing desired procedure
  SBL_HMD hmd,					// module handle
  SBL_WORD ire,				// index of procedure in the module's proc table
  SBL_LPWORD pLineNumber  // line number returned here
  );

// ------------- Interpreter Procedure Query and Enumeration -------------

// Flags in SBL_PROC_INFO
#define SBL_PROC_PRIVATE 1   // Private procedure (otherwise public).
#define SBL_PROC_STATIC  2   // Static procedure. 

typedef struct {
    SBL_HMD hmd;								// module handle
    SBL_SHORT ireProcedure;				// Procedure index.
    SBL_SHORT flags;
    char szName [SBL_CCH_MAX_NAME+1];	// name of procedure
    SBL_SHORT lineNumber;					// first line of procedure (1 based)
    SBL_SHORT lineNumberEnd;				// last line of procedure (1 based)

} SBL_PROC_INFO, SBL_FAR * SBL_LPPROC_INFO;
typedef SBL_PVOID SBL_HENUM_PROC, SBL_FAR * SBL_LPHENUM_PROC;

SBL_STATUS	DLLENTRY SblBeginEnumProc (
    HSBL hSBl,
    SBL_HMD hmd,				// module handle
    SBL_LPHENUM_PROC phEnum	// Address to store the enumeration handle.
);

// Continue enumerating the procedures by
// getting procedure info for next procedure.  Return 0 on
// success, negative on error, and special value SBL_E_ENUMDONE if there
// is no next procedure.
SBL_STATUS	DLLENTRY SblGetNextProc (
    SBL_HENUM_PROC hEnum,	// Returned from SblBeginEnumProc.
    SBL_LPPROC_INFO pProcInfo,  // pointer to a SBL_PROC_INFO to be filled in
				// with information about the procedure.
    SBL_SHORT cbProcInfo		// Size of the SBL_PROC_INFO buffer.
);
SBL_STATUS	DLLENTRY SblEndEnumProc( SBL_HENUM_PROC hEnum );

SBL_STATUS	DLLENTRY SblGetParameterNames(
    HSBL hSbl,
    SBL_HMD hmd,	    // module handle
    SBL_SHORT ireProcedure,	    // procedure id (resource index)
    SBL_LPBYTE pNameList,	    // A Buffer in which to store, a
                            // sequence of null-terminated names,
			    // ending with two consecutive nulls.
    SBL_WORD cbNameList         // Size of the Buffer
    );


// ------------------------------  MODULE API  -----------------------------
//
// ************************************************************************

// --------------------------- Module Structures  --------------------------
typedef SBL_PVOID SBL_HENUM_MODULE, SBL_FAR * SBL_LPHENUM_MODULE;
#define SBL_MODULE_NODATA 1
typedef struct {
    SBL_HMD	   hmd;				// module handle for type
    SBL_WORD	   flags;
    char	   szName [SBL_CCH_MAX_NAME+1];	// name of type
} SBL_MODULE_INFO, SBL_FAR * SBL_LPMODULE_INFO;


// ---------------------- Module API Function Prototypes  -------------------
//
// Begin the enumeration of loaded modules.
// Given language instance, return a ptr (handle) to use in the enumeration.
// Returns 0 if successful, non-zero otherwise.
//

SBL_STATUS DLLENTRY SblBeginEnumModule (
    HSBL		hSbl,		// Sbl language instance handle
    SBL_LPHENUM_MODULE	phEnum		// Adr to store the enumeration handle.
);

//
// Continue enumerating the loaded modules by getting module information
// for the next loaded module.
// Return 0 on success, SBL_E_ENUMDONE if there is no next module, else error.
//

SBL_STATUS	DLLENTRY SblGetNextModule (
    SBL_HENUM_MODULE	hEnum,		// Returned from SblBeginEnumModule.
    SBL_LPMODULE_INFO	pModuleInfo,	// pointer to a SBL_MODULE_INFO to be
					// filled in with information about
					// the loaded module.
    SBL_SHORT		cbModuleInfo	// Size of the SBL_MODULE_INFO buffer.
);

//
// End enumerating the loaded modules.
// Returns 0 if successful, non-zero otherwise.
//

SBL_STATUS	DLLENTRY SblEndEnumModule (
    SBL_HENUM_MODULE	hEnum		// Returned from SblBeginEnumType.
);




typedef SBL_ULONG SBL_DATATYPE, SBL_FAR * SBL_LPDATATYPE;

#define SBL_DT_TYPE_OF(dt)			LOWORD(dt)
#define SBL_DT_INDEX_OF(dt)		HIWORD(dt)

#define SBL_DT_FIXEDSTRINGLEN_OF(dt)	HIWORD(dt)
#define SBL_MAKE_DT_FIXEDSTRING(len)	MAKELONG(SBL_DATATYPE_FIXEDSTRING, len)
#define SBL_MAKE_DT_CONSTINT(val)		\
		MAKELONG(SBL_DATATYPE_CONSTINT|SBL_DATATYPE_BYVAL, val)


#define SBL_DATATYPE_ILLEGAL		((SBL_DATATYPE)0x000F)

#define SBL_DATATYPE_STRING		((SBL_DATATYPE)0x0004)
#define SBL_DATATYPE_VOID			((SBL_DATATYPE)0x0005)
#define SBL_DATATYPE_FIXEDSTRING	((SBL_DATATYPE)0x0006)
#define SBL_DATATYPE_VARIANT		((SBL_DATATYPE)0x0007)
#define SBL_DATATYPE_CONTEXT		((SBL_DATATYPE)0x0009)
#define SBL_DATATYPE_INTEGER		((SBL_DATATYPE)0x0080)
#define SBL_DATATYPE_LONG			((SBL_DATATYPE)0x0081)
#define SBL_DATATYPE_SINGLE		((SBL_DATATYPE)0x0082)
#define SBL_DATATYPE_DOUBLE		((SBL_DATATYPE)0x0083)
#define SBL_DATATYPE_CURRENCY		((SBL_DATATYPE)0x0084)
#define SBL_DATATYPE_DATE			((SBL_DATATYPE)0x0085)

#define SBL_DATATYPE_STRUCTURE	((SBL_DATATYPE)0x2000)
#define SBL_DATATYPE_SYSDIALOG	((SBL_DATATYPE)0x2002)
#define SBL_DATATYPE_ADT			((SBL_DATATYPE)0x2004)
#define SBL_DATATYPE_CLASS			((SBL_DATATYPE)0x2008)
#define SBL_DATATYPE_ANY			((SBL_DATATYPE)0x000d)    // For DLL params only
#define SBL_DATATYPE_BYVAL			((SBL_DATATYPE)0x0100)
#define SBL_DATATYPE_RHS			((SBL_DATATYPE)0x0800)

// Setting SBL_DATATYPE_OPTIONAL allows the basic programmer to skip a parameter
//	I.e. foobar(1,,3)
#define SBL_DATATYPE_OPTIONAL		((SBL_DATATYPE)0x8000)

SBL_STATUS DLLENTRY SblSetDefaultDatatype(
	SBL_DATATYPE dt						// Default datatype for autoDIM'd vars
	);

// The following special datatype is intended for use in DLL parameter
// vectors only.  It allows a constant integer to be passed as a "hidden"
// parameter.  It is intended to allow customers to develop dispatch-type
// interfaces for use by their end-users.  For example, the product developer
// might wish to create a single routine to get, set, and clear the value of
// of some object, "O".  The user would see three separate Basic functions:
// GetO, SetO, and ClrO.  These would all be mapped onto the same DLL
// entry point with different CONSTINT parameters as follows:
//
//		SBL_DATATYPE GetOargV[] = {SBL_DATATYPE_VOID, SBL_MAKE_DT_CONSTINT(0)};
//		SBL_DATATYPE SetOargV[] = {SBL_DATATYPE_VOID, SBL_MAKE_DT_CONSTINT(1)};
//		SBL_DATATYPE ClrOargV[] = {SBL_DATATYPE_VOID, SBL_MAKE_DT_CONSTINT(2)};
//...
//		SblRegisterDLLProc(hSbl, "GetO", 1, GetOargV, "p.dll", 100);
//		SblRegisterDLLProc(hSbl, "SetO", 1, SetOargV, "p.dll", 100);
//		SblRegisterDLLProc(hSbl, "ClrO", 1, ClrOargV, "p.dll", 100);
//
// Note that the ordinal (100) is the same for all three routines which means
// the same c routine will be invoked.  The product developer would then case
// on the value of the first parameter.  The BASIC programmer sees three
// different parameterless functions and is unaware of the implementation.
//
// In general, the position of CONSTINT parameters is governed by their
// position in the registration vector. CONSTINT parameters will always be 
// passed AFTER the member handle when invoking ADT member functions.  
// Several CONSTINT parameters may be defined for a single routine.  They
// may be combined with VARARGS calling conventions and follow the same
// rules as end-user specified fixed parameters.
#define SBL_DATATYPE_CONSTINT		0x1000

// The following is a special index for use in defining recursive structures
// and or ADT's.  Currently support is only provided in ADT member functions
// which may contain arguments of the same type as the ADT.  To build a
// recursive ADT use:
//					SBL_DT_ADTSELF
//
#define	SBL_DATATYPE_IDXSELF		(SBL_DATATYPE)0xFFFF
#define	SBL_DT_ADTSELF				(SBL_DATATYPE)MAKELONG(SBL_DATATYPE_ADT, SBL_DATATYPE_IDXSELF)
#define	SBL_DT_CLASSSELF			(SBL_DATATYPE)MAKELONG(SBL_DATATYPE_CLASS, SBL_DATATYPE_IDXSELF)
// NOTE:  Array datatype bits are set IN ADDITION to base datatype bits
// defined above.
#define SBL_DATATYPE_ARRAY			((SBL_DATATYPE)0x4000)
#define SBL_DATATYPE_DYNARRAY		((SBL_DATATYPE)0x0400)

#define SBL_DT_BASETYPE_OF(dt)	\
				((SBL_DATATYPE)(LOWORD(dt) & ~(SBL_DATATYPE_ARRAY | SBL_DATATYPE_DYNARRAY)))
#define SBL_DT_IS_ARRAY(dt)		\
				((BOOL)(LOWORD(dt) & (SBL_DATATYPE_ARRAY | SBL_DATATYPE_DYNARRAY)))
#define SBL_DT_IS_DYNARRAY(dt)	\
				((BOOL)(LOWORD(dt) & SBL_DATATYPE_DYNARRAY))


#define SBL_MAX_PARAMETERS 16	// Maximum number of formal parameters to a
				// procedure.
// SblGetProcPrototypes returns number of parameters on success.
SBL_STATUS	DLLENTRY SblGetProcPrototype (
  HSBL hSbl,				// language instance containing desired proc
  SBL_HMD hmd,	   // module handle
  SBL_WORD ireProcedure,	// procedure id (resource index)
  SBL_LPDATATYPE lpTypes,  // array of DATATYPES which SblGetProcPrototype
								   //   fills in with argument datatypes
                           //   (SBL_DATATYPE_... values).  Zeroth element
			                  //   is datatype of return value --
			                  //   SBL_DATATYPE_VOID for subroutines.
  SBL_SHORT cTypesMax		// size (number of slots) in lpTypes.  If not
									//   enough, error is returned.
  );

SBL_STATUS	DLLENTRY SblGetProcName (
  HSBL hSbl,					// language instance containing desired fn
  SBL_HMD hmd,				// module handle
  SBL_SHORT ire,				// index of procedure in this module's proc table
  SBL_LPSTR pszProcName    // buffer to receive procedure name -- must be
									// big enough!!
  );


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*                            V A R I A N T S                               */
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

// define the "variant id flag" type
typedef unsigned short SBL_VARTYPE;
typedef SBL_VARTYPE SBL_FAR *SBL_LPVARTYPE;
typedef unsigned short SBL_VARIANT_BOOL;

// Define the currency type
typedef char SBL_CURRENCY[8];
typedef struct {
	 SBL_LONG		Lo;
	 SBL_LONG 		Hi;
	 } SBL_CY, SBL_FAR *SBL_LPCY;

// STRINGS
typedef SBL_LPSTR SBL_BSTR, SBL_FAR *SBL_LPBSTR;

// Dates
typedef double SBL_DATE, SBL_FAR *SBL_LPDATE;

// Error codes
typedef SBL_ULONG SBL_SCODE, SBL_FAR *SBL_LPSCODE;

// This is the list of variant types. One of these constants whould be present
// in the vt field of every variant. Note: If the vt field of a variant is
// set to SBL_VT_OBJECT, Sbl expects that the vt_type_handle will contain the
// upper word of the SBL_DATATYPE for the class of the object.

enum SBL_VARENUM {
	 SBL_VT_EMPTY = 0,  // not specified					 Empty
	 SBL_VT_NULL,		  // propagating NULL				 Null
	 SBL_VT_I2,			  // two-byte signed integer		 Integer
	 SBL_VT_I4,			  // four-byte signed integer		 Long
	 SBL_VT_R4,			  // four-byte floating point		 Single
	 SBL_VT_R8,			  // eight-byte floating point	 Double
	 SBL_VT_CY,			  // currency							 Currency
	 SBL_VT_DATE,		  // date
	 SBL_VT_BSTR,		  // BSTR								 String, sorta


	 SBL_VT_DISPATCH,	  // (IDISPATCH?)
	 SBL_VT_OBJECT = SBL_VT_DISPATCH,
	 SBL_VT_ERROR,		  // error								 SCode
	 SBL_VT_BOOL,		  // boolean							 Boolean
	 SBL_VT_VARIANT,		// useful for variant FAR *
	 SBL_VT_UNKNOWN,	  // (IUNKNOWN?)

	 SBL_VT_MAX
};

// extra flag bits to be or-ed in with SBL_VARENUM
#define SBL_VT_ARRAY			0x2000
#define SBL_VT_BYREF			0x4000
#define SBL_VT_RESERVED		0x8000
#define IS_NUMERIC(vt)	(((vt >= SBL_VT_I2) && (vt <= SBL_VT_DATE)) || (vt == VT_EMPTY))


#ifdef __BORLANDC__
#define UNION_NAME u
#else
#define UNION_NAME
#endif

// The actual variant data structure!
typedef struct SBL_tagVARIANT {
	 SBL_VARTYPE	  vt;
	 SBL_WORD	 	  wReserved1;
	 SBL_WORD	 	  wReserved2;
	 SBL_WORD	 	  wReserved3;
#define vt_type_handle	wReserved3		// SBl uses reserved3 for type handles
													// This is useful for tell sbl what
													// *kind* of object is being returned.
	 union {
		  short		  iVal;					// VT_I2
		  long		  lVal;					// VT_I4
		  float		  fVal;					// VT_R4
		  double		  dVal;					// VT_R8
		  SBL_VARIANT_BOOL bool;			// VT_BOOL
		  SBL_SCODE	  scode;					// VT_ERROR
		  SBL_CY		  cyVal;					// VT_CY
		  SBL_DATE	  date;					// VT_DATE
		  SBL_BSTR	  bstrVal;				// VT_BSTR
				/* Others deleted for now */

		  short		  SBL_FAR * piVal;		// VT_I2 | VT_BYREF
		  long		  SBL_FAR * plVal;		// VT_I4 | VT_BYREF
		  float		  SBL_FAR * pfVal;		// VT_R4 | VT_BYREF
		  double		  SBL_FAR * pdVal;		// VT_R8 | VT_BYREF
		  SBL_VARIANT_BOOL SBL_FAR * pbool;	// VT_BOOL | VT_BYREF
		  SBL_SCODE	  SBL_FAR * pscode;		// VT_ERROR | VT_BYREF
		  SBL_CY		  SBL_FAR * pcyVal;		// VT_CY | VT_BYREF
		  SBL_DATE	  SBL_FAR * pdate;		// VT_DATE | VT_BYREF
		  SBL_BSTR	  SBL_FAR * pbstrVal;	// VT_BSTR | VT_BYREF
				/* Others deleted for now */

		  void		  SBL_FAR * byref;		 // Generic byref - including objects
		  }UNION_NAME;
	 } SBL_VARIANT, SBL_FAR *SBL_LPVARIANT;
typedef SBL_VARIANT SBL_VARIANTARG, SBL_FAR *SBL_LPVARIANTARG;

// This error is stuffed into variants as scode (vt is set to VT_ERROR) when
// a parameter is omitted. I.e. call foo(a, , c)
#define SBL_DISP_E_PARAMNOTFOUND		0x80020004
#define SBL_DISP_E_PARAM_NOT_FOUND		SBL_DISP_E_PARAMNOTFOUND

/*
 * All the above typedefs are made to be SBLized. The following defines make
 * unSBLized versions.
 *
 * If the preprocessor symbol NO_OLE_EQUIVALENTS is defined then unSBLized
 * versions of the names are not defined.  The sbl integrator should define
 * this symbol if they include OLE2.
 *
 * As an added feature, we'll check to see if the ole files were included
 * before this file and set NO_OLE_EQUIVALENTS automagically if they were.
 */
#ifdef _VARIANT_H_		// defined by ole2 in variant.h
#define NO_OLE_EQUIVALENTS
#endif

#ifdef _COMPOBJ_H_		// defined by ole2 in compobj.h
#define NO_OLE_EQUIVALENTS
#endif

#ifndef NO_OLE_EQUIVALENTS

#define VARENUM					SBL_VARENUM
#define VARIANT					SBL_VARIANT
#define LPVARIANT					SBL_LPVARIANT
#define VARIANTARG				SBL_VARIANTARG
#define LPVARIANTARG				SBL_LPVARIANTARG
#define CURRENCY 					SBL_CURRENCY
#define CY							SBL_CY
#define LPCY						SBL_LPCY
#define VARTYPE					SBL_VARTYPE
#define LPVARTYPE					SBL_LPVARTYPE
#define VARIANT_BOOL				SBL_VARIANT_BOOL
#define BSTR						SBL_BSTR
#define LPBSTR						SBL_LPBSTR
#define SCODE						SBL_SCODE
#define LPSCODE					SBL_LPSCODE
#define DATE						SBL_DATE
#define LPDATE						SBL_LPDATE
#define VT_EMPTY					SBL_VT_EMPTY
#define VT_NULL					SBL_VT_NULL
#define VT_I2						SBL_VT_I2
#define VT_I4						SBL_VT_I4
#define VT_R4						SBL_VT_R4
#define VT_R8						SBL_VT_R8
#define VT_CY						SBL_VT_CY
#define VT_DATE					SBL_VT_DATE
#define VT_BSTR					SBL_VT_BSTR
#define VT_VARIANT				SBL_VT_VARIANT
#define VT_DISPATCH				SBL_VT_DISPATCH
#define VT_ERROR					SBL_VT_ERROR
#define VT_BOOL					SBL_VT_BOOL
#define VT_IUNKNOWN				SBL_VT_IUNKNOWN
#define VT_MAX						SBL_VT_MAX
#define VT_ARRAY					SBL_VT_ARRAY
#define VT_BYREF					SBL_VT_BYREF
#define VT_RESERVED				SBL_VT_RESERVED

#define DISP_E_PARAMNOTFOUND	SBL_DISP_E_PARAMNOTFOUND

#endif
// ----------------------- SBL Compiler API -----------------------
// Error severity levels.
#define SBL_COMPILER_OK      10
#define SBL_COMPILER_WARNING 20
#define SBL_COMPILER_ERROR   30

typedef	SBL_STATUS (DLLENTRYPTR SBL_ERRPROC)(SBL_WORD severity, SBL_STATUS errnum, SBL_SHORT line,
   SBL_LPSTR pszFile, SBL_LPSTR pszErr);

typedef SBL_ERRPROC SBL_FAR *SBL_LPERRPROC;
typedef SBL_SHORT (DLLENTRYPTR SBL_IMPLICITPROC) (SBL_LPSTR pszModBaseName);
typedef SBL_IMPLICITPROC SBL_FAR *SBL_LPIMPLICITPROC;

typedef struct {
	SBL_LPSTR pszName;
	SBL_DATATYPE type;
	} SBL_MEMBER,SBL_FAR *SBL_MEMBERLIST;	// Structure member
						// (SblRegisterAppDialogBox)
typedef void (DLLENTRYPTR SBL_CALLBACKPROC)(void);	// Callback function
						// (SblCompile)

SBL_STATUS DLLENTRY SblCompile (
  HSBL hSBL,								// language instance from which we compile
  SBL_LPSTR   pszSrcFileName,			// sourcefilename
  SBL_LPSTR   pszTargetFileName,		// target filename
  SBL_ERRPROC pErrorProc,				// client's procedure for error reporting
  SBL_IMPLICITPROC pImplicitProc,	// client's procedure for telling us
												// whether or not we can assume that
												// a string is the name of a module we
												// run implicitly
  SBL_LPSTR   pszOptions,				// compiler option switches
  SBL_CALLBACKPROC pCallbackProc		// Procedure to be called at start of
												// compile for temporary registrations
  );

SBL_STATUS DLLENTRY SblCompileClass (
  HSBL hSBL,								// language instance from which we compile
  SBL_LPSTR   pszSrcFileName,			// sourcefilename
  SBL_LPSTR   pszTargetFileName,		// target filename
  SBL_ERRPROC pErrorProc,				// client's procedure for error reporting
  SBL_IMPLICITPROC pImplicitProc,	// client's procedure for telling us
												// whether or not we can assume that
												// a string is the name of a module we
												// run implicitly
  SBL_LPSTR   pszOptions,				// compiler option switches
  SBL_CALLBACKPROC pCallbackProc,	// Procedure to be called at start of
												// compile for temporary registrations
  SBL_DATATYPE classType				// Compile this basic program as an
												// instantiable module based on this
												// class.
  );

SBL_STATUS	DLLENTRY SblRegisterDLLProc(
						HSBL hSbl,
						SBL_LPSTR pszFunctionName,
						SBL_SHORT cArgs,
						SBL_LPDATATYPE pTypes,
						SBL_LPSTR pszDLLNAme,
						SBL_SHORT ordinal);

// Flags for SblRegisterDLLProcEx
#define SBL_DECL_PASCAL		0x00				// Pascal calling convention
#define SBL_DECL_CDECL		0x01				// CDECL calling convention
#define SBL_DECL_VARARGS	0x03				// CDECL with variable # args
#define SBL_DECL_VECTOR		0x07				// as above with type vector
#define SBL_STRONG_OVERLOAD 0x08				// permit builtin overloading
#define SBL_DECL_COOPCOMP	0x10				// allow cooperative compilation
#define SBL_DECL_TEMP_ADT	0x20				// generate a temporary ADT
#define SBL_DECL_CMPBINARY	0x40				// use with OPTION COMPARE BINARY
#define SBL_DECL_CMPTEXT	0x80				// use with OPTION COMPARE TEXT
SBL_STATUS	DLLENTRY SblRegisterDLLProcEx(
						HSBL hSbl,
						SBL_WORD flags,
						SBL_LPSTR pszFunctionName,
						SBL_SHORT cArgs,
						SBL_LPDATATYPE pTypes,
						SBL_LPSTR pszDLLNAme,
						SBL_SHORT ordinal);

SBL_STATUS DLLENTRY SblRegisterDLLProcEx2(
						HSBL hsbl, 
						SBL_WORD flags,
						SBL_LPSTR pszFunctionName,
						SBL_SHORT cArgs,
						SBL_LPDATATYPE pTypes,
						SBL_LPSTR pszDLLNAme,
						SBL_SHORT ordinal,
						SBL_LPSTR pszAlias);

SBL_STATUS	DLLENTRY SblRegisterAppDialogBox(
						HSBL hSbl,
						SBL_LPSTR pszDialogName,
						SBL_SHORT cMembers,
						SBL_MEMBERLIST aMembers,
						SBL_SHORT idDialog);

typedef SBL_LPSTR	SBL_FAR * SBL_ADTFNS;

typedef struct {
	SBL_LPSTR		pszMember;	// name of member
	SBL_WORD			flags;		// must be zero - for future use
	SBL_DATATYPE	type;			// use SBL_DATATYPE_*
	SBL_WORD			hMember;		// 16-bit value of client's choice
} SBL_ADTMEMBER, SBL_FAR * SBL_LPADTMEMBER;

typedef struct {
	SBL_LPSTR		pszFunction;	// name of function
	SBL_WORD			flags;			// passed to SblRegisterDLLProcEx
	SBL_SHORT		cArgs;			// number of arguments to function
	SBL_LPDATATYPE	pTypes;			// adr of array containing types
											//  of the return value and formal
											//  parameters.  [0] is return.
	SBL_SHORT		ordinal;			// ordinal of function within DLL
} SBL_ADTMEMFUNC, SBL_FAR * SBL_LPADTMEMFUNC;


typedef SBL_ULONG SBL_HADT;
typedef SBL_LPULONG SBL_LPHADT;

/*
** The following are the prototypes for the Create, Get, Set, and Assign
** functions that the application will define.  These prototypes better
** match the functions that the client supplies or else !!!
*/

// Flags for the wContext arg of the ADTCREATE function
#define SBL_ADT_LOAD		0x0100
#define SBL_ADT_CREATE	0x0200
#define SBL_ADT_DESTROY	0x0400
#define SBL_ADT_GLOBAL	0x0001
#define SBL_ADT_MODULE	0x0002
#define SBL_ADT_LOCAL	0x0004

typedef int (DLLENTRYPTR SBL_ADTCREATE) (
						SBL_WORD wContext,
						SBL_LPHADT phADT,
						SBL_ULONG dwVersion);
typedef int (DLLENTRYPTR SBL_ADTGET) (
						SBL_LPHADT phADT,
						SBL_WORD hMember,
						SBL_PVOID pMem);
typedef int (DLLENTRYPTR SBL_ADTSET) (
						SBL_LPHADT phADT,
						SBL_WORD hMember,
						SBL_PVOID pMem);
typedef int (DLLENTRYPTR SBL_ADTASSIGN) (
						SBL_LPHADT phADTDest,
						SBL_LPHADT phADTSource);

SBL_DATATYPE	DLLENTRY SblRegisterADT(
	HSBL					hSBL,			// SBL session handle
	SBL_LPSTR			szADTname,	// ADT name, e.g. PARAGRAPH
	SBL_ULONG			dwVersion,	// version number or whatever
	SBL_SHORT			cMembers,	// # of members
	SBL_LPADTMEMBER	aMembers,	// member list struct
	SBL_SHORT			cMemFuncs,	// # of member functions
	SBL_LPADTMEMFUNC	aMemFuncs,	// member function array
	SBL_SHORT			cProcs,		// ADTs use 5 client procs
	SBL_ADTFNS			aProcs		// list of client's procs
	);

SBL_STATUS	DLLENTRY SblDumpModule ( HSBL hsbl, SBL_LPSTR pszFileName );


// The following routine will take a date and pick it apart,
// rendering all the component pieces.
void DLLENTRY SblDissectDate(
	SBL_DATE date2rip,
	SBL_LPWORD year,
	SBL_LPWORD month,
	SBL_LPWORD day,
	SBL_LPWORD weekday,
	SBL_LPWORD yearday,
	SBL_LPWORD yearweek,
	SBL_LPWORD hour,
	SBL_LPWORD minute,
	SBL_LPWORD second);

// SblBuildDate will take all the little bits and pieces of a date
// and package them up into a date-value. The return value is 0 for
// success, 1 for error condition.
int DLLENTRY SblBuildDate(
	SBL_SHORT year,
	SBL_SHORT month,
	SBL_SHORT day,
	SBL_SHORT hours,
	SBL_SHORT minutes,
	SBL_SHORT seconds,
	SBL_LPDATE lpret_double);

// ----------------------- SBL Evaluation Kit API -----------------------
SBL_STATUS	DLLENTRY SblEvaluationKitCompile( SBL_LPSTR pszFilename);

SBL_STATUS	DLLENTRY SblEvaluationKitRun( SBL_LPSTR pszFilename);


#ifndef SBL_NOERRORS
// All untrappable runtime SBL error numbers are in the range of -1000
//  to -8999.
// All trappable runtime SBL error numbers are in the range of 1 to 999.
// All SBL compiler warning numbers are in the range of -9000 to -9999.
// ------------------ Interpreter Errors --------------------------------

// Interpreter errors in the range 1-1000 are numbered and worded
// to match VB.  So be careful!!

#define SBL_E_USER_BREAK					-1001	// User Interrupt - not used
																	// internally by SBL.DLL (for
																	// customer convenience).
#define SBL_E_NOMEM							 7		// Out of memory.
#define SBL_E_TYPE_ERROR					 13		// Generic "type mismatch"
#define SBL_E_NO_RESUME						 19		// No resume.
#define SBL_E_INTERNAL_ERROR				 51		// Internal Error
#define SBL_E_BADFILENUM					 52		// Bad file name or number.
#define SBL_E_FILENOTFOUND					 53		// File not found.
#define SBL_E_BADFILEMODE					 54		// Bad file mode.
#define SBL_E_ALREADYOPEN					 55		// File already open.
#define SBL_E_FILEEXISTS					 58		// File already exists.
#define SBL_E_BADRECLEN						 59		// bad record length
#define SBL_E_DISK_FULL						 61		// Disk Full
#define SBL_E_INPUTPAST_EOF				 62		// input went past end-of-file
#define SBL_E_BADRECNUM						 63		// Bad record number.
#define SBL_E_BADFILENAME					 64		// Bad file name.
#define SBL_E_TOOMANYFILES					 67
#define SBL_E_BADDEVICE						 68		// Device unavailable
#define SBL_E_PERMISSION_DENIED			 70		// permission denied
#define SBL_E_DISKNOTREADY					 71		// Disk not ready
#define SBL_E_NOTSAMEDEVICE				 74		// Can't rename with different drive.
#define SBL_E_ACCESSDENIED					 75		// Path/File access error.
#define SBL_E_PATHNOTFOUND					 76		// Path not found.
#define SBL_E_OBJECT_VAR_NOT_SET			 91		// obj var not set
#define SBL_E_INVALID_PATTERN				 93		// Invalid Pattern
#define SBL_E_ILLEGAL_NULL					 94		// Illegal use of null

// The following set of errors came from WinWord. Hopefully VB will not 
// reuse these numbers.

#define SBL_E_CONTROLNOTFOUND			140	 // Control not found.
#define SBL_E_BADCONTROL   			141	 // This statement is not available 
													    // for this control
#define SBL_E_SETFOCUS   	         144    // Focus may not be set to a hidden
														 // or disabled control

#define SBL_E_DIALOG_ACTION4			161	 // Can't bring up a dialog during
														 // action 4
#define SBL_E_NOACTIVEDLG				163	 // no active dialog

// this one is not from WW...
#define SBL_E_DLGNESTING				170	 // dialog nesting too deep.
#define SBL_E_MISSING_PICTURE			171	 // missing picture

#define SBL_E_OBJECT_CREATION_FAIL		 429		// couldn't create an object
#define SBL_E_DYNBIND_FAILED				 438	 	// the name didn't bind
#define SBL_E_DYNBIND_TYPE_MISMATCH		 439		// a type problem with a param
#define SBL_E_DYNBIND_MISC_ERROR			 440		// myterious dynbind error
#define SBL_E_BUFFERTOOBIG					 901	// i/o buffer would be too big
#define SBL_E_OSERR							 902	// Operating system error
#define SBL_E_ERROR							-1003	// ERROR exception was triggered
																	// and not handled via ON ERROR.
#define SBL_E_BADOPCODE						-1004	// Invalid OP Code FATAL!!!
#define SBL_E_HALT							-1005	// Halted Pcode interpreter.
#define SBL_E_CMDFAILED						 102	// CancelButton
#define SBL_E_DIV0							 11		// Divide by 0.
#define SBL_E_FLOAT_OVERFLOW				 6		// Floating Overflow.
#define SBL_E_DOUBLE_OVERFLOW				 6		// Double Overflow.
#define SBL_E_FLOAT_UNDERFLOW				-1009	// Floating underflow.
#define SBL_E_FLOAT_ERROR					-1010	// Floating point error.
#define SBL_E_INTEGER_OVERFLOW			 6		// Integer Overflow.
#define SBL_E_LONG_OVERFLOW				 6		// Long Overflow.
#define SBL_E_SBLIO							-1012	// IO error in SBL file system.
#define SBL_E_SBLFILEFORMAT				-1013	// Bad file format.
#define SBL_E_STACK_OVERFLOW				 28		// Stack overflow.
#define SBL_E_TOOMANYMODULES				-1015	// have already loaded or ref'd max
																	// number of modules.
#define SBL_E_MODULENOTFOUND				-1016	// cannot find module by name
#define SBL_E_PROCNOTFOUND					-1017	// cannot find procedure
#define SBL_E_XPROCNOTFOUND				 903	// external proc not found
#define SBL_E_CANTUNLOAD					-1018	// cannot unload this module
																	// (procedures from it are in use).
#define SBL_E_DLLLOAD						 48		// cannot load specified dll
#define SBL_E_DLLGETPROCADDRESS			 35		// cannot get addr of specified
																	// function in DLL.
#define SBL_E_BADTID							-1020	// bad thread ID
#define SBL_E_UNEXPECTEDNULL				-1021	// unexpected null pointer
#define SBL_E_NOTREADYTID					-1022	// tid is busy or in error state
#define SBL_E_BADARG							 5		// Illegal function call
#define SBL_E_NOERROR						-1028	// No Error (for SblGetErrorInfo)
#define SBL_E_STRINGTOOBIG					 14		// String length > 32767
#define SBL_E_NOSUCHPARM					-1041	// no such parm for proc
#define SBL_E_BADRETTYPE					-1042	// bad return type for proc
#define SBL_E_BADARGTYPE					-1043	// bad argument type for proc
#define SBL_E_BADNUMARGS					-1044	// Wrong number of args for proc
#define SBL_E_VARNOTFOUND					-1045	// variable not found.
#define SBL_E_VARINFO_SIZE					-1046	// Invalid SBL_VAR_INFO buffer size.
#define SBL_E_PROCINFO_SIZE				-1047	// Invalid SBL_PROC_INFO buffer size.
#define SBL_E_CALLFRAMEINFO_SIZE			-1048	// Invalid SBL_CALLFRAME_INFO bufsize.
#define SBL_E_NOPUSHBUTTON					 907	// no pushbutton in dlg box
#define SBL_E_BADCSTMTCALLBACK			-1051	// bad cStmtCallBack to SblSetCallBack.
#define SBL_E_BADMODULEID					-1053	// bad hmd -- module not loaded?
#define SBL_E_MODINFO_SIZE					-1054	// Invalid SBL_MODULE_INFO buffer size.
#define SBL_E_BADFLAGS						-1055	// Bad flags passed in type call
#define SBL_E_BADNESTLVL					-1056	// Bad nest_lvl passed in
#define SBL_E_NOMODULEDATA					-1057	// Module data not initialized
#define SBL_E_GLOBALMISMATCH				 904	// global mismatch at LoadMod or
																	// SblCreateGlobalVar time
#define SBL_E_TYPEMISMATCH					 905	// user-defined type mismatch at
																	// LoadMod or SblCreateGlobal time
#define SBL_E_PROCMISMATCH					 906	// proc interface mismatch at
																	// LoadSblResource time
#define SBL_E_ADTMISMATCH					 909	// ADT version mismatch as LoadMod
																	// time
#define SBL_E_DLGNOTDECLARED				 910	// Dialog box not declared
#define SBL_E_TYPESPACE						-1070	// type array given to
																	// SblGetProcPrototype
																	// wasn't big enough.
#define SBL_E_VERFILESYS					-1071
#define SBL_E_ENUMDONE						-1072
#define SBL_E_MEMBERINFO_SIZE				-1073	// Invalid SBL_MEMBER_INFO buffer size
#define SBL_E_TYPEINFO_SIZE				-1074	// Invalid SBL_TYPE_INFO buffer size
#define SBL_E_TYPENOTFOUND					-1075	// Type not found
#define SBL_E_ARRAYINFO_SIZE				-1076	// Invalid SBL_ARRAY_INFO buffer size
#define SBL_E_UNLOADUNDERFLOW				-1081
#define SBL_E_NOMAIN							 908
#define SBL_E_CALLBACK_SUSPEND      	-1082
#define SBL_E_SUSPEND						-1083	// Suspend the thread
#define SBL_E_ALLDONE						-1084	// Internal error
#define SBL_E_RELOADPROC					-1085	// Error re-loading SBL code we
																	// were running; either after
																	// leave, or
#define SBL_E_RESUME_NO_ERROR				 20		// Resume statement executed when
																	// not in error condition.
																	// (suspended is a special
																	// error state).
#define SBL_E_BUFF_SIZE						-1090	// Invalid Buffer size.
#define SBL_E_ASSERTION						-1098
#define SBL_E_CANTLOADMODULE				-1099
#define SBL_E_BREAKPOINT					-1100	// Code passed to Callback when
																	// a breakpoint occurs.
#define SBL_E_NO_BREAKPOINT				-1101	// Error returned by
																	// SblGet/SetBreakPoint
#define SBL_E_LINE_NOT_EXECUTABLE		-1102	// Error from SblSetBreakPoint
#define SBL_E_UNUSED_BREAKPOINT			-1103	// SblGetBreakPoint returns this to
																	// indicate unused table entry.
#define SBL_E_ACTIVE_THREAD 				-1104 // SblTerminate returns this 
														   // to indicate that there is
															// an active thread belonging
															// to this instance.

// ------------------ Compiler Errors ----------------------------------
#define SBL_E_ILLEGAL_NUM					-2000
#define SBL_E_YACC_ERROR					-2001
#define SBL_E_UNKNOWN_FN					-2002
#define SBL_E_UNKNOWN_FN_IMPLICIT		-9001	// warn, but assume implicit run
																	// of MAIN in some external module.
#define SBL_E_UNDEFINED_VAR				-9002	// only if -w1 in sblcompile.
#define SBL_E_FN_USED_AS_SUB				-9000
#define SBL_E_IF_ALWAYS_TRUE				-9003
#define SBL_E_IF_ALWAYS_FALSE				-9004
#define SBL_E_COMP_INCOMPAT_OBJECTS		-9005
#define SBL_E_CLASS_MEMBER_OBSOLETE		-9006
#define SBL_E_DEFAULT_MEMBER_OBSOLETE	-9007
#define SBL_E_CLASSMETHODERROR			-9008
#define SBL_E_SUB_USED_AS_FN				-2004
#define SBL_E_ARG_TYPE_BY_REF				-2022
#define SBL_E_MISSING_ARGS_IN_PROC		-2023
#define SBL_E_UNKNOWN_VAR					-2025
#define SBL_E_MISSING_GOTO_TARGET		-2030
#define SBL_E_BAD_CONVERSION_STR_NUM	-2040
#define SBL_E_REENTER_COMPILE				-2050
#define SBL_E_DIV_NEEDS_NUMS				-2060
#define SBL_E_INTDIV_NEEDS_NUMS			-2061
#define SBL_E_MINUS_NEEDS_NUM				-2062	// Unary
#define SBL_E_MINUS_NEEDS_NUMS			-2063	// Binary
#define SBL_E_PLUS_NEEDS_NUMS				-2064
#define SBL_E_MUL_NEEDS_NUMS				-2065
#define SBL_E_MOD_NEEDS_NUMS				-2066
#define SBL_E_AND_NEEDS_NUMS				-2067
#define SBL_E_OR_NEEDS_NUMS				-2068
#define SBL_E_XOR_NEEDS_NUMS				-2069
#define SBL_E_EQV_NEEDS_NUMS				-2070
#define SBL_E_IMP_NEEDS_NUMS				-2071
#define SBL_E_OP_MISMATCH_LT				-2080
#define SBL_E_OP_MISMATCH_GT				-2081
#define SBL_E_OP_MISMATCH_LE				-2082
#define SBL_E_OP_MISMATCH_GE				-2083
#define SBL_E_OP_MISMATCH_EQ				-2084
#define SBL_E_OP_MISMATCH_NE				-2085
#define SBL_E_INV_METACMD					-2090
#define SBL_E_INV_CHAR						-2091
#define SBL_E_UNCAUGHT						-2100
#define SBL_E_USING_NOT_IMPL				-2102
#define SBL_E_BAD_TYPE_STMT				-2103
#define SBL_E_USER_TYPES_NOT_IMPL		-2104
#define SBL_E_BAD_DECL						-2105
#define SBL_E_ILLEGAL_STMT					-2107
#define SBL_E_VAR_TYPE_UNKNOWN			-2108
#define SBL_E_VAR_TYPE_CONFLICT			-2109
#define SBL_E_VAR_REDEF						-2110
#define SBL_E_TYPE_NOT_DECL				-2111
#define SBL_E_OBJ_DLL_NOT_DECL			-2112
#define SBL_E_VAR_UNDEF						-2113
#define SBL_E_PARM_TYPE_UNKNOWN			-2114
#define SBL_E_PARM_REDEF					-2115
#define SBL_E_DIMSHARED						-2116
#define SBL_E_INV_SUB_TYPE					-2117
#define SBL_E_INV_FN_TYPE					-2118
#define SBL_E_ASGN_STRUCT_FLD_NOT_IMPL	-2120
#define SBL_E_ILLEGAL_LVAL					-2121
#define SBL_E_LABEL_REDEF					-2122
#define SBL_E_TYPE_REDEF					-2123
#define SBL_E_TYPE_MEMBER_REDEF			-2124
#define SBL_E_NOT_A_STRUCT					-2125
#define SBL_E_NOT_A_STRUCT_FIELD			-2126
#define SBL_E_FUNC_TYPE_CONFLICT			-2127
#define SBL_E_TOOMANYRECORDTYPES			-2150	// too many RecordTypes referenced
#define SBL_E_TOOMANYDLLS					-2151	// too many DLLs referenced
#define SBL_E_TOOMANYDLLFUNCS				-2152	// too many DLL functions referenced
#define SBL_E_TOOMANYCONSTS				-2153	// too much constant space
#define SBL_E_TOOMANYPROCS					-2154	// too many procedures defined.
#define SBL_E_TOOMUCHLOADDATA				-2155	// > 64k of load data
#define SBL_E_TOOMUCHSHAREDDATA			-2156	// > 64k of shared data
#define SBL_E_TOOMUCHBASE					-2157	// module base > 64k
#define SBL_E_TOOMUCHDEBUGDATA			-2158	// >64k of debug data
#define SBL_E_TOOMANYMODULEVARS			-2159	// too many module-level variable defs
#define SBL_E_TOOMANYEXTERNALMODULES	-2160	// too many external modules referenced
#define SBL_E_TOOMANYLOCALVARS			-2161	// too many local-level variable defs
#define SBL_E_TOOMANYGLOBALVARS			-2162	// too many global-level variable defs
#define SBL_E_TOOMANYXFUNCS				-2163	// too many External Functions
#define SBL_E_NLCREATE						-2164	// NameList Error Creation
#define SBL_E_NLINSERT						-2165	// NameList Error Insert
#define SBL_E_NLFREE							-2166	// NameList Error Free
#define SBL_E_NAME_TRUNC					-2200
#define SBL_E_INCL_TOO_NESTED				-2240
#define SBL_E_OPEN_INCL						-2241
#define SBL_E_BAD_ESCAPE					-2242
#define SBL_E_SYMTAB_FULL					-2243
#define SBL_E_CREATE_OUTPUT				-2244
#define SBL_E_WRITE_OUTPUT					-2245
#define SBL_E_BAD_INPUT						-2248
#define SBL_E_FIELD_TYPE					-2249
// Dialog errors:
#define SBL_E_DIALOG_ERROR					-2250
#define SBL_E_NOT_A_DIALOG					-2251
#define SBL_E_BEGIN_ERROR					-2252
#define SBL_E_END_ERROR						-2253
#define SBL_E_DIALOG_DEF_ERROR			-2254
#define SBL_E_UNKNOWN_CONTROL				-2255
#define SBL_E_OK_ERROR						-2256
#define SBL_E_CANCEL_ERROR					-2257
#define SBL_E_GROUPBOX_ERROR				-2258
#define SBL_E_CHECKBOX_ERROR				-2259
#define SBL_E_OPTIONGROUP_ERROR			-2260
#define SBL_E_OPTIONBUTTON_ERROR			-2261
#define SBL_E_TEXT_ERROR					-2262
#define SBL_E_TEXTBOX_ERROR				-2263
#define SBL_E_BUTTONGROUP_ERROR			-2264
#define SBL_E_BUTTON_ERROR					-2265
#define SBL_E_LISTBOX_ERROR				-2266
#define SBL_E_COMBOBOX_ERROR				-2267
#define SBL_E_BUTTON_BEFORE_GROUP		-2268
#define SBL_E_OPTION_BEFORE_GROUP		-2269
#define SBL_E_REDEF_BUTTONGROUP			-2270
#define SBL_E_POSITION_MUST_BE_INT		-2271
#define SBL_E_CONTROL_FIELD_EXISTS		-2272
#define SBL_E_DIALOG_REDEF					-2273
#define SBL_E_TEXT_MUST_BE_STR			-2274
#define SBL_E_NOT_A_SYS_DIALOG			-2275
#define SBL_E_NOT_SAME_RECORD_TYPE		-2276
#define SBL_E_CAPTION_MUST_BE_STR		-2277
#define SBL_E_CAPTION_ERROR				-2278
#define SBL_E_REDEF_CAPTION				-2279
#define SBL_E_MISSING_COLON				-2280
#define SBL_E_MSG								-2281	// diagnostic message
#define SBL_E_INVALID_ALIAS				-2282
#define SBL_E_INVALID_LIB					-2283
#define SBL_E_BAD_RESUME					-2284
#define SBL_E_BAD_ON_ERROR					-2285
#define SBL_E_INVALID_TYPE					-2286
#define SBL_E_INVALID_AS					-2287
#define SBL_E_BAD_GETCURVALUES			-2288
#define SBL_E_BAD_FOR						-2289
#define SBL_E_BAD_SEEK						-2290
//oops
#define SBL_E_BAD_GOTO						-2292
#define SBL_E_BAD_EXIT						-2293
#define SBL_E_WEND_WITHOUT_WHILE			-2294
#define SBL_E_NEXT_WITHOUT_FOR			-2295
#define SBL_E_ENDIF_WITHOUT_IF			-2296
#define SBL_E_ELSE_WITHOUT_IF				-2297
#define SBL_E_ELSEIF_WITHOUT_IF			-2298
#define SBL_E_BAD_LET						-2299
#define SBL_E_BAD_CALL						-2300
#define SBL_E_BAD_EXP_INT					-2301
#define SBL_E_BAD_EXP_BOOL					-2302
#define SBL_E_BAD_EXP_STRING				-2303
#define SBL_E_NO_STRING_END				-2377
#define SBL_E_UNEXPECTED_EOF				-2304
#define SBL_E_PROC_TOO_BIG					-2305
#define SBL_E_MISSING_END_TYPE			-2306
#define SBL_E_LOOP_WITHOUT_DO				-2307
#define SBL_E_BAD_IF							-2308
#define SBL_E_BAD_ELSEIF					-2309
#define SBL_E_MISSING_END_IF				-2310
#define SBL_E_BAD_SELECT					-2311
#define SBL_E_MISSING_END_SELECT			-2312
#define SBL_E_BAD_SELECT_EXP				-2313
#define SBL_E_EXIT_DO_WITHOUT_DO			-2314
#define SBL_E_BAD_ENVIRON					-2315
#define SBL_E_BAD_MID						-2316
#define SBL_E_BAD_INSTR						-2317
#define SBL_E_BAD_SHELL						-2318
#define SBL_E_BAD_RND						-2319
#define SBL_E_BAD_STRING					-2320
#define SBL_E_BAD_MSGBOX					-2321
#define SBL_E_BAD_INPUTBOX					-2322
#define SBL_E_EXIT_FOR_WITHOUT_FOR		-2323
#define SBL_E_TOO_MANY_ARGS				-2330
#define SBL_E_TOO_FEW_ARGS					-2331
#define SBL_E_ILLEGAL_BYVAL				-2334
#define SBL_E_TYPE_MISMATCH				-2335
#define SBL_E_WRONG_RET_TYPE				-2336
#define SBL_E_SUBS_HAVE_NO_TYPE			-2337
#define SBL_E_NAME_CONFLICT				-2340
#define SBL_E_GSD_NOT_REGISTERED			-2341
#define SBL_E_SYSDLG_MISMATCH				-2342
#define SBL_E_PROTO_AFTER_DEF				-2343
#define SBL_E_SUB_REDEF						-2344
#define SBL_E_PROTO_MISMATCH				-2345
#define SBL_E_SUB_FUNC_MISMATCH			-2346
#define SBL_E_NO_MODULE_BYVALS			-2347
#define SBL_E_MAIN_OR_INIT					-2348
#define SBL_E_MISSING_PROC					-2349
#define SBL_E_IMPROPER_RESERVED_WORD	-2350
#define SBL_E_USERTYPE_EXPRESSION		-2360
#define SBL_E_ADT_EXPRESSION				-2376
#define SBL_E_BAD_OBASE_LOC				-2361
#define SBL_E_BAD_OBASE_VAL				-2362
#define SBL_E_BAD_ARRAY_BOUND				-2363
#define SBL_E_BAD_ARRAY_DIM				-2364
#define SBL_E_BAD_ARRAY_REF				-2365
#define SBL_E_NOT_ARRAY						-2366
#define SBL_E_BAD_ARRAY_NDIMS				-2367

#define SBL_E_ARRAY_TOO_BIG				-2368
#define SBL_E_NO_SUBSCRIPTS				-2369
#define SBL_E_BAD_ERASE						-2370

#define SBL_E_BAD_REDIM						-2371
#define SBL_E_NOT_DYNAMIC					-2372
#define SBL_E_REDIM_TYPE					-2373
#define SBL_E_ARRAY_BYVAL					-2374
#define SBL_E_BAD_SUBSCRIPTS				-2375
#define SBL_E_SUB_OUTOFRANGE				 9
#define SBL_E_DUPDEF							 10
#define SBL_E_CANNOT_PRINT					-2380
#define SBL_E_CANNOT_INPUT					-2381
#define SBL_E_FIXED_STRING_TOO_LONG		-2382
#define SBL_E_NO_FIXED_STRING_PARAMS	-2383
#define SBL_E_INVALID_FOR_VAR				-2384
#define SBL_E_BAD_OPEN						-2385
#define SBL_E_OPEN_NO_FOR					-2421
#define SBL_E_BAD_SYSDLG_CALL				-2386
#define SBL_E_CANNOT_WRITE					-2387
#define SBL_E_BAD_WRITE						-2388
#define SBL_E_BAD_INPUT_ST					-2389
#define SBL_E_BAD_PROC						-2390
#define SBL_E_ILLEGAL_BYREF				-2391
#define SBL_E_LITERAL_TOO_LONG			-2392
#define SBL_E_SYNTAX							-2393
#define SBL_E_NOT_CONSTANT					-2394
#define SBL_E_CONST_BAD_TYPE				-2395
#define SBL_E_DEFTYPE_ERROR				-2400
#define SBL_E_DEFTYPE_ORDER				-2401
#define SBL_E_DEFTYPE_DUPLICATE			-2402
#define SBL_E_DEFTYPE_LETTER				-2403
#define SBL_E_LEN_VAR_REQUIRED			-2404
#define SBL_E_TYPE_EMPTY					-2405
#define SBL_E_FIXED_STRING_TOO_SHORT	-2406
#define SBL_E_RECURSIVE_TYPE				-2407
#define SBL_E_ILLEGAL_REDEF				-2408
#define SBL_E_ADT_MEM_AS_FUNC				-2409
#define SBL_E_ADT_FUNC_AS_MEM				-2410
#define SBL_E_CONST_ASSIGN					-2411
#define SBL_E_MISSING_NEXT					-2412
#define SBL_E_BAD_FOR_NEST					-2413
#define SBL_E_WRONG_NEXT					-2414
#define SBL_E_RESUME_NOT_0					-2415
#define SBL_E_MID_NO_ADTS					-2416
#define SBL_E_VAR_OVERFLOW					-2417
#define SBL_E_NO_RET_TYPE					-2418
#define SBL_E_BAD_BYVAL						-2419
#define SBL_E_ILLEGAL_ANY					-2420
#define SBL_E_NOTARRAY_FUNC				-2422
#define SBL_E_FOR_VAR_REUSED				-2423
#define SBL_E_BAD_CONST						-2424
#define SBL_E_ADT_FIELD						-2425
#define SBL_E_BAD_OPTION			      -2426
#define SBL_E_DUPLICATE_OPTION			-2427

#define SBL_E_ILLEGAL_OPERATION			-2430
#define SBL_E_ILLEGAL_CONVERSION			-2431
#define SBL_E_ILLEGAL_DEFAULT_TYPE		-2432

#define SBL_E_REPORTED 						-2433
#define SBL_E_TOOMANYQUALS					-2434
#define SBL_E_FUNC_ASSIGN					-2435

#define SBL_E_TOO_MANY_PARENTS			-2436
#define SBL_E_ILLEGAL_PARENT				-2437
#define SBL_E_NOT_A_CLASS					-2438
#define SBL_E_CONFLICT_RET_TYPE			-2439
#define SBL_E_DUPLICATE_METHOD			-2440
#define SBL_E_DLLNAMETOOLONG				-2441
#define SBL_E_BAD_RESERVED_METHOD_NAME	-2443
#define SBL_E_NO_RESERVED_METHOD_PARAMS_ALLOWED -2444
#define SBL_E_RESERVED_METHOD_NOT_CDECL -2445
#define SBL_E_RESERVED_METHOD_BAD_FUNCKIND -2446
#define SBL_E_CLASS_STRINGS_TOO_BIG		 -2447
#define SBL_E_CLASS_NOT_IN_INSTANCE		 -2448
#define SBL_E_OLD_VERSION_CLASS			 -2449
#define SBL_E_CLASS_INCOMPATABLE			 -2450
#define SBL_E_INVALID_FOR_CONTEXT		 -2451
#define SBL_E_BADREQUEST					 -2452
#define SBL_E_ARRAY_EXPRESSION          -2453
#define SBL_E_BADCOMPILECLASS           -2454
#define SBL_E_MODULEDOESNOTNEED_ME      -2455
#define SBL_E_MODULEREQUIRES_ME         -2456
#define SBL_E_MODULECLASSNEEDSGETINSTANCEDATA  -2457
#define SBL_E_MODULECLASSNEEDSPUTINSTANCEDATA  -2458
#define SBL_E_INVALID_OBJ_REF				-2459
#define SBL_E_CANTWRITEME					-2460
#define SBL_E_TYPEOF_NONOBJ				-2461
#define SBL_E_NO_TYPEOF_FOR_CLASS		-2462
#define SBL_E_BADCONTEXT					-2463
#define SBL_E_NO_NEW_FOR_CLASS			-2464
#define SBL_E_STR_NOT_A_CLASS				-2465
#define SBL_E_PARENT_REGISTERED_CHILDLESS	-2466
#define SBL_E_ILLEGAL_OBJECT_ASSIGNMENT	-2467
#define SBL_E_NOSETPARAM					-2468
#define SBL_E_LSET_TYPE_EMBEDDED			-2469
#define SBL_E_OBJECT_USE					-2470
#define SBL_E_ADT_USE						-2471
#define SBL_E_PUT_IMPL						-2472
#define SBL_E_EXPECTED_VAR					-2473		// expected variable
#define SBL_E_BAD_FUNC						-2474
#define SBL_E_BAD_EXP_LONG					-2475
#define SBL_E_SIMPLECONSTONLY				-2476
#define SBL_E_NOLOCALVAR					-2477
#define SBL_E_NOT_A_CLASS_MEMBER			-2478
#define SBL_E_CONFLICT_CALL_CONV			-2479
#define SBL_E_VIRTUAL_NONVIRTUAL_MIXED	-2480
#define SBL_E_VIRTUAL_SLOTS_MIXED		-2481
#define SBL_E_ILLEGAL_PROPORMETHOD_VALUE -2482
#define SBL_E_PROPERTYSET_NEEDS_1PARAM -2483
#define SBL_E_PROPERTYSET_MUST_BE_VOID	-2484
#define SBL_E_NODLLINFO						-2485
#define SBL_E_NODEFAULTMEMBER				-2486
#define SBL_E_NEWFAILED						-2487
#define SBL_E_ILLEGALCALLBACK				-2488
#define SBL_E_NOCOOPAPPDIALOG				-2489
#define SBL_E_PROPSETNEEDS1RHS			-2490
#define SBL_E_PROPGETNEEDS0RHS			-2491
#define SBL_E_APPLICATIONERROR			-2492
#define SBL_E_PERM_REG_USES_TEMP_THING	-2493
#define SBL_E_NOOBJECTBYREF				-2494
#define SBL_E_OBSOLETE_ENTRY_POINT		-2495
#define SBL_E_QNAN_OVERFLOW				-2496
#define SBL_E_BAD_ULONG_INDEX				-2497
#define SBL_E_DLGFUNCUNDEFINED			-2498
#define SBL_E_PICTURE_ERROR			 	-2499
#define SBL_E_DROPLIST_ERROR			 	-2500
#define SBL_E_NO_UNREGISTER_NOW			-2501
#define SBL_E_OLE2CLASSSTRINGSMUSTBEBYVAL -2502
#define SBL_E_NOBYVALVARIANTS				-2503
#define SBL_E_BADPARAMTYPE					-2504
#define SBL_E_DEFVARMUSTBEOBJECT			-2505
#define SBL_E_NOTVALIDFORTHISCONTEXT	-2506
#define SBL_E_TOOMANYPARAMETERS			-2507
#define SBL_E_ILLEGALCALLCONVFOROLE2	-2508
#define SBL_E_NONOLE2CLASS					-2509
#define SBL_E_CSTRINGS_NESTING			-2510
#define SBL_E_CSRESTORE_NO_SAVE			-2511
#define SBL_E_BAD_WITH			         -2512
#define SBL_E_OPTCMP_NOT_COMPAT			-2532
#define SBL_E_DYNAMIC_DIALOG_ENDED		-2522

#endif // SBL_NOERRORS


// The following #defines are used to id strings in the resource file. sbl.rc
// includes sbl.h, so this is cool. Do not move these to another file. Customers
// need to be able to rebuild sbl.res in the sdk directory. The sdk directory
// contains sbl.h and no other headers.
#define IDS_TYPESTR					200
#define IDS_TIMEBOMB   				210
#define IDS_MSGBOX_CAPTION  		220
#define IDS_DLGBOX_CAPTION 		221
#define IDS_INPUTBOX_CAPTION		222
#define IDS_DLGFONT					223
#define IDS_DLGFONTSIZE				224
#define IDS_PBOK						225
#define IDS_PBCANCEL					226
#define IDS_PBABORT					227
#define IDS_PBRETRY					228
#define IDS_PBIGNORE					229
#define IDS_PBYES						230
#define IDS_PBNO						231
#define IDS_PBX						232
#define IDS_PBY						233
#define IDS_ICONSTOP					234
#define IDS_ICONQUESTION			235
#define IDS_ICONWARNING				236
#define IDS_ICONINFO					237
#define IDS_CCHMAXLIST				238
#define IDS_MSGBOXNULL				239
#define IDS_DEFDLLEXT				250
#define IDS_DEFSBXEXT				251
#define IDS_OLE2_ENABLE				252
#define IDS_OLE2_CLASSNAME			253
#define IDS_OLE2_CREATEOBJECT		254
#define IDS_OLE2_GETOBJECT			255
#define IDS_MAX_NESTING 			256
#define IDS_OLE2_BADRETTYPE		257
#define IDS_NO_CLIPBOARD			258
#define IDS_NO_DDE					259

// These IDS's are used to define error strings that are in the resource file,
// but are not errors in their own right. For instance, if an illegal class
// ID is passed into createobject, the error code is SBL_E_OBJECT_CREATION_FAIL,
// but the error text is IDS_OLE2_BAD_CLSID
#define IDS_OLE2_BAD_CLSID				300
#define IDS_OLE2_CREATE_NOMEM			301
#define IDS_OLE2_MONIKER_SYNTAX		302
#define IDS_OLE2_NOOBJECT				303
#define IDS_OLE2_ACCESS					304
#define IDS_OLE2_FILENOTFOUND			305
#define IDS_OLE2_TOOMANYOPENFILES	306
#define IDS_OLE2_SHAREVIOLATION		307
#define IDS_OLE2_SHAREREQUIRED		308
#define IDS_OLE2_NOIDISPATCH			309
#define IDS_ASSERT_GPFAULT				310

// ------------------ Interpreter Debugging API ------------------------
#ifndef SBL_NODEBUG

typedef struct
    {
    SBL_HMD hmd;
    SBL_WORD line;
    } SBL_BREAKPOINT, SBL_FAR * SBL_LPBREAKPOINT;

// flags for SblSetBreakPoint
#define SBL_BP_SET 0
#define SBL_BP_DELETE 1
#define SBL_BP_CAN_ADJUST 2

SBL_STATUS	DLLENTRY SblGetBreakPoint( HSBL hsbl, SBL_SHORT ixBreakPoint,
		SBL_LPBREAKPOINT pBreakPoint, SBL_SHORT cBreakPoint );
SBL_STATUS	DLLENTRY SblSetBreakPoint( HSBL hsbl, SBL_SHORT ixBreakPoint,
		SBL_LPBREAKPOINT pBreakPoint, SBL_SHORT cBreakPoint, SBL_WORD flags );


typedef struct {
    char		szModuleName [SBL_CCH_MAX_NAME+2];	// name of Module   + NULL
    char 	szProcName [SBL_CCH_MAX_NAME+2];		// name of Proc + NULL
    char 	szName    [SBL_CCH_MAX_NAME+2];		// name of variable + NULL
    SBL_HMD hmd;											// module handle
																// for module variables.
    SBL_SHORT 		ire;									// Index into system resource
																// table for local variables.
    SBL_DATATYPE	type;									// SBL_DATATYPE_* of variable
    SBL_SHORT		flags;								//	SBL_ENUM_??? (see below)
    SBL_WORD		wVarFlags;							// SBL_VARF_??? (see below)
    SBL_SHORT		nest_lvl;							// Nesting Level
    SBL_PVOID 		pData;								// pointer to data
} SBL_VAR_INFO, SBL_FAR * SBL_LPVAR_INFO;

typedef SBL_PVOID SBL_HENUM_VAR, SBL_FAR * SBL_LPHENUM_VAR;

// Flags for SblBeginEnumVar - formal,local,module and global can all be
// specified simultaneously, and they will be returned in this order.
#define SBL_ENUM_LOCAL       0x0001    // Local variables.
#define SBL_ENUM_ALL_LOCALS (0x0002 | SBL_ENUM_LOCAL) // Get Nested Locals too

#define SBL_ENUM_MODULE      0x0004    // Module variables in current module.

#define SBL_ENUM_GLOBAL      0x0008    // Global variables visible to the
										         // specified module.

#define SBL_ENUM_FORMAL      0x0010    // Local variables.
#define SBL_ENUM_ALL_FORMAL (0x0020 | SBL_ENUM_FORMAL)//Get Nested Formals too

#define SBL_ENUM_CONST	     0x0100    // Enumerate constants along with
													// variables in specified scope.  If
													// not specified, only variables will
													// be enumerated

#define SBL_ENUM_MODGLOBAL   0x0200    // Enumerate globals defined within

// Flags in wVarFlags
#define SBL_VARF_REGISTERED  0x0001		// Variable was registered by
													// application

SBL_STATUS	DLLENTRY SblBeginEnumVar (
    SBL_TID tid,			// Thread in which we're running (for local vars).
								// Also used for module vars (for hSbl).
    SBL_HMD hmd,			// module handle
    SBL_WORD flags,		   // Flags.
    SBL_SHORT nest_lvl,	   // Call frame nesting level, or 0 for all frames
    SBL_LPHENUM_VAR phEnum // Address to store the enumeration handle.
);
SBL_STATUS DLLENTRY SblBeginEnumVarObject(
	SBL_TID tid,		// Thread in which we're running (for local vars).
							// Also used for module vars (for hSbl).
	SBL_HMD hmd,
	SBL_WORD flags,			// Flags.
	SBL_SHORT nest_lvl,	// Call frame nesting level, or 0 for all frames
	SBL_LPHENUM_VAR phEnum,	// Address to store the enumeration handle.
	SBL_PVOID me);			// pointer to "me"

// Continue enumerating the variables by
// getting variable info for next variable.  Return 0 on
// success, negative on error, and special value SBL_E_ENUMDONE if there
// is no next variable.
SBL_STATUS	DLLENTRY SblGetNextVar (
    SBL_HENUM_VAR hEnum,		// Returned from SblBeginEnumVar.
    SBL_LPVAR_INFO pVarInfo,	// pointer to a SBL_VAR_INFO to be filled in
										// with information about the variable.
    SBL_SHORT cbVarInfo			// Size of the SBL_VAR_INFO buffer.
);
SBL_STATUS	DLLENTRY SblEndEnumVar( SBL_HENUM_VAR hEnum );

// Given the name of a variable, get its.
// SBL_VAR_INFO.  The caller puts the name is put into the name field of the
// SBLVARINFO before calling.  Return 0 on success, neg on error.  It is
// an error if there is no variable matching the given name.
SBL_STATUS	DLLENTRY SblGetVarInfo (
    SBL_TID tid,					// Thread in which we're running (for local vars).
										// Also used for module vars (for hSbl).
    SBL_HMD hmd,					// module handle
    SBL_LPSTR pszVar,			// Variable Name.
    SBL_WORD flags,				// Flags.
    SBL_SHORT nest_lvl,			// Call frame nesting level, or 0 for current
									//  frame
    SBL_LPVAR_INFO pVarInfo,	// pointer to a SBL_VAR_INFO to be filled in
									// with information about the variable.
    SBL_SHORT cbVarInfo			// Size of the SBL_VAR_INFO buffer.
);
SBL_STATUS	DLLENTRY SblGetVarInfoObject(
    SBL_TID tid,					// Thread in which we're running (for local vars).
										// Also used for module vars (for hSbl).
    SBL_HMD hmd,					// module handle
    SBL_LPSTR pszVar,			// Variable Name.
    SBL_WORD flags,				// Flags.
    SBL_SHORT nest_lvl,			// Call frame nesting level, or 0 for current
									//  frame
    SBL_LPVAR_INFO pVarInfo,	// pointer to a SBL_VAR_INFO to be filled in
									// with information about the variable.
    SBL_SHORT cbVarInfo,		// Size of the SBL_VAR_INFO buffer.
    SBL_PVOID me					// pointer to "me"
);

SBL_STATUS DLLENTRY SblGetGlobal (HSBL hSbl,
			   SBL_LPSTR pszVarName,
			   SBL_LPVAR_INFO pVI,
			   SBL_WORD VISize);

SBL_STATUS	DLLENTRY SblCreateGlobal (
    HSBL hSbl,						// Sbl language instance handle
    SBL_LPSTR pszVar,			// Variable or constant name
    SBL_DATATYPE type,			// SBL_DATATYPE_* of variable
    SBL_PVOID pValue,			// initial value to assign
    SBL_BOOL bConst					// TRUE if creating a constant, FALSE for a var
);

#define SBL_CGE_BCONST		0x01
#define SBL_CGE_NOENUM		0x02
SBL_STATUS	DLLENTRY SblCreateGlobalEx (
    HSBL hSbl,						// Sbl language instance handle
    SBL_LPSTR pszVar,			// Variable or constant name
    SBL_DATATYPE type,			// SBL_DATATYPE_* of variable
    SBL_PVOID pValue,			// initial value to assign
    SBL_WORD wFlags					// TRUE if creating a constant, FALSE for a var
);

SBL_STATUS DLLENTRY SblFreeGlobal(
	HSBL	  hSbl,			// Sbl language instance handle
	SBL_LPSTR   pszName      // name of the global to free.
);

SBL_STATUS DLLENTRY SblFreeAllGlobals(HSBL hSbl);


// this macro is here for compatibility with old API
#define SblCreateGlobalVar(a,b,c,d)	SblCreateGlobal(a,b,c,d,FALSE)

// This function locates the SBL_DATATYPE of the type given by pszName

SBL_STATUS DLLENTRY SblLocateType(HSBL hSbl, 	   // Sbl language instance handle
								   SBL_LPSTR pszName, // Datatype name
									SBL_LPDATATYPE pType);	// pointer to the value 
									                        // to fill

// ---------------------------  Call Frame API  --------------------------

// ------------------------ Call Frame Structures  ------------------------
typedef SBL_PVOID SBL_HENUM_CALLFRAME, SBL_FAR * SBL_LPHENUM_CALLFRAME;

typedef struct {
    char szModuleName [SBL_CCH_MAX_NAME+2];   // Name of module
    char szProcName [SBL_CCH_MAX_NAME+2];     // Name of procedure
    SBL_HMD hmd;                              // Module handle
    SBL_WORD ire;                             // Resource (procedure) index
    SBL_WORD line;                            // Current line of code within
                                              //  source module (1-based).
    SBL_SHORT nest_lvl;                       // Nesting Level of this proc

} SBL_CALLFRAME_INFO, SBL_FAR * SBL_LPCALLFRAME_INFO;

// ------------------- Call Frame API Function Prototypes  ---------------

// Begin the enumeration of call frames for a thread.  Given a thread,
// returns a ptr (handle) to use in the enumeration.
// Returns 0 if successful, non-zero otherwise.
SBL_STATUS	DLLENTRY SblBeginEnumCallFrame (
    SBL_TID		tid,					// Thread to enumeratve call frames in
    SBL_LPHENUM_CALLFRAME phEnum	// Addr to store the enumeration handle.
);

// Continue enumerating the call frames within a thread by getting call frame
// information for next call frame.  Return 0 on success, negative on error,
// and special value SBL_E_ENUMDONE if there is no next call frame.
SBL_STATUS	DLLENTRY SblGetNextCallFrame (
    SBL_HENUM_CALLFRAME  hEnum,				// Returned by SblBeginEnumCallFrame
    SBL_LPCALLFRAME_INFO pCallFrameInfo,	// ptr to a SBL_CALLFRAME_INFO to be
														// filled in with information about
														// the call frame
    SBL_SHORT		cbCallFrameInfo			// Size of the SBL_CALLFRAME_INFO buf
);

// End enumerating the call frames within a thread.
// Returns 0 if successful, non-zero otherwise.
SBL_STATUS	DLLENTRY SblEndEnumCallFrame (
    SBL_HENUM_CALLFRAME	hEnum		// Returned by SblBeginEnumCallFrame
);



// ------------------------------  Type API  -----------------------------
//
// --------------------------- Type Structures  --------------------------
typedef SBL_PVOID SBL_HENUM_TYPE, SBL_FAR * SBL_LPHENUM_TYPE;

typedef struct {
    char	   	szName [SBL_CCH_MAX_NAME+1];	// name of type
    SBL_HMD	   hmd;									// module handle for type
    SBL_SHORT	iType;								// type index of type
    SBL_WORD	cMembers;							// number of types
    SBL_WORD	flags;								// scope flags
	 SBL_WORD	wDefFlags;							// definition flags
} SBL_TYPE_INFO, SBL_FAR * SBL_LPTYPE_INFO;
#define SBL_TYPF_REGISTERED	0x0001			// type was registered
#define SBL_TYPF_USERDIALOG	0x0002			// type is user defined dialog
#define SBL_TYPF_APPDIALOG		0x0004			// type is app defined dialog

typedef struct {
    char	   		szName [SBL_CCH_MAX_NAME+1];	// name of member
    SBL_DATATYPE  type;									// datatype of member
    SBL_WORD	   offset;								// offset of data within struct
} SBL_MEMBER_INFO, SBL_FAR * SBL_LPMEMBER_INFO;

// ---------------------- Type API Function Prototypes  ----------------------
// Begin the enumeration of types within a module.  Given language instance
// and module handle returns a ptr (handle) to use in the enumeration.
// Returns 0 if successful, non-zero otherwise.
SBL_STATUS	DLLENTRY SblBeginEnumType (
    HSBL					hSbl,				// Sbl language instance handle
    SBL_HMD			hmd,					// module handle
    SBL_WORD         flags,         // Flags SBL_(ENUM_MODULE ENUM_GLOBAL)
    SBL_LPHENUM_TYPE hEnum				// Adr to store the enumeration handle.
);

// Continue enumerating the types within a module by getting type information
// for next type.  Return 0 on success, negative on error, and special value
// SBL_E_ENUMDONE if there is no next type.
SBL_STATUS	DLLENTRY SblGetNextType (
    SBL_HENUM_TYPE	hEnum,			// Returned from SblBeginEnumType.
    SBL_LPTYPE_INFO	pTypeInfo,		// pointer to a SBL_TYPE_INFO to be
												// filled in with information about
												// the type.
    SBL_SHORT		cbTypeInfo			// Size of the SBL_TYPE_INFO buffer.
);

// End enumerating the types within a module.
// Returns 0 if successful, non-zero otherwise.
SBL_STATUS	DLLENTRY SblEndEnumType (
    SBL_HENUM_TYPE	hEnum				// Returned from SblBeginEnumType.
);

// Locate a type within a module based upon its name and return its
// type table index.
// Returns 0 if successful (type found), non-zero otherwise.
SBL_STATUS	DLLENTRY SblFindType (
    HSBL				hSbl,				// Sbl language instance handle
    SBL_HMD			hmd,				// module handle of module to search
    SBL_WORD   	flags,         // Flags SBL_(ENUM_MODULE ENUM_GLOBAL)
    SBL_LPSTR		pszTypeName,	// ptr to name of type to be found
    SBL_LPSHORT	piTypeTable		// ptr to int to receive type index
);

// Given a type index and a module handle return information about a type.
// Returns 0 if information successfully returned, non-zero otherwise.
SBL_STATUS	DLLENTRY SblGetTypeInfo (
    HSBL					hSbl,			// Sbl language instance handle
    SBL_HMD				hmd,			// handle of module containing type
    SBL_WORD			flags,      // Flags SBL_(ENUM_MODULE ENUM_GLOBAL)
    SBL_SHORT			iTypeTable,	// index of type in type table
    SBL_LPTYPE_INFO	pTypeInfo,	// pointer to a SBL_TYPE_INFO to be
											// filled in with information about
											// the type.
    SBL_SHORT			cbTypeInfo	// size of SBL_TYPE_INFO structure
);

// Given a type index and a module handle return information about a types'
// members.
// Returns 0 if information successfully returned, non-zero otherwise.
SBL_STATUS	DLLENTRY SblGetTypeMembers (
    HSBL						hSbl,				// Sbl language instance handle
    SBL_HMD					hmd,				// handle of module containing type
    SBL_WORD            flags,			// Flags SBL_(ENUM_MODULE ENUM_GLOBAL)
    SBL_SHORT				iTypeTable,		// index of type in type table
    SBL_LPMEMBER_INFO	pMemberInfo,	// ptr to array of SBL_MEMBER_INFO
													// structures to receive information
													// about type's members
    SBL_SHORT				cbMemberInfo	// total size of pMemberInfo array
													// should be:
													//  cMembers*sizeof(SBL_MEMBER_INFO)
);
//
//	get the value of a member of an ADT
//
SBL_STATUS DLLENTRY SblGetAdtMember(
		HSBL hSbl,				// Sbl language instance handle
		SBL_HMD hmd,			// handle of module
		SBL_WORD flags,		
		SBL_SHORT iType,		// type index of ADT
		SBL_LPHADT phAdt,		// pointer to ADT handle
		SBL_WORD hMember,		//index of member
		SBL_PVOID pMember);	// pointer to return buffer

//
//	set the value of a member of an ADT
//
SBL_STATUS DLLENTRY SblSetAdtMember(
		HSBL hSbl,				// Sbl language instance handle
		SBL_HMD hmd,			// handle of module
		SBL_WORD flags,		
		SBL_SHORT iType,		// type index of ADT
		SBL_LPHADT phAdt,		// pointer to ADT handle
		SBL_WORD hMember,		//index of member
		SBL_PVOID pMember);	// pointer to return buffer

// Defines a new type.
// Returns type index if successful, negative otherwise.
SBL_DATATYPE	DLLENTRY SblRegisterType (
    HSBL						hSbl,				// Sbl language instance handle
    SBL_LPSTR				pszTypeName,	// ptr to name of type to define
    SBL_SHORT				cMembers,		// number of members of type
    SBL_LPMEMBER_INFO	pMemberInfo		// ptr to array of SBL_MEMBER_INFO
													// structures containing definitions
													// of each member
);

// ------------------------------  Array API  ----------------------------
//
// --------------------------- Array Structures  --------------------------
typedef SBL_PVOID	SBL_LPARRAY_DESCR;

// Arrays are row-major...
typedef struct {
    SBL_SHORT		cDims;		// number of dimensions
    SBL_WORD	   cel;			// total number of elements
										// in array (all dimensions)
    SBL_WORD	   cbElSize;	// size of each element in bytes
    SBL_PVOID	   pData;		// ptr to array data
} SBL_ARRAY_INFO, SBL_FAR * SBL_LPARRAY_INFO;

// Array bounds.
typedef struct {
    SBL_SHORT		lb;			// lower bound (SIGNED!!!)
    SBL_SHORT		ub;			// upper bound
}  SBL_ARRAY_BOUNDS, SBL_FAR * SBL_LPARRAY_BOUNDS;

// --------------------- Array API Function Prototypes  ----------------------
//
// Given array descriptor return information about the array.
SBL_STATUS	DLLENTRY SblGetArrayInfo (
    SBL_LPARRAY_DESCR	pADesc,		// pointer to array descriptor
    SBL_LPARRAY_INFO		pAInfo,		// pointer to SBL_ARRAY_INFO structure
												// to receive information about array
    SBL_SHORT				cbAInfo		// size of SBL_ARRAY_INFO structure
);

SBL_STATUS	DLLENTRY SblGetArrayBounds (
    SBL_LPARRAY_DESCR	pADesc,		// pointer to array descriptor
    SBL_LPARRAY_BOUNDS	pBounds,		// ptr to array of SBL_ARRAY_BOUNDS
												// structures to receive bounds
    SBL_SHORT			cBounds			// number of elements in pBounds
												// array
);

SBL_STATUS	DLLENTRY SblGetArrayElement (
    SBL_LPARRAY_DESCR	pADesc,			// pointer to array descriptor
    SBL_SHORT SBL_FAR *		pSubscripts,	// ptr to array of int's containing
													// subscripts (from left to right)
    SBL_SHORT			cSubscripts,		// number of subscripts supplied
    SBL_PVOID SBL_FAR *	ppEl					// ptr to location to receive far
													// ptr to array element
);

// given an array descriptor, change the dimension of a dynamic array
SBL_STATUS DLLENTRY SblRedim(
    HSBL          		hSbl,			// language instance handle
    SBL_LPARRAY_DESCR	pad,			// pointer to array descriptor
    SBL_LPARRAY_BOUNDS	dimens,		// bounds for each dimension
    SBL_WORD 				numDimens,	// number of dimensions
    SBL_WORD 				flags);		// REDIM_PRESERVE or 0

#define REDIM_PRESERVE	1


// -----------------------  Register Var/Const API  -----------------------
//
// ------------------------- Var/Const Structures  ------------------------
typedef SBL_WORD SBL_SCOPE;

#define SBL_SCOPE_GLOBAL	((SBL_WORD)0x0001)
#define SBL_SCOPE_MODULE	((SBL_WORD)0x0002)
#define SBL_SCOPE_LOCAL		((SBL_WORD)0x0004)
#define SBL_SCOPE_DELETED	((SBL_WORD)0x0008)	// for SblUnregisterType

// the following are provided for compatibility with previous releases of SBL
#define SCOPE_GLOBAL	SBL_SCOPE_GLOBAL
#define SCOPE_MODULE	SBL_SCOPE_MODULE
#define SCOPE_LOCAL	SBL_SCOPE_LOCAL
// ------------------- Var/Const Function Prototypes  ---------------------
SBL_STATUS	DLLENTRY SblRegisterConst (
    HSBL          hSbl,			// language instance handle
    SBL_LPSTR		pszName,		// constant name
    SBL_DATATYPE	type,			// type of constant
    SBL_SCOPE		scope,		// SCOPE_GLOBAL or SCOPE_MODULE
    SBL_PVOID		pValue		// pointer to constant value
);

SBL_STATUS	DLLENTRY SblRegisterVar (
    HSBL          hSbl,			// language instance handle
    SBL_LPSTR		pszName,		// constant name
    SBL_DATATYPE	type,			// type of constant
    SBL_SCOPE		scope			// SCOPE_GLOBAL or SCOPE_MODULE
);

// -------------- Set the default variable for an instance -------------------
SBL_STATUS DLLENTRY SblSetDefaultVariable(
	HSBL hsbl,
	SBL_LPSTR name);

#endif // SBL_NODEBUG




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//------------------------- O B J E C T   A P I -------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 * The enumeration SBL_CALLBACKTYPE gives all the possible uses for
 * registering a call back routine with sbl via the entrypoint
 * SblRegisterCallbacks.
 */
typedef enum tagSBL_CALLBACKTYPE {
	SBL_CALLBACK_COOPREG = 0,		// cooperative register
	SBL_CALLBACK_COOPCOMP,			// cooperative compilation - for special methods
	SBL_NOTIFY_THREAD_CREATE,		// notification for threadcreate
	SBL_NOTIFY_MODULE_LOAD,			// notification for module load
	SBL_NOTIFY_MODULE_UNLOAD,		// notification for module unload
	SBL_CALLBACK_TRY_INVOKE,		// invoke a method that may throw an exception
	SBL_CALLBACK_ENUMSIZE,			// for internal SBL use only
	SBL_NOTIFY_FILESYS_REPLACED,	// file system has been replaced
} SBL_CALLBACKTYPE, SBL_FAR *SBL_LPCALLBACKTYPE;
	//
	// the following signature is for routines that handle these notifies:
	// 	SBL_NOTIFY_THREAD_CREATE
	// 	SBL_NOTIFY_MODULE_LOAD
	// 	SBL_NOTIFY_MODULE_UNLOAD
	//
typedef SBL_STATUS (DLLENTRYPTR SBL_LPNOTIFY_ROUTINE)(SBL_CALLBACKTYPE cbt,
														SBL_HCONTEXT hContext);

/*
 * the following typedef's give the proper prototypes for the various callback
 * functions.
 */

/*
 * SBL_GLOBAL_COOP_REGISTER is the prototype for a global cooperative
 * registration routine. The return value should be SBL_REGISTER_ASKAGAIN,
 * SBL_REGISTER_OK or SBL_REGISTER_NOPE (depending on whether the callback
 * routine called one of sbl's register functions to register the name in
 * question).
 */
typedef int DLLENTRY SBL_GLOBAL_COOP_REGISTER
				(SBL_LPSTR pszName,		// name of thing that needs registering
				 SBL_WORD sbl_coop_flags,// bit-wise or of SBL_COOP_* flags
				 SBL_HCONTEXT hcontext);// sbl context handle
typedef SBL_GLOBAL_COOP_REGISTER SBL_FAR *SBL_LPGLOBAL_COOP_REGISTER;

// return values for a SBL_GLOBAL_COOP_REGISTER routine
#define SBL_REGISTER_ASKAGAIN 2 // did not register - but ask again if
										// identifier is seen again
#define SBL_REGISTER_OK		1	// the name was register via a SBL api call
#define SBL_REGISTER_NOPE	0	// the name was not registered

// values for sbl_coop_flags in above prototype
#define SBL_COOP_TYPE		1	// the name was used in a the context of a type
#define SBL_COOP_VARIABLE	2	// the name was used in the context of a variable

// The cooperative compile routine is called whenever a specially flagged
// method or function is encountered.  It provides context information and
// a vector of variants which describe the arguments.  See cooperative
// compilation write-up for more details.
typedef struct tagCOOPCOMP_CONTEXT {
	SBL_LPSTR 	szClassName;	// name of class/adt of which this is a member
										//  qualified list.  NULL for simple function.
	SBL_DATATYPE classType;		// The datatype of the class/adt of which this
										//  is a member
	SBL_LPSTR	szFuncName;		// name of function
} SBL_COOPCOMP_CONTEXT, SBL_FAR *SBL_LPCOOPCOMP_CONTEXT;

typedef SBL_LPVARIANT SBL_FAR *SBL_LPCOOPCOMP_ARGS;

typedef void DLLENTRY SBL_COOP_COMPILE(
			SBL_LPCOOPCOMP_CONTEXT	lpCtx,		// Cooperative Compile Context
			SBL_WORD						cArgs,		// number of arguments
			SBL_LPCOOPCOMP_ARGS		lpArgs,		// Argument information
			SBL_HCONTEXT				hcontext		// context for SblContext use
			);
typedef SBL_COOP_COMPILE SBL_FAR *SBL_LPCOOP_COMPILE;


// The try and invoke routine is called before invoking methods that may throw
// an exception. The try and invoke routine should execute a try block to that
// it can catch any exception that may be thrown. The T&I routine should then
// use SblDispath to invoke the method (passing along the lpCallInfo pointer).
// If an exception is caught, the lpException structure should be filled in and
// a value of 1 should be returned.
//
// If no exception is caught, 0 should be returned.
typedef struct tagSBL_EXECPTION{
	unsigned short wCode;			// set either this or the scode field
	unsigned short wReserved;
///////////////////////////////////////////////////////////////////
// THE NEXT THREE FIELDS SHOULD BE FILLED IN WITH OLE2 BSTR's.
// DO NOT USE SBL BSTR FUNCTIONS TO ALLOCATE THE STRINGS FOR THESE
// FIELDS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///////////////////////////////////////////////////////////////////
	BSTR bstrSource;					// human readable name of source of error
	BSTR bstrDescription;			// human readable description of error
	BSTR bstrHelpFile;				// path name to help file
	unsigned long dwHelpContext;	// help file context index
	void SBL_FAR * pvReserved;

	// this field may be set to point to a routine that is called to fill in
	// the string fields. See Ole2 automation doc.
	void SBL_FAR* pfnDeferredFillIn;

	SCODE scode;						// set either this or the wCode field
} SBL_EXCEPTION, SBL_FAR * SBL_LPEXCEPTION;

typedef int DLLENTRY SBL_TRY_INVOKE(
			SBL_PVOID			lpCallInfo,		// pass on to SblInvoke
			SBL_LPEXCEPTION 	lpException);	// pointer to exception structure
typedef SBL_TRY_INVOKE *SBL_FAR SBL_LPTRY_INVOKE;


/*
 * SBL_CALLBACK defines a structure to register several callbacks at once.
 * It also makes it easy to add new callbacks in future releases without adding
 * new API. The one drawback is that the pointer to the routine is declare
 * as "void *" (since each routine has a different prototype, it's impossible
 * to initialize an array of SBL_CALLBACKs with just calling everything void *),
 * so that there is no prototype checking by the compiler.
 */
typedef struct tagSBL_CALLBACK {
	SBL_CALLBACKTYPE	callbacktype;	// what's this callback for
	void SBL_FAR *routine;				// routine to call
} SBL_CALLBACK, SBL_FAR *SBL_LPCALLBACK;



/*
 * SBL_DLL_ENTRY defines a single entry into a dll. Note: ordinal is ignored
 * except when entryName is NULL.
 */
typedef struct tagSBL_DLL_ENTRY {
	SBL_LPSTR 	szDllName;		// name of dll
	SBL_LPSTR 	szEntryName;	// entry point name
	SBL_WORD		ordinal;			// oridnal of entry point
} SBL_DLL_ENTRY, SBL_FAR *SBL_LPDLL_ENTRY;



/*
 * SBL_FUNCKIND specifies the binding used for a function
 */
typedef enum tagSBL_FUNCKIND {
	SBLFUNC_VIRTUAL,		// function accessed thru v-table
	SBLFUNC_PUREVIRTUAL,	// function accessed thru v-table, but this class
								// doesn't have an implementation (just subclasses do)
	SBLFUNC_STATIC_ADDR,	// Function is accessed by address, with "this" pointer
	SBLFUNC_STATIC_NOTHIS,// Function is accessed by address, without "this" ptr
	SBLFUNC_RESERVED,
	SBLFUNC_BUILTIN=252,	// reserved for internal SBL use
	SBLFUNC_BASIC=253,	// reserved for internal SBL use
	SBLFUNC_DYNAMIC=254,	// function is accessed by _dynamicInvoke reserved methd
	SBLFUNC_STATIC_DLL = 255// Function is accessed via dll entry
} SBL_FUNCKIND, SBL_FAR * SBL_LPFUNCKIND;



/*
 * SBL_INVOKEKIND specifies whether the function should be parsed as a member
 * or as a method.
 */
typedef enum tagSBL_INVOKEKIND {
	SBLINVOKE_FUNC = 1,			// function is a method: object.show 1, 2
	SBLINVOKE_PROPERTYGET = 2,	// function is get member: var = object.member
	SBLINVOKE_PROPERTYSET = 4,	// function is set member: object.member = var
	SBLINVOKE_PROPERTYPUTREF = 8,// func is set member ref: object.object = obj
} SBL_INVOKEKIND, SBL_FAR * SBL_LPINVOKEKIND;



/*
 * SBL_CALLCONV specifies the calling convention of the function.
 */
typedef enum tagSBL_CALLCONV {
	SBLCC_CDECL = 0,			// The MSC CDECL calling convention
	SBLCC_MSCPASCAL,			// The MSC PASCAL calling convention
		SBLCC_PASCAL = SBLCC_MSCPASCAL,
	SBLCC_MSCPPPASCAL,		// MSC++ PASCAL calling convention
	SBLCC_MSCPPCDECL,			// MSC++ CDECL calling convention

	SBLCC_VARARGS,				// The MSC CDECL callconvention with varargs
	SBLCC_VECTOR,				// The MSC CDECL callconvention with vector
	SBLCC_MSCPPVARARGS,		// The MSC++ CDECL callconvention with varargs
	SBLCC_MSCPPVECTOR,		// The MSC++ CDECL callconvention with vector

	SBLCC_OLE2CDECL,			// CDECL with Ole2 parameter translation
	SBLCC_OLE2PASCAL,			// PASCAL with Ole2 parameter translation
	SBLCC_OLE2CPPCDECL,		// MSC++ CDECL with Ole2 parameter translation
	SBLCC_OLE2CPPPASCAL,		// MSC++ PASCAL with Ole2 parameter translation

	SBLCC_BASICPROTO,			// reserved for internal SBL use
	SBLCC_BUILTIN,				// reserved for internal SBL use
	SBLCC_EXTERNBASIC,		// reserved for internal SBL use
	SBLCC_BASIC,				// reserved for internal SBL use
		SBLCC_MAX = SBLCC_BASIC,
} SBL_CALLCONV, SBL_FAR * SBL_LPCALLCONV;

/*
 * SBL_PARAMDATA holds the information for a single parameter.
 */
typedef struct tagSBL_PARAMDATA {
	SBL_LPSTR szName;					// name of parameter (optional)
	SBL_DATATYPE datatype;			// type of variable
	SBL_VARIANT value;				// value for constant arguements
} SBL_PARAMDATA, SBL_FAR *SBL_LPPARAMDATA;


/*
 * SBL_METHODDATA holds the information that defines a method (the top level
 * method definition structure if you will).
 */
typedef struct tagSBL_METHODDATA {
	SBL_LPSTR szName;					// name of method
	SBL_LPPARAMDATA ppdata;			// pointer to parameter data
	SBL_WORD vtable_slot;			// 0 based
	SBL_CALLCONV call_conv;			// calling convention
	SBL_WORD cArgs;					// number of arguments (including cOptArgs)
	SBL_WORD cOptArgs;				// number of optional arguments
	SBL_INVOKEKIND propOrMethod;	// property get/set or method
	SBL_WORD flags;					// see SBL_METHOD_ defines below
	SBL_DATATYPE SblReturn;			// return type
	SBL_FUNCKIND binding;			// type of binding to use
	SBL_LPDLL_ENTRY dllEntry;		// for STATIC_DLL binding
	SBL_PVOID hardAddress;			// address of routine for STATIC_ADDR binding
} SBL_METHODDATA, SBL_FAR *SBL_LPMETHODDATA;

#define SBL_METHOD_OBSOLETE		0x01	// method is obsolete but still supported
													// generate compiler warning when seen
#define SBL_METHOD_BANG				0x02	// This method should be used when the
													// user has used ! instead of .
#define SBL_METHOD_DEFAULT			0x04	// Allow this method to be used for
													// default members
#define SBL_METHOD_COOPCOMP		0x08	// method is cooperatively compiled
#define SBL_METHOD_TEMP_ADT		0x10	// ADT return val should be destroyed
													// at end of statement
#define SBL_METHOD_EXCEPTIONAL	0x20	// This method may throw an exception
													// and should be called using the
													// SBL_CALLBACK_TRY_INVOKE callback
													// function registered with
													// SblRegisterCallbacks
#define SBL_METHOD_TEMP				0x40	// This method should be deleted at the
													// end of the current compile
#define SBL_METHOD_CMPBINARY		0x80	// Match if OPTION COMPARE BINARY
#define SBL_METHOD_CMPTEXT			0x100	// Match if OPTION COMPARE TEXT



/*
 * SblRegisterCallbacks is called to set call back routines for a
 * language instance.
 */
SBL_STATUS DLLENTRY SblRegisterCallbacks(
	HSBL hsbl,							// handle of the language instance
	SBL_WORD numCallbacks,			// num of callbacks pointed to by lpCallbacks
	SBL_LPCALLBACK lpCallbacks);	// points to an array of callback structures

/*
 * Used from inside a SBL_CALLBACK_TRY_INVOKE callback to invoke an external
 * routine.
 */
void DLLENTRY SblInvoke(
	SBL_PVOID callInformation);

/*
 * Classes are registered with an optional cooperative method registration
 * routine. This allows the application to register individual methods only
 * as needed by the Sbl compiler.
 */
typedef int DLLENTRY SBL_REGISTER_METHOD(
	SBL_DATATYPE classType,				// class type
	SBL_APPLHANDLE classApplHandle,	// app handle from class registration
	SBL_LPSTR pszMethodName,			// method name
	SBL_HCONTEXT hcontext);				// handle to sbl context

typedef SBL_REGISTER_METHOD SBL_FAR *SBL_LPREGISTER_METHOD;



/*
 * Classes registered with Sbl may supply a routine to check versions. This
 * routine is called whenever a module is loaded that uses this class. The
 * routine should return 1 if the version is compatable, 0 if not. If the
 * lpcheckVersion parameter to SblRegisterClass is NULL, then SBL will assume
 * that any version of this class if compatable with the current one.
 */
typedef int DLLENTRY SBL_CHECK_VERSION (
	SBL_LPSTR pszClassName,		// name of class
	SBL_APPLHANDLE classApplHandle,	// app handle from class registration
	SBL_DATATYPE classType,		// datatype of class returned by SblRegisterClass
	SBL_ULONG dwVersion,			// version of class from module being loaded
	SBL_HCONTEXT hcontext);		// sbl context handle

typedef SBL_CHECK_VERSION SBL_FAR *SBL_LPCHECK_VERSION;




/*
 * SblRegisterClass is called to register a class with a language instance.
 */
SBL_DATATYPE DLLENTRY SblRegisterClass(
	HSBL hsbl,								// handle of the language instance
	SBL_LPSTR pszClassName,				// name of the class
	SBL_ULONG dwVersion,					// current version of the class
	SBL_APPLHANDLE applClassHandle,	// aribtrary handle for application use
	SBL_WORD cParentClasses,			// the number of parent classes
	SBL_LPDATATYPE lpparentClasses,	// Pointer to array of SBL_DATATYPE
	SBL_WORD cMethods,					// number of methods being registered
	SBL_LPMETHODDATA lpMethods,		// pointer to array of method defs
	SBL_LPREGISTER_METHOD lpregisterMethod,
												// cooperative registration call back
	SBL_LPCHECK_VERSION lpcheckVersion,
												// routine to check version compatability
	SBL_WORD flags,						// random bit flags defined below
	SBL_LPWORD errMethod,				// store index of the errored method here
	SBL_LPSTATUS methodErr);			// store err generated for method reg. here

/*
 * bit values for use in the flags parameter of SblRegisterClass. They may
 * be or-ed togeather in any combination.
 */

#define SBL_CLASS_PERMANENT		0x0001	// registration is kept after end of
#define SBL_CLASS_GENERIC			0x0002	// allow generic references
#define SBL_CLASS_DYNAMIC_BIND	0x0004	// dynamic bind all references
#define SBL_CLASS_INVISIBLE		0x0008	// hide from basic programmer
#define SBL_CLASS_INSTANTIABLE	0x0010	// allow 'new' operator
#define SBL_CLASS_NOSUBCLASSES	0x0020	// this class will have no sub classes
#define SBL_CLASS_OLE2CLASS		0x0040	// this class is also an ole2 class



/*
 * SblRegisterMethod is called to register methods for a class that has
 * already been registered. SblRegisterMethod is not needed if all methods are
 * registered with the call to SblRegisterClass. SblRegisterMethod is normally
 * called when SblCompile invokes the cooperative registration routine for the
 * class.
 */
SBL_STATUS DLLENTRY SblRegisterMethod(
	HSBL hsbl,							// handle of the language instance
	SBL_DATATYPE classType,			// class for which to register methods
	SBL_WORD cMethods,				// number of methods being registered
	SBL_LPMETHODDATA lpMethods,	// pointer to array of method defs
	SBL_LPWORD errMethod);			// store index of errored method here



/*
 * SblGetClass will look up a class definition by name and return the
 * SBL_DATATYPE associated with that class. If the class is undefined AND the
 * application has register a cooperative registration call back function
 * via SblRegisterCallbacks(), then that call back function will be called
 * before SblGetClass returns.
 */
SBL_DATATYPE DLLENTRY SblGetClass(
	HSBL hsbl,					// handle of the language instance
	SBL_LPSTR pszClassName);// Name of class



/*
 * Classes may support dynamic binding (binding of methods and members at
 * runtime). To do this, the application must register s dynamic invoke
 * method for the class. That method must conform to the following prototype.
 * Note: The hcontext parameter is provided for the applications convience and
 * may be safely ignored.
 */
typedef int SBL_FAR SBL_CDECL SBL_DYNAMIC_INVOKE(
		void SBL_FAR *Object,			// pointer to object
		SBL_WORD objTypeHandle,			// the type handle for Object's class.
		SBL_LPSTR methodName,			// name of method/member to invoke
		SBL_WORD cArgs,					// number of args actually entered
		SBL_VARIANT SBL_FAR *Args,		// values of args
		SBL_WORD cNamedArgs,				// number of named args, 0 for now
		SBL_LPSTR SBL_FAR *NamedArgs,	// names of named args, NULL for now
		SBL_WORD invokekind,				// propertyget, etc.
		SBL_VARIANT SBL_FAR *retValue,// place to store return value
		SBL_WORD flags,					// misc bits. see SBL_DI_* below
		SBL_HCONTEXT hcontext);			// sbl context handle

typedef SBL_DYNAMIC_INVOKE SBL_FAR *SBL_LPDYNAMIC_INVOKE;

// values for flags parameter of SBL_DYNAMIC_INVOKE routines
#define SBL_DI_BANG			0x0001	// member was accessed using the bang
												// operator instead of the dot operator

// values that may be returned by SBL_DYNAMIC_ROUTINEs
#define SBL_INVOKE_OK					1	// method was found and invoked
#define SBL_INVOKE_NOMATCH				2	// a method with a suitable signature
													// could not be found
#define SBL_INVOKE_BADNAME				4	// may be or-ed with SBL_INVOKE_NOMATCH,
													// indicates there is no member/method
													// of any signature with the given name
#define SBL_INVOKE_NO_DEFAULT_OBJ	8	// may be or-ed with SBL_INVOKE_NOMATCH,
													// indicates that the default member is
													// not an object

//
// Classes that require reference counting must supply a routine to decrement
// the number of references to an object. Such routines must conform to the
// following prototype:
// Note: The hcontext parameter is provided for the applications convience and
// may be safely ignored.
//
typedef void SBL_FAR SBL_CDECL SBL_DECREF(
		void SBL_FAR *Object,			// pointer to object
		SBL_HCONTEXT hcontext);			// sbl context handle

typedef SBL_DECREF SBL_FAR *SBL_LPDECREF;

// Add ref uses the same prototype as decref
typedef SBL_DECREF SBL_ADDREF, SBL_FAR *SBL_LPADDREF;



//
// Classes that are used as a base class for instantiable objects must supply
// a _getInstanceData and _putInstanceData reserved method. They should match
// the following prototypes:
typedef SBL_LONG SBL_FAR SBL_CDECL SBL_GETINSTANCEDATA(
		void SBL_FAR *Object,			// pointer to object
		SBL_HCONTEXT hcontext);			// sbl context handle
typedef SBL_GETINSTANCEDATA SBL_FAR *SBL_LPGETINSTANCEDATA;

typedef void SBL_FAR SBL_CDECL SBL_PUTINSTANCEDATA(
		void SBL_FAR *Object,			// pointer to object
		SBL_LONG lpInstanceData,		// pointer to instance data (save this!)
		SBL_HCONTEXT hcontext);			// sbl context handle
typedef SBL_PUTINSTANCEDATA SBL_FAR *SBL_LPPUTINSTANCEDATA;

// Free memory used by an instance of an instantiable object
SBL_STATUS DLLENTRY SblFreeObjectVars(
	HSBL hsbl,
	SBL_HMD hmd,
	SBL_PVOID Obj);



//
// Classes that support the 'typeof' operator must supply a method to give
// the type of an object.
typedef void SBL_FAR SBL_CDECL SBL_TYPEOF(
		void SBL_FAR *Object,		// pointer to object
		SBL_DATATYPE SBL_FAR *lpdt,// fill in with datatype or. . .
		SBL_LPSTR SBL_FAR *lplpname,// fill in with pointer to name
		SBL_HCONTEXT hcontext);			// sbl context handle
typedef SBL_TYPEOF SBL_FAR * SBL_LPTYPEOF;


//
// Classes that support the 'new' operator must supply a method to create
// a new object.
typedef void SBL_FAR * SBL_FAR SBL_CDECL SBL_NEW(
		SBL_HCONTEXT hcontext);			// sbl context handle
typedef SBL_NEW SBL_FAR * SBL_LPNEW;



// --------------------- Unregsiter Function Prototypes  ---------------------
// 
// SblUnregisterVarFunc will unregister a variable of function (all overloaded
// versions).
SBL_STATUS DLLENTRY SblUnregisterVarFunc(
	HSBL hsbl,
	SBL_LPSTR name);

// SblUnregisterType will unregister a type/adt/class definition.
SBL_STATUS DLLENTRY SblUnregisterType(
	HSBL hsbl,
	SBL_LPSTR name);

// SblUnregisterClean will scan thru the symbol tables and remove any entries
// that used deleted (unregistered) types.
typedef void (*SBL_UNREG_NOTIFY)
	(HSBL hsbl, SBL_WORD isType, SBL_LPSTR name, SBL_LPSTR methodName);
SBL_STATUS DLLENTRY SblUnregisterClean(
	HSBL hsbl,
	SBL_UNREG_NOTIFY callThis);


// SblUnregisterMethod will remove the definition for a single method from a
// class. Note that all the overloaded methods of the same name are removed.
SBL_STATUS DLLENTRY SblUnregisterMethod(
	HSBL hsbl,
	SBL_DATATYPE classType,
	SBL_LPSTR name);
// --------------------- Variant Function Prototypes  ---------------------

// Initializes a variant.  This must be performed before a variant can be
// used.
void DLLENTRY SblVariantInit(
	 SBL_LPVARIANTARG parg);	  // Variant to initialize

// There are no API calls to set or get variant values.  These can be set
// directly via structure notation:  v.vt = VT_I2; v.i = 2; etc.
// Variants can be converted with SblVariantChangeType, however; the new
// value can then be read directly from the destination variant.

// Changes a variant's type.  The source and destination may be the same or
// different variants, but the destination variant _must_ have been set up
// properly (SblVariantInit) if the change does not occur in-place.
// Returns SBL_E_DISP_TYPE_MISMATCH if the conversion is not possible.
SBL_STATUS DLLENTRY SblVariantChangeType(
	 SBL_LPVARIANTARG pargDest,		// Variant into which to put value
	 SBL_LPVARIANTARG pargSrc,			// Variant from which to get value
	 SBL_WORD wFlags, 					// Unused - must be 0
	 SBL_VARTYPE vtNew);					// new variant type

// Clears a variant - resets the type to empty and frees all associated
// storage.  The variant can then safely be discarded.
SBL_STATUS DLLENTRY SblVariantClear(
	 SBL_LPVARIANTARG parg);	  		// Variant to clear

// Copies one variant into another.  The destination variant _must_ have been 
// set up properly (SblVariantInit).
SBL_STATUS DLLENTRY SblVariantCopy(
	 SBL_LPVARIANTARG pargDest,		// Variant into which to copy value
	 SBL_LPVARIANTARG pargSrc);		// Variant from which to copy value

// Formats a variant according to format string
SBL_STATUS DLLENTRY SblFormat(
	HSBL hsbl,			// handle of sbl instance
	SBL_LPVARIANT argument,		// data to format
	SBL_LPVARIANT Format,		// Format string
	SBL_LPVARIANT result);		// variant to store result in

// Routines to allocate  and free bstr's (a bstr is a string that looks like
// the ole2 version ofstrings). Any string returned as part of a variant into
// SBL must have been allcoated from SBL memory via there entry points. This
// is required to keep SBL from freeing other applications memory into SBL
// mnemory heaps.

// SblAllocStringLen allocates memory for the string and copies the string
// into the new memory. Use this entry for strings with embedded nulls.
SBL_BSTR DLLENTRY SblAllocStringLen(SBL_LPBYTE pch, SBL_WORD cch);

// SblAllocString is more convient. It figures out the length for you. However,
// is uses strlen to figure out the length and so works incorrectly for strings
// with embedded nulls
SBL_BSTR DLLENTRY SblAllocString(SBL_LPSTR sz);

// SblFreeString returns the memory to the free memory list.
void DLLENTRY SblFreeString(BSTR bstr);

// SblStringLen returns the length of the bstr which has been hidden away
// in memory before the actual string. Unlike strlen, SblStringLen does not
// have to scan the string.
SBL_WORD DLLENTRY SblStringLen(BSTR bstr);

/*	 D I A L O G   B O X   C O N T R O L   I D S */

typedef SBL_ULONG				   SBL_CTLID;	  // control id;
typedef SBL_CTLID SBL_FAR *   SBL_LPCTLID;  

//
// Get control id from SBL_CTLID
//
#define SBL_CTLID_ID(x) 	((int)x)

// for compatibility with older versions:
#define SblCtlId(x) 	((int)x)

SBL_STATUS DLLENTRY SblDlgGetControlId(HSBL hsbl, 
				    SBL_HWND hDlg, 
				    SBL_LPSTR pszName,
				    SBL_CTLID SBL_FAR *pID);

#ifdef __cplusplus
}
#endif


#ifdef _MSC_VER
#pragma pack()
#endif

#endif	// SBL_H
