#ifndef _MENULIB_H_
#define _MENULIB_H_

#include "menuobj.h"
#include "o_menus.h"

BOOL MenuHandleCommand(
  short     itemID);

AOBJECTID MenuSearchEntryList(
	AOBJECTID     objID,
  short        itemID);
  
short MenuGenerateItemID(
  void);

void MenuCheckObjectIDs(
  AOBJECTID  objID);

short DeleteItemEntries(
  LPITEMENTRY pItemEntryList,
  short       currentCnt);

HMENU MenuGetMenuHandle(
  AOBJECTID  objID);

HMENU MenuReturnMenuHandle(
  AOBJECTID  objID);

BOOL MenuReleaseMenuHandle(
  AOBJECTID  objID,
  HMENU     hMenu);

short MenuGetTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize);

void MenuAddTextItem(
  AOBJECTID    menuObject,
  AMEMBLOCKID  textBlock,
  short       index);

void MenuAddObjectItem(
  AOBJECTID    menuObject,
  AOBJECTID    itemObject,
  short       index);

void MenuDeleteItem(
  AOBJECTID  menuObject,
  short     index);

void MenuSetItemTitle(
  AOBJECTID  menuObject,
  LPSTR     pTitle,
  short     itemIndex);

BOOL MenuSetDisabledFlag(
  AOBJECTID  menuID);

long MenuGetStatus(
  AOBJECTID  menuID);

BOOL MenuSetTitle(
  AOBJECTID  menuID,
  LPSTR     pTitle);

void MenuAddForeignItem(
  AOBJECTID    ownerObject,
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index);

AOBJECTID MenuAddSubMenu(
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec,
  short       index);

BOOL MenuDeleteForeignItem(
  AOBJECTID    menuObject,
  LPITEMSPEC  pItemSpec);

BOOL MenuDeleteSubMenu(
  AOBJECTID    parentMenu,
  AOBJECTID    subMenu);

LPSTR GetCommandKeyText(
  char        buffer[10],
  char        accelKey, 
  char        accelModifier);

BOOL  MenuSendText(
  AOBJECTID    objID,
  AOBJECTID    itemID);

#endif
