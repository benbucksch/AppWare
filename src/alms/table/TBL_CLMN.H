////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:  "tbl_clmn.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TBL_CLMN_H  // prevent multiple includes
#define __TBL_CLMN_H

// defines /////////////////////////////////////////////////////////////////

#ifndef ALIGN_xxxx
  #define ALIGN_xxxx
  #define ALIGN_ERROR         (-1)
  #define ALIGN_LEFT          310
  #define ALIGN_CENTER        309
  #define ALIGN_RIGHT         311
  #define ALIGN_TITLELEFT       2
  #define ALIGN_TITLECENTER     1
  #define ALIGN_TITLERIGHT      3
#endif

#define COLUMN_DEFAULTTITLE   "New Column"
#define COLUMN_FONT           "MS Sans Serif"
#define COLUMN_TITLEPTSIZE    10
#define COLUMN_ITEMPTSIZE     8
#define COLUMN_DEFAULTWIDTH   80
#define COLUMN_TITLEBUFSIZE   32

// old column structure -- pre-1993 version ////////////////////////////////
#define LOCKCOLUMN          column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN)
#define UNLOCKCOLUMN        AObjUnlockData(theObject, DATA_COLUMN)


struct oldTCOLS
  {
  char      title[COLUMN_TITLEBUFSIZE];
  AOBJECTID  theRef;
  LOGFONT   lfTitle;    // font characteristics for the column title
  int       TiPtSize;   // column title font point size
  LOGFONT   lfColumn;   // font characteristics for the column elements
  int       CiPtSize;   // column element font point size
  COLORREF  tColor;     // title fg color
  COLORREF  itemColor;  // column element fg color
  int       width;      // column width in pixels
  BOOL      bcanEdit;   // flag to specify if a column is editable

  BOOL      unused;
  HFONT     hFont;      // a handle to a font for the column elements

  int       lineHeight; // used to keep all text on the same base line
  int       lineBase;
  int       lineAscent;
  int       tLineHeight;
  int       tLineBase;
  int       tLineAscent;
  int       align;      // column alignment
  };

// column data structure -- actual object data /////////////////////////////

struct TCOLS
  {
  char      title[COLUMN_TITLEBUFSIZE]; // column title string
  AOBJECTID  objRef;         // reference object for the column
  BOOL      bCanEdit;       // flag to specify if a column is editable
  int       align;          // cell alignment
  int       colWidth;       // column width in pixels
  LOGFONT   lfTitle;        // font characteristics for the column title
  int       titlePtSize;    // column title font point size
  COLORREF  titleFG;        // title fg color
  COLORREF  titleBG;        // title bg color;
  int       titleCharWidth; // average character width of the title
  int       titleLeading;   // used for the row spacing
  int       titleAscent;    // used for keeping the baseline constant
  int       titleDescent;   // used for keeping the baseline constant
  LOGFONT   lfCell;         // font characteristics for the column cells
  int       cellPtSize;     // column cell font point size
  COLORREF  cellFG;         // column cell fg color
  COLORREF  cellBG;         // cell bg color
  int       cellCharWidth;  // average character width of the cells
  int       cellLeading;    // used for the row spacing
  int       cellAscent;     // used for keeping the baseline constant
  int       cellDescent;    // used for keeping the baseline constant
  BYTE      titleAlign;     // title alignment

 
  //reserved for future column expansion -- reverse compatibility
  char      Reserved1;    // must be 0
  int       Reserved2;    // must be 0
  int       Reserved3;    // must be 0 ->used on April 28
  int       Reserved4;    // must be 0 ->used on April 28
  };

// COLUMN class definition /////////////////////////////////////////////////

