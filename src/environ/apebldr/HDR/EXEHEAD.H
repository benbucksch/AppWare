//======================================================================
// exehead.h
//
// Contains the SERUIDs of the various runtime lists that are created
// at compile time and used at run time.
//
// jon  9-Jul-92 added exeProcUIDList
// jon 10-Jul-92 added exeListBlock
// jon 19-Oct-92 renamed exeProcUIDList to exeBeadList, for clarity
//======================================================================

#ifndef __EXEHEAD_H__
#define __EXEHEAD_H__

#include "classdef.h"
#include "persdata.h"
#include "seruid.h"
#include "exserrec.h"
#include "exstring.h"
#include "rtuid.h"

	//
  // Values for EXEHEAD.status
  //
  #define   EXH_HASPERSISTENTS    0x00000010

class EXEHEAD:public EXESERIUSRECORD
  {

private:

  LONG                  status;
  EXESTRING             iniFileName;
  EXESTRING             appName;
  PERSISTENTDATAHEADER  pdHeader;

  RUNTIMEUID            exeFuncList;
  RUNTIMEUID            exeObjTypeList;

  RUNTIMEUID            exeEventListBlock;   // record block to hold the list headers
  SERUID                exeEventList;

	RUNTIMEUID            exeLibListBlock;
	SERUID                exeLibList;

  RUNTIMEUID            exeBeadListBlock;
  SERUID                exeBeadList;  // pool of EXEBEAD descriptors that
                               // bind EXE<FUNC,OBJ>INSTs + responses
                               // to the EXEPROCs on each EXEPROCLIST
                               // that hangs from each EXELIB on the
                               // master exeLibList

  RUNTIMEUID            exeResponseListBlock;
  RUNTIMEUID            exeProcListBlock;

public:

  EXEHEAD(
		void);

	EXEHEAD(
		void * thisPtr)
		{
		}

	~EXEHEAD();

  RUNTIMEUID GetExeFuncList(
    void)
    {
    return exeFuncList;
    }

  RUNTIMEUID GetExeObjTypeList(
    void)
    {
    return exeObjTypeList;
    }

  LPEXEEVENTLIST   GetExeEventList();
  SERUID           GetExeEventListUID();

  LPEXELIBLIST     GetExeLibList();

  LPEXEBEADLIST    GetExeBeadList();

  LPEXERECORDBLOCK GetExeResponseListBlock(
    void);

  LPEXERECORDBLOCK GetExeProcListBlock(
    void);

  BOOL HasPersistentData()
    {
    return (0 != (status & EXH_HASPERSISTENTS));
    }

  void SetHasPersistentData()
    {
    status |= EXH_HASPERSISTENTS;
    }

  LPCSTR GetIniFileName()
    {
    return (LPCSTR)iniFileName;
    }

  BOOL InitPersistentDataHeader()
    {
    return pdHeader.Init();
    }

  LPPERSISTENTDATAHEADER GetPersistentDataHeader()
    {
    return &pdHeader;
    }

  };


#endif
