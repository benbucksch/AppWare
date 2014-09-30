/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					ITEMLIB.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Menu item utility routines
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <o_wnd.h>

#include "o_menus.h"
#include "itemobj.h"
#include "itemlib.h"
#include "menulib.h"
#include "menudlg.h"

char* g_cmdKeyToken[] =
  {
  "",         //   0
  "A",        //   1
  "B",        //   2
  "C",        //   3
  "D",        //   4
  "E",        //   5
  "F",        //   6
  "G",        //   7
  "H",        //   8
  "I",        //   9
  "J",        //  10
  "K",        //  11
  "L",        //  12
  "M",        //  13
  "N",        //  14
  "O",        //  15
  "P",        //  16
  "Q",        //  17
  "R",        //  18
  "S",        //  19
  "T",        //  20
  "U",        //  21
  "V",        //  22
  "W",        //  23
  "X",        //  24
  "Y",        //  25
  "Z",        //  26
  NULL,       //  27
  NULL,       //  28
  NULL,       //  29
	NULL,       //  30
	NULL,       //  31
	NULL,       //  32
  NULL,       //  33
  NULL,       //  34
  NULL,       //  35
  NULL,       //  36
  NULL,       //  37
  NULL,       //  38
  NULL,       //  39
  NULL,       //  40
  NULL,       //  41
  NULL,       //  42
  NULL,       //  43
  NULL,       //  44
  "Insert",      //  45
  "Delete",      //  46
  NULL,       //  47
  NULL,       //  48
  NULL,       //  49
  NULL,       //  50
  NULL,       //  51
  NULL,       //  52
  NULL,       //  53
  NULL,       //  54
  NULL,       //  55
  NULL,       //  56
  NULL,       //  57
  NULL,       //  58
  NULL,       //  59
  NULL,       //  60
  NULL,       //  61
  NULL,       //  62
  NULL,       //  63
  NULL,       //  90
  NULL,       //  65
  NULL,       //  66
  NULL,       //  67
  NULL,       //  68
  NULL,       //  69
  NULL,       //  70
  NULL,       //  71
  NULL,       //  72
  NULL,       //  73
  NULL,       //  74
  NULL,       //  75
  NULL,       //  76
  NULL,       //  77
  NULL,       //  78
  NULL,       //  79
  NULL,       //  80
  NULL,       //  81
  NULL,       //  82
  NULL,       //  83
  NULL,       //  84
  NULL,       //  85
  NULL,       //  86
  NULL,       //  87
  NULL,       //  88
  NULL,       //  89
  NULL,       //  90
  NULL,       //  91
  NULL,       //  92
  NULL,       //  93
  NULL,       //  94
  NULL,       //  95
  "NPAD0",    //  96
  "NPAD0",    //  97
  "NPAD0",    //  98
  "NPAD0",    //  99
  "NPAD0",    // 100
  "NPAD0",    // 101
  "NPAD0",    // 102
  "NPAD0",    // 103
  "NPAD0",    // 104
  "NPAD0",    // 105
  NULL,       // 106
  NULL,       // 107
  NULL,       // 108
  NULL,       // 109
  NULL,       // 110
  NULL,       // 111
  "F1",       // 112
  "F2",       // 113
  "F3",       // 114
  "F4",       // 115
  "F5",       // 116
  "F6",       // 117
  "F7",       // 118
  "F8",       // 119
  "F9",       // 120
  "F10",      // 121
  "F11",      // 122
  "F12",      // 123
  "F13",      // 124
  "F14",      // 125
  "F15",      // 126
  "F16",      // 127
  NULL,       // 128
  NULL,       // 129
  NULL,       // 130
  NULL,       // 131
  NULL,       // 132
  NULL,       // 133
  NULL,       // 134
  NULL,       // 135
  NULL,       // 136
  NULL,       // 137
  NULL,       // 138
  NULL,       // 139
  NULL,       // 140
  NULL,       // 141
  NULL,       // 142
  NULL,       // 143
  NULL,       // 144
  "Alt+",     // 145
  "Ctrl+",    // 146
  "Shift+",   // 147
  "+"         // 148
  }; 
  
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ParseUpsf
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL ParseUpsf(
  LPSTR  string,
  char   &accelKey,
  char   &accelModifier)
  {
  BOOL	 control;
  BOOL	 shift;
  UINT	 vkey;
  char	 ckey;
  LPSTR	 p;
  LPSTR	 p1;
  UINT	 fvalue;

	// data0->keyCode structure
	// low 8 bits 0x000000FF valid ascii character - control characters
	// next 8 bits 0x0000FF00 virtual key value
	// high word bit flags for control and shift

	vkey = 0;
  ckey = 0;
  accelKey = accelModifier = 0;
	shift = control = FALSE;

	p = strtok(string, "+");
	p1 = strtok(NULL, "+");

  if (p == NULL)    ///
    return vkey;
    
	if (p1 == NULL) 	// single token
    {
		// must be F1 - F12, delete, insert

		if (p[0] == 'I')
			vkey = VK_INSERT;
    else if (p[0] == 'D')
			vkey = VK_DELETE;
    else if (p[0] == 'F')
		  {
			fvalue = atoi(string+1);
      switch(fvalue)
        {
        case 1:
          vkey = VK_F1;
          break;
        case 2:
          vkey = VK_F2;
          break;
        case 3:
          vkey = VK_F3;
          break;
        case 4:
          vkey = VK_F4;
          break;
        case 5:
          vkey = VK_F5;
          break;
        case 6:
          vkey = VK_F6;
          break;
        case 7:
          vkey = VK_F7;
          break;
        case 8:
          vkey = VK_F8;
          break;
        case 9:
          vkey = VK_F9;
          break;
        case 10:
          vkey = VK_F10;
          break;
        case 11:
          vkey = VK_F11;
          break;
        case 12:
          vkey = VK_F12;
          break;
        default:
          vkey = 0;
          break;
        }
      }
    }
	else	// dual token
	  {
		if (p[0] == 'S')
			shift = TRUE;
    else if (p[0] == 'C')
      control = TRUE;

		if (p1[0] == 'I' && p1[1] == 'n')	// Insert
			vkey = 45;
    else if (p1[0] == 'D' && p1[1] == 'e')	// Delete
			vkey = 46;
    else if (p1[0] == 'F' && isdigit(p1[1]))	// Function Key
		  {
			fvalue = atoi(string+1);
			vkey = fvalue + 111;
		  }
		else
		  {
			ckey = p1[0];
			ckey = (char)((ckey - 'A') + 1);
			vkey = ckey + 64;
      }
	  }

  if (control)
		accelModifier |= OWND_CTRL;
	if (shift)
		accelModifier |= OWND_SHIFT;

	if (ckey)
		accelKey = ckey;           	// control character
	else if (vkey)
		accelKey = (char)vkey;

	return vkey;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemGetID
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
short ItemGetID(
  AOBJECTID  objID)
  {
  LPITEMINFO  pItemInfo;
  short       itemID;

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  if (pItemInfo->itemID == 0)
    pItemInfo->itemID = MenuGenerateItemID();

  itemID = pItemInfo->itemID;

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return itemID;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ValidateName
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void ValidateName(
  AOBJECTID  objID)
  {
	char        objectName[ITEM_MAX_TEXT+1];
  LPITEMINFO  pItemInfo;

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

	if ((pItemInfo->status & ITEM_NEVER_EDITED) == ITEM_NEVER_EDITED)
		{
		//change the name to the name in the subject's object list is the objd
		//was never brought up
		AObjGetName(objID, objectName, ITEM_MAX_TEXT);
		AObjSetData(objID, ITEM_TITLE_DATA, objectName,
     (LONG)lstrlen(objectName)+1);
		//don't set the startup flag to TRUE since if we go back to design time
		//and change the objects name without bringing up the OBJD we want the
		//object to have that name the next time we compile
		}

  AObjUnlockData(objID, ITEM_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemClearToggledOnFlag
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL ItemClearToggledOnFlag(
  AOBJECTID  objID)
  {
  LPITEMINFO  pItemInfo;

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  pItemInfo->status &= (~ITEM_TOGGLED_ON);

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return TRUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemSetToggledOnFlag
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL ItemSetToggledOnFlag(
  AOBJECTID  objID)
  {
  LPITEMINFO  pItemInfo;

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  pItemInfo->status |= ITEM_TOGGLED_ON;

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return TRUE;

  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemGetStatus
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
long ItemGetStatus(
  AOBJECTID  objID)
  {
  LPITEMINFO  pItemInfo;
  long        status;

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  status = pItemInfo->status;

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return status;

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemGetToggledTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
short ItemGetToggledTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  UINT      bufferSize)
  {
  LPSTR pTitle;
  short result = 0;

  //The title data is the same for Menus and Menu items

  pTitle = (LPSTR)AObjLockData(objID, ITEM_TOGGLE_TITLE_DATA);

  if (pTitle != 0)
    {
    lstrcpyn(buffer, pTitle, bufferSize-1);
    buffer[bufferSize-1] = '\0';
    result = (short)lstrlen(buffer);
    AObjUnlockData(objID, ITEM_TOGGLE_TITLE_DATA);
    }
  else
    buffer[0] = '\0';

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemGetTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
short ItemGetTitle(
  AOBJECTID  objID,
  LPSTR     buffer,
  short     bufferSize)
  {
  LPITEMINFO  pItemInfo;
  LPSTR       pTitle;
  short       result;

  if (objID == (AOBJECTID)SEPARATORID)
    {
    buffer[0] = '\0';   // Object ID is for a separator, so don't try to get the title.
    }
  else
    {
    pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

    if (pItemInfo && ((pItemInfo->status & ITEM_NEVER_EDITED) == ITEM_NEVER_EDITED))
      {
      //if AObjGetName returns zero, assume runtime and try to get title.
  	  if (0 == AObjGetName(objID, buffer, bufferSize))
        {
        pTitle = (LPSTR)AObjLockData(objID, ITEM_TITLE_DATA);
        if (pTitle != 0)
          {
          lstrcpyn(buffer, pTitle, bufferSize-1);
          buffer[bufferSize-1] = '\0';
          AObjUnlockData(objID, ITEM_TITLE_DATA);
          }
        else
          buffer[0] = '\0';
        }
      }
    else if (pItemInfo && ((pItemInfo->status & (ITEM_TOGGLE_TYPE|ITEM_TOGGLED_ON)) ==
             (ITEM_TOGGLE_TYPE|ITEM_TOGGLED_ON)))
      {
      pTitle = (LPSTR)AObjLockData(objID, ITEM_TOGGLE_TITLE_DATA);
      if (pTitle != 0)
        {
        lstrcpyn(buffer, pTitle, bufferSize-1);
        buffer[bufferSize-1] = '\0';
        AObjUnlockData(objID, ITEM_TOGGLE_TITLE_DATA);
        }
      else
        buffer[0] = '\0';
      }
    else
      {
      pTitle = (LPSTR)AObjLockData(objID, ITEM_TITLE_DATA);
      if (pTitle != 0)
        {
        lstrcpyn(buffer, pTitle, bufferSize-1);
        buffer[bufferSize-1] = '\0';
        AObjUnlockData(objID, ITEM_TITLE_DATA);
        }
      else
        buffer[0] = '\0';
      }
    AObjUnlockData(objID, ITEM_INFO_DATA);
    }
  result = (short)lstrlen(buffer);
  return result;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemSetTitle
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
BOOL ItemSetTitle(
  AOBJECTID  itemObject,
  LPSTR     pTitle)
  {
  return  AObjSetData(itemObject, ITEM_TITLE_DATA, pTitle, lstrlen(pTitle)+1);
  }
