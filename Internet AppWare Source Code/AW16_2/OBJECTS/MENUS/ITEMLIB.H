#ifndef _ITEMLIB_H_
#define _ITEMLIB_H_

extern  char* g_cmdKeyToken[];

BOOL ParseUpsf(
  LPSTR  string,
  char   &accelKey,
  char   &accelModifier);
  
short ItemGetID(
  OBJECTID  objID);

void ValidateName(
  OBJECTID  objID);

long ItemGetStatus(
  OBJECTID  objID);

BOOL ItemSetToggledOnFlag(
  OBJECTID  objID);

BOOL ItemClearToggledOnFlag(
  OBJECTID  objID);

short ItemGetToggledTitle(
  OBJECTID  objID,
  LPSTR     buffer,
  UINT      bufferSize);

short ItemGetTitle(
  OBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize);

BOOL ItemSetTitle(
  OBJECTID  itemObject,
  LPSTR     pTitle);

#endif