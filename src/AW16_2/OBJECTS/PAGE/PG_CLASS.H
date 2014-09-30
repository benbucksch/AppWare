///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Page
//
// Filename:  "pg_class.cpp"
//
// Description:
//  Page class definition
//
////////////////////////////////////////////////////////////////////////////
#ifndef  PG_CLASS_H
#define  PG_CLASS_H

#include "o_print.h"

#define NUM_PAGE_SIGNALS		1
#define SIGNAL_PAGEFINISHED	0


#define NULLMSG         0
#define hNULL           0L
#define Loop()          while(1)
#define DATACOUNT       2L
#define WINPIXINCH      96      // pixels per inch on of window conversion

#define VERSION_OBJECT	0
#define VERSION_DLL	    1L


#define maxEdit         255

#define OTYPE_PAGE              MAKETYPEID('P', 'a', 'g', 'e')


enum TYPES
{
  STOP,
  LITERAL,
  CHARACTER,
  OBJECT
};

enum ITEMTYPE
{
   FIRST,
   RUN,
   LAST
};


struct OBJHEADER
{
  OBJECTID	window_id;	  // window object that contains items
  ITEMTYPE	type;         // first item, run item, or last item in window list
  OBJECTID	object_id;    // window item id
  int	    	numWndItem;	    // number of window items
};


struct OBJDATA
{
  int		data_number;      // data
  int		length;           // length of the data
};


struct DATA0
{
	int		version;
	int		count;        	// Count of windows - layout instances
	int   nextWindow;   	// Next Window to be called, 0 based
	int		spooled;				// Number of spooled windows - used to compute pages printed
	int		pages;        	// page counter that is incremented during spool window function
	int		hFile;        	// handle to spool file

	char	spoolFile[150]; // spool file name
	BOOL	snap;
};


class PAGE;


class DATA1
{
	public:

	OBJECTID  objid;          // window object id - null for terminater instance

	char*			name;

	POINT			xy;             // in pixels from left and top of layout window

	int				width;          // for layout size only not use to size printing
	int				height;         // for layout size only not use to size printing

  int				pixinch_x;	  	// pixels per inch for layout
	int				pixinch_y;

	DATA1 ( PAGE* page, OBJECTID id );

	DATA1 ( DATA1* data1 );

	~DATA1()
	{
		if (name != NULL)
			delete name;
	}

	void Refresh ( HWND hPageLayout );
	void Resize ( HWND hLayout, HWND hwindow );
};


typedef struct DATA0 far *LPDATA0;
typedef struct DATA1 far *LPDATA1;

class PAGE
	{
	private:

		HWND			hActive;
		HWND            hDialog;
	
		OBJECTID        objectId;

		pAEvtInfo 		eventPtr;			// runtime constructor
		pAObjMessage 	systemPtr;    // runtime constructor

		HWND            hLayout;
		HWND            hRemoveButton;
		HWND            hAddButton;
		HWND            hCopyButton;
		HWND            hEditButton;
		HWND			      hPageLayout;

		int				winpixinch_x;     // window conversion - VGA 96 pixels per inch
 	  int				winpixinch_y;

		int				aspectx;
	  int				aspecty;

		int 			pixinch_x;
 	  int				pixinch_y;

		WORD			grid_height;
		WORD			grid_width;
		WORD			grid_inc_y;
		WORD			grid_inc_x;

		DATA1*		layPtr;

		RECT			dialogRect;

		DATA0*		data0;		// structure DATA0 1 instance
		DATA1*    data1;      // window layout instances; count contained in DATA0->count

		HFONT			hFont;
		LOGFONT		logfont;

		WORD			mousex_save;
		WORD			mousey_save;

	public:

		BOOL			snap;

		HPEN			hPen1;
		HPEN			hPen2;
	
		HWND			hObjectWnd;		// child window handle in layout

		RECT			objectRect;     // in screen coordinates
		RECT			layoutRect;		

		PAGE ( OBJECTID id );
		PAGE ( OBJECTID id, pAEvtInfo eventptr, pAObjMessage systemptr);
	
		void ExportObject ();
		void ImportObject ();

		void SetDialog ( HWND dlg )		{hDialog = dlg;}
		HWND GetActive ( void )				{return hActive;}
		int PixInchX ( void )					{return pixinch_x;}
		int PixInchY ( void )					{return pixinch_y;}
		int WinPixInchX ( void )			{return winpixinch_x;}
		int WinPixInchY ( void )			{return winpixinch_y;}
		int GetCount ( void )					{return data0->count;}
		int Version ( void )					{return data0->version;}
	
		void InitDlgControl ( void );
		void UpdateObject ( void );
		void DeleteListData ( void );
		void WindowList ( void );
		void SelectedList ( void );
		void Add ( void );
		void Remove ( void );
		void Edit ( void );
		void UpdateAddButton ( void );
		void UpdateRemoveButton ( void );
		void UpdateEditButton ( void );
		void MakeWindowTop ( void );
		void Validate ( pAObjMessage sys );
		void CheckObjectID ( void );
		void DataLayoutDelete ( DATA1* layoutPtr );
	
		void DrawLayout ( void );
		void DrawBorder ( HDC hdc );
		void CreateLayoutClass ( void );
		void CreateObjectClass ( void );
		void CreateLayoutWindow ( void );
		void CreateObjectWindow ( DATA1* layPtr);
		void FontSetup ( void );
		void DrawGrid ( HDC hdc );
		void MouseMove ( HWND hwnd, LONG lParam );
		void DrawWindowFrame ( HWND hWnd );
		void SpoolWindow ( pAObjMessage sys );
		OBJECTID UnSpoolObject ( ITEMTYPE& itype );
		void IncrementNextWindow ( void );
		void IncrementPage ( void );
		void Pidr ( void );
		BOOL NewPage ( LPPIDR_BLOCK pidr );
		void DrawObjectName ( void );
		LONG ObjectHealthy ( AObjInfo* infoRecordPtr );

		static BOOL FAR PASCAL _export PageDialog ( HWND hDlg, WORD message, WORD wParam, LONG lParam );
		static LONG FAR PASCAL _export LayoutWndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam);
		static LONG FAR PASCAL _export ObjectWndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam);
	};


// Global Variable Class Manager
class GLOBAL
{
	public:

	HWND      hWnd;               // Window handle
	HANDLE    hInstance;

	int				cbOffset;

	GLOBAL ( void )
	{
    hWnd = 0L;
    hInstance = 0L;
  }
};

#endif