#ifndef _BARLIB_H_
#define _BARLIB_H_

#include "barobj.h"

short DeleteMenuEntries(
  LPMENUENTRY pMenuEntryList,
  short       currentCount);

HMENU BarCreateMenuBar(
  AOBJECTID  barID);

void BarDestroyMenuBar(
  AOBJECTID  objID,
  HMENU     hMenuBar);

BOOL BarHandleCommand(
  AOBJECTID  objID,
  short     itemID);

HMENU BarGetBarHandle(
  AOBJECTID  barID);

void BarReleaseBarHandle(
  AOBJECTID  objID,
  HMENU     hMenu);

void BarRegisterAccelerators(
  AOBJECTID  barObjID,
  AOBJECTID  winObjID);

#endif
