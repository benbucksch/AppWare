//======================================================================
//   exehead.cpp
//
// NOTE:  Resist the TEMPTATION to move the various Getxxx methods to
//        exehead.h to make them inline.  Such a move will HOSE the
//        oip build.  jon 15-Jul-92
//
// jon 19-Oct-92 What was once known as a EXEPROCUID<,LIST> is now
//               more clearly defined as a EXEBEAD<,LIST>
//======================================================================

  #include "almsys.h"

  #include "exehead.h"
  #include "sereclib.h"
	#include "rectypes.h"
	#include "exetypes.h"
	#include "exefunc.h"
  #include "exelistb.h"
  #include "exeevntl.h"
  #include "exelibl.h"
  #include "exebeadl.h"
  #include "rtreclib.h"

//---------------------------------------------------------
// EXEHEAD::EXEHEAD (constructor)
//
// create the lists, empty.
//---------------------------------------------------------
EXEHEAD::EXEHEAD(
  void)
  : iniFileName(),
    pdHeader(),
    status(0)
  {
	exeObjTypeList =
   GetUID(new EXERECORDBLOCK(sizeof(EXEOBJTYPE), RT_EXEOBJTYPE));

	exeFuncList =
   GetUID(new EXERECORDBLOCK(sizeof(EXEFUNCINST), RT_EXEFUNCINST));

  exeEventListBlock =
   GetUID(new EXERECORDBLOCK(sizeof(EXEEVENTLIST), RT_EXEEVENTLIST));
	exeEventList = GetUID(new(exeEventListBlock) EXEEVENTLIST());

  exeLibListBlock =
   GetUID(new EXERECORDBLOCK(sizeof(EXELIBLIST), RT_EXELIBLIST));
	exeLibList     = GetUID(new(exeLibListBlock) EXELIBLIST());

  exeBeadListBlock =
   GetUID(new EXERECORDBLOCK(sizeof(EXEBEADLIST), RT_EXEBEADLIST));
	exeBeadList = GetUID(new(exeBeadListBlock) EXEBEADLIST());

  exeResponseListBlock =
   GetUID(new EXERECORDBLOCK(sizeof(EXERESPONSELIST), RT_EXERESPONSELIST));

  exeProcListBlock =
   GetUID(new EXERECORDBLOCK(sizeof(EXEPROCLIST), RT_EXEPROCLIST));

	}

EXEHEAD::~EXEHEAD()
	{

	delete (LPEXERECORDBLOCK)GetObject(exeObjTypeList);
	delete (LPEXERECORDBLOCK)GetObject(exeFuncList);

  ((LPEXERECORDBLOCK)GetObject(exeBeadListBlock))->SetPurgeFlag();
  ((LPEXERECORDBLOCK)GetObject(exeLibListBlock))->SetPurgeFlag();
  ((LPEXERECORDBLOCK)GetObject(exeEventListBlock))->SetPurgeFlag();
  ((LPEXERECORDBLOCK)GetObject(exeResponseListBlock))->SetPurgeFlag();
  ((LPEXERECORDBLOCK)GetObject(exeProcListBlock))->SetPurgeFlag();

  delete (LPEXEEVENTLIST)GetObject(exeEventList);
  delete (LPEXEBEADLIST)GetObject(exeBeadList);
  delete (LPEXELIBLIST)GetObject(exeLibList);

  ((LPEXERECORDBLOCK)GetObject(exeBeadListBlock))->FreeMemory();
  ((LPEXERECORDBLOCK)GetObject(exeLibListBlock))->FreeMemory();
  ((LPEXERECORDBLOCK)GetObject(exeEventListBlock))->FreeMemory();
  ((LPEXERECORDBLOCK)GetObject(exeResponseListBlock))->FreeMemory();
  ((LPEXERECORDBLOCK)GetObject(exeProcListBlock))->FreeMemory();

  delete (LPEXERECORDBLOCK)GetObject(exeBeadListBlock);
  delete (LPEXERECORDBLOCK)GetObject(exeLibListBlock);
  delete (LPEXERECORDBLOCK)GetObject(exeEventListBlock);
  delete (LPEXERECORDBLOCK)GetObject(exeResponseListBlock);
  delete (LPEXERECORDBLOCK)GetObject(exeProcListBlock);

	}

LPEXEEVENTLIST EXEHEAD::GetExeEventList(
  void)
  {
  return (LPEXEEVENTLIST)GetObject(exeEventList);
  }

SERUID EXEHEAD::GetExeEventListUID(
  void)
  {
  return exeEventList;
  }

LPEXELIBLIST EXEHEAD::GetExeLibList(
  void)
  {
  return (LPEXELIBLIST)GetObject(exeLibList);
  }

LPEXEBEADLIST EXEHEAD::GetExeBeadList(
  void)
  {
  return (LPEXEBEADLIST)GetObject(exeBeadList);
  }

LPEXERECORDBLOCK EXEHEAD::GetExeResponseListBlock(
  void)
  {
  return (LPEXERECORDBLOCK)GetObject(exeResponseListBlock);
  }

LPEXERECORDBLOCK EXEHEAD::GetExeProcListBlock(
  void)
  {
  return (LPEXERECORDBLOCK)GetObject(exeProcListBlock);
  }
