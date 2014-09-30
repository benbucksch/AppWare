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
// FILE:          "classes.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "almsys.h"

#include "upsf_env.h"
#include "upsf_cls.h"

// extern GLOBAL class container ///////////////////////////////////////////

extern GLOBAL G;

// static GLOBAL initializer "Carriage Return - Line Feed" /////////////////

char* GLOBAL::CRLF = "\r\n";

void  ConvertString(
  char*       tag,
  const char* name)

  {
  int len = strlen(name);

  if(len>=AUPSF_IDLENGTH)
    memcpy(tag, name, AUPSF_IDLENGTH);
  else
    {
    memcpy(tag, name, len);
    while(len<AUPSF_IDLENGTH)
      {
      // append spaces to the tag to make it AUPSF_IDLENGTH characters long
      tag[len] = ' ';
      len++;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////

UPSF* GetUPSF()

  {
  return G.GetUPSFPtr();
  }

// Methods for SCOPEINFO class /////////////////////////////////////////////

SCOPEINFO::SCOPEINFO(
  long    Pos,
  LPCSTR  Name,
  long    Index,
  long    Length)

  {
  memcpy((void*)name, (const void*)Name, AUPSF_IDLENGTH);
  index         = Index;
  length        = Length;
  hFields       = NULL;
  fieldList     = NULL;
  numFields     = 0L;
  filePosition  = Pos;
  }

////////////////////////////////////////////////////////////////////////////

SCOPEINFO::~SCOPEINFO()

  {
  if(hFields)
    {
    GlobalUnlock(hFields);
    hFields = GlobalFree(hFields);
    }

  hFields = NULL;
  fieldList = NULL;
  }

////////////////////////////////////////////////////////////////////////////

long  SCOPEINFO::LoadElements(
  HFILE hFile,
  UPSF* upsf)

  {
  // this function will only be called during IMPORT!!!

  FIELD   field;
  long    ret;
  long    pos = filePosition + UPSF_HDRSIZE;
  long    max = pos + length - UPSF_HDRSIZE;

  numFields = 0L;
  hFields = GlobalAlloc(MEMFLAGS, (DWORD)sizeof(FIELD));
  if(!hFields)
    return AUPSF_MEMORYERROR;
  fieldList = (FIELD*)GlobalLock(hFields);
  if(!fieldList)
    return AUPSF_MEMORYERROR;

  while(pos < max)
    {
    // read the header & assign the filePosition
    ret = upsf->BufRead(hFile, pos, &field, sizeof(HEADER));
    if(ret < AUPSF_NOERROR)
      return ret;
    field.filePosition = pos; 

    // store the field
    numFields++;
    GlobalUnlock(hFields);
    hFields = GlobalReAlloc(hFields, numFields*sizeof(FIELD), MEMFLAGS);
    if(!hFields)
      return AUPSF_MEMORYERROR; 
    fieldList = (FIELD*)GlobalLock(hFields);
    if(!fieldList)
      return AUPSF_MEMORYERROR;

    fieldList[(size_t)(numFields-1)] = field;

    // increment the position for the next read
    pos += field.length;
    }

  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

FIELD*  SCOPEINFO::FindElement(
  LPCSTR  Name,
  long    Index)

  {
  long    i = 0L;
  while (i < numFields)
    {
    if(memcmp(Name, fieldList[(size_t)i].name, AUPSF_IDLENGTH))
      {
      // the names weren't the same
      i++;
      continue;
      }

    if(Index != fieldList[(size_t)i].index)
      {
      // the indexes weren't the same
      i++;
      continue;
      }

    // there was a match, so return the pointer
    return &fieldList[(size_t)i];
    }

  return NULL;
  }

////////////////////////////////////////////////////////////////////////////

long  SCOPEINFO::CountElements(
  LPCSTR  Name)

  {
  char  name[AUPSF_IDLENGTH];
  ConvertString(name, Name);

  long  i=0L;
  long  count = 0L;
  while(i<numFields)
    {
    if(!memcmp(name, fieldList[(size_t)i].name, AUPSF_IDLENGTH))
      // the names were the same
      count++;

    i++;
    }

  return count;
  }

// Methods for SCOPE class /////////////////////////////////////////////////

SCOPE::SCOPE()

  {
  hScopeInfo = NULL;
  top = NULL;
  depth = 0;
  }

////////////////////////////////////////////////////////////////////////////

SCOPE::~SCOPE()

  {
  while(top)
    Pop();
  }

////////////////////////////////////////////////////////////////////////////

void  SCOPE::Push(
  long    pos,
  LPCSTR  name,
  long    index,
  long    length)

  {
  depth++;

  if(hScopeInfo)
    {
    GlobalUnlock(hScopeInfo);
    hScopeInfo = GlobalReAlloc(hScopeInfo, depth*sizeof(SCOPEINFO), MEMFLAGS);
    }
  else
    hScopeInfo = GlobalAlloc(MEMFLAGS, depth*sizeof(SCOPEINFO));

  scopeinfo = (SCOPEINFO*)GlobalLock(hScopeInfo);
  top = &scopeinfo[depth-1];

  // call the constructor for the new item
  new(top) SCOPEINFO(pos, name, index, length);
  }

////////////////////////////////////////////////////////////////////////////

SCOPEINFO SCOPE::Pop()

  {
  SCOPEINFO tmp = *top;
  tmp.hFields = NULL;
  tmp.fieldList = NULL;
  
  // call the SCOPEINFO destructor
  delete top;

  // free the memory
  depth--;
  GlobalUnlock(hScopeInfo);
  if(depth>0)
    {
    hScopeInfo = GlobalReAlloc(hScopeInfo, depth*sizeof(SCOPEINFO), MEMFLAGS);
    scopeinfo = (SCOPEINFO*)GlobalLock(hScopeInfo);
    top = &scopeinfo[depth-1];
    }
  else
    {
    depth = 0; // make sure the depth is 0!
    hScopeInfo = GlobalFree(hScopeInfo);
    scopeinfo = NULL;
    top = NULL;
    }

  return tmp;
  }

// Methods for UPSF class //////////////////////////////////////////////////

UPSF::UPSF()

  {
  hScope      = GlobalAlloc(MEMFLAGS, sizeof(SCOPE));
  scope       = new(GlobalLock(hScope)) SCOPE;
  hFile       = HFILE_ERROR;
  hLogFile    = HFILE_ERROR;
  state       = UPSF_IDLE;
  fStatus     = 0;

  hBuffer     = GlobalAlloc(MEMFLAGS, FILE_BUFFER_SIZE);  // allocate a 64k buffer
  memUsed     = 0L; // no memory has been used

  hIDList     = GlobalAlloc(MEMFLAGS, 1L);
  numIDs      = 0L;

  hFonts      = NULL;
  numFonts    = 0L;
  }

////////////////////////////////////////////////////////////////////////////

UPSF::~UPSF()

  {
  if(hFile!=HFILE_ERROR)
    CloseUPSFFile();
  if(hLogFile!=HFILE_ERROR)
     CloseLogFile();
  if(hScope)
    {
    delete scope;
    GlobalUnlock(hScope);
    hScope = GlobalFree(hScope);
    }
  if(hBuffer)
    hBuffer = GlobalFree(hBuffer);
  if(hIDList)
    hIDList = GlobalFree(hIDList);
  if(hFonts)
    hFonts = GlobalFree(hFonts);
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::BufRead(
  HFILE file,
  long  pos,
  void* dest,
  long  numBytes)

  {
  BYTE* buf = (BYTE*)GlobalLock(hBuffer);
  if(!buf)
    return AUPSF_MEMORYERROR;

  // if too much data is to be read, read it without buffering
  if(numBytes>FILE_BUFFER_SIZE)
    {
    _llseek(file, pos, SEEK_SET);
    GlobalUnlock(hBuffer);
    return _hread(file, dest, numBytes);
    }

  if(pos>=bufStart && (pos+numBytes)<=(bufStart+memUsed))
    {
    // the information is in the buffer -- copy it to the destination
    long  offset = pos - bufStart;
    memcpy(dest, &buf[(size_t)offset], (size_t)numBytes);
    }
  else
    {
    // fill the buffer starting at pos
    _llseek(file, pos, SEEK_SET);
    memUsed = _hread(file, buf, FILE_BUFFER_SIZE);
    bufStart = pos;

    // copy the information to the destination
    memcpy(dest, buf, (size_t)numBytes);
    }

  GlobalUnlock(hBuffer);
  return numBytes;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::BufWrite(
  HFILE       file,
  const void* source,
  long        numBytes)

  {
  BYTE* data = (BYTE*)GlobalLock(hBuffer);
  if(!data)
    return AUPSF_MEMORYERROR;

  long  size;

  // write the buffer if too much memory has been used
  if((memUsed+numBytes) > FILE_BUFFER_SIZE)
    {
    if(memUsed!=0L)
      {
      size = _hwrite(file, data, memUsed);
      fileLength += size;

      if(size!=memUsed)
        fStatus |= UPSFSTAT_OUTOFDISKSPACE;
      }
    memUsed = 0L;
    }

  // be careful when numBytes > 64k
  if(numBytes > FILE_BUFFER_SIZE)
    {
    size = _hwrite(file, source, numBytes);
    fileLength += size;
    // memUsed is already 0L (from if((memUsed+numBytes)>0x0000FFFF) statement

    if(size!=numBytes)
      fStatus |= UPSFSTAT_OUTOFDISKSPACE;
    }
  else if(numBytes > 0)
    {
    memcpy(&data[(size_t)memUsed], source, (size_t)numBytes);
    memUsed += numBytes;
    }

  GlobalUnlock(hBuffer);
  return numBytes;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::FlushWriteBuf(
  HFILE file)

  {
  BYTE* data = (BYTE*)GlobalLock(hBuffer);
  if(!data)
    return AUPSF_MEMORYERROR;

  long  size;

  if(memUsed>0L)
    {
    size = _hwrite(file, data, memUsed);
    if(size != memUsed)
      fStatus |= UPSFSTAT_OUTOFDISKSPACE;
    }

  memUsed = 0L;

  GlobalUnlock(hBuffer);
  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CheckFileHeader(
  HFILE file)

  {
  HEADER  head;
  BufRead(file, 0L, &head, sizeof(HEADER));

  // make sure this is a upsf file
  if(!memcmp(head.name, "TopScope", AUPSF_IDLENGTH)
        &&
     (head.index==0L)
        &&
     (head.utype==UNIV_SCOPETYPE)
        &&
     (head.length==fileLength))
    {
    return AUPSF_NOERROR;
    }
  else
    return AUPSF_FILEERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CreateUPSFFile(
  LPCSTR  pathName)

  {
  OFSTRUCT  of;

  // create the file
  hFile = OpenFile(pathName, &of, OF_CREATE);
  if(hFile==HFILE_ERROR)
    return (long)of.nErrCode; // return the MS-DOS error code
  _lclose(hFile);

  // open the file for reading and writing
  hFile = OpenFile(pathName, &of, OF_READWRITE | OF_SHARE_EXCLUSIVE);
  if(hFile==HFILE_ERROR)
    return (long)of.nErrCode; // return the MS-DOS error code

  // store the path name for future calls to upsf 
  lstrcpy(path, of.szPathName);
  fileLength = 0L;
  memUsed = 0L;

  state = UPSF_EXPORTING;
  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::OpenUPSFFile(
  LPCSTR  pathName)

  {
  OFSTRUCT  of;

  // open the file for reading only
  hFile = OpenFile(pathName, &of, OF_READ | OF_SHARE_EXCLUSIVE);
  if(hFile==HFILE_ERROR)
    {
    CloseLogFile(); 
    return (long)of.nErrCode; // return the MS-DOS error code
    }

  // store the path name and fileLength for later upsf calls
  lstrcpy(path, of.szPathName);
  fileLength = _llseek(hFile, 0L, SEEK_END);

  // stuff for buffered i/o
  bufStart = 0L;
  memUsed = 0L;

  state = UPSF_IMPORTING;
  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long UPSF::CloseUPSFFile()

  {
  long  ret;

  // if exporting, flush the buffer and then close the file
  if(GetState()==UPSF_EXPORTING)
    {
    CloseScope();
    FlushWriteBuf(hFile);
    }

  if(hFile!=HFILE_ERROR)
    {
    ret = (long)_lclose(hFile);
    hFile = HFILE_ERROR;
    state = UPSF_IDLE;
    }
  else ret = AUPSF_FILECLOSED;

  if(fStatus & UPSFSTAT_OUTOFDISKSPACE)
    {
    // delete the export file
    OFSTRUCT  of;

    OpenFile(path, &of, OF_DELETE);

    if(ret==AUPSF_NOERROR)
      ret = AUPSF_OUTOFDISKSPACE;
    }

  return ret;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CreateLogFile(
  LPCSTR  pathName,
  BOOL    wantLogFile)

  {
  OFSTRUCT  of;
  BOOL      logError;

  if(wantLogFile)
    {
    // create the file
    hLogFile = OpenFile(pathName, &of, OF_CREATE);
    if(hLogFile != HFILE_ERROR)
      {
      _lclose(hLogFile);
      // open the file for reading and writing
      hLogFile = OpenFile(pathName, &of, OF_READWRITE | OF_SHARE_EXCLUSIVE);
      if(hLogFile==HFILE_ERROR)
        logError = TRUE;
      }
    else
      logError = TRUE;
    }
  else
    hLogFile = HFILE_ERROR;

  if(logError)
    return AUPSF_LOGERROR;
  else
    return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::RegisterObjectID(
  OBJECTID  nativeID,
  long      upsfID)

  {
  IDLIST  ids = {upsfID, nativeID};
  IDLIST* idlist = (IDLIST*)GlobalLock(hIDList);
  if(numIDs!=0L && idlist)
    {
    long i = 0L;
    while (i<numIDs)
      {
      if(upsfID == idlist[(size_t)i].univID)
        {
        GlobalUnlock(hIDList);
        return AUPSF_INVALIDOBJECT;
        }
      i++;
      }
    }

  else if(!idlist && numIDs!=0L)
    {
    GlobalUnlock(hIDList);
    return AUPSF_MEMORYERROR;
    }

  // allocate some space and put the new item at the end
  numIDs++;
  GlobalUnlock(hIDList);
  hIDList = GlobalReAlloc(hIDList, sizeof(IDLIST)*numIDs, MEMFLAGS);
  idlist = (IDLIST*)GlobalLock(hIDList);
  if(!idlist)
    return AUPSF_MEMORYERROR;

  idlist[(size_t)(numIDs-1L)] = ids;
  GlobalUnlock(hIDList);
  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::ClearObjectIDs()

  {
  numIDs = 0L;

  hIDList = GlobalReAlloc(hIDList, 1L, MEMFLAGS);

  if(!hIDList)
    return AUPSF_MEMORYERROR;
  else
    return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long UPSF::ValidateScope
  (
  LPCSTR  name,
  long    index
  )

  {
  SCOPEINFO*  si = scope->GetTop();
  long        lRet;
  char        tag[AUPSF_IDLENGTH];

  ConvertString(tag, name);

  if(si && !strncmp(tag, si->name, AUPSF_IDLENGTH) && (index==si->index))
    lRet = AUPSF_SCOPEOK;
  else
    lRet = AUPSF_NOERROR; // no serious errors, but it didn't match

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CreateScope(
  LPCSTR  name,
  long    index)

  {
  if(hFile==HFILE_ERROR)
    return AUPSF_SCOPEERROR;

  if(state!=UPSF_EXPORTING)
    return AUPSF_NOTEXPORTING;

  if(fStatus & UPSFSTAT_OUTOFDISKSPACE)
    return AUPSF_OUTOFDISKSPACE;

  HEADER  hdr;
  ConvertString(hdr.name, name);
  hdr.index   = index;
  hdr.utype   = UNIV_SCOPETYPE;
  hdr.length  = 0L;

  scope->Push(fileLength + memUsed, hdr.name, hdr.index, hdr.length);

  if(BufWrite(hFile, &hdr, sizeof(HEADER))>0L)
    {
    if(fStatus & UPSFSTAT_OUTOFDISKSPACE)
      return AUPSF_OUTOFDISKSPACE;
    else
      return AUPSF_NOERROR;
    }
  else
    return AUPSF_EXPORTERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::OpenScope(
  LPCSTR  name,
  long    index)

  {
  if(hFile==HFILE_ERROR)
    return AUPSF_SCOPEERROR;

  if(state!=UPSF_IMPORTING)
    return AUPSF_NOTIMPORTING;

  char  tag[AUPSF_IDLENGTH];
  ConvertString(tag, name);

  if(memcmp(tag, "TopScope", AUPSF_IDLENGTH))
    {
    // this is not "TopScope" -- try to find the scope
    FIELD*  field = scope->FindElement(tag, index);
    if(!field)
      return AUPSF_SCOPENOTEXIST;
    scope->Push(field->filePosition, tag, index, field->length);
    }
  else
    {
    long  ret = CheckFileHeader(hFile);
    if(ret != AUPSF_NOERROR)
      return ret;

    scope->Push(0L, tag, index, fileLength);
    }

  return scope->LoadElements(hFile, this);
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CloseScope()

  {
  if(hFile==HFILE_ERROR)
    return AUPSF_SCOPEERROR;

  if(state==UPSF_EXPORTING)
    {
    // update the scope length when EXPORTING
    SCOPEINFO scopeinfo = scope->Pop();
    long      recLength = fileLength + memUsed - scopeinfo.filePosition;

    // write the correct length to the file
    if(fileLength>=(scopeinfo.filePosition + UPSF_SIZEOFFSET))
      {
      // write directly to the file
      _llseek(hFile, (scopeinfo.filePosition + UPSF_SIZEOFFSET), SEEK_SET);
      _lwrite(hFile, (char*)&recLength, sizeof(long));

      // reset the file pointer to the end of the file
      _llseek(hFile, 0L, SEEK_END);
      }
    else
      {
      // write to the memory buffer
      BYTE* mem = (BYTE*)GlobalLock(hBuffer);
      if(!mem)
        return AUPSF_MEMORYERROR;
      UINT  offset = scopeinfo.filePosition + UPSF_SIZEOFFSET - fileLength;
      memcpy(&mem[offset], &recLength, sizeof(long));
      GlobalUnlock(hBuffer);
      }
    }
  else if(state==UPSF_IMPORTING)
    {
    // pop the scope stack when IMPORTING
    scope->Pop();
    }
  else
    return AUPSF_SCOPEERROR;

  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::ScopeLength()

  {
  if(state!=UPSF_IMPORTING)
    return AUPSF_NOTIMPORTING;

  return scope->GetLength();
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::CloseLogFile  ()

  {
  if(hLogFile!=HFILE_ERROR)
    {
    long  len = _llseek(hLogFile, 0L, SEEK_END);
    if(len==0L)
      {
      // no errors occurred .... write "No Errors" to the log file ....
      _lwrite(hLogFile, "No Errors", 9);
      }

    long  ret = _lclose(hLogFile);
    hLogFile = HFILE_ERROR;
    return (ret==0) ? AUPSF_NOERROR : AUPSF_LOGERROR;
    }
  else
    return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::LogComment(
  LPCSTR  comment)

  {
  if(state!=UPSF_IMPORTING)
    return AUPSF_LOGERROR;

  // don't attempt to log if the file handle is invalid
  if(hLogFile==HFILE_ERROR)
    return AUPSF_NOERROR;

  UINT  len = strlen(comment) + strlen(G.CRLF);
  UINT  size;
  size = _lwrite(hLogFile, comment, strlen(comment));
  size += _lwrite(hLogFile, G.CRLF, strlen(G.CRLF));

  if((UINT)size != len)
    {
    len = strlen("\r\nOUT OF DISK SPACE");
    _llseek(hLogFile, len, SEEK_END);
    _lwrite(hLogFile, "\r\nOUT OF DISK SPACE", len);
    _lclose(hLogFile);
    hLogFile = HFILE_ERROR;
    }

  return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::ExportField (
  LPCSTR  fieldName,
  long    fieldIndex,
  long    nativeType,
  long    universalType,
  void*   sourceAddress,
  long    sourceSize)

  {
  if(state != UPSF_EXPORTING)
    return AUPSF_NOTEXPORTING;

  if(fStatus & UPSFSTAT_OUTOFDISKSPACE)
    return AUPSF_OUTOFDISKSPACE;

  HGLOBAL hUniv = GlobalAlloc(MEMFLAGS, 1L);
  if(!hUniv)
    return AUPSF_MEMORYERROR;

  // convert the data to universal types
  long  univSize = 0L;
  long  error = NativeToUniv(nativeType, sourceAddress, sourceSize, universalType, hUniv, univSize);
  if(error!=AUPSF_NOERROR)
    {
    GlobalFree(hUniv);
    return error;
    }

  HEADER  hdr;
  ConvertString(hdr.name, fieldName);
  hdr.index   = fieldIndex;
  hdr.utype   = universalType;
  hdr.length  = univSize + UPSF_HDRSIZE;

  BYTE* data = (BYTE*)GlobalLock(hUniv);
  if(!data && (univSize!=0L))
    {
    GlobalFree(hUniv);
    return AUPSF_MEMORYERROR;
    }

  // write the universal information to the file and check for errors
  BufWrite(hFile, &hdr, sizeof(HEADER));
  BufWrite(hFile, data, (hdr.length - UPSF_HDRSIZE));

  GlobalUnlock(hUniv);
  GlobalFree(hUniv);

  if(fStatus & UPSFSTAT_OUTOFDISKSPACE)
    return AUPSF_OUTOFDISKSPACE;
  else
    return AUPSF_NOERROR;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::ImportField (
  LPCSTR  fieldName,
  long    fieldIndex,
  long    nativeType,
  void*   destinationAddress,
  long    destinationSize)

  {
  if(state != UPSF_IMPORTING)
    return AUPSF_NOTIMPORTING;

  char  tag[AUPSF_IDLENGTH];
  ConvertString(tag, fieldName);

  // find the requested field
  FIELD*  field = scope->FindElement(tag, fieldIndex);
  if(!field)
    return AUPSF_FIELDNOTEXIST;

  long    usize = field->length - UPSF_HDRSIZE;

  HGLOBAL hData = NULL;
  BYTE*   data = NULL;

  if(usize > 0L)
    {
    hData = GlobalAlloc(MEMFLAGS, usize+1);
    if(!hData)
      return AUPSF_MEMORYERROR;
  
    data = (BYTE*)GlobalLock(hData);
    if(!data)
      {
      GlobalFree(hData);
      return AUPSF_MEMORYERROR;
      }

    if(BufRead(hFile, field->filePosition + UPSF_HDRSIZE, data, usize)!=usize)
      {
      GlobalUnlock(hData);
      GlobalFree(hData);
      return AUPSF_IMPORTERROR;
      }
    }

  // convert the data to the requested native type and return the error
  long  error = UnivToNative(field->utype, data, usize, nativeType, destinationAddress, destinationSize);
  if(usize>0L)
    {
    GlobalUnlock(hData);
    GlobalFree(hData);
    }
  return error;
  }

////////////////////////////////////////////////////////////////////////////

long  UPSF::FieldInfo(
  LPCSTR  fieldName,
  long    fieldIndex,
  long    nativeType,
  long*   universalType,
  long*   convertedSize)

  {
  if(state != UPSF_IMPORTING)
    return AUPSF_NOTIMPORTING;

  char  tag[AUPSF_IDLENGTH];
  ConvertString(tag, fieldName);

  // find the requested field
  FIELD*  field = scope->FindElement(tag, fieldIndex);
  if(!field)
    return AUPSF_FIELDNOTEXIST;

  long    usize = field->length - UPSF_HDRSIZE;

  HGLOBAL hData = NULL;
  BYTE*   data = NULL;

  if(usize > 0L)
    {
    hData = GlobalAlloc(MEMFLAGS, usize);
    if(!hData)
      return AUPSF_MEMORYERROR;

    data = (BYTE*)GlobalLock(hData);
    if(!data)
      {
      GlobalFree(hData);
      return AUPSF_MEMORYERROR;
      }

    if(BufRead(hFile, field->filePosition + UPSF_HDRSIZE, data, usize)!=usize)
      {
      GlobalUnlock(hData);
      GlobalFree(hData);
      return AUPSF_IMPORTERROR;
      }
    }

  long  rsize = RequiredSize(nativeType, field->utype, usize, hData);

  if(rsize < AUPSF_NOERROR)
    {
    if(usize > 0L)
      {
      GlobalUnlock(hData);
      GlobalFree(hData);
      }
    return rsize;
    }

  // assign the input variables
  if(universalType)
    *universalType = field->utype;

  if(convertedSize)
    *convertedSize = rsize;

  if(usize > 0L)
    {
    GlobalUnlock(hData);
    GlobalFree(hData);
    }
  return AUPSF_NOERROR;
  }

// Methods for GLOBAL class ////////////////////////////////////////////////

GLOBAL::GLOBAL()

  {
  hGlobalList = NULL;
  pGlobalList = NULL;
  gCount = 0L;
  }

// Destructor //////////////////////////////////////////////////////////////

GLOBAL::~GLOBAL()

  {
  if(hGlobalList)
    {
    GlobalUnlock(hGlobalList);
    GlobalFree(hGlobalList);
    }
  }

// GetGlobalData(HTASK) ////////////////////////////////////////////////////

GLOBALDATA* GLOBAL::GetGlobalData(
  HTASK hTask)

  {
  if(!hGlobalList && gCount > 0L)
    {
    hGlobalList = GlobalAlloc(MEMFLAGS, sizeof(GLOBALDATA));
    pGlobalList = GlobalLock(hGlobalList);
    }

  if(gCount <= 0L)
    return NULL;

  GLOBALDATA* gData = (GLOBALDATA*)pGlobalList;
  if(!gData)
    return NULL;

  for(long i=0L; i<gCount; i++)
    {
    if(gData->hTask == hTask)
      return gData;

    gData++;
    }

  return NULL;
  }

// AddGlobalData(HTASK) ////////////////////////////////////////////////////

GLOBALDATA* GLOBAL::AddGlobalData(
  HTASK hTask)

  {
  if(!hGlobalList)
    {
    hGlobalList = GlobalAlloc(MEMFLAGS, sizeof(GLOBALDATA));
    pGlobalList = GlobalLock(hGlobalList);
    }

  GlobalUnlock(hGlobalList);
  hGlobalList = GlobalReAlloc(hGlobalList, (gCount+1L)*sizeof(GLOBALDATA), MEMFLAGS);
  pGlobalList = GlobalLock(hGlobalList);
  GLOBALDATA* gData = (GLOBALDATA*)pGlobalList;

  if(!gData)
    return NULL;

  gData += (size_t)gCount;

  // initialize the new record
  memset(gData, 0, sizeof(GLOBALDATA));
  gData->hTask = hTask;

  // increment the number of global records
  gCount++;

  return gData;
  }

// RemoveGlobalData(HTASK) /////////////////////////////////////////////////

void  GLOBAL::RemoveGlobalData(
  HTASK hTask)

  {
  if(gCount <= 0)
    {
    if(hGlobalList)
      {
      GlobalUnlock(hGlobalList);
      pGlobalList = NULL;
      hGlobalList = GlobalFree(hGlobalList);
      }
    return;
    }

  GLOBALDATA* gData = (GLOBALDATA*)pGlobalList;
  if(!gData)
    return;

  for(long i=0; i<gCount; i++)
    {
    if(gData[(size_t)i].hTask == hTask)
      break;
    }

  if(i>=gCount)
    return;

  // do the remove stuff -- compact the array
  hmemcpy(&gData[(size_t)i], &gData[(size_t)(i+1)], sizeof(GLOBALDATA)*(gCount-(i+1)));
  gCount--;

  if(gCount <= 0)
    {
    if(hGlobalList)
      {
      GlobalUnlock(hGlobalList);
      pGlobalList = NULL;
      hGlobalList = GlobalFree(hGlobalList);
      }
    }
  else
    {
    GlobalUnlock(hGlobalList);
    hGlobalList = GlobalReAlloc(hGlobalList, gCount*sizeof(GLOBALDATA), MEMFLAGS);
    pGlobalList = GlobalLock(hGlobalList);
    }
  }

// GlobalData() ////////////////////////////////////////////////////////////

GLOBALDATA* GLOBAL::GlobalData()

  {
  HTASK       hTask = GetCurrentTask();
  GLOBALDATA* gData;

  gData = GetGlobalData(hTask);
  if(!gData)
    gData = AddGlobalData(hTask);

  return gData;
  }

// GetUPSFPtr() ////////////////////////////////////////////////////////////

UPSF* GLOBAL::GetUPSFPtr()

  {
  GLOBALDATA* gData = GetGlobalData(GetCurrentTask());

  if(gData)
    return gData->upsf;
  else
    return NULL;
  }

// AddUPSFPtr() ////////////////////////////////////////////////////////////

UPSF* GLOBAL::AddUPSFPtr()

  {
  GLOBALDATA* gData = AddGlobalData(GetCurrentTask());

  // call the constructor for the newly allocated (in AddGlobalData) UPSF pointer
  if(gData)
    return new(gData->upsf) UPSF;
  else
    return NULL;
  }

// RemoveUPSFPtr(UPSF*) ////////////////////////////////////////////////////

void  GLOBAL::RemoveUPSFPtr(UPSF* upsf)

  {
  HTASK       hTask = GetCurrentTask();
  GLOBALDATA* gData = GetGlobalData(hTask);

  if(gData->upsf == upsf)
    {
    // call the UPSF destructor
    delete upsf;

    RemoveGlobalData(hTask);
    } 
  }

////////////////////////////////////////////////////////////////////////////
