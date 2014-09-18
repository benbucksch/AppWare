#ifndef _BARLIB_H_
#define _BARLIB_H_

#include "barobj.h"

short DeleteMenuEntries(
  LPMENUENTRY pMenuEntryList,
  short       currentCount);

HMENU BarCreateMenuBar(
  OBJECTID  barID);

void BarDestroyMenuBar(
  OBJECTID  objID,
  HMENU     hMenuBar);

BOOL BarHandleCommand(
  OBJECTID  objID,
  short     itemID);

HMENU BarGetBarHandle(
  OBJECTID  barID);

void BarReleaseBarHandle(
  OBJECTID  objID,
  HMENU     hMenu);

void BarRegisterAccelerators(
  OBJECTID  barObjID,
  OBJECTID  winObjID);

#endif
