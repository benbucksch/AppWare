/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "op_wnd.h"
#include "o_wnd.h"
#include "o_pict.h"



#define DATAVersion        2 // March 30, 1993  data Version
#define DLLVersion         2 // April 12, 1993  dll Version

// used to set def. size during WIDR
#define PICT_DEFWT 100;
#define PICT_DEFHT 100;


#define ID_OBJECT					 1001
#ifndef IDHELP

#define IDHELP 					 998

#endif

#define I_UNUSED(x)								((void) (x))

// modifier flags
#define RECREATEHANDLE  	 1 // delete old handle and recreate a new one 
#define ALWAYSUSEPALETTE   2 // Always use this pictures palette to draw

#define DESCRIPTIONID			 500
#define STARTUPID       	 501
#define SHUTDOWNID      	 502

#define IDM_CUT      			 101
#define IDM_COPY     			 102
#define IDM_UNDO    			 102
#define IDM_PASTE     		 102
#define PASTEID     			 200
#define LOADFILEID  			 201
#define SELECTABLEID 			 202
#define BITMAPID   				 203
#define METAFILEID 				 204
#define CROPID      			 205
#define ORIGPROPID   			 206
#define SIZETOFITID				 207
#define PICTUREID  				 208
#define CUTID      				 209
#define HELPID     				 211
#define BORDERID   				 212  // used for border bitmap

#define MAXNAME 					 255

#define CREATENEW  				 1
#define CREATEOLD   			 0

#define NULLOBJECTID  		 0

#define SAVE_FROM_FILE		 1
#define SAVE_FROM_CLIPBOARD 2

/**********************DATAS defines*******************************/


#define OBJECTINSTDATA     3
#define PICTDATA0          0 	// PICTDATA structure
#define BMPMETADATA     	 1 	// picture info, tablesize, imageSize,
#define PALETTEDATA        2  // used to store a palette structure

/********************************************************/

#define FILENAME           5 // store a file Name for META Picture
														  // at runtime

#define PICTDATASCOPE      "pictDataScope"
// UNIV_META ID
#define UNIV_METAFILE      AMAKEID('U', 'M', 'E', 'T')
#define BITMAPPICT 				 1
#define METAPICT   				 2
#define ANYPICTYPE 				 3

/************************ SIGNALS ***********************/
#define PICTCHANGEDSIGNAL     0
#define PICTCLICKEDSIGNAL     1
#define PICTACTIVATEDSIGNAL   2
#define PICTDEACTIVATEDSIGNAL 3
#define PICTDBLCLICKEDSIGNAL  4

/*******************************************************/
struct PALETTEINFO {
	HPALETTE  hPalette;
	HPALETTE  hOldPal;
	BOOL    	bUsePal;
	RGBQUAD 	rgbQuad[];

	};

// This data is stored in the data#0 of the PICT Object
// for version 1.0
struct PICTDATAv1 {
	int version   ;  // version # 8/20  1
	BOOL pictType;   //indicates whether the picture is bitmap or a metafile
	WORD pictStyle;  // Crop, fitToScreen, OrigProp etc
	BOOL bSelectable;
	HANDLE hPicture; // handle to the Bitmap/Metafile image
									 // this is used during runtime but
									 // is not saved on the disk.
	BOOL bClipboard; // If the picture was pasted from Clipboard
	BOOL bImageSaved; // if a picture is loaded
	long height;
	long width;

	};



// This data is stored in the data#0 of the PICT Object
// for current version 
struct PICTDATA {
	int version   ;  // version # 8/20
									 // 1.0 - Dec'92
                   // 2.0 - April'93
	BOOL pictType;   //indicates whether the picture is bitmap or a metafile
	WORD pictStyle;  // Crop, fitToScreen, OrigProp etc
	BOOL bSelectable;

	BOOL bClipboard; // If the picture was pasted from Clipboard
	BOOL bImageSaved; // if a picture is loaded
	long height;
	long width;
	BOOL bFrame;    // if we want border around picture Version 2.0

	};




