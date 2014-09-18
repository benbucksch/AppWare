#ifndef _MENULIB_H_
#define _MENULIB_H_

#include "menuobj.h"
#include "o_menus.h"

BOOL MenuHandleCommand(
  short     itemID);

OBJECTID MenuSearchEntryList(
	OBJECTID     objID,
  short        itemID);
  
short MenuGenerateItemID(
  void);

void MenuCheckObjectIDs(
  OBJECTID  objID);

short DeleteItemEntries(
  LPITEMENTRY pItemEntryList,
  short       currentCnt);

HMENU MenuGetMenuHandle(
  OBJECTID  objID);

BOOL MenuReleaseMenuHandle(
  OBJECTID  objID,
  HMENU     hMenu);

short MenuGetTitle(
  OBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize);

void MenuAddTextItem(
  OBJECTID    menuObject,
  MEMBLOCKID  textBlock,
  short       index);

void MenuAddObjectItem(
  OBJECTID    menuObject,
  OBJECTID    itemObject,
  short       index);

void MenuDeleteItem(
  OBJECTID  menuObject,
  short     index);

void MenuSetItemTitle(
  OBJECTID  menuObject,
  LPSTR     pTitle,
  short     itemIndex);

BOOL MenuSetDisabledFlag(
  OBJECTID  menuID);

long MenuGetStatus(
  OBJECTID  menuID);

BOOL MenuSetTitle(
  OBJECTID  menuID,
  LPSTR     pTitle);

void MenuAddForeignItem(
  OBJECTID    ownerObject,
  OBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index);

OBJECTID MenuAddSubMenu(
  OBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index);

BOOL MenuDeleteForeignItem(
  OBJECTID    menuObject,
  LPITEMSPEC  pItemSpec);

BOOL MenuDeleteSubMenu(
  OBJECTID    parentMenu,
  OBJECTID    subMenu);

LPSTR GetCommandKeyText(
  char        buffer[10],
  char        accelKey, 
  char        accelModifier);

BOOL  MenuSendText(
  OBJECTID    objID,
  OBJECTID    itemID);

#endif
