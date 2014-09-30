#ifndef _ITEMLIB_H_
#define _ITEMLIB_H_

extern  char* g_cmdKeyToken[];

BOOL ParseUpsf(
  LPSTR  string,
  char   &accelKey,
  char   &accelModifier);
  
short ItemGetID(
  AOBJECTID  objID);

void ValidateName(
  AOBJECTID  objID);

long ItemGetStatus(
  AOBJECTID  objID);

BOOL ItemSetToggledOnFlag(
  AOBJECTID  objID);

BOOL ItemClearToggledOnFlag(
  AOBJECTID  objID);

short ItemGetToggledTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  UINT      bufferSize);

short ItemGetTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize);

BOOL ItemSetTitle(
  AOBJECTID  itemObject,
  LPSTR     pTitle);

#endif
