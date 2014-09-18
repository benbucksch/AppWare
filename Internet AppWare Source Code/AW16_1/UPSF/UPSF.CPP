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
// FILE:          "upsf.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#include  "almsys.h"

#include  "upsf_cls.h"

////////////////////////////////////////////////////////////////////////////

GLOBAL      G;

// UPSF InitExport function ////////////////////////////////////////////////

long ALMAPI upsfInitExport
  (
  LPCSTR    pathName
  )

  {
  long    lRet = AUPSF_NOERROR;
  UPSF*   upsf = GetUPSF();

  if(upsf)
    {
    G.RemoveUPSFPtr(upsf); // recover from an error
    upsf = GetUPSF();
    if(upsf)
      // there should be no pointer!!
      lRet = AUPSF_CANTINITEXPORT;
    }

  if(lRet == AUPSF_NOERROR)
    {
    upsf = G.AddUPSFPtr();
    if(!upsf)
      lRet = AUPSF_MEMORYERROR;

    lRet = upsf->CreateUPSFFile(pathName);
    if(lRet==AUPSF_NOERROR)
      lRet = upsf->CreateScope("TopScope", 0L);
    }

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF InitImport /////////////////////////////////////////////////////////

long ALMAPI upsfInitImport
  (
  LPCSTR    pathName,
  BOOL      wantLogFile,
  LPCSTR    logPathName
  )

  {
  long    lRet = AUPSF_NOERROR;
  UPSF*   upsf = GetUPSF();

  if(upsf)
    {
    G.RemoveUPSFPtr(upsf); // recover from an error
    upsf = GetUPSF();
    if(upsf)
      // there should be no pointer!!
      lRet = AUPSF_CANTINITIMPORT;
    }

  if(lRet == AUPSF_NOERROR)
    {
    upsf = G.AddUPSFPtr();
    if(!upsf)
      lRet = AUPSF_MEMORYERROR;

    if(lRet == AUPSF_NOERROR)
      {
      BOOL  logError = FALSE;

      lRet = upsf->CreateLogFile(logPathName, wantLogFile);
      if(lRet!=AUPSF_NOERROR)
        logError = TRUE;

      lRet = upsf->OpenUPSFFile(pathName);
      if(lRet==AUPSF_NOERROR)
        {
        lRet = upsf->OpenScope("TopScope", 0L);
        if(lRet==AUPSF_NOERROR && logError)
          lRet = AUPSF_LOGERROR;
        }
      }
    }

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF CloseExport ////////////////////////////////////////////////////////

long ALMAPI upsfCloseExport
  (
  )

  {
  UPSF*   upsf = GetUPSF();
  long    lRet;

  if(upsf)
    {
    if(upsf->GetState()!=UPSF_EXPORTING)
      lRet = AUPSF_NOTEXPORTING;
    else
      {
      lRet = upsf->CloseUPSFFile();
      G.RemoveUPSFPtr(upsf);
      }
    }
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF CloseImport ////////////////////////////////////////////////////////

long ALMAPI upsfCloseImport
  (
  )

  {
  UPSF*   upsf = GetUPSF();
  long    lRet;

  if(upsf)
    {
    if(upsf->GetState()!=UPSF_IMPORTING)
      lRet = AUPSF_NOTIMPORTING;
    else
      {
      lRet = upsf->CloseUPSFFile();
      G.RemoveUPSFPtr(upsf);
      }
    }
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF UpdateObjectID /////////////////////////////////////////////////////

long ALMAPI upsfRegisterObjectID
  (
  OBJECTID  nativeID,
  LONG      upsfID
  )

  {
  UPSF*   upsf = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    {
    if(upsf->GetState()!=UPSF_IMPORTING)
      lRet = AUPSF_NOTIMPORTING;
    else
      lRet = upsf->RegisterObjectID(nativeID, upsfID);
    }
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF ClearObjectIDs /////////////////////////////////////////////////////

long ALMAPI upsfClearObjectIDs
  (
  )

  {
  UPSF*   upsf = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    {
    if(upsf->GetState()!=UPSF_IMPORTING)
      lRet = AUPSF_NOTIMPORTING;
    else
      lRet = upsf->ClearObjectIDs();
    }
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF ValidateScope //////////////////////////////////////////////////////

long ALMAPI upsfValidateScope
  (
  LPCSTR  szScopeName,
  long    lScopeIndex
  )

  {
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  // a valid UPSF pointer must exist or there is an error
  if(upsf && !upsf->MemoryError())
    lRet = upsf->ValidateScope(szScopeName, lScopeIndex);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF CreateScope ////////////////////////////////////////////////////////

long ALMAPI AUpsfCreateScope
  (
  LPCSTR    scopeName,
  LONG      index
  )

  { // available only at Export time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->CreateScope(scopeName, index);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF OpenScope //////////////////////////////////////////////////////////

long ALMAPI AUpsfOpenScope
  (
  LPCSTR    scopeName,
  LONG      index
  )

  { // available only at Import time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->OpenScope(scopeName, index);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF CloseScope /////////////////////////////////////////////////////////

long ALMAPI AUpsfCloseScope
  (
  )

  {
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->CloseScope();
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
}

// UPSF ScopeLength ////////////////////////////////////////////////////////

long ALMAPI upsfScopeLength (VOID)

{
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->ScopeLength();
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
}

// UPSF LogComment /////////////////////////////////////////////////////////

long ALMAPI AUpsfLogComment
  (
  LPCSTR    comment
  )

  { // available only at Import time
  UPSF*   upsf = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->LogComment(comment);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF ExportField ////////////////////////////////////////////////////////

long ALMAPI AUpsfExportField
  (
  LPCSTR    szName,
  LONG      nIndex,
  TYPEID    nativeType,
  TYPEID    universalType,
  LPVOID    sourceAddress,
  LONG      sourceSize
  )

  { // available only at Export time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->ExportField(szName, nIndex, nativeType, universalType, sourceAddress, sourceSize);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF ImportField ////////////////////////////////////////////////////////

long ALMAPI AUpsfImportField
  (
  LPCSTR    fieldName,
  LONG      fieldIndex,
  TYPEID    nativeType,
  LPVOID    destinationAddress,
  LONG      destinationSize
  )

  { // available only at Import time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->ImportField(fieldName, fieldIndex, nativeType, destinationAddress, destinationSize);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF FieldCount /////////////////////////////////////////////////////////

long ALMAPI AUpsfFieldCount
  (
  LPCSTR    fieldName
  )

  { // available only at Import time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->FieldCount(fieldName);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

// UPSF FieldInfo //////////////////////////////////////////////////////////

long ALMAPI AUpsfFieldInfo
  (
  LPCSTR    fieldName,
  LONG      fieldIndex,
  TYPEID    nativeType,
  LPTYPEID  universalType,
  LPLONG    convertedSize
  )

  { // available only at Import time
  UPSF*   upsf  = GetUPSF();
  long    lRet;

  if(upsf && !upsf->MemoryError())
    lRet = upsf->FieldInfo(fieldName, fieldIndex, nativeType, universalType, convertedSize);
  else
    lRet = AUPSF_MEMORYERROR;

  if(upsf && (lRet==AUPSF_MEMORYERROR))
    upsf->SetMemoryError();

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