// This struct is stored in the Data#1 of the Picture Object
// #if it is a BITMAP PICTURE
struct BMP
	{
	BITMAPFILEHEADER    bmpFileHdr;
	BITMAPINFOHEADER    bmpInfoHdr;						//these two fields must remain
	BYTE  	      rgbQuadAndBits[];   //adjacent and in this order

	};
// #else if it is METAFILE

struct METAREC {
	DWORD rdSize;
	WORD rdFunction;
	WORD rdParam[];

	} ;

struct METAFILE {
	METAHEADER metaHdr;
	METARECORD *metaRecs;
	};


// This is a class Defined to be used during the description time
// of the PICTURE OBJECT

class PICTOBJECT {
	PICTDATA 						pictData;     // This is copy of DATA0 (PICTDATA)
	METAFILE 						metaPict;     // store the Metafile Struct
	AOBJECTID 						objectID;

	long 								ImageSize;    // sizeof Bitmap or the METAFILE
	long 								tableSize;    // size of RGBQUAD table
	BITMAPFILEHEADER    bmpFileHdr;
	BITMAPINFOHEADER    bmpInfoHdr;		//these two fields must remain
	BYTE        		    *rgbQuadAndBits;    // Ptr toa buffer hold
																					// RGBQuad and bits
	BYTE        		    *metaBits;
	BOOL		 						bSave;        // If the User clicks on OK in def DLG
	BOOL 								bClientRect; 	// Use this flag to indicate whether we need
																		// to get the ClientRect of the hPaintWindow or
																		// we should use the Rect in the DrawRect
																		// During BUILD time (WIDR) we get the DC of the hPaintWnd
																		// During Run time we use the Rect passed to us in the WITEM_INFO struct
	AMEMBLOCKID 					memID;				// Memblock to which the rgbQuadAndBits fields pts to
	int 								pErrMemLock(int errType); // error handler

public:
	PICTOBJECT(AOBJECTID objID, HWND hWNd);   // Constructor
	~PICTOBJECT();  // destructor

	HANDLE LoadBMP(HANDLE fileHandle);
	HANDLE LoadMetaFile(
			HWND 				hPaintWnd,
			HDC 				hDC,
			LPRECT			lpDrawRect, HANDLE hPict);

	inline	void SetbImageSave(BOOL val) {		pictData.bImageSaved = TRUE; }
	inline  BOOL GetbImageSave() {return pictData.bImageSaved ; }
	inline  WORD  GetPictStyle() {return pictData.pictStyle;}
	inline  void SetPictStyle(WORD val) {pictData.pictStyle = val;}
	inline  int  GetPictType() {return pictData.pictType;}
	inline  void SetPictType(int val) {pictData.pictType= val;}
	inline  void SetbClipboard(int val) {pictData.bClipboard= val;}
	inline  BOOL GetbClipboard() {return pictData.bClipboard;}
	inline	void SetImageSize(long val) {	ImageSize = val; }
	inline 	LONG GetImageSize() {return ImageSize ; }

	inline  BOOL IsSelectable() { return pictData.bSelectable; }
	inline  void SetSelectable(BOOL val) { pictData.bSelectable = val; }

	inline  BOOL IsFramed() {return pictData.bFrame; }
	inline  void SetFrame(BOOL val) {pictData.bFrame = val; }
	inline  void SetbSave(BOOL val) {bSave = val; }
	inline  BOOL GetbSave() {return bSave; }

	void 		ClipBitmap(HDC hDC,LPRECT 			lpDrawRect,HANDLE			hPict);
	HANDLE 	StretchBitmap(HDC hDC, LPRECT lpdrawRect);
	int 		SavePictFile(LPSTR fName);
	int 		SaveMetaData(
						int   fromWhere,  // flag indicating where to save METAFILE from
													 // i.e. CLIPBOARD or a FILE on disk
						LPSTR fileHandle
						);