class COLUMN
  {
  private:
    AOBJECTID  theObject;
    int       numColumns;
    TCOLS*    column_data;

    // private methods
    UINT      AlignToFlags(int index)   {LOCKCOLUMN;
                                         UINT rVal =  column_data[index].align==ALIGN_RIGHT ? DT_RIGHT : (column_data[index].align==ALIGN_CENTER ? DT_CENTER : DT_LEFT);
                                         UNLOCKCOLUMN;
                                         return rVal;}
    UINT      TitleAlignToFlags(int i)  {LOCKCOLUMN;
                                         UINT rVal = column_data[i].titleAlign==ALIGN_TITLERIGHT ? DT_RIGHT : (column_data[i].titleAlign==ALIGN_TITLECENTER ? DT_CENTER : DT_LEFT);
                                         UNLOCKCOLUMN;
                                         return rVal;}
                                         

  public:
    COLUMN(AOBJECTID objID);

    // inline methods
    int       NumColumns()            {return numColumns;}
    void      SetNumColumns(int i)    {numColumns = i; }
    char*     Title(int index)        {LOCKCOLUMN;
                                       char *rVal =  column_data[index].title;
                                       UNLOCKCOLUMN;
                                       return rVal;
                                       }

    AOBJECTID  GetObjectID(int index)   {LOCKCOLUMN;
                                       AOBJECTID rVal = column_data[index].objRef;
                                       UNLOCKCOLUMN;
                                       return rVal;
                                       }

    
    void      SetObjectID(int i, AOBJECTID oID)
                                      {
                                      LOCKCOLUMN;
                                      column_data[i].objRef = oID;
                                      UNLOCKCOLUMN;
                                      }

    int       ColumnWidth(int index)  {
                                      LOCKCOLUMN;
                                      int rVal =  column_data[index].colWidth;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                    
                                      }
    void      SetColumnWidth(int i, int width)
                                      {
                                      LOCKCOLUMN;
                                      column_data[i].colWidth = width;
                                      UNLOCKCOLUMN;
                                      }
    UINT      DT_Flags(int index)     {
                                      LOCKCOLUMN;
                                      UINT rVal = DT_SINGLELINE | DT_NOPREFIX | DT_TOP | AlignToFlags(index);
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    UINT      DT_FlagsTitle(int i)    {
                                      LOCKCOLUMN;
                                      UINT rVal = DT_SINGLELINE | DT_NOPREFIX | DT_TOP | TitleAlignToFlags(i);
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       TitleLeading(int i)     {
                                      LOCKCOLUMN;
                                      int rVal =  column_data[i].titleLeading;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       TitleAscent(int i)      {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].titleAscent;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       TitleDescent(int i)     {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].titleDescent;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       TitleCharWidth(int i)   {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].titleCharWidth;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    LOGFONT*  TitleLogFont(int i)     {
                                      LOCKCOLUMN;
                                      LOGFONT *rVal = &column_data[i].lfTitle;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       CellLeading(int i)      {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].cellLeading;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       CellAscent(int i)       {
                                      LOCKCOLUMN;
                                      int rVal =  column_data[i].cellAscent;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       CellDescent(int i)      {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].cellDescent;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       CellCharWidth(int i)    {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].cellCharWidth;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    LOGFONT*  CellLogFont(int i)      {
                                      LOCKCOLUMN;
                                      LOGFONT *rVal = &column_data[i].lfCell;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    int       Align(int i)            {
                                      LOCKCOLUMN;
                                      int rVal = column_data[i].align;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }                        
    int       TitleAlign(int i)       {
                                      LOCKCOLUMN;
                                      int rVal = (int)column_data[i].titleAlign;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }
    BOOL      CanEdit(int i)          {
                                      LOCKCOLUMN;
                                      BOOL rVal = column_data[i].bCanEdit;
                                      UNLOCKCOLUMN;
                                      return rVal;
                                      }

    BOOL      SetEditAttrib(int i, BOOL bEdit)       {
                                      
                                      LOCKCOLUMN;
                                      column_data[i].bCanEdit = bEdit;
                                      UNLOCKCOLUMN;
                                      return TRUE;
                                      }

    // methods
    void      CreateTitleFonts(HDC hDC, int logPixelsY, BOOL oldWidth);
    void      CreateCellFonts(HDC hDC, int logPixelsY, BOOL oldWidth);
    void      DestroyTitleFonts();
    void      DestroyCellFonts();
    void      AddColFont(int    index,int   numCols);
    void      RemoveColFont(int   index,int   numCols);

    void      InitColumn(AOBJECTID objID);
    int       AddColumn(AOBJECTID objID, char* title, int index);
    void      DeleteColumn(int index);
    BOOL      Export();
    BOOL      Import();
    static BOOL     ExportFont(LPCSTR theScope, long scopeIndex, LOGFONT* lf, HDC hdc);
    static BOOL     ImportFont(LPCSTR theScope, long scopeIndex, LOGFONT* lf, HDC hdc);
    void      ConvertData();
    void      SetDefaults(TCOLS* newColumn, TCOLS *defCol, HDC hdc = NULL);
    void      TitleMetrics(HDC hDC, int logPixelsY, BOOL oldWidth);
    void      CellMetrics(HDC hDC, int logPixelsY, BOOL oldWidth);
    int       MaxTitleAscent();
    int       MaxTitleDescent();
    int       MaxTitleLeading();
    int       MaxCellAscent();
    int       MaxCellDescent();
    int       MaxCellLeading();
    COLORREF  TitleBGColor(int index);
    COLORREF  TitleFGColor(int index);
    COLORREF  CellBGColor(int index);
    COLORREF  CellFGColor(int index);
    int       MaxWidth(int index, BOOL autoFill, long first, long numLoaded);

    // objd column methods
    COLUMN(AOBJECTID objID, HGLOBAL& colHandle, int& numCols);
    int       AddColumn(AOBJECTID objID, HGLOBAL& colHandle, int& numCols, int first, TCOLS *defCol);
    void      DeleteColumn(int index, HGLOBAL& colHandle, int& numCols);
    AOBJECTID  GetObjectID(int index, HGLOBAL colHandle);
    char*     Title(int index, TCOLS* colPtr);
    int       Alignment(int index, TCOLS* colPtr);
    int       TitleAlignment(int index, TCOLS* colPtr);
    BOOL      Editable(int index, TCOLS* colPtr);
    int       Pixels(int index, TCOLS* colPtr);
    double    Characters(int index, TCOLS* colPtr);
    double    Inches(int index, TCOLS* colPtr);
    LOGFONT*  TitleFont(int index, TCOLS* colPtr, int logPixelsY, int width);
    COLORREF  TitleFGColor(int index, TCOLS* colPtr);
    COLORREF  TitleBGColor(int index, TCOLS* colPtr);
    LOGFONT*  CellFont(int index, TCOLS* colPtr, int logPixelsY, int width);
    COLORREF  CellFGColor(int index, TCOLS* colPtr);
    COLORREF  CellBGColor(int index, TCOLS* colPtr);
    AOBJECTID  ObjectID(int index, TCOLS* colPtr);
    void      SetTitle(int index, TCOLS* colPtr, char* newTitle);
    void      SetTitleAlignment(int index, TCOLS* colPtr, char state);
    void      SetAlignment(int index, TCOLS* colPtr, int state);
    void      SetEditable(int index, TCOLS* colPtr, BOOL state);
    int       SetPixels(int index, TCOLS* colPtr, int numPixels);
    double    SetCharacters(int index, TCOLS* colPtr, double numChars);
    double    SetInches(int index, TCOLS* colPtr, double numInches);
    void      SetTitleFont(int index, TCOLS* colPtr, LOGFONT* lf, int deciPointSize);
    void      SetTitleFGColor(int index, TCOLS* colPtr, COLORREF color);
    void      SetTitleBGColor(int index, TCOLS* colPtr, COLORREF color);
    void      SetCellFont(int index, TCOLS* colPtr, LOGFONT* lf, int deciPointSize);
    void      SetCellFGColor(int index, TCOLS* colPtr, COLORREF color);
    void      SetCellBGColor(int index, TCOLS* colPtr, COLORREF color);
    void      SetObjectID(int index, TCOLS* colPtr, AOBJECTID newID);

    // methods that use the objd methods
    int       Pixels(int index)                               {return Pixels(index, column_data);}
    LOGFONT*  TitleFont(int index, int logPixelsY, int width) {return TitleFont(index, column_data, logPixelsY, width);}
    LOGFONT*  CellFont(int index, int logPixelsY, int width)  {return CellFont(index, column_data, logPixelsY, width);}
    void      SetPixels(int index, int numPixels)             {SetPixels(index, column_data, numPixels);}
    void      SetTitle(int index, char* title)                {SetTitle(index, column_data, title);}
  };
typedef COLUMN *   LPCOLUMN;

// constants for column class //////////////////////////////////////////////

#define NATV_LOGFONT  AMAKEID('n', 'L', 'o', 'g')

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __TBL_CLMN_H
