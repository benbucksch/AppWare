////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "upsf_cls.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef   UPSF_CLS_H
#define   UPSF_CLS_H

#ifndef FNAMEOBJ_H
#include "fnameobj.h"
#endif	// !FNAMEOBJ_H

#ifndef   __UPSF_ENV_H
#include  "upsf_env.h"
#endif    // #ifndef __UPSF_ENV_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif


#define MEMFLAGS  GMEM_MOVEABLE

// class prototypes ////////////////////////////////////////////////////////

class UPSF;

// ID translator structure /////////////////////////////////////////////////

struct IDLIST {
  long      univID;
  OBJECTID  nativeID;
  };

// FACES structure -- font face names //////////////////////////////////////

struct  FACES
  {
  char      faceName[LF_FACESIZE];
  };

// UPSF Record header structure ////////////////////////////////////////////

struct  HEADER
  {
  char      name[AUPSF_IDLENGTH];
  long      index;
  TYPEID    utype;
  long      length;
  };

// Linked list of elements in the each UPSF scope ////////////////////////// 

struct  FIELD
  {
  char      name[AUPSF_IDLENGTH];
  long      index;
  TYPEID    utype;
  long      length;
  long      filePosition;
  };

// Stack elements of the UPSF scopes ///////////////////////////////////////

class SCOPEINFO
  {
  private:

  public:
    char        name[AUPSF_IDLENGTH]; // tag of the current scope
    long        index;                // index of the current scope
    long        length;               // length of the current scope
    HGLOBAL     hFields;              // array of fields in the current scope
    FIELD*      fieldList;
    long        numFields;            // number of fields in the current scope
    long        filePosition;         // file position for the start of the scope

    // constructor and destructor
    SCOPEINFO(long Pos, LPCSTR Name, long Index, long Length);
    ~SCOPEINFO();

    // overloaded new and delete operators
    void* operator new(size_t /*numBytes*/, void* scopeinfo)  {return scopeinfo;}
    void  operator delete(void* /*scopeinfo*/)                {return;}

    // methods for managing the elements list
    long        LoadElements(HANDLE hFile, UPSF* upsf);
    FIELD*      FindElement(LPCSTR Name, long Index);
    long        CountElements(LPCSTR Name);
  };

// Stack for the UPSF scopes ///////////////////////////////////////////////

class SCOPE
  {
  private:
    HGLOBAL     hScopeInfo;
    SCOPEINFO*  scopeinfo;
    SCOPEINFO*  top;
    int         depth;

  public:
    // constructor and destructor
    SCOPE(void);
    ~SCOPE();

    // overloaded new and delete operators
    void* operator new(size_t /*numBytes*/, void* scope)  {return scope;}
    void  operator delete(void* /*scope*/)                {return;}

    // methods for SCOPE class
    void        Push(long pos, LPCSTR name, long index, long length);
    SCOPEINFO   Pop(void);

    // inline methods for SCOPE class
    SCOPEINFO*  GetTop(void)                          {return top;}
    long        GetLength(void)                       {return top->length;}
    long        SetLength(long l)                     {return top->length = l;}
    long        LoadElements(HANDLE hFile, UPSF* upsf) {return top->LoadElements(hFile, upsf);}
    FIELD*      FindElement(LPCSTR Name, long Index)  {return top->FindElement(Name, Index);}
    long        CountElements(LPCSTR Name)            {return top->CountElements(Name);}
  };

////////////////////////////////////////////////////////////////////////////

#define UPSFSTAT_OUTOFDISKSPACE   0x0001
#define UPSFSTAT_MEMORYERROR      0x0002

////////////////////////////////////////////////////////////////////////////