	void 		InitObjFieldsFromDatas();
	int			 SaveBMP(
						int fromWhere,
						HANDLE fileHandle
						) ;

	int 	 	InitializeFOpen(HWND hWnd);
	HANDLE  CreatePictH(HDC hDC, LPRECT lpRect);
	int 		PasteFromClipboard(HWND hParent);
	int 		SaveRuntimeData();
	int			ObjectToBitmap(AMEMBLOCKID bufferID,
						BOOL bReturnAsBitmap); // bool indicating whether we want to return
	void 		BitmapToObject(AMEMBLOCKID bufferID);
	HANDLE 	DrawBitmap(HDC  hDC,RECT*	lpDrawRect, HANDLE hPict);
	void 		DrawPicture (HWND	hWndParent,HDC	hDC,HANDLE	hPict,RECT*	 lpDrawRect);
	int 		InitializeFOpen(PICTOBJECT		*pO,HWND 					ghWnd);
	void 		EditCut(HWND hWnd, LPRECT lpRect);
	void 		EditCopy(HWND hWnd, LPRECT lpRect);
	BOOL 		EditPaste(HWND hWnd, LPRECT lpRect);
	void 		CutCopyPict( int flag, HWND hWnd);
	int 		DeletePHandle(HANDLE hPict);
	void 		PrintPicture(HWND 		hWnd,HDC 		hItemDC,LPRECT 	lpRect,HANDLE *lpHPict);
	} ;

/**************************************************************
// class used to save pertinent info for a visual instance
// of a Picture WIndow Item.  Inherits from opLiteWndItem class
// Window Object will call methods (event Handlers) that are
// defined for this class.  (An implementation of window Item Protocol)
// In cases where the methods are not defined (to handle some event), window
// object will assume some default behaviori.e. default opLiteWndItem::methods
// will be called
*************************************************************/
class PICTURE_VINST:public OpLiteWndItem {
  private:
		HANDLE		 			hPaintWnd;  // Handle to the window in which the
										 				// picture is drawn.
		BOOL 	 					isActivated;// if the active can be activated
		HANDLE 					hPict;      // handle to a picture object , v1.1
		void        		DrawActFrame(BOOL bDraw); // Draw/Erase the activate frame

	public:
										PICTURE_VINST	(AOBJECTID oid,
													AOBJECTID 	parentID,
													RECT*		 	rcItem,
													long		 	status);
										~PICTURE_VINST(void);
		RECT						mapRect;
		BOOL						IsActivated		() {return isActivated; }
		void						SetActivated	(BOOL val) { isActivated = val; }
		HANDLE					GetPictureHandle() { return hPict; }
		BOOL            SetPictureHandle(HANDLE hp) { hPict = hp; return TRUE;}
    void ALMAPI     ParentOpened();
    void ALMAPI     ParentClosing();
		void ALMAPI			Destroy				(void);				// MUST override Destroy!
		void ALMAPI			DataChanged		(long details, long modifiers, long variation);
		long ALMAPI			Activate			(int causedBy, BOOL mustKeep);
		void ALMAPI			Deactivated		(void);
		void ALMAPI			Draw					(HDC hdc, RECT* invalidRect);
		void ALMAPI			MouseDown			(POINT* where, int keysDown, int button);
		void ALMAPI  		Print					(HDC hDC, RECT *printRect, int pclPrinter);
		long ALMAPI 		AdjustNewRect	(LPRECT newRect, LONG flag);
		void ALMAPI 		DoubleClicked	(POINT *pt, int kdown, int button);
		void ALMAPI 		Move					(LPRECT newRect);
		void ALMAPI  		Cut						();
		void ALMAPI  		Copy					();
		// void ALMAPI  	Undo();
		void ALMAPI  		Paste					();
		HANDLE 					GethPict()		{ return hPict ; }
		void						SethPict(HANDLE hP) { hPict = hP; }
		PICTOBJECT			*pictObject; // Object Instance pointer
		void ALMAPI     Hide();
		void ALMAPI     Show();

	};
