///////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// textfact.h
// Robert Lundahl/ Edgar C. Jerez
//
// Menu Control Functions
// Start Date 05/28/92
// End   Date 06/../92
//
// Characters allow in string expression
//
// <cr>+<lf>+<tab>+<esc>+<##>+"literal text"+Object Name
///////////////////////////////////////////////////////////////


#define NULLMSG			0
#define hNULL				0L
#define Loop()			while(1)
#define DATACOUNT		5L
#define maxEdit			255

#define SIGNAL_EXPUPDATE			0
#define SIGNAL_VARCHANGED			1
#define SIGNAL_ERROR					2
#define NUM_TEXTFACT_SIGNALS	3

#define	VERSION_OBJECT				0
#define VERSION_DLL						1L

#define OTYPE_TEXTMAKER									MAKEID('T','f','c','t')


///////////////////////////////////////////////////////////////
// enum TYPES.
//
///////////////////////////////////////////////////////////////

enum TYPES
{
	STOP,
	LITERAL,
	CHARACTER,
	OBJECT
};


///////////////////////////////////////////////////////////////
// struct TOKEN.
//
///////////////////////////////////////////////////////////////

struct TOKEN
{
	TYPES	type;
	LONG	multi;
	WORD	length;
};


///////////////////////////////////////////////////////////////
// struct DATAFOUR.
//
///////////////////////////////////////////////////////////////

struct DATAFOUR
{
	BOOL			autoUpdate;
	OBJECTID	resultId;
	int				version;
};


///////////////////////////////////////////////////////////////
// LISTDATA class.
//
///////////////////////////////////////////////////////////////

class LISTDATA
{
	private:
		BYTE*			string;

	public:
		OBJECTID	objid;
		BYTE*			alias;

		LISTDATA ( void );
		LISTDATA ( OBJECTID  id );
		~LISTDATA ( );
		LONG Alias ( void );
		void SetAlias ( BYTE* name );
		void Init ( OBJECTID id, BYTE* name );
};


///////////////////////////////////////////////////////////////
// TEXTFACT class.
//
///////////////////////////////////////////////////////////////

class TEXTFACT
{
	private:
		HWND			hActive;
		HWND			hDialog;
		OBJECTID	objectId;
		HWND			hType;          // window handles to allow hiding
		HWND			hRemoveButton;
		HWND			hAddButton;
		HWND			hEditButton;
		HWND			hAliasTitle;
		HWND			hAlias;
		OBJECTID*		data0;      	// object id(LONG) NULL Terminated
		STRING*		  strclass1;    // data 1 alias names
		TOKEN*			data2;      	// expression token list NULL Terminated
		STRING*			strclass3;    // data 3 literals
		DATAFOUR*   data4;				// auto update(int), result object id(long)

	public:
		TEXTFACT( OBJECTID id );
		~TEXTFACT()
		{
			AObjUnlockData(objectId, 0L);
			AObjUnlockData(objectId, 2L);
			AObjUnlockData(objectId, 4L);
			if (strclass1)
				delete strclass1;
			if (strclass3)
				delete strclass3;
		}
		void SetDialog ( HWND hdlg ) {hDialog = hdlg;}
		HWND GetActive ( void ) {return hActive;}
		int  Version ( void ) {return data4->version;}
		void InitDlgControl ( void );
		void UpdateObject ( void );
		void DeleteListData ( void );
		void FillType ( void );
		void FillVar ( void );
		void FillResult ( OBJECTID id = 0L);
		void Add ( void );
		void Remove ( void );
		void Edit ( void );
		void Alias ( void );
		void MakeExpress ( void );
		WORD Express ( void );
		void UpdateAddButton ( void );
		void UpdateRemoveButton ( void );
		void UpdateEditButton ( void );
		void UpdateAliasControl ( void );
		LONG Validate ( pAObjMessage sys );
		void CheckObjectID ( void );
		void MakeLiteral ( BYTE* express, LONG multi );
		void MakeCharacter ( BYTE* express, LONG multi );
		void MakeCharacterMac ( BYTE* express, LONG multi );
		void MakeObject ( BYTE* express, LONG multi );

		LONG DoAssign (OBJECTID theObject, pAObjMessage systemPtr );
		void DataObjectDelete ( OBJECTID* objPtr );
		void DataTokenDelete ( TOKEN* tokenPtr );
		BOOL Parse ( BYTE* string, pAObjMessage sys = NULL, BOOL isImportTime = FALSE );
		void DrawObjectName ( void );
		LONG ObjectHealthy ( AObjInfo* infoRecordPtr );
		BOOL CheckDuplicate ( void );
		long GetLiteral ( char*& space, WORD& length, pAObjMessage sys, BOOL isImportTime );
		long GetCharacter ( char*& space, pAObjMessage sys, BOOL isImportTime );
		int  GetObject ( char*& space, pAObjMessage sys, BOOL isImportTime );
		BOOL SaveDatas ( void );
		BOOL RestoreDatas ( BOOL error );
		LONG ExportObject ( pAObjMessage systemPtr );
		LONG ImportObject ( pAObjMessage systemPtr );
		void MakeExport ( char* express );
		void MakeExportMac ( char* express );
		static BOOL FAR PASCAL _export TfDialog ( HWND hDlg, WORD message, WORD wParam, LONG lParam );
		void CopyDataPtr (OBJECTID dstObj, OBJECTID srcObj, long index );
		static long far pascal _export NewEditControlProc ( HWND hWnd, WORD message, WORD wParam, long lParam );
};


///////////////////////////////////////////////////////////////
// Global Variable Class.
//
///////////////////////////////////////////////////////////////

class GLOBAL
{
	public:
		HWND			hWnd;										// Window handle
		HINSTANCE	hInstance;  						// Program Data Instance
		int				cbOffset;   						// SerDlg Class Offset
		BOOL			parseError; 						// Temp Global
		FARPROC 	lpfnOldEditControlProc;

		GLOBAL ( void )
		{
			hWnd = 0L;
			hInstance = 0L;
		}
};

int	 GetAppVersion( OBJECTID );

extern "C"
{
// void CopyDataPtr (OBJECTID dstObj, OBJECTID srcObj, long index );
LONG TFac_obj(OBJECTID	theObject, pAObjMessage theSystem);
	LONG FAR PASCAL _export Administration(OBJECTID theObject, pAObjMessage systemPtr);
	LONG FAR PASCAL _export GetSerVersion ( TYPEID theType );
}