class UPSF
  {
  private:
    HGLOBAL   hScope;     // scope handle
    SCOPE*    scope;      // scope stack pointer

    HANDLE    hFile;      // file handle for UPSF routines
    HANDLE    hLogFile;   // file handle for the LogComment routine
    long      state;      // UPSF state: IDLE, EXPORTING, IMPORTING
    long      fileLength; // the current length of the file (EXPORT)
    long      bufStart;   // buffer starting point (IMPORT)

    HGLOBAL   hBuffer;    // buffered i/o memory handle
    long      memUsed;    // # of bytes used from the buffer

    HGLOBAL   hIDList;    // list of Windows OBJECTIDs and UPSF objectRefs
    long      numIDs;     // the total number of IDs in the list

    WORD      fStatus;    // UPSFSTAT_xxxx flags
    SFilePath	path;			  // path name for the upsf file

  public:
    HGLOBAL   hFonts;     // handle to array of font face names
    long      numFonts;   // number of different font face names

    // constructor and destructor
    UPSF();
    ~UPSF();

    // inline methods to status access
    inline void SetMemoryError()                        {fStatus |= UPSFSTAT_MEMORYERROR;}
    inline BOOL MemoryError()                           {return (fStatus & UPSFSTAT_MEMORYERROR) ? TRUE : FALSE;}

    // buffered i/o methods for UPSF class
    long        BufRead(HANDLE file, long pos, void* dest, long numBytes);
    long        BufWrite(HANDLE file, const void* source, long numBytes);
    long        FlushWriteBuf(HANDLE file);
    long        CheckFileHeader(HANDLE file);

    // methods for UPSF class
    DWORD       CreateUPSFFile(LPCSTR pathName);
    DWORD       OpenUPSFFile(LPCSTR pathName);
    long        CloseUPSFFile(void);
    long        CreateLogFile(LPCSTR pathName, BOOL wantLogFile);
    long        RegisterObjectID(OBJECTID nativeID, TYPEID upsfID);
    long        ClearObjectIDs(void);
    long        ValidateScope(LPCSTR tag, long index);
    long        CreateScope(LPCSTR tag, long index);
    long        OpenScope(LPCSTR tag, long index);
    long        CloseScope(void);
    long        ScopeLength(void);
    long        CloseLogFile(void);
    long        LogComment(LPCSTR comment);
    long        ExportField(LPCSTR fieldName, long fieldIndex, long nativeType, long universalType, LPCVOID sourceAddress, long sourceSize);
    long        ImportField(LPCSTR fieldName, long fieldIndex, long nativeType, void* destinationAddress, long destinationSize);
    long        FieldCount(LPCSTR fieldName)  {return scope->CountElements(fieldName);}
    long        FieldInfo(LPCSTR fieldName, long fieldIndex, long nativeType, long* universalType, long* convertedSize);

    // methods for determining the required native size
    long        RequiredSize(long ntype, long utype, long usize, HGLOBAL hUniv);
    long        Req_Size_For_UNum(long natvType);
    long        Req_Size_For_UInt(long natvType);
    long        Req_Size_For_UTxt(long natvType, long univLen, HGLOBAL hUniv);
    long        Get_Req_String_Size(long univLen, HGLOBAL hUniv);
    long        Req_Size_For_UBit(long natvType, long univLen, HGLOBAL hUniv);
    long        Get_Req_Bitmap_Size(long univLen, HGLOBAL hUniv);
    long        Get_Req_BitmapCore_Size(long univLen, HGLOBAL hUniv);
    long        Req_Size_For_URef(long natvType);
    long        Req_Size_For_UClr(long natvType);
    long        Req_Size_For_UPnt(long natvType);
    long        Req_Size_For_URct(long natvType);
    long        Req_Size_For_UFon(long natvType);

    // UnivToNative translation methods for UPSF class
    long        UnivToNative(long univ, void* univPtr, long univLen, long native, void* nativePtr, long nativeLen);
    long        Uxxx_To_Nxxx(void* univPtr, long univLen, void* natvPtr, long natvLen);
    long        UNum_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    BYTE        UNum_To_Byte(void*  univPtr,  long  univLen);
    char        UNum_To_Char(void*  univPtr,  long  univLen);
    WORD        UNum_To_Word(void*  univPtr,  long  univLen);
    short       UNum_To_Short(void* univPtr,  long  univLen);
    DWORD       UNum_To_DWord(void* univPtr,  long  univLen);
    long        UNum_To_Long(void*  univPtr,  long  univLen);
    float       UNum_To_Float(void* univPtr,  long  univLen);
    double      UNum_To_Double(void*  univPtr,  long  univLen);
    long double UNum_To_LDouble(void* univPtr,  long  univLen);
    long        UInt_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    long        UTxt_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    long        UTxt_To_String(void* natvPtr, long natvLen, void* univPtr, long univLen);
    long        UBit_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    long        UBit_To_BitmapInfo(void* univPtr, long univLen, void* natvPtr, long natvLen);
    long        UBit_To_BitmapCore(void* univPtr, long univLen, void* natvPtr, long natvLen);
    long        UBit_To_Bitmap(void* univPtr, long univLen, void* natvPtr, long natvLen);
    long        URef_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    long        URef_To_ObjID(void* univPtr);
    OBJECTID    URef_To_Object(void*  univPtr,  OBJECTID& natvID);
    long        GetNativeID(long upsfID, OBJECTID& natvID);
    long        UClr_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    COLORREF    UClr_To_RGBColor(void*  univPtr);
    long        UPnt_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    POINT       UPnt_To_Point(void* univPtr);
    long        URct_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    RECT        URct_To_Rect(void*  univPtr);
    long        UFon_To_xxxx(void* univPtr, long univLen, long natv, void* natvPtr, long natvLen);
    long        UFon_To_Font(void* natvPtr, void* univPtr, long univLen);

    // NativeToUniv translation methods for UPSF class
    long        NativeToUniv(long native, LPCVOID nativePtr, long nativeLen, long univ, HGLOBAL& hUniv, long& univSize);
    long        Nxxx_To_Uxxx(HGLOBAL& hUniv, long& univSize, LPCVOID natvPtr, long natvLen);
    long        xxxx_To_UNum(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        Byte_To_UNum(HGLOBAL& hUniv, long& univSize, BYTE byte);
    long        Char_To_UNum(HGLOBAL& hUniv, long& univSize, char Char);
    long        Word_To_UNum(HGLOBAL& hUniv, long& univSize, WORD word);
    long        Short_To_UNum(HGLOBAL& hUniv, long& univSize, short Short);
    long        DWord_To_UNum(HGLOBAL& hUniv, long& univSize, DWORD dword);
    long        Long_To_UNum(HGLOBAL& hUniv, long& univSize, long Long);
    long        Float_To_UNum(HGLOBAL& hUniv, long& univSize, float Float);
    long        Double_To_UNum(HGLOBAL& hUniv, long& univSize, double Double);
    long        LDouble_To_UNum(HGLOBAL& hUniv, long& univSize, long double LDouble);
    long        xxxx_To_UInt(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        xxxx_To_UTxt(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        String_To_UTxt(HGLOBAL& hUniv, long& univSize, char * string, long length);
    long        xxxx_To_UBit(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        BITMAP_To_UBit(HGLOBAL& hUniv, long& univSize, BITMAP bitmap, HBITMAP hBmp = NULL);
    long        Bitmap_To_UBit(HGLOBAL& hUniv, long& univSize, LPCVOID natvPtr, long natvLen);
    long        BitmapInfo_To_UBit(HGLOBAL& hUniv, long& univSize, LPCVOID natvPtr, long natvLen);
    long        BitmapCore_To_UBit(HGLOBAL& hUniv, long& univSize, LPCVOID natvPtr, long natvLen);
    long        hBitmap_To_UBit(HGLOBAL& hUniv, long& univSize, HBITMAP hBitmap, long natvLen);
    long        hBrush_To_UBit(HGLOBAL& hUniv, long& univSize, HBRUSH hBrush, long natvLen);
    long        hMetaFile_To_UBit(HGLOBAL& hUniv, long& univSize, HMETAFILE hMetaFile, long natvLen);
    long        xxxx_To_URef(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        xxxx_To_UClr(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        RGBColor_To_UClr(short* univPtr, COLORREF rgbColor);
    long        xxxx_To_UPnt(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        Point_To_UPnt(long* univPtr, POINT point);
    long        xxxx_To_URct(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);
    long        Rect_To_URct(long* univPtr, RECT rect);
    long        xxxx_To_UFon(HGLOBAL& hUniv, long& univSize, long natv, LPCVOID natvPtr, long natvLen);

    // inline methods for UPSF class
    long        GetState(void)  {return state;}
  };

// Global Class for managing global data ///////////////////////////////////

class GLOBAL
  {
  private:
		UPSF*					m_pUPSF;

  public:
    static char*  CRLF;

    // constructor & destructor
    GLOBAL();
    ~GLOBAL();

    // methods
    UPSF*         GetUPSFPtr();
    UPSF*         AddUPSFPtr();
    void          RemoveUPSFPtr(UPSF* upsf);
  };

// Internal UPSF constants /////////////////////////////////////////////////

#define     BRUSH_WIDTH       8
#define     BRUSH_HEIGHT      8

#define     UNIV_SCOPETYPE    MAKEID('S', 'C', 'O', 'P')

#define     UPSF_IDLE         1L
#define     UPSF_EXPORTING    2L
#define     UPSF_IMPORTING    3L

#define     UPSF_HDRSIZE      (long)(sizeof(char)*8 + sizeof(long) * 3)
#define     UPSF_SIZEOFFSET   (long)(sizeof(char)*8 + sizeof(long) * 2)

#define     SEEK_SET          0
#define     SEEK_CUR          1
#define     SEEK_END          2

#define     ATTRIB_NORMAL     0
#define     ATTRIB_READONLY   1
#define     ATTRIB_HIDDEN     2
#define     ATTRIB_SYSTEM     3

#define     FILE_BUFFER_SIZE  0x0000FFFEL

// Internal function prototypes ////////////////////////////////////////////

extern "C"
  {
  int CALLBACK  EnumAllFaces(const ENUMLOGFONT* lf, const NEWTEXTMETRIC* /*tm*/, int /*nFontType*/, LPARAM lData);
  }

void  ConvertString(char* tag, const char* name);

UPSF* GetUPSF();

// inline pow2 function ////////////////////////////////////////////////////

inline long pow2
  (
  WORD  exponent
  )

  {
  return (0x00000001 << exponent);
  }

////////////////////////////////////////////////////////////////////////////

#endif  //#ifdef  UPSF_CLS_H
